// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

class AVehiclePlayerController;

//HUD widget base class
class SVehicleHUDWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SVehicleHUDWidget)
	{}

	SLATE_ARGUMENT(TWeakObjectPtr<class UWorld>, OwnerWorld)

	SLATE_END_ARGS()

	/** Needed for every widget */
	void Construct(const FArguments& InArgs);

protected:
	EVisibility GetInfoTextVisibility() const;
	FSlateColor GetInfoTextColor() const;
	FText		GetInfoText() const;

	AVehiclePlayerController* PCOwner;

	/** Pointer to our parent World */
	TWeakObjectPtr<UWorld> OwnerWorld;
};


