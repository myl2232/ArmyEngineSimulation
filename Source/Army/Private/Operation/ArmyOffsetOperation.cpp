#include "ArmyOffsetOperation.h"
#include "ArmyToolsModule.h"
#include "ArmyRectSelect.h"
#include "ArmyMouseCapture.h"
#include "ArmyRoom.h"
#include "ArmySceneData.h"
#include "ArmyDesignEditor.h"
#include "ArmyGameInstance.h"
#include "ArmyMath.h"
#include "ArmyWallLine.h"

FArmyOffsetOperation::FArmyOffsetOperation(EModelType InBelongModel)
    :FArmyOperation(InBelongModel), OffsetValue(22)
    , bOffsetSetted(false)
{
	SelectedLine = NULL;
	SolidLine = MakeShareable(new FArmyLine());
	SolidLine->SetLineColor(FArmySceneData::Get()->GetWallLinesColor());
	DashLine = MakeShareable(new FArmyLine());
	DashLine->bIsDashLine = true;
	DashLine->SetBaseColor(FLinearColor::Gray);
}

void FArmyOffsetOperation::Init()
{
}

void FArmyOffsetOperation::InitWidget(TSharedPtr<SOverlay> InParentWidget)
{
	InParentWidget->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(InputBoxWidget, SArmyInputBox)
			.OnInputBoxCommitted(FOnTextCommitted::CreateRaw(this, &FArmyOffsetOperation::OnInputBoxCommitted))
		.OnKeyDown(FInputKeyDelegate::CreateRaw(this, &FArmyOffsetOperation::OnInputBoxKeyDown))
		];
}

void FArmyOffsetOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (IsOperating())
	{
		SolidLine->Draw(PDI, View);
		DashLine->Draw(PDI, View);
		/*if (SelectedArcWall.IsValid())
		{
			HelpArcWall->Draw(PDI, View);
		}*/
	}
}

void FArmyOffsetOperation::BeginOperation(XRArgument InArg /* = XRArgument() */)
{
    bOffsetSetted = false;

    // 默认显示输入框，方便输入偏移值
    InputBoxWidget->Show(true);
    InputBoxWidget->SetFocus(true);
	FVector2D MousePos;
	GVC->GetMousePosition(MousePos);
    FVector InputBoxPos = GVC->PixelToWorld(MousePos.X, MousePos.Y, 0) + FVector(10, 10, 0);
    SetInputBoxPos(InputBoxPos);

	FArmyToolsModule::Get().GetRectSelectTool()->Start();
	FArmyToolsModule::Get().GetRectSelectTool()->SetSelectUnit(FArmyRectSelect::Unit_Primitive);

    GGI->Window->ShowMessage(MT_Normal, TEXT("请输入墙厚尺寸，并点击Enter键确认"));

	TArray<FArmyRectSelect::FPrimitiveInfo> Lines = FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedPrimitives();
	if (Lines.Num() > 0)
	{
		SelectedLine = StaticCastSharedPtr<FArmyLine>(Lines[Lines.Num() - 1].Primitive);//TODO:临时处理，取最后一个
		if (SelectedLine.IsValid())
		{
			OperationLine();
		}
	}
}

void FArmyOffsetOperation::EndOperation()
{
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();

	bClickedEnter = false;
	InputBoxWidget->Show(false);

	if (SelectedLine.IsValid())
	{
		//SelectedLine->bShowPoint = true;
		//SelectedLine->SetState(OS_Normal);
		SelectedLine = NULL;
	}
	/*if (SelectedArcWall.IsValid())
	{
		SelectedArcWall->bShowPoint = true;
		SelectedArcWall->SetState(OS_Normal);
		SelectedArcWall = NULL;
	}*/
	SolidLine->SetStart(FVector::ZeroVector);
	SolidLine->SetEnd(FVector::ZeroVector);

	FArmyToolsModule::Get().GetRectSelectTool()->SetSelectUnit(FArmyRectSelect::Unit_Object);
}

