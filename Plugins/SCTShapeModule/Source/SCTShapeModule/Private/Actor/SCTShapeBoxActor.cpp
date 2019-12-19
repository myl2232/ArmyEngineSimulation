#include "SCTShapeBoxActor.h"
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
#include "MaterialManager.h"


ASCTBoundingBoxActor::ASCTBoundingBoxActor()
: fWidth(80.0)
, fDepth(55.0)
, fHeight(220.0)
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("BoundingBoxRoot"));

	BoundingBoxPrimitive = CreateDefaultSubobject<UPNTPrimitive>(TEXT("BoundingBoxPrimitive"));
	//BoundingBoxPrimitive->SetCollisionProfileName("BlockBoundingBox"/*UCollisionProfile::NoCollision_ProfileName*/);
	BoundingBoxPrimitive->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	WireframePrimitive = CreateDefaultSubobject<UPNTPrimitive>(TEXT("WireframePrimitive"));
	//WireframePrimitive->SetCollisionProfileName("BlockBoundingBox"/*UCollisionProfile::NoCollision_ProfileName*/);
	WireframePrimitive->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	UpdatePrimitive();
}

// Called when the game starts or when spawned
void ASCTBoundingBoxActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASCTBoundingBoxActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASCTBoundingBoxActor::SetShape(FSCTShape* InShape)
{
	ShapeData = InShape;
}
FSCTShape* ASCTBoundingBoxActor::GetShape()
{
	return ShapeData;
}

void ASCTBoundingBoxActor::UpdateActorDimension()
{
	if (!bNeedGetShapeSize) return;
	fWidth = ShapeData->GetShapeWidth() * 0.1f;
	fDepth = ShapeData->GetShapeDepth() * 0.1f;
	fHeight = ShapeData->GetShapeHeight() * 0.1f;
	
	BoundBoxMinPoint = { -fWidth / 2.0f, -fDepth / 2.0f, -fHeight / 2.0f };
	BoundBoxMaxPoint = { fWidth / 2.0f, fDepth / 2.0f, fHeight / 2.0f };
	BoundBoxCenter = { fWidth / 2.0f, fDepth / 2.0f, fHeight / 2.0f };
	
	UpdatePrimitive();

	//板件修正外延值
	if (ShapeData->GetShapeType() == ST_Board)
	{
		FBoardShape *BoardShape = static_cast<FBoardShape*>(ShapeData);
		FVector ActorPosDelta(BoardShape->GetLeftExtension(), BoardShape->GetBackExtension(), 0.0);
		SetActorRelativeLocation(ActorPosDelta / 10.f);
	}
}

void  ASCTBoundingBoxActor::SetActorDimension(float InWidth, float InDepth, float InHeight)
{
	fWidth = InWidth * 0.1f;
	fDepth = InDepth * 0.1f;
	fHeight = InHeight * 0.1f;
	
	BoundBoxMinPoint = { -fWidth / 2.0f, -fDepth / 2.0f, -fHeight / 2.0f };
	BoundBoxMaxPoint = { fWidth / 2.0f, fDepth / 2.0f, fHeight / 2.0f };
	BoundBoxCenter = { fWidth / 2.0f, fDepth / 2.0f, fHeight / 2.0f };

	UpdatePrimitive();
	//板件修正外延值
	if (ShapeData->GetShapeType() == ST_Board)
	{
		FBoardShape *BoardShape = static_cast<FBoardShape*>(ShapeData);
		FVector ActorPosDelta(BoardShape->GetLeftExtension(), BoardShape->GetBackExtension(), 0.0);
		SetActorRelativeLocation(ActorPosDelta / 10.f);
	}
}

void ASCTBoundingBoxActor::SetActorDimension(const FVector & InCenter, const FVector & InMinPoint, const FVector & InMaxPoint)
{
	const FVector SizeVector = InMaxPoint - InMinPoint;
	fWidth = SizeVector.X;
	fDepth = SizeVector.Y;
	fHeight = SizeVector.Z;
		
	BoundBoxMinPoint = InMinPoint;
	BoundBoxMaxPoint = InMaxPoint;
	BoundBoxCenter = InCenter;
	
	UpdatePrimitive();
	//板件修正外延值
	if (ShapeData->GetShapeType() == ST_Board)
	{
		FBoardShape *BoardShape = static_cast<FBoardShape*>(ShapeData);
		FVector ActorPosDelta(BoardShape->GetLeftExtension(), BoardShape->GetBackExtension(), 0.0);
		SetActorRelativeLocation(ActorPosDelta / 10.f);
	}
}

