// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleGame.h"
#include "SVehicleControlsSetup.h"
#include "VehicleStyle.h"
#include "VehicleMenuSoundsWidgetStyle.h"
#include "VehicleMenuItemWidgetStyle.h"
#include "VehicleGameUserSettings.h"
#include "UI/VehicleHUD.h"

#define LOCTEXT_NAMESPACE "SVehicleControlsSetup"

void SVehicleControlsSetup::Construct(const FArguments& InArgs)
{
	PopulateFriendlyKeyNameMap();

	// Create a binding restriction to stop silly bindings for the steering
	BindingRestriction* Restrict = AddBindingRestriction("MoveRight", true, false);
	if (Restrict != nullptr)
	{
		Restrict->DisallowedKeys.Add(EKeys::Gamepad_LeftTriggerAxis);
		Restrict->DisallowedKeys.Add(EKeys::Gamepad_RightTriggerAxis);
		Restrict->DisallowedKeys.Add(EKeys::Gamepad_LeftY); 
		Restrict->DisallowedKeys.Add(EKeys::Gamepad_RightY);
	}

	MenuItemStyle = &FVehicleStyle::Get().GetWidgetStyle<FVehicleMenuItemStyle>("DefaultVehicleMenuItemStyle");

	OwnerHUD = InArgs._OwnerHUD;
	OwnerWidget = InArgs._OwnerWidget;
	bAwaitingKeyPress = false;

	ChildSlot
	.VAlign(VAlign_Fill)
	.HAlign(HAlign_Fill)
	[
		SNew(SBox)  
		.WidthOverride(600)
		.HeightOverride(200)
		[
			SAssignNew(ActionBindingsList, SListView<TSharedPtr<FActionBinding>>)
			.ItemHeight(20)
			.ListItemsSource(&ActionBindings)
			.SelectionMode(ESelectionMode::Single)
			.OnGenerateRow(this, &SVehicleControlsSetup::MakeListViewWidget)
			.OnSelectionChanged(this, &SVehicleControlsSetup::KeyMapSelectionChanged)
			.OnMouseButtonDoubleClick(this,&SVehicleControlsSetup::OnListItemDoubleClicked)
		]
	];

}

void SVehicleControlsSetup::UpdateActionBindings(FName GroupName)
{
	if (!GEngine)
	{
		return;
	}

	if(!OwnerHUD.IsValid())
	{
		return;
	}

	UWorld *World = OwnerHUD.Get()->GetWorld();

	CurrentGroup = GroupName;
	UE_LOG(LogTemp, Warning, TEXT("CurrentGroup is %s"), *GroupName.ToString());
	int32 SelectedItemIndex = ActionBindings.IndexOfByKey(SelectedItem);
	ActionBindings.Empty();
	UVehicleGameUserSettings* UserSettings = CastChecked<UVehicleGameUserSettings>(GEngine->GetGameUserSettings());

	const TArray<FInputActionKeyMapping>& Actions = GetDefault<UInputSettings>()->ActionMappings;
	//order of items will depend on keybindings ui config, not on the ordering from player input
	UE_LOG(LogTemp, Warning, TEXT("**********"));
	for (int32 i = 0; i < Actions.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("(%s, %s, %s)"), *GroupName.ToString(), *Actions[i].ActionName.ToString(), *Actions[i].Key.ToString());
	}
	UE_LOG(LogTemp, Warning, TEXT("**********"));
	for (int32 i = 0; i < Actions.Num(); i++)
	{
		FInputActionKeyMapping AddAction;
		for (int32 j = 0; j < UserSettings->KeybindingsUIConfig.Num(); j++)
		{
			if (UserSettings->KeybindingsUIConfig[j].ActionName == Actions[i].ActionName)
			{
			}
		}
	}

