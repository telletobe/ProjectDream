// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldTimeController/TimeSubsystemManager.h"
#include "WorldTimeController.h"

void UTimeSubsystemManager::Initialize(FSubsystemCollectionBase& Collection)
{
	UGameInstanceSubsystem::Initialize(Collection);
	GameTimer = UWorldTimeController::GetInstance();
	UE_LOG(LogTemp,Warning,TEXT("Call TimeSubSystemManager!"));

}

void UTimeSubsystemManager::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("ShtuDown TimeSubSystemManager!"));
	UGameInstanceSubsystem::Deinitialize();
}


void UTimeSubsystemManager::Tick(float DeltaTime)
{
}

void UTimeSubsystemManager::StartWorldTime(UWorld* World)
{
	
	if (!GameTimer || !World) return;

	GameTimer->Init(World);
	
}
