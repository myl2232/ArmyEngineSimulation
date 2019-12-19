#include "SCTXRStyle.h"

#include "Styling/CoreStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "SCTXRStyleSet.h"

TSharedPtr<ISlateStyle> FSCTXRStyle::Instance = nullptr;

TSharedPtr<FSCTXRStyleSet> FSCTXRStyle::Create()
{
    TSharedPtr<FSCTXRStyleSet> NewStyleSet = MakeShareable(new FSCTXRStyleSet());
    NewStyleSet->Initialize();
	FSlateStyleRegistry::RegisterSlateStyle(*NewStyleSet.Get());
    return NewStyleSet;
}

void FSCTXRStyle::Startup()
{
    Instance = Create();
    SetStyle(Instance.ToSharedRef());
}

void FSCTXRStyle::Shutdown()
{
    ResetToDefault();
    Instance.Reset();
}

ISlateStyle& FSCTXRStyle::Get()
{
    return *(Instance.Get());
}

const FSlateBrush* FSCTXRStyle::GetBrush(FName PropertyName, const ANSICHAR* Specifier /*= nullptr*/)
{
    return Instance->GetBrush(PropertyName, Specifier);
}

const FLinearColor& FSCTXRStyle::GetColor(FName PropertyName, const ANSICHAR* Specifier /*= nullptr*/)
{
    return Instance->GetColor(PropertyName, Specifier);
}

void FSCTXRStyle::ResetToDefault()
{
    SetStyle(FCoreStyle::Create("SCTXRStyle"));
}

void FSCTXRStyle::SetStyle(const TSharedRef<class ISlateStyle>& NewStyle)
{
    if (Instance != NewStyle)
    {
        if (Instance.IsValid())
        {
            FSlateStyleRegistry::UnRegisterSlateStyle(*Instance.Get());
        }

        Instance = NewStyle;

        if (Instance.IsValid())
        {
            FSlateStyleRegistry::RegisterSlateStyle(*Instance.Get());
        }
        else
        {
            ResetToDefault();
        }
    }
}