#pragma once

#include "ArmyDesignModeController.h"
#include "ArmyHardModeDetail.h"
#include "SArmyHardModeContentBrowser.h"
#include "ArmyTools/Public/XRClipper.h"



class FArmyBaseSurfaceController : public FArmyDesignModeController
{
public:
	virtual void PostInit();

	virtual void ReplaceTextureOperation(TSharedPtr< FContentItemSpace::FContentItem> NewItem=nullptr);

	virtual bool InputKey(FViewport* Viewport, FKey Key, EInputEvent Event) override;

	virtual void ProcessClickInViewPortClient(FViewport* ViewPort, FKey Key, EInputEvent Event);

	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI);

	virtual void EndOperation() override;

	virtual void SetCurrentEditSurface(TSharedPtr<class FArmyRoomSpaceArea> InSurface);

	virtual void Tick(float DeltaSeconds) override;

	virtual TSharedPtr<SWidget> MakeCustomTitleBarWidget() override;

	virtual void BeginMode() override;

	virtual bool EndMode() override;

    virtual void Quit() override;

	FReply OnReturnViewAngleCommand();

	void OnRectAreaCommand();
	void OnCircleAreaCommand();
	void OnRegularPolygonAreaCommand();
	void OnFreePolygonAreaCommand();

	/** 设置属性面板的可见性 */
	void SetPropertyVisibility(EVisibility _Visibility);

	TSharedPtr<SWidget> CreateToolBarRightArea();

	TSharedPtr<SArmyHardModeContentBrowser> GetHardModeContentBrowser() { return LeftPanel; }
	FSimpleDelegate FreeViewModeCallback;

	virtual void CollectAllLinesAndPoints(TArray< TSharedPtr<FArmyLine> >& InOutLines, TArray< TSharedPtr<FArmyPoint> >& InOutPoints, TArray<TSharedPtr<FArmyCircle>>& InOutCircles) override;

	void OnSaveCommand();
	void OnDeleteCommand();
	void OnEmptyCommand();
	void OnUndoCommand();
	void OnRedoCommand();
	void OnReplaceCommand();
	void OnMaterialBrushCommand();

	void MouseMove(FViewport* ViewPort, int32 X, int32 Y)override;

	//检查当前编辑面及面上区域是否有被选中，true为存在选中区域，否则为false
	bool CheckHasSelectedArea();

	TSharedPtr<FArmyBaseArea> GetSelectedArea() { return selected; };

	class AXRShapeActor* HightLightActor;
	
	//郭子阳
	//移动一个绘制区域
	//@Ret 是否移动成功
	bool MoveDrawArea(TSharedPtr<FArmyObject> InSelected, FTransform trans, int32 dir);


	bool TestIntersectoin(TSharedPtr<FArmyObject> InSelected, FTransform trans, int32 dir);

	TArray<FVector> TestInterLines;

protected:
	void SetModifyAreaOperation(FViewport* ViewPort, FKey Key, EInputEvent Event);
	void StartTrackingDueToInput(const FArmyInputEventState& InPutState);
	void StopTracking();
	bool IsTracking = false;
	TArray<TSharedPtr<FArmyBaseArea>>	PickOperation(const FVector& pos, class UArmyEditorViewportClient* InViewportClient);
	virtual bool ShouldTick();
	FVector RayIntersectionWithCurrentSurface(const FVector2D& InPos);

    void TransToCurrentPlane(FVector& InPos);
    void TransPlaneToWorld(FVector& InPos);

    /** @欧石楠 起铺点是否被选中 */
    bool IsLayingPointSelected();

    /** @欧石楠 起铺点捕捉位置 */
    bool CapturePoint(FVector2D& OutCapturedPos);

    /** @欧石楠 计算墙面、地面、顶面的造型在彼此的投影区域 */
    void CalculateWallProjectionAreas();
    void CalculateFloorProjectionAreas();
    void CalculateRoofProjectionAreas();

private:
	
	//进入模式时是否使用赋材质操作
	bool StartWithReplaceOperation = false; 


private:
	void SetCurrentOperation(const FVector2D& InPos, TSharedPtr<FArmyBaseArea> InSelectedArea);
	void ClearCurrentEditorSurfaceData();
	void CalculateCurrentEditSurfaceOutLineInfo();

private:
	TArray<FVector> RoofFallOffRegionVerts;
	TArray<FDynamicMeshVertex> PillarBeamAddWallVerts;
	void DrawRoofFallOffRegion(const FSceneView* View, FPrimitiveDrawInterface* PDI);
	void DrawWallAttachPillarOrBeamOrAddWall(const FSceneView* View, FPrimitiveDrawInterface* PDI);
	TArray<TSharedPtr<AreaCompare>>  CombinePaths(Paths InPath);
	float FindWallWidth(TArray<TSharedPtr<FArmyHardware>>& RoomWallSurfaces, const FVector& start, const FVector& end);
	TArray< TSharedPtr<class FArmyHardware> > CurrentSufaceHardWare;
	TArray<TWeakPtr<FArmyObject>> CurrentSurfaceSimpleComponent;
	void TriangleArea(const TArray<FVector>& OutAreas, const TArray<TArray<FVector>>& Holes);
	virtual void DrawSolidWall(FPrimitiveDrawInterface* PDI, const FSceneView* View);
	TArray<FDynamicMeshVertex> SolidWallArea;
	TArray<FDynamicMeshVertex> AddWallArea;
	TArray<FArmyRoomSpaceArea::SurfaceHoleInfo> SurfaceHolesInfos;

protected:
	FVector2D ViewportSize;
	FVector2D LastViewportMousePos;
	FVector2D ViewportMousePos;
	FVector WorldStartPos;
	FVector WorldEndPos;
	FVector2D LastMouseClick;
	FVector2D CurrentMouseClick;
	TSharedPtr<FArmyBaseArea> selected;
	TSharedPtr<FArmyBaseArea> selectedOld;
	TSharedPtr<class FArmyRoomSpaceArea> CurrentEditSurface;

	TSharedPtr<class FArmyAxisRuler> AxisRuler;
	TSharedPtr<class SArmyHardModeContentBrowser> LeftPanel;

	//属性面板
	TSharedPtr<class FArmyHardModeDetail> SufaceDetail;

	//墙顶地返回硬装按钮
	TSharedPtr<class SWidget> ReturnWidget;


	FCanExecuteAction CheckHasSelectedAreaObj;

	bool bHoverRectEdge = false;
	//@郭子阳
	//被Hover的面，目前只支持rectArea
	TWeakPtr<FArmyBaseArea> HoveredArea;

    /** @欧石楠 起铺点 */
    TSharedPtr<class FArmyLayingPoint> LayingPoint;

    /** @欧石楠 是否在移动起铺点 */
    bool bMovingLayingPoint = false;

	FVector HoverEdgeStart = FVector::ZeroVector;
	FVector HoverEdgeEnd = FVector::ZeroVector;
	int32 AddAreaState = 0;

	TArray<FVector> InnearSurfaceBoundary;
	TArray<FVector> OutSurfarceBoundary;

	TArray<TArray<FVector>> AddWallBoundary;

	//瀑布流目录管理
	TSharedPtr<class SArmyCategoryOrganizationManage> CategoryOrgManageSurface;
};