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

	if (UGameInventory* Inv = UGameInventory::Get())
	{
		Inv->OnItemAdded.AddDynamic(this, &UAchievementsSubsystem::HandleItemAdded);
	}
	HandleLogin();
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
	//view 업데이트 시 갱신 필요
	if (ViewsCash.Num() != 0 && DefIdsCash.Num() != 0) {
		OutViewArr = ViewsCash;
		OutIdsArr = DefIdsCash;
		return;
	}

	for (const auto& Data : Definition)
	{
		FAchievementViewData ViewData;
		const FName& AchievementID = Data.Value.Id;
		const FAchievementState* AchievementState = GetAchievementStateById(AchievementID);

		ViewData.Title = Data.Value.Title;
		ViewData.Description = Data.Value.Description;
		ViewData.TargetValue = Data.Value.Target;
		if (AchievementState != nullptr)
		{
			ViewData.Progress = AchievementState->Progress;
			ViewData.bUnlocked = (AchievementState->UnlockedTime != FDateTime::MaxValue()); // false
		}
		else
		{
			ViewData.Progress = 0;
			ViewData.bUnlocked = false;
		}
		OutViewArr.Add(ViewData);
		OutIdsArr.Add(AchievementID);
		IdsByView.Add({AchievementID,ViewData});
		UE_LOG(LogTemp,Warning,TEXT("잠금해제 bUnlocked : %d"), ViewData.bUnlocked);
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

bool UAchievementsSubsystem::HandleAchivementEvent(FName& EventId)
{
	//// 현재 미사용 코드
	AchievementIDParse::FParseResult Result;
	AchievementIDParse::ParseID(EventId,Result);

	if (!Result.bValid) return false;
	if (Result.bHasClearRule)
	{
		HandleLogin();
	}
	else if (Result.bHasItemData)
	{
		HandleItemAdded(Result.ItemCat,Result.ItemID);
	}

	return false;
}

void UAchievementsSubsystem::UpdateProgress(const FAchievementDef& OutDef,const FName& EventId)
{
	FAchievementState* State = States.Find(EventId);

	if (!State) return;

	if (OutDef.AchieveType == EAchievementType::Instant)
	{
		State->Progress++;
		State->UnlockedTime = FDateTime::UtcNow();
		return;
	}

	State->Progress++;
	if (OutDef.Target == State->Progress)
	{
		State->UnlockedTime = FDateTime::UtcNow();

	}
	// 진행 상황 저장 필요
	if (FAchievementViewData* Data = IdsByView.Find(OutDef.Id))
	{
		Data->Progress++;
		if (Data->TargetValue <= Data->Progress)
		{
			Data->bUnlocked = true;
		}
	}
}

void UAchievementsSubsystem::HandleLogin()
{
	TArray<FAchievementDef>* AchievementDef = DefsByEventType.Find(EClearRule::Login);
	if (!AchievementDef) return;

	for (const auto& Achievement : *AchievementDef)
	{
		UpdateProgress(Achievement, Achievement.Id);
		OnAchievementUpdated.Broadcast(Achievement.Id);
		return;	
	}
}

void UAchievementsSubsystem::HandleItemAdded(EItemCategory ItemCategory, int32 ItemID)
{
	TArray<FAchievementDef>* AchievementDef = DefsByEventType.Find(EClearRule::InventoryAdded);
	if (!AchievementDef) return;

	for (const auto& Achievement : *AchievementDef)
	{
		AchievementIDParse::FParseResult Result;
		if (AchievementIDParse::ParseID(Achievement.Id, Result))
		{
			if (!Result.bValid || !Result.bHasItemData) return;
			if (ItemCategory != Result.ItemCat || ItemID != Result.ItemID)  continue;
			UpdateProgress(Achievement, Achievement.Id);
			OnAchievementUpdated.Broadcast(Achievement.Id);
		}
		/*if (AchievementIDParse::ParseItemTypeAndId(Achievement.Id, ParseCat, ItemId))
		{
			if (ParseCat != ItemCategory || ItemId != ItemID)
			{
				continue;
			}
			UpdateProgress(Achievement, Achievement.Id);
			UE_LOG(LogTemp,Warning,TEXT(""));
			OnAchievementUpdated.Broadcast(Achievement.Id);
			return;
		}*/
	}
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

bool AchievementIDParse::StringToItemType(const FString& S, EItemCategory& Out)
{
	if (S.Equals(TEXT("Equipment"), ESearchCase::IgnoreCase))
	{
		Out = EItemCategory::Equipment;
		return true;
	}
	if (S.Equals(TEXT("Consumable"), ESearchCase::IgnoreCase))
	{
		Out = EItemCategory::Consumable;
		return true;
	}
	if (S.Equals(TEXT("Other"), ESearchCase::IgnoreCase))
	{
		Out = EItemCategory::Other;
		return true;
	}
	return false;
}

bool AchievementIDParse::StringToAchievementType(const FString& S, EClearRule& Out)
{
	if (S.Equals(TEXT("Login")))
	{
		Out = EClearRule::Login;
		return true;
	}
	if (S.Equals(TEXT("InventoryAdded")))
	{
		Out = EClearRule::InventoryAdded;
		return true;
	}
	return false;
}

// 디버깅 용
const TCHAR* AchievementIDParse::ToString(EItemCategory Cat)
{
	switch (Cat)
	{
	case EItemCategory::Equipment:  return TEXT("Equipment");
	case EItemCategory::Consumable: return TEXT("Consumable");
	case EItemCategory::Other:      return TEXT("Other");
	default:                        return TEXT("Unknown");
	}
}

bool AchievementIDParse::ParseID(const FName& AchievementID, FParseResult& OutFParseReulst)
{
	const FString S = AchievementID.ToString();
	TArray<FString> Tokens;
	S.ParseIntoArray(Tokens, TEXT("_"), true);

	if (Tokens.Num() == 1)
	{
		// 현재 업적 해당사항 없음.
		return false;
	}
	else if (Tokens.Num() == 2)
	{
		// 업적ID_EClearRule
		ParseAchievementType(AchievementID, OutFParseReulst);
		return true;
	}
	else if (Tokens.Num() == 3)
	{
		// 업적ID_EItemCategory_ItemID
		ParseItemTypeAndId(AchievementID, OutFParseReulst);
		return true;
	}
	return false;
}


bool AchievementIDParse::ParseItemTypeAndId(const FName AchievementId, FParseResult& OutFParseReulst)
{
	const FString S = AchievementId.ToString();
	TArray<FString> Tokens;
	S.ParseIntoArray(Tokens, TEXT("_"), true);
	if (Tokens.Num() < 3) return false;

	int32 Num = 0;
	if (LexTryParseString<int32>(Num, *Tokens.Last()))
	{
		OutFParseReulst.ItemID = Num;
		OutFParseReulst.bHasItemData = true;

		EItemCategory ItemCategory;
		if (StringToItemType(Tokens[Tokens.Num() - 2], ItemCategory))
		{
			OutFParseReulst.ItemCat = ItemCategory;
			OutFParseReulst.bValid = true;
		}

		return true;
	}
	return false;
}

bool AchievementIDParse::ParseAchievementType(const FName AchievementId, FParseResult& OutFParseReulst)
{	
	const FString S = AchievementId.ToString();
	TArray<FString> Tokens;
	S.ParseIntoArray(Tokens, TEXT("_"), true);
	if (Tokens.Num() < 2)
	{
		return false;
	}

	EClearRule ClearRuleType;
	if (StringToAchievementType(Tokens[Tokens.Num() - 1], ClearRuleType))
	{
		OutFParseReulst.Rule = ClearRuleType;
		OutFParseReulst.bHasClearRule = true;
		OutFParseReulst.bValid = true;
	}
	return true;
	
}
