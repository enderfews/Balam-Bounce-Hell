#pragma once

#include "Factories/Factory.h"
#include "ControllerVibrationCreateFactory.generated.h"

UCLASS()
class UControllerVibrationCreateFactory : public UFactory
{
	GENERATED_BODY()
	
public:
public:
	UControllerVibrationCreateFactory(const FObjectInitializer& ObjectInitializer);
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
