// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ProjectDreamHUD.generated.h"

/**
 * 
 */

class UUserInventory;
class UAchievementListWidget;

UCLASS()
class PROJECTDREAM_API AProjectDreamHUD : public AHUD
{
	GENERATED_BODY()
	
public :
	AProjectDreamHUD();
	void BeginPlay();

private:

	UPROPERTY(EditAnywhere, Category ="UMG")
	TSubclassOf<UUserInventory> InventoryWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserInventory> InventoryWidget;

	UPROPERTY(EditAnywhere, Category = "UMG")
	TSubclassOf< UAchievementListWidget> AchievementWidgetClass;

	UPROPERTY()
	TObjectPtr<UAchievementListWidget> AchievementWidget;


};
