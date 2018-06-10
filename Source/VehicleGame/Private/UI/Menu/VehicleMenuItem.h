// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "SVehicleMenuItem.h"

namespace EVehicleMenuItemType
{
	enum Type
	{
		Standard,
		CustomWidget,
	};
};


class FVehicleMenuItem
{
public:
	/** menu item text */
	FText Text;

	/** sub menu if present */
	TSharedPtr< TArray<FVehicleMenuItem> > SubMenu;

	/** menu item type */
	EVehicleMenuItemType::Type MenuItemType;

	/** shared pointer to actual slate widget representing the menu item */
	TSharedPtr<SVehicleMenuItem> Widget;

	/** shared pointer to actual slate widget representing the custom menu item, ie whole options screen */
	TSharedPtr<SWidget> CustomWidget;

	/** delegate declaration */
	DECLARE_DELEGATE(FOnConfirmMenuItem);

	/** delegate, which is executed by SVehicleMenuWidget if user confirms this menu item */
	FOnConfirmMenuItem OnConfirmMenuItem;

	/** constructor accepting menu item text */
	FVehicleMenuItem(FText _Text)
	{
		Text = _Text;
		MenuItemType = EVehicleMenuItemType::Standard;
		SubMenu = MakeShareable(new TArray<FVehicleMenuItem>());
	}

	/** Custom widgets cannot contain sub menus, all functionality must be handled by custom widget itself */
	FVehicleMenuItem(TSharedPtr<SWidget> _Widget)
	{
		MenuItemType = EVehicleMenuItemType::CustomWidget;
		CustomWidget = _Widget;
	}

};

namespace MenuHelper
{
	template< class UserClass >	
	FORCEINLINE FTimerHandle PlaySoundAndCall(UWorld* World, const FSlateSound& Sound, UserClass* inObj, typename FVehicleMenuItem::FOnConfirmMenuItem::TUObjectMethodDelegate< UserClass >::FMethodPtr inMethod)
	{
		FSlateApplication::Get().PlaySound(Sound);

		FTimerHandle Result;
		if (World->IsPaused())
		{
			// just call it right away
			FTimerDelegate::CreateUObject(inObj, inMethod).Execute();
		}
		else
		{
			// timers don't run when game is paused
			const float SoundDuration = FMath::Max(FSlateApplication::Get().GetSoundDuration(Sound), 0.1f);
			World->GetTimerManager().SetTimer(Result, FTimerDelegate::CreateUObject(inObj, inMethod), SoundDuration, false);
		}

		return Result;
	}

	template< class UserClass >	
	FORCEINLINE FTimerHandle PlaySoundAndCallSP(UWorld* World, const FSlateSound& Sound, UserClass* inObj, typename FVehicleMenuItem::FOnConfirmMenuItem::TSPMethodDelegate< UserClass >::FMethodPtr inMethod)
	{
		FSlateApplication::Get().PlaySound(Sound);

		FTimerHandle Result;
		const float SoundDuration = FMath::Max(FSlateApplication::Get().GetSoundDuration(Sound), 0.1f);
		World->GetTimerManager().SetTimer(Result, FTimerDelegate::CreateSP(inObj, inMethod), SoundDuration, false);

		return Result;
	}

	template< class UserClass >	
	FORCEINLINE FTimerHandle PlaySoundAndCallSP(UWorld* World, const FSlateSound& Sound, UserClass* inObj, typename FVehicleMenuItem::FOnConfirmMenuItem::TSPMethodDelegate_Const< UserClass >::FMethodPtr inMethod)
	{
		FSlateApplication::Get().PlaySound(Sound);

		FTimerHandle Result;
		const float SoundDuration = FMath::Max(FSlateApplication::Get().GetSoundDuration(Sound), 0.1f);
		World->GetTimerManager().SetTimer(Result, FTimerDelegate::CreateSP(inObj, inMethod), SoundDuration, false);

		return Result;
	}

	FORCEINLINE float GetSoundPlaybackPosition(UWorld* World, const FSlateSound& Sound, FTimerHandle inHandle)
	{
		const float SoundDuration = FMath::Max(FSlateApplication::Get().GetSoundDuration(Sound), 0.1f);
		return World->GetTimerManager().GetTimerElapsed(inHandle) / SoundDuration;
	}
}
