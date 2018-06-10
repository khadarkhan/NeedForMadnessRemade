// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "VehicleTypes.h"
#include "VehicleHUD.generated.h"

class AVehicleTrackPoint;
class SWeakWidget;
class SVehicleMenuWidget;
class SVehicleHUDWidget;
class SVehicleControlsSetup;

namespace EVehicleGameMenu
{
	enum Type
	{
		RestartTrack,
		RemapControls,
		RemapController,
		ChangeQuality,
		ReturnToMainMenu,
		Quit,
	};
};

namespace ETextAlignH
{
	enum Type
	{
		Left,
		Center,
		Right
	};
};

namespace ETextAlignV
{
	enum Type
	{
		Top,
		Center,
		Bottom
	};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHighscoreNameAccepted, const FString&, NewHighscoreName);

UCLASS()
class AVehicleHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

	/** in game menu pointer */
	MenuPtr MainMenu;

	/** controls settings menu */
	MenuPtr ControlsMenu;

	/** main HUD update loop */
	virtual void DrawHUD() override;

protected:
	virtual void BeginPlay() override;

public:
	/** shows/hides in game menu */
	void ToggleGameMenu();

	/** returns if game menu is currently active */
	bool IsGameMenuUp() const;

	/** enables/disables game HUD display */
	void EnableHUD(bool bEnable);

protected:

	/** if game HUD should be drawn */
	bool bDrawHUD;

	/** if game menu is currently opened*/
	bool bIsGameMenuUp;

	/** last notified track point */
	TWeakObjectPtr<AVehicleTrackPoint> LastTrackPoint;

	/** game menu container widget - used for removing */
	TSharedPtr<SWeakWidget> GameMenuContainer;

	/** HUD menu widget */
	TSharedPtr<SVehicleMenuWidget> MyHUDMenuWidget;

	/** HUD widget */
	TSharedPtr<SVehicleHUDWidget> VehicleHUDWidget;

	/** In game controls remapping widget */
	TSharedPtr<SVehicleControlsSetup> ControlsSetupWidget;

	/** graphics quality menu item */
	FVehicleMenuItem* QualityMenuItem;

	/** quality descriptions list */
	TArray<FText> LowHighList;

	/** called when a hit box is clicked on. Provides the name associated with that box. */
	virtual void NotifyHitBoxClick(FName BoxName) override;

	/** menu callback */
	void ExecuteMenuAction(EVehicleGameMenu::Type Action);

	/** creates in game menu */
	void BuildMenuWidgets();

	/** removes widget from viewport */
	void DetachGameMenu();
	
	/** restarts track */
	void RestartTrack();

	/** returns to main menu */
	void ReturnToMenu();

	/** quits the game */
	void Quit();

	/** Used to display debug/helper messages eg Server/Client. */
	void DrawDebugInfoString(const FString& Text, float PosX, float PosY, bool bAlignLeft, bool bAlignTop, const FColor& TextColor);

	/** speed meter material default instance reference */
	UPROPERTY()
	UMaterialInstanceConstant* SpeedMeterMaterialConst;

	/** speed meter material with current parameters */
	UPROPERTY()
	UMaterialInstanceDynamic* SpeedMeterMaterial;

	/** font used in HUD */
	UPROPERTY()
	UFont* HUDFont;

	/** timer background */
	UPROPERTY()
	UTexture2D* TimerBackground;

	/** place background */
	UPROPERTY()
	UTexture2D* PlaceBackground;

	/** current highscore name */
	TArray<char> HighScoreName;

	/** current letter to change while entering highscore name */
	uint8 CurrentLetter;

	/* if we should show enter name prompt */
	uint8 bEnterNamePromptActive : 1;

	/** up button texture */
	UTexture2D* UpButtonTexture;

	/** down button texture*/
	UTexture2D* DownButtonTexture;

	/** UI Scale */
	float UIScale;
};
