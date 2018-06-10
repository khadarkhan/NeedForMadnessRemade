// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "VehicleTypes.h"

//class declare
class SVehicleMenuWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SVehicleMenuWidget)
	: _MenuHUD()
	{}

	/** weak pointer to the parent HUD base */
	SLATE_ARGUMENT(TWeakObjectPtr<AHUD>, MenuHUD)

	/** is this main menu or in game menu? */
	SLATE_ARGUMENT(bool, HideBackground)

	/** always goes here */
	SLATE_END_ARGS()

	/** delegate declaration */
	DECLARE_DELEGATE(FOnMenuHidden);

	/** external delegate to call when in-game menu should be hidden using controller buttons - 
	it's workaround as when joystick is captured, even when sending FReply::Unhandled, binding does not recieve input :( */
	DECLARE_DELEGATE(FOnToggleMenu);

	/** every widget needs a construction function */
	void Construct(const FArguments& InArgs);

	/** update function. Kind of a hack. Allows us to only start fading in once we are done loading. */
	virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

	/** to have the mouse cursor show up at all times, we need the widget to handle all mouse events */
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	/** key down handler */
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	/** Says that we can support keyboard focus */
	virtual bool SupportsKeyboardFocus() const override { return true; }

	/** The menu sets up the appropriate mouse settings upon focus */
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;

	/** setups animation lengths, start points and launches initial animations */
	void SetupAnimations();

	/** builds left menu panel */
	void BuildLeftPanel(bool bGoingBack);

	/** builds inactive next menu panel (current selections sub menu preview) */
	void BuildRightPanel();

	/** starts animations to enter sub menu, it will become active menu */
	void EnterSubMenu();

	/** starts reverse animations to go one level up in menu hierarchy */
	void MenuGoBack();

	/** confirms current menu item and performs an action */
	void ConfirmMenuItem();

	/** call to rebuild menu and start animating it */
	void BuildAndShowMenu();

	/** call to hide menu */
	void HideMenu();

	/** add item to menu */
	static FVehicleMenuItem* AddMenuItem(MenuPtr &SubMenu, FText Text);

	/** add custom widget to menu (for example whole controls remapping window) */
	static FVehicleMenuItem* AddCustomWidgetMenuItem(MenuPtr &SubMenu, TSharedPtr<SWidget> CustomWidget);

	/** main menu for this instance of widget */
	MenuPtr MainMenu;

	/** currently active menu */
	MenuPtr CurrentMenu;

	/** next menu (for transition and displaying as the right menu) */
	MenuPtr NextMenu;

	/** stack of previous menus */
	TArray<MenuPtr> MenuHistory;

	/** delegate, which is executed when menu is finished hiding */
	FOnMenuHidden OnMenuHidden;

	/** bind if menu should be hidden from outside by controller button */
	FOnToggleMenu OnToggleMenu;

	/** default - start button, change to use different */
	FKey ControllerHideMenuKey;

	/** if console is currently opened */
	bool bConsoleVisible;

private:

	/** sets hit test invisibility when console is up */
	EVisibility GetSlateVisibility() const;

	/** getters used for animating the menu */
	FVector2D GetBottomScale() const;
	FLinearColor GetBottomColor() const;
	FLinearColor GetTopColor() const;
	FMargin GetMenuOffset() const;
	FMargin GetLeftMenuOffset() const;
	FMargin GetSubMenuOffset() const;

	/** callback for when one of the N buttons is clicked */
	FReply ButtonClicked(int32 ButtonIndex);

	/** gets menu item text */
	FText GetMenuItemText(int32 Index) const;

	/** this function starts the entire fade in process */
	void FadeIn();

	/** our curve sequence and the related handles */
	FCurveSequence MenuWidgetAnimation;

	/** used for menu background scaling animation at the beginning */ 
	FCurveHandle BottomScaleYCurve;

	/** used for main menu logo fade in animation at the beginning  */
	FCurveHandle TopColorCurve;

	/** used for menu background fade in animation at the beginning */
	FCurveHandle BottomColorCurve;

	/** used for menu buttons slide in animation at the beginning */
	FCurveHandle ButtonsPosXCurve;

	/** sub menu transition animation sequence */
	FCurveSequence SubMenuWidgetAnimation;

	/** sub menu transition animation curve */
	FCurveHandle SubMenuScrollOutCurve;

	/** current menu transition animation sequence */
	FCurveSequence LeftMenuWidgetAnimation;

	/** current menu transition animation curve */
	FCurveHandle LeftMenuScrollOutCurve;

	/** weak pointer to our parent HUD */
	TWeakObjectPtr<class AHUD> MyMenuHUD;

	/** selected index of current menu */
	int32 SelectedIndex;

	/** right panel animating flag */
	uint8 bSubMenuChanging : 1;

	/** left panel animating flag */
	uint8 bLeftMenuChanging : 1;

	/** going back to previous menu animation flag */
	uint8 bGoingBack : 1;

	/** flag when playing hiding animation */
	uint8 bMenuHiding : 1;

	/** if this is in game menu, do not show background or logo */
	uint8 bHideBackground : 1;
	
	/** menu that will override current one after transition animation */
	TSharedPtr< TArray<class FVehicleMenuItem> > PendingLeftMenu;

	/** left(current) menu layout box */
	TSharedPtr<SVerticalBox> LeftBox;

	/** right(sub) menu layout box */
	TSharedPtr<SVerticalBox> RightBox;

	/** style for this menu */
	const struct FVehicleMenuStyle *MenuStyle;
};

