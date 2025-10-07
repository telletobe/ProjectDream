// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/UserInventorySlot.h"
#include "Components/TextBlock.h"
#include <ProjectDreamPlayerController.h>
#include "ProjectDreamCharacter.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "ProjectDreamHUD.h"
#include "Components/ScrollBox.h"
#include "UserInventory.h"

void UUserInventorySlot::NativeConstruct()
{
	SetVisibility(ESlateVisibility::Visible);

	if (OwnerUI.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Slot Binding"));
		OwnerUI->UpdateSlotIndex.AddUniqueDynamic(this,&UUserInventorySlot::UpdateSlotIndex);
	}

}

void UUserInventorySlot::NativeDestruct()
{
	if (OwnerUI.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Slot Binding remove"));
		OwnerUI->UpdateSlotIndex.RemoveAll(this);
	}
}

void UUserInventorySlot::UpdateData(const FGameItemData& InData)
{
	GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::MakeRandomColor(),TEXT("Call UpdateData"));

	if (ItemName)
	{
		ItemName->SetText(FText::FromString(InData.ItemName));
	}

	if (ItemQty)
	{
		ItemQty->SetText(FText::AsNumber(InData.ItemQty));
	}

	if (ItemCategory)
	{
		ItemCategory->SetText(FText::FromString(StaticEnum<ECategory>()->GetNameStringByValue((int64)InData.ItemCategory)));
	}

	if (ItemWeight)
	{
		ItemWeight->SetText(FText::AsNumber(InData.ItemWeight * InData.ItemQty));
	}

	SetSlotIndex(InData.ItemIndex);
	UniqueID = InData.UniqueID;
	ItemID = InData.ItemID;
}

void UUserInventorySlot::UpdateSlotIndex(int32 Index)
{
	UScrollBox* ScrollBox = Cast<UScrollBox>(GetParent());
	if (ScrollBox)
	{
		if (SlotIndex < Index) return;
		SetSlotIndex(SlotIndex - 1);
	}
}

FReply UUserInventorySlot::NativeOnPreviewMouseButtonDown(const FGeometry& InGeo, const FPointerEvent& InMouseEvent)
{
	GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::MakeRandomColor(),TEXT("call NativeOnMouseButtonDown"));
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	return Super::NativeOnPreviewMouseButtonDown(InGeo, InMouseEvent);
}

void UUserInventorySlot::NativeOnDragDetected(const FGeometry& InGeo, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOp)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::MakeRandomColor(), TEXT("call NativeOnDragDetected"));
	UDragDropOperation* Op = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());

	Op->Payload = this;
	Op->Pivot = EDragPivot::MouseDown;
	
	UUserInventorySlot* Visual =
		CreateWidget<UUserInventorySlot>(GetOwningPlayer(), GetClass());

	if (Visual)
	{
		// 현재 슬롯의 텍스트를 그대로 복사
		if (Visual->ItemName && ItemName)
			Visual->ItemName->SetText(ItemName->GetText());

		if (Visual->ItemQty && ItemQty)
			Visual->ItemQty->SetText(ItemQty->GetText());

		if (Visual->ItemCategory && ItemCategory)
			Visual->ItemCategory->SetText(ItemCategory->GetText());

		if (Visual->ItemWeight && ItemWeight)
			Visual->ItemWeight->SetText(ItemWeight->GetText());

		// 드래그 비주얼은 입력을 먹지 않도록
		Visual->SetVisibility(ESlateVisibility::HitTestInvisible);

		// (선택) 원본 슬롯과 같은 크기로 보이게 하고 싶다면:
		 const FVector2D Size = GetCachedGeometry().GetLocalSize();
		 Visual->SetDesiredSizeInViewport(Size);

		Op->DefaultDragVisual = Visual;
	}

	Op->DefaultDragVisual = Visual;

	OutOp = Op;
}

float UUserInventorySlot::GetItemWeight()
{
	if (ItemWeight)
	{
		FString WeightStr = ItemWeight->GetText().ToString();
		return FCString::Atof(*WeightStr);
	}
	return 0.0f;
}

int32 UUserInventorySlot::GetItemQty()
{
	int32 Qty = 0;
	if (ItemQty)
	{
		const FString S = ItemQty->GetText().ToString();
		LexTryParseString<int32>(Qty, *S);
	}
	return Qty;
}

void UUserInventorySlot::SetSlotIndex(int32 Index)
{
	SlotIndex = Index;
}

void UUserInventorySlot::SetupOwner(UUserInventory* InOwner)
{
	if (InOwner)
	{
		OwnerUI = InOwner;
	}

	return;
}

FGameItemData UUserInventorySlot::MakeItemData()
{
	FGameItemData OutData;

	if (ItemName)
	{
		OutData.ItemName = ItemName->GetText().ToString();
	}

	if (ItemQty)
	{
		OutData.ItemQty = FCString::Atoi(*(ItemQty->GetText().ToString()));
	}

	if (ItemCategory)
	{
		
		 FString CategoryText = ItemCategory->GetText().ToString();

		 if (CategoryText.Equals("Equipment"))
		 {
			 OutData.ItemCategory = ECategory::Equipment;
		 }
		 else if (CategoryText.Equals("Consumable"))
		 {
			 OutData.ItemCategory = ECategory::Consumable;
		 }
		 else
		 {
			 OutData.ItemCategory = ECategory::Other;
		 }		 		 
	}

	if (ItemWeight)
	{
		OutData.ItemWeight = FCString::Atof(*(ItemWeight->GetText().ToString()));
	}

	if (UniqueID.IsValid())
	{
		OutData.UniqueID = UniqueID;
	}

	OutData.ItemID = ItemID;

	return OutData;

}

FGameItemData UUserInventorySlot::MakeItemData(int32 DropNum = 1)
{
	FGameItemData OutData;

	if (ItemName)
	{
		OutData.ItemName = ItemName->GetText().ToString();
	}

	if (ItemQty)
	{
		OutData.ItemQty = DropNum;
	}

	if (ItemCategory)
	{

		FString CategoryText = ItemCategory->GetText().ToString();

		if (CategoryText.Equals("Equipment"))
		{
			OutData.ItemCategory = ECategory::Equipment;
		}
		else if (CategoryText.Equals("Consumable"))
		{
			OutData.ItemCategory = ECategory::Consumable;
		}
		else
		{
			OutData.ItemCategory = ECategory::Other;
		}
	}

	if (ItemWeight)
	{
		if (ItemQty)
		{
			float OneItemWeight = FCString::Atof(*(ItemWeight->GetText().ToString())) /  FCString::Atof(*(ItemQty->GetText().ToString()));
			OutData.ItemWeight = OneItemWeight * DropNum;
		}		
	}

	if (UniqueID.IsValid())
	{
		OutData.UniqueID = UniqueID;
	}

	OutData.ItemID = ItemID;

	return OutData;

}

