#include "SArmyMessage.h"
#include "Style/ArmyStyle.h"
#include "Engine.h"

void SArmyMessage::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SBox)
        .HeightOverride(40)
        [
			SAssignNew(BackgroundBorder, SBorder)
			.Padding(FMargin(1))
			.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SAssignNew(MessageBorder, SBorder)
				.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(FMargin(16,0,16,0))
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					[
						SAssignNew(Img_Icon, SImage)
					]

					+ SHorizontalBox::Slot()
					.Padding(8, 0, 0, 0)
					.AutoWidth()
					.VAlign(VAlign_Center)
					[
						SAssignNew(TB_Message, STextBlock)
						.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
					]
				]
			]
        ]
    ];

    HideMessageImpl();
}

void SArmyMessage::HideMessageImpl()
{
    SetVisibility(EVisibility::Hidden);
}

void SArmyMessage::ShowMessage(EArmyMessageType MessageType, const FString& Message)
{
    if (MessageType == EArmyMessageType::MT_Success)
    {
		BackgroundBorder->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF8BC7A2"));
        MessageBorder->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF2A2E2F"));
        Img_Icon->SetImage(FArmyStyle::Get().GetBrush("Icon.Success"));
    }
    else if (MessageType == EArmyMessageType::MT_Warning)
	{
		BackgroundBorder->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FFFF8080"));
        MessageBorder->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF2E2C2F"));
        Img_Icon->SetImage(FArmyStyle::Get().GetBrush("Icon.Warning"));
	}
	else if (MessageType == EArmyMessageType::MT_Normal)
	{
		BackgroundBorder->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF5D5F66"));
		MessageBorder->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"));
		Img_Icon->SetImage(FArmyStyle::Get().GetBrush("Icon.Normal"));
	}

    TB_Message->SetText(FText::FromString(Message));
	TB_Message->SetColorAndOpacity(FLinearColor::White);
    SetVisibility(EVisibility::HitTestInvisible);

	GEngine->GameViewport->GetWorld()->GetTimerManager().SetTimer(MessageTimeHandle, FTimerDelegate::CreateRaw(this, &SArmyMessage::HideMessage), 3.f, false);
}

void SArmyMessage::ShowMessageWithoutTimeout(EArmyMessageType MessageType, const FString& Message)
{
	if (MessageType == EArmyMessageType::MT_Success)
	{
		BackgroundBorder->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF8BC7A2"));
		MessageBorder->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF2A2E2F"));
		Img_Icon->SetImage(FArmyStyle::Get().GetBrush("Icon.Success"));
	}
	else if (MessageType == EArmyMessageType::MT_Warning)
	{
		BackgroundBorder->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FFFF8080"));
		MessageBorder->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF2E2C2F"));
		Img_Icon->SetImage(FArmyStyle::Get().GetBrush("Icon.Warning"));
	}
	else if (MessageType == EArmyMessageType::MT_Normal)
	{
		BackgroundBorder->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF5D5F66"));
		MessageBorder->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"));
		Img_Icon->SetImage(FArmyStyle::Get().GetBrush("Icon.Normal"));
	}

	TB_Message->SetText(FText::FromString(Message));
	TB_Message->SetColorAndOpacity(FLinearColor::White);
	SetVisibility(EVisibility::HitTestInvisible);
}

void SArmyMessage::HideMessage()
{
	HideMessageImpl();
}