// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleGame.h"
#include "VehicleGameMode.h"
#include "Track/VehicleTrackPoint.h"
#include "Player/VehiclePlayerController.h"
#include "VehicleGameState.h"
#include "Landscape.h"

AVehicleGameMode::AVehicleGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	RaceStartTime = 0;
	RaceFinishTime = 0;	
	bLockingActive = false;

	GameStateClass = AVehicleGameState::StaticClass();
	if ((GEngine != nullptr ) && ( GEngine->GameViewport != nullptr))
	{
		GEngine->GameViewport->SetSuppressTransitionMessage( true );
	}

	PrimaryActorTick.bCanEverTick = true;
}

void AVehicleGameMode::StartPlay()
{
	Super::StartPlay();

	EnablePlayerLocking();
}

AActor* AVehicleGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	APlayerStart* BestStart = nullptr;

	// find first non occupied start
	for (TActorIterator<APlayerStart> PlayerStartIt(GetWorld()); PlayerStartIt; ++PlayerStartIt)
	{
		APlayerStart* TestSpot = *PlayerStartIt;

		bool bBlocked = false;
		for (FConstPawnIterator PawnIt = GetWorld()->GetPawnIterator(); PawnIt; ++PawnIt)
		{
			if (*PawnIt != nullptr && ((*PawnIt)->GetActorLocation() - TestSpot->GetActorLocation()).SizeSquared2D() < 10000)
			{
				bBlocked = true;
				break;
			}
		}
		
		if (!bBlocked)
		{
			BestStart = TestSpot;
			break;
		}
	}

	return BestStart ? BestStart : Super::ChoosePlayerStart_Implementation(Player);
}

APawn* AVehicleGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, class AActor* StartSpot)
{
	check(StartSpot);
	bool bGotStart = false;
	//APawn* NewPawn = Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
	FVector StartLocation = StartSpot->GetActorLocation();
	FRotator StartRotation(ForceInit);
	StartRotation.Yaw = StartSpot->GetActorRotation().Yaw;
	FRotator EachRot = StartRotation;
	const FVector TraceOffset(0, 0, 250);
	const float CheckSize = 600.0f;

	// Check the respawn for a valid position. Initially check the first position then 4 directions from there
	for (int32 iAngle = 0; iAngle < 5; iAngle++)
	{
		FVector NewLocation = StartLocation;

		FVector RotationOffset = iAngle == 0 ? FVector::ZeroVector : FVector::ForwardVector;
		RotationOffset *= CheckSize;
		FVector NewOff = EachRot.RotateVector(RotationOffset);

		const FVector NewPos = StartLocation + NewOff;
		const FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(SpawnTrace), true);
		const FVector TraceStart = NewPos + TraceOffset;
		const FVector TraceEnd = NewPos - TraceOffset;
		FHitResult Hit;

		GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Vehicle, TraceParams);

		if (Hit.bBlockingHit == true)
		{
			ALandscape* LandObject = Cast<ALandscape>(Hit.Actor.Get());
			if (LandObject != nullptr)
			{
				StartLocation = NewPos;
				bGotStart = true;
				break;
			}
		}
		EachRot.Yaw += 90.0f;
	}

	// Move the spawn Z up a little so we drop onto the track
	StartLocation.Z += 150.0f;
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = Instigator;
	APawn* ResultPawn = GetWorld()->SpawnActor<APawn>(GetDefaultPawnClassForController(NewPlayer), StartLocation, StartRotation, SpawnInfo);
	check(ResultPawn != nullptr);
	return ResultPawn;
}

void AVehicleGameMode::PostLogin(APlayerController* NewPlayer)
{
	AVehiclePlayerController* VehiclePC = Cast<AVehiclePlayerController>(NewPlayer);
	if (VehiclePC)
	{
		VehiclePC->SetHandbrakeForced(bLockingActive && !IsRaceActive());
		AVehicleGameState* VehicleGameState = GetVehicleGameState();
		if (VehicleGameState != nullptr)
		{
			VehicleGameState->NumRacers++;
		}
	}

	Super::PostLogin(NewPlayer);
}

