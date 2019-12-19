#include "ArmyShapeRuler.h"
#include "TypedPrimitive.h"
#include "Classes/Kismet/GameplayStatics.h"

constexpr float RulerHeight = 10.0f;
constexpr float HalfRulerHeight = RulerHeight * 0.5f;

AShapeRulerActor::AShapeRulerActor()
    : CachedAlignment(XRWHCMode::ERulerAlignment::RA_Top)
    , CachedLength(0.0f)
{
    PrimaryActorTick.bCanEverTick = false;
    bIsSelectable = 0;

    UPNTPrimitive *RulerComponent = CreateDefaultSubobject<UPNTPrimitive>(TEXT("Ruler"));

    UObject *TempObj = StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("Material'/Game/XRCommon/Material/M_ForegroundBaseColorUnlit.M_ForegroundBaseColorUnlit'"));
    if (TempObj && TempObj->IsA<UMaterialInterface>())
    {
        RulerComponent->SetMaterial(0, Cast<UMaterialInterface>(TempObj));
    }
    RootComponent = RulerComponent;
}

void AShapeRulerActor::SetRulerAlignDirection(XRWHCMode::ERulerAlignDirection InAlignDir)
{ 
    switch (InAlignDir)
    {
        case XRWHCMode::ERulerAlignDirection::RAD_X:
            SetActorRotation(FRotator(0.0f, 0.0f, 90.0f));
            break;
        case XRWHCMode::ERulerAlignDirection::RAD_Y:
            SetActorRotation(FRotator(0.0f, 90.0f, -90.0f));
            break;
        case XRWHCMode::ERulerAlignDirection::RAD_Z:
            SetActorRotation(FRotator(90.0f, -90.0f, 0.0f));
            break;
    }
}

void AShapeRulerActor::SetRulerAlignment(XRWHCMode::ERulerAlignment InAlignment)
{
    CachedAlignment = InAlignment;
    BuildRuler();
}

void AShapeRulerActor::SetRulerLength(float InLength)
{
    CachedLength = InLength;
    BuildRuler();
}

FVector AShapeRulerActor::GetValueDesiredLocation() const
{
    FVector DesiredLocation;
    switch (CachedAlignment)
    {
        case XRWHCMode::ERulerAlignment::RA_Top:
            DesiredLocation.Set(CachedLength * 0.5f, HalfRulerHeight, 0.0f);
            break;
        case XRWHCMode::ERulerAlignment::RA_Center:
            DesiredLocation.Set(CachedLength * 0.5f, 0.0f, 0.0f);
            break;
        case XRWHCMode::ERulerAlignment::RA_Bottom:
            DesiredLocation.Set(CachedLength * 0.5f, -HalfRulerHeight, 0.0f);
            break;
    }
    return GetActorTransform().TransformPosition(DesiredLocation);
}

void AShapeRulerActor::BuildRuler()
{
    FPVertex Vertices[6];
    switch (CachedAlignment)
    {
        case XRWHCMode::ERulerAlignment::RA_Top:
            Vertices[0].Position.Set(0.0f, 0.0f, 0.0f);
            Vertices[1].Position.Set(0.0f, RulerHeight, 0.0f);
            Vertices[2].Position.Set(0.0f, HalfRulerHeight, 0.0f);
            Vertices[3].Position.Set(CachedLength, HalfRulerHeight, 0.0f);
            Vertices[4].Position.Set(CachedLength, 0.0f, 0.0f);
            Vertices[5].Position.Set(CachedLength, RulerHeight, 0.0f);
            break;
        case XRWHCMode::ERulerAlignment::RA_Center:
            Vertices[0].Position.Set(0.0f, -HalfRulerHeight, 0.0f);
            Vertices[1].Position.Set(0.0f, HalfRulerHeight, 0.0f);
            Vertices[2].Position.Set(0.0f, 0.0f, 0.0f);
            Vertices[3].Position.Set(CachedLength, 0.0f, 0.0f);
            Vertices[4].Position.Set(CachedLength, -HalfRulerHeight, 0.0f);
            Vertices[5].Position.Set(CachedLength, HalfRulerHeight, 0.0f);
            break;
        case XRWHCMode::ERulerAlignment::RA_Bottom:
            Vertices[0].Position.Set(0.0f, -RulerHeight, 0.0f);
            Vertices[1].Position.Set(0.0f, 0.0f, 0.0f);
            Vertices[2].Position.Set(0.0f, -HalfRulerHeight, 0.0f);
            Vertices[3].Position.Set(CachedLength, -HalfRulerHeight, 0.0f);
            Vertices[4].Position.Set(CachedLength, 0.0f, 0.0f);
            Vertices[5].Position.Set(CachedLength, -RulerHeight, 0.0f);
            break;
    }
    TArray<FPVertex> RulerVertices(Vertices, 6);

    uint16 Indices[] = { 0, 1, 2, 3, 4, 5 };
    TArray<uint16> RulerIndices(Indices, 6);

    Cast<UPNTPrimitive>(RootComponent)->SetPrimitiveInfo(false, RulerVertices, RulerIndices);
}

