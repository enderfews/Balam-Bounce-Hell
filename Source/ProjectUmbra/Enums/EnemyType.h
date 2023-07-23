// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnemyType.generated.h"

UENUM(BlueprintType)
enum class EEnemyType :uint8 {
	Normal=0 UMETA(DisplayName="Normal"),
	Kamikaze=1<<0 UMETA(DisplayName="Kamikaze"),
	Sniper=1<<1 UMETA(DisplayName="Sniper"),
	Circular=1<<2 UMETA(DisplayName="Circular"),
	Shotgun = 1 << 3 UMETA(DisplayName = "Shotgun"),
	Burst = 1 << 4 UMETA(DisplayName = "Burst")
};
