// Fill out your copyright notice in the Description page of Project Settings.


#include "RoverPawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "WheeledVehicleMovementComponent4W.h"

//Difinition for input names for avoid misswriting
static const FName NAME_SteerInput("Steer");
static const FName NAME_ThrottleInput("Throttle");

ARoverPawn::ARoverPawn()
{
	UWheeledVehicleMovementComponent4W* Rover4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	//Adjust the tire loading
	Rover4W->MinNormalizedTireLoad = 0.f;
	Rover4W->MinNormalizedTireLoadFiltered = 0.2f;
	Rover4W->MaxNormalizedTireLoad = 2.f;
	Rover4W->MaxNormalizedTireLoadFiltered = 2.f;

	//Torque setup - how much torque we have on given RPM
	Rover4W->MaxEngineRPM = 5700.f;
	Rover4W->EngineSetup.TorqueCurve.GetRichCurve()->Reset();
	Rover4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(0.f, 400.f);
	Rover4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(1890.f, 500.f);
	Rover4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(5730.f, 400.f);

	//Steering setup
	Rover4W->SteeringCurve.GetRichCurve()->Reset();
	Rover4W->SteeringCurve.GetRichCurve()->AddKey(0.f, 1.f);
	Rover4W->SteeringCurve.GetRichCurve()->AddKey(40.f, 0.7f);
	Rover4W->SteeringCurve.GetRichCurve()->AddKey(120.f, 0.6f);

	Rover4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_4W;
	Rover4W->DifferentialSetup.FrontRearSplit = 0.65f;

	//Automatic Gearbox setup
	Rover4W->TransmissionSetup.bUseGearAutoBox = true;
	Rover4W->TransmissionSetup.GearSwitchTime = 0.15f;
	Rover4W->TransmissionSetup.GearAutoBoxLatency = 1.f;
	
	//Create a springarm that holds the camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 250.f;
	SpringArm->bUsePawnControlRotation = true;

	//Create the camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("DefaultCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->FieldOfView = 90.f;
}

void ARoverPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//Getiing the current vehicle speed and convert it from cm/s to km/h
	CurrentSpeed = int(CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement())->GetForwardSpeed() * 0.036);
}

void ARoverPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Bind input
	PlayerInputComponent->BindAxis(NAME_SteerInput, this, &ARoverPawn::ApplySteering);
	PlayerInputComponent->BindAxis(NAME_ThrottleInput, this, &ARoverPawn::ApplyThrottle);
	PlayerInputComponent->BindAxis("ViewVertical", this, &ARoverPawn::ViewVertical);
	PlayerInputComponent->BindAxis("ViewHorizontal", this, &ARoverPawn::ViewHorizontal);

	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &ARoverPawn::OnHandbrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &ARoverPawn::OnHandbrakeReleased);

}

void ARoverPawn::ApplyThrottle(float Throttle)
{
	GetVehicleMovementComponent()->SetThrottleInput(Throttle);
}

void ARoverPawn::ApplySteering(float Steering)
{
	GetVehicleMovementComponent()->SetSteeringInput(Steering);

}

void ARoverPawn::ViewVertical(float Pitch)
{
	if (abs(Pitch) > 0.001f)
	{
		AddControllerPitchInput(Pitch);
	}
}

void ARoverPawn::ViewHorizontal(float Yaw)
{
	if (abs(Yaw) > 0.001f)
	{
		AddControllerYawInput(Yaw);
	}
}

void ARoverPawn::OnHandbrakePressed()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void ARoverPawn::OnHandbrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);

}
