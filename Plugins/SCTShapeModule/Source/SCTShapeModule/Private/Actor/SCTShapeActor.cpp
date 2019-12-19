#include "SCTShapeActor.h"
#include "SCTBoardActor.h"
#include "SCTModelActor.h"
#include "SCTCompActor.h"
#include "SCTShape.h"
#include "Engine/Engine.h"
#include "SCTBoardShape.h"


ASCTShapeActor::ASCTShapeActor() 
: bCanDragByMouse(true),
ShapeData(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Shape"));
}

void ASCTShapeActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASCTShapeActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ASCTShapeActor::NotifyActorBeginCursorOver()
{
}

void ASCTShapeActor::NotifyActorEndCursorOver()
{
}

void ASCTShapeActor::NotifyActorOnClicked(FKey ButtonPressed)
{
}

void ASCTShapeActor::NotifyActorOnReleased(FKey ButtonReleased)
{
}

void ASCTShapeActor::SetShapeActorHiddenInGameRecursively(bool bHidden)
{
	AActor::SetActorHiddenInGame(bHidden);
	TArray<AActor*> AllChildrenActor;
	GetAttachedActors(AllChildrenActor);
	for (int32 i=0; i<AllChildrenActor.Num(); ++i)
	{
		if (AllChildrenActor[i]->IsA<ASCTShapeActor>())
		{
			ASCTShapeActor* CurChildShapeActor = StaticCast<ASCTShapeActor*>(AllChildrenActor[i]);
			CurChildShapeActor->SetShapeActorHiddenInGameRecursively(bHidden);
		}
	}
}

void ASCTShapeActor::UpdateActorDimension()
{
	FVector ActorSize(ShapeData->GetShapeWidth(), ShapeData->GetShapeDepth(), ShapeData->GetShapeHeight());
}
void ASCTShapeActor::UpdateActorPosition()
{
	FVector ActorPos(ShapeData->GetShapePosX(), ShapeData->GetShapePosY(), ShapeData->GetShapePosZ());
	SetActorRelativeLocation(ActorPos / 10.f);
}
void ASCTShapeActor::UpdateActorRotation()
{
	FRotator ActorRot(ShapeData->GetShapeRotY(), ShapeData->GetShapeRotZ(), ShapeData->GetShapeRotX());
	SetActorRelativeRotation(ActorRot);
}

void ASCTShapeActor::SetActorPosition(const FVector& InPos)
{
	SetActorRelativeLocation(InPos / 10.f);
}

FVector ASCTShapeActor::GetActorPosition() const
{
	return RootComponent->GetRelativeTransform().GetLocation() * 10.0f;
}

void ASCTShapeActor::SetActorRotator(const FRotator& InRotator)
{
	SetActorRelativeRotation(InRotator);
}

FRotator ASCTShapeActor::GetActorRotator() const
{
	return RootComponent->GetRelativeTransform().GetRotation().Rotator();
}

void ASCTShapeActor::SetShapeMoveDir(EActorMoveDir Dir)
{
	ActorMoveDir = Dir;
}

EActorMoveDir ASCTShapeActor::GetShapeMoveDir()
{
	return ActorMoveDir;
}

FVector ASCTShapeActor::GetBoxCenter() const
{
	return GetCollisionBox().GetCenter();
}

void ASCTShapeActor::UpdateActorMouseDragState()
{
	// 比较直接的方式来检测公式是否包含变量
	auto CheckIsOnlyNumber = [](const FString &Str) -> bool {
		TCHAR Zero = TEXT('0');
		TCHAR Nine = TEXT('9');
		TCHAR Null = TEXT('\0');
		for (auto &c : Str.GetCharArray()) {
			if (c >= Zero && c <= Nine)
				continue;
			else if (c == Null)
				continue;
			else
				return false;
		}
		return true;
	};
}

void ASCTShapeActor::SetShape(FSCTShape* InShape)
{
	ShapeData = InShape;
}

FSCTShape* ASCTShapeActor::GetShape()
{
	return ShapeData;
}

void ASCTShapeActor::AttachToActorOverride(ASCTShapeActor *ParentActor, 
	const FAttachmentTransformRules &AttachmentRules, FName SockerName/* = NAME_None*/)
{
	AttachToActor(ParentActor, AttachmentRules, SockerName);
	ActorAttachedTo = ParentActor;
}

void ASCTShapeActor::DetachFromActorOverride(const FDetachmentTransformRules& DetachmentRules)
{
	DetachFromActor(DetachmentRules);
	ActorAttachedTo = nullptr;
}

void ASCTShapeActor::DestroyActorWithChildren()
{
	DestroyActorWithChildrenRecursive(this);
}

bool ASCTShapeActor::GetVisibleStateFromParent() const
{
	if (ActorAttachedTo)
	{
		return ActorAttachedTo->bHidden == 0;
	}
	return true;
}

void ASCTShapeActor::DestroyActorWithChildrenRecursive(AActor *Actor)
{
	TArray<AActor*> AttachedActors;
	Actor->GetAttachedActors(AttachedActors);
	for (int32 Child = 0; Child < AttachedActors.Num(); ++Child)
	{
		DestroyActorWithChildrenRecursive(AttachedActors[Child]);
	}
	Actor->Destroy();
}

