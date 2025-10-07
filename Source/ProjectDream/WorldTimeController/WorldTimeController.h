// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WorldTimeController.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, EditInlineNew)
class PROJECTDREAM_API UWorldTimeController : public UObject
{
	GENERATED_BODY()
	
private:
	UWorldTimeController();
public :
	static UWorldTimeController* GetInstance();

	void Init(UWorld* CurrentWorld);
	void Tick(float DeltaTime);

protected:

private:
	static UWorldTimeController* Instance;
	
	float CurrentTimeOfDay = 8.0f;
	float DayLengthInSeconds = 120.0f; // 기준 : 초(게임 내 하루)
	

	void UpdateSunRotation();

	UPROPERTY(EditAnywhere, Category = "Actors");
	TWeakObjectPtr<class ADirectionalLight> SunLight;

	UPROPERTY(EditAnywhere, Category = "Curves | Sun")
	UCurveFloat* SunPitchCureve;

};
