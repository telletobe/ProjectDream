// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryDropCatcher.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTDREAM_API UInventoryDropCatcher : public UUserWidget
{
	GENERATED_BODY()
	
public :
	UPROPERTY()
	TWeakObjectPtr<class UGameInventory> InventoryRef;

	UPROPERTY()
	TWeakObjectPtr<class APlayerController> PCRef;

	void InitCatcher(UGameInventory* InInventory, APlayerController* InPC);

	
protected:
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeo, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOp) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOp) override;



private:
	void HandleDropToWorld(const FVector2D& ScreenPos, UObject* Payload);
};
