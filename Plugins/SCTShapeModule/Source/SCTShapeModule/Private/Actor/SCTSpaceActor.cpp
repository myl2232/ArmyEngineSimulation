#include "SCTSpaceActor.h"
#include "TypedPrimitive.h"
#include "SCTShape.h"
#include "ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/CollisionProfile.h"
#include "Engine/World.h"
#include "Components/LineBatchComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "Engine/Engine.h"
#include "SCTBoardShape.h"
#include "SCTSpaceShape.h"
#include "SCTOutline.h"
#include "MaterialManager.h"


ASCTSpaceActor::ASCTSpaceActor()
: fWidth(80.0),
fDepth(55.0),
fHeight(220.0)
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("BoundingBoxRoot"));

	BoundingBoxPrimitive = CreateDefaultSubobject<UPNTPrimitive>(TEXT("BoundingBoxPrimitive"));
	//BoundingBoxPrimitive->SetCollisionProfileName("BlockBoundingBox"/*UCollisionProfile::NoCollision_ProfileName*/);
	BoundingBoxPrimitive->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	WireframePrimitive = CreateDefaultSubobject<UPNTPrimitive>(TEXT("WireframePrimitive"));
	//WireframePrimitive->SetCollisionProfileName("BlockBoundingBox"/*UCollisionProfile::NoCollision_ProfileName*/);
	WireframePrimitive->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void ASCTSpaceActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASCTSpaceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASCTSpaceActor::SetShape(FSCTShape* InShape)
{
	ShapeData = InShape;
}
FSCTShape* ASCTSpaceActor::GetShape()
{
	return ShapeData;
}

void ASCTSpaceActor::UpdateActorDimension()
{
	fWidth = ShapeData->GetShapeWidth() * 0.1f ;
	fDepth = ShapeData->GetShapeDepth() * 0.1f ;
	fHeight = ShapeData->GetShapeHeight() * 0.1f;

	UpdatePrimitive();
}

void ASCTSpaceActor::SetLineMaterial(UMaterialInterface* InMaterial)
{
	WireframePrimitive->SetMaterial(0, InMaterial);
}

void ASCTSpaceActor::SetRegionMaterial(UMaterialInterface* InMaterial)
{
	BoundingBoxPrimitive->SetMaterial(0, InMaterial);
}

void ASCTSpaceActor::SetLineColor(const FLinearColor &InColor)
{
	UMaterialInstanceDynamic *MtlInstDyn = Cast<UMaterialInstanceDynamic>(WireframePrimitive->GetMaterial(0));
	if (MtlInstDyn)
	{
		MtlInstDyn->SetVectorParameterValue(TEXT("Emissive"), InColor);
	}
}

void ASCTSpaceActor::SetLineAlpha(float InAlpha)
{
	UMaterialInstanceDynamic *MtlInstDyn = Cast<UMaterialInstanceDynamic>(WireframePrimitive->GetMaterial(0));
	if (MtlInstDyn)
	{
		MtlInstDyn->SetScalarParameterValue(TEXT("Alpha"), InAlpha);
	}
}

void ASCTSpaceActor::SetRegionColor(const FLinearColor &InColor)
{
	UMaterialInstanceDynamic *MtlInstDyn = Cast<UMaterialInstanceDynamic>(BoundingBoxPrimitive->GetMaterial(0));
	if (MtlInstDyn)
	{
		MtlInstDyn->SetVectorParameterValue(TEXT("Emissive"), InColor);
	}
}

void ASCTSpaceActor::SetRegionAlpha(float InAlpha)
{
	UMaterialInstanceDynamic *MtlInstDyn = Cast<UMaterialInstanceDynamic>(BoundingBoxPrimitive->GetMaterial(0));
	if (MtlInstDyn)
	{
		MtlInstDyn->SetScalarParameterValue(TEXT("Alpha"), InAlpha);
	}
}

void ASCTSpaceActor::SetCollisionProfileName(FName InProfileName)
{
	BoundingBoxPrimitive->SetCollisionProfileName(InProfileName);
	WireframePrimitive->SetCollisionProfileName(InProfileName);
	UpdateActorDimension();
}

void ASCTSpaceActor::SetIsEnableCollision(bool bEnable)
{
	BoundingBoxPrimitive->SetCollisionProfileName(
		bEnable ? UCollisionProfile::BlockAll_ProfileName : UCollisionProfile::NoCollision_ProfileName);
}

