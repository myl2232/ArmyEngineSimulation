#include "ArmyEditableNumberBoxSlate.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBox.h"
#include "ArmyToolTipSlate.h"
#include "ArmyStyle.h"

/**
* Construct this widget
*
* @param	InArgs	The declaration data for this widget
*/
void SArmyEditableNumberBox::Construct(const FArguments& InArgs)
{
	check(InArgs._Style);
	SetStyle(InArgs._Style);

	PaddingOverride = InArgs._Padding;
	FontOverride = InArgs._Font;
	ForegroundColorOverride = InArgs._ForegroundColor;
	BackgroundColorOverride = InArgs._BackgroundColor;
	ReadOnlyForegroundColorOverride = InArgs._ReadOnlyForegroundColor;
    MaxTextLength = InArgs._MaxTextLength;
    OnTextChanged = InArgs._OnTextChanged;

    TAttribute<FMargin> MarginAttr = TAttribute<FMargin>::Create(TAttribute<FMargin>::FGetter::CreateSP(this, &SArmyEditableNumberBox::GetEditableTextPadding));

	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SArmyEditableNumberBox::GetBorderImage)
		.BorderBackgroundColor(this, &SArmyEditableNumberBox::DetermineBackgroundColor)
		.ForegroundColor(this, &SArmyEditableNumberBox::DetermineForegroundColor)
		.Padding(0)
		[
			SNew(SOverlay)

			+ SOverlay::Slot()
		    .VAlign(VAlign_Fill)
		    .HAlign(HAlign_Fill)
            .Padding(MarginAttr)
		    [
			    SAssignNew(PaddingBox, SBox)
			    .Padding(this, &SArmyEditableNumberBox::DeterminePadding)
		        .VAlign(VAlign_Center)
		        [
			        SAssignNew(EditableText, SArmyEditableText)
			        .Text(InArgs._Text)
		            .HintText(InArgs._HintText)
		            .SearchText(InArgs._SearchText)
		            .Font(this, &SArmyEditableNumberBox::DetermineFont)
		            .IsReadOnly(InArgs._IsReadOnly)
		            .IsPassword(InArgs._IsPassword)
		            .IsCaretMovedWhenGainFocus(InArgs._IsCaretMovedWhenGainFocus)
		            .SelectAllTextWhenFocused(InArgs._SelectAllTextWhenFocused)
		            .RevertTextOnEscape(InArgs._RevertTextOnEscape)
		            .ClearKeyboardFocusOnCommit(InArgs._ClearKeyboardFocusOnCommit)
		            .AllowContextMenu(InArgs._AllowContextMenu)
		            .OnContextMenuOpening(InArgs._OnContextMenuOpening)
		            .OnTextChanged_Lambda([this](const FText& InText) {
                        if (MaxTextLength <= 0)
                        {
                            return;
                        }

                        // 限制文本输入不超过设置的最大长度
                        FString InputStr = InText.ToString();
                        if (InputStr.Len() > MaxTextLength)
                        {
                            SetText(FText::FromString(InputStr.Left(MaxTextLength)));
                        }
                        else
                        {
                            OnTextChanged.ExecuteIfBound(InText);
                        }
                    })
		            .OnTextCommitted(InArgs._OnTextCommitted)
		            .MinDesiredWidth(InArgs._MinDesiredWidth)
		            .SelectAllTextOnCommit(InArgs._SelectAllTextOnCommit)
		            .OnKeyDownHandler(InArgs._OnKeyDownHandler)
		            .VirtualKeyboardType(InArgs._VirtualKeyboardType)
		            .VirtualKeyboardTrigger(InArgs._VirtualKeyboardTrigger)
		            .VirtualKeyboardDismissAction(InArgs._VirtualKeyboardDismissAction)
		            .TextShapingMethod(InArgs._TextShapingMethod)
		            .TextFlowDirection(InArgs._TextFlowDirection)
                    .AllowContextMenu(false)
		        ]
		    ]

            + SOverlay::Slot()
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Center)
            .Padding(0, 0, 8, 0)
            [
                SAssignNew(Img_ToolTip, SImage)
                .Image(FArmyStyle::Get().GetBrush("Icon.Alert"))
                .ToolTip(SNew(SArmyToolTip).Text(InArgs._TipText))
                .Visibility(EVisibility::Collapsed)
            ]
		]
	);

	/*@欧石楠 设置仅整数**/
	EditableText->SetIfOnlyInteger(InArgs._IsIntegerOnly.Get());
	/*@欧石楠 设置仅浮点数**/
	EditableText->SetIfOnlyFloat(InArgs._IsFloatOnly.Get());
}

