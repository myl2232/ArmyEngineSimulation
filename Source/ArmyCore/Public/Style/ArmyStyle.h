#pragma once

#include "ISlateStyle.h"

class FArmyStyleSet;

struct FSlateBrush;

class ARMYCORE_API FArmyStyle
{
public:
    static TSharedPtr<FArmyStyleSet> Create();

    static void Startup();

    static void Shutdown();

    static ISlateStyle& Get();

    template<class T>
    static const T& GetWidgetStyle(FName PropertyName, const ANSICHAR* Specifier = nullptr)
    {
        return Instance->GetWidgetStyle<T>(PropertyName, Specifier);
    }

    static const FSlateBrush* GetBrush(FName PropertyName, const ANSICHAR* Specifier = nullptr);

    static const FLinearColor& GetColor(FName PropertyName, const ANSICHAR* Specifier = nullptr);

    static void ResetToDefault();

protected:
    static void SetStyle(const TSharedRef<class ISlateStyle>& NewStyle);

private:
    /** slate样式单例 */
    static TSharedPtr<ISlateStyle> Instance;
};