// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleGame.h"
#include "UI/Menu/VehicleHUD_Menu.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "SVehicleMenuWidget.h"
#include "VehicleMenuItem.h"
#include "SVehicleControlsSetup.h"
#include "VehicleStyle.h"
#include "VehicleGameLoadingScreen.h"
#include "VehicleMenuSoundsWidgetStyle.h"
#include "VehicleGameUserSettings.h"

#define LOCTEXT_NAMESPACE "VehicleGame.HUD.Menu"

AVehicleHUD_Menu::AVehicleHUD_Menu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	LowHighList.Add(LOCTEXT("LowQuality","LOW QUALITY"));
	LowHighList.Add(LOCTEXT("HighQuality","HIGH QUALITY"));
}

void AVehicleHUD_Menu::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Now that we are here, build our menu widget
	RebuildWidgets();
}

void AVehicleHUD_Menu::PlaySolo()
{
	GEngine->SetClientTravel(GetWorld(), TEXT("/Game/Maps/VehicleEntry"), TRAVEL_Absolute);
	FSlateApplication::Get().SetAllUserFocusToGameViewport();
	ShowLoadingScreen();
}

void AVehicleHUD_Menu::HostLocal()
{
	GEngine->SetClientTravel(GetWorld(), TEXT("/Game/Maps/VehicleEntry?listen"), TRAVEL_Absolute);
	FSlateApplication::Get().SetAllUserFocusToGameViewport();
	ShowLoadingScreen();
}

void AVehicleHUD_Menu::Quit()
{
	GetOwningPlayerController()->ConsoleCommand("quit");
}

void AVehicleHUD_Menu::ShowLoadingScreen()
{
	IVehicleGameLoadingScreenModule* LoadingScreenModule = FModuleManager::LoadModulePtr<IVehicleGameLoadingScreenModule>("VehicleGameLoadingScreen");
	LoadingScreenModule->StartInGameLoadingScreen();
}

void AVehicleHUD_Menu::ExecuteMenuAction(EVehicleMenu::Type Action)
{	
	UVehicleGameUserSettings* UserSettings = CastChecked<UVehicleGameUserSettings>(GEngine->GetGameUserSettings());
	int32 NewQuality = UserSettings->GetGraphicsQuality() == 1 ? 0 : 1;

	const FVehicleMenuSoundsStyle& MenuSounds = FVehicleStyle::Get().GetWidgetStyle<FVehicleMenuSoundsStyle>("DefaultVehicleMenuSoundsStyle");

	switch (Action)
	{
		case EVehicleMenu::PlaySolo:
			MenuHelper::PlaySoundAndCall(PlayerOwner->GetWorld(),MenuSounds.StartGameSound,this,&AVehicleHUD_Menu::PlaySolo);
			break;
		case EVehicleMenu::HostLocal:
			MenuHelper::PlaySoundAndCall(PlayerOwner->GetWorld(),MenuSounds.StartGameSound,this,&AVehicleHUD_Menu::HostLocal);
			break;
		case EVehicleMenu::JoinLocal:
			GEngine->SetClientTravel(GetWorld(), TEXT("127.0.0.1"), TRAVEL_Absolute);
			FSlateApplication::Get().SetAllUserFocusToGameViewport();
			break;
		case EVehicleMenu::RemapControls:
			//show our special menu
			ControlsSetupWidget->UpdateActionBindings(FName("Keyboard"));
			MenuWidget->NextMenu = ControlsMenu;
			MenuWidget->EnterSubMenu();
			break;
		case EVehicleMenu::RemapController:
			ControlsSetupWidget->UpdateActionBindings(FName("Controller"));
			MenuWidget->NextMenu = ControlsMenu;
			MenuWidget->EnterSubMenu();
			break;
		case EVehicleMenu::ChangeQuality:
			if (UserSettings)
			{
				FSlateApplication::Get().PlaySound(MenuSounds.AcceptChangesSound);
				UserSettings->SetGraphicsQuality(NewQuality);
				QualityMenuItem->Text = LowHighList[NewQuality];
				UserSettings->ApplySettings(false);
			}
			break;
		case EVehicleMenu::Quit:
			MenuHelper::PlaySoundAndCall(PlayerOwner->GetWorld(),MenuSounds.ExitGameSound,this,&AVehicleHUD_Menu::Quit);
			break;

		default:
			MenuWidget->MenuGoBack();
			break;
	}
}

