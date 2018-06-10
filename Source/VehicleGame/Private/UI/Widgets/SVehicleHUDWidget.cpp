// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleGame.h"
#include "SVehicleHUDWidget.h"
#include "VehicleStyle.h"
#include "VehicleGameMode.h"

#define LOCTEXT_NAMESPACE "VehicleGame.HUD"

void SVehicleHUDWidget::Construct(const FArguments& InArgs)
{
	FSlateFontInfo StartMessageFontStyle("UI_Vehicle_Font", 32);

	OwnerWorld = InArgs._OwnerWorld;

	ChildSlot
	.VAlign(VAlign_Fill)
	.HAlign(HAlign_Fill)
	[
		SNew(SOverlay)
		+SOverlay::Slot() // game menu background overlay
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SOverlay)
			+SOverlay::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(SBorder)
				.Visibility(this, &SVehicleHUDWidget::GetInfoTextVisibility)
				.BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
				[
					SNew(STextBlock)
					.TextStyle(FVehicleStyle::Get(), "VehicleGame.MenuTextStyle")
					.ColorAndOpacity(this,&SVehicleHUDWidget::GetInfoTextColor)
					.Text(this,&SVehicleHUDWidget::GetInfoText)
					.Font(StartMessageFontStyle)
				]
			]
		]
	];
}

FText SVehicleHUDWidget::GetInfoText() const
{
	AVehicleGameMode* const MyGame = OwnerWorld->GetAuthGameMode<AVehicleGameMode>();
	return MyGame->GetGameInfoText();
}

EVisibility SVehicleHUDWidget::GetInfoTextVisibility() const
{
	AVehicleGameMode* const MyGame = OwnerWorld->GetAuthGameMode<AVehicleGameMode>();
	return (MyGame && !MyGame->GetGameInfoText().IsEmpty()) ? EVisibility::Visible : EVisibility::Collapsed;
}

FSlateColor SVehicleHUDWidget::GetInfoTextColor() const
{
	const float AnimSpeedModifier = 0.8f;
	float Alpha = FMath::Abs(FMath::Sin(OwnerWorld->GetTimeSeconds())*AnimSpeedModifier);
	return FLinearColor(1,1,1,Alpha);
}

#undef LOCTEXT_NAMESPACE