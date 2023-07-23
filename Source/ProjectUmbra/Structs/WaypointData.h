// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProjectUmbra/Enums/EnemyType.h"
#include "ProjectUmbra/Structs/EnemyRef.h"
#include "WaypointData.generated.h"


class AWaypoint;

USTRUCT(BlueprintType)
struct PROJECTUMBRA_API FWaypointData 
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Waypoint")
		AWaypoint* m_pWaypoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Sniper Teleport Waypoints")
		TArray<AWaypoint*> m_pSniperWaypoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Enemy types")
		TArray<EEnemyType> m_TEnemyTypes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Enemy amounts")
		TArray<int> m_TEnemyAmounts;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Enemy Refs")
		TArray<FEnemyRef> m_TEnemyRefs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Time between respawn")
		float m_fTimeBetweenRespawn = 5.f;

	FTimerHandle m_FEnemyTimer;
	FTimerHandle m_FSpawnSoundTimer;
};
