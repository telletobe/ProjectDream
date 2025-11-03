// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/RedDot/RedDotSubSystem.h"
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
			TMap<FName, int32>& SeenMap = AchieveSubSys->GetSeenReivision();
			if (const FAchievementState* State = States.Find(EventId))
			{
				SeenMap.Add(EventId, State->GetRevision());
				//이곳에서 SeenRevision 저장이 필요할 경우 저장고려 혹은 다른위치에서 저장
				UE_LOG(LogTemp,Warning,TEXT("Call MarkSeen"));
			}
		}
		else
		{
			return;
		}
	}
	// 저장 TestCode
	// SaveNow 템플릿화 필요
	//TArray<FAchievementSeen> Arr;
	//Arr.Reserve(SeenRevisionById.Num());
	//for (const TPair<FName, FAchievementSeen>& KVP : SeenRevisionById)
	//{
	//	Arr.Add(KVP.Value);
	//}
	//SaveJson::SaveArrayToFile(AchievementsSeenSlot, Arr);
}

