// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldTimeController/TimeSubsystem.h"
#include "WorldTimeController.h"

void UTimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UGameInstanceSubsystem::Initialize(Collection);
	GameTimer = UWorldTimeController::GetInstance();
}

void UTimeSubsystem::Deinitialize()
{
	UGameInstanceSubsystem::Deinitialize();
}


void UTimeSubsystem::Tick(float DeltaTime)
{
}

void UTimeSubsystem::StartWorldTime(UWorld* World)
{
	
	if (!GameTimer || !World) return;

	GameTimer->Init(World);
	
}
