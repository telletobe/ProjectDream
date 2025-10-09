// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DreamAchievements.h"
#include "AchievementsManager.h"
#include "AchievementsSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAchievementUpdated, FName , Id);

class UAchievementsManager;

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

	void UnlockInstant(FName Id)
	{
		if (Manager) Manager->UnlockAchieve(Id);
	}

	bool IsUnlocked(FName Id) const
	{
		return Manager ? Manager->IsUnlocked(Id) : false;
	}

	int32 GetProgress(FName Id) const
	{
		return Manager ? Manager->GetProgress(Id) : 0;
	}

	UAchievementsManager* GetManager() const { return Manager; }

	UFUNCTION()
	void HandleAchieveEvent(FName EventId);

	void RequestSave(const TMap<FName,FAchievementState>& StateData);

private:
	TMap<FName, FAchievementState> LoadNow();
	void SaveNow(const TMap<FName, FAchievementState>& InStates);
	void FlushPendingSave();
	void GetDefAchieve(TArray<FAchievementDef>& OutDefs) const;
private:
	UPROPERTY()
	UAchievementsManager* Manager = nullptr;

	UPROPERTY(EditAnywhere, config, Category = "Achievements")
	TSoftObjectPtr<class UDreamAchievements> AchievementData;

	bool UpdateAchieve(FName& EventId);
	bool HandleProgressEvent(FName& EventId, const FAchievementDef& OutDef, FAchievementState& OutState);
	bool UpdateProgress(FName& EventId);
	TMap<FName, FAchievementState> PendingState;
	FTimerHandle SaveTimerHandle;
	float SaveDelay = 2.0f;
};
