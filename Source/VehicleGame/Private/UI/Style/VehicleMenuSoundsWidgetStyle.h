// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateWidgetStyleContainerBase.h"
#include "VehicleMenuSoundsWidgetStyle.generated.h"

/**
 * Represents the common menu sounds used in the vehicle game
 */
USTRUCT()
struct FVehicleMenuSoundsStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FVehicleMenuSoundsStyle();
	virtual ~FVehicleMenuSoundsStyle();

	// FSlateWidgetStyle
	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };
	static const FVehicleMenuSoundsStyle& GetDefault();

	/**
	 * The sound that should play when starting the game
	 */	
	UPROPERTY(EditAnywhere, Category=Sound)
	FSlateSound StartGameSound;
	FVehicleMenuSoundsStyle& SetStartGameSound(const FSlateSound& InStartGameSound) { StartGameSound = InStartGameSound; return *this; }

	/**
	 * The sound that should play when exiting the game
	 */	
	UPROPERTY(EditAnywhere, Category=Sound)
	FSlateSound ExitGameSound;
	FVehicleMenuSoundsStyle& SetExitGameSound(const FSlateSound& InExitGameSound) { ExitGameSound = InExitGameSound; return *this; }

	/**
	 * The sound that should play when accepting UI changes
	 */	
	UPROPERTY(EditAnywhere, Category=Sound)
	FSlateSound AcceptChangesSound;
	FVehicleMenuSoundsStyle& SetAcceptChangesSound(const FSlateSound& InAcceptChangesSound) { AcceptChangesSound = InAcceptChangesSound; return *this; }

	/**
	 * The sound that should play when discarding UI changes
	 */	
	UPROPERTY(EditAnywhere, Category=Sound)
	FSlateSound DiscardChangesSound;
	FVehicleMenuSoundsStyle& SetDiscardChangesSound(const FSlateSound& InDiscardChangesSound) { DiscardChangesSound = InDiscardChangesSound; return *this; }

	/**
	 * The sound that should play when changing the selected menu item
	 */	
	UPROPERTY(EditAnywhere, Category=Sound)
	FSlateSound MenuItemChangeSound;
	FVehicleMenuSoundsStyle& SetMenuItemChangeSound(const FSlateSound& InMenuItemChangeSound) { MenuItemChangeSound = InMenuItemChangeSound; return *this; }
};


/**
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UVehicleMenuSoundsWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_UCLASS_BODY()

public:
	/** The actual data describing the sounds */
	UPROPERTY(Category=Appearance, EditAnywhere, meta=(ShowOnlyInnerProperties))
	FVehicleMenuSoundsStyle SoundsStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast< const struct FSlateWidgetStyle* >( &SoundsStyle );
	}
};
