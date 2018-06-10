// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleTypes.generated.h"
#pragma once


typedef TSharedPtr<TArray<class FVehicleMenuItem>> MenuPtr;

USTRUCT()
struct FKeybindingUIConfig
{
	GENERATED_USTRUCT_BODY()

	/** action name if mapping action */
	UPROPERTY()
	FName ActionName;

	/** axis name if mapping axis to key */
	UPROPERTY()
	FName AxisName;

	/** Key to bind it to. */
	UPROPERTY()
	FKey Key;

	/** axis scale is required to find proper mapping */
	UPROPERTY()
	float Scale;

	/** name to display in user interface */
	UPROPERTY()
	FText DisplayName;

	/** group name */
	UPROPERTY()
	FName GroupName;
	
	/** Key must have assigned value */
	FKeybindingUIConfig()
	{
	}
};


/** keep in sync with VehicleImpactEffect, VehicleWheelEffect  */
/** This enum type is deprecated, so please do not use this **/
UENUM()
namespace EPhysMaterialType
{
	enum Type
	{
		Unknown,
		Asphalt,
		Dirt,
		Water,
		Wood,
		Stone,
		Metal,
	};
}

/** When you add new types, make sure you add to 
 *	[/Script/Engine.PhysicsSettings] section DefaultEngine.INI 
 */
#define VEHICLE_SURFACE_Default		SurfaceType_Default
#define VEHICLE_SURFACE_Asphalt		SurfaceType1
#define VEHICLE_SURFACE_Dirt		SurfaceType2
#define VEHICLE_SURFACE_Water		SurfaceType3
#define VEHICLE_SURFACE_Wood		SurfaceType4
#define VEHICLE_SURFACE_Stone		SurfaceType5
#define VEHICLE_SURFACE_Metal		SurfaceType6
#define VEHICLE_SURFACE_Grass		SurfaceType7
#define VEHICLE_SURFACE_Gravel		SurfaceType8