void ASCTSpaceActor::UpdatePrimitive()
{
	check(ShapeData);
	FSpaceShape* SpaceShape = StaticCast<FSpaceShape*>(ShapeData);
	TSharedPtr<FSCTOutline> SpaceOutline = SpaceShape->GetSpaceOutline();
	check(SpaceOutline.IsValid());

	EOutlineType OutlineType = SpaceOutline->GetOutlineType();
	switch (OutlineType)
	{
	case OLT_LeftGirder:
	{
		UpdatePrimitiveForLeftGirder(SpaceOutline);
		break;
	}
	case OLT_RightGirder:
	{
		UpdatePrimitiveForRightGirder(SpaceOutline);
		break;
	}
	case OLT_BackGirder:
	{
		UpdatePrimitiveForBackGirder(SpaceOutline);
		break;
	}
	case OLT_LeftPiller:
	{
		UpdatePrimitiveForLeftPiller(SpaceOutline);
		break;
	}
	case OLT_RightPiller:
	{
		UpdatePrimitiveForRightPiller(SpaceOutline);
		break;
	}
	case OLT_MiddlePiller:
	{
		UpdatePrimitiveForMiddlePiller(SpaceOutline);
		break;
	}
	case OLT_CornerPiller:
	{
		UpdatePrimitiveForCornerPiller(SpaceOutline);
		break;
	}
	default:
		UpdatePrimitiveForNoneOutline(SpaceOutline);
		break;
	}
}

void ASCTSpaceActor::UpdatePrimitiveForNoneOutline(TSharedPtr<FSCTOutline> InOutline)
{
	TArray<FPVertex> Vertices = {
		FPVertex(FVector(0.0f, fDepth, 0.0f)),
		FPVertex(FVector(0.0f, 0.0f, 0.0f)),
		FPVertex(FVector(fWidth, 0.0f, 0.0f)),
		FPVertex(FVector(fWidth, fDepth, 0.0f)),
		FPVertex(FVector(0.0f, fDepth, fHeight)),
		FPVertex(FVector(0.0f, 0.0f, fHeight)),
		FPVertex(FVector(fWidth, 0.0f, fHeight)),
		FPVertex(FVector(fWidth, fDepth, fHeight))
	};

	TArray<uint16> Indices = {
		0, 5, 4,
		0, 1, 5,
		0, 4, 7,
		0, 7, 3,
		3, 7, 6,
		3, 6, 2,
		1, 0, 3,
		1, 3, 2,
		1, 6, 5,
		1, 2, 6,
		4, 5, 7,
		5, 6, 7
	};
	BoundingBoxPrimitive->SetPrimitiveInfo(true, Vertices, Indices);

	TArray<uint16> LineIndices = {
		4, 5,
		5, 6,
		6, 7,
		4, 7,
		0, 4,
		5, 1,
		6, 2,
		7, 3,
		0, 3,
		0, 1,
		1, 2,
		3, 2,
	};
	WireframePrimitive->SetPrimitiveInfo(false, Vertices, LineIndices);
}

void ASCTSpaceActor::UpdatePrimitiveForLeftGirder(TSharedPtr<FSCTOutline> InOutline)
{
	float GWidth = InOutline->GetOutlineParamValueByRefName(TEXT("GW")) * 0.1f;
	float GHeight = InOutline->GetOutlineParamValueByRefName(TEXT("GH")) * 0.1f;

	TArray<FPVertex> Vertices = {
		FPVertex(FVector(0.0f, 0.0f, 0.0f)),
		FPVertex(FVector(0.0f, 0.0f, fHeight - GHeight)),
		FPVertex(FVector(GWidth, 0.0f, fHeight - GHeight)),
		FPVertex(FVector(GWidth, 0.0f, fHeight)),
		FPVertex(FVector(fWidth, 0.0f, fHeight)),
		FPVertex(FVector(fWidth, 0.0f, 0.0f)),

		FPVertex(FVector(0.0f, fDepth, 0.0f)),
		FPVertex(FVector(0.0f, fDepth, fHeight - GHeight)),
		FPVertex(FVector(GWidth, fDepth, fHeight - GHeight)),
		FPVertex(FVector(GWidth, fDepth, fHeight)),
		FPVertex(FVector(fWidth, fDepth, fHeight)),
		FPVertex(FVector(fWidth, fDepth, 0.0f))
	};

	TArray<uint16> RigonIndices = {
		0, 2, 1,
		5, 2, 0,
		4, 2, 5,
		3, 2, 4,

		7, 8, 6,
		6, 8, 11,
		11, 8, 10,
		10, 8, 9,

		0, 1, 6,
		6, 1, 7,

		1, 2, 7,
		7, 2, 8,
		
		2, 3, 8,
		8, 3, 9,

		3, 4, 9,
		9, 4, 10,

		4, 5, 10,
		10, 5, 11,
		
		5, 0, 11,
		11, 0, 6
	};
	BoundingBoxPrimitive->SetPrimitiveInfo(true, Vertices, RigonIndices);

	TArray<uint16> LineIndices = {
		0, 1,
		1, 2,
		2, 3,
		3, 4,
		4, 5,
		5, 0,

		6, 7,
		7, 8,
		8, 9,
		9, 10,
		10, 11,
		11, 6, 

		0, 6, 
		1, 7, 
		2, 8, 
		3, 9, 
		4, 10, 
		5, 11
	};
	WireframePrimitive->SetPrimitiveInfo(false, Vertices, LineIndices);
}

