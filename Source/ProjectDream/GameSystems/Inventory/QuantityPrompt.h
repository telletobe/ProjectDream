// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuantityPrompt.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTDREAM_API UQuantityPrompt : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;
	void SetSlotData(class UUserInventorySlot* UserSlot);
	void SetNumberText(int32 Number);
private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ButtonOK;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ButtonCancle;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USpinBox> NumberTextBox;

	UFUNCTION()
	void OnClickOK();

	UFUNCTION()
	void OnClickCancle();

	TWeakObjectPtr<class UUserInventorySlot> SlotData;
};
