// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DreamGameInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTDREAM_API UDreamGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	//void Save(const FString& SlotName = TEXT("TestSave"));
	//void Load(const FString& SlotName = TEXT("TestSave"));
	bool SaveInventoryJson(const FString& SlotName,class UGameInventory* Inv);
	bool LoadInventoryJson(const FString& SlotName);
protected:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:

	//void SaveInternal(const FString& SlotName);
	//void LoadInternal(const FString& SlotName);
	//
	//// 자동 세이브
	//UFUNCTION()
	//void HandleAppWillEnterBackground();
	//UFUNCTION()
	//void HandlePreExit();
	//UFUNCTION()
	//void HandlePrePIEEnded(bool bIsSimulating);

	//int32 PendingLoadRetries = 0;
};
