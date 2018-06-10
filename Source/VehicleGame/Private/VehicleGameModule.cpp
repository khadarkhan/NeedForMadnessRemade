// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleGame.h"
#include "VehicleStyle.h"
#include "VehicleMenuSoundsWidgetStyle.h"
#include "VehicleMenuWidgetStyle.h"
#include "VehicleMenuItemWidgetStyle.h"



class FVehicleGameModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
		//Hot reload hack
		FSlateStyleRegistry::UnRegisterSlateStyle(FVehicleStyle::GetStyleSetName());
		FVehicleStyle::Initialize();
	}

	virtual void ShutdownModule() override
	{
		FVehicleStyle::Shutdown();
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE( FVehicleGameModule, VehicleGame, "VehicleGame" );

DEFINE_LOG_CATEGORY(LogVehicle)
