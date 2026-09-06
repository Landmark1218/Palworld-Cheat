#include "JsonEnumGenerator.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "AssetToolsModule.h"
#include "Engine/UserDefinedEnum.h"
#include "Kismet2/EnumEditorUtils.h"
#include "UObject/MetaData.h"
#include "UObject/UnrealType.h"

void UJsonEnumGenerator::CreateEnumFromJson(FString JsonFilePath, FString AssetName, FString OutPath)
{
    FString myVeryLongJsonFileContentString = TEXT("");
    bool bDidFileReadSucceed = false;

    UE_LOG(LogTemp, Log, TEXT("JSONファイルの読み込みを開始するよ: %s"), *JsonFilePath);
    bDidFileReadSucceed = FFileHelper::LoadFileToString(myVeryLongJsonFileContentString, *JsonFilePath);

    if (bDidFileReadSucceed == false)
    {
        UE_LOG(LogTemp, Error, TEXT("ファイルが読み込めなかったから中断するね: %s"), *JsonFilePath);
        return; 
    }

    TSharedPtr<FJsonObject> parsedJsonRootObject;
    TSharedRef<TJsonReader<>> jsonReaderInstance = TJsonReaderFactory<>::Create(myVeryLongJsonFileContentString);
    
    bool bWasDeserializationSuccessful = false;
    bWasDeserializationSuccessful = FJsonSerializer::Deserialize(jsonReaderInstance, parsedJsonRootObject);

    if (bWasDeserializationSuccessful == true)
    {
        if (parsedJsonRootObject.IsValid() == true)
        {
            FAssetToolsModule& assetToolsModuleReference = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");

            UObject* newlyCreatedAssetObject = assetToolsModuleReference.Get().CreateAsset(AssetName, OutPath, UUserDefinedEnum::StaticClass(), nullptr);
            UUserDefinedEnum* targetUserDefinedEnumAsset = Cast<UUserDefinedEnum>(newlyCreatedAssetObject);

            if (targetUserDefinedEnumAsset != nullptr)
            {
                // ★ここを修正：CppFormはbyte型なので uint8* で取得して正しく Namespaced (1) に書き換える
                FProperty* foundCppFormProperty = FindFProperty<FProperty>(targetUserDefinedEnumAsset->GetClass(), TEXT("CppForm"));
                if (foundCppFormProperty != nullptr)
                {
                    uint8* propertyValueAddress = foundCppFormProperty->ContainerPtrToValuePtr<uint8>(targetUserDefinedEnumAsset);
                    if (propertyValueAddress != nullptr)
                    {
                        *propertyValueAddress = 1; // 1 = ECppForm::Namespaced
                        UE_LOG(LogTemp, Log, TEXT("CppFormをuint8として無事にNamespacedに書き換えた！"));
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("警告：CppFormプロパティが見つからなかった..."));
                }

                TArray<FString> extractedKeyList;
                TArray<FString> extractedValueList;
                
                int32 jsonElementCounter = 0;
                for (auto& jsonKeyValuePair : parsedJsonRootObject->Values)
                {
                    FString currentMapKey = jsonKeyValuePair.Key;
                    FString currentMapValue = jsonKeyValuePair.Value->AsString();
                    
                    extractedKeyList.Add(currentMapKey);
                    extractedValueList.Add(currentMapValue);
                    
                    jsonElementCounter = jsonElementCounter + 1;
                }

                UE_LOG(LogTemp, Log, TEXT("取得したパッシブスキルの数: %d 個"), jsonElementCounter);

                int32 currentLoopIndex = 0;
                while (currentLoopIndex < extractedKeyList.Num())
                {
                    FString targetDisplayNameKey = extractedKeyList[currentLoopIndex];
                    FString targetInternalIdVal = extractedValueList[currentLoopIndex];

                    if (currentLoopIndex >= 1)
                    {
                        FEnumEditorUtils::AddNewEnumeratorForUserDefinedEnum(targetUserDefinedEnumAsset);
                    }

                    FString enumeratorBaseNameString = TEXT("NewEnumerator");
                    enumeratorBaseNameString.AppendInt(currentLoopIndex);
                    
                    FName fullGeneratedEnumName = *targetUserDefinedEnumAsset->GenerateFullEnumName(*enumeratorBaseNameString);

                    targetUserDefinedEnumAsset->DisplayNameMap.Add(fullGeneratedEnumName, FText::FromString(targetDisplayNameKey));
                    
                    UPackage* currentAssetPackage = targetUserDefinedEnumAsset->GetOutermost();
                    UMetaData* assetMetadataPointer = currentAssetPackage ? currentAssetPackage->GetMetaData() : nullptr;
                    
                    if (assetMetadataPointer != nullptr)
                    {
                        FString tooltipMetaKeyName = fullGeneratedEnumName.ToString();
                        tooltipMetaKeyName = tooltipMetaKeyName + TEXT(".ToolTip");
                        
                        assetMetadataPointer->SetValue(targetUserDefinedEnumAsset, *tooltipMetaKeyName, *targetInternalIdVal);
                    }

                    currentLoopIndex = currentLoopIndex + 1;
                }

                FEnumEditorUtils::EnsureAllDisplayNamesExist(targetUserDefinedEnumAsset);
                targetUserDefinedEnumAsset->PostEditChange();
                targetUserDefinedEnumAsset->MarkPackageDirty();

                UE_LOG(LogTemp, Log, TEXT("処理完了！Enumアセットが正常に生成されたはず！"));
            }
        }
    }
}