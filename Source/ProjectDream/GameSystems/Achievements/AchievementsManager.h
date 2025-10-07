// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DreamAchievements.h"
#include "AchievementUIViewData.h"
#include "AchievementsManager.generated.h"

UCLASS()
class PROJECTDREAM_API UAchievementsManager : public UObject
{
	GENERATED_BODY()
	
public:
	//TFunction<void(const TMap<FName, FAchievementState>&)> SaveFunc;
	TFunction<TMap<FName, FAchievementState>()> LoadFunc;

	void Initialize(const TArray<FAchievementDef>& InDefs);

	UFUNCTION()
	bool IsUnlocked(FName Id) const;

	UFUNCTION()
	int32 GetProgress(FName Id) const;

	UFUNCTION()
	void AddProgress(FName Id, int32 Delta = 1);

	UFUNCTION()
	void UnlockAchieve(FName Id);

	// 모든 상태 가져오기
	TMap<FName, FAchievementState>& GetAllStates();
	const TMap<FName, FAchievementDef>& GetAllDefs() const;

	// 업적 하나의 정보 가져오기
	FAchievementState* GetStateById(const FName& Id);
	const FAchievementDef* GetDefById(const FName& Id) const;

	void GetAllAchievementsViewData(TArray<FAchievementViewData>& OutViews, TArray<FName>& OutIds);
	void GetAchievementViewDataById(FAchievementViewData& Out,const FName& EventId);

private:
	UPROPERTY()
	TMap <FName, FAchievementDef> Definition;

	UPROPERTY()
	TMap<FName, FAchievementState> States;

	inline void EnsureStateExists(FName Id)
	{
		if (!States.Contains(Id))
		{
			FAchievementState NewS; NewS.Id = Id;
			States.Add(Id, NewS);
		}
	}

	//inline void SaveNow()
	//{
	//	if (SaveFunc) SaveFunc(States);
	//}
};
