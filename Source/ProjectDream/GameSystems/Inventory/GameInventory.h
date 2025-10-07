// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "GameInventory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChangeInventoryData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangeInventoryDataWithIndex, int32, Index);

class AGameItem;


UENUM()
enum class ECategory : int8
{
	Equipment	UMETA(DisPlayName = "Equipment"), // need UID
	Consumable	UMETA(DisplayName = "Consumable"),
	Other		UMETA(DisplayName = "Other")
};

USTRUCT(BlueprintType)
struct FGameItemData
{
	GENERATED_BODY()

public:

	FGameItemData() : ItemCategory(ECategory::Other), ItemID(-1), ItemName(TEXT("")), ItemDescription(TEXT("")), bCanEquipment(false), bUseQuickSlot(false), ItemQty(0), ItemWeight(0.0)
	{
	}	
	bool operator ==(const FGameItemData& Other) const;
	bool operator !=(const FGameItemData& Other) const;

	FGameItemData& SetItemCategory(ECategory Category);
	FGameItemData& SetItemID(int32 ID);
	FGameItemData& SetItemName(FString Name);
	FGameItemData& SetItemDescription(FString Description);
	FGameItemData& SetCanEquipment(bool CanEquipment);
	FGameItemData& SetUseQuickSlot(bool UseQuickSlot);
	FGameItemData& SetItemWeight(float Weight);
	FGameItemData& SetItemQty(int Qty);
	bool MakeUniqueID();

	void PrintUID() const;

	bool IsValid() const { return (ItemID >= 1 && ItemQty > 0);}

	UPROPERTY(EditAnywhere, Category = "Data",SaveGame)
	ECategory ItemCategory;
	UPROPERTY(EditAnywhere, Category = "Data", SaveGame)
	int32 ItemID;
	UPROPERTY(EditAnywhere, Category = "Data", SaveGame)
	FString ItemName;
	UPROPERTY(EditAnywhere, Category = "Data", SaveGame)
	FString ItemDescription;
	UPROPERTY(EditAnywhere, Category = "Data", SaveGame)
	bool bCanEquipment;
	UPROPERTY(EditAnywhere, Category = "Data", SaveGame)
	bool bUseQuickSlot;
	UPROPERTY(VisibleAnywhere, Category = "Data", SaveGame)
	FGuid UniqueID;

	UPROPERTY(EditAnywhere, Category = "Data", SaveGame)
	int32 ItemQty;
	UPROPERTY(EditAnywhere, Category = "Data", SaveGame)
	float ItemWeight;
	UPROPERTY(VisibleAnywhere, Category = "Data", SaveGame)
	int32 ItemIndex;

private:

	
};

UCLASS()
class PROJECTDREAM_API UGameInventory : public UObject
{
	GENERATED_BODY()
	
public :
	UGameInventory();
	bool AddToInventory(const FGameItemData& ItemData);
	void InitInventory(int32 Size);
	void ItemDrop(int32 TargetIndex);
	bool CreateItemDataToUIWithDrop(const FGameItemData& DropData);

	const TArray<FGameItemData>& GetInventoryData() const { return InventoryData; }
	const FGameItemData& GetInventoryData(int32 Index) const { return InventoryData[Index]; }
	int32 Num() const { return InventoryData.Num(); }

	FChangeInventoryData ChangeInventoryData;
	FChangeInventoryDataWithIndex ChangeInventoryDataWithIndex;

	void AddToQty(int32 ItemIndex ,int32 ItemQty);
	void MinusToQty(int32 ItemIndex, int32 ItemQty);
	void SetOwner(class AProjectDreamCharacter* P);

	TArray<FGameItemData> GetInventoryData() { return InventoryData; }
	bool SaveInventoryData();
	bool SetInventoryData(TArray<FGameItemData> LoadData);
private:

	TArray<FGameItemData> InventoryData;
	TWeakObjectPtr<class AProjectDreamCharacter> Player;
	
};
