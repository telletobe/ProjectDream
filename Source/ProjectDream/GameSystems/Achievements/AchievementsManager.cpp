// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/Achievements/AchievementsManager.h"
#include "ProjectDreamCharacter.h"

void UAchievementsManager::Initialize(const TArray<FAchievementDef>& InDefs)
{
	Definition.Empty();

	// 업적 정의 가져오기
	for (const auto& Def : InDefs)
	{
		if (Def.Id.IsNone()) continue;
		Definition.Add(Def.Id, Def);
		EnsureStateExists(Def.Id);
	}

	// 저장되어 있는 유저의 업적 진행도 정보.
	if (LoadFunc)
	{
		const TMap<FName, FAchievementState> Loaded = LoadFunc();
		for (const auto& It : Loaded)
		{
			if (Definition.Contains(It.Key))
			{
				States.FindOrAdd(It.Key) = It.Value;
			}
		}
	}

	UE_LOG(LogTemp,Warning,TEXT("Manager Init"));
}

bool UAchievementsManager::IsUnlocked(FName Id) const
{
	if (const FAchievementState* S = States.Find(Id)) return S->bUnlocked;
	return false;
}

int32 UAchievementsManager::GetProgress(FName Id) const
{
	if (const FAchievementState* S = States.Find(Id)) return S->Progress;
	return 0;
}

void UAchievementsManager::AddProgress(FName Id, int32 Delta)
{
	const FAchievementDef* Def = Definition.Find(Id);
	if (!Def || Def->AchieveType != EAchievementType::Progressive) return;

	FAchievementState* S = States.Find(Id);
	if (!S) return;

	//const int32 Target = FMath::Max(1, Def->Target);
	//S.Progress = FMath::Clamp(S.Progress + Delta, 0, Target);
	S->Progress += Delta;

	if (S->Progress >= Def->Target)
	{
		UnlockAchieve(Id);
	}

}

void UAchievementsManager::UnlockAchieve(FName Id)
{
	const FAchievementDef* Def = Definition.Find(Id);
	if (!Def) return;

	FAchievementState* S = States.Find(Id);
	if (!S) return;

	S->bUnlocked = true;
	S->UnlockedAt = FDateTime::Now();

}

TMap<FName, FAchievementState>& UAchievementsManager::GetAllStates()
{
	return States;
}

const TMap<FName, FAchievementDef>& UAchievementsManager::GetAllDefs() const
{
	if (Definition.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Empty"));
	}
	return Definition;
}

FAchievementState* UAchievementsManager::GetStateById(const FName& Id)
{
	if (States.IsEmpty()) return nullptr;

	return States.Find(Id);
}

const FAchievementDef* UAchievementsManager::GetDefById(const FName& Id) const
{
	if (Definition.IsEmpty()) return nullptr;

	return Definition.Find(Id);
}

void UAchievementsManager::GetAllAchievementsViewData(TArray<FAchievementViewData>& OutViews , TArray<FName>& OutIds)
{
	OutViews.Reset();
	OutIds.Reset();

	for (const auto& KVP : States)
	{
		const FName Id = KVP.Key;
		const FAchievementState& State = KVP.Value;
		const FAchievementDef* Def = Definition.Find(Id);

		OutIds.Add(Id);
		
		FAchievementViewData ViewData;	
		ViewData.Title = Def ? Def->Title : FText::FromName(Id);
		ViewData.Description = Def ? Def->Description : FText::GetEmpty();
		ViewData.TargetValue = Def ? Def->Target : 0.f;
		ViewData.Progress = State.Progress;
		ViewData.bUnlocked = State.bUnlocked;
		OutViews.Add(ViewData);
	}
}

void UAchievementsManager::GetAchievementViewDataById(FAchievementViewData& Out, const FName& EventId)
{
	const FAchievementState* State = States.Find(EventId);
	const FAchievementDef* Def = Definition.Find(EventId);

	if (!State || !Def) return;

	Out.Title = Def->Title;
	Out.Description = Def->Description;
	Out.TargetValue = Def->Target;
	Out.Progress = State->Progress;
	Out.bUnlocked = State->bUnlocked;
}