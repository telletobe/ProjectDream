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

void UAchievementListWidget::RefreshAll()
{
	if (!AchieveList) return;
	AchieveList->ClearListItems();
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
				TArray<FAchievementViewData> Views;
				TArray<FName> AchieveIds;

				AchieveList->ClearListItems();
				SubSys->GetAllViewData(Views, AchieveIds);

				BulidItemsAndIdMap(Views, AchieveIds, Items);
			}
		}
	}

	AchieveList->SetListItems(Items);
}

bool UAchievementListWidget::BulidItemsAndIdMap(const TArray<FAchievementViewData>& Views, const TArray<FName>& AchieveIds, TArray<UObject*>& OutItems)
{
	if (bInitailize) return false;
	check(Views.Num() == AchieveIds.Num());

	OutItems.Reset();
	OutItems.Reserve(Views.Num());

	for (int32 i = 0; i < Views.Num(); ++i)
	{
		auto* Row = NewObject<UAchieveViewWrapper>(this);
		Row->Data = Views[i];

		OutItems.Add(Row);
		IdToItem.Add(AchieveIds[i], Row);
	}

	bInitailize = true;
	return true;
}

void UAchievementListWidget::UpdateAchieveEntry(FName EventId)
{
	if (!AchieveList) return;	
	SyncMapToId(EventId);
	RefreshAll();
}

void UAchievementListWidget::HandleItemClicked(UObject* Item)
{
	UE_LOG(LogTemp,Warning,TEXT("Call HanelItemCilcked"));
	if (UUserWidget* EntryWidget = AchieveList->GetEntryWidgetFromItem(Item))
	{	
		if (UAchievementEntryWidget* Entry = Cast<UAchievementEntryWidget>(EntryWidget))
		{
			if (Entry->HasRedDot())
			{
				Entry->OffRedDot();

				const FName* EventId = IdToItem.FindKey(Cast<UAchieveViewWrapper>(Item));
				if (EventId)
				{
					if (UGameInstance* GI = GetGameInstance())
					{
						if (URedDotSubSystem* SubSys = GI->GetSubsystem<URedDotSubSystem>())
						{
							SubSys->MarkSeen(*EventId);
						}
					}
				}
				else
				{
					return;
				}
			}
			else
			{
				return;
			}		
		}
	}	
}

void UAchievementListWidget::SyncMapToId(FName& EventId)
{
	FAchievementViewData NewView;

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UAchievementsSubsystem* SubSys = GI->GetSubsystem<UAchievementsSubsystem>())
		{
			SubSys->GetViewDataById(NewView, EventId);

			UAchieveViewWrapper* RowPtr = *IdToItem.Find(EventId);
			if (!RowPtr)
			{
				return;
			}
			RowPtr->Data = NewView;
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