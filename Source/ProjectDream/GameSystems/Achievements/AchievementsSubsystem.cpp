// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/Achievements/AchievementsSubsystem.h"
#include "DreamAchievements.h"
#include "AchievementViewData.h"
#include <Kismet/GameplayStatics.h>
#include "../Save/JsonSaveGame.h"
#include <GameSystems/Save/DreamSaveGame.h>
//#include "GameSystems/Common/GameEventBus/GameEventBus.h"

static const FString AchievementsSlot = TEXT("Achievements");

/* 업적이 하는 일
업적 정의데이터, 유저 상태 데이터 가져오기
업적 유저상태에 따른 변경
업적 갱신을 업적 아이디 뒤 _ 토큰으로 확인
업적 ID 예시 : ID_ItemType_ItemID
			  ID_AchievementType
*/
void UAchievementsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	TArray<FAchievementDef> AchieveDefs;
	LoadAchievementDef(AchieveDefs);
							
	for (const auto& Def : AchieveDefs)
	{
		if (Def.Id.IsNone()) 
		{
			UE_LOG(LogTemp, Warning, TEXT("[Ach] Skip: empty Id (title=%s)"),
				*Def.Title.ToString());
			continue;
		}
		Definition.Add(Def.Id, Def);
		States.FindOrAdd(Def.Id);
		DefsByEventType.FindOrAdd(Def.EventType).Add(Def);
	}
	/*해당 로그는 EventType의 배열이 생성돼지않을 경우 nullptr을 반환해서 에러가 발생*/
	UE_LOG(LogTemp, Warning, TEXT("게임아이템 획득 업적 길이 : %d"), DefsByEventType.Find(EGameEventType::InventoryAdded)->Num());
	UE_LOG(LogTemp, Warning, TEXT("로그인 업적 길이 : %d"), DefsByEventType.Find(EGameEventType::Login)->Num());

	if (UGameInventory* Inv = UGameInventory::Get())
	{
		Inv->OnItemAdded.AddDynamic(this, &UAchievementsSubsystem::HandleItemAdded);
	}
}

void UAchievementsSubsystem::LoadAchievementDef(TArray<FAchievementDef>& OutDefs) const
{
	const FSoftObjectPath Path = AchievementData.ToSoftObjectPath();
	UE_LOG(LogTemp, Warning, TEXT("AchievementData Path = %s"), *Path.ToString());

	if (Path.IsValid())
	{
		if (UDreamAchievements* DA = AchievementData.LoadSynchronous())
		{
			OutDefs = DA->AchievementList;
		}
	}
}

void UAchievementsSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

const FAchievementDef* UAchievementsSubsystem::GetAchievementDefById(const FName& EventId) const
{
	return Definition.Find(EventId);
}

const FAchievementState* UAchievementsSubsystem::GetAchievementStateById(const FName& EventId) const
{
	return States.Find(EventId);
}

const TMap <FName, FAchievementDef> UAchievementsSubsystem::GetAllAchievementDef()
{
	return Definition;
}

const TMap<FName, FAchievementState> UAchievementsSubsystem::GetAllAchievementState()
{
	return States;
}



void UAchievementsSubsystem::HandleAchieveEvent(FName EventId)
{
	UE_LOG(LogTemp, Warning, TEXT("HandleAchieveEvent Call"));

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
void UAchievementsSubsystem::GetViewData(TArray<FAchievementViewData>& OutViewArr, TArray<FName>& OutIdsArr)
{
	FAchievementViewData ViewData;

	if (ViewsCash.Num() != 0 && DefIdsCash.Num() != 0) {
		OutViewArr = ViewsCash;
		OutIdsArr = DefIdsCash;
		return;
	}

	for (const auto& Data : Definition)
	{
		const FName& AchievementID = Data.Value.Id;
		const FAchievementState* AchievementState = GetAchievementStateById(AchievementID);

		ViewData.Title = Data.Value.Title;
		ViewData.Description = Data.Value.Description;
		ViewData.TargetValue = Data.Value.Target;
		if (AchievementState != nullptr)
		{
			ViewData.Progress = AchievementState->Progress;
			ViewData.bUnlocked = (AchievementState->UnlockedTime != FDateTime::MaxValue());
		}
		else
		{
			ViewData.Progress = 0;
			ViewData.bUnlocked = false;
		}
		OutViewArr.Add(ViewData);
		OutIdsArr.Add(AchievementID);
		IdsByView.Add({AchievementID,ViewData});
	}

	if (ViewsCash.Num() <= 0 && DefIdsCash.Num() <= 0)
	{
		ViewsCash = OutViewArr;
		DefIdsCash = OutIdsArr;	
	}
	return;
}

void UAchievementsSubsystem::GetViewDataById(FAchievementViewData& OutView,const FName& EventId)
{
	if (IdsByView.Num() == 0) return;

	FAchievementViewData ViewData;
	//view 업데이트 시 갱신 필요

	if (IdsByView.Find(EventId))
	{
		OutView = *IdsByView.Find(EventId);
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
	return true;
}

bool UAchievementsSubsystem::HandleProgressEvent(FName& EventId, const FAchievementDef& OutDef, FAchievementState& OutState)
{
	if (OutDef.Target >= OutState.Progress && OutState.UnlockedTime != FDateTime::MaxValue()) // 업적 클리어 전
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
	return true;
}

void UAchievementsSubsystem::HandleItemAdded(EItemCategory ItemCategory, int32 ItemID)
{
	TArray<FAchievementDef>* AchievementDef = DefsByEventType.Find(EGameEventType::InventoryAdded);
	if (!AchievementDef) return;

	EItemCategory ParseCat = EItemCategory::Other;
	int32 ItemId = -1;
	FName AchieveID;

	for (const auto& Achievement : *AchievementDef)
	{
		if (AchIdParse::ParseItemTypeAndId(Achievement.Id, ParseCat, ItemId))
		{
			if (ParseCat != ItemCategory || ItemId != ItemID)
			{
				UE_LOG(LogTemp, Warning, TEXT("업적이 일치하지 않음"));
				continue;
			}

			FAchievementState* State = States.Find(Achievement.Id);
			if (!State)
			{
				UE_LOG(LogTemp, Warning, TEXT("업적 검색 실패"));
				return;
			}
			State->Progress++;
			// 진행 상황 저장 필요.
			AchieveID = Achievement.Id;
			UE_LOG(LogTemp,Warning,TEXT("업적 진행도 증가!"));
			if (FAchievementViewData* Data = IdsByView.Find(AchieveID))
			{
				Data->Progress++;
			}
			OnAchievementUpdated.Broadcast(AchieveID);
			
			return;
		}
	}
	/*TESTCODE*/
	/*int32 ItemCode = 0;
	AchIdParse::EItemCategory ItemType;
	if (AchIdParse::ParseItemTypeAndId(AchievementDef.Id, ItemType, ItemCode))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemType : %s"), AchIdParse::ToString(ItemType));
		UE_LOG(LogTemp, Warning, TEXT("ItemCode : %d"), ItemCode);
	}*/
	/*TESTCODE*/
	return;
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
