// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "Rover6W.generated.h"

/**
 * 
 */
UCLASS()
class REDGRAVITY_API ARover6W : public AWheeledVehicle
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int Score;

public:

	ARover6W(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	//Movement
	void ApplyThrottle(float Throttle);

	void ApplySteering(float Steering);

	//Look around
	void ViewVertical(float Pitch);

	void ViewHorizontal(float Yaw);

	//Handbreak
	void OnHandbrakePressed();

	void OnHandbrakeReleased();

protected:

	//Springarm that holds the camera
	UPROPERTY(Category = Camera, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	//Camera that will be our viewport
	UPROPERTY(Category = Camera, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

};
