// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleGame.h"
#include "Engine/Console.h"
#include "SVehicleMenuWidget.h"
#include "VehicleMenuItem.h"
#include "SVehicleMenuItem.h"
#include "VehicleStyle.h"
#include "VehicleMenuSoundsWidgetStyle.h"
#include "VehicleMenuWidgetStyle.h"

#define LOCTEXT_NAMESPACE "SVehicleMenuWidget"

EVisibility SVehicleMenuWidget::GetSlateVisibility() const
{
	return bConsoleVisible ? EVisibility::HitTestInvisible : EVisibility::Visible;
}

FVehicleMenuItem* SVehicleMenuWidget::AddMenuItem(MenuPtr &SubMenu, FText Text)
{
	if (!SubMenu.IsValid())
	{
		SubMenu = MakeShareable(new TArray<FVehicleMenuItem>());
	}
	SubMenu->Add(FVehicleMenuItem(Text));
	return &SubMenu->Last();
}

FVehicleMenuItem* SVehicleMenuWidget::AddCustomWidgetMenuItem(MenuPtr &SubMenu, TSharedPtr<SWidget> CustomWidget)
{
	if (!SubMenu.IsValid())
	{
		SubMenu = MakeShareable(new TArray<FVehicleMenuItem>());
	}
	SubMenu->Add(FVehicleMenuItem(CustomWidget));
	return &SubMenu->Last();
}

void SVehicleMenuWidget::Construct(const FArguments& InArgs)
{
	MenuStyle = &FVehicleStyle::Get().GetWidgetStyle<FVehicleMenuStyle>("DefaultVehicleMenuStyle");

	bConsoleVisible = false;
	SelectedIndex = 0;
	MyMenuHUD = InArgs._MenuHUD;
	bHideBackground = InArgs._HideBackground;
	ControllerHideMenuKey = EKeys::Gamepad_Special_Right;
	Visibility.Bind(this, &SVehicleMenuWidget::GetSlateVisibility);

	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.Padding(TAttribute<FMargin>(this,&SVehicleMenuWidget::GetMenuOffset))
			[
				SNew(SBorder)
				.BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
				.Padding(FMargin(20.0f))
				.DesiredSizeScale(this, &SVehicleMenuWidget::GetBottomScale)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				[
					SNew(SBorder)
					.BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
					.ColorAndOpacity(this, &SVehicleMenuWidget::GetBottomColor)
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Left)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(TAttribute<FMargin>(this,&SVehicleMenuWidget::GetLeftMenuOffset))
						[
							SAssignNew(LeftBox, SVerticalBox)
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(TAttribute<FMargin>(this,&SVehicleMenuWidget::GetSubMenuOffset))
						[
							SAssignNew(RightBox, SVerticalBox)
						]
					]
				]
			]
			
		]
	];
}

void SVehicleMenuWidget::BuildAndShowMenu()
{
	if (!CurrentMenu.IsValid())
	{
		return;
	}

	//Build left menu panel
	bLeftMenuChanging = false;
	bGoingBack = false;
	BuildLeftPanel(bGoingBack);

	//sets up whole main menu animations and launches them
	SetupAnimations();

	// Set up right side and launch animation if there is any submenu
	if (CurrentMenu->Num() > SelectedIndex)
	{
		NextMenu = (*CurrentMenu)[SelectedIndex].SubMenu;
		if (NextMenu.IsValid())
		{
			bSubMenuChanging = true;
		}
	}

	bMenuHiding = false;

	FSlateApplication::Get().PlaySound(MenuStyle->MenuEnterSound);
}

void SVehicleMenuWidget::HideMenu()
{
	if (!bMenuHiding)
	{
		if(MenuWidgetAnimation.IsAtEnd())
		{
			MenuWidgetAnimation.PlayReverse(this->AsShared());
		}
		else
		{
			MenuWidgetAnimation.Reverse();
		}
		bMenuHiding = true;
	}
}