void FArmyOffsetOperation::Tick()
{
    if (SelectedLine.IsValid())
    {
        OperationLine();
    }
	/*else if (SelectedArcWall.IsValid())
	{
		OperationArcWall();
	}*/
    else
    {
        if (bOffsetSetted)
        {
            // 输入偏移值后可以高亮需要操作的线段
           /* if (!LocalManager.Pin()->ForceHover(OT_Wall))
            {
                LocalManager.Pin()->ForceHover(OT_ArcWall);
            }*/
        }
        else
        {
            // 提供输入框输入偏移值
            InputBoxWidget->SetFocus(true);
			FVector2D MousePos;
			GVC->GetMousePosition(MousePos);
			FVector InputBoxPos = GVC->PixelToWorld(MousePos.X, MousePos.Y, 0) + FVector(10, 10, 0);
            SetInputBoxPos(InputBoxPos);
        }
    }
}

bool FArmyOffsetOperation::InputKey(UArmyEditorViewportClient* InViewPortClient, FViewport* InViewPort, FKey Key, EInputEvent Event)
{
	if (Key == EKeys::LeftMouseButton && Event == EInputEvent::IE_Pressed)
	{
		if (IsOperating())
		{
			// 让偏移生效
			FVector Offset = DashLine->GetEnd() - DashLine->GetStart();
			if (Offset.Size() > 0.0f)
			{
				Exec_ApplyOffset(Offset);
			}
		}
		else
		{
			// 选中要操作的线段
			TArray<FArmyRectSelect::FPrimitiveInfo> Lines = FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedPrimitives();
			if (Lines.Num() > 0)
			{
				SelectedLine = StaticCastSharedPtr<FArmyLine>(Lines[Lines.Num() - 1].Primitive);//TODO:临时处理，取最后一个
				if (SelectedLine.IsValid() && !bClickedEnter)
				{
					GGI->Window->ShowMessage(MT_Normal, TEXT("请输入墙厚尺寸，并点击Enter键确认"));
				}
				OperationLine();
			}
		}
	}
	else if ((Key == EKeys::RightMouseButton || Key == EKeys::Escape) && Event == EInputEvent::IE_Pressed)
		EndOperation();

	return false;
}

void FArmyOffsetOperation::Exec_ApplyOffset(const FVector& Offset)
{
	if (SelectedLine.IsValid())
	{
		TSharedPtr<FArmyWallLine> NewLine = MakeShareable(new FArmyWallLine(SelectedLine.Get()));
		NewLine->GetCoreLine()->SetState(FArmyPrimitive::OPS_Normal);
		NewLine->GetCoreLine()->SetStart(NewLine->GetCoreLine()->GetStart() + Offset);
		NewLine->GetCoreLine()->SetEnd(NewLine->GetCoreLine()->GetEnd() + Offset);
		FArmyToolsModule::Get().GetMouseCaptureTool()->AddAuxiliaryPoint(NewLine->GetCoreLine()->GetStartPointer());
		FArmyToolsModule::Get().GetMouseCaptureTool()->AddAuxiliaryPoint(NewLine->GetCoreLine()->GetEndPointer());
		NewLine->GetCoreLine()->GetStartPointer()->AddReferenceLine(NewLine->GetCoreLine());
		NewLine->GetCoreLine()->GetEndPointer()->AddReferenceLine(NewLine->GetCoreLine());
		TSharedPtr<FArmyRoom> Room = MakeShareable(new FArmyRoom());
		SCOPE_TRANSACTION(TEXT("添加空间"));
		FArmySceneData::Get()->Add(Room, XRArgument(1).ArgUint32(E_HomeModel).ArgFName(FName(TEXT("空间"))));
		Room->AddLine(NewLine);

		GGI->DesignEditor->CurrentController->SetOperation(0, 1);//开启绘制户型功能
	}
	/*else if (SelectedArcWall.IsValid())
	{
		TSharedPtr<FArmyArcWall> NewArcWall = MakeShareable(new FArmyArcWall(HelpArcWall.Get()));
		float angle = SelectedArcWall->GetArcAngle();
		FVector basePoint = SelectedArcWall->GetCenterPosition();
		FVector middlePoint = SelectedArcWall->ArcMiddlePoint->Point->Pos + Offset;
		FVector endPoint = FRotator(0, angle / 2, 0).RotateVector(middlePoint - basePoint) + basePoint;
		FVector startPoint = FRotator(0, -angle / 2, 0).RotateVector(middlePoint - basePoint) + basePoint;
		NewArcWall->SetArcWallThreePoint(startPoint, endPoint, middlePoint);
		XRArgument Arg = XRArgument().ArgFName(FName(*FString(TEXT("基础类")))).ArgString(NewArcWall->GetName()).ArgUint32(NewArcWall->GetUniqueID());
		COMMAND_ADD(LocalManager.Pin(), NewArcWall, Arg);
	}*/

    // 重置
    EndOperation();
}

