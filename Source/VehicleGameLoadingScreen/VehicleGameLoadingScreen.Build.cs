// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

// This module must be loaded "PreLoadingScreen" in the .uproject file, otherwise it will not hook in time!

public class VehicleGameLoadingScreen : ModuleRules
{
    public VehicleGameLoadingScreen(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateIncludePaths.Add("../../VehicleGame/Source/VehicleGameLoadingScreen/Private");

        PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
			}
		);

        PrivateDependencyModuleNames.AddRange(
			new string[] {
				"MoviePlayer",
				"Slate",
				"SlateCore",
				"InputCore"
			}
		);
	}
}
