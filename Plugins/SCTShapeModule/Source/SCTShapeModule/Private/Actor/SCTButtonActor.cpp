#include "SCTButtonActor.h"
#include "TypedPrimitive.h"
#include "ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/CollisionProfile.h"

ASCTButtonActor::ASCTButtonActor()
: fWidth(50.0),
fDepth(50.0),
fHeight(50.0)
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ButtonRoot"));

	BoundingPrimitive = CreateDefaultSubobject<UPNTPrimitive>(TEXT("ButtonBounding"));
	BoundingPrimitive->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	BoundingPrimitive->SetCollisionProfileName("BlockAllDynamic");
	UObject* TempObj = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("Material'/Game/Material/ForegroundColorUnlitT.ForegroundColorUnlitT'"));
	UMaterialInstanceDynamic *DynMaterial = UMaterialInstanceDynamic::Create(Cast<UMaterialInterface>(TempObj), nullptr);
	DynMaterial->SetVectorParameterValue(TEXT("Emissive"), FLinearColor::White);
	DynMaterial->SetScalarParameterValue(TEXT("Alpha"), 1.0f);
	BoundingPrimitive->SetMaterial(0, DynMaterial);

	WireframePrimitive = CreateDefaultSubobject<UPNTPrimitive>(TEXT("ButtonWireframe"));
	WireframePrimitive->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	WireframePrimitive->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	UMaterialInstanceDynamic *DynMaterial1 = UMaterialInstanceDynamic::Create(Cast<UMaterialInterface>(TempObj), nullptr);
	DynMaterial1->SetVectorParameterValue(TEXT("Emissive"), FLinearColor::Black);
	DynMaterial1->SetScalarParameterValue(TEXT("Alpha"), 1.0f);
	WireframePrimitive->SetMaterial(0, DynMaterial1);
}

// Called when the game starts or when spawned
void ASCTButtonActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASCTButtonActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASCTButtonActor::SetCollisionProfileName(FName InCollisionProfileName)
{
	BoundingPrimitive->SetCollisionProfileName(InCollisionProfileName);
	//WireframePrimitive->SetCollisionProfileName(bEnable ? UCollisionProfile::BlockAll_ProfileName : UCollisionProfile::NoCollision_ProfileName);
}

void ASCTButtonActor::SetShapeData(FSCTShape* InShape)
{
	ShapeData = InShape;
}

void ASCTButtonActor::SetRefName(const FString& InName)
{
	RefName = InName;
}

const FString& ASCTButtonActor::GetRefName() const
{
	return RefName;
}

void ASCTButtonActor::SetDimension(float InWidth, float InDepth, float InHeight)
{
	fWidth = InWidth * 0.1f;
	fDepth = InDepth * 0.1f;
	fHeight = InHeight * 0.1f;

	UpdateBBox();
}

void ASCTButtonActor::SetLineColor(const FLinearColor &InColor)
{
	UMaterialInstanceDynamic *MtlInstDyn = Cast<UMaterialInstanceDynamic>(WireframePrimitive->GetMaterial(0));
	if (MtlInstDyn)
	{
		MtlInstDyn->SetVectorParameterValue(TEXT("Emissive"), InColor);
	}
}

void ASCTButtonActor::SetRegionColor(const FLinearColor &InColor)
{
	UMaterialInstanceDynamic *MtlInstDyn = Cast<UMaterialInstanceDynamic>(BoundingPrimitive->GetMaterial(0));
	if (MtlInstDyn)
	{
		MtlInstDyn->SetVectorParameterValue(TEXT("Emissive"), InColor);
	}
}

void ASCTButtonActor::UpdateBBox()
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
	BoundingPrimitive->SetPrimitiveInfo(true, Vertices, Indices);

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

