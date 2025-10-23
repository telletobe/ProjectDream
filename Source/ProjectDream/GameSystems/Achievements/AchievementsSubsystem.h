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
	void GetViewData(TArray<FAchievementViewData>& OutViewArr, TArray<FName>& OutIdsArr);
	void GetViewDataById(FAchievementViewData& OutView,const FName& EventId);
	void RequestSave(const TMap<FName,FAchievementState>& StateData);
	UFUNCTION()	void HandleAchieveEvent(FName EventId); // 현재 미사용
private:
	TMap<FName, FAchievementState> LoadNow();
	void SaveNow(const TMap<FName, FAchievementState>& InStates);
	void FlushPendingSave();
	void LoadAchievementDef(TArray<FAchievementDef>& OutDefs) const;
	bool UpdateAchieve(FName& EventId);
	bool HandleAchivementEvent(FName& EventId); // 현재 미사용
	void UpdateProgress(const FAchievementDef& OutDef, const FName& EventId);
private:
	void HandleLogin();
	UFUNCTION()	void HandleItemAdded(EItemCategory ItemCategory, int32 ItemID);
private:
	UPROPERTY(EditAnywhere, config, Category = "Achievements")
	TSoftObjectPtr<class UDreamAchievements> AchievementData;
	TMap<FName, FAchievementState> PendingState;					
	FTimerHandle SaveTimerHandle;
	float SaveDelay = 2.0f;

	TMap <FName, FAchievementDef> Definition;
	TMap<EClearRule, TArray<FAchievementDef>> DefsByEventType;
	//
	TArray<FAchievementViewData> ViewsCash;
	TArray<FName> DefIdsCash;
	TMap<FName, FAchievementViewData> IdsByView;
	//
	UPROPERTY() TMap<FName, FAchievementState> States;

};

namespace AchievementIDParse
{
	struct FParseResult
	{
		bool bValid = false;
		bool bHasClearRule = false;
		bool bHasItemData = false;

		EClearRule		Rule = EClearRule::EventNone;
		EItemCategory	ItemCat = EItemCategory::Other;
		int32			ItemID = INDEX_NONE;
	};

	bool StringToItemType(const FString& S, EItemCategory& Out);
	bool StringToAchievementType(const FString& S, EClearRule& Out);
	// 디버깅 용
	const TCHAR* ToString(EItemCategory Cat);
	//----------------------------------------------
	//코드 병합중...
	bool ParseID(const FName& AchievementID, FParseResult& OutFParseReulst);
	//-----------------------------------------------

	bool ParseItemTypeAndId(const FName AchievementId, FParseResult& OutFParseReulst);
	bool ParseAchievementType(const FName AchievementId, FParseResult& OutFParseReulst);
	
}