void SVehicleMenuWidget::SetupAnimations()
{
	//Setup a curve
	const float StartDelay = 0.0f;
	const float SecondDelay = bHideBackground ? 0.f : 0.3f;
	const float AnimDuration = 0.5f;
	const float MenuChangeDuration = 0.2f;

	MenuWidgetAnimation = FCurveSequence();
	SubMenuWidgetAnimation = FCurveSequence();
	SubMenuScrollOutCurve = SubMenuWidgetAnimation.AddCurve(0,MenuChangeDuration,ECurveEaseFunction::QuadInOut);

	MenuWidgetAnimation = FCurveSequence();
	LeftMenuWidgetAnimation = FCurveSequence();
	LeftMenuScrollOutCurve = LeftMenuWidgetAnimation.AddCurve(0,MenuChangeDuration,ECurveEaseFunction::QuadInOut);
	LeftMenuWidgetAnimation.Play(this->AsShared());

	//Define the fade in animation
	TopColorCurve = MenuWidgetAnimation.AddCurve(StartDelay, AnimDuration, ECurveEaseFunction::QuadInOut);

	//now, we want these to animate some time later

	//rolling out
	BottomScaleYCurve = MenuWidgetAnimation.AddCurve(StartDelay+SecondDelay, AnimDuration, ECurveEaseFunction::QuadInOut);
	//fading in
	BottomColorCurve = MenuWidgetAnimation.AddCurve(StartDelay+SecondDelay, AnimDuration, ECurveEaseFunction::QuadInOut);
	//moving from left side off screen
	ButtonsPosXCurve = MenuWidgetAnimation.AddCurve(StartDelay+SecondDelay, AnimDuration, ECurveEaseFunction::QuadInOut);
}

void SVehicleMenuWidget::BuildLeftPanel(bool bInGoingBack)
{
	if (!CurrentMenu.IsValid())
	{
		//do not build anything if we do not have any active menu
		return;
	}
	LeftBox->ClearChildren();
	if (bLeftMenuChanging)
	{
		if (bInGoingBack && MenuHistory.Num() > 0)
		{
			CurrentMenu = MenuHistory.Pop();
			if ((*CurrentMenu).Num() > 0 && (*CurrentMenu)[0].SubMenu.IsValid())
			{
				NextMenu = (*CurrentMenu)[0].SubMenu;
				bSubMenuChanging = true;
			}
		} 
		else if (PendingLeftMenu.IsValid())
		{
			MenuHistory.Push(CurrentMenu);
			CurrentMenu = PendingLeftMenu;
		}
	}

	//Setup the buttons
	for(int32 i = 0; i < (*CurrentMenu).Num(); ++i)
	{
		TSharedPtr<SWidget> TmpWidget;
		if ((*CurrentMenu)[i].MenuItemType == EVehicleMenuItemType::Standard)
		{
			TmpWidget = SAssignNew((*CurrentMenu)[i].Widget, SVehicleMenuItem)
				.OwnerHUD(MyMenuHUD)
				.OnClicked(this, &SVehicleMenuWidget::ButtonClicked, i)
				.Text(this, &SVehicleMenuWidget::GetMenuItemText, i );
		} 
		else if ((*CurrentMenu)[i].MenuItemType == EVehicleMenuItemType::CustomWidget)
		{
			TmpWidget = (*CurrentMenu)[i].CustomWidget;
		}
		LeftBox->AddSlot()	.HAlign(HAlign_Left)	.AutoHeight()
		[
			TmpWidget.ToSharedRef()
		];
	}
	
	SelectedIndex = 0;
	FVehicleMenuItem* FirstMenuItem = CurrentMenu->IsValidIndex(SelectedIndex) ? &(*CurrentMenu)[SelectedIndex] : nullptr;
	if (FirstMenuItem != nullptr && FirstMenuItem->MenuItemType == EVehicleMenuItemType::Standard)
	{
		FirstMenuItem->Widget->SetMenuItemActive(true);
		FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
	}
	
}

void SVehicleMenuWidget::BuildRightPanel()
{
	RightBox->ClearChildren();
	
	if (!NextMenu.IsValid() || (*NextMenu).Num() == 0) return;

	for(int32 i = 0; i < (*NextMenu).Num(); ++i)
	{
		//Only standard menu items supported in right panel
		if ((*NextMenu)[i].MenuItemType == EVehicleMenuItemType::Standard)
		{
			TSharedPtr<SVehicleMenuItem> TmpButton;
			TmpButton = SAssignNew((*NextMenu)[i].Widget, SVehicleMenuItem)
				.OwnerHUD(MyMenuHUD)
				.Text((*NextMenu)[i].Text)
				.InactiveTextAlpha(0.3f);
			RightBox->AddSlot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					TmpButton.ToSharedRef()
				];
		}
	}
}

FText SVehicleMenuWidget::GetMenuItemText(int32 Index) const
{
	return (*CurrentMenu)[Index].Text;
}