void ASCTBoundingBoxActor::SetLineMaterial(UMaterialInterface* InMaterial)
{
	WireframePrimitive->SetMaterial(0, InMaterial);
}

void ASCTBoundingBoxActor::SetRegionMaterial(UMaterialInterface* InMaterial)
{
	BoundingBoxPrimitive->SetMaterial(0, InMaterial);
}

void ASCTBoundingBoxActor::SetLineColor(const FLinearColor &InColor)
{
	UMaterialInstanceDynamic *MtlInstDyn = Cast<UMaterialInstanceDynamic>(WireframePrimitive->GetMaterial(0));
	if (MtlInstDyn)
	{
		MtlInstDyn->SetVectorParameterValue(TEXT("Emissive"), InColor);
	}
}

void ASCTBoundingBoxActor::SetLineAlpha(float InAlpha)
{
	UMaterialInstanceDynamic *MtlInstDyn = Cast<UMaterialInstanceDynamic>(WireframePrimitive->GetMaterial(0));
	if (MtlInstDyn)
	{
		MtlInstDyn->SetScalarParameterValue(TEXT("Alpha"), InAlpha);
	}
}

void ASCTBoundingBoxActor::SetRegionColor(const FLinearColor &InColor)
{
	UMaterialInstanceDynamic *MtlInstDyn = Cast<UMaterialInstanceDynamic>(BoundingBoxPrimitive->GetMaterial(0));
	if (MtlInstDyn)
	{
		MtlInstDyn->SetVectorParameterValue(TEXT("Emissive"), InColor);
	}
}

void ASCTBoundingBoxActor::SetRegionAlpha(float InAlpha)
{
	UMaterialInstanceDynamic *MtlInstDyn = Cast<UMaterialInstanceDynamic>(BoundingBoxPrimitive->GetMaterial(0));
	if (MtlInstDyn)
	{
		MtlInstDyn->SetScalarParameterValue(TEXT("Alpha"), InAlpha);
	}
}

void ASCTBoundingBoxActor::SetIsEnableCollision(bool bEnable)
{
	BoundingBoxPrimitive->SetCollisionProfileName(
		bEnable ? UCollisionProfile::BlockAll_ProfileName : UCollisionProfile::NoCollision_ProfileName);
}

void ASCTBoundingBoxActor::SetCollisionProfileName(FName InProfileName)
{
	BoundingBoxPrimitive->SetCollisionProfileName(InProfileName);
	WireframePrimitive->SetCollisionProfileName(InProfileName);
	UpdateActorDimension();
}

void ASCTBoundingBoxActor::UpdatePrimitive()
{
#if 1
	TArray<FPVertex> Vertices = {
		FVector(-fWidth / 2.0f, fDepth / 2.0f, -fHeight / 2.0f) + BoundBoxCenter,
		FVector(-fWidth / 2.0f, -fDepth / 2.0f,-fHeight / 2.0f) + BoundBoxCenter,
		FVector(fWidth / 2.0f, -fDepth / 2.0f, -fHeight / 2.0f) + BoundBoxCenter,
		FVector(fWidth / 2.0f, fDepth / 2.0f, -fHeight / 2.0f) + BoundBoxCenter,
		FVector(-fWidth / 2.0f, fDepth / 2.0f, fHeight / 2.0f) + BoundBoxCenter,
		FVector(-fWidth / 2.0f, -fDepth / 2.0f, fHeight / 2.0f) + BoundBoxCenter,
		FVector(fWidth / 2.0f, -fDepth / 2.0f, fHeight / 2.0f) + BoundBoxCenter,
		FVector(fWidth / 2.0f, fDepth / 2.0f, fHeight / 2.0f) + BoundBoxCenter
	};

#else
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
#endif
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



ASCTWireframeActor::ASCTWireframeActor()
	: fWidth(10.0)
	, fDepth(10.0)
	, fHeight(10.0)
	, LineWidth(0.3)
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WireframeRoot"));
	WireframePrimitive = CreateDefaultSubobject<UPNTPrimitive>(TEXT("WireframePrimitive"));
	WireframePrimitive->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	WireframePrimitive->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

//  WireframePrimitive->SetRenderInMainPass(false);
//  WireframePrimitive->SetRenderCustomDepth(true);

	UpdateBBox();
}

