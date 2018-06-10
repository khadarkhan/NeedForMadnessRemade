// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleGame.h"
#include "VehicleMenuItemWidgetStyle.h"

FVehicleMenuItemStyle::FVehicleMenuItemStyle()
{
}

FVehicleMenuItemStyle::~FVehicleMenuItemStyle()
{
}

const FName FVehicleMenuItemStyle::TypeName(TEXT("FVehicleMenuItemStyle"));

const FVehicleMenuItemStyle& FVehicleMenuItemStyle::GetDefault()
{
	static FVehicleMenuItemStyle Default;
	return Default;
}

void FVehicleMenuItemStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
	OutBrushes.Add(&BackgroundBrush);
	OutBrushes.Add(&HighlightBrush);

	ControlsListTableRowStyle.GetResources(OutBrushes);
}


UVehicleMenuItemWidgetStyle::UVehicleMenuItemWidgetStyle( const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer)
{
	
}
