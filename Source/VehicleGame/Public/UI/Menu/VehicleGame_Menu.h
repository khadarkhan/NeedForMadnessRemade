// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "VehicleGame_Menu.generated.h"

class AController;

UCLASS()
class AVehicleGame_Menu : public AGameModeBase
{
	GENERATED_UCLASS_BODY()

	/** skip it, menu doesn't require player start or pawn */
	virtual void RestartPlayer(AController* NewPlayer) override;
};
