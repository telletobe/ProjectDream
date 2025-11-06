// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../GameSystems/Achievements/AchievementViewData.h"
#include "AchievementListWidget.generated.h"

class UAchieveViewWrapper;
UCLASS()
class PROJECTDREAM_API UAchievementListWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
private:
	UFUNCTION()	void OnOffUI();
	void RefreshAll();
	bool BulidItemsAndIdMap(const TArray<FAchievementViewData>& Views, const TArray<FName>& AchieveIds, TArray<UObject*>& OutItems);
	UFUNCTION()	void UpdateAchieveEntry(FName EventId);
	UFUNCTION() void HandleItemClicked(UObject* Item);
	void SyncMapToId(FName& EventId);
private:
	UPROPERTY(meta = (BindWidget)) TObjectPtr<class UListView> AchieveList;
	UPROPERTY() TMap<FName, UAchieveViewWrapper*> IdToItem;
	bool bInitailize = false;
};