void SArmyEditableNumberBox::SetStyle(const FEditableTextBoxStyle* InStyle)
{
	Style = InStyle;

	if (Style == nullptr)
	{
		FArguments Defaults;
		Style = Defaults._Style;
	}

	check(Style);

	BorderImageNormal = &Style->BackgroundImageNormal;
	BorderImageHovered = &Style->BackgroundImageHovered;
	BorderImageFocused = &Style->BackgroundImageFocused;
	BorderImageReadOnly = &Style->BackgroundImageReadOnly;
}

void SArmyEditableNumberBox::SetText(const TAttribute< FText >& InNewText)
{
	EditableText->SetText(InNewText);
}

void SArmyEditableNumberBox::SetOnKeyDownHandler(FOnKeyDown InOnKeyDownHandler)
{
	EditableText->SetOnKeyDownHandler(InOnKeyDownHandler);
}


void SArmyEditableNumberBox::SetTextShapingMethod(const TOptional<ETextShapingMethod>& InTextShapingMethod)
{
	EditableText->SetTextShapingMethod(InTextShapingMethod);
}


void SArmyEditableNumberBox::SetTextFlowDirection(const TOptional<ETextFlowDirection>& InTextFlowDirection)
{
	EditableText->SetTextFlowDirection(InTextFlowDirection);
}


bool SArmyEditableNumberBox::AnyTextSelected() const
{
	return EditableText->AnyTextSelected();
}


void SArmyEditableNumberBox::SelectAllText()
{
	EditableText->SelectAllText();
}


void SArmyEditableNumberBox::ClearSelection()
{
	EditableText->ClearSelection();
}


FText SArmyEditableNumberBox::GetSelectedText() const
{
	return EditableText->GetSelectedText();
}

void SArmyEditableNumberBox::GoTo(const FTextLocation& NewLocation)
{
	EditableText->GoTo(NewLocation);
}

void SArmyEditableNumberBox::ScrollTo(const FTextLocation& NewLocation)
{
	EditableText->ScrollTo(NewLocation);
}

void SArmyEditableNumberBox::BeginSearch(const FText& InSearchText, const ESearchCase::Type InSearchCase, const bool InReverse)
{
	EditableText->BeginSearch(InSearchText, InSearchCase, InReverse);
}

void SArmyEditableNumberBox::AdvanceSearch(const bool InReverse)
{
	EditableText->AdvanceSearch(InReverse);
}

void SArmyEditableNumberBox::ShowToolTip(bool bShow)
{
    EVisibility Visibility = bShow ? EVisibility::Visible : EVisibility::Collapsed;
    Img_ToolTip->SetVisibility(Visibility);
}

const FSlateBrush* SArmyEditableNumberBox::GetBorderImage() const
{
	if (EditableText->IsTextReadOnly())
	{
		return BorderImageReadOnly;
	}
	else if (EditableText->HasKeyboardFocus())
	{
		return BorderImageFocused;
	}
	else
	{
		if (EditableText->IsHovered())
		{
			return BorderImageHovered;
		}
		else
		{
			return BorderImageNormal;
		}
	}
}


bool SArmyEditableNumberBox::SupportsKeyboardFocus() const
{
	return StaticCastSharedPtr<SWidget>(EditableText)->SupportsKeyboardFocus();
}


bool SArmyEditableNumberBox::HasKeyboardFocus() const
{
	// Since keyboard focus is forwarded to our editable text, we will test it instead
	return SBorder::HasKeyboardFocus() || EditableText->HasKeyboardFocus();
}


FReply SArmyEditableNumberBox::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	FReply Reply = FReply::Handled();

	if (InFocusEvent.GetCause() != EFocusCause::Cleared)
	{
		// Forward keyboard focus to our editable text widget
		Reply.SetUserFocus(EditableText.ToSharedRef(), InFocusEvent.GetCause());
	}

	return Reply;
}


