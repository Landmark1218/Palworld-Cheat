// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonAsEnumBPLibrary.generated.h"

/**
 * jsonの "表示名":"説明(内部名っぽい文字列)" のペアからUser Defined Enumを作るだけの雑ツール
 */
UCLASS()
class JSONASENUM_API UJsonAsEnumBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// JsonPath      : 読み込むjsonのローカルファイルパス (フルパス)
	// DestinationPath : Enumを作るコンテンツパス (例 /Game/Enums)
	// EnumName      : 作成後のEnumアセット名
	UFUNCTION(BlueprintCallable, Category = "JsonAsEnum", meta = (DisplayName = "Create Enum From Json"))
	static bool CreateEnumFromJson(const FString& JsonPath, const FString& DestinationPath, const FString& EnumName);
};
