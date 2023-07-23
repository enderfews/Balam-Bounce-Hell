// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CheckpointData.generated.h"


class ABaseEnemy;

USTRUCT(BlueprintType)
struct PROJECTUMBRA_API FCheckpointData 
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Enemy Class")
		UClass* m_UEnemyClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Enemy ref")
		ABaseEnemy* m_pEnemyRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Enemy location")
		FVector m_vEnemyLoc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Enemy rotation")
		FRotator m_rEnemyRot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Enemy health")
		float m_fEnemyHealth;

	bool operator==(const FCheckpointData& _checkData) const
	{
		return(m_pEnemyRef == _checkData.m_pEnemyRef) || ((m_vEnemyLoc == _checkData.m_vEnemyLoc) && (m_rEnemyRot == _checkData.m_rEnemyRot) && (m_fEnemyHealth == _checkData.m_fEnemyHealth));
	}
};
