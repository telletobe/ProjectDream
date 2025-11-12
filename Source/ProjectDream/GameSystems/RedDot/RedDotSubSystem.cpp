// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/RedDot/RedDotSubSystem.h"
#include "GameSystems/Achievements/AchievementsSubsystem.h"



void URedDotSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	RedDotCountByType.SetNum(ToIndex(ERedDotType::MAX));
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

void URedDotSubSystem::IncrementRedDot()
{
	RedDotCountByType[ToIndex(ERedDotType::Achievement)]++;
	UE_LOG(LogTemp,Warning,TEXT("RedDot 갯수 : %d"), RedDotCountByType[ToIndex(ERedDotType::Achievement)]);
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
	UE_LOG(LogTemp, Warning, TEXT("RedDot 갯수 : %d"), RedDotCountByType[ToIndex(ERedDotType::Achievement)]);
}

inline int32 URedDotSubSystem::ToIndex(ERedDotType RedDotType)
{
	return static_cast<int32>(RedDotType);
}