void FArmyOffsetOperation::OperationLine()
{
	FVector2D MousePos;
	GVC->GetMousePosition(MousePos);
	FVector Mouse3D = GVC->PixelToWorld(MousePos.X, MousePos.Y, 0);
	Mouse3D.Z = 0;

    // 计算线段偏移的方向
    FVector Projection = FArmyMath::GetProjectionPoint(Mouse3D, SelectedLine->GetStart(), SelectedLine->GetEnd());
    FVector Direction = FArmyMath::GetLineDirection(Projection, Mouse3D);

    // 确保鼠标到线段的投影点位置不超出线段两端
    if (!FArmyMath::IsPointOnLineSegment2D(FVector2D(Projection), FVector2D(SelectedLine->GetStart()), FVector2D(SelectedLine->GetEnd()), 0.01f))
    {
        float StartDistance = FVector::Distance(SelectedLine->GetStart(), Projection);
        float EndDistance = FVector::Distance(SelectedLine->GetEnd(), Projection);
        if (StartDistance < EndDistance)
        {
            Projection = SelectedLine->GetStart();
        }
        else
        {
            Projection = SelectedLine->GetEnd();
        }
    }
	FVector SolidStart = SelectedLine->GetStart() + Direction * OffsetValue;
	FVector SolidEnd = SelectedLine->GetEnd() + Direction * OffsetValue;

	//判断偏移后的点是否在room内，如果在取反方向
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
	FVector CurrentPoint = (SolidStart + SolidEnd) / 2.f;
	for (auto It : RoomList)
	{
		TArray< TSharedPtr<FArmyLine> > Lines;
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		Room->GetLines(Lines);
		bool isBreak = false;
		for (auto ItLine : Lines)
		{
			if (ItLine == SelectedLine)
			{
				if (FArmyMath::IsPointInOrOnPolygon2D(CurrentPoint, Room->GetWorldPoints(true)))
				{
					Direction *= -1;
					SolidStart = SelectedLine->GetStart() + Direction * OffsetValue;
					SolidEnd = SelectedLine->GetEnd() + Direction * OffsetValue;
					isBreak = true;
					break;
				}
			}
		}
		if (isBreak)
			break;
	}

    // 设置线段偏移后的示意位置
    SolidLine->SetStart(SolidStart);
    SolidLine->SetEnd(SolidEnd);
    DashLine->SetStart(Projection);
    DashLine->SetEnd(Projection + Direction * OffsetValue);
    UpdataInputBoxPosition(DashLine->GetStart(), DashLine->GetEnd());
}

