// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "VehicleTypes.h"
#include "VehicleGameUserSettings.generated.h"

UCLASS()
class UVehicleGameUserSettings : public UGameUserSettings
{
	GENERATED_UCLASS_BODY()

	/** Applies all current user settings to the game and saves to permanent storage (e.g. file), optionally checking for command line overrides. */
	virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;


	/** Checks if any user settings is different from current */
	virtual bool IsDirty() const;

	/** Is the mouse inverted? */
	bool GetInvertedMouse() const
	{
		return bInvertedMouse;
	}

	/** Setter for the invert mouse */
	void SetInvertedMouse(bool bInvert)
	{
		bInvertedMouse = bInvert;
	}

	/** Getter for the mouse sensitivity */
	float GetMouseSensitivity() const
	{
		return MouseSensitivity;
	}

	void SetMouseSensitivity(float InSensitivity)
	{
		MouseSensitivity = InSensitivity;
	}

	int32 GetGraphicsQuality() const
	{
		return GraphicsQuality;
	}

	void SetGraphicsQuality(int32 InGraphicsQuality)
	{
		GraphicsQuality = InGraphicsQuality;
	}

	/** Checks if the Mouse Sensitivity user setting is different from current */
	bool IsMouseSensitivityDirty() const;

	/** Checks if the Inverted Mouse user setting is different from current */
	bool IsInvertedMouseDirty() const;

	/** This function resets all settings to the current system settings */
	virtual void ResetToCurrentSettings() override;

	// interface UGameUserSettings
	virtual void SetToDefaults() override;

	void SetLowQuality();
	void SetHighQuality();

private:
	/** Is out mouse inverted or not? */
	UPROPERTY(config)
	bool bInvertedMouse;

	/** Holds the mouse sensitivity */
	UPROPERTY(config)
	float MouseSensitivity;

	/**
	 * Graphics Quality
	 *	0 = Low
	 *	1 = High
	 */
	UPROPERTY(config)
	int32 GraphicsQuality;

public:
	/** Defines friendly names for action mappings */
	UPROPERTY(config)
	TArray<FKeybindingUIConfig> KeybindingsUIConfig;
};