// Called when the game starts or when spawned
void ASCTWireframeActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASCTWireframeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDynamicScaleLineSizeFollowCamera)
	{
		ULocalPlayer *Player = UGameplayStatics::GetPlayerController(GWorld, 0)->GetLocalPlayer();
		FSceneViewProjectionData ProjectionData;
		Player->GetProjectionData(GEngine->GameViewport->Viewport, eSSP_FULL, ProjectionData);
		LineScale = ProjectionData.ComputeViewProjectionMatrix().TransformFVector4(FVector4(GetActorLocation(), 1.0f)).W *
			(4.0f / ProjectionData.GetViewRect().Width() / ProjectionData.ProjectionMatrix.M[0][0]);

		UpdateBBox();
	}
}

void ASCTWireframeActor::UpdateActorDimension()
{
	if (!bNeedGetShapeSize) return;
	fWidth = ShapeData->GetShapeWidth() * 0.1f;
	fDepth = ShapeData->GetShapeDepth() * 0.1f;
	fHeight = ShapeData->GetShapeHeight() * 0.1f;

	BoundBoxMinPoint = { -fWidth / 2.0f, -fDepth / 2.0f, -fHeight / 2.0f };
	BoundBoxMaxPoint = { fWidth / 2.0f, fDepth / 2.0f, fHeight / 2.0f };
	BoundBoxCenter = { fWidth / 2.0f, fDepth / 2.0f, fHeight / 2.0f };
	
	UpdateBBox();

	//板件修正外延值
	if (ShapeData->GetShapeType() == ST_Board)
	{
		FBoardShape *BoardShape = static_cast<FBoardShape*>(ShapeData);
		FVector ActorPosDelta(BoardShape->GetLeftExtension(), BoardShape->GetBackExtension(), 0.0);
		SetActorRelativeLocation(ActorPosDelta / 10.f);
	}
}

void ASCTWireframeActor::SetActorDimension(float InWidth, float InDepth, float InHeight)
{
	fWidth = InWidth * 0.1f;
	fDepth = InDepth * 0.1f;
	fHeight = InHeight * 0.1f;
	BoundBoxMinPoint = { -fWidth / 2.0f, -fDepth / 2.0f, -fHeight / 2.0f };
	BoundBoxMaxPoint = { fWidth / 2.0f, fDepth / 2.0f, fHeight / 2.0f };
	BoundBoxCenter = { fWidth / 2.0f, fDepth / 2.0f, fHeight / 2.0f };
	
	UpdateBBox();	

	//板件修正外延值
	if (ShapeData->GetShapeType() == ST_Board)
	{
		FBoardShape *BoardShape = static_cast<FBoardShape*>(ShapeData);
		FVector ActorPosDelta(BoardShape->GetLeftExtension(), BoardShape->GetBackExtension(), 0.0);
		SetActorRelativeLocation(ActorPosDelta / 10.f);
	}
}

void ASCTWireframeActor::SetActorDimension(const FVector & InCenter, const FVector & InMinPoint, const FVector & InMaxPoint)
{
	const FVector SizeVector = InMaxPoint - InMinPoint;	
	fWidth = SizeVector.X;
	fDepth = SizeVector.Y;
	fHeight = SizeVector.Z;
	BoundBoxMinPoint = InMinPoint;
	BoundBoxMaxPoint = InMaxPoint;
	BoundBoxCenter = InCenter;
	UpdateBBox();
	//板件修正外延值
	if (ShapeData->GetShapeType() == ST_Board)
	{
		FBoardShape *BoardShape = static_cast<FBoardShape*>(ShapeData);
		FVector ActorPosDelta(BoardShape->GetLeftExtension(), BoardShape->GetBackExtension(), 0.0);
		SetActorRelativeLocation(ActorPosDelta / 10.f);
	}
}

void ASCTWireframeActor::SetLineMaterial(UMaterialInterface* InMaterial)
{
	WireframePrimitive->SetMaterial(0, InMaterial);
}

 void ASCTWireframeActor::SetLineColor(const FLinearColor &InColor)
 {
 	UMaterialInstanceDynamic *MtlInstDyn = Cast<UMaterialInstanceDynamic>(WireframePrimitive->GetMaterial(0));
 	if (MtlInstDyn)
 	{
 		MtlInstDyn->SetVectorParameterValue(TEXT("Emissive"), InColor);
 	}
 }

void ASCTWireframeActor::SetLineWidth(float InValue)
{
	LineWidth = InValue;
	UpdateBBox();
}

void ASCTWireframeActor::SetIsEnableCollision(bool bEnable)
{
	WireframePrimitive->SetCollisionProfileName(
		bEnable ? UCollisionProfile::BlockAll_ProfileName : UCollisionProfile::NoCollision_ProfileName);
}

