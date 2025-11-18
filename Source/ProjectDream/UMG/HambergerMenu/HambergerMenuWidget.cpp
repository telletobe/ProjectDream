// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/HambergerMenu/HambergerMenuWidget.h"
#include "Components/Button.h"
#include "../Achievement/AchievementListWidget.h"
#include "../RedDot/RedDotWidget.h"
#include "GameSystems/RedDot/RedDotSubSystem.h"

void UHambergerMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (Btn_Achievement)
	{
		Btn_Achievement->OnClicked.AddUniqueDynamic(this, &UHambergerMenuWidget::OnOffAchievementWidget);
	}

	if (WBP_RedDot)
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (URedDotSubSystem* RedDotSubSys = GI->GetSubsystem<URedDotSubSystem>())
			{
				if (RedDotSubSys->IsTickable())
				{
					WBP_RedDot->OnRedDot();
				}
				else
				{
					WBP_RedDot->OffRedDot();
				}
				
			}
		}
	}
}

void UHambergerMenuWidget::OnOffAchievementWidget()
{
	if (WBP_AchievementListWidget)
	{
		WBP_AchievementListWidget->OnOffUI();
	}
}

void UHambergerMenuWidget::InitHambergerMenu(UUserWidget* InWidget)
{
	if (UAchievementListWidget* AchievementListWidget = Cast<UAchievementListWidget>(InWidget))
	{
		WBP_AchievementListWidget = AchievementListWidget;
	}
}
