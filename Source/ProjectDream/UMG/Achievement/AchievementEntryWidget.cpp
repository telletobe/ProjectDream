// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/Achievement/AchievementEntryWidget.h"
#include "../GameSystems/Achievements/AchieveViewWrapper.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void UAchievementEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (!ListItemObject)  return;

	if (const UAchieveViewWrapper* Row = Cast<UAchieveViewWrapper>(ListItemObject))
	{
		Item = Row->Data;
	}
	SyncFromItem();
}

void UAchievementEntryWidget::SyncFromItem()
{
	UE_LOG(LogTemp,Warning,TEXT("Call SyncFromItem"));

	if (TextTitle)
	{
		TextTitle->SetText(Item.Title);
		UE_LOG(LogTemp,Warning,TEXT("Title : %s "), *Item.Title.ToString());
	}

	if (DescText)
	{
		DescText->SetText(Item.Description);
	}

	if (ProgressText)
	{
		ProgressText->SetText(Item.GetProgressText());
		UE_LOG(LogTemp, Warning, TEXT("Title : %s "), *Item.GetProgressText().ToString());
	}

	if (StatusText)
	{
		StatusText->SetText(Item.GetStatusText());
		UE_LOG(LogTemp, Warning, TEXT("Title : %s "), *Item.GetStatusText().ToString());
	}

	if (AchieveClear)
	{
		if (Item.bUnlocked)
		{
			AchieveClear->SetPercent(1.0f);
		}
	}
}

void UAchievementEntryWidget::SetViewData(FAchievementViewData ViewData)
{
	Item = ViewData;
	SyncFromItem();
}


void UAchievementEntryWidget::UpdateProgress()
{
	if (AchieveClear)
	{
		//AchieveClear->SetPercent();
	}
}