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
	bool Init(int32 InvSize = 30);
	bool AddToInventory(TPair<int32,EItemCategory> NewItmeKeyPair,UWorld* CurrentWorld, int32 NewItemQty = 1);
	bool SetInventoryData(TArray<FDreamGameItemInstance> LoadData);
	int32 GetInventorySize() const { return InventoryData.Num(); }
	TArray<FDreamGameItemInstance> GetInventoryData() const { return InventoryData; }
	const FDreamGameItemInstance& GetInventoryDataByIdx(int32 Index) const { return InventoryData[Index]; }
	bool SaveInventoryData();
	float GetInventoryWeight() const;

private:
	UGameInventory();
	int32 FindEmptySlotIndex();
	void ItemAddedBroadCast(EItemCategory Category, int32 ItemID);
private:
	static TObjectPtr<UGameInventory> Instance;
	TArray<FDreamGameItemInstance> InventoryData;

};
