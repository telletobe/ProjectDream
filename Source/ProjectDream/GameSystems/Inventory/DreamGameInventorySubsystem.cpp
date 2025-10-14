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
				UE_LOG(LogTemp, Warning, TEXT("데이터 아이디 : %d"), Data.GetItemID());
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
					UE_LOG(LogTemp,Warning,TEXT("중복 정의된 데이터가 있습니다. 해당 데이터를 무시합니다."));
					continue;
				}
				CategoryMap.Add(Category, Data);
			}		
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("아이템 카탈로그 길이 : %d"),ItemsCatalog.Num());

	if (!PlayerInventory)
	{
		PlayerInventory = UGameInventory::Get();
		if (!PlayerInventory)
		{
			UE_LOG(LogTemp,Warning,TEXT("인벤토리 초기화 실패"));
			return;
		}

		if (PlayerInventory->Init())
		{
			UE_LOG(LogTemp,Warning,TEXT("인벤토리 초기화 완료"));
		}
	}
	else
	{
		PlayerInventory->Init();
	}

	// 인벤토리 데이터 로딩
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
		UE_LOG(LogTemp, Warning, TEXT("아이템 정의 데이터 없음"));
		return TMap<int32, TMap<EItemCategory, FDreamGameItemDef>>();
	}
	return ItemsCatalog;
}

