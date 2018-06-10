// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleGame.h"
#include "Pawns/BuggyPawn.h"
#include "VehicleWheel.h"

#include "Particles/ParticleSystemComponent.h"
#include "Player/VehiclePlayerController.h"
#include "WheeledVehicleMovementComponent.h"
#include "Track/VehicleTrackPoint.h"
#include "Effects/VehicleImpactEffect.h"
#include "Effects/VehicleDustType.h"

#include "AudioThread.h"

TMap<uint32, ABuggyPawn::FVehicleDesiredRPM> ABuggyPawn::BuggyDesiredRPMs;

ABuggyPawn::ABuggyPawn(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer)
{
	/** Camera strategy:
	 *  We want to keep a constant distance between car's location and camera.
	 *  We want to keep roll and pitch fixed
	 *	We want to interpolate yaw very slightly
	 *	We want to keep car almost constant in screen space width and height (i.e. if you draw a box around the car its center would be near constant and its dimensions would only vary on sharp turns or declines */

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->TargetOffset = FVector(0.f, 0.f, 400.f);
	SpringArm->SetRelativeRotation( FRotator(0.f, 0.f, 0.f) );
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 675.0f; 
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 7.f;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;	

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;

	EngineAC = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineAudio"));
	EngineAC->SetupAttachment(GetMesh());

	SkidAC = CreateDefaultSubobject<UAudioComponent>(TEXT("SkidAudio"));
	SkidAC->bAutoActivate = false;	//we don't want to start skid right away
	SkidAC->SetupAttachment(GetMesh());
	SkidThresholdVelocity = 30;
	SkidFadeoutTime = 0.1f;
	LongSlipSkidThreshold = 0.3f;
	LateralSlipSkidThreshold = 0.3f;
	SkidDurationRequiredForStopSound = 1.5f;
	
	SpringCompressionLandingThreshold = 250000.f;
	bTiresTouchingGround = false;

	ImpactEffectNormalForceThreshold = 100000.f;
}

void ABuggyPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (EngineAC)
	{
		EngineAC->SetSound(EngineSound);
		EngineAC->Play();
	}

	if (SkidAC)
	{
		SkidAC->SetSound(SkidSound);
		SkidAC->Stop();
	}
}

void ABuggyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABuggyPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABuggyPawn::MoveRight);

	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &ABuggyPawn::OnHandbrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &ABuggyPawn::OnHandbrakeReleased);
}

void ABuggyPawn::MoveForward(float Val)
{
	AVehiclePlayerController* MyPC = Cast<AVehiclePlayerController>(GetController());
	UWheeledVehicleMovementComponent* VehicleMovementComp = GetVehicleMovementComponent();
	if (VehicleMovementComp == nullptr || (MyPC && MyPC->IsHandbrakeForced()))
	{
		return;
	}

	VehicleMovementComp->SetThrottleInput(Val);
}

void ABuggyPawn::MoveRight(float Val)
{
	AVehiclePlayerController* MyPC = Cast<AVehiclePlayerController>(GetController());
	UWheeledVehicleMovementComponent* VehicleMovementComp = GetVehicleMovementComponent();
	if (VehicleMovementComp == nullptr || (MyPC && MyPC->IsHandbrakeForced()))
	{
		return;
	}
	VehicleMovementComp->SetSteeringInput(Val);
}

void ABuggyPawn::OnHandbrakePressed()
{
	AVehiclePlayerController *VehicleController = Cast<AVehiclePlayerController>(GetController());
	UWheeledVehicleMovementComponent* VehicleMovementComp = GetVehicleMovementComponent();
	if (VehicleMovementComp != nullptr)
	{
		VehicleMovementComp->SetHandbrakeInput(true);
	}
}

void ABuggyPawn::OnHandbrakeReleased()
{
	bHandbrakeActive = false;
	UWheeledVehicleMovementComponent* VehicleMovementComp = GetVehicleMovementComponent();
	if (VehicleMovementComp != nullptr)
	{
		GetVehicleMovementComponent()->SetHandbrakeInput(false);
	}
}

void ABuggyPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateWheelEffects(DeltaSeconds);

	if (AVehiclePlayerController* VehiclePC = Cast<AVehiclePlayerController>(GetController()))
	{
		const uint32 VehiclePCID = VehiclePC->GetUniqueID();
		FVehicleDesiredRPM DesiredRPM;
		DesiredRPM.DesiredRPM = GetEngineRotationSpeed();
		DesiredRPM.TimeStamp = GetWorld()->GetTimeSeconds();
		FAudioThread::RunCommandOnAudioThread([VehiclePCID, DesiredRPM]()
		{
			BuggyDesiredRPMs.Add(VehiclePCID, DesiredRPM);
		});
	}
}

void ABuggyPawn::UnPossessed()
{
	if (AVehiclePlayerController* VehiclePC = Cast<AVehiclePlayerController>(GetController()))
	{
		const uint32 VehiclePCID = VehiclePC->GetUniqueID();
		FAudioThread::RunCommandOnAudioThread([VehiclePCID]()
		{
			BuggyDesiredRPMs.Remove(VehiclePCID);
		});
	}

	// Super clears controller, so do the behavior first
	Super::UnPossessed();
}

bool ABuggyPawn::GetVehicleDesiredRPM_AudioThread(const uint32 VehicleID, FVehicleDesiredRPM& OutDesiredRPM)
{
	check(IsInAudioThread());
	if (FVehicleDesiredRPM* DesiredRPM = BuggyDesiredRPMs.Find(VehicleID))
	{
		OutDesiredRPM = *DesiredRPM;
		return true;
	}
	return false;
}

void ABuggyPawn::SpawnNewWheelEffect(int WheelIndex)
{
	DustPSC[WheelIndex] = NewObject<UParticleSystemComponent>(this);
	DustPSC[WheelIndex]->bAutoActivate = true;
	DustPSC[WheelIndex]->bAutoDestroy = false;
	DustPSC[WheelIndex]->RegisterComponentWithWorld(GetWorld());
	DustPSC[WheelIndex]->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, GetVehicleMovement()->WheelSetups[WheelIndex].BoneName);
}

void ABuggyPawn::UpdateWheelEffects(float DeltaTime)
{
	if (GetVehicleMovement() && bTiresTouchingGround == false && LandingSound)	//we don't update bTiresTouchingGround until later in this function, so we can use it here to determine whether we're landing
	{
		float MaxSpringForce = GetVehicleMovement()->GetMaxSpringForce();
		if (MaxSpringForce > SpringCompressionLandingThreshold)
		{
			UGameplayStatics::PlaySoundAtLocation(this, LandingSound, GetActorLocation());
		}
	}

	bTiresTouchingGround = false;

	if (DustType && !bIsDying &&
		GetVehicleMovement() && GetVehicleMovement()->Wheels.Num() > 0)
	{
		const float CurrentSpeed = GetVehicleSpeed();
		for (int32 i = 0; i < ARRAY_COUNT(DustPSC); i++)
		{
			UPhysicalMaterial* ContactMat = GetVehicleMovement()->Wheels[i]->GetContactSurfaceMaterial();
			if (ContactMat != nullptr)
			{
				bTiresTouchingGround = true;
			}
			UParticleSystem* WheelFX = DustType->GetDustFX(ContactMat, CurrentSpeed);

			const bool bIsActive = DustPSC[i] != nullptr && !DustPSC[i]->bWasDeactivated && !DustPSC[i]->bWasCompleted;
			UParticleSystem* CurrentFX = DustPSC[i] != nullptr ? DustPSC[i]->Template : nullptr;
			if (WheelFX != nullptr && (CurrentFX != WheelFX || !bIsActive))
			{
				if (DustPSC[i] == nullptr || !DustPSC[i]->bWasDeactivated)
				{
					if (DustPSC[i] != nullptr)
					{
						DustPSC[i]->SetActive(false);
						DustPSC[i]->bAutoDestroy = true;
					}
					SpawnNewWheelEffect(i);
				}
				DustPSC[i]->SetTemplate(WheelFX);
				DustPSC[i]->ActivateSystem();
			}
			else if (WheelFX == nullptr && bIsActive)
			{
				DustPSC[i]->SetActive(false);
			}
		}
	}

	if (SkidAC != nullptr)
	{
		FVector Vel = GetVelocity();
		bool bVehicleStopped = Vel.SizeSquared2D() < SkidThresholdVelocity*SkidThresholdVelocity;
		bool TireSlipping = GetVehicleMovement()->CheckSlipThreshold(LongSlipSkidThreshold, LateralSlipSkidThreshold);
		bool bWantsToSkid = bTiresTouchingGround && !bVehicleStopped && TireSlipping;

		float CurrTime = GetWorld()->GetTimeSeconds();
		if (bWantsToSkid && !bSkidding)
		{
			bSkidding = true;
			SkidAC->Play();
			SkidStartTime = CurrTime;
		}
		if (!bWantsToSkid && bSkidding)
		{
			bSkidding = false;
			SkidAC->FadeOut(SkidFadeoutTime, 0);
			if (CurrTime - SkidStartTime > SkidDurationRequiredForStopSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, SkidSoundStop, GetActorLocation());
			}
		}
	}
}

