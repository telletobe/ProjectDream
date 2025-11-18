// Fill out your copyright notice in the Description page of Project Settings.

#include "UMG/Achievement/AchievementListWidget.h"
#include "ProjectDreamCharacter.h"
#include "GameSystems/Achievements/AchievementsSubsystem.h"
#include "GameSystems/Achievements/AchievementView.h"
#include "ProjectDreamPlayerController.h"
#include "AchievementEntryWidget.h"
#include "Components/ListView.h"
#include "GameSystems/RedDot/RedDotSubSystem.h"

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
		AchieveList->RegenerateAllEntries();
	}
	
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

void UAchievementListWidget::UpdateAchieveEntry(FName EventId)
{
	if (!AchieveList) return;	
	RefreshAll();
}

void UAchievementListWidget::HandleItemClicked(UObject* Item)
{
	UE_LOG(LogTemp,Warning,TEXT("Call HandleItemCilcked"));

	if (UAchievementView* View = Cast<UAchievementView>(Item))
	{		
		const FName& EventId = View->AchievementID;

		if (EventId.IsValid())
		{
			if (UGameInstance* GI = GetGameInstance())
			{
				if (UAchievementsSubsystem* AchieveSubSys = GI->GetSubsystem<UAchievementsSubsystem>())
				{
					AchieveSubSys->ClaimReward(EventId);
				}
			}
		}
		else
		{
			return;
		}	
	}

	if (AchieveList)
	{
		if (UUserWidget* EntryWidget = AchieveList->GetEntryWidgetFromItem(Item))
		{
			if (auto* Entry = Cast<UAchievementEntryWidget>(EntryWidget))
			{
				Entry->SyncFromItem(Item);   
				AchieveList->RegenerateAllEntries();
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
	case ESlateVisibility::Hidden:
		SetVisibility(ESlateVisibility::Visible);
		break;
	default:
		break;
	}
}