//	return;
	for (int32 j=0; j < UserSettings->KeybindingsUIConfig.Num(); j++)
	{
		for (int32 i=0; i < Actions.Num();i++)
		{
			if (UserSettings->KeybindingsUIConfig[j].GroupName == GroupName && 
				UserSettings->KeybindingsUIConfig[j].ActionName == Actions[i].ActionName &&
				UserSettings->KeybindingsUIConfig[j].Key == Actions[i].Key)
			{
//				UE_LOG(LogTemp, Warning, TEXT("ACCEPT (%s, %s, %s)"), *UserSettings->KeybindingsUIConfig[j].GroupName.ToString(), *UserSettings->KeybindingsUIConfig[j].ActionName.ToString(), *UserSettings->KeybindingsUIConfig[j].Key.ToString());
				ActionBindings.Add(MakeShareable(new FActionBinding(&UserSettings->KeybindingsUIConfig[j], &Actions[i])));
			}
			else
			{
//				UE_LOG(LogTemp, Warning, TEXT("Reject (%s, %s, %s) based on (%s, %s, %s)"), *UserSettings->KeybindingsUIConfig[j].GroupName.ToString(), *UserSettings->KeybindingsUIConfig[j].ActionName.ToString(), *UserSettings->KeybindingsUIConfig[j].Key.ToString(), *GroupName.ToString(), *Actions[i].ActionName.ToString(), *Actions[i].Key.ToString());
			}
		}
	}
//	return;
	const TArray<FInputAxisKeyMapping>& AxisActions = GetDefault<UInputSettings>()->AxisMappings;
	for (int32 j=0; j < UserSettings->KeybindingsUIConfig.Num(); j++)
	{
		for (int32 i=0; i < AxisActions.Num();i++)
		{
			if (UserSettings->KeybindingsUIConfig[j].GroupName == GroupName && 
				UserSettings->KeybindingsUIConfig[j].AxisName == AxisActions[i].AxisName &&
				UserSettings->KeybindingsUIConfig[j].Scale == AxisActions[i].Scale &&
				UserSettings->KeybindingsUIConfig[j].Key == AxisActions[i].Key)
			{
				UE_LOG(LogTemp, Warning, TEXT("AXIS ACCEPT (%s, %s, %f, %s)"), *UserSettings->KeybindingsUIConfig[j].GroupName.ToString(), *UserSettings->KeybindingsUIConfig[j].AxisName.ToString(), UserSettings->KeybindingsUIConfig[j].Scale, *UserSettings->KeybindingsUIConfig[j].Key.ToString());
				ActionBindings.Add(MakeShareable(new FActionBinding(&UserSettings->KeybindingsUIConfig[j], &AxisActions[i])));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AXIS Reject (%s, %s, %f, %s) based on (%s, %s, %f, %s)"), *UserSettings->KeybindingsUIConfig[j].GroupName.ToString(), *UserSettings->KeybindingsUIConfig[j].AxisName.ToString(), UserSettings->KeybindingsUIConfig[j].Scale, *UserSettings->KeybindingsUIConfig[j].Key.ToString(), *GroupName.ToString(), *AxisActions[i].AxisName.ToString(), AxisActions[i].Scale, *AxisActions[i].Key.ToString());
			}
		}
	}

	ActionBindingsList->RequestListRefresh();
	if (ActionBindings.Num() > 0)
	{
		ActionBindingsList->UpdateSelectionSet();
		ActionBindingsList->SetSelection(ActionBindings[SelectedItemIndex > -1 && SelectedItemIndex < ActionBindings.Num() ? SelectedItemIndex : 0], ESelectInfo::OnNavigation);
	}
}

void SVehicleControlsSetup::OnFocusLost( const FFocusEvent& InFocusEvent )
{
	if (InFocusEvent.GetCause() != EFocusCause::SetDirectly)
	{
		FSlateApplication::Get().SetKeyboardFocus(SharedThis( this ));
	}
}

FReply SVehicleControlsSetup::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	if (bAwaitingKeyPress)
	{
		return FReply::Handled().SetUserFocus(SharedThis( this ));
	}
	return FReply::Handled().SetUserFocus(ActionBindingsList.ToSharedRef());
}

void SVehicleControlsSetup::KeyMapSelectionChanged(TSharedPtr<FActionBinding> InItem, ESelectInfo::Type SelectInfo)
{
	if (SelectInfo == ESelectInfo::OnMouseClick)
	{
		bAwaitingKeyPress = false;
	}
	SelectedItem = InItem;

	const FVehicleMenuSoundsStyle& MenuSounds = FVehicleStyle::Get().GetWidgetStyle<FVehicleMenuSoundsStyle>("DefaultVehicleMenuSoundsStyle");
	FSlateApplication::Get().PlaySound(MenuSounds.MenuItemChangeSound);
}

void SVehicleControlsSetup::OnListItemDoubleClicked(TSharedPtr<FActionBinding> InItem)
{
	SelectedItem = InItem;
	bAwaitingKeyPress = true;
	FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
}

