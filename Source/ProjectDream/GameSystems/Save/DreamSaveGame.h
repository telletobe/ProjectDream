// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "../Inventory/GameInventory.h"
#include "DreamSaveGame.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FInventorySave
{
	GENERATED_BODY()


public:
	FInventorySave() {};

	UPROPERTY(SaveGame)
	TArray<FGameItemData> SaveData;

	// 세이브 시 필요한 파일들 추가
};

UCLASS()
class PROJECTDREAM_API UDreamSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	FInventorySave& GetInventorySaveData() { return InventorySaveData; }

protected:

private:
	
	UPROPERTY(SaveGame)
	FInventorySave InventorySaveData;

};
