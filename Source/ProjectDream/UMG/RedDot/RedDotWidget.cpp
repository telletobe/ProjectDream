// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/RedDot/RedDotWidget.h"
#include "components/Border.h"

void URedDotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void URedDotWidget::OnRedDot()
{
	if (RedDot)
	{
		RedDot->SetVisibility(ESlateVisibility::Visible);
	}
}

void URedDotWidget::OffRedDot()
{
	if (RedDot)
	{
		RedDot->SetVisibility(ESlateVisibility::Hidden);
	}
}

bool URedDotWidget::HasRedDot()
{
	if (RedDot)
	{
		ESlateVisibility Visible = RedDot->GetVisibility();
		switch (Visible)
		{
		case ESlateVisibility::Visible:
			return true;
			break;
		default:
			return false;
			break;
		}
	}
	return false;
}


