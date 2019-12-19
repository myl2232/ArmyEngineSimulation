#include "ArmyEditorViewport.h"
#include "ArmyEditorViewportCommands.h"
#include "ArmyUnrealWidget.h"
#include "SceneViewport.h"

SArmyEditorViewport::SArmyEditorViewport()
	: LastTickTime(0)
{
	Client = NULL;
}

SArmyEditorViewport::~SArmyEditorViewport()
{
	if (Client && Client->IsValidLowLevel())
	{
		Client->Viewport = NULL;
		delete Client;
		Client = nullptr;
	}
}

void SArmyEditorViewport::Construct(const FArguments& InArgs)
{
	Client = InArgs._ViewportClient;

	SViewport::Construct(
		SViewport::FArguments()
		.RenderDirectlyToWindow(InArgs._RenderDirectlyToWindow)
		.EnableGammaCorrection(InArgs._EnableGammaCorrection)
		.EnableStereoRendering(InArgs._EnableStereoRendering)
	);

	if (Client)
	{
		Client->VisibilityDelegate.BindSP(this, &SArmyEditorViewport::IsVisible);
	}
	else
	{
		//UArmyLevelEditorViewportClient* LevelClient = Cast<UArmyLevelEditorViewportClient>(Client);
		//if (!LevelClient)
		//{
			//UArmyEditorViewportClient* ViewportClient = MakeEditorViewportClient();
			//ViewportClient->VisibilityDelegate.BindSP(this, &SArmyEditorViewport::IsVisible);

			//SceneViewport = MakeShareable(new FSceneViewport(ViewportClient, SharedThis(this)));
			//ViewportClient->Viewport = SceneViewport.Get();
			//SetViewportInterface(SceneViewport.ToSharedRef());
			//Client = ViewportClient;
		//}

        Client->VisibilityDelegate.BindSP(this, &SArmyEditorViewport::IsVisible);

		SceneViewport = MakeShareable(new FSceneViewport(Client, SharedThis(this)));
        Client->Viewport = SceneViewport.Get();
		SetViewportInterface(SceneViewport.ToSharedRef());
	}

	CommandList = MakeShareable(new FUICommandList);
	FArmyEditorViewportCommands::Register();
	BindCommands();
}

FReply SArmyEditorViewport::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (CommandList->ProcessCommandBindings(InKeyEvent))
	{
		Client->Invalidate();
		return FReply::Handled();
	}

	return SViewport::OnKeyDown(MyGeometry, InKeyEvent);
}

void SArmyEditorViewport::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	LastTickTime = FPlatformTime::Seconds();
}

bool SArmyEditorViewport::IsRealtime() const
{
	return Client->IsRealtime();
}

bool SArmyEditorViewport::IsVisible() const
{
    return true;
}

void SArmyEditorViewport::OnToggleRealtime()
{
	if (Client->IsRealtime())
	{
		Client->SetRealtime(false);
// 		if (ActiveTimerHandle.IsValid())
// 		{
// 			UnRegisterActiveTimer(ActiveTimerHandle.Pin().ToSharedRef());
// 		}
	}
	else
	{
		Client->SetRealtime(true);
		//ActiveTimerHandle = RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &SEditorViewport::EnsureTick));
	}
}

void SArmyEditorViewport::BindCommands()
{
	FUICommandList& CommandListRef = *CommandList;
	const FArmyEditorViewportCommands& Commands = FArmyEditorViewportCommands::Get();

	CommandListRef.MapAction(Commands.ToggleRealTime, FExecuteAction::CreateSP(this, &SArmyEditorViewport::OnToggleRealtime));
	CommandListRef.MapAction(Commands.TranslateMode, FExecuteAction::CreateUObject(Client, &UArmyEditorViewportClient::SetWidgetMode, FArmyWidget::WM_Translate));
	CommandListRef.MapAction(Commands.RotateMode, FExecuteAction::CreateUObject(Client, &UArmyEditorViewportClient::SetWidgetMode, FArmyWidget::WM_Rotate));
	CommandListRef.MapAction(Commands.ScaleMode, FExecuteAction::CreateUObject(Client, &UArmyEditorViewportClient::SetWidgetMode, FArmyWidget::WM_Scale));
	CommandListRef.MapAction(Commands.CycleTransformGizmos, FExecuteAction::CreateSP(this, &SArmyEditorViewport::OnCycleTransformGizmos));
	//CommandListRef.MapAction(Commands.FocusViewportToSelection, FExecuteAction::CreateSP(this, &SArmyEditorViewport::OnFocusViewportToSelection));
	CommandListRef.MapAction(Commands.Perspective, FExecuteAction::CreateUObject(Client, &UArmyEditorViewportClient::SetViewportType, EArmyLevelViewportType::LVT_Perspective));
	CommandListRef.MapAction(Commands.Top, FExecuteAction::CreateUObject(Client, &UArmyEditorViewportClient::SetViewportType, EArmyLevelViewportType::LVT_OrthoXY));
    CommandListRef.MapAction(Commands.Orbit, FExecuteAction::CreateUObject(Client, &UArmyEditorViewportClient::SetOrbitType));
}

void SArmyEditorViewport::OnCycleTransformGizmos()
{
	FArmyWidget::EWidgetMode WidgetMode = Client->GetWidgetMode();
	if (WidgetMode == FArmyWidget::WM_Translate)
	{
		WidgetMode = FArmyWidget::WM_Rotate;
	}
	else if (WidgetMode == FArmyWidget::WM_Rotate)
	{
		WidgetMode = FArmyWidget::WM_Scale;
	}
	else if (WidgetMode == FArmyWidget::WM_Scale)
	{
		WidgetMode = FArmyWidget::WM_Translate;
	}
	else
	{
		WidgetMode = FArmyWidget::WM_Translate;
	}
	Client->SetWidgetMode(WidgetMode);
}

void SArmyEditorViewport::OnFocusViewportToSelection()
{

}

UWorld* SArmyEditorViewport::GetWorld() const
{
	return Client->GetWorld();
}
