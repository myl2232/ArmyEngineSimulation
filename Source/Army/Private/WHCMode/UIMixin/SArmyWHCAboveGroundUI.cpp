#include "UIMixin/SArmyWHCAboveGroundUI.h"
#include "WHCMode/SArmyWHCModeAttrPanel.h"
#include "WHCMode/XRShapeFrameActor.h"
#include "WHCMode/XRWHCabinet.h"
#include "WHCMode/XRWHCModeCabinetOperation.h"
#include "WHCMode/XRWHCabinetAutoMgr.h"
#include "ArmyStyle.h"
#include "Data/WHCModeData/XRWHCModeData.h"

TSharedPtr<SWidget> FArmyWHCAboveGroundUI::MakeWidget()
{
    return SNew(SBox)
        .HeightOverride(24.0f)
        [
            SNew(SBorder)
            .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF38393D"))
            .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
            .Padding(FMargin(8.0f, 0.0f, 0.0f, 0.0f))
            .VAlign(VAlign_Center)
            [
                SNew(SEditableText)
                .Text_Lambda(
                    [this]() -> FText {
                        AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
                        if (ShapeFrame != nullptr)
                        {
                            check(ShapeFrame->ShapeInRoomRef != nullptr);
                            float AboveGround = ShapeFrame->ShapeInRoomRef->AboveGround * 10.0f;
                            return FText::FromString(FString::Printf(TEXT("%d"), FMath::RoundToInt(AboveGround)));
                        }
                        else
                            return FText::FromString(TEXT("0"));
                    }
                )
                .Font(FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12").Font)
                .ColorAndOpacity(FLinearColor::White)
                .SelectAllTextWhenFocused(true)
                .OnIsTypedCharValid_Lambda(
                    [](const TCHAR c) -> bool {
                        return (c >= TEXT('0')) && (c <= TEXT('9'));
                    }
                )
                .OnTextCommitted_Lambda(
                    [this](const FText& InText, const ETextCommit::Type InTextAction) {
                        AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
                        if (ShapeFrame == nullptr)
                            return;
                        if (InTextAction == ETextCommit::OnEnter)
                        {
                            float NewHeight = FCString::Atof(*InText.ToString());
                            check(ShapeFrame->ShapeInRoomRef != nullptr);
                            if (CheckIfChangeAllCabHeights(ShapeFrame->ShapeInRoomRef))
                                gCabinMgr.OnUIAboveGroundChange(NewHeight);
                            else
                                gCabinMgr.OnUIAboveGroundChangeSingle(NewHeight, ShapeFrame->ShapeInRoomRef);
                            Container->CabinetOperationRef->CalculateShapeDistanceFromWall(ShapeFrame->ShapeInRoomRef);
                        }
                    }
                )
            ]
        ];
}

bool FArmyWHCAboveGroundUI::CheckIfChangeAllCabHeights(FShapeInRoom *InShapeInRoom) const
{
    ECabinetType Type = (ECabinetType)InShapeInRoom->Shape->GetShapeCategory();
    switch (Type)
    {
        case ECabinetType::EType_OnGroundCab:
        case ECabinetType::EType_TallCab:
            return true;
        default:
            return false;
    }
}