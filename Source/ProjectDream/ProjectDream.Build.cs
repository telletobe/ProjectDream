// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectDream : ModuleRules
{
	public ProjectDream(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Json",
			"JsonUtilities",
			"Slate",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			   "Slate", "SlateCore"
		});

		PublicIncludePaths.AddRange(new string[] {
			"ProjectDream",
			"ProjectDream/Variant_Platforming",
			"ProjectDream/Variant_Combat",
			"ProjectDream/Variant_Combat/AI",
			"ProjectDream/Variant_SideScrolling",
			"ProjectDream/Variant_SideScrolling/Gameplay",
			"ProjectDream/Variant_SideScrolling/AI",
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
