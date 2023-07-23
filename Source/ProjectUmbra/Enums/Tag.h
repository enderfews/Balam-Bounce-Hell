// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tag.generated.h"

UENUM(BlueprintType)
enum class ETag :uint8 {
	Explosive=0 UMETA(DisplayName="Explosive"),
	Piercing=1<<1 UMETA(DisplayName="Piercing"),
	Bouncing=1<<2 UMETA(DisplayName="Bouncing"),
	Slime=1<<3 UMETA(DisplayName="Slime")
};
