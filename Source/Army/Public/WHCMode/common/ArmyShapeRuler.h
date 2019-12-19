#pragma once
#include "GameFramework/Actor.h"
#include "ArmyShapeRuler.generated.h"

namespace XRWHCMode
{
    enum class ERulerAlignDirection : uint8
    {
        RAD_X,
        RAD_Y,
        RAD_Z
    };

    enum class ERulerAlignment : uint8
    {
        RA_Top,
        RA_Center,
        RA_Bottom
    };
}

UCLASS()
class AShapeRulerActor : public AActor
{
    GENERATED_BODY()

public:
    AShapeRulerActor();    
    void SetRulerAlignDirection(XRWHCMode::ERulerAlignDirection InAlignDir);
    void SetRulerAlignment(XRWHCMode::ERulerAlignment InAlignment);
    void SetRulerLength(float InLength);
    float GetRulerLength() const { return CachedLength; }
    FVector GetValueDesiredLocation() const;

private:
    void BuildRuler();

private:
    float CachedLength;
    XRWHCMode::ERulerAlignment CachedAlignment;
};

class FShapeRuler
{
public:
    FShapeRuler(XRWHCMode::ERulerAlignDirection InRulerAlignDir, const TSharedPtr<SOverlay> &InRulerOverlay);
    ~FShapeRuler();

    void SetRulerAlignment(XRWHCMode::ERulerAlignment InAlignment);
    void SetRulerLocation(const FVector &InLocation);

    void SetRulerLength(float InLength);
    float GetRulerLength() const { return RulerActor->GetRulerLength(); }

    void SetRulerValueIsReadOnly(bool bReadOnly);
    bool IsRulerValueReadOnly() const { return bReadOnlyRuler; }

    XRWHCMode::ERulerAlignDirection GetRulerAlignDirection() const { return RulerAlignDirection; }

    const TSharedPtr<class SEditableText>& GetRulerLabel() const { return RulerValue; }
    AShapeRulerActor* GetRulerActor() const { return RulerActor; }

    void ShowRuler(bool bShow);
    bool IsRulerVisible() const;

    void UpdateRulerPositionPerFrame();

    DECLARE_DELEGATE_OneParam(FOnRulerLengthChanged, const FString&)
    FOnRulerLengthChanged OnRulerLengthChanged;

private:
    void Callback_RulerValueTextChanged(const FText &InText, ETextCommit::Type InType);
    bool Callback_IsNumericChar(const TCHAR InChar);

private:
    AShapeRulerActor *RulerActor;
    SOverlay::FOverlaySlot *RulerValueSlot;
    TSharedPtr<SBox> RulerValueBox;
    TSharedPtr<class SBorder> RulerValueBackground;
    TSharedPtr<class SEditableText> RulerValue;
    TWeakPtr<SOverlay> RulerOverlay;
    XRWHCMode::ERulerAlignDirection RulerAlignDirection;
    bool bReadOnlyRuler = false;
};