void ASCTSpaceActor::UpdatePrimitiveForRightGirder(TSharedPtr<FSCTOutline> InOutline)
{
	float GWidth = InOutline->GetOutlineParamValueByRefName(TEXT("GW")) * 0.1f;
	float GHeight = InOutline->GetOutlineParamValueByRefName(TEXT("GH")) * 0.1f;

	TArray<FPVertex> Vertices = {
		FPVertex(FVector(0.0f, 0.0, 0.0f)),
		FPVertex(FVector(0.0f, 0.0f, fHeight)),
		FPVertex(FVector(fWidth - GWidth, 0.0f, fHeight)),
		FPVertex(FVector(fWidth - GWidth, 0.0f, fHeight - GHeight)),
		FPVertex(FVector(fWidth, 0.0f, fHeight - GHeight)),
		FPVertex(FVector(fWidth, 0.0f, 0.0f)),

		FPVertex(FVector(0.0f, fDepth, 0.0f)),
		FPVertex(FVector(0.0f, fDepth, fHeight)),
		FPVertex(FVector(fWidth - GWidth, fDepth, fHeight)),
		FPVertex(FVector(fWidth - GWidth, fDepth, fHeight - GHeight)),
		FPVertex(FVector(fWidth, fDepth, fHeight - GHeight)),
		FPVertex(FVector(fWidth, fDepth, 0.0f))
	};

	TArray<uint16> RigonIndices = {
		2, 3, 1,
		1, 3, 0,
		0, 3, 5,
		5, 3, 4,

		7, 9, 8,
		6, 9, 7,
		11, 9, 6,
		10, 9, 11,

		0, 6, 7,
		0, 7, 1,

		1, 7, 8,
		1, 8, 2,

		2, 8, 9,
		2, 9, 3,

		3, 9, 10,
		3, 10, 4,

		4, 10, 11,
		4, 11, 5,

		5, 11, 6,
		5, 6, 0
	};
	BoundingBoxPrimitive->SetPrimitiveInfo(true, Vertices, RigonIndices);

	TArray<uint16> LineIndices = {
		0, 1,
		1, 2,
		2, 3,
		3, 4,
		4, 5,
		5, 0,

		6, 7,
		7, 8,
		8, 9,
		9, 10,
		10, 11,
		11, 6,

		0, 6,
		1, 7,
		2, 8,
		3, 9,
		4, 10,
		5, 11
	};
	WireframePrimitive->SetPrimitiveInfo(false, Vertices, LineIndices);
}

