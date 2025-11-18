// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/ProjectDreamHUD.h"
#include "UMG/Inventory/UserInventory.h"
#include "UMG/HambergerMenu/HambergerMenuWidget.h"
#include "UMG/Achievement/AchievementListWidget.h"
DEFINE_LOG_CATEGORY(LogHUD);

AProjectDreamHUD::AProjectDreamHUD()
{

}

void AProjectDreamHUD::BeginPlay()
{
	Super::BeginPlay();

	if (Widgets.Num() == 0) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();

	if (!PC) return;

	UHambergerMenuWidget* HamWidget = nullptr;
	UAchievementListWidget* AchievementListWidget = nullptr;

	for (const TSubclassOf<UUserWidget>& WidgetClass : Widgets)
	{
		if (!WidgetClass) continue;
		UUserWidget* NewWidget = CreateWidget<UUserWidget>(PC,WidgetClass);		

		if (auto* Ach = Cast<UAchievementListWidget>(NewWidget))
		{
			AchievementListWidget = Ach;
		}
		else if (auto* Ham = Cast<UHambergerMenuWidget>(NewWidget))
		{
			HamWidget = Ham;
		}

		if (NewWidget->IsA(UHambergerMenuWidget::StaticClass()))
		{
			NewWidget->AddToViewport();
		}
		else
		{
			NewWidget->AddToViewport();
			NewWidget->SetVisibility(ESlateVisibility::Hidden);
		}

	}	

	if (HamWidget && AchievementListWidget)
	{
		HamWidget->InitHambergerMenu(AchievementListWidget);
	}
}
