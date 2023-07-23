// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnemyCombatState.generated.h"


UENUM(BlueprintType)
enum class EEnemyCombatState : uint8
{
	OnShootingRange=0 UMETA(DisplayName="On Shooting Range"),
	OnPursuitRange =1<<0  UMETA(DisplayName = "On Pursuit Range"),
	OnPunchRange = 1<<1 UMETA(DisplayName = "On Punch Range"),
	OnStayBack = 1<<2 UMETA(DisplayName="On Stay Back"),
	END = 1<<3 UMETA(DisplayName="END")
};
