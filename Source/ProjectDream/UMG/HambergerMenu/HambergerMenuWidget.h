// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HambergerMenuWidget.generated.h"

class URedDotWidget;
class UButton;
class UAchievementListWidget;

UCLASS()
class PROJECTDREAM_API UHambergerMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& Geo, float InDeltaTime) override;
	UFUNCTION() void OnOffAchievementWidget();
	UFUNCTION() void OnOffRedDot(bool HasRedDot);
	void InitHambergerMenu(UUserWidget* InWidget);
private:

	UPROPERTY(meta = (BindWidget))	TObjectPtr<URedDotWidget> WBP_RedDot;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton>Btn_Achievement;
	UPROPERTY() TObjectPtr<UAchievementListWidget> WBP_AchievementListWidget;
};
