// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"

class AUTOMATINATOR_API FAutomatinatorModule : public IModuleInterface {

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};