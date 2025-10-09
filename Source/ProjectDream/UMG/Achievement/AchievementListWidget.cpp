// Fill out your copyright notice in the Description page of Project Settings.

#include "UMG/Achievement/AchievementListWidget.h"
#include "ProjectDreamCharacter.h"
#include "GameSystems/Achievements/AchievementsSubsystem.h"
#include "ProjectDreamPlayerController.h"
#include "AchievementEntryWidget.h"
#include "Components/ListView.h"
#include "GameSystems/Achievements/AchieveViewWrapper.h"

void UAchievementListWidget::NativeConstruct()
{
	if (!AchieveManager)
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (auto* SubSys = GI->GetSubsystem<UAchievementsSubsystem>())
			{
				AchieveManager = SubSys->GetManager();
			}			
		}
	}

	if (UGameInstance* GI = GetGameInstance())
	{
		if (auto* SubSys = GI->GetSubsystem<UAchievementsSubsystem>())
		{
			SubSys->OnAchievementUpdated.AddUniqueDynamic(this, &UAchievementListWidget::UpdateAchieveEntry);
		}
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
	if (!AchieveList || !AchieveManager) return;

	TArray<FAchievementViewData> Views;
	TArray<FName> AchieveIds;
	AchieveManager->GetAllAchievementsViewData(Views, AchieveIds);

	AchieveList->ClearListItems();

	TArray<UObject*> Items;
	Items.Reserve(Views.Num());

	ensure(Views.Num() == AchieveIds.Num());


	for (int32 i = 0; i < Views.Num(); i++)
	{
		auto* Row = NewObject<UAchieveViewWrapper>(this);
		Row->AchieveId = AchieveIds[i];
		Row->Data = Views[i];
		Items.Add(Row);
		IdToItem.Add(Row->AchieveId, Row);
	}
	

	AchieveList->SetListItems(Items);
}

void UAchievementListWidget::UpdateAchieveEntry(FName EventId)
{
	UE_LOG(LogTemp,Warning,TEXT("Call UpdateAchieveEntry"));

	FAchievementViewData View;

	//
	if (!AchieveManager)
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UAchievementsSubsystem* SubSys = GI->GetSubsystem<UAchievementsSubsystem>())
			{
				SubSys->GetManager()->GetAchievementViewDataById(View, EventId);
			}
		}
	}
	//

	AchieveManager->GetAchievementViewDataById(View, EventId);

	if (UAchieveViewWrapper* Item = IdToItem.FindRef(EventId))
	{
		Item->Data = View;

		UE_LOG(LogTemp, Warning, TEXT("ViewData : %s %d"),*View.Title.ToString(), View.Progress);


		if (UUserWidget* Row = AchieveList->GetEntryWidgetFromItem(Item))
		{
			if (auto* Entry = Cast<UAchievementEntryWidget>(Row))
			{
				Entry->SetViewData(View);
			}
		}
	}
}
