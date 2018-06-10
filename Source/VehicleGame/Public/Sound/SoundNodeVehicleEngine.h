// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Sound/SoundNode.h"
#include "SoundNodeVehicleEngine.generated.h"

USTRUCT()
struct FVehicleEngineDatum
{
	GENERATED_USTRUCT_BODY()

	/* The FadeInRPM at which to start hearing this sound. */
	UPROPERTY(EditAnywhere, Category=VehicleDatum )
	float FadeInRPMStart;

	/* RPM at which this sound has faded in completely. */
	UPROPERTY(EditAnywhere, Category=VehicleDatum )
	float FadeInRPMEnd;

	/* The FadeOutRPM is where this sound starts fading out. */
	UPROPERTY(EditAnywhere, Category=VehicleDatum )
	float FadeOutRPMStart;

	/* RPM at which this sound is no longer audible. */
	UPROPERTY(EditAnywhere, Category=VehicleDatum )
	float FadeOutRPMEnd;

	/** Pitch multiplier at end of RPM range **/
	UPROPERTY(EditAnywhere, Category=VehicleDatum)
	float MaxPitchMultiplier;

	FVehicleEngineDatum()
		: FadeInRPMStart(0)
		, FadeInRPMEnd(0)
		, FadeOutRPMStart(0)
		, FadeOutRPMEnd(0)
		, MaxPitchMultiplier(1.0f)
	{
	}
};

/**
 * Mix and shift pitch of samples depending on engine's RPM
 */
UCLASS(hidecategories=Object, editinlinenew)
class USoundNodeVehicleEngine : public USoundNode
{
	GENERATED_UCLASS_BODY()

	/**
	 * Each input needs to have the correct data filled in so the SoundNodeVehicleEngine is able
	 * to determine which sounds to play
	 */
	UPROPERTY(EditAnywhere, export, editfixedsize, Category=VehicleEngine)
	TArray<struct FVehicleEngineDatum> EngineSamples;

public:
	// Begin USoundNode interface. 
	virtual void ParseNodes( FAudioDevice* AudioDevice, const UPTRINT NodeWaveInstanceHash, FActiveSound& ActiveSound, const FSoundParseParameters& ParseParams, TArray<FWaveInstance*>& WaveInstances ) override;
	virtual int32 GetMaxChildNodes() const override 
	{ 
		return MAX_ALLOWED_CHILD_NODES;  
	}
	virtual void CreateStartingConnectors( void ) override;
	virtual void InsertChildNode( int32 Index ) override;
	virtual void RemoveChildNode( int32 Index ) override;
#if WITH_EDITOR
	/** Ensure amount of Cross Fade inputs matches new amount of children */
	virtual void SetChildNodes(TArray<USoundNode*>& InChildNodes) override;
#endif //WITH_EDITOR
	// End USoundNode interface. 

	void StoreCurrentRPM(FAudioDevice* AudioDevice, FActiveSound& ActiveSound, const FSoundParseParameters& ParseParams, float MaxRPM);

private:

	float CurrentMaxRPM;

	UPROPERTY()
	float CurrentRPM;

	float CurrentRPMStoreTime;
};
