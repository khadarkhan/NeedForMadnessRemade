// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "VehicleBlueprintLibrary.generated.h"

UCLASS()
class UVehicleBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/** Finishes the race */
	UFUNCTION(BlueprintCallable, Category=Game, meta=(WorldContext="WorldContextObject"))
	static void FinishRace(UObject* WorldContextObject);

	/** Shows blinking message at the bottom of the screen */
	UFUNCTION(BlueprintCallable, Category=Game, meta=(WorldContext="WorldContextObject"))
	static void SetInfoText(UObject* WorldContextObject, FString InfoText);

	/** Hides info message */
	UFUNCTION(BlueprintCallable, Category=Game, meta=(WorldContext="WorldContextObject"))
	static void HideInfoText(UObject* WorldContextObject);

	/** shows game menu */
	UFUNCTION(BlueprintCallable, Category=HUD, meta=(WorldContext="WorldContextObject"))
	static void ShowGameMenu(UObject* WorldContextObject);

	/** shows/hides game HUD */
	UFUNCTION(BlueprintCallable, Category=HUD, meta=(WorldContext="WorldContextObject"))
	static void ShowHUD(UObject* WorldContextObject, bool bEnable);
};
