// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "VehicleTrackPoint.generated.h"

/*
 * Tracking point used to ensure full laps are completed
 */
UCLASS()
class AVehicleTrackPoint : public AActor
{
	GENERATED_UCLASS_BODY()

#if WITH_EDITORONLY_DATA
private:
	UPROPERTY()
	UBillboardComponent* SpriteComponent;

	UPROPERTY()
	UArrowComponent* ArrowComponent;
public:
#endif

	/** set checkpoint for touching vehicle */
	virtual void NotifyActorBeginOverlap(AActor* Other) override;

#if WITH_EDITORONLY_DATA
	/** Returns SpriteComponent subobject **/
	FORCEINLINE UBillboardComponent* GetSpriteComponent() const { return SpriteComponent; }
	/** Returns ArrowComponent subobject **/
	FORCEINLINE UArrowComponent* GetArrowComponent() const { return ArrowComponent; }
#endif
};
