// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AchievementUIViewData.generated.h"

USTRUCT(BlueprintType)
struct FAchievementViewData
{
	GENERATED_BODY()

//	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Title;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 TargetValue = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Progress = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool  bUnlocked = false;

	// 계산 필드(편의)
	float GetPercent() const
	{
		if (TargetValue > 0) { return FMath::Clamp((float)Progress / (float)TargetValue, 0.f, 1.f); }
		return bUnlocked ? 1.f : 0.f;
	}

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
		return bUnlocked ? FText::FromString(TEXT("완료")) : FText::FromString(TEXT("진행중"));
	}
};