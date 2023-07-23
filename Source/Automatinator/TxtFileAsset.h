// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TxtFileAsset.generated.h"

/**
 * 
 */
UCLASS()
class AUTOMATINATOR_API UTxtFileAsset : public UObject
{
	GENERATED_BODY()
	
public:
	FString m_sFileDir;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Attributes",DisplayName="Lines")
	TArray<FString> m_TLines;


	UTxtFileAsset();

	UFUNCTION(BlueprintCallable)
	void WriteLine(int i,const FString& _s);
	UFUNCTION(BlueprintCallable)
	void ReadLine(int i,FString& s_);
	UFUNCTION(BlueprintCallable)
	void AddChanges();
};
