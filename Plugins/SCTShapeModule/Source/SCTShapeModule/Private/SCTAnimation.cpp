
#include "SCTAnimation.h"
#include "SCTShape.h"
#include "Actor/SCTShapeActor.h"

FAnimation::FAnimation()
{
	ShapeRef = nullptr;
	State = 0;
	IsReverse = 0;
	Duration = 2;
	CurTime = 0;
}

FAnimation::~FAnimation()
{

}

void FAnimation::InitDefault(FSCTShape* InShape) {
	if (InShape == nullptr) return;
	
	ShapeRef = InShape;
	Duration = 1.2f;

	FTransform Transform = ShapeRef->GetShapeActor()->GetRootComponent()->GetRelativeTransform();
	AnimationInitPos = Transform.GetLocation();
	AnimationInitRot = Transform.GetRotation().Rotator();
}

void FAnimation::ReinitDefault()
{
	if (ShapeRef == nullptr) return;
	FTransform Transform = ShapeRef->GetShapeActor()->GetRootComponent()->GetRelativeTransform();
	AnimationInitPos = Transform.GetLocation();
	AnimationInitRot = Transform.GetRotation().Rotator();
}

void FAnimation::AnimationTick(float DeltaTime) {
	return;
}

void FAnimation::Stop() {
	State = IsReverse = 0;
	CurTime = 0;

	SetShapePos(AnimationInitPos);
	SetShapeRotator(AnimationInitRot);
}

bool FAnimation::IsInitState() const {
	return State == 0 && IsReverse == 0;
}

void FAnimation::Start() {
	if (ShapeRef == nullptr) { return; }

	if (State == 0) {
		State = 1;

		FTransform Transform = ShapeRef->GetShapeActor()->GetRootComponent()->GetRelativeTransform();
		AnimationStartPos = Transform.GetLocation();
		AnimationStartRot = Transform.GetRotation().Rotator();
	}
	else if (State == 1) {
		// 暂停
		State = 2;
	}
	else {
		State = 1;
	}
}

void FAnimation::Tick(float DeltaTime) {
	if (State == 1) {
		AnimationTick(DeltaTime);
	}
}

void FAnimation::SetShapePos(const FVector& InPos) {
	ShapeRef->GetShapeActor()->SetActorRelativeLocation(InPos);
}

void FAnimation::SetShapeRotator(const FRotator& InRotator) {
	ShapeRef->GetShapeActor()->SetActorRelativeRotation(InRotator);
}


FLeftOpenDoorAnimation::FLeftOpenDoorAnimation()
: Angle(0.0f)
{
}

void FLeftOpenDoorAnimation::SetAnimationParams(float InAngle)
{
	Angle = InAngle;
}

void FLeftOpenDoorAnimation::AnimationTick(float DeltaTime) {
	if (CurTime < Duration) 
	{
		float depth = ShapeRef->GetShapeDepth() * 0.1f;

		FVector InterpPos = FMath::InterpEaseOut(AnimationStartPos, 
			AnimationStartPos + FVector::ForwardVector * depth * (IsReverse ? -1 : 1),
			CurTime / Duration, 5.0f);
		SetShapePos(InterpPos);

		float InterpYaw = FMath::InterpEaseOut(AnimationStartRot.Yaw, 
			AnimationStartRot.Yaw + Angle * (IsReverse ? -1 : 1),
			CurTime / Duration, 5.0f);
		SetShapeRotator(FRotator(AnimationStartRot.Pitch, InterpYaw, AnimationStartRot.Roll));

		CurTime += DeltaTime;
	}
	else 
	{
		State = 0;
		CurTime = 0.0f;

		IsReverse = (IsReverse + 1)%2;
	}
}

FRightOpenDoorAnimation::FRightOpenDoorAnimation()
: Angle(0.0f)
{

}

void FRightOpenDoorAnimation::SetAnimationParams(float InAngle)
{
	Angle = InAngle;
}

void FRightOpenDoorAnimation::AnimationTick(float DeltaTime) {
	if (CurTime < Duration) {
		float depth = ShapeRef->GetShapeDepth() * 0.1f;
		float DoorWidth = ShapeRef->GetShapeWidth() * 0.1f;

		FVector RotStartVector = AnimationStartRot.RotateVector(FVector::ForwardVector);
		FVector RotCenter = AnimationStartPos + RotStartVector * DoorWidth;

		FVector RotBaseVector = AnimationInitRot.RotateVector(FVector::ForwardVector);
		RotCenter = FMath::InterpEaseOut(RotCenter, RotCenter + RotBaseVector * depth * (IsReverse ? 1 : -1), CurTime / Duration, 5.0f);

		float InterpYaw = FMath::InterpEaseOut(0.0f, Angle, CurTime / Duration, 5.0f);
		FRotator Rotator = AnimationStartRot;
		Rotator.Yaw += InterpYaw * (IsReverse ? 1 : -1);

		FVector RotVector = Rotator.RotateVector(FVector::ForwardVector);
		FVector OriginalPos = RotCenter - RotVector * DoorWidth;

		SetShapePos(OriginalPos);
		SetShapeRotator(Rotator);

		CurTime += DeltaTime;
	}
	else {
		State = 0;
		CurTime = 0.0f;
		
		IsReverse = (IsReverse + 1) % 2;
	}
}

FTopOpenDoorAnimation::FTopOpenDoorAnimation()
: Angle(0.0f)
{}

void FTopOpenDoorAnimation::SetAnimationParams(float InAngle)
{
	Angle = InAngle;
}

void FTopOpenDoorAnimation::AnimationTick(float DeltaTime)
{
	if (CurTime < Duration) {
		float depth = ShapeRef->GetShapeDepth() * 0.1f;
		float DoorHeight = ShapeRef->GetShapeHeight() * 0.1f;

		FVector RotStartVector = AnimationStartRot.RotateVector(FVector::UpVector);
		FVector RotCenter = AnimationStartPos + RotStartVector * DoorHeight;

		FVector RotBaseVector = AnimationInitRot.RotateVector(FVector::UpVector);
		RotCenter = FMath::InterpEaseOut(RotCenter, RotCenter + RotBaseVector * depth * (IsReverse ? 1 : -1), CurTime / Duration, 5.0f);

		float InterpRoll = FMath::InterpEaseOut(0.0f, Angle, CurTime / Duration, 5.0f);
		FRotator Rotator = AnimationStartRot;
		Rotator.Roll += InterpRoll * (IsReverse ? 1 : -1);

		FVector RotVector = Rotator.RotateVector(FVector::UpVector);
		FVector OriginalPos = RotCenter - RotVector * DoorHeight;

		SetShapePos(OriginalPos);
		SetShapeRotator(Rotator);

		CurTime += DeltaTime;
	}
	else {
		State = 0;
		CurTime = 0.0f;
		
		IsReverse = (IsReverse + 1) % 2;
	}
}

FDrawerAnimation::FDrawerAnimation()
{

}

void FDrawerAnimation::AnimationTick(float DeltaTime) {
	if (CurTime < Duration) {

		float length = ShapeRef->GetShapeDepth() * 0.0667f;

		FVector InterpPos = FMath::InterpEaseOut(AnimationStartPos,
			AnimationStartPos + FVector::RightVector * length * (IsReverse ? -1 : 1),
			CurTime / Duration, 5.0f);

		SetShapePos(InterpPos);

		CurTime += DeltaTime;
	}
	else {
		State = 0;
		CurTime = 0.0f;

		IsReverse = (IsReverse + 1) % 2;
	}
}