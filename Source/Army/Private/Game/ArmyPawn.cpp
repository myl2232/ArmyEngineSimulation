#include "ArmyPawn.h"
#include "GameFramework/PlayerInput.h"
#include "SteamVRFunctionLibrary.h"
#include "IArmyTrackingSystem.h"
#include "IHeadMountedDisplay.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AXRPawn::AXRPawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	HalfCapsuleHeight = 88.f;

	LeftArmMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("LeftArmMesh"));
	RightArmMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("RightArmMesh"));
	LeftArmMesh->SetupAttachment(GetCapsuleComponent());
	RightArmMesh->SetupAttachment(GetCapsuleComponent());

	GetCharacterMovement()->SetWalkableFloorAngle(80.f);
	GetCharacterMovement()->MaxStepHeight = 30.f;
	GetCharacterMovement()->JumpZVelocity = 200.f;

	LeftArmMesh->SetVisibility(false);
	RightArmMesh->SetVisibility(false);
}

void AXRPawn::CalcCamera(float DeltaTime, FMinimalViewInfo & OutResult)
{
	if (GEngine->XRSystem.IsValid() && GEngine->XRSystem->GetHMDDevice() && GEngine->XRSystem->GetHMDDevice()->IsHMDEnabled())
	{
		if (GEngine->StereoRenderingDevice->IsStereoEnabled())
		{
			FVector HMDLoc;
			FRotator HMDRot;
			USteamVRFunctionLibrary::GetTrackedDevicePositionAndOrientation(0, HMDLoc, HMDRot);
			OutResult.Location = GetActorLocation();
			OutResult.Rotation = HMDRot;
			OutResult.Location.Z -= HalfCapsuleHeight;
			return;
		}
	}

	FVector CameraLoc(0.f, 0.f, 0.f);
	FRotator CameraRot(0.f, 0.f, 0.f);
	GetActorEyesViewPoint(CameraLoc, CameraRot);

	//Super::CalcCamera(DeltaTime, OutResult);
	OutResult.Rotation.Roll = 0;
	OutResult.Location = CameraLoc;
	OutResult.Rotation = CameraRot;
}

void AXRPawn::GetActorDebug(TArray<FString>& DebugInfo)
{
	DebugInfo.Add("--------XR--------");
	DebugInfo.Add(TEXT("Test_HMDLoc：") + FString::Printf(TEXT("%.2f,%.2f,%.2f"), Test_HMDLoc.X, Test_HMDLoc.Y, Test_HMDLoc.Z));
	DebugInfo.Add(TEXT("Test_HMDRot：") + FString::Printf(TEXT("%d,%d,%d"), Test_HMDRot.Pitch, Test_HMDRot.Yaw, Test_HMDRot.Roll));
}

void AXRPawn::DisplayDebug(class UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	TArray<FString> DebugInfo;
	GetActorDebug(DebugInfo);
	UFont* RenderFont = GEngine->GetSmallFont();
	Canvas->SetDrawColor(255, 0, 0);
	int32 XPos = 4.f;
	for (int32 i = 0; i < DebugInfo.Num(); i++)
	{
		if (YPos > Canvas->SizeY)
		{
			YPos = 10.f;
			XPos += Canvas->SizeX / 2;
		}
		Canvas->DrawText(RenderFont, "	" + DebugInfo[i], XPos, YPos);
		YPos += YL;
	}
}

void AXRPawn::BeginPlay()
{
	Super::BeginPlay();
	BaseEyeHeight = 62.f;
	GetCapsuleComponent()->SetCapsuleHalfHeight(HalfCapsuleHeight);
}

void AXRPawn::MoveForward(float Val)
{
	if ((Controller != NULL) && (Val != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Val);
	}
}

void AXRPawn::MoveRight(float Val)
{
	if ((Controller != NULL) && (Val != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Val);
	}
}

void AXRPawn::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AXRPawn::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AXRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AXRPawn::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(InputComponent);

	static bool bBindingsAdded = false;
	if (!bBindingsAdded)
	{
		bBindingsAdded = true;

		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Jump", EKeys::V));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TurnRate", EKeys::E, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TurnRate", EKeys::Q, -1.f));

		InputComponent->BindAxis("Key_W", this, &AXRPawn::MoveForward);
		InputComponent->BindAxis("Key_S", this, &AXRPawn::MoveForward);
		InputComponent->BindAxis("Key_A", this, &AXRPawn::MoveRight);
		InputComponent->BindAxis("Key_D", this, &AXRPawn::MoveRight);

		//UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("PadX", EKeys::Gamepad_LeftThumbstick, 1.f));
		//UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("PadY", EKeys::Gamepad_LeftY, 1.f));
	}

	check(InputComponent);
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAxis("MoveForward", this, &AXRPawn::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AXRPawn::MoveRight);
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AXRPawn::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AXRPawn::LookUpAtRate);
}