void SVehicleMenuWidget::EnterSubMenu()
{
	bLeftMenuChanging = true;
	bGoingBack = false;
	FSlateApplication::Get().PlaySound(MenuStyle->MenuEnterSound);
}

void SVehicleMenuWidget::MenuGoBack()
{
	if (MenuHistory.Num() > 0)
	{
		bLeftMenuChanging = true;
		bGoingBack = true;
		FSlateApplication::Get().PlaySound(MenuStyle->MenuExitSound);
	} 
	else if (bHideBackground) // only when it's in-game menu variant
	{
		FSlateApplication::Get().PlaySound(MenuStyle->MenuExitSound);
		OnToggleMenu.ExecuteIfBound();
	}

}

void SVehicleMenuWidget::ConfirmMenuItem()
{
	if ((*CurrentMenu)[SelectedIndex].OnConfirmMenuItem.IsBound())
	{
		(*CurrentMenu)[SelectedIndex].OnConfirmMenuItem.Execute();
	} 
	else if ((*CurrentMenu)[SelectedIndex].SubMenu.IsValid() && (*CurrentMenu)[SelectedIndex].SubMenu->Num() > 0)
	{
		EnterSubMenu();
	}
}

void SVehicleMenuWidget::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
	//Always tick the super
	SCompoundWidget::Tick( AllottedGeometry, InCurrentTime, InDeltaTime );

	//ugly code seeing if the console is open
	UConsole* ViewportConsole = (GEngine->GameViewport != nullptr) ? GEngine->GameViewport->ViewportConsole : nullptr;
	if (ViewportConsole != nullptr && (ViewportConsole->ConsoleState == "Typing" || ViewportConsole->ConsoleState == "Open"))
	{
		if (!bConsoleVisible)
		{
			bConsoleVisible = true;
			FSlateApplication::Get().SetAllUserFocusToGameViewport();
		}
	}
	else
	{
		if (bConsoleVisible)
		{
			bConsoleVisible = false;
			FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
		}
	}

	if (MenuWidgetAnimation.IsAtStart() && !bMenuHiding)
	{
		//Start the menu widget animation, set keyboard focus
		FadeIn();
	}
	else if (MenuWidgetAnimation.IsAtStart() && bMenuHiding)
	{
		bMenuHiding = false;
		//Send event, so menu can be removed
		OnMenuHidden.ExecuteIfBound();
	}

	if (MenuWidgetAnimation.IsAtEnd())
	{
		if (bLeftMenuChanging)
		{
			if (LeftMenuWidgetAnimation.IsAtEnd())
			{
				PendingLeftMenu = NextMenu;
				if (NextMenu.IsValid() && NextMenu->Num() > 0
					&& ((*NextMenu))[0].SubMenu.IsValid() && ((*NextMenu))[0].SubMenu->Num() > 0)
				{
					NextMenu = ((*NextMenu))[0].SubMenu;
				} 
				else 
				{
					NextMenu.Reset();
				}
				bSubMenuChanging = true;

				LeftMenuWidgetAnimation.PlayReverse(this->AsShared());
			}
			if (!LeftMenuWidgetAnimation.IsPlaying())
			{
				if (CurrentMenu.IsValid() && CurrentMenu->Num() > 0)
				{
					BuildLeftPanel(bGoingBack);
					LeftMenuWidgetAnimation.Play(this->AsShared());
				}
				//Focus the custom widget
				if (CurrentMenu->Num() == 1 && (*CurrentMenu)[0].MenuItemType == EVehicleMenuItemType::CustomWidget)
				{
					FSlateApplication::Get().SetKeyboardFocus((*CurrentMenu)[0].CustomWidget);
				}
				bLeftMenuChanging = false;
			}
		}
		if (bSubMenuChanging)
		{
			if (SubMenuWidgetAnimation.IsAtEnd())
			{
				SubMenuWidgetAnimation.PlayReverse(this->AsShared());
			}
			if (!SubMenuWidgetAnimation.IsPlaying())
			{
				if (NextMenu.IsValid() && NextMenu->Num() > 0)
				{
					BuildRightPanel();
					SubMenuWidgetAnimation.Play(this->AsShared());
				}
				bSubMenuChanging = false;
			}
		}
	}
}

FMargin SVehicleMenuWidget::GetMenuOffset() const
{
	return FMargin(380-1200+ButtonsPosXCurve.GetLerp()*1200, 0,0,0);
}

FMargin SVehicleMenuWidget::GetLeftMenuOffset() const
{
	return FMargin(0, 0,-LeftBox->GetDesiredSize().X+LeftMenuScrollOutCurve.GetLerp()*(LeftBox->GetDesiredSize().X),0);
}

