#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
//#include "SteamVRFunctionLibrary.h"
#include "ArmyPawn.generated.h"

UCLASS()
class XR_API AXRPawn : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AXRPawn(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		UStaticMeshComponent* LeftArmMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		UStaticMeshComponent* RightArmMesh;

	float BaseTurnRate;
	float BaseLookUpRate;

	float HalfCapsuleHeight;

	//@马云龙 测试数据
	FVector Test_HMDLoc;
	FRotator Test_HMDRot;

public:

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult);	
	void GetActorDebug(TArray<FString>& DebugInfo);
	virtual void DisplayDebug(class UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
	//protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MoveForward(float Val);
	void MoveRight(float Val);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
