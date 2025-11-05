// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/Inventory/GameInventory.h"
#include "DreamGameInventorySubsystem.h"
#include "../Save/JsonSaveGame.h"
#include "ProjectDreamCharacter.h"

// 아이템 데이터가 없으면 아이템 카테고리가 None

TObjectPtr<UGameInventory> UGameInventory::Instance = nullptr;
static const FString SaveInventory = TEXT("Inventory");

void UGameInventory::Init(int32 InvSize) 
{
	InventoryData.Init(FDreamGameItemInstance(), InvSize);
}

bool UGameInventory::SaveInventoryData()
{
	if (!SaveJson::SaveArrayToFile(SaveInventory, InventoryData))
	{
		return false;
	}
	return true;
}

bool UGameInventory::LoadInventoryData()
{
	TArray<FDreamGameItemInstance> LoadData;
	if (SaveJson::LoadArrayFromFile(SaveInventory, LoadData))
	{
		InventoryData = LoadData;
		ChangeInventoryData.Broadcast();
		return true;
	}
	return false;
}


bool UGameInventory::SetInventoryData()
{
	if (!LoadInventoryData())
	{
		return false;
	}
	ChangeInventoryData.Broadcast();
	return true;
}

UGameInventory::UGameInventory()
{
}

int32 UGameInventory::FindEmptySlotIndex()
{
	int32 FindEmptyIdx = INDEX_NONE;

	if (InventoryData.Num() <= 0)
	{
		return INDEX_NONE;
	}

	for (int32 i = 0; i < InventoryData.Num(); i++)
	{
		if (InventoryData[i].GetItemID() == INVALID_ITEM_ID)
		{
			FindEmptyIdx = i;
			return FindEmptyIdx;
		}
	}
	return INDEX_NONE;
}

void UGameInventory::ItemAddedBroadCast(EItemCategory Category, int32 ItemID)
{
	ChangeInventoryData.Broadcast();
	OnItemAdded.Broadcast(Category, ItemID);
}

UGameInventory* UGameInventory::Get()
{
	if (Instance)
	{
		return Instance;
	}
	else
	{
		Instance = NewObject<UGameInventory>();
		Instance->AddToRoot();
	}
	return Instance;
}

// 현재 아이템은 1개씩만 들어온다고 가정, Qty는 활용하지않음.
bool UGameInventory::AddToInventory(int32 NewItemID, EItemCategory NewItemCategory, UWorld* CurrentWorld, int32 NewItemQty)
{
	int32 EmptySlot = FindEmptySlotIndex();
	if (EmptySlot == INDEX_NONE) 
	{
		
		return false;
	}

	if (CurrentWorld)
	{
		if (UGameInstance* GI = CurrentWorld->GetGameInstance())
		{
			if (const UDreamGameInventorySubsystem* InvSubSys = GI->GetSubsystem<UDreamGameInventorySubsystem>())
			{
				const FDreamGameItemDef* ItemDef = InvSubSys->GetItemDefByKey(NewItemID, NewItemCategory);
				if (!ItemDef) 
				{
					return false;
				}
				FDreamGameItemInstance NewItem(NewItemID, NewItemCategory);
				UE_LOG(LogTemp, Warning, TEXT("Item ID : %d"), NewItemID);
				NewItem.MakeUniqueID();

				// 인벤토리에 같은 아이템이 있는지 검색
				if (InventoryData.Find(NewItem) != INDEX_NONE)
				{
					const int32 FindItemIdx = InventoryData.Find(NewItem);					
					// 아이템이 최대로 쌓엿을 경우 아이템 분할
					if ((InventoryData[FindItemIdx].GetItemStackCnt() + NewItem.GetItemStackCnt()) > ItemDef->GetMaxStackCnt())
					{
						const int32 RemainingStackCnt = ItemDef->GetMaxStackCnt();
						const int32 ToMoveStackCnt = InventoryData[FindItemIdx].GetItemStackCnt() - RemainingStackCnt;
						// 기존에 할당되어있는 인벤토리 공간중 빈 곳을 순회해서 찾음.
						InventoryData[EmptySlot] = NewItem;
						InventoryData[EmptySlot].SetItemStackCnt(ToMoveStackCnt);
						SaveInventoryData();
						ItemAddedBroadCast(NewItem.GetItemCategory(), NewItem.GetItemID());
						return true;
					}
					InventoryData[FindItemIdx].AddItemStack(NewItem.GetItemStackCnt());
					SaveInventoryData();
					ItemAddedBroadCast(NewItem.GetItemCategory(), NewItem.GetItemID());
					return true;
				}
				// 인벤토리에 같은 아이템이 없는경우
				else
				{
					InventoryData[EmptySlot] = NewItem;
					SaveInventoryData();
					ItemAddedBroadCast(NewItem.GetItemCategory(), NewItem.GetItemID());
					return true;
				}
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
	else
	{
		return false;
	}
}