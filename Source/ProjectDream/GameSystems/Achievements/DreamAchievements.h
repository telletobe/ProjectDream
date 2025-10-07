// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DreamAchievements.generated.h"

UENUM(BlueprintType)
enum class EAchievementType : uint8
{
	Instant,	// 즉시형
	Progressive // 누적형
};

UENUM(BlueprintType)
enum class EGameEventType : uint8
{
	GameStarted,
	ItemGet
};

// 업적 정의
USTRUCT(BlueprintType)
struct FAchievementDef
{
	GENERATED_BODY()

	// 업적을 코드/데이터상에서 구분
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FName Id = NAME_None; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EAchievementType AchieveType = EAchievementType::Instant;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) EGameEventType EventType = EGameEventType::GameStarted;

	//즉시형이면 값 무시 혹은 0 유지
	// 업적이 누적용일 경우. ex) 킬 카운트
	UPROPERTY(EditAnywhere, BlueprintReadOnly) int32 Target = 0; 

	// UI용 업적 제목
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FText Title; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FText Description;
};

// 저장용 플레이어의 업적 상태
USTRUCT(BlueprintType)
struct FAchievementState
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, EditAnywhere,BlueprintReadOnly)
	FName Id = NAME_None;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly)
	int32 Progress = 0;
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly)
	bool bUnlocked = false;
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadOnly)
	FDateTime UnlockedAt;
};

UCLASS()
class PROJECTDREAM_API UDreamAchievements : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FAchievementDef> AchievementList;
};
