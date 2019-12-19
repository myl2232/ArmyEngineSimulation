#include "ArmyFacsimileOperation.h"
#include "ArmyModalManager.h"
#include "SBox.h"
#include "SArmyEditableNumberBox.h"
#include "ArmyReferenceImage.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "ArmyEngineResource.h"
#include "ArmySceneData.h"
#include "ArmyToolsModule.h"
#include "ArmyRectSelect.h"
#include "ArmyStyle.h"
#include "ArmyGameInstance.h"

#define LOCTEXT_NAMESPACE "ArmyFacsimileOperation"

#define DRAW_FACSIMILE_MSG TEXT("请先绘制一条直线，然后输入该直线所代表的实际长度")

FArmyFacsimileOperation::FArmyFacsimileOperation(EModelType InBelongModel) : FArmyOperation(InBelongModel)
{
    CurrentState = -1;
    ENB_ScaleValue = nullptr;
}

bool OpenFileDialog(UGameViewportClient* InViewportClient, const FString& DialogTitle, const FString& DefaultPath, TArray<FString>& OutFileNames, FString _FileType, bool _bMulty)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform)
	{
		const void* ParentWindowWindowHandle = NULL;
		ParentWindowWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(InViewportClient->GetGameViewportWidget());
		FString FolderName;
		const bool bFolderSelected = DesktopPlatform->OpenFileDialog(
			ParentWindowWindowHandle,
			DialogTitle,
			DefaultPath,
			TEXT(""),
			_FileType,//TEXT("Pictures (*.jpg)|*.jpg;*.jpeg|Pictures (*.png)|*.png"),
			_bMulty ? EFileDialogFlags::Multiple : EFileDialogFlags::None,
			OutFileNames
		);

		if (bFolderSelected)
		{
			return true;
		}
	}
	return false;
}

void FArmyFacsimileOperation::DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, const FSceneView* View, FCanvas* Canvas)
{
	if (CurrentState == 2)
	{
		FCanvasLineItem FacsimileLine(FacsimileStartPos, FacsimileEndPos);
        FacsimileLine.SetColor(FArmyStyle::Get().GetColor("Color.FFFF9800"));
		FacsimileLine.Draw(Canvas);

        FVector2D FacsimileDir = (FacsimileEndPos - FacsimileStartPos).GetSafeNormal();
        FVector2D UpDir = FacsimileDir.GetRotated(-90.f);
        FCanvasLineItem StartLine(FacsimileStartPos + UpDir * 12.f, FacsimileStartPos - UpDir * 12.f);
        StartLine.SetColor(FArmyStyle::Get().GetColor("Color.FFFF9800"));
        StartLine.Draw(Canvas);

        FCanvasLineItem EndLine(FacsimileEndPos + UpDir * 12.f, FacsimileEndPos - UpDir * 12.f);
        EndLine.SetColor(FArmyStyle::Get().GetColor("Color.FFFF9800"));
        EndLine.Draw(Canvas);
	}
}

void FArmyFacsimileOperation::BeginOperation(XRArgument InArg)
{
    FArmyToolsModule::Get().GetRectSelectTool()->End();

    // false为导入底图，true跳过导入底图，直接绘制比例尺
    if (InArg._ArgBoolean)
    {
        GGI->Window->ShowMessageWithoutTimeout(MT_Normal, DRAW_FACSIMILE_MSG);
        GVC->CurCursor = EMouseCursor::Crosshairs;
        CurrentState = 1;
    }
    else
    {
        TArray<FString> OutPaths;
        OpenFileDialog(GVC, TEXT("请选择一张底图"), FPaths::ConvertRelativePathToFull(FPaths::RootDir()), OutPaths, TEXT("*.jpg *.png *.bmp(24位)|*.jpg;*.jpeg;*.png;*.bmp"), false);
        if (OutPaths.Num() > 0)
        {
            TSharedPtr<FArmyReferenceImage> Facsimile = MakeShareable(new FArmyReferenceImage());
            if (Facsimile->LoadFacsimile(OutPaths[0]))
            {
                FArmySceneData::Get()->SetFacsimile(Facsimile);

                GGI->Window->ShowMessageWithoutTimeout(MT_Normal, DRAW_FACSIMILE_MSG);
                GVC->CurCursor = EMouseCursor::Crosshairs;
                CurrentState = 1;
            }
        }
        else
        {
            EndOperation();
        }
    }
}

void FArmyFacsimileOperation::EndOperation()
{
	FArmyOperation::EndOperation();
	CurrentState = -1;
    GGI->Window->DismissModalDialog();
    GGI->Window->HideMessage();
    GVC->CurCursor = EMouseCursor::Default;
}

void FArmyFacsimileOperation::Tick()
{
	if (CurrentState == 2)
	{
		FVector2D MousePos;
		GVC->GetMousePosition(MousePos);
		FVector2D TempPoint = MousePos - FacsimileStartPos;
		if (FMath::Abs(TempPoint.X) > FMath::Abs(TempPoint.Y))
		{
			FacsimileEndPos.X = MousePos.X;
			FacsimileEndPos.Y = FacsimileStartPos.Y;
		}
		else
		{
			FacsimileEndPos.X = FacsimileStartPos.X;
			FacsimileEndPos.Y = MousePos.Y;
		}
	}
}