void AVehicleGameMode::BroadcastRaceState()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AVehiclePlayerController* VehiclePC = Cast<AVehiclePlayerController>(*It);
		if (VehiclePC)
		{
			VehiclePC->SetHandbrakeForced(bLockingActive && !IsRaceActive());
		}
	}
}

void AVehicleGameMode::StartRace()
{
	if (!IsRaceActive())
	{
		AVehicleGameState* VehicleGameState = GetGameState<AVehicleGameState>();
		if (VehicleGameState != nullptr)
		{			
			VehicleGameState->bIsRaceActive = true;
		}
		RaceStartTime = GetWorld()->GetTimeSeconds();
		BroadcastRaceState();
	}
}

void AVehicleGameMode::FinishRace()
{
	if (IsRaceActive())
	{
		AVehicleGameState* VehicleGameState = GetGameState<AVehicleGameState>();
		if (VehicleGameState != nullptr)
		{
			VehicleGameState->bIsRaceActive = false;
		}
		RaceFinishTime = GetWorld()->GetTimeSeconds();
		BroadcastRaceState();
	}
}

void AVehicleGameMode::Tick(float DeltaSeconds)
{
	AVehicleGameState* VehicleGameState = GetGameState<AVehicleGameState>();
	if (VehicleGameState != nullptr)
	{
		const float CurrentTime = IsRaceActive() ? GetWorld()->GetTimeSeconds() : RaceFinishTime;
		VehicleGameState->TotalTime = CurrentTime - RaceStartTime;
	}
}

void AVehicleGameMode::EnablePlayerLocking()
{
	bLockingActive = true;
	BroadcastRaceState();
}

void AVehicleGameMode::SetGamePaused(bool bIsPaused)
{
	AVehiclePlayerController* const MyPlayer = Cast<AVehiclePlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	if (MyPlayer != nullptr)
	{
		// Cache the current pause state
		bool bWasPaused = MyPlayer->IsPaused();	
		// Apply new state
		MyPlayer->SetPause(bIsPaused);
	}
	
}

bool AVehicleGameMode::IsRaceActive() const
{
	AVehicleGameState* VehicleGameState = GetGameState<AVehicleGameState>();
	bool bIsActive = false;
	if (VehicleGameState != nullptr)
	{
		bIsActive = VehicleGameState->bIsRaceActive;
	}
	return bIsActive;
}

bool AVehicleGameMode::HasRaceStarted() const
{
	return (RaceStartTime > 0.0f);
}

bool AVehicleGameMode::HasRaceFinished() const
{
	return HasRaceStarted() && !IsRaceActive();
}

float AVehicleGameMode::GetRaceTimer() const
{
	// Return the time from the game state
	AVehicleGameState* VehicleGameState = GetGameState<AVehicleGameState>();
	if (VehicleGameState != nullptr)
	{
		return VehicleGameState->TotalTime;
	}
	// We shouldn't really not have a game state but just in case
	const float CurrentTime = IsRaceActive() ? GetWorld()->GetTimeSeconds() : RaceFinishTime;
	return CurrentTime - RaceStartTime;
}

AVehicleGameState* AVehicleGameMode::GetVehicleGameState() const
{
	return GetGameState<AVehicleGameState>();
}


void AVehicleGameMode::SetGameInfoText(const FText& InText)
{
	GameInfoText = InText;
}

const FText& AVehicleGameMode::GetGameInfoText() const
{
	return GameInfoText;
}


AActor* AVehicleGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	AVehiclePlayerController* VehicleController = Cast<AVehiclePlayerController>(Player);
	AActor* RestartSpot = nullptr;
	if ((VehicleController != nullptr) && (VehicleController->LastTrackPoint != nullptr))
	{
		RestartSpot = Cast<AActor>(VehicleController->LastTrackPoint);
	}
	if (RestartSpot == nullptr)
	{
		RestartSpot = Super::FindPlayerStart_Implementation(Player, IncomingName);		
	}
	return RestartSpot;
}


