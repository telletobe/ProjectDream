// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/Inventory/QuantityPrompt.h"
#include "Components/Button.h"
#include "Components/SpinBox.h"
#include "../UMG/UserInventorySlot.h"
#include "ProjectDreamCharacter.h"
#include "GameSystems/Inventory/GameInventory.h"

void UQuantityPrompt::NativeConstruct()
{
	if (ButtonOK)
	{
		ButtonOK->OnClicked.AddUniqueDynamic(this, &UQuantityPrompt::OnClickOK);
		UE_LOG(LogTemp, Warning, TEXT("ButtonOK Bind"));
	}

	if (ButtonCancle)
	{
		ButtonCancle->OnClicked.AddUniqueDynamic(this, &UQuantityPrompt::OnClickCancle);
		UE_LOG(LogTemp,Warning,TEXT("ButtonCancle Bind"));
	}

}


void UQuantityPrompt::SetSlotData(UUserInventorySlot* UserSlot)
{
	if (!UserSlot) return;

	SlotData = UserSlot;
}

void UQuantityPrompt::SetNumberText(int32 Number)
{
	if (NumberTextBox)
	{
		NumberTextBox->SetValue(Number);
	}
}

void UQuantityPrompt::OnClickOK()
{
	if (!NumberTextBox || !SlotData.IsValid()) return;
	
	if (SlotData->GetItemQty() < NumberTextBox->GetValue())
	{
		//갖고있는 아이템보다 많이 버리려함
		//Warning Widget Open
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	else if(NumberTextBox->GetValue() <= 0)
	{
		//아이템을 버리지않거나 - 입력
		//Warning Widget Open
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	else
	{
		if (AProjectDreamCharacter* Player = Cast<AProjectDreamCharacter>(GetOwningPlayerPawn()))
		{
			if (UGameInventory* Inv = Cast<UGameInventory>(Player->GetItemInventory()))
			{
				if (SlotData.IsValid())
				{
					if (Inv->CreateItemDataToUIWithDrop(SlotData->MakeItemData(NumberTextBox->GetValue())))
					{
						UE_LOG(LogTemp, Warning, TEXT("DropItem Create Succeed"));
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("DropItem Create fail"));
					}
				}
				else
				{
					UE_LOG(LogTemp,Warning,TEXT("SlotData Invalid"));
				}
				Inv->MinusToQty(SlotData->GetSlotIndex(),NumberTextBox->GetValue());
			}
		}
	}
	SetVisibility(ESlateVisibility::Collapsed);
}

void UQuantityPrompt::OnClickCancle()
{
	SetVisibility(ESlateVisibility::Collapsed);
	if (NumberTextBox)
	{
		NumberTextBox->SetValue(1);
	}
}
