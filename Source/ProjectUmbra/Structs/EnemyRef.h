// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EnemyRef.generated.h"


class ABaseEnemy;

USTRUCT(BlueprintType)
struct PROJECTUMBRA_API FEnemyRef 
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Enemy refs")
		TArray<ABaseEnemy*> m_TEnemyRefs;
};
