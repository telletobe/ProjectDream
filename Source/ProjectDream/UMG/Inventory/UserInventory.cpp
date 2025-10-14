// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/Inventory/UserInventory.h"
#include "GameSystems/Inventory/GameInventory.h"
#include "GameSystems/Inventory/InventoryViewWrapper.h"
#include "GameSystems/Inventory/DreamGameInventorySubsystem.h"
#include "Components/ListView.h"
#include "ProjectDreamCharacter.h"
#include "ProjectDreamPlayerController.h"
#include "Blueprint/DragDropOperation.h"
#include "Components/Border.h" 
#include "Components/HorizontalBox.h"
#include <Blueprint/WidgetLayoutLibrary.h>
#include "Components/TextBlock.h"
#include "../UMG/Inventory/QuantityPrompt.h"
#include "../UMG/Inventory/DreamInventoryEntryWidget.h"
#include "Components/Button.h"

DEFINE_LOG_CATEGORY(InventoryUIWidget);

void UUserInventory::NativeConstruct()
{
	AProjectDreamPlayerController* PC = Cast<AProjectDreamPlayerController>(GetOwningPlayer());
	if (PC)
	{
		AProjectDreamCharacter* Player = Cast<AProjectDreamCharacter>(PC->GetPawn());
		if (Player)
		{
			Player->OnInventoryEvent.AddUniqueDynamic(this, &UUserInventory::OnOffInventory);
		}
	}

	if (ButtonClose)
	{
		ButtonClose->OnClicked.AddUniqueDynamic(this, &UUserInventory::OnOffInventory);
	}
	
	if (UGameInventory* Inv = UGameInventory::Get())
	{
		Inv->ChangeInventoryData.AddUniqueDynamic(this, &UUserInventory::UpdateInventoryUI);
		UE_LOG(InventoryUIWidget,Warning,TEXT("Binding Inventory"));
	}
}

void UUserInventory::NativeOnInitialized()
{
	if (DropNumPromptClass)
	{
		DropNumPrompt = CreateWidget<UQuantityPrompt>(GetWorld(), DropNumPromptClass);		
	}

}

void UUserInventory::OnOffInventory()
{
	ESlateVisibility CurrentVisible = GetVisibility();

	switch (CurrentVisible)
	{
	case ESlateVisibility::Visible:
		SetVisibility(ESlateVisibility::Hidden);
		break;
	case ESlateVisibility::Collapsed:
		break;
	case ESlateVisibility::Hidden:
		SetVisibility(ESlateVisibility::Visible);
		break;
	case ESlateVisibility::HitTestInvisible:
		break;
	case ESlateVisibility::SelfHitTestInvisible:
		break;
	default:
		break;
	}
}

void UUserInventory::UpdateInventoryUI()
{
	if (!ItemList)
	{
		return;
	}

	TArray<FDreamGameItemInstance> UserInventory = UGameInventory::Get()->GetInventoryData();
	if (UserInventory.Num() <= 0)
	{
		ItemList->ClearListItems();
		return;
	}

	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UDreamGameInventorySubsystem* InvSubSys = GetGameInstance()->GetSubsystem<UDreamGameInventorySubsystem>();
	if (!InvSubSys) return;

	TMap<int32, TMap<EItemCategory,FDreamGameItemDef>> ItemDefs;
	ItemDefs = InvSubSys->GetAllItemDefs();
	if (ItemDefs.Num() <= 0)
	{
		return;
	}

	TArray<UObject*> ViewItems;
	ViewItems.Reserve(UserInventory.Num());

	for (const FDreamGameItemInstance& Instance : UserInventory)
	{
		if (Instance.GetItemID() == INVALID_ITEM_ID)
		{
			continue;
		}
		const FDreamGameItemDef* Def = InvSubSys->GetItemDefByKey(Instance.GetItemID(), Instance.GetItemCategory());
		if (!Def)
		{
			return;
		}

		UInventoryViewWrapper* Row = NewObject<UInventoryViewWrapper>(this);
		Row->ViewData.ItemCategory = EnumTextUtils::GetDisplayName(Instance.GetItemCategory());
		Row->ViewData.ItemName = FText::FromString(Def->ItemName);
		Row->ViewData.ItemDescription = FText::FromString(Def->ItemDescription);
		Row->ViewData.ItemWeight = Def->ItemWeight * Instance.GetItemStackCnt();
		Row->ViewData.ItemStackCnt = Instance.GetItemStackCnt();

		ViewItems.Add(Row);
	}
	UE_LOG(InventoryUIWidget, Warning, TEXT("Item View 데이터 길이 : %d"), ViewItems.Num());
	ItemList->SetListItems(ViewItems);
	ItemList->RequestRefresh();
	UE_LOG(LogTemp, Warning, TEXT("Call UpdateInventoryUI"));
}

