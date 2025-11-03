// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AchievementViewData.generated.h"

USTRUCT(BlueprintType)
struct FAchievementViewData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Title;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 TargetValue = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Progress = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FDateTime  UnlockedTime = FDateTime::MinValue();
	UPROPERTY(Transient) bool bShowRedDot = false;
	bool operator== (const FAchievementViewData& Rhs);
	FAchievementViewData() = default;

	FText GetProgressText() const
	{
		if (TargetValue > 0)
		{
			return FText::FromString(FString::Printf(TEXT("%d / %d"), Progress, TargetValue));
		}
		return FText::FromString(TEXT("정보없음"));
	}

	FText GetStatusText() const
	{
		return UnlockedTime != FDateTime::MinValue() ? FText::FromString(TEXT("완료")) : FText::FromString(TEXT("진행중"));
	}
};