bool FArmyFacsimileOperation::InputKey(UArmyEditorViewportClient * InViewPortClient, FViewport * InViewPort, FKey Key, EInputEvent Event)
{
	if (InViewPort->KeyState(EKeys::LeftMouseButton) && Event == IE_Pressed)
	{
		if (CurrentState == 1)
		{
			GVC->GetMousePosition(FacsimileStartPos);			
			FacsimileEndPos = FacsimileStartPos;
			CurrentState = 2;
			return true;
		}
		else if (CurrentState == 2)
		{
			FVector2D MousePos;
			GVC->GetMousePosition(MousePos);			
			FVector2D TempPoint = MousePos - FacsimileStartPos;
			if (FMath::Abs(TempPoint.X) > FMath::Abs(TempPoint.Y))
			{
				FacsimileEndPos.X = MousePos.X;
				FacsimileEndPos.Y = FacsimileStartPos.Y;
			}
			else
			{
				FacsimileEndPos.X = FacsimileStartPos.X;
				FacsimileEndPos.Y = MousePos.Y;
			}

            GGI->Window->HideMessage();
            GVC->CurCursor = EMouseCursor::Default;
			CurrentState = 0;
			ShowInputScaleWindow();

			return true;
		}
	}

	return false;
}

void FArmyFacsimileOperation::ShowInputScaleWindow()
{
    TSharedPtr<SWidget> ScaleValueWidget = nullptr;

	SAssignNew(ScaleValueWidget, SBox)
	.WidthOverride(420)
    .HeightOverride(122)
    .Padding(20) 
	[
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Fill)
        [
            SNew(SBox)
            .HeightOverride(32)
            [
                SNew(SHorizontalBox)

		        + SHorizontalBox::Slot()
		        .VAlign(VAlign_Center)
		        .AutoWidth()
		        [
			        SNew(STextBlock)
			        .Text(MAKE_TEXT("请输入真实长度"))
                    .TextStyle(FArmyStyle::Get(), "ArmyText_12")
		        ]

		        + SHorizontalBox::Slot()
		        .AutoWidth()
                .Padding(16, 0, 8, 0)
		        [
			        SNew(SBox)
			        .WidthOverride(250)
			        [
                        SAssignNew(ENB_ScaleValue, SArmyEditableNumberBox)
				        .Style(FArmyStyle::Get(), "EditableTextBox")
                        .HintText(MAKE_TEXT("1000mm - 100000mm"))
				        .IsFloatOnly(true)
				        .Text(FText::FromString("1000"))
                        .MaxTextLength(6)
                        .OnTextCommitted_Lambda([this](const FText& InText, ETextCommit::Type InCommitedType) {
                            // 输入框回车设置比例尺
                            if (InCommitedType == ETextCommit::OnEnter)
                            {
                                int32 Value = FCString::Atoi(*(InText.ToString()));
                                Value = FMath::Clamp<int32>(Value, 1000, 100000);
                                SetFacsimileScale(Value);
                                EndOperation();
                            }
                        })
			        ]
		        ]

                + SHorizontalBox::Slot()
		        .VAlign(VAlign_Center)
		        .FillWidth(1.f)
		        [
			        SNew(STextBlock)
			        .Text(MAKE_TEXT("mm"))
                    .TextStyle(FArmyStyle::Get(), "ArmyText_12")
		        ]
            ]
        ]

		+ SVerticalBox::Slot()
        .Padding(0, 20, 0, 0)
        .FillHeight(1.f)
        .HAlign(HAlign_Right)
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
            .Padding(0, 0, 20, 0)
            .AutoWidth()
            [
                SNew(SBox)
                .WidthOverride(80)
                .HeightOverride(30)
                [
                    SNew(SButton)
                    .ButtonStyle(FArmyStyle::Get(), "Button.Orange")
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                    .OnClicked_Lambda([this]() {
                        // 确认键设置比例尺
                        float Value = FCString::Atof(*(ENB_ScaleValue->GetText().ToString()));
                        SetFacsimileScale(Value);
                        EndOperation();
                        return FReply::Handled();
                    })
                    [
                        SNew(STextBlock)
                        .Text(MAKE_TEXT("确认"))
                        .TextStyle(FArmyStyle::Get(), "ArmyText_12")
                    ]
                ]
            ]
            
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SBox)
                .WidthOverride(80)
                .HeightOverride(30)
                [
                    SNew(SButton)
                    .ButtonStyle(FArmyStyle::Get(), "Button.Gray.FF353638")
                    .OnClicked_Lambda([this]() {
                        CurrentState = 1;
                        GVC->CurCursor = EMouseCursor::Crosshairs;
                        GGI->Window->DismissModalDialog();
                        GGI->Window->ShowMessageWithoutTimeout(MT_Normal, DRAW_FACSIMILE_MSG);
                        return FReply::Handled();
                    })
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Center)
                    [
                        SNew(STextBlock)
                        .Text(MAKE_TEXT("重新绘制"))
                        .TextStyle(FArmyStyle::Get(), "ArmyText_12")
                    ]
                ]
            ]
        ]
	];

    GGI->Window->PresentModalDialog(TEXT("设置比例尺"), ScaleValueWidget->AsShared());
}

void FArmyFacsimileOperation::SetFacsimileScale(int32 InScale)
{
    FVector4 EndVector4 = GVC->PixelToWorld(FacsimileEndPos.X, FacsimileEndPos.Y, 0);
    FacsimileEndPos.X = EndVector4.X;
    FacsimileEndPos.Y = EndVector4.Y;
    FVector4 StartVector4 = GVC->PixelToWorld(FacsimileStartPos.X, FacsimileStartPos.Y, 0);
    FacsimileStartPos.X = StartVector4.X;
    FacsimileStartPos.Y = StartVector4.Y;
    float TempLen = (FacsimileEndPos - FacsimileStartPos).Size();
    if (FArmySceneData::Get()->GetFacsimile().IsValid() && CurrentState == 0)
    {
        FArmySceneData::Get()->GetFacsimile().Pin()->SetScale(InScale / (10 * TempLen));
        EndOperation();
    }
}

#undef LOCTEXT_NAMESPACE