void SVehicleControlsSetup::MoveSelection(int32 MoveBy)
{
	int32 SelectedItemIndex = ActionBindings.IndexOfByKey(SelectedItem);
	int32 NewSelectionIndex = SelectedItemIndex + MoveBy;
	if (NewSelectionIndex > -1 && SelectedItemIndex + MoveBy < ActionBindings.Num())
	{
		if (bAwaitingKeyPress) // exit binding mode
		{
			bAwaitingKeyPress = false;
		}

		const FVehicleMenuSoundsStyle& MenuSounds = FVehicleStyle::Get().GetWidgetStyle<FVehicleMenuSoundsStyle>("DefaultVehicleMenuSoundsStyle");
		FSlateApplication::Get().PlaySound(MenuSounds.MenuItemChangeSound);
		ActionBindingsList->SetItemSelection(ActionBindings[NewSelectionIndex], true, ESelectInfo::OnNavigation);
		SelectedItem = ActionBindings[NewSelectionIndex];
	}
}

FReply SVehicleControlsSetup::OnAnalogValueChanged(const FGeometry& MyGeometry, const FAnalogInputEvent& InAnalogInputEvent)
{
	FReply Result = FReply::Unhandled();
	const FKey Key = InAnalogInputEvent.GetKey();
	float AnalogValue = FMath::Abs(InAnalogInputEvent.GetAnalogValue());
	float RawAnalogValue = InAnalogInputEvent.GetAnalogValue();
	const float Threshold = 0.5f;
	if (AnalogValue >= Threshold )
	{
		if (bAwaitingKeyPress && CurrentGroup == FName("Controller"))
		{			
			RemapKey(SelectedItem, Key, true, false);
			Result = FReply::Handled();
		}
	}
	return Result;
}

void SVehicleControlsSetup::RemapKey(TSharedPtr<FActionBinding> Binding, const FKey Key, bool bForcedAxis, bool bForcedAction)
{
	if (GEngine)
	{
		if(!OwnerHUD.IsValid())
		{
			return;
		}

		if (IsBindingAllowed(Binding, Key, bForcedAxis, bForcedAction) == false)
		{
			return;
		}
		UWorld *World = OwnerHUD.Get()->GetWorld();

		UInputSettings* InputSettings = GetMutableDefault<UInputSettings>();
		if (Binding->InputMapping != nullptr)
		{
			Binding->BindConfig->Key = Key;
			FInputActionKeyMapping NewMapping(Binding->InputMapping->ActionName,Key);
			InputSettings->RemoveActionMapping(*Binding->InputMapping);
			InputSettings->AddActionMapping(NewMapping);
		}
		else if (Binding->AxisMapping != nullptr)
		{
			Binding->BindConfig->Key = Key;
			FInputAxisKeyMapping NewMapping(Binding->AxisMapping->AxisName,Key,Binding->AxisMapping->Scale);
			InputSettings->RemoveAxisMapping(*Binding->AxisMapping);
			InputSettings->AddAxisMapping(NewMapping);
		}
		UVehicleGameUserSettings* UserSettings = CastChecked<UVehicleGameUserSettings>(GEngine->GetGameUserSettings());
		UserSettings->SaveSettings();

		//save immediately
		GetMutableDefault<UInputSettings>()->SaveKeyMappings();
		FSlateApplication::Get().SetKeyboardFocus(ActionBindingsList.ToSharedRef());
		UpdateActionBindings(CurrentGroup);
		bAwaitingKeyPress = false;
	}
}

