// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AchievementListWidget.generated.h"

class UAchievementView;
UCLASS()
class PROJECTDREAM_API UAchievementListWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
private:
	UFUNCTION()	void OnOffUI();
	void RefreshAll();
	UFUNCTION()	void UpdateAchieveEntry(FName EventId);
	UFUNCTION() void HandleItemClicked(UObject* Item);
private:
	UPROPERTY(meta = (BindWidget)) TObjectPtr<class UListView> AchieveList;
	UPROPERTY() TMap<FName, UAchievementView*> IdToItem;
	bool bInitailize = false;
};
