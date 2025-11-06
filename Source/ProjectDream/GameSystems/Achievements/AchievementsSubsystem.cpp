// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/Achievements/AchievementsSubsystem.h"
#include "DreamAchievements.h"
#include "AchievementViewData.h"
#include <Kismet/GameplayStatics.h>
#include "../Save/JsonSaveGame.h"
#include <GameSystems/Save/DreamSaveGame.h>
//#include "GameSystems/Common/GameEventBus/GameEventBus.h"

static const FString AchievementsSlot = TEXT("Achievements");
static const FString AchievementsSeenSlot = TEXT("AchievementsSeenState");

/* 업적이 하는 일
업적 정의데이터, 유저 상태 데이터 가져오기
업적 유저상태에 따른 변경
업적 갱신을 업적 아이디 뒤 _ 토큰으로 확인
업적 ID 예시 : EClearRule_ItemType_ItemID
			   임의이름_ClearRuleType
*/
void UAchievementsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	TArray<FAchievementDef> AchieveDefs;
	TArray<FAchievementState> LoadState;
	LoadAchievementDef(AchieveDefs);

	// 업적 로딩
	SaveJson::LoadArrayFromFile(AchievementsSlot, LoadState);

							
	for (const auto& Def : AchieveDefs)
	{
		if (Def.Id.IsNone()) 
		{
			continue;
		}
		Definition.Add(Def.Id, Def);
		FAchievementState& State = States.FindOrAdd(Def.Id);
		if (State.Id.IsNone())          // 로드본에 Id가 비어있을 수도 있음
		{
			State.Id = Def.Id;          // 반드시 보정		
		}
		DefsByEventType.FindOrAdd(Def.EventType).Add(Def);
	}

	for (auto& State : LoadState)
	{
		States.FindOrAdd(State.Id) = State;
	}

	if (UGameInventory* Inv = UGameInventory::Get())
	{
		Inv->OnItemAdded.AddDynamic(this, &UAchievementsSubsystem::DispatchAchivementEvent);
	}
	HandleLogin();
}

void UAchievementsSubsystem::LoadAchievementDef(TArray<FAchievementDef>& OutDefs) const
{
	const FSoftObjectPath Path = AchievementData.ToSoftObjectPath();

	if (Path.IsValid())
	{
		if (UDreamAchievements* DA = AchievementData.LoadSynchronous())
		{
			OutDefs = DA->AchievementList;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Load Failed"));
		}
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("Load Path Invalid"));
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
	return const_cast<UAchievementsSubsystem*>(this)->GetAchievementStateById(EventId);
}

FAchievementState* UAchievementsSubsystem::GetAchievementStateById(const FName& EventId)
{
	return States.Find(EventId);
}

const TMap <FName, FAchievementDef>& UAchievementsSubsystem::GetAllAchievementDef() const
{
	return Definition;
}

const TMap<FName, FAchievementState>& UAchievementsSubsystem::GetAllAchievementState() const
{
	return States;
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
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("World가 존재하지않음."));
	}
}

void UAchievementsSubsystem::GetAllViewData(TArray<FAchievementViewData>& OutViewArr, TArray<FName>& OutIdsArr)
{
	if (IdsByView.Num() != 0)
	{
		for (const auto& Elem : IdsByView)
		{
			OutIdsArr.Add(Elem.Key);
			OutViewArr.Add(Elem.Value);
		}
		return;
	}
	// View 새로 생성
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
			ViewData.UnlockedTime = AchievementState->UnlockedTime;
			ViewData.bRewardClaimed = AchievementState->bRewardClaimed;
		}
		else
		{
			ViewData.Progress = 0;
			ViewData.UnlockedTime = FDateTime::MinValue();
			ViewData.bRewardClaimed = false;
		}
		OutViewArr.Add(ViewData);
		OutIdsArr.Add(AchievementID);
		IdsByView.Add({AchievementID,ViewData});
	}
	return;
}

