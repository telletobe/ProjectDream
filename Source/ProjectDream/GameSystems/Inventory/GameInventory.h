// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DreamItemDTO.h"
#include "GameInventory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChangeInventoryData);



UCLASS()
class PROJECTDREAM_API UGameInventory : public UObject
{
	GENERATED_BODY()
public:
	FChangeInventoryData ChangeInventoryData;
	
public :
	static UGameInventory* Get();

	bool Init(int32 InvSize = 30);
	bool AddToInventory(TPair<int32,EItemCategory> NewItmeKeyPair,UWorld* CurrentWorld);
	//void ItemDrop(int32 TargetIndex);
	//bool CreateItemDataToUIWithDrop(const FDreamGameItemDef& DropData);

	TArray<FDreamGameItemInstance> GetInventoryData() const { return InventoryData; }
	const FDreamGameItemInstance& GetInventoryDataByIdx(int32 Index) const { return InventoryData[Index]; }
	int32 Num() const { return InventoryData.Num(); }

	void AddToQty(int32 ItemIndex ,int32 ItemQty);
	void MinusToQty(int32 ItemIndex, int32 ItemQty);

	bool SaveInventoryData();
	bool SetInventoryData(TArray<FDreamGameItemInstance> LoadData);

private:
	UGameInventory();
	int32 FindEmptySlotIndex();
private:
	static TObjectPtr<UGameInventory> Instance;
	TArray<FDreamGameItemInstance> InventoryData;

};
