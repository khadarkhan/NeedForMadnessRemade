// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleGame.h"
#include "VehicleMenuWidgetStyle.h"

FVehicleMenuStyle::FVehicleMenuStyle()
{
}

FVehicleMenuStyle::~FVehicleMenuStyle()
{
}

const FName FVehicleMenuStyle::TypeName(TEXT("FVehicleMenuStyle"));

const FVehicleMenuStyle& FVehicleMenuStyle::GetDefault()
{
	static FVehicleMenuStyle Default;
	return Default;
}

void FVehicleMenuStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
}


UVehicleMenuWidgetStyle::UVehicleMenuWidgetStyle( const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer)
{
	
}
