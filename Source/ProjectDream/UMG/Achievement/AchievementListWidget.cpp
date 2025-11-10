// Fill out your copyright notice in the Description page of Project Settings.

#include "UMG/Achievement/AchievementListWidget.h"
#include "ProjectDreamCharacter.h"
#include "GameSystems/Achievements/AchievementsSubsystem.h"
#include "ProjectDreamPlayerController.h"
#include "AchievementEntryWidget.h"
#include "Components/ListView.h"
#include "GameSystems/Achievements/AchievementView.h"

void UAchievementListWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (UGameInstance* GI = GetGameInstance())
	{
		if (auto* SubSys = GI->GetSubsystem<UAchievementsSubsystem>())
		{
			SubSys->OnAchievementUpdated.AddUniqueDynamic(this, &UAchievementListWidget::UpdateAchieveEntry);
		}
	}

	if (AchieveList)
	{
		AchieveList->OnItemClicked().AddUObject(this, &UAchievementListWidget::HandleItemClicked);
	}

	AProjectDreamPlayerController* PC = Cast<AProjectDreamPlayerController>(GetOwningPlayer());
	if (PC)
	{
		AProjectDreamCharacter* Player = Cast<AProjectDreamCharacter>(PC->GetPawn());
		if (Player)
		{
			Player->OnAchievementEvent.AddUniqueDynamic(this, &UAchievementListWidget::OnOffUI);
		}
	}
	RefreshAll();
}

void UAchievementListWidget::RefreshAll()
{
	if (!AchieveList) return;
	TArray<UObject*> Items;

	if (bInitailize)
	{
		Items.Reserve(IdToItem.Num());

		// IdToItem의 Value(Wrapper)들만 모아서 다시 SetListItems
		for (auto& Elem : IdToItem)
		{
			if (Elem.Value)
			{
				Items.Add(Elem.Value);
			}
		}
		AchieveList->RegenerateAllEntries();
	}
	else
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UAchievementsSubsystem* SubSys = GI->GetSubsystem<UAchievementsSubsystem>())
			{
				TArray<FName> AchieveIds;
				AchieveIds = SubSys->GetAllAchievementIds();
				
				for (const FName& Id : AchieveIds)
				{
					UAchievementView* ItemObj = NewObject<UAchievementView>(this);
					ItemObj->AchievementID = Id;

					Items.Add(ItemObj);
				}

				AchieveList->ClearListItems();
				AchieveList->SetListItems(Items);
				bInitailize = true;
			}
		}
	}
}

void UAchievementListWidget::UpdateAchieveEntry(FName EventId)
{
	if (!AchieveList) return;	
	RefreshAll();
}

void UAchievementListWidget::HandleItemClicked(UObject* Item)
{
	UE_LOG(LogTemp,Warning,TEXT("Call HanelItemCilcked"));
	if (UUserWidget* EntryWidget = AchieveList->GetEntryWidgetFromItem(Item))
	{	
		if (UAchievementEntryWidget* Entry = Cast<UAchievementEntryWidget>(EntryWidget))
		{			
			const FName* EventId = IdToItem.FindKey(Cast<UAchievementView>(Item));
			if (EventId)
			{
				// 레드닷 로직 실행
			}
			else
			{
				return;
			}	
		}
	}	
}

void UAchievementListWidget::OnOffUI()
{
	ESlateVisibility Visible = GetVisibility();

	switch (Visible)
	{
	case ESlateVisibility::Visible:
		SetVisibility(ESlateVisibility::Hidden);
		break;
	case ESlateVisibility::Collapsed:
		break;
	case ESlateVisibility::Hidden:
		SetVisibility(ESlateVisibility::Visible);
		break;
	case ESlateVisibility::HitTestInvisible:
		break;
	case ESlateVisibility::SelfHitTestInvisible:
		break;
	default:
		break;
	}
}