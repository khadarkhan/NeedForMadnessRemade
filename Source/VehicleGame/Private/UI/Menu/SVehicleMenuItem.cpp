// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleGame.h"
#include "SVehicleMenuItem.h"
#include "VehicleStyle.h"
#include "VehicleMenuItemWidgetStyle.h"

void SVehicleMenuItem::Construct(const FArguments& InArgs)
{
	MenuItemStyle = &FVehicleStyle::Get().GetWidgetStyle<FVehicleMenuItemStyle>("DefaultVehicleMenuItemStyle");

	OwnerHUD = InArgs._OwnerHUD;
	Text = InArgs._Text;
	OnClicked = InArgs._OnClicked;
	bIsActiveMenuItem = false;
	//if attribute is set, use its value, otherwise uses default
	InactiveTextAlpha = InArgs._InactiveTextAlpha.Get(1.0f);

	ChildSlot
	.VAlign(VAlign_Fill)
	.HAlign(HAlign_Fill)
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SBox)
			.WidthOverride(374.0f)
			.HeightOverride(23.0f)
			[
				SNew(SImage)
				.Image(&MenuItemStyle->BackgroundBrush)
			]
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SBox)
			.WidthOverride(374.0f)
			.HeightOverride(23.0f)
			[
				SNew(SImage)
				.ColorAndOpacity(this,&SVehicleMenuItem::GetButtonBgColor)
				.Image(&MenuItemStyle->HighlightBrush)
			]
		]
		+SOverlay::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(FMargin(40.0f,0,0,0))
		[
			SNew(STextBlock)
			.TextStyle(FVehicleStyle::Get(), "VehicleGame.MenuTextStyle")
			.ColorAndOpacity(this,&SVehicleMenuItem::GetButtonTextColor)
			.Text(Text)
		]
		
	];
}



FSlateColor SVehicleMenuItem::GetButtonTextColor() const
{
	FLinearColor ResultColor;
	if (bIsActiveMenuItem)
	{
		ResultColor = MenuItemStyle->ControlsListTableRowStyle.SelectedTextColor.GetSpecifiedColor();
	} 
	else
	{
		ResultColor = MenuItemStyle->ControlsListTableRowStyle.TextColor.GetSpecifiedColor();
		ResultColor.A *= InactiveTextAlpha;
	}
	return ResultColor;
}

FSlateColor SVehicleMenuItem::GetButtonBgColor() const
{
	float BgAlpha;
	const float MinAlpha = 0.7f;
	const float MaxAlpha = 1.0f;
	const float AnimSpeedModifier = 1.5f;
	float GameTime = OwnerHUD->GetWorld()->GetRealTimeSeconds();
	float AnimPercent = FMath::Abs(FMath::Sin(GameTime*AnimSpeedModifier));
	if (bIsActiveMenuItem)
	{
		BgAlpha = FMath::Lerp(MinAlpha, MaxAlpha, AnimPercent);
	} 
	else
	{
		BgAlpha = 0.0f;
	}
	return FLinearColor(1,1,1,BgAlpha);
}

FReply SVehicleMenuItem::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	//execute our "OnClicked" delegate, if we have one
	if(OnClicked.IsBound() == true)
	{
		return OnClicked.Execute();
	}

	return FReply::Handled();
}


FReply SVehicleMenuItem::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Handled();
}

FReply SVehicleMenuItem::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) 
{
	return FReply::Unhandled();
}

void SVehicleMenuItem::SetMenuItemActive(bool bIsMenuItemActive)
{
	this->bIsActiveMenuItem = bIsMenuItemActive;
}
