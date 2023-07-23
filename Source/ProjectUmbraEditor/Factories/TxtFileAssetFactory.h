// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "TxtFileAssetFactory.generated.h"

/**
 * 
 */
UCLASS()
class  UTxtFileAssetFactory : public UFactory
{
	GENERATED_BODY()
public:
	UTxtFileAssetFactory();

	 UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
};
