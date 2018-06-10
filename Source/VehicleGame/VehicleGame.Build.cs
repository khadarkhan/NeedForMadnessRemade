// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VehicleGame : ModuleRules
{
	public VehicleGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"Landscape",
                "PhysXVehicles"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"InputCore",
				"Slate",
				"SlateCore",
				"VehicleGameLoadingScreen",
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
				"VehicleGame/Private/UI/Widgets",
				"VehicleGame/Private/UI/Style",
			}
		);
	}
}
