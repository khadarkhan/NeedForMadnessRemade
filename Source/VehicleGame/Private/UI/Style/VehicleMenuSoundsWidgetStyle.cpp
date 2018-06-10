// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleGame.h"
#include "VehicleMenuSoundsWidgetStyle.h"

FVehicleMenuSoundsStyle::FVehicleMenuSoundsStyle()
{
}

FVehicleMenuSoundsStyle::~FVehicleMenuSoundsStyle()
{
}

const FName FVehicleMenuSoundsStyle::TypeName(TEXT("FVehicleMenuSoundsStyle"));

const FVehicleMenuSoundsStyle& FVehicleMenuSoundsStyle::GetDefault()
{
	static FVehicleMenuSoundsStyle Default;
	return Default;
}

void FVehicleMenuSoundsStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
}


UVehicleMenuSoundsWidgetStyle::UVehicleMenuSoundsWidgetStyle( const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer)
{
	
}
