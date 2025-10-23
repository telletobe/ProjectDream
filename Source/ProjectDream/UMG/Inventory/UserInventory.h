// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameSystems/Inventory/GameInventory.h"
#include "UserInventory.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(InventoryUIWidget, Log, All);

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
	
	//UFUNCTION()
	//void UpdateInventoryUIWithIdx(int32 index);
	void RebuildList();

	//bool CheckMousePointInUI(const FDragDropEvent& E);

//	virtual bool NativeOnDrop(const FGeometry& G, const FDragDropEvent& E, UDragDropOperation* Op) override;
	virtual void NativeOnInitialized() override;

private:

	UPROPERTY(meta = (BindWidget)) TObjectPtr<class UBorder> InventoryBorder;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<class UListView> ItemList;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<class UHorizontalBox> InventoryTopBar;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<class UHorizontalBox> InventoryDescription;

	UPROPERTY()	TObjectPtr<class UTextBlock> WeightText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ButtonClose;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UQuantityPrompt> DropNumPromptClass;

	UPROPERTY()
	TObjectPtr<class UQuantityPrompt> DropNumPrompt;

private:
	void UpdateWeightText();
};

namespace EnumTextUtils
{
	template<typename TEnum>
	FText GetDisplayName(TEnum Value)
	{
		static_assert(TIsEnum<TEnum>::Value, "TEnum must be an Enum");
		if (const UEnum* EnumPtr = StaticEnum<TEnum>())
		{
			return EnumPtr->GetDisplayNameTextByValue(static_cast<int64>(Value));
		}
		return FText::FromString(TEXT("Invalid"));
	}
}
