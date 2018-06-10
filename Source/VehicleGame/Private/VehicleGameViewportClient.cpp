// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleGame.h"
#include "VehicleGameViewportClient.h"

UVehicleGameViewportClient::UVehicleGameViewportClient(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetSuppressTransitionMessage(true);
}

#if WITH_EDITOR
void UVehicleGameViewportClient::DrawTransition(UCanvas* Canvas)
{
	if (GetOuterUEngine() != nullptr)
	{
		TEnumAsByte<enum ETransitionType> Type = GetOuterUEngine()->TransitionType;
		switch (Type)
		{
		case TT_Connecting:
			DrawTransitionMessage(Canvas, NSLOCTEXT("GameViewportClient", "ConnectingMessage", "CONNECTING").ToString());
			break;
		case TT_WaitingToConnect:
			DrawTransitionMessage(Canvas, NSLOCTEXT("GameViewportClient", "Waitingtoconnect", "Waiting to connect...").ToString());
			break;
		}
	}
}
#endif //WITH_EDITOR