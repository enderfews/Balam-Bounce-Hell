#pragma once

#include "BulletLUTCreateFactory.generated.h"

UCLASS()
class UBulletLUTCreateFactory : public UFactory
{
	GENERATED_BODY()
	
public:
	UBulletLUTCreateFactory(const FObjectInitializer& ObjectInitializer);
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
