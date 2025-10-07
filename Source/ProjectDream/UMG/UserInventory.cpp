// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/UserInventory.h"
#include "GameSystems/Inventory/GameInventory.h"
#include "ProjectDreamCharacter.h"
#include "ProjectDreamPlayerController.h"
#include "Components/Button.h"
#include "UserInventorySlot.h"
#include "Components/ScrollBox.h"
#include "Blueprint/DragDropOperation.h"
#include "Components/Border.h" 
#include "Components/HorizontalBox.h"
#include <Blueprint/WidgetLayoutLibrary.h>
#include "Components/TextBlock.h"
#include "ProjectDream/GameSystems/Inventory/QuantityPrompt.h"

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
			Player->OnReadyInventory.AddUniqueDynamic(this, &UUserInventory::BindInventory);	
		}
	}

	if (ButtonClose)
	{
		ButtonClose->OnClicked.AddUniqueDynamic(this, &UUserInventory::OnOffInventory);
	}
}

void UUserInventory::NativeOnInitialized()
{
	if (DropNumPromptClass)
	{
	UE_LOG(LogTemp, Warning, TEXT("Call DropNumPrompt"));
	DropNumPrompt = CreateWidget<UQuantityPrompt>(GetWorld(), DropNumPromptClass);		
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::MakeRandomColor(), TEXT("Call DropNumPrompt NONE"));
		UE_LOG(LogTemp, Warning, TEXT("Call DropNumPrompt NONE"));
	}
}

void UUserInventory::OnOffInventory()
{
	UE_LOG(LogTemp,Warning,TEXT("Call OnOffInventory"));
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
	GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::MakeRandomColor(),TEXT("Call UpdateInventoryUI"));
	UE_LOG(LogTemp, Warning, TEXT("Call UpdateInventoryUI"));
	if (!Inventory.IsValid() || !ItemScroll || !SlotWidgetClass) return;

	const TArray<FGameItemData>& Rows = Inventory->GetInventoryData();

	for (int32 i = SlotWidgets.Num(); i < Rows.Num(); i++)
	{
		UUserInventorySlot* NewSlot = CreateWidget<UUserInventorySlot>(GetOwningPlayer(), SlotWidgetClass);
		if (!NewSlot) continue;
		NewSlot->UpdateData(Rows[i]);
		NewSlot->SetupOwner(this);
		ItemScroll->AddChild(NewSlot);		
		SlotWidgets.Add(NewSlot);
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::MakeRandomColor(), FString::Printf(TEXT("I : %d"),i));
	}
	UpdateWeightText();
}

void UUserInventory::UpdateInventoryUIWithIdx(int32 index)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::MakeRandomColor(), TEXT("Call UpdateInventoryUI(index)"));
	UE_LOG(LogTemp, Warning, TEXT("Call UpdateInventoryUI(index)"));
	if (!Inventory.IsValid() || !ItemScroll || !SlotWidgetClass) return;

	if (SlotWidgets[index])
	{
		if (Inventory.IsValid())
		{
			if (SlotWidgets.Num() != Inventory->Num())
			{
				// case Item Drop
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::MakeRandomColor(), TEXT("Call UpdateInventoryUI : Drop"));
				UpdateSlotIndex.Broadcast(index);
				SlotWidgets[index]->RemoveFromParent();				
				SlotWidgets.RemoveAt(index);
				UpdateWeightText();
				return;
			}
			else
			{
				// case Item Get or ItemDrop
				TArray<FGameItemData> InvData = Inventory->GetInventoryData();
				const FGameItemData IndexItemData = InvData[index];
				SlotWidgets[index]->UpdateData(IndexItemData);
				UpdateWeightText();
			}			
		}
	}
	else
	{
		UE_LOG(LogTemp,Error,TEXT("SlotWidget Index invalid!"));
	}
}

void UUserInventory::BindInventory(UGameInventory* PlayerInventory)
{
	if (Inventory.IsValid())
	{
		Inventory->ChangeInventoryData.RemoveAll(this);
	}

	if (PlayerInventory)
	{
		Inventory = PlayerInventory;
		if (Inventory.IsValid())
		{
			Inventory->ChangeInventoryData.AddUniqueDynamic(this, &UUserInventory::UpdateInventoryUI);
			Inventory->ChangeInventoryDataWithIndex.AddUniqueDynamic(this, &UUserInventory::UpdateInventoryUIWithIdx);
		}		
	}
}

