// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AchievementView.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTDREAM_API UAchievementView : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY() FName AchievementID = NAME_None;
};
