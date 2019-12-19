#include "ArmyInputBoxSlate.h"
#include "ArmyEditableLabelSlate.h"
#include "Engine/Engine.h"
#include "Style/ArmyStyle.h"
#include "Widgets/Input/SEditableText.h"

void SArmyInputBox::Construct(const FArguments& InArgs)
{
    OnInputBoxCommitted = InArgs._OnInputBoxCommitted;
	OnInputBoxForThisCommitted = InArgs._OnInputBoxForThisCommitted;
    OnKeyDownCallback = InArgs._OnKeyDown;
	CachedLength = 0.f;
    ChildSlot
    [
        SNew(SBox)
        .MinDesiredWidth(32)
        .HAlign(HAlign_Center)
        [
            SNew(SBorder)
            .BorderBackgroundColor(FArmyStyle::GetColor("Color.White"))
            .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
            .Padding(FMargin(2, 2, 2, 2))
            [
                SAssignNew(InputText, SArmyEditableText)
                .Text(FText::FromString(TEXT("0 mm")))
				.SelectAllTextWhenFocused(true)
				.SelectAllTextOnCommit(true)
                .OnTextCommitted(this, &SArmyInputBox::OnTextCommitted)
                .OnKeyDownHandler(this, &SArmyInputBox::OnKeyDown)
                .OnIsTypedCharValid(this, &SArmyInputBox::IsCharacterNumeric)
                .Style(FArmyStyle::Get(), "EditableText")
                .AllowContextMenu(false)
            ]
        ]
    ];

    SetVisibility(EVisibility::Collapsed);
}

FReply SArmyInputBox::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
    OnKeyDownCallback.ExecuteIfBound(InKeyEvent);

	if (InKeyEvent.GetKey() == EKeys::Enter)
	{
		OnTextCommitted(InputText->GetText(), ETextCommit::OnEnter);
		OnTextCommittedForThis(InputText->GetText(), ETextCommit::OnEnter, this->AsShared());
		return FReply::Handled();
	}
   return FReply::Unhandled();
}



void SArmyInputBox::Show(bool bShow)
{
    if (bShow)
    {
        SetVisibility(EVisibility::Visible);
    }
    else
    {
        SetVisibility(EVisibility::Collapsed);
        SetFocus(false);
    }
}

void SArmyInputBox::SetFocus(bool bFocus)
{
	if (bFocus)
	{
		FSlateApplication::Get().SetKeyboardFocus(InputText);
		InputText->SelectAllText();
	}
	else
	{
        FSlateApplication::Get().ClearKeyboardFocus();
		FSlateApplication::Get().SetAllUserFocusToGameViewport();
	}
}

void SArmyInputBox::SetPos(const FVector2D& Pos)
{
    SetRenderTransform(FSlateRenderTransform(Pos));
}

void SArmyInputBox::SetTransform(const FTransform2D& trans)
{
	SetRenderTransform(trans);
}

void SArmyInputBox::SetInputText(const FText& InText)
{
    InputText->SetText(InText);
}

bool SArmyInputBox::SetStartAndEnd(const FVector& Start, const FVector& End,bool bIgnoreZ)
{
    // 显示输入框并填入当前的线段长度，如果长度为0则不显示
	FVector TempStart = Start;
	FVector TempEnd = End;
	if (bIgnoreZ)
	{
		TempEnd.Z = 0;
		TempStart.Z = 0;
	}

    float LineLength = FVector::Distance(TempStart, TempEnd);
	/*if (TempStart.Equals(TempEnd, 0.4))
	{
		LineLength = 0;
	}*/
	if (!FMath::IsNearlyEqual(CachedLength, LineLength, 0.001f)) {
		//Show(true);
		CachedLength = LineLength;
		// 设置输入框显示的线段长度
		const FText LineLengthText = FText::FromString(FString::Printf(TEXT("%.0f mm"), LineLength * 10));
		SetInputText(LineLengthText);
	}    	
    return LineLength > 0;
}


void SArmyInputBox::RefreshWithCachedLength()
{
	const FText LineLengthText = FText::FromString(FString::Printf(TEXT("%.0f mm"), CachedLength * 10));
	SetInputText(LineLengthText);
}

void SArmyInputBox::Revert()
{
	if (InputText.IsValid())
	{
		InputText->Revert();
	}
}
bool SArmyInputBox::IsCharacterNumeric(const TCHAR InChar) const
{
    return (InChar >= 48 && InChar <= 57);
}

void SArmyInputBox::OnTextCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	float TempInValue = FCString::Atof(*InText.ToString()) / 10.f;
	if (FMath::IsNearlyEqual(CachedLength, TempInValue, 0.0001f))
	{
		const FText LineLengthText = FText::FromString(FString::Printf(TEXT("%.0f mm"), TempInValue * 10));
		SetInputText(LineLengthText);
	}	
    OnInputBoxCommitted.ExecuteIfBound(InText, InTextAction);
	//OnTextCommittedForThis(InText, ETextCommit::OnEnter, this->AsShared());
}

void SArmyInputBox::OnTextCommittedForThis(const FText& InText, const ETextCommit::Type InTextAction, TSharedPtr<SWidget> InThisWidget)
{
	OnInputBoxForThisCommitted.ExecuteIfBound(InText, InTextAction, this->AsShared());
}
