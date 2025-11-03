// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RedDotSubSystem.generated.h"

USTRUCT(BlueprintType)
struct FAchievementSeen
{
	GENERATED_BODY()
	UPROPERTY(SaveGame) int32 LastSeenRevision = 0;
};

UCLASS()
class PROJECTDREAM_API URedDotSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	void MarkSeen(const FName& EventId);
private:

};
