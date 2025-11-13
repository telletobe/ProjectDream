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
		if (UAchievementsSubsystem* AchieveSubSys = GI->GetSubsystem<UAchievementsSubsystem>())
		{
			TMap<FName,FAchievementState> AchieveState = AchieveSubSys->GetAllAchievementState();
			TArray<FAchievementState> AchieveStateArr;
			AchieveState.GenerateValueArray(AchieveStateArr);

			for (int32 i = 0; i < AchieveStateArr.Num(); i++)
			{
				if (AchieveStateArr[i].UnlockedTime != FDateTime::MinValue() && AchieveStateArr[i].bRewardClaimed == false)
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

bool URedDotSubSystem::CheckRedDotCount()
{
	if (RedDotCountByType[ToIndex(ERedDotType::Achievement)] != 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void URedDotSubSystem::IncrementRedDot(ERedDotType RedDotType)
{
	RedDotCountByType[ToIndex(RedDotType)]++;
	UE_LOG(LogTemp,Warning,TEXT("RedDot 갯수 : %d"), RedDotCountByType[ToIndex(RedDotType)]);
}

void URedDotSubSystem::ClearAchievementRedDot(const FName& EventId)
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAchievementsSubsystem* AchieveSubSys = GI->GetSubsystem<UAchievementsSubsystem>())
		{
			AchieveSubSys->ClaimReward(EventId);
		}
	}
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