FReply SVehicleControlsSetup::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) 
{
	FReply Result = FReply::Unhandled();
	const FKey Key = InKeyEvent.GetKey();
	if (Key == EKeys::Escape && bAwaitingKeyPress)
	{
		const FVehicleMenuSoundsStyle& MenuSounds = FVehicleStyle::Get().GetWidgetStyle<FVehicleMenuSoundsStyle>("DefaultVehicleMenuSoundsStyle");
		FSlateApplication::Get().PlaySound(MenuSounds.DiscardChangesSound);
		bAwaitingKeyPress = false;
		Result = FReply::Handled();
		FSlateApplication::Get().SetKeyboardFocus(ActionBindingsList.ToSharedRef());
	} 
	else if (bAwaitingKeyPress && (CurrentGroup == FName("Keyboard") || CurrentGroup == FName("Controller")))
	{
		RemapKey(SelectedItem,Key,false,true);
		Result = FReply::Handled();
	}
	else if (Key == EKeys::Enter || Key == EKeys::Virtual_Accept)
	{
		bAwaitingKeyPress = true;
		const FVehicleMenuSoundsStyle& MenuSounds = FVehicleStyle::Get().GetWidgetStyle<FVehicleMenuSoundsStyle>("DefaultVehicleMenuSoundsStyle");
		FSlateApplication::Get().PlaySound(MenuSounds.AcceptChangesSound);
		Result = FReply::Handled();
		FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
	}
	else if (Key == EKeys::Up || Key == EKeys::Gamepad_DPad_Up || Key == EKeys::Gamepad_LeftStick_Up)
	{
		MoveSelection(-1);
		Result = FReply::Handled();
	}
	else if (Key == EKeys::Down || Key == EKeys::Gamepad_DPad_Down || Key == EKeys::Gamepad_LeftStick_Down)
	{
		MoveSelection(1);
		Result = FReply::Handled();
	}
	return Result;
}

TSharedRef<ITableRow> SVehicleControlsSetup::MakeListViewWidget(TSharedPtr<FActionBinding> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	const int32 BoxWidth = 300;
	return
		SNew( STableRow< TSharedPtr<FActionBinding> >, OwnerTable )
		.Style(&MenuItemStyle->ControlsListTableRowStyle)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot() 
			[
				SNew(SBox)
				.WidthOverride(BoxWidth)
				.HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text( Item->BindConfig->DisplayName )
					.TextStyle(FVehicleStyle::Get(), "VehicleGame.MenuTextStyle")			
				]
			]
			+SHorizontalBox::Slot() 
			[
				SNew(SBox)
				.WidthOverride(BoxWidth)
				.HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text( this,  &SVehicleControlsSetup::GetItemKeyBinding, Item)
					.TextStyle(FVehicleStyle::Get(), "VehicleGame.MenuTextStyle")
				]
			]
		];
}