void ABuggyPawn::OnTrackPointReached(AVehicleTrackPoint* NewCheckpoint)
{
	AVehiclePlayerController* MyPC = Cast<AVehiclePlayerController>(GetController());
	if (MyPC)
	{
		MyPC->OnTrackPointReached(NewCheckpoint);
	}
}

bool ABuggyPawn::IsHandbrakeActive() const
{
	return bHandbrakeActive;
}

float ABuggyPawn::GetVehicleSpeed() const
{
	return (GetVehicleMovement()) ? FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) : 0.0f;
}

float ABuggyPawn::GetEngineRotationSpeed() const
{
	return (GetVehicleMovement()) ? FMath::Abs(GetVehicleMovement()->GetEngineRotationSpeed()) : 0.0f;
}

float ABuggyPawn::GetEngineMaxRotationSpeed() const
{
	return (GetVehicleMovement()) ? FMath::Abs(GetVehicleMovement()->MaxEngineRPM) : 1.f;
}

void ABuggyPawn::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalForce, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalForce, Hit);

	if (ImpactTemplate && NormalForce.SizeSquared() > FMath::Square(ImpactEffectNormalForceThreshold))
	{
		FTransform const SpawnTransform(HitNormal.Rotation(), HitLocation);
		AVehicleImpactEffect* EffectActor = GetWorld()->SpawnActorDeferred<AVehicleImpactEffect>(ImpactTemplate, SpawnTransform);
		if (EffectActor)
		{
			float DotBetweenHitAndUpRotation = FVector::DotProduct(HitNormal, GetMesh()->GetUpVector());
			EffectActor->HitSurface = Hit;
			EffectActor->HitForce = NormalForce;
			EffectActor->bWheelLand = DotBetweenHitAndUpRotation > 0.8f;
			UGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
		}
	}

	if (ImpactCameraShake)
	{
		AVehiclePlayerController* PC = Cast<AVehiclePlayerController>(Controller);
		if (PC != nullptr && PC->IsLocalController())
		{
			PC->ClientPlayCameraShake(ImpactCameraShake, 1);
		}
	}
}

float ABuggyPawn::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Cast<APainCausingVolume>(DamageCauser) != nullptr)
	{
		Die();
	}

	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}


void ABuggyPawn::FellOutOfWorld(const class UDamageType& dmgType)
{
	Die();
}

bool ABuggyPawn::CanDie() const
{
	if ( bIsDying										// already dying
		|| IsPendingKill()								// already destroyed
		|| Role != ROLE_Authority)						// not authority
	{
		return false;
	}

	return true;
}

void ABuggyPawn::Die()
{
	if (CanDie())
	{
		OnDeath();
	}
}

void ABuggyPawn::OnRep_Dying()
{
	if (bIsDying == true)
	{
		OnDeath();
	}
}

void ABuggyPawn::TornOff()
{
	Super::TornOff();

	SetLifeSpan(1.0f);
}

void ABuggyPawn::OnDeath()
{
	AVehiclePlayerController* MyPC = Cast<AVehiclePlayerController>(GetController());
	bReplicateMovement = false;
	bTearOff = true;
	bIsDying = true;

	DetachFromControllerPendingDestroy();

	// hide and disable
	TurnOff();
	SetActorHiddenInGame(true);

	if (EngineAC)
	{
		EngineAC->Stop();
	}

	if (SkidAC)
	{
		SkidAC->Stop();
	}
	
	PlayDestructionFX();
	// Give use a finite lifespan
	SetLifeSpan( 0.2f );	
}

void ABuggyPawn::PlayDestructionFX()
{
	if (DeathFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, DeathFX, GetActorLocation(), GetActorRotation());
	}

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}
}

void ABuggyPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABuggyPawn, bIsDying);
}