FShapeRuler::FShapeRuler(XRWHCMode::ERulerAlignDirection InRulerAlignDir, const TSharedPtr<class SOverlay> &InRulerOverlay)
: RulerAlignDirection(InRulerAlignDir)
, RulerOverlay(InRulerOverlay)
{
    RulerActor = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World()->SpawnActor<AShapeRulerActor>(
        AShapeRulerActor::StaticClass(),
        FVector::ZeroVector,
        FRotator::ZeroRotator
    );
    RulerActor->SetActorHiddenInGame(true);
    RulerActor->SetRulerAlignDirection(InRulerAlignDir);

    RulerValueSlot = &InRulerOverlay->AddSlot()
    .VAlign(VAlign_Center)
	.HAlign(HAlign_Center);

    SAssignNew(RulerValueBox, SBox)
    .WidthOverride(88.0f)
    .HeightOverride(24.0f)
    [
        SNew(SBorder)
        .BorderImage(FArmyStyle::Get().GetBrush("Icon.White"))
        .BorderBackgroundColor(FLinearColor::Black)
        .Padding(FMargin(1.0f))
        [
            SAssignNew(RulerValueBackground, SBorder)
            .BorderImage(FArmyStyle::Get().GetBrush("Icon.White"))
            .BorderBackgroundColor(FLinearColor::White)
            .Padding(FMargin(0.0f))
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            [
                SAssignNew(RulerValue, SEditableText)
                .Font(FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12").Font)
                .Text(FText::FromString(TEXT("0")))
                .ColorAndOpacity(FLinearColor::Black)
                .SelectAllTextWhenFocused(true)
                .OnTextCommitted_Raw(this, &FShapeRuler::Callback_RulerValueTextChanged)
                .OnIsTypedCharValid_Raw(this, &FShapeRuler::Callback_IsNumericChar)
            ]
        ]
    ];
}

FShapeRuler::~FShapeRuler()
{
    RulerActor->Destroy();
    RulerValueSlot->DetachWidget();
    RulerOverlay.Pin()->RemoveSlot(RulerValue.ToSharedRef());
}

void FShapeRuler::SetRulerAlignment(XRWHCMode::ERulerAlignment InAlignment)
{
    RulerActor->SetRulerAlignment(InAlignment);
}

void FShapeRuler::SetRulerLocation(const FVector &InLocation)
{
    RulerActor->SetActorRelativeLocation(InLocation);
}

void FShapeRuler::SetRulerLength(float InLength)
{
    RulerActor->SetRulerLength(InLength);
    RulerValue->SetText(FText::FromString(FString::Printf(TEXT("%dmm"), FMath::RoundToInt(InLength * 10.0f))));
}

void FShapeRuler::SetRulerValueIsReadOnly(bool bReadOnly)
{
    RulerValue->SetIsReadOnly(bReadOnly);
    RulerValue->SetColorAndOpacity(bReadOnly ? FArmyStyle::Get().GetColor("Color.FF666666") : FLinearColor::Black);
    RulerValueBackground->SetBorderBackgroundColor(bReadOnly ? FArmyStyle::Get().GetColor("Color.FFE6E6E6") : FLinearColor::White);
    bReadOnlyRuler = bReadOnly;
}

void FShapeRuler::ShowRuler(bool bShow)
{
    RulerActor->SetActorHiddenInGame(!bShow);
    if (bShow)
        RulerValueSlot->operator[] (RulerValueBox.ToSharedRef());
    else   
        RulerValueSlot->DetachWidget();
}

bool FShapeRuler::IsRulerVisible() const
{
    return RulerActor->bHidden == 0;
}

void FShapeRuler::UpdateRulerPositionPerFrame()
{
    FVector ValueDesiredLocation = RulerActor->GetValueDesiredLocation();

    FVector2D ScreenPosition;

    const FVector &ViewLocation = GVC->GetViewLocation();
    const FRotator &ViewRotation = GVC->GetViewRotation();
    FVector ViewDir = ViewRotation.RotateVector(FVector::ForwardVector);
    float ProjectDis = FVector::DotProduct(ViewDir, ValueDesiredLocation - ViewLocation);
    if (ProjectDis < 0.0f)
    {
        RulerValueBox->SetVisibility(EVisibility::Collapsed);
        return;
    }
    else
        RulerValueBox->SetVisibility(EVisibility::Visible);
    
    GVC->WorldToPixel(ValueDesiredLocation, ScreenPosition);

    FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

    GVC->WorldToPixel(ValueDesiredLocation, ScreenPosition);
    ScreenPosition -= ViewportSize / 2.0f;

    RulerValueBox->SetRenderTransform(FSlateRenderTransform(ScreenPosition));
}

void FShapeRuler::Callback_RulerValueTextChanged(const FText &InText, ETextCommit::Type InType)
{
    OnRulerLengthChanged.ExecuteIfBound(InText.ToString());
}

bool FShapeRuler::Callback_IsNumericChar(const TCHAR InChar)
{
    return (InChar >= 48 && InChar <= 57);
}