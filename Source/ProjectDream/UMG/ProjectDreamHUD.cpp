// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/ProjectDreamHUD.h"
#include "UserInventory.h"
#include "UMG/Achievement/AchievementListWidget.h"

AProjectDreamHUD::AProjectDreamHUD()
{

}

void AProjectDreamHUD::BeginPlay()
{
	Super::BeginPlay();

	if (InventoryWidgetClass)
	{
		InventoryWidget = CreateWidget<UUserInventory>(GetWorld(), InventoryWidgetClass);
		if (InventoryWidget)
		{
			InventoryWidget->AddToViewport();
			InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::MakeRandomColor(), TEXT("InventoryInit"));
			UE_LOG(LogTemp, Warning, TEXT("InventoryInit"));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::MakeRandomColor(), TEXT("InventoryInit Fail"));
			UE_LOG(LogTemp, Warning, TEXT("Inventory Fail"));
		}
		
	}

	if (AchievementWidgetClass)
	{
		AchievementWidget = CreateWidget<UAchievementListWidget>(GetWorld(), AchievementWidgetClass);
		if (AchievementWidget)
		{
			AchievementWidget->AddToViewport();
			AchievementWidget->SetVisibility(ESlateVisibility::Hidden);
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::MakeRandomColor(), TEXT("Achievement Init"));
			UE_LOG(LogTemp,Warning,TEXT("Achieve UI"));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::MakeRandomColor(), TEXT("Achievement Fail"));
			UE_LOG(LogTemp, Warning, TEXT("Achieve UI Fail"));
		}
	}



}
