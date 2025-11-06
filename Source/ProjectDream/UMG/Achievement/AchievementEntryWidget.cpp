// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/Achievement/AchievementEntryWidget.h"
#include "../GameSystems/Achievements/AchieveViewWrapper.h"
#include "../GameSystems/Achievements/AchievementsSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"

void UAchievementEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (!ListItemObject)  return;

	if (UAchieveViewWrapper* Row = Cast<UAchieveViewWrapper>(ListItemObject))
	{
		if (!Row)
		{
			Item = nullptr;
			return;
		}
		Item = &Row->Data;
	}
	SyncFromItem();
}

void UAchievementEntryWidget::SyncFromItem()
{
	if (!Item) return;

	if (TextTitle) TextTitle->SetText(Item->Title);
	if (DescText) DescText->SetText(Item->Description);
	if (ProgressText) ProgressText->SetText(Item->GetProgressText());
	if (StatusText) StatusText->SetText(Item->GetStatusText());

	if (AchieveClear)
	{
		if (Item->UnlockedTime != FDateTime::MinValue())
		{
			AchieveClear->SetPercent(1.0f);
		}
		else if (Item->TargetValue > 0)
		{
			float Percent = (float)Item->Progress / (float)Item->TargetValue;
			AchieveClear->SetPercent(Percent);
		}
	}

	if (Item->UnlockedTime != FDateTime::MinValue() && Item->bRewardClaimed == false )
	{
		OnRedDot();
	}
}

void UAchievementEntryWidget::SetViewItem(FAchievementViewData* ViewItem)
{
	Item = ViewItem;
}

void UAchievementEntryWidget::OffRedDot()
{
	if (!Item) return;
	Item->bRewardClaimed = true;
	if (RedDot)
	{
		RedDot->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UAchievementEntryWidget::OnRedDot()
{
	if (RedDot)
	{
		RedDot->SetVisibility(ESlateVisibility::Visible);
	}
}

bool UAchievementEntryWidget::HasRedDot()
{
	if (RedDot)
	{
		ESlateVisibility RedDotState = RedDot->GetVisibility();
		switch (RedDotState)
		{
		case ESlateVisibility::Visible:
			return true;
		case ESlateVisibility::Hidden:
			return false;
		default:
			return false;
		}
	}
	return false;
}
