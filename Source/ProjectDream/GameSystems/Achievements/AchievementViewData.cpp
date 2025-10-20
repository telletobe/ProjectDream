// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/Achievements/AchievementViewData.h"

bool FAchievementViewData::operator==(const FAchievementViewData& Rhs)
{
	return Title.EqualTo(Rhs.Title) && Description.EqualTo(Rhs.Description) && TargetValue == Rhs.TargetValue;
}
