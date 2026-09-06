#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonEnumGenerator.generated.h"

UCLASS()
class UJsonEnumGenerator : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "MyEnumPlugin")
    static void CreateEnumFromJson(FString JsonFilePath, FString AssetName, FString OutPath);
};