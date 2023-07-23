#include "ProjectUmbraEditor.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "ISettingsContainer.h"
#include "ISettingsModule.h"
#include "UnrealEdGlobals.h"
#include "AssetActions/BulletLUTActions.h"
#include "AssetActions/ControllerVibrationActions.h"
#include "AssetDetails/BulletLUTDetails.h"
#include "ComponentVisualizers/MovementComponentVisualizer.h"
#include "Editor/UnrealEdEngine.h"
#include "ProjectUmbra/Components/ObjectMovementComponent.h"
#include "ProjectUmbra/CustomDataType/BulletLUT.h"
#include "ProjectUmbra/CustomProjectSettings/ProjectUmbraSettings.h"

IMPLEMENT_MODULE(FProjectUmbraEditor, ProjectUmbraEditor)

void FProjectUmbraEditor::StartupModule()
{
	//Register AssetActions
	{
		//Get AssetTools singleton
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

		//Create custom category for assets
		const EAssetTypeCategories::Type NewCategory =
			AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("ProjectUmbra")), FText::FromString("ProjectUmbra"));

		//Register BulletLUT actions
		TSharedPtr<IAssetTypeActions> BulletLUTActions = MakeShareable(new FBulletLUTActions(NewCategory));
		AssetTools.RegisterAssetTypeActions(BulletLUTActions.ToSharedRef());

		m_AssetActions.Add(BulletLUTActions);

		//Register ControllerVibration actions
		TSharedPtr<IAssetTypeActions> ControllerVibrationActions = MakeShareable(new FControllerVibrationActions(NewCategory));
		AssetTools.RegisterAssetTypeActions(ControllerVibrationActions.ToSharedRef());

		//Save actions to array to unregister them later
		m_AssetActions.Add(ControllerVibrationActions);
	}

	//Register custom DetailsViews
	{
		//Get PropertyEditorModule
		FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

		//BulletLUT Details
		PropertyEditorModule.RegisterCustomClassLayout(
			UBulletLUT::StaticClass()->GetFName(),
			FOnGetDetailCustomizationInstance::CreateStatic(&FBulletLUTDetails::MakeInstance));
	}

	//Register Component Visualizers
	if(GUnrealEd)
	{
		TSharedPtr<FComponentVisualizer> TComponentVisualizerPtr = MakeShareable(new FMovementComponentVisualizer());
		
		GUnrealEd->RegisterComponentVisualizer(UObjectMovementComponent::StaticClass()->GetFName(), TComponentVisualizerPtr);
		TComponentVisualizerPtr->OnRegister();
	}
	
	//Register Custom Settings
    {
        ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
        if (SettingsModule)
        {
            TSharedPtr<ISettingsContainer> ProjectSettingsContainer = SettingsModule->GetContainer("Project");
            ProjectSettingsContainer->DescribeCategory("ProjectUmbraSettings", FText::FromString("Project Umbra Settings"), FText::FromString("Configuration variables for Project Umbra"));

            SettingsModule->RegisterSettings("Project", "ProjectUmbraSettings", "ProjectUmbraSettings",
                FText::FromString("Project Umbra Settings"),
                FText::FromString("Configuration variables for Project Umbra"),
                GetMutableDefault<UProjectUmbraSettings>()
            );
        }
    }
        
	IBaseModuleInterface::StartupModule();
}

void FProjectUmbraEditor::ShutdownModule()
{
	//Unregister AssetActions
	{
		//Get AssetToolsSingleton if the module is loaded
		if(FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();

			//Loop asset actions and unregister them
			for(int32 i = 0; i < m_AssetActions.Num(); ++i)
			{
				AssetTools.UnregisterAssetTypeActions(m_AssetActions[i].ToSharedRef());
			}
		}
		
		m_AssetActions.Empty();
	}

	//Unregister custom DetailsViews
	{
		if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
		{
			FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

			//BulletLUT Details
			PropertyModule.UnregisterCustomClassLayout(UBulletLUT::StaticClass()->GetFName());
		}
	}

	//Unregister Component Visualizers
	 if(GUnrealEd)
	 {
	 	GUnrealEd->UnregisterComponentVisualizer(UObjectMovementComponent::StaticClass()->GetFName());
	 }
	 
	 //Unregister Custom Settings
     ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
     if (SettingsModule)
     {
         SettingsModule->UnregisterSettings("Project", "ExampleCategory", "ExampleSettings");
     }
	
	IBaseModuleInterface::ShutdownModule();
}

void FProjectUmbraEditor::AddModuleListeners()
{
}
