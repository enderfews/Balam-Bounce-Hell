// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ShootingType.generated.h"

UENUM(BlueprintType)
enum class EShootingType : uint8 {
	Normal=0 UMETA(DisplayName="Normal"),
	Shotgun=1<<1 UMETA(DisplayName="Shotgun")
};
