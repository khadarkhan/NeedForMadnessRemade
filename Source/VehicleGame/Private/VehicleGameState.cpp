// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleGame.h"
#include "VehicleGameState.h"

AVehicleGameState::AVehicleGameState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NumRacers = 0;
	TotalTime = 0;
	bTimerPaused = false;
	bIsRaceActive = false;
	// need to tick when paused to check king state.
	PrimaryActorTick.bCanEverTick = true;
	SetTickableWhenPaused(true);
}

void AVehicleGameState::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );

	DOREPLIFETIME( AVehicleGameState, NumRacers );
	DOREPLIFETIME( AVehicleGameState, TotalTime );
	DOREPLIFETIME( AVehicleGameState, bTimerPaused );
	DOREPLIFETIME( AVehicleGameState, bIsRaceActive );
	
}

float AVehicleGameState::GetTotalTime()
{
	return TotalTime;
}

bool AVehicleGameState::IsRaceActive() const
{
	return bIsRaceActive;
}