// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "../GameSystems/Achievements/DreamAchievements.h"
#include "GameEventBus.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameEvent,FName , EventId);


/**
 * 
 */
UCLASS()
class PROJECTDREAM_API UGameEventBus : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY()
	FOnGameEvent OnGameEvent;

	void Publish(const FName& EventId) { OnGameEvent.Broadcast(EventId); }
};
