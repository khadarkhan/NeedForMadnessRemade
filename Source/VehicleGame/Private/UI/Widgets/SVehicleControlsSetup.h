// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "VehicleTypes.h"

class AVehicleHUD;

struct FActionBinding
{
	FKeybindingUIConfig* BindConfig;
	const FInputActionKeyMapping* InputMapping;
	const FInputAxisKeyMapping* AxisMapping;

	FActionBinding(FKeybindingUIConfig* InBindConfig, const FInputActionKeyMapping* InInputMapping)
	{
		BindConfig = InBindConfig;
		InputMapping = InInputMapping;
		AxisMapping = nullptr;
	}
	FActionBinding(FKeybindingUIConfig* InBindConfig, const FInputAxisKeyMapping* InAxisMapping)
	{
		BindConfig = InBindConfig;
		InputMapping = nullptr;
		AxisMapping = InAxisMapping;
	}
};

// Key binding restriction helper
struct BindingRestriction
{
	BindingRestriction(const FString& InRestrictionIdent, bool InAxisAllowed, bool InActionAllowed)
	{
		RestrictionIdent = InRestrictionIdent;
		bAxisAllowed = InAxisAllowed;
		bActionAllowed = InActionAllowed;		
	};
	
	// Identification of the binding to restrict
	FString		RestrictionIdent;
	
	// Set to false if we dont allow any AXIS controls to be bound
	uint32		bAxisAllowed : 1;

	// Set to false if we dont allow any ACTION controls to be bound
	uint32		bActionAllowed : 1;

	// Specifically disallowed keys. 
	TArray<FKey> DisallowedKeys;
};


// Class for managing the remapping of controls
class SVehicleControlsSetup : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SVehicleControlsSetup)
	{}

	SLATE_ARGUMENT(TWeakObjectPtr<AVehicleHUD>, OwnerHUD)
	SLATE_ARGUMENT(TSharedPtr<SWidget>, OwnerWidget)

	SLATE_END_ARGS()

	/** needed for every widget */
	void Construct(const FArguments& InArgs);

	/** if we want to receive focus */
	virtual bool SupportsKeyboardFocus() const override { return true; }

	/** focus received handler - keep the ActionBindingsList focused */
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;
	
	/** focus lost handler - keep the ActionBindingsList focused */
	virtual void OnFocusLost( const FFocusEvent& InFocusEvent ) override;

	/** key down handler */
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	/** Called when an analog control changes  */
	virtual FReply OnAnalogValueChanged(const FGeometry& MyGeometry, const FAnalogInputEvent& InAnalogInputEvent) override;

	/** SListView item double clicked */
	void OnListItemDoubleClicked(TSharedPtr<FActionBinding> InItem);

	/** Creates single item widget, called for every list item */
	TSharedRef<ITableRow> MakeListViewWidget(TSharedPtr<FActionBinding> Item, const TSharedRef<STableViewBase>& OwnerTable);

	/** Selection changed handler */
	void KeyMapSelectionChanged(TSharedPtr<FActionBinding> InItem, ESelectInfo::Type SelectInfo);

	/** Getter for key binding text */
	FText GetItemKeyBinding(TSharedPtr<FActionBinding> Item) const;

	/** Fill/update action binding list, should be called before showing this control */
	void UpdateActionBindings(FName GroupName);

	/** Selects item at current + MoveBy index */
	void MoveSelection(int32 MoveBy);

protected:
	FText GetKeyText(FKey Key) const;
	
	/* Populate the list of keyname maps to make the internal key names more end user friendly */
	void PopulateFriendlyKeyNameMap();
	
	/* Map of keys that relate to a given string */
	typedef TMap< FString, TArray<FKey> >	StringRemapLookup;

	/* Per console string map map type */
	typedef TMap< EConsoleForGamepadLabels::Type, StringRemapLookup >	PerConsoleKeyNames;
	
	/* Key name remaping map */
	PerConsoleKeyNames FriendlyNameLookup;

	/* 
	 * Add a string entry than can be used to replace keynames
	 *
	 * @param	KeyId				ID of the key
	 * @param	FriendlyName		End-User friendly name to represent the key
	 * @param	InControllerType	Controller type
	 * @returns	Array of keys to which the given string should represent.
	 */
	TArray<FKey>* AddFriendlyKeyNameRemap(const FKey& KeyID, const FString& FriendlyName, const EConsoleForGamepadLabels::Type InControllerType);

	/** 
	 * Remaps key binding and saves config 
	 *
	 * @param	KeyId				ID of the key
	 * @param	FriendlyName		End-User friendly name to represent the key
	 * @param	InControllerType	Controller type
	 * @param	IsForcedAxis		Is the remap coming from an AXIS only source (EG Analog stick)
	 * @param	bool IsForcedAction	Is the remap coming from an ACTION only source (EG Keyboard)
	 *
	 */
	void RemapKey(TSharedPtr<FActionBinding> Binding, const FKey Key, bool IsForcedAxis, bool IsForcedAction);

	/*
	* Restrict a key binding to an axis, action or exclude specific keys
	*
	* @param	InControlName		Name of the control to restrict
	* @param	bAxisAllowed		set true to allow AXIS bindings for control
	* @param	bActionAllowed		set true to allow ACTION bindings for control
	* @returns	Pointer to restriction struct to which specific keys can be added, or nullptr if the key mapping does not exist in the settings.
	*/
	BindingRestriction* AddBindingRestriction(const FString& InControlName, bool bAxisAllowed, bool bActionAllowed);

	/*
	* Is this binding allowed*
	*
	* @param	Binding			
	* @param	Key				
	* @param	bForcedAxis	
	* @param	bForcedAction
	* @returns	true if the binding is OK
	*/
	bool IsBindingAllowed(TSharedPtr<FActionBinding> Binding, const FKey Key, bool bForcedAxis, bool bForcedAction);

	/** Action bindings array */
	TArray< TSharedPtr<FActionBinding> > ActionBindings;

	/** action bindings list slate widget */
	TSharedPtr< SListView< TSharedPtr<FActionBinding> > > ActionBindingsList; 

	/** Currently selected list item */
	TSharedPtr<FActionBinding> SelectedItem;

	/** Flag if we are awaiting key press or cancel (Esc) */
	bool bAwaitingKeyPress;

	/** Current control group */
	FName CurrentGroup;

	/** Pointer to our parent HUD */
	TWeakObjectPtr<class AVehicleHUD> OwnerHUD;

	/** Pointer to our parent widget */
	TSharedPtr<class SWidget> OwnerWidget;

	/** Style for this menu item */
	const struct FVehicleMenuItemStyle *MenuItemStyle;

	/* List of binding restrictions */
	TArray< BindingRestriction > BindingRestrictions;
};