void FArmyOffsetOperation::OperationArcWall()
{
    // 计算线段偏移的方向
	/*FVector Horizontal = (SelectedArcWall->RightStaticPoint->Point->Pos - SelectedArcWall->LeftStaticPoint->Point->Pos).GetSafeNormal();
	FVector Vertical = Horizontal.RotateAngleAxis(-90, FVector(0, 0, 1));
	FVector Projection = FArmyMath::GetProjectionPoint(LocalManager.Pin()->GetMousePos(), FirstSelectedArcWallPoint, FirstSelectedArcWallPoint + Horizontal * 50.0f);
    FVector Direction = (LocalManager.Pin()->GetMousePos() - Projection).GetSafeNormal();

	float Angle = SelectedArcWall->GetArcAngle();
	FVector BasePoint = SelectedArcWall->GetCenterPosition();
	FVector MiddlePoint = SelectedArcWall->ArcMiddlePoint->Point->Pos + OffsetValue * Direction;
	FVector EndPoint = FRotator(0, Angle / 2, 0).RotateVector(MiddlePoint - BasePoint) + BasePoint;
	FVector StartPoint = FRotator(0, -Angle / 2, 0).RotateVector(MiddlePoint - BasePoint) + BasePoint;
	DashLine->Start = SelectedArcWall->ArcMiddlePoint->Point->Pos;
	DashLine->End = MiddlePoint;

	UpdataInputBoxPosition(DashLine->Start, DashLine->End);
	HelpArcWall->SetArcWallThreePoint(StartPoint, EndPoint, MiddlePoint);*/
}

void FArmyOffsetOperation::OnInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
        if (IsOperating())
        {
            // 偏移固定距离复制
            const int32 Distance = FCString::Atoi(*InText.ToString());
            if (Distance > 0)
            {
				// 记录偏移值
				OffsetValue = Distance / 10.f;

            	FVector Direction = FArmyMath::GetLineDirection(DashLine->GetStart(), DashLine->GetEnd());
            	FVector Offset = (Distance / 10.f) * Direction;
            	/*if (SelectedArcWall.IsValid())
            	{
            		FVector first = SelectedArcWall->GetCenterPosition();
            		FVector second = SelectedArcWall->ArcMiddlePoint->Point->Pos;
            		FVector helpArcPoint = HelpArcWall->ArcMiddlePoint->Point->Pos;
            		Offset = (Distance / 10.0f) * (SelectedArcWall->ArcMiddlePoint->Point->Pos - SelectedArcWall->GetCenterPosition()).GetSafeNormal();
            		if (FVector::DotProduct(helpArcPoint - first, helpArcPoint - second) < 0)
            		{
            			Offset *= -1.0f;
            		}
            	}*/

            	Exec_ApplyOffset(Offset);
            }
        }
        else
        {
            const int32 InputValue = FCString::Atoi(*InText.ToString());
            if (InputValue > 0)
            {
                // 记录偏移值
                OffsetValue = InputValue / 10.f;
                bOffsetSetted = true;

                InputBoxWidget->Show(false);

                GGI->Window->ShowMessage(MT_Normal, TEXT("请选择要偏移的内墙线"));//没有选择线段时会有的提示
            } 
        }

		bClickedEnter = true;
	}
}

void FArmyOffsetOperation::OnInputBoxKeyDown(const FKeyEvent& InKeyEvent)
{
	//GGI->DesignEditor->OnInputBoxKeyDown(InKeyEvent);
}

bool FArmyOffsetOperation::IsOperating()
{
	//return (SelectedWall.IsValid() || SelectedArcWall.IsValid());
	return SelectedLine.IsValid();
}

void FArmyOffsetOperation::SetInputBoxPos(const FVector& InPos)
{
    FVector2D TempViewportSize;
    GVC->GetViewportSize(TempViewportSize);
    FVector2D InputBoxPos;
    GVC->WorldToPixel(InPos, InputBoxPos);
    InputBoxPos -= TempViewportSize / 2;
    InputBoxWidget->SetPos(InputBoxPos);
}

void FArmyOffsetOperation::UpdataInputBoxPosition(const FVector& InStartPos, const FVector& InEndPos)
{
	bool bShowInputBox = InputBoxWidget->SetStartAndEnd(InStartPos, InEndPos);
	if (bShowInputBox)
	{
		InputBoxWidget->SetFocus(true);

        // 设置输入框位置
		FVector LineCenter = (InStartPos + InEndPos) / 2;
        SetInputBoxPos(LineCenter);
	}
}
