// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/RedDot/RedDotSubSystem.h"
#include "RedDotState.h"
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

void URedDotSubSystem::OnRedDot()
{
	RedDotCountByType[ToIndex(ERedDotType::Achievement)]++;
	UE_LOG(LogTemp,Warning,TEXT("Call OnRedDot"));
}

void URedDotSubSystem::OffRedDot(const FName& EventId)
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAchievementsSubsystem* AchieveSubSys = GI->GetSubsystem<UAchievementsSubsystem>())
		{
			FAchievementState* State = AchieveSubSys->GetAchievementStateById(EventId);
			State->bRewardClaimed = true;
			RedDotCountByType[ToIndex(ERedDotType::Achievement)]--;

			AchieveSubSys->RequestSave(AchieveSubSys->GetAllAchievementState());
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Call OffRedDot"));
}

inline int32 URedDotSubSystem::ToIndex(ERedDotType RedDotType)
{
	return static_cast<int32>(RedDotType);
}

