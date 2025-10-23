// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/Inventory/DreamInventoryEntryWidget.h"
#include "../GameSystems/Inventory/DreamGameInventorySubsystem.h"
#include "../GameSystems/Inventory/InventoryViewWrapper.h"
#include "Components/TextBlock.h"


void UDreamInventoryEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (!ListItemObject)  return;

	if (const UInventoryViewWrapper* Row = Cast< UInventoryViewWrapper>(ListItemObject))
	{
		ItemView = Row->ViewData;
	}
	SyncFromItem();
}

void UDreamInventoryEntryWidget::SyncFromItem()
{
	if (ItemName)
	{
		ItemName->SetText(ItemView.ItemName);
	}
	if (ItemNum)
	{
		ItemNum->SetText(FText::AsNumber(ItemView.ItemStackCnt));
	}
	if (ItemCategory)
	{
		ItemCategory->SetText(ItemView.ItemCategory);
	}
	if (ItemDesc)
	{
		ItemDesc->SetText(ItemView.ItemDescription);
	}
	if (ItemWeight)
	{
		FNumberFormattingOptions NumberFormat;
		NumberFormat.MinimumFractionalDigits = 2; // 최소 소수점 2자리
		NumberFormat.MaximumFractionalDigits = 2; // 최대 소수점 2자리
		ItemWeight->SetText(FText::AsNumber(ItemView.ItemWeight,&NumberFormat));
	}
}