void ASCTWireframeActor::SetCollisionProfileName(FName InProfileName)
{
	WireframePrimitive->SetCollisionProfileName(InProfileName);
	UpdateActorDimension();
}

void ASCTWireframeActor::SetShape(FSCTShape* InShape)
{
	ShapeData = InShape;
}

FSCTShape* ASCTWireframeActor::GetShape()
{
	return ShapeData;
}

// void ASCTWireframeActor::SetCustomDepthStencilValue(int32 InValue)
// {
// 	WireframePrimitive->SetCustomDepthStencilValue(InValue);
// }

void ASCTWireframeActor::UpdateBBoxEx()
{
#if 1	
	Positions = {
		FVector(-fWidth / 2.0f, fDepth / 2.0f, 0.0f) + BoundBoxCenter,
		FVector(-fWidth / 2.0f, -fDepth / 2.0f, 0.0f) + BoundBoxCenter,
		FVector(fWidth / 2.0f, -fDepth / 2.0f, 0.0f) + BoundBoxCenter,
		FVector(fWidth / 2.0f, fDepth / 2.0f, 0.0f) + BoundBoxCenter,
		FVector(-fWidth / 2.0f, fDepth / 2.0f, fHeight / 2.0f) + BoundBoxCenter,
		FVector(-fWidth / 2.0f, -fDepth / 2.0f, fHeight / 2.0f) + BoundBoxCenter,
		FVector(fWidth / 2.0f, -fDepth / 2.0f, fHeight / 2.0f) + BoundBoxCenter,
		FVector(fWidth / 2.0f, fDepth / 2.0f, fHeight / 2.0f) + BoundBoxCenter
	};	
#else
	Positions = {
		FVector(0.0f, fDepth, 0.0f),
		FVector(0.0f, 0.0f, 0.0f),
		FVector(fWidth, 0.0f, 0.0f),
		FVector(fWidth, fDepth, 0.0f),
		FVector(0.0f, fDepth, fHeight),
		FVector(0.0f, 0.0f, fHeight),
		FVector(fWidth, 0.0f, fHeight),
		FVector(fWidth, fDepth, fHeight)
};
#endif

	Indices = {
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

	TArray<FPVertex> OutVertices;
	TArray<uint16> OutIndices;
	GenerateLineBox54(OutVertices, OutIndices);
	AddIndices(OutIndices, 8 * 0);
	GenerateLineBox56(OutVertices, OutIndices);
	AddIndices(OutIndices, 8 * 1);
	GenerateLineBox67(OutVertices, OutIndices);
	AddIndices(OutIndices, 8 * 2);
	GenerateLineBox47(OutVertices, OutIndices);
	AddIndices(OutIndices, 8 * 3);
	GenerateLineBox04(OutVertices, OutIndices);
	AddIndices(OutIndices, 8 * 4);
	GenerateLineBox15(OutVertices, OutIndices);
	AddIndices(OutIndices, 8 * 5);
	GenerateLineBox26(OutVertices, OutIndices);
	AddIndices(OutIndices, 8 * 6);
	GenerateLineBox37(OutVertices, OutIndices);
	AddIndices(OutIndices, 8 * 7);
	GenerateLineBox03(OutVertices, OutIndices);
	AddIndices(OutIndices, 8 * 8);
	GenerateLineBox10(OutVertices, OutIndices);
	AddIndices(OutIndices, 8 * 9);
	GenerateLineBox12(OutVertices, OutIndices);
	AddIndices(OutIndices, 8 * 10);
	GenerateLineBox23(OutVertices, OutIndices);
	AddIndices(OutIndices, 8 * 11);

	WireframePrimitive->SetPrimitiveInfo(false, OutVertices, OutIndices);
}

void ASCTWireframeActor::UpdateBBox()
{		
#if 1	
	TArray<FPVertex> Vertices = {
		FPVertex(FVector(-fWidth / 2.0f, fDepth / 2.0f, -fHeight / 2.0f) + BoundBoxCenter),
		FPVertex(FVector(-fWidth / 2.0f, -fDepth / 2.0f, -fHeight / 2.0f) + BoundBoxCenter),
		FPVertex(FVector(fWidth / 2.0f, -fDepth / 2.0f, -fHeight / 2.0f) + BoundBoxCenter),
		FPVertex(FVector(fWidth / 2.0f, fDepth / 2.0f, -fHeight / 2.0f) + BoundBoxCenter),
		FPVertex(FVector(-fWidth / 2.0f, fDepth / 2.0f, fHeight / 2.0f) + BoundBoxCenter),
		FPVertex(FVector(-fWidth / 2.0f, -fDepth / 2.0f, fHeight / 2.0f) + BoundBoxCenter),
		FPVertex(FVector(fWidth / 2.0f, -fDepth / 2.0f, fHeight / 2.0f) + BoundBoxCenter),
		FPVertex(FVector(fWidth / 2.0f, fDepth / 2.0f, fHeight / 2.0f) + BoundBoxCenter)
	};
#else
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
#endif

	TArray<uint16> LineIndices = {
		5, 4,
		5, 6,
		6, 7,
		4, 7,
		0, 4,
		1, 5,
		2, 6,
		3, 7,
		0, 3,
		1, 0,
		1, 2,
		2, 3,
	};

	TArray<FPVertex> OutVertices;
	TArray<uint16> OutIndices;
	for (int32 i=0; i<12; ++i)
	{
		GenerateLineBox(OutVertices, OutIndices, Vertices[LineIndices[2 * i]].Position, Vertices[LineIndices[2 * i + 1]].Position);
	}

	WireframePrimitive->SetPrimitiveInfo(true, OutVertices, OutIndices);
}

void ASCTWireframeActor::GenerateLineBox(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices,
	const FVector& StartPnt, const FVector& EndigPnt)
{
	float LineWidthVaue = LineWidth * LineScale;
	int32 PntCount = OutVertices.Num();

	if (StartPnt.X != EndigPnt.X)
	{
		OutVertices.Add(FPVertex(FVector(StartPnt.X, StartPnt.Y + LineWidthVaue, StartPnt.Z - LineWidthVaue)));   //0
		OutVertices.Add(FPVertex(FVector(StartPnt.X, StartPnt.Y - LineWidthVaue, StartPnt.Z - LineWidthVaue)));   //1
		OutVertices.Add(FPVertex(FVector(EndigPnt.X, EndigPnt.Y - LineWidthVaue, EndigPnt.Z - LineWidthVaue)));   //2
		OutVertices.Add(FPVertex(FVector(EndigPnt.X, EndigPnt.Y + LineWidthVaue, EndigPnt.Z - LineWidthVaue)));   //3
		OutVertices.Add(FPVertex(FVector(StartPnt.X, StartPnt.Y + LineWidthVaue, StartPnt.Z + LineWidthVaue)));   //4
		OutVertices.Add(FPVertex(FVector(StartPnt.X, StartPnt.Y - LineWidthVaue, StartPnt.Z + LineWidthVaue)));   //5
		OutVertices.Add(FPVertex(FVector(EndigPnt.X, EndigPnt.Y - LineWidthVaue, EndigPnt.Z + LineWidthVaue)));   //6
		OutVertices.Add(FPVertex(FVector(EndigPnt.X, EndigPnt.Y + LineWidthVaue, EndigPnt.Z + LineWidthVaue)));   //7
	} 
	else if (StartPnt.Y != EndigPnt.Y)
	{
		OutVertices.Add(FPVertex(FVector(EndigPnt.X - LineWidthVaue, EndigPnt.Y, EndigPnt.Z - LineWidthVaue)));   //0
		OutVertices.Add(FPVertex(FVector(StartPnt.X - LineWidthVaue, StartPnt.Y, StartPnt.Z - LineWidthVaue)));   //1
		OutVertices.Add(FPVertex(FVector(StartPnt.X + LineWidthVaue, StartPnt.Y, StartPnt.Z - LineWidthVaue)));   //2
		OutVertices.Add(FPVertex(FVector(EndigPnt.X + LineWidthVaue, EndigPnt.Y, EndigPnt.Z - LineWidthVaue)));   //3
		OutVertices.Add(FPVertex(FVector(EndigPnt.X - LineWidthVaue, EndigPnt.Y, EndigPnt.Z + LineWidthVaue)));   //4
		OutVertices.Add(FPVertex(FVector(StartPnt.X - LineWidthVaue, StartPnt.Y, StartPnt.Z + LineWidthVaue)));   //5
		OutVertices.Add(FPVertex(FVector(StartPnt.X + LineWidthVaue, StartPnt.Y, StartPnt.Z + LineWidthVaue)));   //6
		OutVertices.Add(FPVertex(FVector(EndigPnt.X + LineWidthVaue, EndigPnt.Y, EndigPnt.Z + LineWidthVaue)));   //7
	}
	else if (StartPnt.Z != EndigPnt.Z)
	{
		OutVertices.Add(FPVertex(FVector(StartPnt.X - LineWidthVaue, StartPnt.Y + LineWidthVaue, StartPnt.Z)));   //0
		OutVertices.Add(FPVertex(FVector(StartPnt.X - LineWidthVaue, StartPnt.Y - LineWidthVaue, StartPnt.Z)));   //1
		OutVertices.Add(FPVertex(FVector(StartPnt.X + LineWidthVaue, StartPnt.Y - LineWidthVaue, StartPnt.Z)));   //2
		OutVertices.Add(FPVertex(FVector(StartPnt.X + LineWidthVaue, StartPnt.Y + LineWidthVaue, StartPnt.Z)));   //3
		OutVertices.Add(FPVertex(FVector(EndigPnt.X - LineWidthVaue, EndigPnt.Y + LineWidthVaue, EndigPnt.Z)));   //4
		OutVertices.Add(FPVertex(FVector(EndigPnt.X - LineWidthVaue, EndigPnt.Y - LineWidthVaue, EndigPnt.Z)));   //5
		OutVertices.Add(FPVertex(FVector(EndigPnt.X + LineWidthVaue, EndigPnt.Y - LineWidthVaue, EndigPnt.Z)));   //6
		OutVertices.Add(FPVertex(FVector(EndigPnt.X + LineWidthVaue, EndigPnt.Y + LineWidthVaue, EndigPnt.Z)));   //7
	}
	else
	{
		//check(false);
	}

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
	for (int32 i=0; i<12*3; ++i)
	{
		OutIndices.Add(Indices[i] + PntCount);
	}
}

void ASCTWireframeActor::GenerateLineBox10(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices)
{
	float LineWidthVaue = LineWidth * LineScale;
	FVector Pnt0 = Positions[1];
	FVector Pnt1 = Positions[0];

	OutVertices.Add(FPVertex(FVector(Pnt1.X - LineWidthVaue, Pnt1.Y, Pnt1.Z - LineWidthVaue)));   //0
	OutVertices.Add(FPVertex(FVector(Pnt0.X - LineWidthVaue, Pnt0.Y, Pnt0.Z - LineWidthVaue)));   //1
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y, Pnt0.Z - LineWidthVaue)));   //2
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y, Pnt1.Z - LineWidthVaue)));   //3
	OutVertices.Add(FPVertex(FVector(Pnt1.X - LineWidthVaue, Pnt1.Y, Pnt1.Z)));   //4
	OutVertices.Add(FPVertex(FVector(Pnt0.X - LineWidthVaue, Pnt0.Y, Pnt0.Z)));   //5
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y, Pnt0.Z)));   //6
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y, Pnt1.Z)));   //7
}
void ASCTWireframeActor::GenerateLineBox12(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices)
{
	float LineWidthVaue = LineWidth * LineScale;
	FVector Pnt0 = Positions[1];
	FVector Pnt1 = Positions[2];

	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y, Pnt0.Z - LineWidthVaue)));   //0
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y - LineWidthVaue, Pnt0.Z - LineWidthVaue)));   //1
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y - LineWidthVaue, Pnt1.Z - LineWidthVaue)));   //2
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y, Pnt1.Z - LineWidthVaue)));   //3
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y, Pnt0.Z)));   //4
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y - LineWidthVaue, Pnt0.Z)));   //5
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y - LineWidthVaue, Pnt1.Z)));   //6
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y, Pnt1.Z)));   //7
}
void ASCTWireframeActor::GenerateLineBox03(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices)
{
	float LineWidthVaue = LineWidth * LineScale;
	FVector Pnt0 = Positions[0];
	FVector Pnt1 = Positions[3];

	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y + LineWidthVaue, Pnt0.Z - LineWidthVaue)));   //0
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y, Pnt0.Z - LineWidthVaue)));   //1
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y, Pnt1.Z - LineWidthVaue)));   //2
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y + LineWidthVaue, Pnt1.Z - LineWidthVaue)));   //3
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y + LineWidthVaue, Pnt0.Z)));   //4
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y, Pnt0.Z)));   //5
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y, Pnt1.Z)));   //6
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y + LineWidthVaue, Pnt1.Z)));   //7
}
void ASCTWireframeActor::GenerateLineBox23(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices)
{
	float LineWidthVaue = LineWidth * LineScale;
	FVector Pnt0 = Positions[2];
	FVector Pnt1 = Positions[3];

	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y, Pnt1.Z - LineWidthVaue)));   //0
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y, Pnt0.Z - LineWidthVaue)));   //1
	OutVertices.Add(FPVertex(FVector(Pnt0.X + LineWidthVaue, Pnt0.Y, Pnt0.Z - LineWidthVaue)));   //2
	OutVertices.Add(FPVertex(FVector(Pnt1.X + LineWidthVaue, Pnt1.Y, Pnt1.Z - LineWidthVaue)));   //3
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y, Pnt1.Z)));   //4
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y, Pnt0.Z)));   //5
	OutVertices.Add(FPVertex(FVector(Pnt0.X + LineWidthVaue, Pnt0.Y, Pnt0.Z)));   //6
	OutVertices.Add(FPVertex(FVector(Pnt1.X + LineWidthVaue, Pnt1.Y, Pnt1.Z)));   //7
}
void ASCTWireframeActor::GenerateLineBox54(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices)
{
	float LineWidthVaue = LineWidth * LineScale;
	FVector Pnt0 = Positions[5];
	FVector Pnt1 = Positions[4];

	OutVertices.Add(FPVertex(FVector(Pnt1.X - LineWidthVaue, Pnt1.Y, Pnt1.Z)));   //0
	OutVertices.Add(FPVertex(FVector(Pnt0.X - LineWidthVaue, Pnt0.Y, Pnt0.Z)));   //1
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y, Pnt0.Z)));   //2
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y, Pnt1.Z)));   //3
	OutVertices.Add(FPVertex(FVector(Pnt1.X + LineWidthVaue, Pnt1.Y, Pnt1.Z + LineWidthVaue)));   //4
	OutVertices.Add(FPVertex(FVector(Pnt0.X + LineWidthVaue, Pnt0.Y, Pnt0.Z + LineWidthVaue)));   //5
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y, Pnt0.Z + LineWidthVaue)));   //6
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y, Pnt1.Z + LineWidthVaue)));   //7
}
void ASCTWireframeActor::GenerateLineBox56(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices)
{
	float LineWidthVaue = LineWidth * LineScale;
	FVector Pnt0 = Positions[5];
	FVector Pnt1 = Positions[6];

	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y, Pnt0.Z)));   //0
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y - LineWidthVaue, Pnt0.Z)));   //1
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y - LineWidthVaue, Pnt1.Z)));   //2
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y, Pnt1.Z)));   //3
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y, Pnt0.Z + LineWidthVaue)));   //4
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y - LineWidthVaue, Pnt0.Z + LineWidthVaue)));   //5
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y - LineWidthVaue, Pnt1.Z + LineWidthVaue)));   //6
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y, Pnt1.Z + LineWidthVaue)));   //7
}
void ASCTWireframeActor::GenerateLineBox47(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices)
{
	float LineWidthVaue = LineWidth * LineScale;
	FVector Pnt0 = Positions[4];
	FVector Pnt1 = Positions[7];

	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y + LineWidthVaue, Pnt0.Z)));   //0
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y, Pnt0.Z)));   //1
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y, Pnt1.Z)));   //2
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y + LineWidthVaue, Pnt1.Z)));   //3
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y + LineWidthVaue, Pnt0.Z + LineWidthVaue)));   //4
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y, Pnt0.Z + LineWidthVaue)));   //5
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y, Pnt1.Z + LineWidthVaue)));   //6
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y + LineWidthVaue, Pnt1.Z + LineWidthVaue)));   //7
}
void ASCTWireframeActor::GenerateLineBox67(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices)
{
	float LineWidthVaue = LineWidth * LineScale;
	FVector Pnt0 = Positions[6];
	FVector Pnt1 = Positions[7];

	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y, Pnt1.Z)));   //0
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y, Pnt0.Z)));   //1
	OutVertices.Add(FPVertex(FVector(Pnt0.X + LineWidthVaue, Pnt0.Y, Pnt0.Z)));   //2
	OutVertices.Add(FPVertex(FVector(Pnt1.X + LineWidthVaue, Pnt1.Y, Pnt1.Z)));   //3
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y, Pnt1.Z + LineWidthVaue)));   //4
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y, Pnt0.Z + LineWidthVaue)));   //5
	OutVertices.Add(FPVertex(FVector(Pnt0.X + LineWidthVaue, Pnt0.Y, Pnt0.Z + LineWidthVaue)));   //6
	OutVertices.Add(FPVertex(FVector(Pnt1.X + LineWidthVaue, Pnt1.Y, Pnt1.Z + LineWidthVaue)));   //7
}
void ASCTWireframeActor::GenerateLineBox04(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices)
{
	float LineWidthVaue = LineWidth * LineScale;
	FVector Pnt0 = Positions[0];
	FVector Pnt1 = Positions[4];

	OutVertices.Add(FPVertex(FVector(Pnt0.X - LineWidthVaue, Pnt0.Y + LineWidthVaue, Pnt0.Z)));   //0
	OutVertices.Add(FPVertex(FVector(Pnt0.X - LineWidthVaue, Pnt0.Y, Pnt0.Z)));   //1
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y, Pnt0.Z)));   //2
	OutVertices.Add(FPVertex(FVector(Pnt0.X + LineWidthVaue, Pnt0.Y, Pnt0.Z)));   //3
	OutVertices.Add(FPVertex(FVector(Pnt1.X - LineWidthVaue, Pnt1.Y + LineWidthVaue, Pnt1.Z)));   //4
	OutVertices.Add(FPVertex(FVector(Pnt1.X - LineWidthVaue, Pnt1.Y, Pnt1.Z)));   //5
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y, Pnt1.Z)));   //6
	OutVertices.Add(FPVertex(FVector(Pnt1.X + LineWidthVaue, Pnt1.Y, Pnt1.Z)));   //7
}
void ASCTWireframeActor::GenerateLineBox15(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices)
{
	float LineWidthVaue = LineWidth * LineScale;
	FVector Pnt0 = Positions[1];
	FVector Pnt1 = Positions[5];

	OutVertices.Add(FPVertex(FVector(Pnt0.X - LineWidthVaue, Pnt0.Y, Pnt0.Z)));   //0
	OutVertices.Add(FPVertex(FVector(Pnt0.X - LineWidthVaue, Pnt0.Y - LineWidthVaue, Pnt0.Z)));   //1
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y - LineWidthVaue, Pnt0.Z)));   //2
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y, Pnt0.Z)));   //3
	OutVertices.Add(FPVertex(FVector(Pnt1.X - LineWidthVaue, Pnt1.Y, Pnt1.Z)));   //4
	OutVertices.Add(FPVertex(FVector(Pnt1.X - LineWidthVaue, Pnt1.Y - LineWidthVaue, Pnt1.Z)));   //5
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y - LineWidthVaue, Pnt1.Z)));   //6
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y, Pnt1.Z)));   //7
}
void ASCTWireframeActor::GenerateLineBox26(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices)
{
	float LineWidthVaue = LineWidth * LineScale;
	FVector Pnt0 = Positions[2];
	FVector Pnt1 = Positions[6];

	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y, Pnt0.Z)));   //0
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y - LineWidthVaue, Pnt0.Z)));   //1
	OutVertices.Add(FPVertex(FVector(Pnt0.X + LineWidthVaue, Pnt0.Y - LineWidthVaue, Pnt0.Z)));   //2
	OutVertices.Add(FPVertex(FVector(Pnt0.X + LineWidthVaue, Pnt0.Y, Pnt0.Z)));   //3
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y, Pnt1.Z)));   //4
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y - LineWidthVaue, Pnt1.Z)));   //5
	OutVertices.Add(FPVertex(FVector(Pnt1.X + LineWidthVaue, Pnt1.Y - LineWidthVaue, Pnt1.Z)));   //6
	OutVertices.Add(FPVertex(FVector(Pnt1.X + LineWidthVaue, Pnt1.Y, Pnt1.Z)));   //7
}
void ASCTWireframeActor::GenerateLineBox37(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices)
{
	float LineWidthVaue = LineWidth * LineScale;
	FVector Pnt0 = Positions[3];
	FVector Pnt1 = Positions[7];

	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y + LineWidthVaue, Pnt0.Z)));   //0
	OutVertices.Add(FPVertex(FVector(Pnt0.X, Pnt0.Y, Pnt0.Z)));   //1
	OutVertices.Add(FPVertex(FVector(Pnt0.X + LineWidthVaue, Pnt0.Y, Pnt0.Z)));   //2
	OutVertices.Add(FPVertex(FVector(Pnt0.X + LineWidthVaue, Pnt0.Y + LineWidthVaue, Pnt0.Z)));   //3
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y + LineWidthVaue, Pnt1.Z)));   //4
	OutVertices.Add(FPVertex(FVector(Pnt1.X, Pnt1.Y, Pnt1.Z)));   //5
	OutVertices.Add(FPVertex(FVector(Pnt1.X + LineWidthVaue, Pnt1.Y, Pnt1.Z)));   //6
	OutVertices.Add(FPVertex(FVector(Pnt1.X + LineWidthVaue, Pnt1.Y + LineWidthVaue, Pnt1.Z)));   //7
}

void ASCTWireframeActor::AddIndices(TArray<uint16>& OutIndices, int32 PntCount)
{
	for (int32 i = 0; i < 12 * 3; ++i)
	{
		OutIndices.Add(Indices[i] + PntCount);
	}
}
