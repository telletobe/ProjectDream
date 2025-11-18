// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "../Achievements/DreamAchievements.h"
#include "RedDotSubSystem.generated.h"

class UAchievementsSubsystem;

UENUM()
enum class ERedDotType : int8
{
	Achievement,
	Inventory,
	MAX
};

UCLASS()
class PROJECTDREAM_API URedDotSubSystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	int32 CheckAchievementRedDotCount();
	void IncrementRedDot(ERedDotType RedDotType);
	void ClearAchievementRedDot(const FName& EventId);
public:
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override
	{
		// 프로파일러에서 표시되는 카테고리 태그
		RETURN_QUICK_DECLARE_CYCLE_STAT(UTimeSubsystemManager, STATGROUP_Tickables);
	}

private:
	inline int32 ToIndex(ERedDotType RedDotType);
private:
	TArray<int32> RedDotCountByType;
	TObjectPtr<UAchievementsSubsystem> AchieveSubSys = nullptr;
};
