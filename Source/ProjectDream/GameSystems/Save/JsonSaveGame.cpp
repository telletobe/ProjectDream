// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystems/Save/JsonSaveGame.h"

static constexpr int64 ONLY_SAVEGAME = CPF_SaveGame;

// 폴더의 경로 반환 물리적인 경로가 존재하지 않는 경우 "" 를 반환.
FString SaveJson::GetSaveDir()
{
#if PLATFORM_ANDROID
	return FPath::ProjectPersistentDownloadDir() / TEXT("SaveJson");
#else
		return FPaths::ProjectSavedDir() / TEXT("SaveJson"); // ProjectSavedDir(): FStaticData 객체를 지연생성
#endif
	//return FPaths::ProjectSavedDir() / TEXT("SaveJson");
}

FString SaveJson::MakeSavePath(const FString& SlotName)
{
	return GetSaveDir() / (SlotName + TEXT(".json"));
}

