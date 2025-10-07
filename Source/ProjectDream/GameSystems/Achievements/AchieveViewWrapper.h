// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AchievementUIViewData.h"
#include "AchieveViewWrapper.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTDREAM_API UAchieveViewWrapper : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FAchievementViewData Data;	

	UPROPERTY()
	FName AchieveId;
};
