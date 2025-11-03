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
			const TMap<FName,FAchievementState>& States = AchieveSubSys->GetAllAchievementState();
			//TMap<FName, int32>& SeenMap = AchieveSubSys->GetSeenReivision();
			if (const FAchievementState* State = States.Find(EventId))
			{
				//SeenMap.Add(EventId, State->Seen.GetRevision());
				UE_LOG(LogTemp,Warning,TEXT("Call MarkSeen"));
			}
		}
		else
		{
			return;
		}
	}
}

