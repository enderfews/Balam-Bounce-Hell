#pragma once

#include "CoreGlobals.h"
#include "Modules/ModuleInterface.h"
#include "Templates/SharedPointer.h"

/**
 * Interface used by tools to be notified on the startup and shutdown of a module.
 * The module needs to register the tool.
 */
class IModuleListenerInterface
{
public:
	virtual void OnStartupModule() {}
	virtual void OnShutdownModule() {}
};

/**
 * Base class for creating modules with listeners, it adds a function to register
 * the tools to the module and logic to call them on startup and shutdown.
 */
class IBaseModuleInterface : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		if(!IsRunningCommandlet())
		{
			AddModuleListeners();
			for(int32 i = 0; i < ModuleListeners.Num(); ++i)
			{
				ModuleListeners[i]->OnStartupModule();
			}
		}
	}

	virtual void ShutdownModule() override
	{
		for(int32 i = 0; i < ModuleListeners.Num(); ++i)
		{
			ModuleListeners[i]->OnShutdownModule();
		}
	}

	virtual void AddModuleListeners() {}

protected:
	TArray<TSharedRef<IModuleListenerInterface>> ModuleListeners;
};