// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DreamItemDTO.h"
#include "GameInventory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChangeInventoryData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemAdded, EItemCategory, ItemCategory, int32, ItemID);


UCLASS()
class PROJECTDREAM_API UGameInventory : public UObject
{
	GENERATED_BODY()
public:
	FChangeInventoryData ChangeInventoryData;
	FOnInventoryItemAdded OnItemAdded;
public :
	static UGameInventory* Get();
	void Init(int32 InvSize = 30);
	bool AddToInventory(int32 NewItemID, EItemCategory NewItemCategory,UWorld* CurrentWorld, int32 NewItemQty = 1);
	int32 GetInventorySize() const { return InventoryData.Num(); }
	TArray<FDreamGameItemInstance> GetInventoryData() const { return InventoryData; }
	const FDreamGameItemInstance& GetInventoryDataByIdx(int32 Index) const { return InventoryData[Index]; }
	float GetInventoryWeight() const;
	bool LoadInventoryData();
	bool SetInventoryData();
private:
	bool SaveInventoryData();
private:
	UGameInventory();
	int32 FindEmptySlotIndex();
	void ItemAddedBroadCast(EItemCategory Category, int32 ItemID);
private:
	static TObjectPtr<UGameInventory> Instance;
	UPROPERTY(SaveGame) TArray<FDreamGameItemInstance> InventoryData;

};
