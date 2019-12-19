#include "ArmyStyle.h"

#include "Styling/CoreStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "ArmyStyleSet.h"

TSharedPtr<ISlateStyle> FArmyStyle::Instance = nullptr;

TSharedPtr<FArmyStyleSet> FArmyStyle::Create()
{
    TSharedPtr<FArmyStyleSet> NewStyleSet = MakeShareable(new FArmyStyleSet());
    NewStyleSet->Initialize();
	FSlateStyleRegistry::RegisterSlateStyle(*NewStyleSet.Get());
    return NewStyleSet;
}

void FArmyStyle::Startup()
{
    Instance = Create();
    SetStyle(Instance.ToSharedRef());
}

void FArmyStyle::Shutdown()
{
    ResetToDefault();
    Instance.Reset();
}

ISlateStyle& FArmyStyle::Get()
{
    return *(Instance.Get());
}

const FSlateBrush* FArmyStyle::GetBrush(FName PropertyName, const ANSICHAR* Specifier /*= nullptr*/)
{
    return Instance->GetBrush(PropertyName, Specifier);
}

const FLinearColor& FArmyStyle::GetColor(FName PropertyName, const ANSICHAR* Specifier /*= nullptr*/)
{
    return Instance->GetColor(PropertyName, Specifier);
}

void FArmyStyle::ResetToDefault()
{
    SetStyle(FCoreStyle::Create("ArmyStyle"));
}

void FArmyStyle::SetStyle(const TSharedRef<class ISlateStyle>& NewStyle)
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