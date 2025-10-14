// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldTimeController/TimeSubsystem.h"
#include "WorldTimeController.h"

void UTimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UGameInstanceSubsystem::Initialize(Collection);
	GameTimer = UWorldTimeController::GetInstance();
	UE_LOG(LogTemp,Warning,TEXT("Call TimeSubSystemManager!"));

}

void UTimeSubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("ShtuDown TimeSubSystemManager!"));
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
