// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"
#include "ProbabilityManager.generated.h"

class UProjectUmbraGameInstance;
class UProjectUmbraSettings;
class AHealthPack;
class ATagDrop;

UCLASS(Blueprintable)
class PROJECTUMBRA_API UProbabilityManager : public UObject
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	TArray<ATagDrop*> m_TCurrentTagDrops;
	UPROPERTY()
	TArray<AHealthPack*> m_TCurrentHealthDrops;
	UPROPERTY()
	APlayerCharacter* m_APlayerPtr;
	
	float m_fProbabilityAt1Health;
	float m_fProbabilityAt2Health;
	float m_fProbabilityAt3Health;
	float m_fProbabilityAt4Health;
	float m_fProbabilityAt5Health;
	
	float m_fBaseProbability;
	float m_fProbabilityReductionPerFloorTag;
	float m_fProbabilityReductionIfPlayerHasTag;

	UPROPERTY()
	UProjectUmbraSettings* m_UProjectSettings;
	UPROPERTY()
	UProjectUmbraGameInstance* m_UProjectGameInstance;

public:
	// Sets default values for this actor's properties
	UProbabilityManager();

	void Init(UProjectUmbraSettings* _UProjectSettings, UProjectUmbraGameInstance* _UProjectGameInstance);

	bool CanSpawnTagDrop();
	UFUNCTION(BlueprintCallable)
	bool CanSpawnHealthDrop();

	void RegisterTagDrop(ATagDrop* _ADropToRegister);
	void UnRegisterTagDrop(ATagDrop* _ADropToUnRegister);
	UFUNCTION(BlueprintCallable)
	void RegisterHealthDrop(AHealthPack* _ADropToRegister);
	void UnRegisterHealthDrop(AHealthPack* _ADropToUnRegister);
	void SetPlayer(APlayerCharacter* _APlayerPtr);
};
