#include "SArmyThrobber.h"
#include "Style/ArmyStyle.h"
#include "Widgets/Images/SImage.h"
#include "Interfaces/IHttpRequest.h"

void SArmyThrobber::Construct(const FArguments& InArgs)
{
    ImgRotDegree = 0;

    ChildSlot
    [
        SNew(SBorder)
        .Padding(FMargin(0))
        .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.None"))
        .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        [
            SNew(SBox)
            .HeightOverride(48)
		    [
			    SNew(SBorder)
			    .Padding(FMargin(20, 0))
			    .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF3F4042"))
			    .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
			    [
				    SNew(SHorizontalBox)

				    + SHorizontalBox::Slot()
				    .AutoWidth()
				    .VAlign(VAlign_Center)
				    [
                        SAssignNew(Img_Throbble, SImage)
                        .Image(FArmyStyle::Get().GetBrush("Icon.Throbber"))
				    ]

				    + SHorizontalBox::Slot()
				    .Padding(FMargin(10, 0, 0, 0))
				    .AutoWidth()
				    .VAlign(VAlign_Center)
				    [
					    SAssignNew(TB_Message, STextBlock)
					    .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
				    ]

				    + SHorizontalBox::Slot()
                    .Padding(FMargin(10, 0, 0, 0))
				    .AutoWidth()
				    .VAlign(VAlign_Center)
				    [
                        SAssignNew(Btn_Cancel, SButton)
                        .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Close"))
                        .OnClicked(this, &SArmyThrobber::OnCancelClicked)
				    ]
			    ]
		    ]
        ]
    ];

    ActiveTimerHandle = RegisterActiveTimer(1.f / 60.f, FWidgetActiveTimerDelegate::CreateSP(this, &SArmyThrobber::ActiveTick));
}

void SArmyThrobber::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    ImgRotDegree = (ImgRotDegree + 6) % 360;
    Img_Throbble->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
    Img_Throbble->SetRenderTransform(FSlateRenderTransform(FQuat2D(FMath::DegreesToRadians(ImgRotDegree))));
}

void SArmyThrobber::SetCanceable(bool bCanceable, TSharedPtr<IHttpRequest> InCanceableRequest)
{
    if (bCanceable)
    {
        Btn_Cancel->SetVisibility(EVisibility::Visible);
        CanceableRequest = InCanceableRequest;
    }
    else
    {
        Btn_Cancel->SetVisibility(EVisibility::Collapsed);
    }
}

void SArmyThrobber::SetOnCancel(FSimpleDelegate InOnCancel)
{
    Btn_Cancel->SetVisibility(EVisibility::Visible);
    OnCancel = InOnCancel;
}

FReply SArmyThrobber::OnCancelClicked()
{
    OnCancel.ExecuteIfBound();

    if (CanceableRequest.IsValid())
    {
        CanceableRequest->OnProcessRequestComplete().Unbind();
        CanceableRequest->CancelRequest();
        CanceableRequest = nullptr;
    }
    SetVisibility(EVisibility::Hidden);

    return FReply::Handled();
}

EActiveTimerReturnType SArmyThrobber::ActiveTick(double InCurrentTime, float InDeltaTime)
{
    return EActiveTimerReturnType::Continue;
}