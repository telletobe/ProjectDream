// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/Achievement/AchievementEntryWidget.h"
#include "../GameSystems/Achievements/AchievementView.h"
#include "../GameSystems/Achievements/AchievementsSubsystem.h"
#include "../GameSystems/Achievements/DreamAchievements.h"
#include "../GameSystems/RedDot/RedDotSubSystem.h"
#include "UMG/RedDot/RedDotWidget.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"

void UAchievementEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	if (!ListItemObject)  return;

	SyncFromItem(ListItemObject);
}

void UAchievementEntryWidget::NativeOnEntryReleased()
{
	if (TextTitle)      TextTitle->SetText(FText::GetEmpty());
	if (DescText)       DescText->SetText(FText::GetEmpty());
	if (ProgressText)   ProgressText->SetText(FText::GetEmpty());
	if (StatusText)     StatusText->SetText(FText::GetEmpty());

	if (AchieveClear)   AchieveClear->SetPercent(0.f);

	if (WBP_RedDotWidget) WBP_RedDotWidget->OffRedDot();
}

void UAchievementEntryWidget::SyncFromItem(UObject* ListItemObject)
{
	if (!ListItemObject) return;

	UAchievementView* Item = Cast< UAchievementView>(ListItemObject);
	if (!Item) return;

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAchievementsSubsystem* AchieveSubSys = GI->GetSubsystem<UAchievementsSubsystem>())
		{
			const FAchievementState* State = AchieveSubSys->GetAchievementStateById(Item->AchievementID);
			const FAchievementDef* Def = AchieveSubSys->GetAchievementDefById(Item->AchievementID);

			if (!State || !Def) return;

			if (TextTitle) TextTitle->SetText(Def->Title);
			if (DescText) DescText->SetText(Def->Description);
			if (ProgressText) ProgressText->SetText(GetProgressText(Def,State));
			if (StatusText) StatusText->SetText(GetStatusText(State));

			if (AchieveClear)
			{
				if (State->UnlockedTime != FDateTime::MinValue())
				{
					AchieveClear->SetPercent(1.0f);
					if (State->bRewardClaimed == false)
					{
						if (WBP_RedDotWidget)
						{
							WBP_RedDotWidget->OnRedDot();
						}
					}
				}
				else if (Def->Target > 0)
				{
					float Percent = (float)State->Progress / (float)Def->Target;
					AchieveClear->SetPercent(Percent);
				}
			}
			
		}
	}
}

bool UAchievementEntryWidget::HasRedDot() const
{
	return WBP_RedDotWidget->HasRedDot();
}

void UAchievementEntryWidget::OffRedDot() const
{
	if (WBP_RedDotWidget)
	{
		WBP_RedDotWidget->OffRedDot();
	}
}

FText UAchievementEntryWidget::GetProgressText(const FAchievementDef* Def, const  FAchievementState* State) const
{
	if (Def->Target > 0)
	{
		return FText::FromString(FString::Printf(TEXT("%d / %d"), State->Progress, Def->Target));
	}
	return FText::FromString(TEXT("정보없음")); 
}

FText UAchievementEntryWidget::GetStatusText(const FAchievementState* State) const
{
	return State->UnlockedTime != FDateTime::MinValue() ? FText::FromString(TEXT("완료")) : FText::FromString(TEXT("미완료"));
}

