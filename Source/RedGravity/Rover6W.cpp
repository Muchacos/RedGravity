// Fill out your copyright notice in the Description page of Project Settings.


#include "Rover6W.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/WheeledVehicleMovementComponentNW.h"

//Difinition for input names for avoid misswriting
static const FName NAME_SteerInput("Steer");
static const FName NAME_ThrottleInput("Throttle");

ARover6W::ARover6W(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UWheeledVehicleMovementComponentNW>(AWheeledVehicle::VehicleMovementComponentName))
{
	UWheeledVehicleMovementComponentNW* RoverMC = Cast<UWheeledVehicleMovementComponentNW>(GetVehicleMovement());

	//Adjust the number of wheels
	RoverMC->WheelNum = EWheelNum::WN_Six;

	//Adjust the tire loading
	RoverMC->MinNormalizedTireLoad = 0.f;
	RoverMC->MinNormalizedTireLoadFiltered = 0.2f;
	RoverMC->MaxNormalizedTireLoad = 2.f;
	RoverMC->MaxNormalizedTireLoadFiltered = 2.f;

	//Torque setup - how much torque we have on given RPM
	RoverMC->MaxEngineRPM = 5700.f;
	RoverMC->EngineSetup.TorqueCurve.GetRichCurve()->Reset();
	RoverMC->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(0.f, 400.f);
	RoverMC->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(1890.f, 500.f);
	RoverMC->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(5730.f, 400.f);

	//Steering setup
	RoverMC->SteeringCurve.GetRichCurve()->Reset();
	RoverMC->SteeringCurve.GetRichCurve()->AddKey(0.f, 1.f);
	RoverMC->SteeringCurve.GetRichCurve()->AddKey(40.f, 0.7f);
	RoverMC->SteeringCurve.GetRichCurve()->AddKey(120.f, 0.6f);

	RoverMC->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_4W;
	RoverMC->DifferentialSetup.FrontRearSplit = 0.65f;

	//Automatic Gearbox setup
	RoverMC->TransmissionSetup.bUseGearAutoBox = true;
	RoverMC->TransmissionSetup.GearSwitchTime = 0.15f;
	RoverMC->TransmissionSetup.GearAutoBoxLatency = 1.f;

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

void ARover6W::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Getiing the current vehicle speed and convert it from cm/s to km/h
	CurrentSpeed = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement())->GetForwardSpeed() * 0.036;
}

void ARover6W::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Bind input
	PlayerInputComponent->BindAxis(NAME_SteerInput, this, &ARover6W::ApplySteering);
	PlayerInputComponent->BindAxis(NAME_ThrottleInput, this, &ARover6W::ApplyThrottle);
	PlayerInputComponent->BindAxis("ViewVertical", this, &ARover6W::ViewVertical);
	PlayerInputComponent->BindAxis("ViewHorizontal", this, &ARover6W::ViewHorizontal);

	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &ARover6W::OnHandbrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &ARover6W::OnHandbrakeReleased);

}

void ARover6W::ApplyThrottle(float Throttle)
{
	GetVehicleMovementComponent()->SetThrottleInput(Throttle);
}

void ARover6W::ApplySteering(float Steering)
{
	GetVehicleMovementComponent()->SetSteeringInput(Steering);

}

void ARover6W::ViewVertical(float Pitch)
{
	if (abs(Pitch) > 0.001f)
	{
		AddControllerPitchInput(Pitch);
	}
}

void ARover6W::ViewHorizontal(float Yaw)
{
	if (abs(Yaw) > 0.001f)
	{
		AddControllerYawInput(Yaw);
	}
}

void ARover6W::OnHandbrakePressed()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void ARover6W::OnHandbrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);

}
