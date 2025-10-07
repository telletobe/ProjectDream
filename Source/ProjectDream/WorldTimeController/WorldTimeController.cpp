// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldTimeController/WorldTimeController.h"
#include "WorldTimeController.h"
#include "Engine/DirectionalLight.h"
#include "EngineUtils.h"

UWorldTimeController* UWorldTimeController::Instance = nullptr;

UWorldTimeController::UWorldTimeController()
{

}

UWorldTimeController* UWorldTimeController::GetInstance()
{
	if (!Instance)
	{
		Instance = NewObject<UWorldTimeController>();
		Instance->AddToRoot();
	}
	return Instance;
}


void UWorldTimeController::Init(UWorld* CurrentWorld)
{
	if (!CurrentWorld) return;

	CurrentTimeOfDay = 6.0f;

	for (TActorIterator<ADirectionalLight> It(CurrentWorld); It; ++It)
	{
		SunLight = TWeakObjectPtr<ADirectionalLight>(*It);
		break;
	}

	if (!SunLight.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::MakeRandomColor(), TEXT("WorldTimeController : DirectionalLight not Found in World!"));
		UE_LOG(LogTemp,Warning,TEXT("WorldTimeController : DirectionalLight not Found in World!"));
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::MakeRandomColor(), TEXT("WorldTimeController : Init Succed"));
}

void UWorldTimeController::Tick(float DeltaTime)
{

	if (DayLengthInSeconds <= 0.0f) return;

	float TimeScale = 24.0f / DayLengthInSeconds;
	CurrentTimeOfDay += DeltaTime * TimeScale;
	if (CurrentTimeOfDay >= 24.0f) CurrentTimeOfDay -= 24.0f;
	
	UpdateSunRotation();
	
}

void UWorldTimeController::UpdateSunRotation()
{
	if (!SunLight.IsValid()) return;
	float DayTime = CurrentTimeOfDay / 24.0f; 
	float PitchDegrees = FMath::Fmod((DayTime - 0.25f) * 360.0f + 180.0f, 360.0f);
	float YawDegrees = DayTime * 360.0f;

	FRotator NewRot = FRotator(PitchDegrees, YawDegrees, 0.0f);
	SunLight->SetActorRotation(NewRot);

}


