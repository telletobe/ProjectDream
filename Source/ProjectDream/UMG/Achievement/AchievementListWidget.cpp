// Fill out your copyright notice in the Description page of Project Settings.

#include "UMG/Achievement/AchievementListWidget.h"
#include "ProjectDreamCharacter.h"
#include "GameSystems/Achievements/AchievementsSubsystem.h"
#include "GameSystems/RedDot/RedDotSubSystem.h"
#include "ProjectDreamPlayerController.h"
#include "AchievementEntryWidget.h"
#include "Components/ListView.h"
#include "GameSystems/Achievements/AchieveViewWrapper.h"

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

void UAchievementListWidget::RefreshAll()
{
	if (!AchieveList) return;

	IdToItem.Reset();
	TArray<FAchievementViewData> Views;
	TArray<FName> AchieveIds;

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAchievementsSubsystem* SubSys = GI->GetSubsystem<UAchievementsSubsystem>())
		{
			SubSys->GetAllViewData(Views,AchieveIds);
		}
	}

	AchieveList->ClearListItems();

	TArray<UObject*> Items;
	Items.Reserve(Views.Num());

	for (int32 i = 0; i < Views.Num(); i++)
	{
		auto* Row = NewObject<UAchieveViewWrapper>(this);
		Row->Data = Views[i];
		Items.Add(Row);
		IdToItem.Add(AchieveIds[i], Row);
	}
	AchieveList->SetListItems(Items);
}

void UAchievementListWidget::UpdateAchieveEntry(const FName EventId)
{
	if (!AchieveList) return;

	FAchievementViewData View = FAchievementViewData();
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAchievementsSubsystem* SubSys = GI->GetSubsystem<UAchievementsSubsystem>())
		{
			SubSys->GetViewDataById(View,EventId);
		}
	}

	if (UAchieveViewWrapper* Item = *IdToItem.Find(EventId))
	{
		if (UUserWidget* Row = AchieveList->GetEntryWidgetFromItem(Item))
		{
			if (UAchievementEntryWidget* Entry = Cast<UAchievementEntryWidget>(Row))
			{
				Entry->SetViewItem(&View);
				Entry->SyncFromItem();	
				
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateAchieveEntry null"));
		return;
	}
}

void UAchievementListWidget::HandleItemClicked(UObject* Item)
{
	if (UUserWidget* EntryWidget = AchieveList->GetEntryWidgetFromItem(Item))
	{	
		if (UAchievementEntryWidget* Entry = Cast<UAchievementEntryWidget>(EntryWidget))
		{
			Entry->OffRedDot();

			const FName* EventId = IdToItem.FindKey(Cast<UAchieveViewWrapper>(Item));
			if (EventId)
			{
				if (UGameInstance* GI = GetGameInstance())
				{
					if (auto* SubSys = GI->GetSubsystem<URedDotSubSystem>())
					{
						SubSys->MarkSeen(*EventId);
					}
				}

				if (UGameInstance* GI = GetGameInstance())
				{
					if (UAchievementsSubsystem* SubSys = GI->GetSubsystem<UAchievementsSubsystem>())
					{
						FAchievementState* State = SubSys->GetAchievementStateById(*EventId);
						if (State->UnlockedTime != FDateTime::MinValue() && State->bRewardClaimed == false)
						{
							State->bRewardClaimed = true;
						}
					}
				}
			}
		}
	}	
}
