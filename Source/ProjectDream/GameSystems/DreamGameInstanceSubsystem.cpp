// Fill out your copyright notice in the Description page of Project Settings.


#include "DreamGameInstanceSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "save/DreamSaveGame.h"
#include "ProjectDreamCharacter.h"
#include "ProjectDreamPlayerController.h"
#include "Inventory/GameInventory.h"
#include "Save/JsonSaveGame.h"


bool UDreamGameInstanceSubsystem::SaveInventoryJson(const FString& SlotName, UGameInventory* Inv)
{
	if (!Inv)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			AProjectDreamPlayerController* PC = Cast<AProjectDreamPlayerController>(World->GetFirstPlayerController());
			if (PC)
			{
				AProjectDreamCharacter* Player = Cast<AProjectDreamCharacter>(PC->GetCharacter());
				if (Player)
				{
					Inv = Player->GetItemInventory();
				}
				else return false;
			}
			else return false;
		}
		else return false;
	}

	FInventorySave Data;
	Data.SaveData = Inv->GetInventoryData();

	bool bPretty = true;
	bool bOk = SaveJson::SaveToFile(SlotName, Data, bPretty);

	return bOk;
}

bool UDreamGameInstanceSubsystem::LoadInventoryJson(const FString& SlotName)
{
	FInventorySave LoadData;
	if (!SaveJson::LoadFromFile(SlotName, LoadData))
	{
		UE_LOG(LogTemp,Warning,TEXT("Load Failed"));
		return false;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		AProjectDreamPlayerController* PC = Cast<AProjectDreamPlayerController>(World->GetFirstPlayerController());
		if (PC)
		{
			AProjectDreamCharacter* Player = Cast<AProjectDreamCharacter>(PC->GetCharacter());
			if (Player)
			{
				UGameInventory* Inv = Player->GetItemInventory();
				if (Inv)
				{
					Inv->SetInventoryData(LoadData.SaveData);
					
					TArray<FGameItemData> PlayerInv = Inv->GetInventoryData();
					for (int32 i = 0; i < PlayerInv.Num(); i++)
					{
						PlayerInv[i].ItemIndex = i;
					}

					UE_LOG(LogTemp,Warning,TEXT("Load Succeded"));
					return true;
				}

			}
			else return false;
		}
		else return false;
	}
	else return false;



	return false;
}

void UDreamGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	//FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddUObject(this,&UDreamGameInstanceSubsystem::HandleAppWillEnterBackground);
	//FCoreDelegates::OnPreExit.AddUObject(this,&UDreamGameInstanceSubsystem::HandlePreExit);

//#if WITH_EDITOR
//	FEditorDelegates::PrePIEEnded.AddUObject(this, &UDreamGameInstanceSubsystem::HandlePrePIEEnded);
//#endif

}

void UDreamGameInstanceSubsystem::Deinitialize()
{
//#if WITH_EDITOR
//	FEditorDelegates::PrePIEEnded.RemoveAll(this);
//#endif
//	FCoreDelegates::ApplicationWillEnterBackgroundDelegate.RemoveAll(this);
//	FCoreDelegates::OnPreExit.RemoveAll(this);
}


//
//void UDreamGameInstanceSubsystem::SaveInternal(const FString& SlotName)
//{
//	UWorld* World = GetWorld();
//	if (!World || World->bIsTearingDown) return;
//
//	UDreamSaveGame* SaveObj = Cast<UDreamSaveGame>(UGameplayStatics::CreateSaveGameObject(UDreamSaveGame::StaticClass()));
//
//	if (!SaveObj) return;
//
//	FInventorySave* InvSave = &SaveObj->GetInventorySaveData();
//
//	if (!InvSave)
//	{
//		InvSave->SaveData.Reset();
//	}
//
//	// 인벤토리 정보 가져와서 저장
//	AProjectDreamPlayerController* PC = Cast<AProjectDreamPlayerController>(UGameplayStatics::GetPlayerController(World, 0));
//	if (PC)
//	{
//		AProjectDreamCharacter* Player = Cast<AProjectDreamCharacter>(PC->GetCharacter());
//		if (Player)
//		{
//			UGameInventory* Inv = Player->GetItemInventory();
//			if (!Inv->GetInventoryData().IsEmpty())
//			{
//				SaveObj->GetInventorySaveData().SaveData = Inv->GetInventoryData();
//				UE_LOG(LogTemp, Warning, TEXT("Save Internal"));
//			}
//			else
//			{
//				UE_LOG(LogTemp, Warning, TEXT("Save : InvData Null"));
//			}
//		}
//		
//	}
//	UGameplayStatics::SaveGameToSlot(SaveObj, SlotName, 0);
//	UE_LOG(LogTemp, Warning, TEXT("Save Succeded"));
//}
//
//void UDreamGameInstanceSubsystem::Save(const FString& SlotName)
//{
//	SaveInternal(TEXT("TestSave"));
//	UE_LOG(LogTemp, Warning, TEXT("Save Call"));
//}
//
//void UDreamGameInstanceSubsystem::Load(const FString& SlotName)
//{
//	LoadInternal(TEXT("TestSave"));
//	UE_LOG(LogTemp, Warning, TEXT("Load Call"));
//}


//void UDreamGameInstanceSubsystem::LoadInternal(const FString& SlotName)
//{
//	UWorld* World = GetWorld();
//	if (!World) return;
//	
//
//	if (USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(SlotName, 0))
//	{
//		if (UDreamSaveGame* SaveObj = Cast<UDreamSaveGame>(Loaded))
//		{
//			FInventorySave* InvSave = &SaveObj->GetInventorySaveData();
//
//			AProjectDreamPlayerController* PC = Cast<AProjectDreamPlayerController>(UGameplayStatics::GetPlayerController(World,0));
//
//			if (!PC) return;
//
//			AProjectDreamCharacter* Player = Cast<AProjectDreamCharacter>(PC->GetPawn());
//
//			if (!Player)
//			{
//				if (PendingLoadRetries++ < 30)
//				{
//					Load(SlotName);
//				}
//				else
//				{
//					UE_LOG(LogTemp,Warning,TEXT("Player not Ready"));
//					PendingLoadRetries = 0;
//				}
//				return;
//			}
//			else
//			{
//				UGameInventory* Inv = Player->GetItemInventory();
//				
//				if (!Inv) return;
//				Inv->SetInventoryData(SaveObj->GetInventorySaveData().SaveData);
//
//				if (!Inv->GetInventoryData().IsEmpty())
//				{
//					SaveObj->GetInventorySaveData().SaveData = Inv->GetInventoryData();
//					UGameplayStatics::SaveGameToSlot(SaveObj, SlotName, 0);
//					UE_LOG(LogTemp, Warning, TEXT("Load Internal"));
//				}
//				else
//				{
//					UE_LOG(LogTemp, Warning, TEXT("LOAD :InvData Null"));
//				}
//			}
//		}
//	}
//	UE_LOG(LogTemp, Warning, TEXT("Load Succeded"));
//}


//void UDreamGameInstanceSubsystem::HandleAppWillEnterBackground()
//{
//	SaveInternal(TEXT("TestSave"));
//}
//
//void UDreamGameInstanceSubsystem::HandlePreExit()
//{
//	SaveInternal(TEXT("TestSave"));
//}
//
//
//#if WITH_EDITOR
//void UDreamGameInstanceSubsystem::HandlePrePIEEnded(bool bIsSimulating)
//{
//}
//#endif
//
//
