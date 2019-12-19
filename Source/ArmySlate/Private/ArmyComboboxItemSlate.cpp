#include "ArmyComboboxItemSlate.h"
#include "Widgets/SCompoundWidget.h"
#include "STextBlock.h"
#include "SBox.h"
#include "SBoxPanel.h"
#include "ArmyStyle.h"

void SArmyComboboxItem::Construct(const FArguments& InArgs)
{
	TextNormalColor = InArgs._TextNormalColor;
	TextSelectedColor = InArgs._TextSelectedColor;
	NormalImage = InArgs._NormalImage;
	SelectedImage = InArgs._SelectedImage;
	Text = InArgs._Text;
	ImagePadding = InArgs._ImagePadding;
	TextBlockPadding = InArgs._TextBlockPadding;
	Height = InArgs._Height;

	ChildSlot
	[
		SNew(SBox)
		.HeightOverride(Height)
		.VAlign(VAlign_Center)
		.HAlign(InArgs._ItemHorizontalAlignment.Get())
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.Padding(ImagePadding)
			.AutoWidth()
			[
				SAssignNew(Image, SImage)
				.Image(NormalImage)
			]

			+ SHorizontalBox::Slot()
			.Padding(TextBlockPadding)
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SAssignNew(TextBlock, STextBlock)
				.Text(Text)
				.TextStyle(InArgs._TextStyle)
				.ColorAndOpacity(FLinearColor::White)
			]
		]
	];
}

FReply SArmyComboboxItem::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	Image->SetImage(SelectedImage);
	TextBlock->SetColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFFF9800"));
	return FReply::Unhandled();
}

void SArmyComboboxItem::SetTextColorAndImageBrushNormal()
{
	Image->SetImage(NormalImage);
	TextBlock->SetColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"));
}

void SArmyComboboxItem::SetTextColorAndImageBrushSelected()
{
    Image->SetImage(SelectedImage);
    TextBlock->SetColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFFF9800"));
}

FString SArmyComboboxItem::GetText()
{
	return Text.Get().ToString();
}
