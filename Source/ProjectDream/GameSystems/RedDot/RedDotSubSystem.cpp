// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/RedDot/RedDotSubSystem.h"
#include "GameSystems/Achievements/AchievementsSubsystem.h"

void URedDotSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency<UAchievementsSubsystem>();

	RedDotCountByType.SetNum(ToIndex(ERedDotType::MAX));

	if (UGameInstance* GI = GetGameInstance())
	{
		if (AchieveSubSys = GI->GetSubsystem<UAchievementsSubsystem>())
		{
			const TMap<FName,FAchievementState>& AchieveStateMap = AchieveSubSys->GetAllAchievementState();
			
			for (const auto& KVP : AchieveStateMap)
			{
				const FAchievementState& State = KVP.Value;
				if (State.UnlockedTime != FDateTime::MinValue() && State.bRewardClaimed == false)
				{
					IncrementRedDot(ERedDotType::Achievement);
				}
			}
		}
	}
}

void URedDotSubSystem::Deinitialize()
{
	Super::Deinitialize();
}

bool URedDotSubSystem::HasAnyRedDot()
{
	for (int32 Count : RedDotCountByType)
	{
		if (Count > 0)
		{
			return true;
		}
	}
	return false;
}

void URedDotSubSystem::IncrementRedDot(ERedDotType RedDotType)
{
	RedDotCountByType[ToIndex(RedDotType)]++;
	UE_LOG(LogTemp, Warning, TEXT("RedDot 갯수 : %d"), RedDotCountByType[ToIndex(ERedDotType::Achievement)]);
}

void URedDotSubSystem::ClearAchievementRedDot(const FName& EventId)
{
	RedDotCountByType[ToIndex(ERedDotType::Achievement)]--;
	if (RedDotCountByType[ToIndex(ERedDotType::Achievement)] <= 0)
	{
		RedDotCountByType[ToIndex(ERedDotType::Achievement)] = 0;
	}
	UE_LOG(LogTemp, Warning, TEXT("RedDot 갯수 : %d"), RedDotCountByType[ToIndex(ERedDotType::Achievement)]);
}

inline int32 URedDotSubSystem::ToIndex(ERedDotType RedDotType)
{
	return static_cast<int32>(RedDotType);
}

bool URedDotSubSystem::IsGamePlaying() const
{
	const UWorld* World = GetWorld();
	return World && World->IsGameWorld();
	
}

