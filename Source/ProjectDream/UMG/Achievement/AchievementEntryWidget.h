// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../GameSystems/Achievements/AchievementUIViewData.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "AchievementEntryWidget.generated.h"

/**
 * 
 */

class UTextBlock;
class UProgressBar;

UCLASS()
class PROJECTDREAM_API UAchievementEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	void UpdateProgress();
	void SyncFromItem();
	void SetViewData(FAchievementViewData ViewData);
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TextTitle;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DescText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ProgressText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StatusText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> AchieveClear;

	UPROPERTY() FAchievementViewData Item;
};
