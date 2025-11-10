// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/Inventory/DreamGameInventorySubsystem.h"
#include "GameInventory.h"
#include "DreamGameInventorySubsystem.h"
DEFINE_LOG_CATEGORY(LOGInvSubSystem);

void UDreamGameInventorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const FSoftObjectPath DataPath = ItemDefinitions.ToSoftObjectPath();
	//처음 로딩시 LoadSynchronous, 로딩 후 데이터 존재시 Get
	if (DataPath.IsValid())
	{
		ItemsCatalog.Reset();
		TArray<FDreamGameItemDef> ItemDefs;
		if (UDreamItemDTO* ItemDTO = ItemDefinitions.LoadSynchronous())
		{
			ItemDefs = ItemDTO->ItemsDef;
			for (const FDreamGameItemDef& Data : ItemDefs)
			{
				if (Data.GetItemID() == INVALID_ITEM_ID)
				{
					UE_LOG(LogTemp, Warning, TEXT("잘못 정의된 데이터가 있습니다. 해당 데이터를 무시합니다."));
					continue;
				}
				const int32 ItemID = Data.GetItemID();
				const EItemCategory Category = Data.ItemCategory;
				TMap<EItemCategory, FDreamGameItemDef>& CategoryMap = ItemsCatalog.FindOrAdd(ItemID);
				if (CategoryMap.Contains(Category))
				{
					UE_LOG(LogTemp,Warning,TEXT("중복 정의된 데이터가 있습니다. 뒤에 정의된 데이터를 무시합니다."));
					continue;
				}
				CategoryMap.Add(Category, Data);
			}		
		}
	}

	if (!PlayerInventory)
	{
		PlayerInventory = UGameInventory::Get();
		PlayerInventory->Init();
	}
	else
	{
		PlayerInventory->Init();
	}

	// 인벤토리 데이터 로딩
	PlayerInventory->LoadInventoryData();
}

void UDreamGameInventorySubsystem::Deinitialize()
{
	Super::Deinitialize();
}

const FDreamGameItemDef* UDreamGameInventorySubsystem::GetItemDefByKey(int32 ItemID,EItemCategory Category) const
{
	if (ItemsCatalog.Num() <= 0) return nullptr;

	if (const auto* CategoryMap = ItemsCatalog.Find(ItemID))
	{
		return CategoryMap->Find(Category);
	}

	return nullptr;
}

const TMap<int32, TMap<EItemCategory, FDreamGameItemDef>>  UDreamGameInventorySubsystem::GetAllItemDefs()
{
	if (ItemsCatalog.Num() <= 0) 
	{
		return TMap<int32, TMap<EItemCategory, FDreamGameItemDef>>();
	}
	return ItemsCatalog;
}

float UDreamGameInventorySubsystem::GetInventoryWeight() const
{
	float TotalWeight = 0.f;

	if (!PlayerInventory) return 0.0f;

	TArray<FDreamGameItemInstance> InventoryData = PlayerInventory.Get()->GetInventoryData();

	for (int32 i = 0; i < InventoryData.Num(); i++)
	{
		if (InventoryData[i].GetItemCategory() == EItemCategory::None) continue;
		const FDreamGameItemDef* Def = GetItemDefByKey(InventoryData[i].GetItemID(), InventoryData[i].GetItemCategory());
		if (!Def) continue;
		TotalWeight += Def->GetItemWeight() * InventoryData[i].GetItemStackCnt();
	}

	return TotalWeight;
}
