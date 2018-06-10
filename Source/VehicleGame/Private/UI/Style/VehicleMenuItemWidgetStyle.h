// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateWidgetStyleContainerBase.h"
#include "VehicleMenuItemWidgetStyle.generated.h"

/**
 * Represents the the appearance of an SVehicleMenuItem
 */
USTRUCT()
struct FVehicleMenuItemStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FVehicleMenuItemStyle();
	virtual ~FVehicleMenuItemStyle();

	// FSlateWidgetStyle
	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };
	static const FVehicleMenuItemStyle& GetDefault();

	/**
	 * The brush used to draw the menu item background
	 */	
	UPROPERTY(EditAnywhere, Category=Appearance)
	FSlateBrush BackgroundBrush;
	FVehicleMenuItemStyle& SetBackgroundBrush(const FSlateBrush& InBackgroundBrush) { BackgroundBrush = InBackgroundBrush; return *this; }

	/**
	 * The brush used to draw the menu highlight
	 */	
	UPROPERTY(EditAnywhere, Category=Appearance)
	FSlateBrush HighlightBrush;
	FVehicleMenuItemStyle& SetHighlightBrush(const FSlateBrush& InHighlightBrush) { HighlightBrush = InHighlightBrush; return *this; }

	/**
	 * The style used for the controls list table row
	 */	
	UPROPERTY(EditAnywhere, Category=Appearance)
	FTableRowStyle ControlsListTableRowStyle;
	FVehicleMenuItemStyle& SetControlsListTableRowStyle(const FTableRowStyle& InControlsListTableRowStyle) { ControlsListTableRowStyle = InControlsListTableRowStyle; return *this; }
};


/**
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UVehicleMenuItemWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_UCLASS_BODY()

public:
	/** The actual data describing the menu item's appearance. */
	UPROPERTY(Category=Appearance, EditAnywhere, meta=(ShowOnlyInnerProperties))
	FVehicleMenuItemStyle MenuItemStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast< const struct FSlateWidgetStyle* >( &MenuItemStyle );
	}
};