void ASCTSpaceActor::UpdatePrimitiveForBackGirder(TSharedPtr<FSCTOutline> InOutline)
{
	float GDepth = InOutline->GetOutlineParamValueByRefName(TEXT("GD")) * 0.1f;
	float GHeight = InOutline->GetOutlineParamValueByRefName(TEXT("GH")) * 0.1f;

	TArray<FPVertex> Vertices = {
		FPVertex(FVector(0.0f, 0.0f, 0.0f)),
		FPVertex(FVector(0.0f, fDepth, 0.0f)),
		FPVertex(FVector(0.0f, fDepth, fHeight)),
		FPVertex(FVector(0.0f, GDepth, fHeight)),
		FPVertex(FVector(0.0f, GDepth, fHeight - GHeight)),
		FPVertex(FVector(0.0f, 0.0f, fHeight - GHeight)),

		FPVertex(FVector(fWidth, 0.0f, 0.0f)),
		FPVertex(FVector(fWidth, fDepth, 0.0f)),
		FPVertex(FVector(fWidth, fDepth, fHeight)),
		FPVertex(FVector(fWidth, GDepth, fHeight)),
		FPVertex(FVector(fWidth, GDepth, fHeight - GHeight)),
		FPVertex(FVector(fWidth, 0.0f, fHeight - GHeight))
	};

	TArray<uint16> RigonIndices = {
		0, 4, 5,
		1, 4, 0,
		2, 4, 1,
		3, 4, 2,

		11, 10, 6,
		6, 10, 7,
		7, 10, 8,
		8, 10, 9,

		0, 6, 7,
		0, 7, 1,

		1, 7, 8,
		1, 8, 2,

		2, 8, 9,
		2, 9, 3,

		3, 9, 10,
		3, 10, 4,

		4, 10, 11,
		4, 11, 5,

		5, 11, 6,
		5, 6, 0
	};
	BoundingBoxPrimitive->SetPrimitiveInfo(true, Vertices, RigonIndices);

	TArray<uint16> LineIndices = {
		0, 1,
		1, 2,
		2, 3,
		3, 4,
		4, 5,
		5, 0,

		6, 7,
		7, 8,
		8, 9,
		9, 10,
		10, 11,
		11, 6,

		0, 6,
		1, 7,
		2, 8,
		3, 9,
		4, 10,
		5, 11
	};
	WireframePrimitive->SetPrimitiveInfo(false, Vertices, LineIndices);
}

void ASCTSpaceActor::UpdatePrimitiveForLeftPiller(TSharedPtr<FSCTOutline> InOutline)
{
	float PWidth = InOutline->GetOutlineParamValueByRefName(TEXT("PW")) * 0.1f;
	float PDepth = InOutline->GetOutlineParamValueByRefName(TEXT("PD")) * 0.1f;

	TArray<FPVertex> Vertices = {
		FPVertex(FVector(PWidth, 0.0f, 0.0f)),
		FPVertex(FVector(fWidth, 0.0f, 0.0f)),
		FPVertex(FVector(fWidth, fDepth, 0.0f)),
		FPVertex(FVector(0.0f, fDepth, 0.0f)),
		FPVertex(FVector(0.0f, PDepth, 0.0f)),
		FPVertex(FVector(PWidth, PDepth, 0.0f)),

		FPVertex(FVector(PWidth, 0.0f, fHeight)),
		FPVertex(FVector(fWidth, 0.0f, fHeight)),
		FPVertex(FVector(fWidth, fDepth, fHeight)),
		FPVertex(FVector(0.0f, fDepth, fHeight)),
		FPVertex(FVector(0.0f, PDepth, fHeight)),
		FPVertex(FVector(PWidth, PDepth, fHeight))
	};

	TArray<uint16> RigonIndices = {
		1, 5, 0,
		2, 5, 1,
		3, 5, 2,
		4, 5, 3,

		6, 11, 7,
		7, 11, 8,
		8, 11, 9,
		9, 11, 10,

		0, 6, 7,
		0, 7, 1,

		1, 7, 8,
		1, 8, 2,

		2, 8, 9,
		2, 9, 3,

		3, 9, 10,
		3, 10, 4,

		4, 10, 11,
		4, 11, 5,

		5, 11, 6,
		5, 6, 0
	};
	BoundingBoxPrimitive->SetPrimitiveInfo(true, Vertices, RigonIndices);

	TArray<uint16> LineIndices = {
		0, 1,
		1, 2,
		2, 3,
		3, 4,
		4, 5,
		5, 0,

		6, 7,
		7, 8,
		8, 9,
		9, 10,
		10, 11,
		11, 6,

		0, 6,
		1, 7,
		2, 8,
		3, 9,
		4, 10,
		5, 11
	};
	WireframePrimitive->SetPrimitiveInfo(false, Vertices, LineIndices);
}