void SVehicleControlsSetup::PopulateFriendlyKeyNameMap()
{
	FriendlyNameLookup.Empty();
	TArray<FKey>* Remap;

	// Ps4 Controls	
	// Pad
	AddFriendlyKeyNameRemap(EKeys::Gamepad_DPad_Up, LOCTEXT("Gamepad_DPad_Up", "DPad Up").ToString(), EConsoleForGamepadLabels::PS4);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_DPad_Down, LOCTEXT("Gamepad_DPad_Down", "DPad Down").ToString(), EConsoleForGamepadLabels::PS4);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_DPad_Right, LOCTEXT("Gamepad_DPad_Right", "DPad Right").ToString(), EConsoleForGamepadLabels::PS4);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_DPad_Left, LOCTEXT("Gamepad_DPad_Left", "DPad Left").ToString(), EConsoleForGamepadLabels::PS4);
	
	// Buttons
	AddFriendlyKeyNameRemap(EKeys::Gamepad_FaceButton_Top, LOCTEXT("Gamepad_FaceButton_Top", "Triangle").ToString(), EConsoleForGamepadLabels::PS4);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_FaceButton_Bottom, LOCTEXT("Gamepad_FaceButton_Bottom", "Cross").ToString(), EConsoleForGamepadLabels::PS4);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_FaceButton_Left, LOCTEXT("Gamepad_FaceButton_Left", "Square").ToString(), EConsoleForGamepadLabels::PS4);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_FaceButton_Right, LOCTEXT("Gamepad_FaceButton_Right", "Circle").ToString(), EConsoleForGamepadLabels::PS4);
	
	AddFriendlyKeyNameRemap(EKeys::Gamepad_Special_Left, LOCTEXT("PS4_Gamepad_Special_Left", "Click Touchpad").ToString(), EConsoleForGamepadLabels::PS4);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_Special_Left, LOCTEXT("PS4_Gamepad_Special_Right", "Options").ToString(), EConsoleForGamepadLabels::PS4);
	
	// Triggers
	AddFriendlyKeyNameRemap(EKeys::Gamepad_LeftShoulder, LOCTEXT("PS4_LeftShoulder", "L1").ToString(), EConsoleForGamepadLabels::PS4);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_RightTriggerAxis, LOCTEXT("PS4_RightShoulder", "R1").ToString(), EConsoleForGamepadLabels::PS4);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_LeftThumbstick, LOCTEXT("Gamepad_LeftThumbstick", "L3").ToString(), EConsoleForGamepadLabels::PS4);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_RightThumbstick, LOCTEXT("Gamepad_RightThumbstick", "R3").ToString(), EConsoleForGamepadLabels::PS4);

	Remap = AddFriendlyKeyNameRemap(EKeys::Gamepad_LeftTriggerAxis, LOCTEXT("PS4_LeftTrigger", "L2").ToString(), EConsoleForGamepadLabels::PS4);
	Remap->AddUnique(EKeys::Gamepad_LeftTrigger);

	Remap = AddFriendlyKeyNameRemap(EKeys::Gamepad_RightTriggerAxis, LOCTEXT("PS4_RightTrigger", "R2").ToString(), EConsoleForGamepadLabels::PS4);
	Remap->AddUnique(EKeys::Gamepad_RightTrigger);
	
	// Analog sticks
	Remap = AddFriendlyKeyNameRemap(EKeys::Gamepad_LeftX, LOCTEXT("Gamepad_Left_Stick_X", "Left Stick X Axis").ToString(), EConsoleForGamepadLabels::PS4);
	Remap->AddUnique(EKeys::Gamepad_LeftStick_Left);
	Remap->AddUnique(EKeys::Gamepad_LeftStick_Right);

	Remap = AddFriendlyKeyNameRemap(EKeys::Gamepad_LeftY, LOCTEXT("Gamepad_Left_Stick_Y", "Left Stick Y Axis").ToString(), EConsoleForGamepadLabels::PS4);
	Remap->AddUnique(EKeys::Gamepad_LeftStick_Up);
	Remap->AddUnique(EKeys::Gamepad_LeftStick_Down);

	Remap = AddFriendlyKeyNameRemap(EKeys::Gamepad_RightX, LOCTEXT("Gamepad_Right_Stick_X", "Right Stick X Axis").ToString(), EConsoleForGamepadLabels::PS4);
	Remap->AddUnique(EKeys::Gamepad_RightStick_Left);
	Remap->AddUnique(EKeys::Gamepad_RightStick_Right);

	Remap = AddFriendlyKeyNameRemap(EKeys::Gamepad_RightY, LOCTEXT("Gamepad_Right_Stick_Y", "Right Stick Y Axis").ToString(), EConsoleForGamepadLabels::PS4);
	Remap->AddUnique(EKeys::Gamepad_RightStick_Up);
	Remap->AddUnique(EKeys::Gamepad_RightStick_Down);
			
	
	// XBox One
	// DPad
	AddFriendlyKeyNameRemap(EKeys::Gamepad_DPad_Up, LOCTEXT("XBoxOne_DPad_Up", "DPad Up").ToString(), EConsoleForGamepadLabels::XBoxOne);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_DPad_Down, LOCTEXT("XBoxOne_DPad_Down", "DPad Down").ToString(), EConsoleForGamepadLabels::XBoxOne);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_DPad_Right, LOCTEXT("XBoxOne_DPad_Right", "DPad Right").ToString(), EConsoleForGamepadLabels::XBoxOne);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_DPad_Left, LOCTEXT("XBoxOne_DPad_Left", "DPad Left").ToString(), EConsoleForGamepadLabels::XBoxOne);
	
	// Buttons
	AddFriendlyKeyNameRemap(EKeys::Gamepad_FaceButton_Top, LOCTEXT("Gamepad_FaceButton_Top", "Y Button").ToString(), EConsoleForGamepadLabels::XBoxOne);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_FaceButton_Bottom, LOCTEXT("Gamepad_FaceButton_Bottom", "A Button").ToString(), EConsoleForGamepadLabels::XBoxOne);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_FaceButton_Left, LOCTEXT("Gamepad_FaceButton_Left", "X Button").ToString(), EConsoleForGamepadLabels::XBoxOne);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_FaceButton_Right, LOCTEXT("Gamepad_FaceButton_Right", "B Button").ToString(), EConsoleForGamepadLabels::XBoxOne);

	// Specials
	AddFriendlyKeyNameRemap(EKeys::Gamepad_Special_Left, LOCTEXT("Gamepad_SpecialLeft", "View").ToString(), EConsoleForGamepadLabels::XBoxOne);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_Special_Right, LOCTEXT("Gamepad_SpecialRight", "Menu").ToString(), EConsoleForGamepadLabels::XBoxOne);

	// Triggers
	AddFriendlyKeyNameRemap(EKeys::Gamepad_LeftShoulder, LOCTEXT("XBoxOne_LeftShoulder", "LB").ToString(), EConsoleForGamepadLabels::XBoxOne);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_RightTriggerAxis, LOCTEXT("XBoxOne_RightShoulder", "RB").ToString(), EConsoleForGamepadLabels::XBoxOne);
	
	Remap = AddFriendlyKeyNameRemap(EKeys::Gamepad_LeftTriggerAxis, LOCTEXT("XBoxOne_LeftTrigger", "Left Trigger").ToString(), EConsoleForGamepadLabels::XBoxOne);
	Remap->AddUnique(EKeys::Gamepad_LeftTrigger);

	Remap = AddFriendlyKeyNameRemap(EKeys::Gamepad_RightTriggerAxis, LOCTEXT("XBoxOne_RightTrigger", "Right Trigger").ToString(), EConsoleForGamepadLabels::XBoxOne);
	Remap->AddUnique(EKeys::Gamepad_RightTrigger);
		
	// Analog sticks
	AddFriendlyKeyNameRemap(EKeys::Gamepad_LeftThumbstick, LOCTEXT("Gamepad_LeftThumbstick", "L Click Stick").ToString(), EConsoleForGamepadLabels::XBoxOne);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_RightThumbstick, LOCTEXT("Gamepad_RightThumbstick", "R Stick Click").ToString(), EConsoleForGamepadLabels::XBoxOne);

	Remap = AddFriendlyKeyNameRemap(EKeys::Gamepad_LeftX, LOCTEXT("XBoxOne_Left_Stick_X", "Left Stick X Axis").ToString(), EConsoleForGamepadLabels::XBoxOne);
	Remap->AddUnique(EKeys::Gamepad_LeftStick_Left);
	Remap->AddUnique(EKeys::Gamepad_LeftStick_Right);

	Remap = AddFriendlyKeyNameRemap(EKeys::Gamepad_LeftY, LOCTEXT("XBoxOne_Left_Stick_Y", "Left Stick Y Axis").ToString(), EConsoleForGamepadLabels::XBoxOne);
	Remap->AddUnique(EKeys::Gamepad_LeftStick_Down);
	Remap->AddUnique(EKeys::Gamepad_LeftStick_Up);

	Remap = AddFriendlyKeyNameRemap(EKeys::Gamepad_RightX, LOCTEXT("XBoxOne_Right_Stick_Y", "Right Stick X Axis").ToString(), EConsoleForGamepadLabels::XBoxOne);
	Remap->AddUnique(EKeys::Gamepad_RightStick_Left);
	Remap->AddUnique(EKeys::Gamepad_RightStick_Right);

	Remap = AddFriendlyKeyNameRemap(EKeys::Gamepad_RightY, LOCTEXT("XBoxOne_Right_Stick_X", "Right Stick Y Axis").ToString(), EConsoleForGamepadLabels::XBoxOne);
	Remap->AddUnique(EKeys::Gamepad_RightStick_Down);
	Remap->AddUnique(EKeys::Gamepad_RightStick_Up);
		
	// Other (controller on PC)
	// DPad
	AddFriendlyKeyNameRemap(EKeys::Gamepad_DPad_Up, LOCTEXT("Gamepad_DPad_Up", "DPad Up").ToString(), EConsoleForGamepadLabels::None);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_DPad_Down, LOCTEXT("Gamepad_DPad_Down", "DPad Down").ToString(), EConsoleForGamepadLabels::None);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_DPad_Right, LOCTEXT("Gamepad_DPad_Right", "DPad Right").ToString(), EConsoleForGamepadLabels::None);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_DPad_Left, LOCTEXT("Gamepad_DPad_Left", "DPad Left").ToString(), EConsoleForGamepadLabels::None);

	// Buttons
	AddFriendlyKeyNameRemap(EKeys::Gamepad_FaceButton_Top, LOCTEXT("Gamepad_FaceButton_Top", "Top Face Button").ToString(), EConsoleForGamepadLabels::None);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_FaceButton_Bottom, LOCTEXT("Gamepad_FaceButton_Bottom", "Bottom Face Button").ToString(), EConsoleForGamepadLabels::None);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_FaceButton_Left, LOCTEXT("Gamepad_FaceButton_Left", "Left Face Button").ToString(), EConsoleForGamepadLabels::None);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_FaceButton_Right, LOCTEXT("Gamepad_FaceButton_Right", "Right Face Button").ToString(), EConsoleForGamepadLabels::None);
	
	// Specials
	AddFriendlyKeyNameRemap(EKeys::Gamepad_Special_Left, LOCTEXT("Gamepad_Special_Left", "Back").ToString(), EConsoleForGamepadLabels::None);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_Special_Right, LOCTEXT("Gamepad_Special_Right", "Start").ToString(), EConsoleForGamepadLabels::None);

	// Triggers/Shoulder
	AddFriendlyKeyNameRemap(EKeys::Gamepad_LeftShoulder, LOCTEXT("Gamepad_LeftShoulder", "Left Shoulder").ToString(), EConsoleForGamepadLabels::None);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_RightShoulder, LOCTEXT("Gamepad_RightShoulder", "Right Shoulder").ToString(), EConsoleForGamepadLabels::None);
	
	Remap = AddFriendlyKeyNameRemap(EKeys::Gamepad_LeftTriggerAxis, LOCTEXT("Gamepad_LeftTrigger", "Left Trigger").ToString(), EConsoleForGamepadLabels::None);
	Remap->AddUnique(EKeys::Gamepad_LeftTrigger); 

	Remap = AddFriendlyKeyNameRemap(EKeys::Gamepad_RightTriggerAxis, LOCTEXT("Gamepad_RightTrigger", "Right Trigger").ToString(), EConsoleForGamepadLabels::None);
	Remap->AddUnique(EKeys::Gamepad_RightTrigger);
		
	// Analog sticks
	AddFriendlyKeyNameRemap(EKeys::Gamepad_LeftThumbstick, LOCTEXT("Gamepad_LeftThumbstick", "L Click Stick").ToString(), EConsoleForGamepadLabels::None);
	AddFriendlyKeyNameRemap(EKeys::Gamepad_RightThumbstick, LOCTEXT("Gamepad_RightThumbstick", "R Stick Click").ToString(), EConsoleForGamepadLabels::None);

 	Remap = AddFriendlyKeyNameRemap(EKeys::Gamepad_LeftX, LOCTEXT("Gamepad_Left_Stick_X", "Left Stick X Axis").ToString(), EConsoleForGamepadLabels::None);
	Remap->AddUnique(EKeys::Gamepad_LeftStick_Left);
	Remap->AddUnique(EKeys::Gamepad_LeftStick_Right);

	Remap = AddFriendlyKeyNameRemap(EKeys::Gamepad_LeftY, LOCTEXT("Gamepad_Left_Stick_Y", "Left Stick Y Axis").ToString(), EConsoleForGamepadLabels::None);
	Remap->AddUnique(EKeys::Gamepad_LeftStick_Down);
	Remap->AddUnique(EKeys::Gamepad_LeftStick_Up);

	Remap = AddFriendlyKeyNameRemap(EKeys::Gamepad_RightX, LOCTEXT("Gamepad_Right_Stick_X", "Right Stick X Axis").ToString(), EConsoleForGamepadLabels::None);	
	Remap->AddUnique(EKeys::Gamepad_RightStick_Left);
	Remap->AddUnique(EKeys::Gamepad_RightStick_Right);

	Remap = AddFriendlyKeyNameRemap(EKeys::Gamepad_RightY, LOCTEXT("Gamepad_Right_Stick_Y", "Right Stick Y Axis").ToString(), EConsoleForGamepadLabels::None);
	Remap->AddUnique(EKeys::Gamepad_RightStick_Down);
	Remap->AddUnique(EKeys::Gamepad_RightStick_Up);
}

