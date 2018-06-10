// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleGame.h"
#include "VehicleStyle.h"
#include "SlateGameResources.h"

TSharedPtr< FSlateStyleSet > FVehicleStyle::VehicleStyleInstance = nullptr;

void FVehicleStyle::Initialize()
{
	if ( !VehicleStyleInstance.IsValid() )
	{
		VehicleStyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle( *VehicleStyleInstance );
	}
}

void FVehicleStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle( *VehicleStyleInstance );
	ensure( VehicleStyleInstance.IsUnique() );
	VehicleStyleInstance.Reset();
}

FName FVehicleStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("VehicleStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( FPaths::ProjectContentDir() / "Slate"/ RelativePath + TEXT(".png"), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( FPaths::ProjectContentDir() / "Slate"/ RelativePath + TEXT(".png"), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( FPaths::ProjectContentDir() / "Slate"/ RelativePath + TEXT(".png"), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( FPaths::ProjectContentDir() / "Slate"/ RelativePath + TEXT(".ttf"), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( FPaths::ProjectContentDir() / "Slate"/ RelativePath + TEXT(".otf"), __VA_ARGS__ )

TSharedRef< FSlateStyleSet > FVehicleStyle::Create()
{
	TSharedRef<FSlateStyleSet> StyleRef = FSlateGameResources::New(FVehicleStyle::GetStyleSetName(), "/Game/UI/Styles", "/Game/UI/Styles");
	FSlateStyleSet& Style = StyleRef.Get();

	// Fonts still need to be specified in code for now
	Style.Set("VehicleGame.MenuTextStyle", FTextBlockStyle()
		.SetFont(TTF_FONT("Fonts/Roboto-Black", 20))
		.SetColorAndOpacity(FLinearColor::White)
		.SetShadowColorAndOpacity(FLinearColor::Black)
		.SetShadowOffset(FIntPoint(-1,1))
		);

	return StyleRef;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FVehicleStyle::ReloadTextures()
{
	FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const ISlateStyle& FVehicleStyle::Get()
{
	return *VehicleStyleInstance;
}
