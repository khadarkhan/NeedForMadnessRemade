// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateWidgetStyleContainerBase.h"
#include "VehicleMenuWidgetStyle.generated.h"

/**
 * Represents the the appearance of an SVehicleMenuWidget
 */
USTRUCT()
struct FVehicleMenuStyle : public FSlateWidgetStyle
{
	GENERATED_USTRUCT_BODY()

	FVehicleMenuStyle();
	virtual ~FVehicleMenuStyle();

	// FSlateWidgetStyle
	virtual void GetResources(TArray<const FSlateBrush*>& OutBrushes) const override;
	static const FName TypeName;
	virtual const FName GetTypeName() const override { return TypeName; };
	static const FVehicleMenuStyle& GetDefault();

	/**
	 * The sound that should play when entering a sub-menu
	 */	
	UPROPERTY(EditAnywhere, Category=Sound)
	FSlateSound MenuEnterSound;
	FVehicleMenuStyle& SetMenuEnterSound(const FSlateSound& InMenuEnterSound) { MenuEnterSound = InMenuEnterSound; return *this; }

	/**
	 * The sound that should play when leaving a sub- menu
	 */	
	UPROPERTY(EditAnywhere, Category=Sound)
	FSlateSound MenuExitSound;
	FVehicleMenuStyle& SetMenuExitSound(const FSlateSound& InMenuExitSound) { MenuExitSound = InMenuExitSound; return *this; }
};


/**
 */
UCLASS(hidecategories=Object, MinimalAPI)
class UVehicleMenuWidgetStyle : public USlateWidgetStyleContainerBase
{
	GENERATED_UCLASS_BODY()

public:
	/** The actual data describing the menu's appearance. */
	UPROPERTY(Category=Appearance, EditAnywhere, meta=(ShowOnlyInnerProperties))
	FVehicleMenuStyle MenuStyle;

	virtual const struct FSlateWidgetStyle* const GetStyle() const override
	{
		return static_cast< const struct FSlateWidgetStyle* >( &MenuStyle );
	}
};
