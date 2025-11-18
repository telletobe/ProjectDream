// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/RedDot/RedDotSubSystem.h"
#include "GameSystems/Achievements/AchievementsSubsystem.h"

//레드닷 UI에서 서브시스템을 조회하여 UI갱신
/*
서브시스템에서 업적의 상태를 Tick으로 확인하여 레드닷의 유무 파악
-> 유무 파악 후 ui로 전파
*/
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

void URedDotSubSystem::Tick(float DeltaTime)
{
	
}

int32 URedDotSubSystem::CheckAchievementRedDotCount()
{
	return RedDotCountByType[ToIndex(ERedDotType::Achievement)];

}

void URedDotSubSystem::IncrementRedDot(ERedDotType RedDotType)
{
	RedDotCountByType[ToIndex(RedDotType)]++;
	UE_LOG(LogTemp, Warning, TEXT("RedDot 갯수 : %d"), RedDotCountByType[ToIndex(ERedDotType::Achievement)]);
}

void URedDotSubSystem::ClearAchievementRedDot(const FName& EventId)
{
	if (!AchieveSubSys)
	{
		AchieveSubSys->ClaimReward(EventId);
	}
	RedDotCountByType[ToIndex(ERedDotType::Achievement)]--;
	if (RedDotCountByType[ToIndex(ERedDotType::Achievement)] <= 0)
	{
		RedDotCountByType[ToIndex(ERedDotType::Achievement)] = 0;
	}
	UE_LOG(LogTemp, Warning, TEXT("RedDot 갯수 : %d"), RedDotCountByType[ToIndex(ERedDotType::Achievement)]);
}

bool URedDotSubSystem::IsTickable() const
{
	for (int32 Count : RedDotCountByType)
	{
		if (Count > 0) return true;
	}

	return false;
}

inline int32 URedDotSubSystem::ToIndex(ERedDotType RedDotType)
{
	return static_cast<int32>(RedDotType);
}

