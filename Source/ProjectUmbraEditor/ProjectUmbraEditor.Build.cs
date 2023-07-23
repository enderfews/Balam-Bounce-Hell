// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class ProjectUmbraEditor : ModuleRules
{
	public ProjectUmbraEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "LevelEditor", "Slate", "AIModule", "ComponentVisualizers" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "SlateCore", "UnrealEd", "ProjectUmbra", "PropertyEditor","Automatinator" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}