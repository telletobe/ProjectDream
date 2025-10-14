// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../GameSystems/Inventory/DreamItemDTO.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "DreamInventoryEntryWidget.generated.h"

class UTextBlock;

UCLASS()
class PROJECTDREAM_API UDreamInventoryEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	void SyncFromItem();
private:
	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<UImage> Iamge;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemName;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemNum;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemCategory;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemDesc;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemWeight;

	FDreamGameItemView ItemView;
};
