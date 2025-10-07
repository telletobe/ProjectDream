// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectDreamGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class AProjectDreamGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	AProjectDreamGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
protected:

private:
	class UTimeSubsystemManager* WorldTimerManager;
	class UWorldTimeController* WorldTime;
};



