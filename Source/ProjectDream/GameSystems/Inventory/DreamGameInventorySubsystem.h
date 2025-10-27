// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DreamItemDTO.h"
#include "DreamGameInventorySubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LOGInvSubSystem, Log, All);

class UGameInventory;

UCLASS(config = Game, DefaultConfig)
class PROJECTDREAM_API UDreamGameInventorySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	UPROPERTY(Editanywhere,config, Category ="LoadData") TSoftObjectPtr<UDreamItemDTO> ItemDefinitions;

	const FDreamGameItemDef* GetItemDefByKey(int32 ItemID,EItemCategory Category) const;
	const TMap<int32, TMap<EItemCategory, FDreamGameItemDef>> GetAllItemDefs();
	TObjectPtr<UGameInventory> GetInventory() { return PlayerInventory.Get(); }
	float GetInventoryWeight() const;
private:
	TMap<int32,TMap<EItemCategory,FDreamGameItemDef>> ItemsCatalog;
	TObjectPtr<UGameInventory> PlayerInventory = nullptr;
};
