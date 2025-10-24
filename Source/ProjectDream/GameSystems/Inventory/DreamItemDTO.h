// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DreamItemDTO.generated.h"

static constexpr int32 INVALID_ITEM_ID = -1;

UENUM()
enum class EItemCategory : int8
{
	Equipment	UMETA(DisPlayName = "Equipment"), // need UID
	Consumable	UMETA(DisplayName = "Consumable"),
	Other		UMETA(DisplayName = "Other"),
	None		UMETA(DisplayName = "None")
};

// 정의 데이터
USTRUCT(BlueprintType)
struct FDreamGameItemDef
{
	GENERATED_BODY()

public:
	FDreamGameItemDef() : ItemCategory(EItemCategory::Other), ItemName(TEXT("NAME_None")), ItemDescription(TEXT("DESC_NONE")), ItemWeight(0.0), MaxStackCnt(1), ItemID(INVALID_ITEM_ID) {}
	bool operator ==(const FDreamGameItemDef& Other) const;
	bool operator !=(const FDreamGameItemDef& Other) const;
	int32 GetItemID() const { return ItemID; }
	int32 GetMaxStackCnt() const { return MaxStackCnt; }
	float GetItemWeight() const { return ItemWeight; }
public:
	UPROPERTY(EditAnywhere, Category = "Data")		 EItemCategory ItemCategory;
	UPROPERTY(EditAnywhere, Category = "Data")		 FString ItemName;
	UPROPERTY(EditAnywhere, Category = "Data")		 FString ItemDescription;
	UPROPERTY(EditAnywhere, Category = "Data")		 float ItemWeight;
private:
	UPROPERTY(EditAnywhere, Category = "Data")		 int32 MaxStackCnt;
	UPROPERTY(EditAnywhere, Category = "Data")		 int32 ItemID;

};

// 인게임 데이터
USTRUCT(BlueprintType)
struct FDreamGameItemInstance
{
	GENERATED_BODY()
public:
	FDreamGameItemInstance() : InstancePair(INVALID_ITEM_ID,EItemCategory::Other), ItemStackCnt(0) {};
	FDreamGameItemInstance(int32 ItemID, EItemCategory ItemCategory,int32 NewItemStackCnt = 1);
	int32 GetItemID() const { return InstancePair.Key; }
	bool AddItemStack(int32 NewITemStackCnt = 1);
	int32 GetItemStackCnt() const { return ItemStackCnt; }
	EItemCategory GetItemCategory() const { return InstancePair.Value; }
	bool SetItemStackCnt(int32 CurrentItemStack);
	bool operator== (const FDreamGameItemInstance& rhs) const;
	bool MakeUniqueID();
	FString GetUniqueID() const { return UniqueID.ToString(); }
private:
	TPair<int32, EItemCategory> InstancePair;
	int32 ItemStackCnt = 0;
	FGuid UniqueID;
};

USTRUCT()
struct FDreamGameItemView
{
	GENERATED_BODY()
public:
	FText ItemCategory;
	FText ItemName;
	FText ItemDescription;
	float ItemWeight;
	int32 ItemStackCnt;
};

UCLASS()
class PROJECTDREAM_API UDreamItemDTO : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Definition")
	TArray<FDreamGameItemDef> ItemsDef;
};
