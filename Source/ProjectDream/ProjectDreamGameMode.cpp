// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectDreamGameMode.h"
#include "WorldTimeController/TimeSubsystemManager.h"
#include "WorldTimeController/WorldTimeController.h"
#include "Subsystems/GameInstanceSubsystem.h"  

void AProjectDreamGameMode::BeginPlay()
{
	Super::BeginPlay();
	UGameInstance* GI = GetGameInstance();

	if (GI)
	{
		WorldTimerManager = GI->GetSubsystem<UTimeSubsystemManager>();
		if (!WorldTimerManager) return;
		WorldTime = UWorldTimeController::GetInstance();
		if (!WorldTime) return;
		WorldTime->Init(GetWorld());
	}

}

void AProjectDreamGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!WorldTime) return;
	WorldTime->Tick(DeltaTime);

}

AProjectDreamGameMode::AProjectDreamGameMode()
{
	// stub
	PrimaryActorTick.bCanEverTick = true;
}