void UAchievementsSubsystem::GetViewDataById(FAchievementViewData& OutView,const FName& EventId)
{
	if (IdsByView.Num() == 0) return;
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

bool UAchievementsSubsystem::HandleAchivementEvent(FName& EventId)
{	
	AchievementIDParse::FParseResult Result;
	AchievementIDParse::ParseID(EventId,Result);

	if (!Result.bValid) return false;
	if (Result.bHasClearRule)
	{
		if (Result.Rule == EClearRule::Login)
		{
			HandleLogin();
		}
		else if (Result.Rule == EClearRule::InventoryAdded)
		{
			HandleItemAdded(Result.ItemCat, Result.ItemID);
		}
	}
	else if (Result.bHasItemData)
	{
		HandleItemAdded(Result.ItemCat, Result.ItemID);
	}
	return false;
}

void UAchievementsSubsystem::UpdateView(FAchievementViewData& OutViewData,FAchievementState& OutState)
{
	OutViewData.Progress = OutState.Progress;
	OutViewData.bRewardClaimed = OutState.bRewardClaimed;
	OutViewData.UnlockedTime = OutState.UnlockedTime;
}

void UAchievementsSubsystem::UpdateState(FAchievementState& OutStateData, const FAchievementDef& OutDef)
{
	if (OutDef.AchieveType == EAchievementType::Instant)
	{
		OutStateData.Progress++;
		OutStateData.UnlockedTime = FDateTime::UtcNow();
	}
	else
	{
		OutStateData.Progress++;
		if (OutDef.Target <= OutStateData.Progress)
		{
			OutStateData.UnlockedTime = FDateTime::UtcNow();
		}
	}
	RequestSave(States);
}

void UAchievementsSubsystem::UpdateProgress(const FName& EventId)
{
	FAchievementState* State = States.Find(EventId);
	FAchievementViewData* Data = IdsByView.Find(EventId);

	if (!State) return;
	if (State->UnlockedTime != FDateTime::MinValue() && State->bRewardClaimed == true)
	{
		return;
	}
	else if (State->UnlockedTime != FDateTime::MinValue() && State->bRewardClaimed == false)
	{
		return;
	}

	UpdateState(*State, *Definition.Find(EventId));
	if (!Data) return;
	UpdateView(*Data, *State);
	OnAchievementUpdated.Broadcast(EventId);
}

void UAchievementsSubsystem::HandleLogin()
{
	TArray<FAchievementDef>* AchievementDef = DefsByEventType.Find(EClearRule::Login);
	if (!AchievementDef) return;

	for (const auto& Achievement : *AchievementDef)
	{
		UpdateProgress(Achievement.Id);
	}
	return;
}

void UAchievementsSubsystem::HandleItemAdded(EItemCategory ItemCategory, int32 ItemID)
{	
	TArray<FAchievementDef>* AchievementDef = DefsByEventType.Find(EClearRule::InventoryAdded);

	if (!AchievementDef) return;
	if (ItemCategory == EItemCategory::None) return;

	for (const auto& Achievement : *AchievementDef)
	{
		AchievementIDParse::FParseResult Result;
		if (AchievementIDParse::ParseID(Achievement.Id, Result))
		{
			if (!Result.bValid || !Result.bHasItemData) return;
			if (ItemCategory != Result.ItemCat || ItemID != Result.ItemID)  continue;
			UpdateProgress(Achievement.Id);
		}
	}
}

void UAchievementsSubsystem::DispatchAchivementEvent(EItemCategory ItemCategory, int32 ItemID)
{
	FName ResultID(*FString::Printf(TEXT("ItemAdded_%s_%d"), AchievementIDParse::ToString(ItemCategory), ItemID));
	HandleAchivementEvent(ResultID);
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

bool AchievementIDParse::ParseClearRuleToken(const FString& S, EClearRule& Out)
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
		if (ParseAchievementType(Tokens, OutFParseReulst)) return true;
	}
	else if (Tokens.Num() == 3)
	{
		// 업적ID_EItemCategory_ItemID
		if (ParseItemTypeAndId(Tokens, OutFParseReulst)) return true;
	}
	return false;
}

bool AchievementIDParse::ParseItemTypeAndId(const TArray<FString>& OutTokens, FParseResult& OutFParseReulst)
{
	int32 Num = 0;
	if (LexTryParseString<int32>(Num, *OutTokens.Last()))
	{
		OutFParseReulst.ItemID = Num;
		OutFParseReulst.bHasItemData = true;

		EItemCategory ItemCategory;
		if (StringToItemType(OutTokens[OutTokens.Num() - 2], ItemCategory))
		{
			OutFParseReulst.ItemCat = ItemCategory;
			OutFParseReulst.bValid = true;
		}
		return true;
	}
	return false;
}

bool AchievementIDParse::ParseAchievementType(const TArray<FString>& OutTokens, FParseResult& OutFParseReulst)
{	
	EClearRule ClearRuleType;
	if (ParseClearRuleToken(OutTokens[OutTokens.Num() - 1], ClearRuleType))
	{
		OutFParseReulst.Rule = ClearRuleType;
		OutFParseReulst.bHasClearRule = true;
		OutFParseReulst.bValid = true;
	}
	return false;
}
