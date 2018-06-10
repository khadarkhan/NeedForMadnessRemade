// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "VehicleTypes.h"
#include "WheeledVehicle.h"
#include "BuggyPawn.generated.h"

class AVehicleTrackPoint;
class UVehicleDustType;
class AVehicleImpactEffect;

UCLASS()
class ABuggyPawn : public AWheeledVehicle
{
	GENERATED_UCLASS_BODY()

	// Begin Actor overrides
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalForce, const FHitResult& Hit) override;
	virtual void FellOutOfWorld(const UDamageType& dmgType) override;
	// End Actor overrides

	// Begin Pawn overrides
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void TornOff() override;
	virtual void UnPossessed() override;
	// End Pawn overrides

	/** Identifies if pawn is in its dying state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, ReplicatedUsing = OnRep_Dying)
	uint32 bIsDying:1;

	/** replicating death on client */
	UFUNCTION()
	void OnRep_Dying();

	/** Returns True if the pawn can die in the current state */
	virtual bool CanDie() const;

	/** Kills pawn. [Server/authority only] */
	virtual void Die();

	/** Event on death [Server/Client] */
	virtual void OnDeath();

	/** notify about touching new checkpoint */
	void OnTrackPointReached(AVehicleTrackPoint* TrackPoint);

	/** is handbrake active? */
	UFUNCTION(BlueprintCallable, Category="Game|Vehicle")
	bool IsHandbrakeActive() const;

	/** get current speed */
	float GetVehicleSpeed() const;

	/** get current RPM */
	float GetEngineRotationSpeed() const;

	/** get maximum RPM */
	float GetEngineMaxRotationSpeed() const;
		
	//////////////////////////////////////////////////////////////////////////
	// Input handlers

	/** event call on handbrake input */
	void OnHandbrakePressed();
	void OnHandbrakeReleased();

	void MoveForward(float Val);
	void MoveRight(float Val);

	struct FVehicleDesiredRPM
	{
		float DesiredRPM;
		float TimeStamp;
	};

	static bool GetVehicleDesiredRPM_AudioThread(const uint32 VehicleID, FVehicleDesiredRPM& OutDesiredRPM);

private:
	/** Spring arm that will offset the camera */
	UPROPERTY(Category=Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	/** AudioThread authoritative cache of desired RPM keyed by owner ID for SoundNodeVehicleEngine to reference */
	static TMap<uint32, FVehicleDesiredRPM> BuggyDesiredRPMs;

protected:

	/** dust FX config */
	UPROPERTY(Category=Effects, EditDefaultsOnly)
	UVehicleDustType* DustType;

	/** impact FX config */
	UPROPERTY(Category=Effects, EditDefaultsOnly)
	TSubclassOf<AVehicleImpactEffect> ImpactTemplate;

	/** The minimum amount of normal force that must be applied to the chassis to spawn an Impact Effect */
	UPROPERTY(EditAnywhere, Category = Effects)
	float ImpactEffectNormalForceThreshold;

	/** explosion FX */
	UPROPERTY(Category=Effects, EditDefaultsOnly)
	UParticleSystem* DeathFX;

	/** explosion sound */
	UPROPERTY(Category=Effects, EditDefaultsOnly)
	USoundCue* DeathSound;

	/** engine sound */
	UPROPERTY(Category=Effects, EditDefaultsOnly)
	USoundCue* EngineSound;

private:
	/** audio component for engine sounds */
	UPROPERTY(Category = Effects, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAudioComponent* EngineAC;
protected:

	/** landing sound */
	UPROPERTY(Category = Effects, EditDefaultsOnly)
	USoundCue* LandingSound;

	/** dust FX components */
	UPROPERTY(Transient)
	UParticleSystemComponent* DustPSC[4];

	/** skid sound loop */
	UPROPERTY(Category=Effects, EditDefaultsOnly)
	USoundCue* SkidSound;

	/** skid sound stop */
	UPROPERTY(Category=Effects, EditDefaultsOnly)
	USoundCue* SkidSoundStop;

	/** skid fadeout time */
	UPROPERTY(Category = Effects, EditDefaultsOnly)
	float SkidFadeoutTime;

	/** skid effects cannot play if velocity is lower than this */
	UPROPERTY(Category=Effects, EditDefaultsOnly)
	float SkidThresholdVelocity;

	/** skid effects will play if absolute value of tire longitudinal slip is more than this. */
	UPROPERTY(Category = Effects, EditDefaultsOnly, meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0"))
	float LongSlipSkidThreshold;

	/** skid effects will play if absolute value of tire lateral slip is more than this. */
	UPROPERTY(Category = Effects, EditDefaultsOnly, meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0"))
	float LateralSlipSkidThreshold;

private:
	/** audio component for skid sounds */
	UPROPERTY()
	UAudioComponent* SkidAC;
protected:

	/** The amount of spring compression required during landing to play sound */
	UPROPERTY(Category = Effects, EditDefaultsOnly)
	float SpringCompressionLandingThreshold;

	/** whether tires are currently touching ground */
	bool bTiresTouchingGround;

	/** if skidding is shorter than this value, SkidSoundStop won't be played */
	UPROPERTY(Category=Effects, EditDefaultsOnly)
	float SkidDurationRequiredForStopSound;

	/** is vehicle currently skidding */
	bool bSkidding;

	/** time when skidding started */
	float SkidStartTime;

	/** camera shake on impact */
	UPROPERTY(Category=Effects, EditDefaultsOnly)
	TSubclassOf<UCameraShake> ImpactCameraShake;

	/** How much throttle forward (max 1.0f) or reverse (max -1.0f) */
	float ThrottleInput;

	/** How far the wheels are turned to the right (max 1.0f) or to the left (max -1.0f) */
	float TurnInput;

	/** is handbrake active? */
	uint32 bHandbrakeActive : 1;

	/** if key is being held to control the throttle, ignore other controllers */
	uint32 bKeyboardThrottle : 1;

	/** if key is being held to control the turning, ignore other controllers */
	uint32 bKeyboardTurn : 1;

	/** if turn left action key is pressed */
	uint32 bTurnLeftPressed : 1;

	/** if turn right action key is pressed */
	uint32 bTurnRightPressed : 1;

	/** if accelerate action key is pressed */
	uint32 bAcceleratePressed : 1;

	/** if break/reverse action key is pressed */
	uint32 bBreakReversePressed : 1;


	/** when entering new surface type, spawn new particle system, allowing old one to fade away nicely */
	void SpawnNewWheelEffect(int WheelIndex);

	/** update effects under wheels */
	void UpdateWheelEffects(float DeltaTime);

	/** Plays explosion particle and audio. */
	void PlayDestructionFX();

protected:
	/** Returns SpringArm subobject **/
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }
	/** Returns Camera subobject **/
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	/** Returns EngineAC subobject **/
	FORCEINLINE UAudioComponent* GetEngineAC() const { return EngineAC; }
	/** Returns SkidAC subobject **/
	FORCEINLINE UAudioComponent* GetSkidAC() const { return SkidAC; }
};