void ASCTSpaceActor::UpdatePrimitiveForRightPiller(TSharedPtr<FSCTOutline> InOutline)
{
	float PWidth = InOutline->GetOutlineParamValueByRefName(TEXT("PW")) * 0.1f;
	float PDepth = InOutline->GetOutlineParamValueByRefName(TEXT("PD")) * 0.1f;

	TArray<FPVertex> Vertices = {
		FPVertex(FVector(0.0f, 0.0f, 0.0f)),
		FPVertex(FVector(fWidth - PWidth, 0.0f, 0.0f)),
		FPVertex(FVector(fWidth - PWidth, PDepth, 0.0f)),
		FPVertex(FVector(fWidth, PDepth, 0.0f)),
		FPVertex(FVector(fWidth, fDepth, 0.0f)),
		FPVertex(FVector(0.0f, fDepth, 0.0f)),

		FPVertex(FVector(0.0f, 0.0f, fHeight)),
		FPVertex(FVector(fWidth - PWidth, 0.0f, fHeight)),
		FPVertex(FVector(fWidth - PWidth, PDepth, fHeight)),
		FPVertex(FVector(fWidth, PDepth, fHeight)),
		FPVertex(FVector(fWidth, fDepth, fHeight)),
		FPVertex(FVector(0.0f, fDepth, fHeight))
	};

	TArray<uint16> RigonIndices = {
		1, 2, 0,
		0, 2, 5,
		5, 2, 4,
		4, 2, 3,

		6, 8, 7,
		11, 8, 6,
		10, 8, 11,
		9, 8, 10,

		0, 6, 7,
		0, 7, 1,

		1, 7, 8,
		1, 8, 2,

		2, 8, 9,
		2, 9, 3,

		3, 9, 10,
		3, 10, 4,

		4, 10, 11,
		4, 11, 5,

		5, 11, 6,
		5, 6, 0
	};
	BoundingBoxPrimitive->SetPrimitiveInfo(true, Vertices, RigonIndices);

	TArray<uint16> LineIndices = {
		0, 1,
		1, 2,
		2, 3,
		3, 4,
		4, 5,
		5, 0,

		6, 7,
		7, 8,
		8, 9,
		9, 10,
		10, 11,
		11, 6,

		0, 6,
		1, 7,
		2, 8,
		3, 9,
		4, 10,
		5, 11
	};
	WireframePrimitive->SetPrimitiveInfo(false, Vertices, LineIndices);
}

void ASCTSpaceActor::UpdatePrimitiveForMiddlePiller(TSharedPtr<FSCTOutline> InOutline)
{
	float PPosX = InOutline->GetOutlineParamValueByRefName(TEXT("PX")) * 0.1f;
	float PWidth = InOutline->GetOutlineParamValueByRefName(TEXT("PW")) * 0.1f;
	float PDepth = InOutline->GetOutlineParamValueByRefName(TEXT("PD")) * 0.1f;

	TArray<FPVertex> Vertices = {
		FPVertex(FVector(0.0f, 0.0f, 0.0f)),
		FPVertex(FVector(PPosX, 0.0f, 0.0f)),
		FPVertex(FVector(PPosX, PDepth, 0.0f)),
		FPVertex(FVector(PPosX + PWidth, PDepth, 0.0f)),
		FPVertex(FVector(PPosX + PWidth, 0.0f, 0.0f)),
		FPVertex(FVector(fWidth, 0.0f, 0.0f)),
		FPVertex(FVector(fWidth, fDepth, 0.0f)),
		FPVertex(FVector(0.0f, fDepth, 0.0f)),

		FPVertex(FVector(0.0f, 0.0f, fHeight)),
		FPVertex(FVector(PPosX, 0.0f, fHeight)),
		FPVertex(FVector(PPosX, PDepth, fHeight)),
		FPVertex(FVector(PPosX + PWidth, PDepth, fHeight)),
		FPVertex(FVector(PPosX + PWidth, 0.0f, fHeight)),
		FPVertex(FVector(fWidth, 0.0f, fHeight)),
		FPVertex(FVector(fWidth, fDepth, fHeight)),
		FPVertex(FVector(0.0f, fDepth, fHeight))
	};

	TArray<uint16> RigonIndices = {
		1, 2, 0,
		0, 2, 7,
		7, 2, 6,
		6, 2, 3,
		6, 3, 5, 
		5, 3, 4, 

		8, 10, 9,
		15, 10, 8,
		14, 10, 15,
		14, 11, 10, 
		13, 11, 14, 
		12, 11, 13, 

		0, 8, 9,
		0, 9, 1,

		1, 9, 10,
		1, 10, 2,

		2, 10, 11,
		2, 11, 3,

		3, 11, 12,
		3, 12, 4,

		4, 12, 13,
		4, 13, 5,

		5, 13, 14,
		5, 14, 6,

		6, 14, 15, 
		6, 15, 7,

		7, 15, 8, 
		7, 8, 0
	};
	BoundingBoxPrimitive->SetPrimitiveInfo(true, Vertices, RigonIndices);

	TArray<uint16> LineIndices = {
		0, 1,
		1, 2,
		2, 3,
		3, 4,
		4, 5,
		5, 6,
		6, 7,
		7, 0,

		8, 9,
		9, 10,
		10, 11,
		11, 12,
		12, 13,
		13, 14,
		14, 15,
		15, 8,

		0, 8,
		1, 9,
		2, 10,
		3, 11,
		4, 12,
		5, 13,
		6, 14,
		7, 15
	};
	WireframePrimitive->SetPrimitiveInfo(false, Vertices, LineIndices);
}

