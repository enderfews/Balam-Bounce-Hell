// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "EnemyMoveMode.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EEnemyMoveMode : uint8 {
	WanderAroundMap =0 UMETA(DisplayName="Wander around map"),
	WanderOnItsArea =1<<0 UMETA(DisplayName="Wander on its area"),
	Guard = 1<<1 UMETA(DisplayName="Guard"),
	FollowWP = 1<<2 UMETA(DisplayName="Follow WP"),
	END = 1<<3 UMETA(DisplayName = "END")
};
