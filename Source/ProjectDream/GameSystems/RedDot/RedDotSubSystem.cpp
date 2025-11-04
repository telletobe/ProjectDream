// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/RedDot/RedDotSubSystem.h"
#include "RedDotState.h"
#include "GameSystems/Achievements/AchievementsSubsystem.h"


void URedDotSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void URedDotSubSystem::Deinitialize()
{
	Super::Deinitialize();
}

void URedDotSubSystem::MarkSeen(const FName& EventId)
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAchievementsSubsystem* AchieveSubSys = GI->GetSubsystem<UAchievementsSubsystem>())
		{
			FAchievementState* State = AchieveSubSys->GetAchievementStateById(EventId);
			State->bRewardClaimed = true;
			AchieveSubSys->RequestSave(AchieveSubSys->GetAllAchievementState());
		}
		else
		{
			return;
		}
	}
}

