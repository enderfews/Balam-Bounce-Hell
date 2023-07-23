// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SniperTeleportPattern.generated.h"


UENUM(BlueprintType)
enum class ESniperTeleportPattern : uint8
{
	Randomly = 0 UMETA(DisplayName="Randomly"),
	FollowWaypointsLinearly = 1<<0 UMETA(DisplayName="Follow Waypoints Linearly"),
	FollowWaypointsRandomly = 1<<1 UMETA(DisplayName="Follow Waypoints Randomly")
};
