// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TimeSubsystemManager.generated.h"

class UWorldTimeController;

UCLASS()
class PROJECTDREAM_API UTimeSubsystemManager : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override
	{
		// 프로파일러에서 표시되는 카테고리 태그
		RETURN_QUICK_DECLARE_CYCLE_STAT(UTimeSubsystemManager, STATGROUP_Tickables);
	}

	void StartWorldTime(UWorld* World);

	UWorldTimeController* GetGameTimer() { return GameTimer.Get(); }
private:
	UPROPERTY()
	TObjectPtr<UWorldTimeController> GameTimer;
};
