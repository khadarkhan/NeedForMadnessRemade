// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleGame.h"
#include "UI/Menu/VehicleGame_Menu.h"
#include "UI/Menu/VehicleHUD_Menu.h"

AVehicleGame_Menu::AVehicleGame_Menu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	HUDClass = AVehicleHUD_Menu::StaticClass();
}

void AVehicleGame_Menu::RestartPlayer(AController* NewPlayer)
{
	// don't restart
}
