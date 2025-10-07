// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/Inventory/GameInventory.h"
#include "GameItem.h"
#include "ProjectDreamCharacter.h"
#include "../DreamGameInstance.h"
#include "../DreamGameInstanceSubsystem.h"
#include "../Save/JsonSaveGame.h"

bool FGameItemData::operator==(const FGameItemData& Other) const
{
	return ItemCategory == Other.ItemCategory && ItemID == Other.ItemID && UniqueID == Other.UniqueID;
			
}

bool FGameItemData::operator!=(const FGameItemData& Other) const
{
	return !(*this == Other);
}

FGameItemData& FGameItemData::SetItemCategory(ECategory Category)
{
	ItemCategory = Category;
	return *this;
}

FGameItemData& FGameItemData::SetItemID(int32 ID)
{
	ItemID = ID;
	return *this;
}

FGameItemData& FGameItemData::SetItemName(FString Name)
{
	ItemName = Name;
	return *this;
}

FGameItemData& FGameItemData::SetItemDescription(FString Description)
{
	ItemDescription = Description;
	return *this;
}

FGameItemData& FGameItemData::SetCanEquipment(bool CanEquipment)
{
	bCanEquipment = CanEquipment;
	return *this;
}

FGameItemData& FGameItemData::SetUseQuickSlot(bool UseQuickSlot)
{
	bUseQuickSlot = UseQuickSlot;
	return *this;
}

FGameItemData& FGameItemData::SetItemWeight(float Weight)
{
	ItemWeight = Weight;
	return *this;
}

FGameItemData& FGameItemData::SetItemQty(int Qty)
{
	ItemQty = Qty;
	return *this;
}

bool FGameItemData::MakeUniqueID()
{
	if (ItemCategory == ECategory::Equipment)
	{
		UniqueID = FGuid::NewGuid();
		return true;
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("not used UniqueID"));
	}
	return false;
}

void FGameItemData::PrintUID() const
{
	if (UniqueID.IsValid())
	{
		FString GUidStr = UniqueID.ToString();
		UE_LOG(LogTemp,Warning,TEXT("Item UID : %s"), *GUidStr);
	}
}

bool UGameInventory::SaveInventoryData()
{
	if (!SaveJson::SaveArrayToFile(TEXT("Inventory"), InventoryData))
	{
		return false;
	}
	return true;
}

bool UGameInventory::SetInventoryData(TArray<FGameItemData> LoadData)
{
	if (LoadData.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Array Is Empty"));
		return false;
	}

	InventoryData = LoadData;

	ChangeInventoryData.Broadcast();
	return true;
}

UGameInventory::UGameInventory()
{
}

void UGameInventory::InitInventory(int32 Size)
{
	Size = FMath::Max(0, Size);
	if (Size >= 50)
	{
		Size = 50;
	}

	InventoryData.SetNum(Size);
}

void UGameInventory::ItemDrop(int32 TargetIndex)
{
	if (TargetIndex <= INDEX_NONE) return;
	if (!InventoryData.IsValidIndex(TargetIndex)) return;

	InventoryData.RemoveAt(TargetIndex);
	ChangeInventoryDataWithIndex.Broadcast(TargetIndex);

	if (SaveInventoryData())
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance valid"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance Invalid"));
	}
}

void UGameInventory::AddToQty(int32 ItemIndex, int32 ItemQty)
{
	if (!InventoryData.IsValidIndex(ItemIndex)) return;
	InventoryData[ItemIndex].ItemQty += ItemQty;
	ChangeInventoryDataWithIndex.Broadcast(ItemIndex);
}

void UGameInventory::MinusToQty(int32 ItemIndex, int32 ItemQty)
{
	if (!InventoryData.IsValidIndex(ItemIndex)) return;
	InventoryData[ItemIndex].ItemQty -= ItemQty;

	if (InventoryData[ItemIndex].ItemQty == 0)
	{
		ItemDrop(ItemIndex);
	}
	else
	{
		ChangeInventoryDataWithIndex.Broadcast(ItemIndex);
	}
}

void UGameInventory::SetOwner(AProjectDreamCharacter* P)
{
	if (P)
	{
		Player = P;
	}
}

bool UGameInventory::CreateItemDataToUIWithDrop(const FGameItemData& DropData)
{
	if (!Player.IsValid())  return false;

	UWorld* World = Player->GetWorld();
	if (!World) return false;

	FVector SpawnLocation = Player->GetActorLocation() + Player->GetActorForwardVector() * 50.0f;
	FRotator SpawnRotator = Player->GetActorRotation();

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AGameItem* DropItem = World->SpawnActor<AGameItem>(
		AGameItem::StaticClass(),
		SpawnLocation,
		SpawnRotator,
		Params);

	if (DropItem)
	{
		DropItem->SetItemData(DropData);
	}

	return true;
}

bool UGameInventory::AddToInventory(const FGameItemData& ItemData)
{
	if (!ItemData.IsValid())
	{
		UE_LOG(LogTemp,Warning,TEXT("Invalid ItemData"));
		return false;
	}
	ItemData.PrintUID();
	FGameItemData Data;

	// 같은 아이템이 있다면 아이템 데이터의 갯수만 증가후 배열에 추가하지않음.
	if (InventoryData.Contains(ItemData))
	{
		int32 Index = InventoryData.Find(ItemData);
		AddToQty(Index, ItemData.ItemQty);
		UE_LOG(LogTemp,Warning,TEXT("Contains Item"));

		if (SaveInventoryData())
		{		
			UE_LOG(LogTemp, Warning, TEXT("Item Save Succeded"));
		} else
		{
			UE_LOG(LogTemp,Warning,TEXT("Item Save  Failed"));
		}
		return true;
	}

	Data.SetItemName(ItemData.ItemName)
		.SetItemCategory(ItemData.ItemCategory)
		.SetItemID(ItemData.ItemID)
		.SetItemDescription(TEXT("Item"))
		.SetUseQuickSlot(false)
		.SetItemWeight(ItemData.ItemWeight)
		.SetItemQty(ItemData.ItemQty);

	int32 NewIndex = InventoryData.Add(Data);
	if (InventoryData[NewIndex].IsValid())
	{
		InventoryData[NewIndex].ItemIndex = NewIndex;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::MakeRandomColor(), TEXT("Get Item!"));
	UE_LOG(LogTemp,Warning,TEXT("Get Item!"));

	ChangeInventoryData.Broadcast(); // call UserInventory.cpp UpdateInventoryUI()

	if (SaveInventoryData())
	{
		UE_LOG(LogTemp, Warning, TEXT("Item Save Succeded"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Item Save  Failed"));
	}

	return true;
}