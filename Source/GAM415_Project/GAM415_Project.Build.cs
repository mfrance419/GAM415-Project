// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GAM415_Project : ModuleRules
{
	public GAM415_Project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