FReply SArmyEditableNumberBox::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::Escape && EditableText->HasKeyboardFocus())
	{
		// Clear focus
		return FReply::Handled().SetUserFocus(SharedThis(this), EFocusCause::Cleared);
	}

	return FReply::Unhandled();
}

FSlateColor SArmyEditableNumberBox::DetermineForegroundColor() const
{
	check(Style);

	if (EditableText->IsTextReadOnly())
	{
		if (ReadOnlyForegroundColorOverride.IsSet())
		{
			return ReadOnlyForegroundColorOverride.Get();
		}
		if (ForegroundColorOverride.IsSet())
		{
			return ForegroundColorOverride.Get();
		}

		return Style->ReadOnlyForegroundColor;
	}
	else
	{
		return ForegroundColorOverride.IsSet() ? ForegroundColorOverride.Get() : Style->ForegroundColor;
	}
}

void SArmyEditableNumberBox::SetHintText(const TAttribute< FText >& InHintText)
{
	EditableText->SetHintText(InHintText);
}


void SArmyEditableNumberBox::SetSearchText(const TAttribute<FText>& InSearchText)
{
	EditableText->SetSearchText(InSearchText);
}


FText SArmyEditableNumberBox::GetSearchText() const
{
	return EditableText->GetSearchText();
}


void SArmyEditableNumberBox::SetIsReadOnly(TAttribute< bool > InIsReadOnly)
{
	EditableText->SetIsReadOnly(InIsReadOnly);
}


void SArmyEditableNumberBox::SetIsPassword(TAttribute< bool > InIsPassword)
{
	EditableText->SetIsPassword(InIsPassword);
}


void SArmyEditableNumberBox::SetFont(const TAttribute<FSlateFontInfo>& InFont)
{
	FontOverride = InFont;
}

void SArmyEditableNumberBox::SetTextBoxForegroundColor(const TAttribute<FSlateColor>& InForegroundColor)
{
	ForegroundColorOverride = InForegroundColor;
}

void SArmyEditableNumberBox::SetTextBoxBackgroundColor(const TAttribute<FSlateColor>& InBackgroundColor)
{
	BackgroundColorOverride = InBackgroundColor;
}


void SArmyEditableNumberBox::SetReadOnlyForegroundColor(const TAttribute<FSlateColor>& InReadOnlyForegroundColor)
{
	ReadOnlyForegroundColorOverride = InReadOnlyForegroundColor;
}


void SArmyEditableNumberBox::SetMinimumDesiredWidth(const TAttribute<float>& InMinimumDesiredWidth)
{
	EditableText->SetMinDesiredWidth(InMinimumDesiredWidth);
}


void SArmyEditableNumberBox::SetIsCaretMovedWhenGainFocus(const TAttribute<bool>& InIsCaretMovedWhenGainFocus)
{
	EditableText->SetIsCaretMovedWhenGainFocus(InIsCaretMovedWhenGainFocus);
}


void SArmyEditableNumberBox::SetSelectAllTextWhenFocused(const TAttribute<bool>& InSelectAllTextWhenFocused)
{
	EditableText->SetSelectAllTextWhenFocused(InSelectAllTextWhenFocused);
}


void SArmyEditableNumberBox::SetRevertTextOnEscape(const TAttribute<bool>& InRevertTextOnEscape)
{
	EditableText->SetRevertTextOnEscape(InRevertTextOnEscape);
}


void SArmyEditableNumberBox::SetClearKeyboardFocusOnCommit(const TAttribute<bool>& InClearKeyboardFocusOnCommit)
{
	EditableText->SetClearKeyboardFocusOnCommit(InClearKeyboardFocusOnCommit);
}


void SArmyEditableNumberBox::SetSelectAllTextOnCommit(const TAttribute<bool>& InSelectAllTextOnCommit)
{
	EditableText->SetSelectAllTextOnCommit(InSelectAllTextOnCommit);
}

void SArmyEditableNumberBox::SetAllowContextMenu(TAttribute<bool> InAllowContextMenu)
{
	EditableText->SetAllowContextMenu(InAllowContextMenu);
}
