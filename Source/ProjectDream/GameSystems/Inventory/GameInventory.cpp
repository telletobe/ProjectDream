// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/Inventory/GameInventory.h"
#include "DreamGameInventorySubsystem.h"
#include "../Save/JsonSaveGame.h"
#include "ProjectDreamCharacter.h"

TObjectPtr<UGameInventory> UGameInventory::Instance = nullptr;

bool UGameInventory::Init(int32 InvSize) 
{
	InventoryData.Init(FDreamGameItemInstance(), InvSize);

	if (InventoryData.Num() <= 0)
	{
		return false;
	}
	else
	{
		return true;
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

bool UGameInventory::SetInventoryData(TArray<FDreamGameItemInstance> LoadData)
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

int32 UGameInventory::FindEmptySlotIndex()
{
	int32 FindEmptyIdx = INDEX_NONE;

	if (InventoryData.Num() <= 0)
	{
		UE_LOG(LogTemp,Warning,TEXT("인벤토리 초기화돼지 않음"));
		return INDEX_NONE;
	}

	for (int32 i = 0; i < InventoryData.Num(); i++)
	{
		if (InventoryData[i].GetItemID() == INVALID_ITEM_ID)
		{
			FindEmptyIdx = i;
			UE_LOG(LogTemp, Warning, TEXT("빈칸 찾음"));
			return FindEmptyIdx;
		}
	}
	
	return INDEX_NONE;
}

//void UGameInventory::ItemDrop(int32 TargetIndex)
//{
//	if (TargetIndex <= INDEX_NONE) return;
//	//if (!InventoryData.IsValidIndex(TargetIndex)) return;
//
//	//InventoryData.RemoveAt(TargetIndex);
//	////ChangeInventoryDataWithIndex.Broadcast(TargetIndex);
//
//	//if (SaveInventoryData())
//	//{
//	//	UE_LOG(LogTemp, Warning, TEXT("GameInstance valid"));
//	//}
//	//else
//	//{
//	//	UE_LOG(LogTemp, Warning, TEXT("GameInstance Invalid"));
//	//}
//}

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

//bool UGameInventory::CreateItemDataToUIWithDrop(const FDreamGameItemDef& DropData)
//{
//	//if (!Player.IsValid())  return false;
//
//	//UWorld* World = Player->GetWorld();
//	//if (!World) return false;
//
//	//FVector SpawnLocation = Player->GetActorLocation() + Player->GetActorForwardVector() * 50.0f;
//	//FRotator SpawnRotator = Player->GetActorRotation();
//
//	//FActorSpawnParameters Params;
//	//Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//
//	//AGameItem* DropItem = World->SpawnActor<AGameItem>(
//	//	AGameItem::StaticClass(),
//	//	SpawnLocation,
//	//	SpawnRotator,
//	//	Params);
//
//	//if (DropItem)
//	//{
//	//	DropItem->SetItemData(DropData);
//	//}
//
//	return true;
//}

bool UGameInventory::AddToInventory(TPair<int32, EItemCategory> NewItmeKeyPair, UWorld* CurrentWorld)
{
	if (CurrentWorld)
	{
		if (UGameInstance* GI = CurrentWorld->GetGameInstance())
		{
			if (const UDreamGameInventorySubsystem* InvSubSys = GI->GetSubsystem<UDreamGameInventorySubsystem>())
			{
				const FDreamGameItemDef* ItemDef = InvSubSys->GetItemDefByKey(NewItmeKeyPair.Key, NewItmeKeyPair.Value);
				if (!ItemDef) 
				{
					UE_LOG(LogTemp, Warning, TEXT("아이템의 정의가 없음"));
					return false;
				}
				FDreamGameItemInstance NewItem(NewItmeKeyPair.Key, NewItmeKeyPair.Value);
				NewItem.MakeUniqueID();
				UE_LOG(LogTemp,Warning,TEXT("획득한 아이템 아이디 : %d"),ItemDef->GetItemID());

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
						int32 EmptySlot = FindEmptySlotIndex();
						InventoryData[EmptySlot] = NewItem;
						InventoryData[EmptySlot].SetItemStackCnt(ToMoveStackCnt);
						ChangeInventoryData.Broadcast();
						return true;
					}
					InventoryData[FindItemIdx].AddItemStack(NewItem.GetItemStackCnt());
					ChangeInventoryData.Broadcast();
					return true;
				}
				// 인벤토리에 같은 아이템이 없는경우
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("새로운 아이템 획득"));
					const int32 EmptySlot = FindEmptySlotIndex();

					if (EmptySlot > InventoryData.Num() || EmptySlot == INDEX_NONE)
					{
						UE_LOG(LogTemp, Warning, TEXT("인벤토리 공간없음"));
						return false;
					}

					InventoryData[EmptySlot] = NewItem;
					ChangeInventoryData.Broadcast();
					return true;
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("서브시스템 정보 누락"));
				return false;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("인스턴스 정보 누락"));
			return false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("월드 정보 누락"));
		return false;
	}
}