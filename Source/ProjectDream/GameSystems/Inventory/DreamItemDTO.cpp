// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/Inventory/DreamItemDTO.h"

bool FDreamGameItemDef::operator==(const FDreamGameItemDef& Other) const
{
	return ItemCategory == Other.ItemCategory && ItemID == Other.ItemID;
}

bool FDreamGameItemDef::operator!=(const FDreamGameItemDef& Other) const
{
	return !(*this == Other);
}

bool FDreamGameItemInstance::MakeUniqueID()
{
	if (InstancePair.Value == EItemCategory::Equipment)
	{
		UniqueID = FGuid::NewGuid();
		return true;
	}

	return false;
}

FDreamGameItemInstance::FDreamGameItemInstance(int32 ItemID, EItemCategory ItemCategory, int32 NewItemStackCnt)
{
	InstancePair.Key = ItemID;
	InstancePair.Value = ItemCategory;
	ItemStackCnt += NewItemStackCnt;
}

bool FDreamGameItemInstance::AddItemStack(int32 NewITemStackCnt)
{
	if (NewITemStackCnt <= 0) return false;
	ItemStackCnt += NewITemStackCnt;
	return true;
}

bool FDreamGameItemInstance::SetItemStackCnt(int32 CurrentItemStack)
{
	if (CurrentItemStack <= 0) return false;
	ItemStackCnt = CurrentItemStack;
	return true;
}

bool FDreamGameItemInstance::operator==(const FDreamGameItemInstance& rhs) const
{
	if (InstancePair.Key == rhs.GetItemID())
	{
		if (InstancePair.Value == rhs.GetItemCategory())
		{
			return GetUniqueID().Equals(rhs.GetUniqueID());
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

}
