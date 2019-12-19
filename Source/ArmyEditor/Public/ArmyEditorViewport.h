#pragma once

#include "CoreMinimal.h"
#include "SViewport.h"
#include "ArmyEditorViewportClient.h"

class ARMYEDITOR_API SArmyEditorViewport : public SViewport
{
public:

	SLATE_BEGIN_ARGS(SArmyEditorViewport) 
		: _ViewportClient(NULL)
		, _RenderDirectlyToWindow(false)
		, _EnableGammaCorrection(false)
		, _EnableStereoRendering(false)
	{}
	SLATE_ARGUMENT(UArmyEditorViewportClient*, ViewportClient)
	SLATE_ARGUMENT(bool, RenderDirectlyToWindow)
	SLATE_ARGUMENT(bool, EnableGammaCorrection)
	SLATE_ARGUMENT(bool, EnableStereoRendering)
	SLATE_END_ARGS()

	SArmyEditorViewport();
	virtual ~SArmyEditorViewport();

	void Construct(const FArguments& InArgs);

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	virtual UArmyEditorViewportClient* MakeEditorViewportClient() { return NULL; }

	bool IsRealtime() const;

	virtual bool IsVisible() const;

	void SetParentTab(TSharedRef<SDockTab> InParentTab) { ParentTab = InParentTab; }

	void OnToggleRealtime();

	const TSharedPtr<class FUICommandList> GetCommandList() const { return CommandList; }

	UArmyEditorViewportClient* GetViewportClient() const { return Client; }

protected:
	virtual void BindCommands();
	virtual void OnCycleTransformGizmos();
	virtual void OnFocusViewportToSelection();

	virtual UWorld* GetWorld() const;

protected:

	TSharedPtr<FUICommandList> CommandList;

	UArmyEditorViewportClient* Client;

	/** Viewport that renders the scene provided by the viewport client */
	TSharedPtr<FSceneViewport> SceneViewport;

	double LastTickTime;

	/** Widget where the scene viewport is drawn in */
	//TSharedPtr<SViewport> ViewportWidget;

private:
	TWeakPtr<class SDockTab> ParentTab;

};