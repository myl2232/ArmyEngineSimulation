#include "ArmyShapeFrameActor.h"
#include "TypedPrimitive.h"
#include "Engine/CollisionProfile.h"
#include "ArmyWHCabinet.h"

AXRShapeFrame::AXRShapeFrame()
{
    PrimaryActorTick.bCanEverTick = false;

    FramePrimitive = CreateDefaultSubobject<UPNTPrimitive>("ArmyShapeFrame");
    FramePrimitive->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    UObject* MtlObj = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("Material'/Game/XRCommon/Material/M_BaseColorT.M_BaseColorT'"));
    if (MtlObj && MtlObj->IsA<UMaterialInterface>())
    {
        UMaterialInterface *Mtl = Cast<UMaterialInterface>(MtlObj);
        UMaterialInstanceDynamic *DynMtl = UMaterialInstanceDynamic::Create(Mtl, nullptr);
        DynMtl->SetScalarParameterValue("Opacity", 0.0f);
        FramePrimitive->SetMaterial(0, DynMtl);
    }

    FramePrimitive->bSelectable = 0;
    FramePrimitive->SetCollisionProfileName("BlockWHCShape");

    RootComponent = FramePrimitive;

    // Tags.Emplace(TEXT("Immovable"));
}

void AXRShapeFrame::RefreshFrame(float InWidth, float InDepth, float InHeight)
{
    float HalfWidth = InWidth * 0.5f;
    float Depth = InDepth;

	float FrontExt = ShapeInRoomRef->GetSunBoardWidth(1)*2.0f;
	float BackExt = ShapeInRoomRef->GetSunBoardWidth(0)*2.0f;

    TArray<FPVertex> Vertices = {
		FPVertex(FVector(-HalfWidth-BackExt, Depth, 0.0f)),
		FPVertex(FVector(-HalfWidth-BackExt, 0.0f, 0.0f)),
		FPVertex(FVector(HalfWidth+ FrontExt, Depth, 0.0f)),
		FPVertex(FVector(HalfWidth+ FrontExt, 0.0f, 0.0f)),
		FPVertex(FVector(-HalfWidth-BackExt, Depth, InHeight)),
		FPVertex(FVector(-HalfWidth-BackExt, 0.0f, InHeight)),
		FPVertex(FVector(HalfWidth+ FrontExt, Depth, InHeight)),
		FPVertex(FVector(HalfWidth+ FrontExt, 0.0f, InHeight))
	};

	TArray<uint16> Indices = {
		0, 4, 5,
		0, 5, 1,
		2, 7, 6,
        2, 3, 7,
        4, 7, 5,
        4, 6, 7,
        0, 1, 3,
        0, 3, 2,
        0, 6, 4,
        0, 2, 6,
        1, 5, 7,
        1, 7, 3
	};
	FramePrimitive->SetPrimitiveInfo(true, Vertices, Indices);
}