TArray<FKey>* SVehicleControlsSetup::AddFriendlyKeyNameRemap(const FKey& KeyID, const FString& InFriendlyName, const EConsoleForGamepadLabels::Type InControllerType)
{
	if (FriendlyNameLookup.Contains(InControllerType) == false)
	{
		FriendlyNameLookup.Add(InControllerType);
	}
	
	ensureMsgf(FriendlyNameLookup[InControllerType].Contains(InFriendlyName) == false, TEXT("Adding duplicate string '%s'"), *InFriendlyName);
	FriendlyNameLookup[InControllerType].Add(InFriendlyName);
	FriendlyNameLookup[InControllerType][InFriendlyName].Add(KeyID);
	
	return &FriendlyNameLookup[InControllerType][InFriendlyName];
}

//Temporary solution until some proper mapping is done in engine
FText SVehicleControlsSetup::GetKeyText(FKey Key) const
{
	if (FriendlyNameLookup.Contains(EKeys::ConsoleForGamepadLabels) == true)
	{
		for (auto Iter(FriendlyNameLookup[EKeys::ConsoleForGamepadLabels].CreateConstIterator()); Iter; ++Iter)
		{
			FString KeyString = Iter.Key();
			auto KeyList = Iter.Value();
			if( Iter.Value().Contains(Key) )
			{
				return FText::FromString(KeyString);
			}
		}
	}
	return Key.GetDisplayName();
}