//
//bool UUserInventory::NativeOnDrop(const FGeometry& G, const FDragDropEvent& E, UDragDropOperation* Op)
//{
//	if (!Inventory.IsValid() || !Op) return false;
//
//	UUserInventorySlot* DragSlot = Cast<UUserInventorySlot>(Op->Payload);
//
//	if (!DragSlot) return false;
//
//	int TargetIndex = DragSlot->GetSlotIndex();
//
//	if (!SlotWidgets.IsValidIndex(TargetIndex)) 
//	{
//		UE_LOG(LogTemp,Warning,TEXT("TargetIndex :%d "),TargetIndex);
//		return false;
//	}
//
//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::MakeRandomColor(), TEXT("NativeOnDrop")); 
//
//	if (CheckMousePointInUI(E))
//	{
//		return false;
//	}
//	else
//	{
//		if (SlotWidgets[TargetIndex]->GetItemQty() <= 1)
//		{
//			const FDreamGameItemDef OutData = SlotWidgets[TargetIndex]->MakeItemData();
//			if(Inventory->CreateItemDataToUIWithDrop(OutData))
//			{
//				
//				UE_LOG(LogTemp, Warning, TEXT("DropItem Create Succeed"));
//			}
//			else
//			{
//				UE_LOG(LogTemp, Warning, TEXT("DropItem Create fail"));
//			}
//			
//			Inventory->ItemDrop(TargetIndex); 
//
//		}
//		else
//		{
//			// call Item Drop Num configuration
//			if (DropNumPrompt)
//			{
//				DropNumPrompt->SetSlotData(DragSlot);
//				DropNumPrompt->SetNumberText(DragSlot->GetItemQty());
//				if (DropNumPrompt->IsInViewport())
//				{
//					DropNumPrompt->SetVisibility(ESlateVisibility::Visible);
//				}
//				else
//				{
//					DropNumPrompt->AddToViewport();
//				}
//				
//				// On OK click, drop the item in the chosen quantity.
//				// If Cancel is clicked, do not drop the item.
//			}
//			UE_LOG(InventoryUIWidget,Warning,TEXT("call Item Drop Num configuration"));
//		}
//	}
//
//	return true;
//}
//
//
//void UUserInventory::UpdateWeightText()
//{
//	if (!WeightText) return;
//
//	float TotalWeight = 0.0f;
//
//	if (!SlotWidgets.IsEmpty())
//	{
//		for (int32 i = 0; i < SlotWidgets.Num(); i++)
//		{
//			TotalWeight += SlotWidgets[i]->GetItemWeight();
//		}
//	}
//
//	FText Result = FText::Format(
//		NSLOCTEXT("UUserInventory", "WeightFormat", "{0} / 50"),
//		FText::AsNumber(TotalWeight)
//	);
//
//	WeightText->SetText(Result);
//	return;
//}
//
//// 마우스 포인트 계산을 여기서
//bool UUserInventory::CheckMousePointInUI(const FDragDropEvent& E)
//{
//	if (!ItemScroll) return false;
//
//	const FGeometry& WidgetGeometry = GetCachedGeometry();
//
//	const FVector2D ScreenPos = E.GetScreenSpacePosition(); // 모니터내의 마우스 좌표
//	const FVector2D MousePos = WidgetGeometry.AbsoluteToLocal(ScreenPos);  // 뷰포트 내의 마우스 좌표
//
//	if (InventoryBorder)
//	{
//		const FGeometry& BorderGeometry = InventoryBorder->GetCachedGeometry();	
//		const FVector2D InventoryTL = WidgetGeometry.AbsoluteToLocal(BorderGeometry.GetAbsolutePosition());
//		const FVector2D InventoryBR = WidgetGeometry.AbsoluteToLocal(BorderGeometry.LocalToAbsolute(BorderGeometry.GetLocalSize()));
//
//		const bool bInSide = (MousePos.X >= InventoryTL.X && MousePos.X < InventoryBR.X &&
//					    	  MousePos.Y >= InventoryTL.Y && MousePos.Y < InventoryBR.Y);
//	
//		if (!bInSide)
//		{
//			return false;
//		}
//	}
//	return true;
//}
