// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DreamAchievements.h"
#include "../RedDot/RedDotState.h"
#include "../Inventory/DreamItemDTO.h"
#include "AchievementViewData.h"
#include "AchievementsSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAchievementUpdated,FName, EventId);

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
public:
	const FAchievementDef* GetAchievementDefById(const FName& EventId) const;
	const FAchievementState* GetAchievementStateById(const FName& EventId) const;
	FAchievementState* GetAchievementStateById(const FName& EventId);
	const TMap <FName, FAchievementDef>& GetAllAchievementDef() const;
	const TMap<FName, FAchievementState>& GetAllAchievementState() const;
	void GetAllViewData(TArray<FAchievementViewData>& OutViewArr, TArray<FName>& OutIdsArr);
	void GetViewDataById(FAchievementViewData& OutView,const FName& EventId);
	UFUNCTION()	void DispatchAchivementEvent(EItemCategory ItemCategory, int32 ItemID);
public:
	void RequestSave(const TMap<FName,FAchievementState>& StateData);
private:
	void UpdateView(FAchievementViewData& OutViewData , FAchievementState& OutState);
	void UpdateState(FAchievementState& OutStateData, const FAchievementDef& OutDef);
	void UpdateProgress(const FName& EventId);
	bool HandleAchivementEvent(FName& EventId);
	void HandleItemAdded(EItemCategory ItemCategory, int32 ItemID);
	void HandleLogin();
private:
	TMap<FName, FAchievementState> LoadNow();
	void SaveNow(const TMap<FName, FAchievementState>& InStates);
	void FlushPendingSave();
	void LoadAchievementDef(TArray<FAchievementDef>& OutDefs) const;
private:
	UPROPERTY(EditAnywhere, config, Category = "Achievements")
	TSoftObjectPtr<class UDreamAchievements> AchievementData;
	TMap<FName, FAchievementState> PendingState;					
	FTimerHandle SaveTimerHandle;
	float SaveDelay = 2.0f;

	TMap <FName, FAchievementDef> Definition;
	TMap<EClearRule, TArray<FAchievementDef>> DefsByEventType;
	TMap<FName, FAchievementViewData> IdsByView;
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
		EItemCategory	ItemCat = EItemCategory::None;
		int32			ItemID = INDEX_NONE;
	};

	bool StringToItemType(const FString& S, EItemCategory& Out);
	bool ParseClearRuleToken(const FString& S, EClearRule& Out);
	// 디버깅 용
	const TCHAR* ToString(EItemCategory Cat);

	bool ParseID(const FName& AchievementID, FParseResult& OutFParseReulst);
	bool ParseItemTypeAndId(const TArray<FString>& OutTokens, FParseResult& OutFParseReulst);
	bool ParseAchievementType(const TArray<FString>& OutTokens, FParseResult& OutFParseReulst);
	
}