bool UUserInventory::NativeOnDrop(const FGeometry& G, const FDragDropEvent& E, UDragDropOperation* Op)
{
	if (!Inventory.IsValid() || !Op) return false;

	UUserInventorySlot* DragSlot = Cast<UUserInventorySlot>(Op->Payload);

	if (!DragSlot) return false;

	int TargetIndex = DragSlot->GetSlotIndex();

	if (!SlotWidgets.IsValidIndex(TargetIndex)) 
	{
		UE_LOG(LogTemp,Warning,TEXT("TargetIndex :%d "),TargetIndex);
		return false;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::MakeRandomColor(), TEXT("NativeOnDrop")); 

	if (CheckMousePointInUI(E))
	{
		return false;
	}
	else
	{
		if (SlotWidgets[TargetIndex]->GetItemQty() <= 1)
		{
			const FGameItemData OutData = SlotWidgets[TargetIndex]->MakeItemData();
			UE_LOG(LogTemp,Warning,TEXT("NewData Temp: %d"), OutData.ItemID);
			if(Inventory->CreateItemDataToUIWithDrop(OutData))
			{
				
				UE_LOG(LogTemp, Warning, TEXT("DropItem Create Succeed"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("DropItem Create fail"));
			}
			
			Inventory->ItemDrop(TargetIndex); 

		}
		else
		{
			// call Item Drop Num configuration
			if (DropNumPrompt)
			{
				DropNumPrompt->SetSlotData(DragSlot);
				DropNumPrompt->SetNumberText(DragSlot->GetItemQty());
				if (DropNumPrompt->IsInViewport())
				{
					DropNumPrompt->SetVisibility(ESlateVisibility::Visible);
				}
				else
				{
					DropNumPrompt->AddToViewport();
				}
				
				// On OK click, drop the item in the chosen quantity.
				// If Cancel is clicked, do not drop the item.
			}
			UE_LOG(InventoryUIWidget,Warning,TEXT("call Item Drop Num configuration"));
		}
	}

	return true;
}


void UUserInventory::UpdateWeightText()
{
	if (!WeightText) return;

	float TotalWeight = 0.0f;

	if (!SlotWidgets.IsEmpty())
	{
		for (int32 i = 0; i < SlotWidgets.Num(); i++)
		{
			TotalWeight += SlotWidgets[i]->GetItemWeight();
		}
	}

	FText Result = FText::Format(
		NSLOCTEXT("UUserInventory", "WeightFormat", "{0} / 50"),
		FText::AsNumber(TotalWeight)
	);

	WeightText->SetText(Result);
	return;
}

// 마우스 포인트 계산을 여기서
bool UUserInventory::CheckMousePointInUI(const FDragDropEvent& E)
{
	if (!ItemScroll) return false;

	const FGeometry& WidgetGeometry = GetCachedGeometry();

	const FVector2D ScreenPos = E.GetScreenSpacePosition(); // 모니터내의 마우스 좌표
	const FVector2D MousePos = WidgetGeometry.AbsoluteToLocal(ScreenPos);  // 뷰포트 내의 마우스 좌표

	if (InventoryBorder)
	{
		const FGeometry& BorderGeometry = InventoryBorder->GetCachedGeometry();	
		const FVector2D InventoryTL = WidgetGeometry.AbsoluteToLocal(BorderGeometry.GetAbsolutePosition());
		const FVector2D InventoryBR = WidgetGeometry.AbsoluteToLocal(BorderGeometry.LocalToAbsolute(BorderGeometry.GetLocalSize()));

		const bool bInSide = (MousePos.X >= InventoryTL.X && MousePos.X < InventoryBR.X &&
					    	  MousePos.Y >= InventoryTL.Y && MousePos.Y < InventoryBR.Y);
	
		if (!bInSide)
		{
			return false;
		}
	}
	return true;
}