FText SVehicleControlsSetup::GetItemKeyBinding(TSharedPtr<FActionBinding> Item) const
{
	FText ResultText = LOCTEXT("None","NONE");
	if (bAwaitingKeyPress && SelectedItem == Item)
	{
		ResultText = LOCTEXT("PressAnyKey","PRESS ANY KEY...");
	} 
	else if (Item->InputMapping != nullptr)
	{
		ResultText = GetKeyText(Item->InputMapping->Key);
	} 
	else if (Item->AxisMapping != nullptr)
	{
		ResultText = GetKeyText(Item->AxisMapping->Key);
	}

	return  ResultText;
}

BindingRestriction* SVehicleControlsSetup::AddBindingRestriction(const FString& InControlName, bool InAxisAllowed, bool InActionAllowed)
{
	// Check if we have already assigned a restriction to this control
	for (int32 Bind = 0; Bind < BindingRestrictions.Num(); Bind++)
	{
		if (BindingRestrictions[Bind].RestrictionIdent == InControlName)
		{
			return &BindingRestrictions[Bind];
		}
	}

	// Check if the control exists in the bindings, and create a new restriction if it does
	UVehicleGameUserSettings* UserSettings = CastChecked<UVehicleGameUserSettings>(GEngine->GetGameUserSettings());	
	for (int32 Bind = 0; Bind < UserSettings->KeybindingsUIConfig.Num(); Bind++)
	{
		FKeybindingUIConfig& EachConfig = UserSettings->KeybindingsUIConfig[Bind];
		bool bAxisNameMatched = EachConfig.AxisName.ToString().Compare(InControlName) == 0;
		bool bActionNameMatched = EachConfig.ActionName.ToString().Compare(InControlName) == 0;
		if ((bActionNameMatched == true) || (bAxisNameMatched == true))
		{
			int32 Index = BindingRestrictions.Add(BindingRestriction(InControlName, InAxisAllowed, InActionAllowed));
			return &BindingRestrictions[Index];
		}
	}
	return nullptr;
}

