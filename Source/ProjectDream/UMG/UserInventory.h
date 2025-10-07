// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameSystems/Inventory/GameInventory.h"
#include "UserInventory.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(InventoryUIWidget, Log, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateSlotIndex,int32, Index);

UCLASS()
class PROJECTDREAM_API UUserInventory : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnOffInventory();

	UFUNCTION()
	void UpdateInventoryUI();
	
	UFUNCTION()
	void UpdateInventoryUIWithIdx(int32 index);

	UFUNCTION()
	void BindInventory(UGameInventory* PlayerInventory);

	void RebuildList();

	bool CheckMousePointInUI(const FDragDropEvent& E);

	virtual bool NativeOnDrop(const FGeometry& G, const FDragDropEvent& E, UDragDropOperation* Op) override;
	virtual void NativeOnInitialized() override;

	FUpdateSlotIndex UpdateSlotIndex;
private:

	UPROPERTY()
	FGameItemData InventoryData;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBorder> InventoryBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UScrollBox> ItemScroll = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UHorizontalBox> InventoryTopBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UHorizontalBox> InventoryDescription;

	UPROPERTY()
	TObjectPtr<class UTextBlock> WeightText;

	UPROPERTY(EditAnywhere,Category="Inventory | UI")
	TSubclassOf<class UUserInventorySlot> SlotWidgetClass;

	UPROPERTY()
	TArray<TObjectPtr<UUserInventorySlot>> SlotWidgets;

	UPROPERTY()
	TWeakObjectPtr<UGameInventory> Inventory = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ButtonClose;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UQuantityPrompt> DropNumPromptClass;

	UPROPERTY()
	TObjectPtr<class UQuantityPrompt> DropNumPrompt;


private:
	void UpdateWeightText();
};
