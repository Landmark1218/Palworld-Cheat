#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "Engine/UserDefinedEnum.h"
#include "JsonEnumFactory.generated.h"

UCLASS()
class UJsonEnumFactory : public UFactory
{
    GENERATED_BODY()
public:
    UJsonEnumFactory()
    {
        SupportedClass = UUserDefinedEnum::StaticClass();
        bCreateNew = true;
        bEditAfterNew = true;
    }

    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override
    {
        return NewObject<UUserDefinedEnum>(InParent, InName, Flags);
    }
};