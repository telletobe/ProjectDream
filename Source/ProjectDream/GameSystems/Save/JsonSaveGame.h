// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "DreamSaveGame.h"

/**
 * 
 */

namespace SaveJson 
{
	FString GetSaveDir();
	FString MakeSavePath(const FString& SlotName);

	template<typename TStruct>
	bool ToJsonString(const TStruct& InData, FString& OutJson, bool bPretty = true)
	{
		const int64 CheckFlags = 0;
		const int64 SkipFlags = 0;
		const int32 Indent = bPretty ? 1 : 0;
		return FJsonObjectConverter::UStructToJsonObjectString(TStruct::StaticStruct(), &InData, OutJson, CheckFlags, SkipFlags, Indent);
	}

	template<typename TStruct>
	bool FromJsonString(const FString& InJson, TStruct& OutData)
	{
		const int64 CheckFlags = 0;
		const int64 SkipFlags = 0;
		return FJsonObjectConverter::JsonObjectStringToUStruct(InJson, &OutData, CheckFlags, SkipFlags);
	}

	template<typename TStruct>
	bool SaveToFile(const FString& SlotName, const TStruct& Data, bool bPretty)
	{
		FString Json;
		if (!ToJsonString<TStruct>(Data, Json, bPretty))
		{
			return false;
		}

		IFileManager::Get().MakeDirectory(*GetSaveDir(),true);
		const FString Path = MakeSavePath(SlotName);

		if (!FFileHelper::SaveStringToFile(Json, *Path, FFileHelper::EEncodingOptions::ForceUTF8))
		{
			return false;
		}
		return true;
	}

	template<typename TStruct>
	bool LoadFromFile(const FString& SlotName, TStruct& OutData)
	{
		const FString Path = MakeSavePath(SlotName);
		if (!FPaths::FileExists(Path))
		{
			return false;
		}

		FString Json;
		if (!FFileHelper::LoadFileToString(Json, *Path))
		{
			return false;
		}

		if (!FromJsonString<TStruct>(Json, OutData))
		{
			return false;
		}
		return true;
	}

	template<typename ElemStruct>
	inline bool ArrayToJsonString(const TArray<ElemStruct>& InArray, FString& OutJson, bool bPretty /*= true*/)
	{
		const int64 CheckFlags = 0;
		const int64 SkipFlags = 0;

		// 1) 각 요소를 FJsonObject로 변환하여 JSON 배열 구성
		TArray<TSharedPtr<FJsonValue>> JsonArray;
		JsonArray.Reserve(InArray.Num());

		for (const ElemStruct& Elem : InArray)
		{
			TSharedPtr<FJsonObject> Obj = MakeShared<FJsonObject>();
			if (!FJsonObjectConverter::UStructToJsonObject(
				ElemStruct::StaticStruct(), &Elem, Obj.ToSharedRef(), CheckFlags, SkipFlags))
			{
				return false;
			}
			JsonArray.Add(MakeShared<FJsonValueObject>(Obj));
		}

		// 2) 배열을 문자열로 직렬화 (분기마다 개별 writer 생성)
		if (bPretty)
		{
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJson); // Pretty
			return FJsonSerializer::Serialize(JsonArray, Writer);
		}
		else
		{
			TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer =
				TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutJson); // Condensed
			return FJsonSerializer::Serialize(JsonArray, Writer);
		}
	}

	template<typename ElemStruct>
	inline bool ArrayFromJsonString(const FString& InJson, TArray<ElemStruct>& OutArray)
	{
		const int64 CheckFlags = 0;
		const int64 SkipFlags = 0;
		return FJsonObjectConverter::JsonArrayStringToUStruct(
			InJson, &OutArray, CheckFlags, SkipFlags);
	}

	template<typename ElemStruct>
	inline bool SaveArrayToFile(const FString& SlotName, const TArray<ElemStruct>& InArray, bool bPretty = true)
	{
		FString Json;
		if (!ArrayToJsonString<ElemStruct>(InArray, Json, bPretty)) return false;

		IFileManager::Get().MakeDirectory(*GetSaveDir(), true);
		return FFileHelper::SaveStringToFile(Json, *MakeSavePath(SlotName), FFileHelper::EEncodingOptions::ForceUTF8);
	}

	template<typename ElemStruct>
	inline bool LoadArrayFromFile(const FString& SlotName, TArray<ElemStruct>& OutArray)
	{
		const FString Path = MakeSavePath(SlotName);
		if (!FPaths::FileExists(Path)) return false;

		FString Json;
		if (!FFileHelper::LoadFileToString(Json, *Path)) return false;
		return ArrayFromJsonString<ElemStruct>(Json, OutArray);
	}
}
