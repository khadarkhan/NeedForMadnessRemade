// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleGame.h"
#include "UI/VehicleHUD.h"
#include "Style/VehicleStyle.h"
#include "Widgets/SVehicleHUDWidget.h"
#include "Menu/SVehicleMenuWidget.h"
#include "Menu/VehicleMenuItem.h"
#include "Widgets/SVehicleControlsSetup.h"
#include "VehicleMenuSoundsWidgetStyle.h"
#include "VehicleGameUserSettings.h"
#include "VehicleGameMode.h"

#define LOCTEXT_NAMESPACE "VehicleGame.HUD.Menu"

AVehicleHUD::AVehicleHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> SpeedMeterObj(TEXT("/Game/UI/HUD/Materials/M_VH_HUD_SpeedMeter_UI"));

	static ConstructorHelpers::FObjectFinder<UFont> HUDFontOb(TEXT("/Game/UI/HUD/UI_Vehicle_Font"));

	static ConstructorHelpers::FObjectFinder<UTexture2D> TimerBgObj(TEXT("/Game/UI/HUD/Background/T_VH_Hud_Timer_Background"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> PlaceBgObj(TEXT("/Game/UI/HUD/Background/T_VH_Hud_Place_Background"));


	static ConstructorHelpers::FObjectFinder<UTexture2D> UpButtonTextureOb(TEXT("/Game/UI/HUD/UpButton"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> DownButtonTextureOb(TEXT("/Game/UI/HUD/DownButton"));

	UpButtonTexture = UpButtonTextureOb.Object;
	DownButtonTexture = DownButtonTextureOb.Object;

	HUDFont = HUDFontOb.Object;

	TimerBackground = TimerBgObj.Object;
	PlaceBackground = PlaceBgObj.Object;
	
	SpeedMeterMaterialConst = SpeedMeterObj.Object;


	LowHighList.Add(LOCTEXT("LowQuality","LOW QUALITY"));
	LowHighList.Add(LOCTEXT("HighQuality","HIGH QUALITY"));

	UIScale = 1.0f;

	CurrentLetter = 0;
	bEnterNamePromptActive = false;
	bDrawHUD = true;
}


void AVehicleHUD::BeginPlay()
{
	SpeedMeterMaterial = UMaterialInstanceDynamic::Create(SpeedMeterMaterialConst, nullptr);
}

void AVehicleHUD::DrawHUD()
{
	Super::DrawHUD();
	BuildMenuWidgets();
	if (GetNetMode() != NM_Standalone)
	{
		FString NetModeDesc = (GetNetMode() == NM_Client) ? TEXT("Client") : TEXT("Server");
		DrawDebugInfoString(NetModeDesc, 256.0f,32.0f, true, true, FColor::White);
	}
}

void AVehicleHUD::DrawDebugInfoString(const FString& Text, float PosX, float PosY, bool bAlignLeft, bool bAlignTop, const FColor& TextColor)
{
#if !UE_BUILD_SHIPPING
	float SizeX, SizeY;
	UFont* Font = GEngine->GetSmallFont();
	Canvas->StrLen(Font, Text, SizeX, SizeY);

	const float X = PosX;
	const float Y = PosY;
	FCanvasTileItem TileItem(FVector2D(X, Y), FVector2D(SizeX, SizeY), FLinearColor::Gray);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);

	FCanvasTextItem TextItem(FVector2D(X, Y), FText::FromString(Text), Font, TextColor);
	TextItem.EnableShadow(FLinearColor::Black);
	FFontRenderInfo FontInfo;
	FontInfo.bEnableShadow = true;
	TextItem.FontRenderInfo = FontInfo;
	Canvas->DrawItem(TextItem);
#endif
}

void AVehicleHUD::NotifyHitBoxClick(FName BoxName)
{
	Super::NotifyHitBoxClick(BoxName);

	if (BoxName.GetPlainNameString() == "Letter")
	{
		CurrentLetter = BoxName.GetNumber();
	}
	if (BoxName == "Up")
	{
		if (HighScoreName[CurrentLetter] < 'Z')
		{
			HighScoreName[CurrentLetter]++;
		}
	}
	if (BoxName == "Down")
	{
		if (HighScoreName[CurrentLetter] > 'A')
		{
			HighScoreName[CurrentLetter]--;
		}
	}
	if (BoxName == "OK")
	{
		bEnterNamePromptActive = false;
		if (PlayerOwner)
		{
			PlayerOwner->bShowMouseCursor = bEnterNamePromptActive;
		}

		FString EnteredName = FString();
		for (int32 i=0; i < HighScoreName.Num(); i++)
		{
			EnteredName.AppendChar(HighScoreName[i]);
		}
	}
}


void AVehicleHUD::BuildMenuWidgets()
{
	if (!GEngine || !GEngine->GameViewport)
	{
		return;
	}
	int32 CurrentQuality = 1;
	UVehicleGameUserSettings* UserSettings = nullptr;

	UserSettings = CastChecked<UVehicleGameUserSettings>(GEngine->GetGameUserSettings());
	if (UserSettings)
	{
		CurrentQuality = UserSettings->GetGraphicsQuality();
	}

	if (!VehicleHUDWidget.IsValid())
	{
		SAssignNew(VehicleHUDWidget, SVehicleHUDWidget)
		.OwnerWorld(GetWorld());

		GEngine->GameViewport->AddViewportWidgetContent(
			SNew(SWeakWidget)
			.PossiblyNullContent(VehicleHUDWidget.ToSharedRef())
			);

	}

	if (!MyHUDMenuWidget.IsValid())
	{
		SAssignNew(MyHUDMenuWidget, SVehicleMenuWidget)
			.Cursor(EMouseCursor::Default)
			.MenuHUD(this)
			.HideBackground(true);

		SVehicleMenuWidget::AddMenuItem(MainMenu,LOCTEXT("RestartTrack", "RESTART TRACK"))
			->OnConfirmMenuItem.BindUObject(this, &AVehicleHUD::ExecuteMenuAction, EVehicleGameMenu::RestartTrack);
		SVehicleMenuWidget::AddMenuItem(MainMenu,LOCTEXT("Options", "OPTIONS"));
		QualityMenuItem = SVehicleMenuWidget::AddMenuItem(MainMenu->Last().SubMenu,LowHighList[CurrentQuality]);
		QualityMenuItem->OnConfirmMenuItem.BindUObject(this, &AVehicleHUD::ExecuteMenuAction, EVehicleGameMenu::ChangeQuality);
		SVehicleMenuWidget::AddMenuItem(MainMenu->Last().SubMenu,LOCTEXT("KeyboardControls", "KEYBOARD CONTROLS"))
			->OnConfirmMenuItem.BindUObject(this, &AVehicleHUD::ExecuteMenuAction, EVehicleGameMenu::RemapControls);
		SVehicleMenuWidget::AddMenuItem(MainMenu->Last().SubMenu,LOCTEXT("ControllerControls", "CONTROLLER CONTROLS"))
			->OnConfirmMenuItem.BindUObject(this, &AVehicleHUD::ExecuteMenuAction, EVehicleGameMenu::RemapController);
		SVehicleMenuWidget::AddMenuItem(MainMenu,LOCTEXT("Quit", "QUIT"))
			->OnConfirmMenuItem.BindUObject(this, &AVehicleHUD::ExecuteMenuAction, EVehicleGameMenu::Quit);

		MyHUDMenuWidget->MainMenu = MyHUDMenuWidget->CurrentMenu = MainMenu;
		MyHUDMenuWidget->OnMenuHidden.BindUObject(this,&AVehicleHUD::DetachGameMenu);
		MyHUDMenuWidget->OnToggleMenu.BindUObject(this,&AVehicleHUD::ToggleGameMenu);

		//this menu is not connected with the rest
		SVehicleMenuWidget::AddCustomWidgetMenuItem(ControlsMenu,SAssignNew(ControlsSetupWidget,SVehicleControlsSetup).OwnerWidget(MyHUDMenuWidget).OwnerHUD(this));

		bIsGameMenuUp = false;
	}
}

void AVehicleHUD::DetachGameMenu()
{
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(GameMenuContainer.ToSharedRef());
	}
	FSlateApplication::Get().SetAllUserFocusToGameViewport();
	bIsGameMenuUp = false;
	PlayerOwner->SetCinematicMode(bIsGameMenuUp,false,false,true,true);
}

bool AVehicleHUD::IsGameMenuUp() const
{
	return bIsGameMenuUp;
}

void AVehicleHUD::EnableHUD(bool bEnable)
{
	bDrawHUD = bEnable;
}

void AVehicleHUD::ToggleGameMenu()
{
	if (!MyHUDMenuWidget.IsValid())
	{
		return;
	}

	if (bIsGameMenuUp && MyHUDMenuWidget->CurrentMenu != MainMenu)
	{
		MyHUDMenuWidget->MenuGoBack();
		return;
	}

	// Make the game pause (network games will not pause!)
	AVehicleGameMode* const MyGame = GetWorld()->GetAuthGameMode<AVehicleGameMode>();
	if( MyGame )
	{
		MyGame->SetGamePaused( !bIsGameMenuUp );
	}

	if (!bIsGameMenuUp)
	{
		GEngine->GameViewport->AddViewportWidgetContent(
			SAssignNew(GameMenuContainer,SWeakWidget)
			.PossiblyNullContent(MyHUDMenuWidget.ToSharedRef())
			);
		MyHUDMenuWidget->BuildAndShowMenu();
		bIsGameMenuUp = true;
		PlayerOwner->SetCinematicMode(bIsGameMenuUp,false,false,true,true);
	} 
	else 
	{
		MyHUDMenuWidget->HideMenu();
		bIsGameMenuUp = false;
	}
}

void AVehicleHUD::RestartTrack()
{
	GetOwningPlayerController()->RestartLevel();
	FSlateApplication::Get().SetAllUserFocusToGameViewport();
}

void AVehicleHUD::ReturnToMenu()
{
// 	FString RemoteReturnReason = NSLOCTEXT("NetworkErrors", "HostHasLeft", "Host has left the game.").ToString();
	FText LocalReturnReason = FText::GetEmpty();

	if (GetNetMode() < NM_Client)
	{
		APlayerController* Controller = nullptr;
		FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator();
		Iterator.Reset();
		for(; Iterator; ++Iterator)
		{
			Controller = Iterator->Get();
			if (Controller && Controller->IsLocalPlayerController() && Controller->IsPrimaryPlayer())
			{
				Controller->ClientReturnToMainMenuWithTextReason(LocalReturnReason);
			}
		}
	}
	else
	{
		GetOwningPlayerController()->ClientReturnToMainMenuWithTextReason(LocalReturnReason);
	}
}

void AVehicleHUD::Quit()
{
	GetOwningPlayerController()->ConsoleCommand("quit");
}

void AVehicleHUD::ExecuteMenuAction(EVehicleGameMenu::Type Action)
{
	UVehicleGameUserSettings* UserSettings = CastChecked<UVehicleGameUserSettings>(GEngine->GetGameUserSettings());
	int32 NewQuality = (UserSettings->GetGraphicsQuality() == 1 ? 0 : 1);

	const FVehicleMenuSoundsStyle& MenuSounds = FVehicleStyle::Get().GetWidgetStyle<FVehicleMenuSoundsStyle>("DefaultVehicleMenuSoundsStyle");

	switch (Action)
	{
	case EVehicleGameMenu::RestartTrack:
		MenuHelper::PlaySoundAndCall(PlayerOwner->GetWorld(),MenuSounds.StartGameSound,this,&AVehicleHUD::RestartTrack);
		MyHUDMenuWidget->MenuGoBack();
		break;
	case EVehicleGameMenu::RemapControls:
		ControlsSetupWidget->UpdateActionBindings(FName("Keyboard"));
		MyHUDMenuWidget->NextMenu = ControlsMenu;
		MyHUDMenuWidget->EnterSubMenu();
		break;
	case EVehicleGameMenu::RemapController:
		ControlsSetupWidget->UpdateActionBindings(FName("Controller"));
		MyHUDMenuWidget->NextMenu = ControlsMenu;
		MyHUDMenuWidget->EnterSubMenu();
		break;
	case EVehicleGameMenu::ChangeQuality:
		if (UserSettings)
		{
			FSlateApplication::Get().PlaySound(MenuSounds.AcceptChangesSound);
			UserSettings->SetGraphicsQuality(NewQuality);
			QualityMenuItem->Text = LowHighList[NewQuality];
			UserSettings->ApplySettings(false);
		}
		break;
	case EVehicleGameMenu::Quit:
		MenuHelper::PlaySoundAndCall(PlayerOwner->GetWorld(),MenuSounds.ExitGameSound,this,&AVehicleHUD::Quit);
		break;

	default:
		MyHUDMenuWidget->MenuGoBack();
		break;
	}
}

#undef LOCTEXT_NAMESPACE
