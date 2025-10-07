// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AchievementListWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTDREAM_API UAchievementListWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;
private:
	UFUNCTION()
	void OnOffUI();
	
	void RefreshAll();

	UFUNCTION()
	void UpdateAchieveEntry(FName EventId);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UListView> AchieveList;
	
	UPROPERTY()
	TObjectPtr<class UAchievementsManager> AchieveManager = nullptr;

	UPROPERTY()
	TMap<FName, TObjectPtr<class UAchieveViewWrapper>> IdToItem;
};
