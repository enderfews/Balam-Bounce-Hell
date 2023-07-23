#pragma once

#include "IAssetTypeActions.h"
#include "IBaseModuleInterface.h"
#include "Modules/ModuleManager.h"

class FProjectUmbraEditor : public IBaseModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual void AddModuleListeners() override;

	/**
	 * Shortcut function to get the module from the module manager
	 */
	static FProjectUmbraEditor& Get()
	{
		return FModuleManager::LoadModuleChecked<FProjectUmbraEditor>("ProjectUmbraEditor");
	}

	/**
	 * Shortcut function to get if the module is loaded from the module manager
	 */
	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ProjectUmbraEditor");
	}

private:
		TArray<TSharedPtr<IAssetTypeActions>> m_AssetActions;
};
