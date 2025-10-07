// Fill out your copyright notice in the Description page of Project Settings.


#include "AchievementsManager.h"
#include "DreamAchievements.h"
#include "GameSystems/Achievements/AchievementsSubsystem.h"
#include <Kismet/GameplayStatics.h>
#include "../Save/JsonSaveGame.h"
#include <GameSystems/Save/DreamSaveGame.h>
#include "GameSystems/Common/GameEventBus/GameEventBus.h"

static const FString AchievementsSlot = TEXT("Achievements");

void UAchievementsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Manager = NewObject<UAchievementsManager>(this);
	check(Manager);

	/*
	* GPT :
		Manager는 저장과 로드를 호출만 해주면 돼기때문에 
		람다를 활용하여 의존성을 낮춰주고, 
		현재는 JSon을 활용하지만, 다른 세이브 방식을 이용할 경우, 클래스 수정없이
		람다를 활용한 저장소 교체로 유연성을 갖춤.
		테스트 시 테스트용 람다를 적용시켜 테스트 환경에서도 동작을 변경하기 쉬움.

		만들어보니 Manager가 Save를 호출할 필요가없는데?
	*/

	//Manager->SaveFunc = [this](const TMap<FName,FAchievementState>& States)
	//{
	//	if (UGameInstance* GI = GetGameInstance())
	//	{
	//		if (UAchievementsSubsystem* SubSys = GI->GetSubsystem<UAchievementsSubsystem>())
	//		{
	//			SubSys->RequestSave(States);
	//		}
	//	}
	//};

	Manager->LoadFunc = []() -> TMap<FName, FAchievementState>
	{
			TArray<FAchievementState> Arr;
			TMap<FName, FAchievementState> Out;

			if (SaveJson::LoadArrayFromFile<FAchievementState>(TEXT("Achievements"), Arr))
			{
				for (const FAchievementState& S : Arr)
				{
					if (!S.Id.IsNone())
					{
						Out.Add(S.Id, S);
					}
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("LoadFunc Call"));
			return Out;
	};

	TArray<FAchievementDef> Defs;
	// 업적  데이터 에셋을 가져오기 위한 부분
	const FSoftObjectPath Path = AchievementData.ToSoftObjectPath();
	UE_LOG(LogTemp,Warning,TEXT("AchievementData Path = %s"),*Path.ToString());
	
	GetDefAchieve(Defs);
	
	Manager->Initialize(Defs);
	UE_LOG(LogTemp,Warning,TEXT("AchievementSubSystem Call"));

	if (UGameEventBus* Bus = Collection.InitializeDependency<UGameEventBus>())
	{
		Bus->OnGameEvent.AddUniqueDynamic(this, &UAchievementsSubsystem::HandleAchieveEvent);
		UE_LOG(LogTemp, Warning, TEXT("Handle Game Event Add"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Bus 서브시스템이 존재하지 않음."));
	}

}

void UAchievementsSubsystem::GetDefAchieve(TArray<FAchievementDef>& OutDefs) const
{
	const FSoftObjectPath Path = AchievementData.ToSoftObjectPath();
	UE_LOG(LogTemp, Warning, TEXT("AchievementData Path = %s"), *Path.ToString());

	if (AchievementData.IsValid())
	{
		OutDefs = AchievementData.Get()->AchievementList;
	}
	else if (Path.IsValid())
	{
		if (UDreamAchievements* DA = AchievementData.LoadSynchronous())
		{
			UE_LOG(LogTemp, Warning, TEXT("Loading Achievment Definition"));
			OutDefs = DA->AchievementList;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Achievement Definition Data Loading Failed"));
	}
}


void UAchievementsSubsystem::Deinitialize()
{
	if (UGameEventBus* Bus = GetGameInstance()->GetSubsystem<UGameEventBus>())
	{
		Bus->OnGameEvent.RemoveAll(this);
	}
	UE_LOG(LogTemp, Warning, TEXT("AchievementSubSystem Down"));
}

void UAchievementsSubsystem::HandleAchieveEvent(FName EventId)
{
	UE_LOG(LogTemp, Warning, TEXT("HandleAchieveEvent Call"));

	if (!Manager) return;

	const FAchievementDef* Definition =  Manager->GetDefById(EventId);

	if (!Definition) return; // 사용되지 않는 업적

	TMap<FName, FAchievementState>& Data = Manager->GetAllStates();
	FAchievementState* PlayerState =  Manager->GetStateById(EventId);

	if (!PlayerState ) 
	{
		UE_LOG(LogTemp,Warning,TEXT("플레이어 업적 정보를 가져오지 못했습니다."));
		return;
	}

	if (Definition->AchieveType == EAchievementType::Instant) // 즉시형
	{
		if (Definition->EventType == EGameEventType::GameStarted)
		{
			if (UpdateAchieve(EventId))
			{
				RequestSave(Manager->GetAllStates());
				UE_LOG(LogTemp, Warning, TEXT("Save 완료!"));
				OnAchievementUpdated.Broadcast(EventId);
			}
		}
		else if (Definition->EventType == EGameEventType::ItemGet)
		{
			if (UpdateAchieve(EventId))
			{
				RequestSave(Manager->GetAllStates());
				UE_LOG(LogTemp, Warning, TEXT("귀한 아이템을 얻으면 세이브"));
				OnAchievementUpdated.Broadcast(EventId);
			}
		}
	}
	else if(Definition->AchieveType == EAchievementType::Progressive)// 누적형
	{
		if (Definition->EventType == EGameEventType::GameStarted)
		{
			if (HandleProgressEvent(EventId, *Definition, *PlayerState))
			{
				RequestSave(Manager->GetAllStates());
				UE_LOG(LogTemp, Warning, TEXT("Save 완료!"));
				OnAchievementUpdated.Broadcast(EventId);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Save 실패"));
			}
		}
		else if (Definition->EventType == EGameEventType::ItemGet)
		{
			if (HandleProgressEvent(EventId, *Definition, *PlayerState))
			{
				RequestSave(Manager->GetAllStates());
				UE_LOG(LogTemp, Warning, TEXT("Save 완료!"));
				OnAchievementUpdated.Broadcast(EventId);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Save 실패"));
			}
		}
	}
}

void UAchievementsSubsystem::RequestSave(const TMap<FName, FAchievementState>& StateData)
{
	PendingState = StateData;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SaveTimerHandle);
		World->GetTimerManager().SetTimer(
			SaveTimerHandle,this,&UAchievementsSubsystem::FlushPendingSave,SaveDelay
		);
		UE_LOG(LogTemp,Warning,TEXT("Call RequestSave"));
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("World가 존재하지않음."));
	}
}

