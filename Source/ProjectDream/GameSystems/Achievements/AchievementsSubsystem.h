// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DreamAchievements.h"
#include "../Inventory/DreamItemDTO.h"
#include "AchievementViewData.h"
#include "AchievementsSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAchievementUpdated, FName , Id);

UCLASS(config=Game, DefaultConfig)
class PROJECTDREAM_API UAchievementsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FAchievementUpdated OnAchievementUpdated;
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void AddProgress(FName Id, int32 Delta = 1);
	void UnlockInstant(FName Id);
	bool IsUnlocked(FName Id) const;
	int32 GetProgress(FName Id) const;
	const FAchievementDef* GetAchievementDefById(const FName& EventId) const;
	const FAchievementState* GetAchievementStateById(const FName& EventId) const;
	const TMap <FName, FAchievementDef> GetAllAchievementDef();
	const TMap<FName, FAchievementState> GetAllAchievementState();
	UFUNCTION()
	void HandleAchieveEvent(FName EventId);
	void RequestSave(const TMap<FName,FAchievementState>& StateData);
	void GetViewData(TArray<FAchievementViewData>& OutViewArr, TArray<FName>& OutIdsArr);
	void GetViewDataById(FAchievementViewData& OutView,const FName& EventId);
private:
	TMap<FName, FAchievementState> LoadNow();
	void SaveNow(const TMap<FName, FAchievementState>& InStates);
	void FlushPendingSave();
	void LoadAchievementDef(TArray<FAchievementDef>& OutDefs) const;
	bool UpdateAchieve(FName& EventId);
	bool HandleProgressEvent(FName& EventId, const FAchievementDef& OutDef, FAchievementState& OutState);
	bool UpdateProgress(FName& EventId);
	UFUNCTION()
	void HandleItemAdded(EItemCategory ItemCategory, int32 ItemID);
private:
	UPROPERTY(EditAnywhere, config, Category = "Achievements")
	TSoftObjectPtr<class UDreamAchievements> AchievementData;
	TMap<FName, FAchievementState> PendingState;					
	FTimerHandle SaveTimerHandle;
	float SaveDelay = 2.0f;

	TMap <FName, FAchievementDef> Definition;
	TMap<EGameEventType, TArray<FAchievementDef>> DefsByEventType;
	//
	TArray<FAchievementViewData> ViewsCash;
	TArray<FName> DefIdsCash;
	TMap<FName, FAchievementViewData> IdsByView;
	//
	UPROPERTY() TMap<FName, FAchievementState> States;

};

namespace AchIdParse
{
	static bool StringToItemType(const FString& S, EItemCategory& Out)
	{
		if (S.Equals(TEXT("Equipment"), ESearchCase::IgnoreCase))
		{
			Out = EItemCategory::Equipment;
			return true;
		}
		if (S.Equals(TEXT("Consumable"), ESearchCase::IgnoreCase))
		{
			Out = EItemCategory::Consumable;
			return true;
		}
		if (S.Equals(TEXT("Other"), ESearchCase::IgnoreCase))
		{
			Out = EItemCategory::Other;
			return true;
		}
		return false;
	} 

	static bool StringToAchievementType(const FString& S, EGameEventType& Out)
	{
		if (S.Equals(TEXT("Login")))
		{
			Out = EGameEventType::Login;
			return true;
		}
		if (S.Equals(TEXT("InventoryAdded")))
		{
			Out = EGameEventType::InventoryAdded;
			return true;
		}
		return false;
	}

	static bool ParseItemTypeAndId(const FName AchievementId, EItemCategory& OutType, int32& OutItemId)
	{
		const FString S = AchievementId.ToString();
		TArray<FString> Tokens;
		S.ParseIntoArray(Tokens, TEXT("_"), true);
		if (Tokens.Num() < 3) return false;

		int32 Num = 0;
		if (LexTryParseString<int32>(Num, *Tokens.Last()))
		{
			OutItemId = Num;
			
			EItemCategory ItemCategory;
			if (StringToItemType(Tokens[Tokens.Num() - 2], ItemCategory)) OutType = ItemCategory;
			
			return true;
		}
		return false;		
	}

	static bool ParseAchievemType(const FName AchievementId, EGameEventType& OutType)
	{
		const FString S = AchievementId.ToString();
		TArray<FString> Tokens;
		S.ParseIntoArray(Tokens, TEXT("_"), true);
		if (Tokens.Num() < 2)
		{
			return false;
		}

		EGameEventType GameEventType;
		if (StringToAchievementType(Tokens[Tokens.Num() - 1], GameEventType)) OutType = GameEventType;
		return true;
	}

	static const TCHAR* ToString(EItemCategory Cat)
	{
		switch (Cat)
		{
		case EItemCategory::Equipment:  return TEXT("Equipment");
		case EItemCategory::Consumable: return TEXT("Consumable");
		case EItemCategory::Other:      return TEXT("Other");
		default:                        return TEXT("Unknown");
		}
	}

}