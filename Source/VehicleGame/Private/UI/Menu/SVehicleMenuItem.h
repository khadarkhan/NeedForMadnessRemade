// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

//class declare
class SVehicleMenuItem : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SVehicleMenuItem)
	{}

	/** Weak pointer to the parent HUD base */
	SLATE_ARGUMENT(TWeakObjectPtr<AHUD>, OwnerHUD)

	/** Called when the button is clicked */
	SLATE_EVENT(FOnClicked, OnClicked)

	/** menu item text attribute */
	SLATE_ATTRIBUTE(FText, Text)

	/** menu item text transparency when item is not active, optional argument */
	SLATE_ARGUMENT(TOptional<float>, InactiveTextAlpha)

	SLATE_END_ARGS()

	/** Needed for every widget */
	void Construct(const FArguments& InArgs);

	/** Says that we can support keyboard focus */
	virtual bool SupportsKeyboardFocus() const override { return true; }

	/** mouse button down callback */
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	/** mouse button up callback */
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	/** mouse move callback */
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	/** sets this menu item as active (selected) */
	void SetMenuItemActive(bool bIsMenuItemActive);


protected:
	/** The delegate to execute when the button is clicked */
	FOnClicked OnClicked;

private:
	/** menu item text attribute */
	TAttribute< FText > Text;

	/** getter for menu item background color */
	FSlateColor GetButtonBgColor() const;

	/** getter for menu item text color */
	FSlateColor GetButtonTextColor() const;

	/** inactive text alpha value*/
	float InactiveTextAlpha;

	/** active item flag */
	bool bIsActiveMenuItem;

	/** Pointer to our parent HUD */
	TWeakObjectPtr<class AHUD> OwnerHUD;

	/** style for this menu item */
	const struct FVehicleMenuItemStyle *MenuItemStyle;
};