void UAchievementsSubsystem::FlushPendingSave()
{
	UE_LOG(LogTemp,Warning,TEXT("Call FlushPending Save"));
	if (PendingState.Num() == 0) return;

	TArray<FAchievementState> StateArr;

	for (const auto& KVP : PendingState)
	{
		StateArr.Add(KVP.Value);
	}

	SaveJson::SaveArrayToFile(TEXT("Achievements"), StateArr);
	PendingState.Empty();
}

bool UAchievementsSubsystem::UpdateAchieve(FName& EventId)
{
	if (!Manager) return false;
	Manager->UnlockAchieve(EventId);

	return true;
}

bool UAchievementsSubsystem::HandleProgressEvent(FName& EventId, const FAchievementDef& OutDef, FAchievementState& OutState)
{
	if (OutDef.Target >= OutState.Progress && !OutState.bUnlocked) // 업적 클리어 전
	{
		UpdateProgress(EventId);

		if (OutDef.Target <= OutState.Progress)
		{
			if (UpdateAchieve(EventId))
			{
				UE_LOG(LogTemp, Warning, TEXT("업적 클리어"));
				return true;				
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("누적 정보를 저장하였습니다."));
				return true;
			}
		}
		return true;
	}
	else // 이미 클리어 됀 후. 저장을 하지않거나 누적 상황 별도 갱신
	{
		if (UpdateProgress(EventId))
		{
			UE_LOG(LogTemp, Warning, TEXT("누적정보 세이브 완료"));
			return true;
		}
	}

	return false;
}

bool UAchievementsSubsystem::UpdateProgress(FName& EventId)
{
	if (!Manager) return false;

	Manager->AddProgress(EventId);
	return true;
}

void UAchievementsSubsystem::SaveNow(const TMap<FName, FAchievementState>& InStates)
{
	// TMap -> TArray 변환 후 저장
	TArray<FAchievementState> Arr;
	Arr.Reserve(InStates.Num());
	for (const TPair<FName, FAchievementState>& KVP : InStates)
	{
		Arr.Add(KVP.Value);
	}

	const bool bOk = SaveJson::SaveArrayToFile<FAchievementState>(AchievementsSlot, Arr, /*bPretty=*/true);
	if (!bOk)
	{
		UE_LOG(LogTemp, Error, TEXT("[Achievements] JSON save failed: %s"), *AchievementsSlot);
	}
}

TMap<FName, FAchievementState> UAchievementsSubsystem::LoadNow()
{
	TMap<FName, FAchievementState> Out;

	// 파일 -> 배열 로드
	TArray<FAchievementState> Arr;
	if (!SaveJson::LoadArrayFromFile<FAchievementState>(AchievementsSlot, Arr))
	{
		// 최초 실행 등 파일이 없을 수 있음
		return Out;
	}

	// 배열 -> 맵 환원
	for (const FAchievementState& S : Arr)
	{
		if (!S.Id.IsNone())
		{
			Out.Add(S.Id, S);
		}
	}
	return Out;
}