void AVehicleHUD_Menu::RebuildWidgets(bool bHotReload)
{
	MenuWidget.Reset();

	int32 CurrentQuality = 1;
	UVehicleGameUserSettings* UserSettings = CastChecked<UVehicleGameUserSettings>(GEngine->GetGameUserSettings());
	CurrentQuality = UserSettings->GetGraphicsQuality();

	if (GEngine && GEngine->GameViewport)
	{
		UGameViewportClient* GVC = GEngine->GameViewport;
		SAssignNew(MenuWidget, SVehicleMenuWidget)
			.Cursor(EMouseCursor::Default)
			.MenuHUD(this)
			.HideBackground(false);

		GVC->AddViewportWidgetContent(
			SNew(SWeakWidget)
			.PossiblyNullContent(MenuWidget.ToSharedRef()));

		SVehicleMenuWidget::AddMenuItem(MainMenu,LOCTEXT("PlaySolo", "PLAY SOLO"));
		SVehicleMenuWidget::AddMenuItem(MainMenu->Last().SubMenu,LOCTEXT("Mountains", "MOUNTAINS"))
			->OnConfirmMenuItem.BindUObject(this, &AVehicleHUD_Menu::ExecuteMenuAction, EVehicleMenu::PlaySolo);
		SVehicleMenuWidget::AddMenuItem(MainMenu,LOCTEXT("MultiPlayer", "MULTIPLAYER"));
			SVehicleMenuWidget::AddMenuItem(MainMenu->Last().SubMenu,LOCTEXT("HostLocal", "HOST"))
				->OnConfirmMenuItem.BindUObject(this, &AVehicleHUD_Menu::ExecuteMenuAction, EVehicleMenu::HostLocal);
			SVehicleMenuWidget::AddMenuItem(MainMenu->Last().SubMenu,LOCTEXT("JoinLocal", "JOIN LOCALHOST"))
				->OnConfirmMenuItem.BindUObject(this, &AVehicleHUD_Menu::ExecuteMenuAction, EVehicleMenu::JoinLocal);
		SVehicleMenuWidget::AddMenuItem(MainMenu,LOCTEXT("Options", "OPTIONS"));
		QualityMenuItem = SVehicleMenuWidget::AddMenuItem(MainMenu->Last().SubMenu,LowHighList[CurrentQuality]);
		QualityMenuItem->OnConfirmMenuItem.BindUObject(this, &AVehicleHUD_Menu::ExecuteMenuAction, EVehicleMenu::ChangeQuality);
		SVehicleMenuWidget::AddMenuItem(MainMenu->Last().SubMenu,LOCTEXT("KeyboardControls", "KEYBOARD CONTROLS"))
			->OnConfirmMenuItem.BindUObject(this, &AVehicleHUD_Menu::ExecuteMenuAction, EVehicleMenu::RemapControls);
		SVehicleMenuWidget::AddMenuItem(MainMenu->Last().SubMenu,LOCTEXT("ControllerControls", "CONTROLLER CONTROLS"))
			->OnConfirmMenuItem.BindUObject(this, &AVehicleHUD_Menu::ExecuteMenuAction, EVehicleMenu::RemapController);
		SVehicleMenuWidget::AddMenuItem(MainMenu,LOCTEXT("Quit", "QUIT"))
			->OnConfirmMenuItem.BindUObject(this, &AVehicleHUD_Menu::ExecuteMenuAction, EVehicleMenu::Quit);

		MenuWidget->MainMenu = MenuWidget->CurrentMenu = MainMenu;
		
		//this menu is not connected with the rest
		SVehicleMenuWidget::AddCustomWidgetMenuItem(ControlsMenu,SAssignNew(ControlsSetupWidget,SVehicleControlsSetup).OwnerWidget(MenuWidget));

		MenuWidget->BuildAndShowMenu();
	}
}


#undef LOCTEXT_NAMESPACE
