// Fill out your copyright notice in the Description page of Project Settings.

#include "ArmyEditableLabelSlate.h"
#include "SlateOptMacros.h"
#include "Style/ArmyStyle.h"
#include "Regex.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SSArmyEditableLabel::Construct(const FArguments& InArgs)
{
	bCheckNumeric = InArgs._CheckNumeric;
	ChildSlot
	[
		SNew(SBox)
		.MinDesiredWidth(32)
		.HAlign(HAlign_Fill)
		[
			SNew(SBorder)
			.BorderBackgroundColor(FArmyStyle::GetColor("Color.White"))
			.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
			.Padding(FMargin(2, 2, 2, 2))
			[
				SAssignNew(InputText, SArmyEditableText)
				.Text(FText::FromString(TEXT("0")))
				.SelectAllTextWhenFocused(true)
				.SelectAllTextOnCommit(true)
				.OnTextCommitted(this, &SSArmyEditableLabel::OnTextCommitted)
				.OnIsTypedCharValid(this, &SSArmyEditableLabel::IsCharacterNumeric)
                .Style(FArmyStyle::Get(), "EditableText")
                .AllowContextMenu(false)
			]
		]
	];
	InputText->SetIfOnlyInteger(InArgs._IsIntegerOnly.Get());
}
bool SSArmyEditableLabel::IsCharacterNumeric(const TCHAR InChar) const
{
	if (bCheckNumeric.Get(true))
	{
		return (InChar >= 48 && InChar <= 57) || (InChar == 45);
	}
	return true;
}
void SSArmyEditableLabel::OnTextCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter || InTextAction == ETextCommit::OnUserMovedFocus)
	{
		OnInputBoxCommitted.ExecuteIfBound(InText, this);
	}
}
void SSArmyEditableLabel::SetInputText(const FText& InText)
{
	InputText->SetText(InText);
}
void SSArmyEditableLabel::SetFocus()
{
	FSlateApplication::Get().SetKeyboardFocus(InputText);
	InputText->SelectAllText();
}
void SSArmyEditableLabel::Revert()
{
	InputText->Revert();
}
bool SSArmyEditableLabel::HasTextChangedFromOriginal()
{
	return InputText->HasTextChangedFromOriginal();
}
FReply SSArmyEditableLabel::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		FSlateApplication::Get().SetAllUserFocusToGameViewport();
	}
	return FReply::Handled();
}
FString SSArmyEditableLabel::GetText()
{	
	return InputText->GetText().ToString();
}

void SArmyEditableText::Construct(const FArguments& InArgs)
{
    SEditableText::Construct(InArgs);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply SArmyEditableText::OnKeyChar(const FGeometry & MyGeometry, const FCharacterEvent & InCharacterEvent)
{
	if ((!bOnlyInteger) && (!bOnlyFloat)) {
		return SEditableText::OnKeyChar(MyGeometry, InCharacterEvent);
	}
	/*@欧石楠 暴力魔改。。。**/
	const TCHAR Character = InCharacterEvent.GetCharacter();

	switch (Character)
	{
		case TCHAR('.'):
			if (!bOnlyFloat)
				return FReply::Handled();
		case TCHAR(8):				
		case TCHAR('\t'):				
		case TCHAR('\n'):		
		case 1:				
		case 3:				
		case 13:			
		case 22:		
		case 24:		
		case 25:		
		case 26:			
		case 27:			
		case 127:			
			return EditableTextLayout->HandleKeyChar(InCharacterEvent);

		default:
			FString CharStr = "";
			CharStr.AppendChar(InCharacterEvent.GetCharacter());
			if (CheckNumberIsVaild(CharStr)) {
				return EditableTextLayout->HandleKeyChar(InCharacterEvent);
			}
			return FReply::Handled();
	}	
}

void SArmyEditableText::SetIfOnlyInteger(bool IfOnly)
{
	bOnlyInteger = IfOnly;
}

void SArmyEditableText::SetIfOnlyFloat(bool IfOnly)
{
	bOnlyFloat = IfOnly;
}
bool SArmyEditableText::HasTextChangedFromOriginal()
{
	return EditableTextLayout->HasTextChangedFromOriginal();
}
bool SArmyEditableText::CheckNumberIsVaild(const FString & str)
{
	FString Reg = TEXT("^[0-9]+([.][0-9]+){0,1}$");
	FRegexPattern Pattern(Reg);
	FRegexMatcher regMatcher(Pattern, str);
	regMatcher.SetLimits(0, str.Len());
	return regMatcher.FindNext();
}