bool SVehicleControlsSetup::IsBindingAllowed(TSharedPtr<FActionBinding> Binding, const FKey Key, bool bForcedAxis, bool bForcedAction)
{
	for (int32 Bind = 0; Bind < BindingRestrictions.Num(); Bind++)
	{
		const BindingRestriction& EachRestriction = BindingRestrictions[Bind];
		if ((Key.IsValid() == true ) && (Key.IsModifierKey() == false ) )
		{			
			const bool bKeyIsAxis = Key.IsFloatAxis() || Key.IsVectorAxis();
			const bool bKeyIsAction = !bKeyIsAxis;
			const FString AxisName = Binding.Get()->AxisMapping != nullptr ? Binding.Get()->AxisMapping->AxisName.ToString() : FString();
			const FString InputName = Binding.Get()->InputMapping != nullptr ? Binding.Get()->InputMapping->ActionName.ToString() : FString();
			const bool bNameMatched = ((EachRestriction.RestrictionIdent == AxisName) ||
				(EachRestriction.RestrictionIdent == InputName));
			if (bNameMatched == true )
			{
				if (EachRestriction.DisallowedKeys.Contains(Key) == false)
				{
					if (((bKeyIsAxis == true) || (bForcedAxis == true) ) && (EachRestriction.bAxisAllowed == false))
					{
						return false;
					}
					
					if (((bKeyIsAction == true) || (bForcedAction == true)) && (EachRestriction.bActionAllowed == false))
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
		}
	}
	return true;
}

#undef LOCTEXT_NAMESPACE


