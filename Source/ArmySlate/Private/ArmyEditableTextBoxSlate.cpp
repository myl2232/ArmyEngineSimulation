#include "ArmyEditableTextBoxSlate.h"
#include "ArmyStyle.h"

#include "SEditableTextBox.h"

void SArmyEditableTextBox::Construct(const FArguments& InArgs)
{
    IconImage = InArgs._IconImage;
    MaxTextLength = InArgs._MaxTextLength;
    OnTextChanged = InArgs._OnTextChanged;

    TSharedPtr<SOverlay> Overlay = nullptr;

    ChildSlot
    [
        SNew(SBorder)
        .Padding(0)
        .BorderBackgroundColor(FArmyStyle::Get().GetColor("InputBox.Gray"))
        .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
        [
            SNew(SBox)
            .WidthOverride(328)
            .HeightOverride(44)
            [
                SAssignNew(Overlay, SOverlay)

                + SOverlay::Slot()
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Fill)
                [
                    SAssignNew(ETB, SEditableTextBox)
                    .Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
                    .IsPassword(InArgs._IsPassword)
                    .HintText(InArgs._HintText)
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
                    .AllowContextMenu(false)
                ]
            ]
        ]
    ];

    if (IconImage.IsValid())
    {
        Overlay->AddSlot()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Center)
            .Padding(10, 0, 0, 0)
            [
                IconImage.ToSharedRef()
            ];

        ETB->SetPadding(FMargin(44, 0, 0, 0));
    }
}

FText SArmyEditableTextBox::GetText()
{
	return ETB->GetText();
}

void SArmyEditableTextBox::SetText(const FText& InText)
{
    ETB->SetText(InText);
}

void SArmyPropertyEditableTextBox::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SBox)
        .WidthOverride(114)
        .HeightOverride(24)
        [
            SNew(SEditableTextBox)
            .Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
            .IsReadOnly(InArgs._IsReadOnly)
            .Text(InArgs._Text)
            .OnTextChanged(InArgs._OnTextChanged)
        ]
    ];
}