void ASCTSpaceActor::UpdatePrimitiveForCornerPiller(TSharedPtr<FSCTOutline> InOutline)
{
	float CWidth = InOutline->GetOutlineParamValueByRefName(TEXT("CW")) * 0.1f;
	float CDepth = InOutline->GetOutlineParamValueByRefName(TEXT("CD")) * 0.1f;
	float PWidth = InOutline->GetOutlineParamValueByRefName(TEXT("PW")) * 0.1f;
	float PDepth = InOutline->GetOutlineParamValueByRefName(TEXT("PD")) * 0.1f;

	TArray<FPVertex> Vertices = {
		FPVertex(FVector(0.0f, 0.0f, 0.0f)),
		FPVertex(FVector(fWidth - PWidth, 0.0f, 0.0f)),
		FPVertex(FVector(fWidth - PWidth, PDepth, 0.0f)),
		FPVertex(FVector(fWidth, PDepth, 0.0f)),
		FPVertex(FVector(fWidth, fDepth, 0.0f)),
		FPVertex(FVector(CWidth, fDepth, 0.0f)),
		FPVertex(FVector(CWidth, fDepth - CDepth, 0.0f)),
		FPVertex(FVector(0.0f, fDepth - CDepth, 0.0f)),

		FPVertex(FVector(0.0f, 0.0f, fHeight)),
		FPVertex(FVector(fWidth - PWidth, 0.0f, fHeight)),
		FPVertex(FVector(fWidth - PWidth, PDepth, fHeight)),
		FPVertex(FVector(fWidth, PDepth, fHeight)),
		FPVertex(FVector(fWidth, fDepth, fHeight)),
		FPVertex(FVector(CWidth, fDepth, fHeight)),
		FPVertex(FVector(CWidth, fDepth - CDepth, fHeight)),
		FPVertex(FVector(0.0f, fDepth - CDepth, fHeight))
	};

	TArray<uint16> RigonIndices = {
		0, 1, 2,
		0, 2, 6,
		0, 6, 7,
		4, 5, 6,
		4, 6, 2,
		4, 2, 3,

		8, 15, 14,
		8, 14, 10,
		8, 10, 9,
		12, 11, 10,
		12, 10, 14,
		12, 14, 13,

		0, 8, 9,
		0, 9, 1,

		1, 9, 10,
		1, 10, 2,

		2, 10, 11,
		2, 11, 3,

		3, 11, 12,
		3, 12, 4,

		4, 12, 13,
		4, 13, 5,

		5, 13, 14,
		5, 14, 6,

		6, 14, 15,
		6, 15, 7,

		7, 15, 8,
		7, 8, 0
	};
	BoundingBoxPrimitive->SetPrimitiveInfo(true, Vertices, RigonIndices);

	TArray<uint16> LineIndices = {
		0, 1,
		1, 2,
		2, 3,
		3, 4,
		4, 5,
		5, 6,
		6, 7,
		7, 0,

		8, 9,
		9, 10,
		10, 11,
		11, 12,
		12, 13,
		13, 14,
		14, 15,
		15, 8,

		0, 8,
		1, 9,
		2, 10,
		3, 11,
		4, 12,
		5, 13,
		6, 14,
		7, 15
	};
	WireframePrimitive->SetPrimitiveInfo(false, Vertices, LineIndices);
}
