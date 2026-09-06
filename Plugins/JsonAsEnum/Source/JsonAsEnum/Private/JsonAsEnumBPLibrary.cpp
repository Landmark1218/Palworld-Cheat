// Copyright Epic Games, Inc. All Rights Reserved.

#include "JsonAsEnumBPLibrary.h"

#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "UObject/Package.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet2/EnumEditorUtils.h"
#include "Engine/UserDefinedEnum.h"

bool UJsonAsEnumBPLibrary::CreateEnumFromJson(const FString& JsonPath, const FString& DestinationPath, const FString& EnumName)
{
	FString JsonStr;
	if (!FFileHelper::LoadFileToString(JsonStr, *JsonPath))
	{
		UE_LOG(LogTemp, Error, TEXT("JsonAsEnum: jsonが読めない -> %s"), *JsonPath);
		return false;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonStr);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("JsonAsEnum: jsonのパースに失敗した"));
		return false;
	}

	// パスの整形。頭にスラッシュ無いとロングパッケージ名として認識されないので
	FString CleanPath = DestinationPath;
	CleanPath.RemoveFromEnd(TEXT("/"));
	if (!CleanPath.StartsWith(TEXT("/")))
	{
		CleanPath = TEXT("/") + CleanPath;
	}
	const FString PackageName = CleanPath + TEXT("/") + EnumName;

	if (FPackageName::DoesPackageExist(PackageName))
	{
		UE_LOG(LogTemp, Warning, TEXT("JsonAsEnum: 同名のアセットが既にある -> %s"), *PackageName);
		return false;
	}

	UPackage* Package = CreatePackage(*PackageName);

	UUserDefinedEnum* NewEnum = Cast<UUserDefinedEnum>(FEnumEditorUtils::CreateUserDefinedEnum(Package, FName(*EnumName), RF_Public | RF_Standalone | RF_Transactional));
	if (!NewEnum)
	{
		UE_LOG(LogTemp, Error, TEXT("JsonAsEnum: enumの生成に失敗した"));
		return false;
	}

	int32 Index = 0;
	for (const auto& Pair : JsonObject->Values)
	{
		// Key = 表示名(日本語), Value = 説明側の文字列
		FEnumEditorUtils::AddNewEnumeratorForUserDefinedEnum(NewEnum);
		FEnumEditorUtils::SetEnumeratorDisplayName(NewEnum, Index, FText::FromString(Pair.Key));

		FString Desc;
		if (Pair.Value.IsValid())
		{
			Pair.Value->TryGetString(Desc);
		}
		NewEnum->SetMetaData(TEXT("ToolTip"), *Desc, Index);

		++Index;
	}

	Package->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(NewEnum);

	// 作成後にそのまま保存。Ctrl+S忘れて後で見当たらない、みたいなのを避けるため
	const FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	return UPackage::SavePackage(Package, NewEnum, RF_Public | RF_Standalone, *PackageFileName);
}