FMargin SVehicleMenuWidget::GetSubMenuOffset() const
{
	return FMargin(0, 0,-RightBox->GetDesiredSize().X+SubMenuScrollOutCurve.GetLerp()*RightBox->GetDesiredSize().X,0);
}


FVector2D SVehicleMenuWidget::GetBottomScale() const
{
	return FVector2D(BottomScaleYCurve.GetLerp(), BottomScaleYCurve.GetLerp());
}

FLinearColor SVehicleMenuWidget::GetBottomColor() const
{
	return FMath::Lerp(FLinearColor(1,1,1,0), FLinearColor(1,1,1,1), BottomColorCurve.GetLerp());
}

FLinearColor SVehicleMenuWidget::GetTopColor() const
{
	return FMath::Lerp(FLinearColor(1,1,1,0), FLinearColor(1,1,1,1), TopColorCurve.GetLerp());
}

FReply SVehicleMenuWidget::ButtonClicked(int32 ButtonIndex)
{
	if (SelectedIndex != ButtonIndex)
	{
		TSharedPtr<SVehicleMenuItem> MenuItem = (*CurrentMenu)[SelectedIndex].Widget;
		MenuItem->SetMenuItemActive(false);
		SelectedIndex = ButtonIndex;
		MenuItem = (*CurrentMenu)[SelectedIndex].Widget;
		MenuItem->SetMenuItemActive(true);
		NextMenu = (*CurrentMenu)[SelectedIndex].SubMenu;
		bSubMenuChanging = true;
		const FVehicleMenuSoundsStyle& MenuSounds = FVehicleStyle::Get().GetWidgetStyle<FVehicleMenuSoundsStyle>("DefaultVehicleMenuSoundsStyle");
		FSlateApplication::Get().PlaySound(MenuSounds.MenuItemChangeSound);
	}
	else if (SelectedIndex == ButtonIndex)
	{
		ConfirmMenuItem();
	}

	return FReply::Handled().SetUserFocus(SharedThis(this), EFocusCause::SetDirectly);
}

void SVehicleMenuWidget::FadeIn()
{
	MenuWidgetAnimation.Play(this->AsShared());
	//Go into UI mode
	FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
}

FReply SVehicleMenuWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	//If we clicked anywhere, jump to the end
	if(MenuWidgetAnimation.IsPlaying())
	{
		MenuWidgetAnimation.JumpToEnd();
	}

	//Set the keyboard focus 
	return FReply::Handled()
		.SetUserFocus(SharedThis(this), EFocusCause::SetDirectly);
}

FReply SVehicleMenuWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Result = FReply::Unhandled();
	const FKey Key = InKeyEvent.GetKey();
	
	if (Key == EKeys::Up || Key == EKeys::Gamepad_DPad_Up || Key == EKeys::Gamepad_LeftStick_Up)
	{
		if (SelectedIndex > 0)
		{
			ButtonClicked(SelectedIndex - 1);
		}
		Result = FReply::Handled();
	}
	else if (Key == EKeys::Down || Key == EKeys::Gamepad_DPad_Down || Key == EKeys::Gamepad_LeftStick_Down)
	{
		if (SelectedIndex + 1 < (*CurrentMenu).Num())
		{
			ButtonClicked(SelectedIndex + 1);
		}
		Result = FReply::Handled();
	}
	else if (Key == EKeys::Enter || Key == EKeys::Virtual_Accept)
	{
		ConfirmMenuItem();
		Result = FReply::Handled();
	} 
	else if (Key == EKeys::Escape || Key == EKeys::Virtual_Back || Key == EKeys::Gamepad_Special_Left)
	{
		MenuGoBack();
		Result = FReply::Handled();
	}
	else if (Key == ControllerHideMenuKey)
	{
		OnToggleMenu.ExecuteIfBound();
		Result = FReply::Handled();
	}

	return Result;
}

FReply SVehicleMenuWidget::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	//Focus the custom widget
	if (CurrentMenu->Num() == 1 && (*CurrentMenu)[0].MenuItemType == EVehicleMenuItemType::CustomWidget)
	{
		return FReply::Handled().SetUserFocus((*CurrentMenu)[0].CustomWidget.ToSharedRef(), EFocusCause::SetDirectly);
	}

	return FReply::Handled().ReleaseMouseCapture().SetUserFocus(SharedThis( this ));
}

#undef LOCTEXT_NAMESPACE
