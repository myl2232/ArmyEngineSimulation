#include "ArmyDesignModeController.h"
#include "ArmyViewportClient.h"
#include "SArmyDesignFrame.h"
#include "ArmyGlobalActionCallBack.h"

void FArmyDesignModeController::Init()
{
    /**@欧石楠使全局菜单里的快捷键生效*/
    CommandList = FArmyGlobalActionCallBack::Get().GetGlobalCommandsList();

    FArmyViewController::Init();

    InitOperations();
}

bool FArmyDesignModeController::EndMode()
{
    IsCurrentModel = false;
    EndOperation();
    
    return true;
}

void FArmyDesignModeController::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	FArmyToolsModule::Get().GetRectSelectTool()->Draw(PDI, View);

    if (CurrentOperation.IsValid())
    {
        CurrentOperation->Draw(PDI, View);
    }

}

void FArmyDesignModeController::Draw(FViewport* InViewport, const FSceneView* View, FCanvas* SceneCanvas)
{
	FArmyToolsModule::Get().GetRectSelectTool()->Draw(GVC, InViewport, SceneCanvas);

	if (FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture)
	{
		FArmyToolsModule::Get().GetMouseCaptureTool()->Draw(GVC, InViewport, SceneCanvas);
	}
	if (CurrentOperation.IsValid())
	{
		CurrentOperation->DrawHUD(GVC, InViewport, View, SceneCanvas);
	}


}

bool FArmyDesignModeController::InputKey(FViewport* Viewport, FKey Key, EInputEvent Event)
{
    if (CurrentOperation.IsValid() && CurrentOperation->InputKey(GVC, Viewport, Key, Event))
    {
		return true;
		//如果Operation返回false证明右键点击可以结束当前的Operation
    }
	else if (Event == IE_Pressed && (Key == EKeys::RightMouseButton || Key == EKeys::Escape))
	{
		SetOperation(-1);
		FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	}

    return false;
}

void FArmyDesignModeController::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
    if (CurrentOperation.IsValid())
    {
        CurrentOperation->ProcessClick(View, HitProxy, Key, Event, HitX, HitY);
    }

}

void FArmyDesignModeController::Tick(float DeltaSeconds)
{
    if (CurrentOperation.IsValid())
    {
        CurrentOperation->Tick();
    }

}

void FArmyDesignModeController::MouseMove(FViewport* Viewport, int32 X, int32 Y)
{
	if (FArmyToolsModule::Get().GetRectSelectTool()->GetState() > 0)
	{
		FArmyToolsModule::Get().GetRectSelectTool()->MouseMove(GVC, X, Y,AllCanHoverObjects);
	}
	if (FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture)
	{
		TArray<TSharedPtr<FArmyLine>> Lines;
		TArray<TSharedPtr<FArmyCircle>> Circles;
		TArray<TSharedPtr<FArmyPoint>> Points;
		CollectAllLinesAndPoints(Lines, Points, Circles);

		FArmyToolsModule::Get().GetMouseCaptureTool()->Capture(GVC, X, Y, Lines, Points);
	}

    if (CurrentOperation.IsValid())
    {
        CurrentOperation->MouseMove(GVC, Viewport, X, Y);
    }

}

bool FArmyDesignModeController::MouseDrag(FViewport* Viewport, FKey Key)
{
	if (CurrentOperation.IsValid())
	{
		CurrentOperation->MouseDrag(GVC, Viewport, Key);
	}
	else if (FArmyToolsModule::Get().GetRectSelectTool()->GetState() > 0)
		return FArmyToolsModule::Get().GetRectSelectTool()->MouseDrag(Viewport, Key);

	return false;
}

void FArmyDesignModeController::SetOperation(uint8 Operation, XRArgument InArg)
{
    // 结束上一个操作
    if (CurrentOperation.IsValid())
	{
		FormerOperation = CurrentOperation;
		CurrentOperation->EndOperation();
    }
     
    CurrentOperation = OperationMap.FindRef(Operation);

	if (CurrentOperation.IsValid())
	{
		CurrentOperation->BeginOperation(InArg);
	}
	else
	{
		EndOperation();
	}
}

void FArmyDesignModeController::EndOperation()
{
	CurrentOperation = NULL;

	FArmyToolsModule::Get().GetMouseCaptureTool()->Reset();
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = false;
}

const TSharedPtr<FArmyOperation> FArmyDesignModeController::GetCurrentOperation() const
{
	return CurrentOperation;
}

void FArmyDesignModeController::HideDesignPanel(bool bHidden)
{
    DesignFrame->HideDesignPanel(bHidden);
}

uint32 FArmyDesignModeController::GetCurrentOperationType() const
{
	for (auto& It : OperationMap)
	{
		if (CurrentOperation == It.Value)
			return It.Key;
	}

	return -1;
}

bool FArmyDesignModeController::ShouldTick()
{
    GVC->GetViewportSize(ViewportSize);
    GVC->GetMousePosition(ViewportMousePos);
    if (ViewportSize.X == 0 || ViewportSize.Y == 0)
    {
        return false;
    }

    if (LastViewportMousePos == ViewportMousePos || ViewportMousePos < -ViewportSize)
    {
        return false;
    }

    LastViewportMousePos = ViewportMousePos;

    return true;
}

TSharedPtr<SWidget> FArmyDesignModeController::MakeContentWidget()
{
    SAssignNew(DesignFrame, SArmyDesignFrame);

    SetPanelContent(DesignFrame->LeftPanel, MakeLeftPanelWidget());
    SetPanelContent(DesignFrame->RightPanel, MakeRightPanelWidget());
    SetPanelContent(DesignFrame->ToolBar, MakeToolBarWidget());
    SetPanelContent(DesignFrame->SettingBar, MakeSettingBarWidget());

    return DesignFrame;
}

void FArmyDesignModeController::SetPanelContent(TSharedPtr<SBox> Panel, TSharedPtr<SWidget> Content)
{
    if (Content.IsValid())
    {
        Panel->SetVisibility(EVisibility::Visible);
        Panel->SetContent(Content.ToSharedRef());
    }
    else
    {
        Panel->SetVisibility(EVisibility::Collapsed);
    }
}