// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RedDotWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTDREAM_API URedDotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	void OnRedDot();
	void OffRedDot();
	bool HasRedDot();
private:
	UPROPERTY(meta = (BindWidget)) TObjectPtr<class UBorder> RedDot;
	
	
};
