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
		UE_LOG(LogTemp,Warning,TEXT("Entry ItemProgress : %d"),Item.Progress);
	}
	SyncFromItem();
}

void UAchievementEntryWidget::SyncFromItem()
{
	UE_LOG(LogTemp,Warning,TEXT("Call SyncFromItem"));

	if (TextTitle)
	{
		TextTitle->SetText(Item.Title);
	}

	if (DescText)
	{
		DescText->SetText(Item.Description);
	}

	if (ProgressText)
	{
		ProgressText->SetText(Item.GetProgressText());
	}

	if (StatusText)
	{
		StatusText->SetText(Item.GetStatusText());
	}

	if (AchieveClear)
	{
		if (Item.bUnlocked)
		{
			AchieveClear->SetPercent(1.0f);
		}
	}
}

void UAchievementEntryWidget::SetViewItem(FAchievementViewData* ViewItem)
{
	Item = *ViewItem;
}


void UAchievementEntryWidget::UpdateProgress()
{
	if (AchieveClear)
	{
		//AchieveClear->SetPercent();
	}
}