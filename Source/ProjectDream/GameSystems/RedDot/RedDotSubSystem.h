// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RedDotSubSystem.generated.h"

UENUM()
enum class ERedDotType : int8
{
	Achievement,
	Inventory,
	MAX
};

UCLASS()
class PROJECTDREAM_API URedDotSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	bool CheckRedDotCount();
	void OnRedDot();
	void OffRedDot(const FName& EventId);
private:
	inline int32 ToIndex(ERedDotType RedDotType);
private:
	TArray<int32> RedDotCountByType;
};
