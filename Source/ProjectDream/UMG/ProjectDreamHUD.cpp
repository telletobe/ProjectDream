// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/ProjectDreamHUD.h"
#include "UMG/Inventory/UserInventory.h"
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

	for (const TSubclassOf<UUserWidget>& WidgetClass : Widgets)
	{
		if (!WidgetClass) continue;
		UUserWidget* NewWidget = CreateWidget<UUserWidget>(PC,WidgetClass);		
		NewWidget->AddToViewport();
		NewWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	
}
