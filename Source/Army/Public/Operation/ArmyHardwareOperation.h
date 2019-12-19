#pragma once

#include "ArmySceneData.h"
#include "ArmyOperation.h"
#include "ArmyRoom.h"

class FArmyHardware;

class FArmyHardwareOperation : public FArmyOperation
{
public:
    FArmyHardwareOperation(EModelType InBelongModel);

    //~ Begin FArmyController Interface
	virtual void InitWidget(TSharedPtr<SOverlay> InParentWidget = NULL) {}
    virtual void BeginOperation(XRArgument InArg = XRArgument()) override;
    virtual void EndOperation() override;
    virtual void SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPoint) override;
    virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
    virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;
    //~ End FArmyController Interface

	/**	鼠标移动*/
	virtual void MouseMove(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y) override;
	virtual void MouseDrag(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, FKey Key) override;
    TSharedPtr<FArmyHardware> Exec_AddHardware(TSharedPtr<FArmyHardware> ResultHardware);
    void Exec_ModifyHardware(TSharedPtr<FArmyHardware> ModifyingHardware, TSharedPtr<FArmyHardware> ResultHardware);

	/**@欧石楠 用于外部调用更新一次数据捕捉*/
	void ForceCaptureDoubleLine(TSharedPtr<FArmyHardware> TempHW, EModelType InModelType = E_HomeModel);

protected:
    /** 是否正在操作点 */
    bool IsOperating();

    /** @欧石楠 在判断当前操作门窗错误时，需要将门窗还原到操作之前的状态 */
    void ResumeHardware(TSharedPtr<FArmyHardware> InHardware);

    /** @欧石楠 获得当前模式 */
    EModelType GetCurrentModel();

private:
    template<class T>
    void ModifyHardware(TSharedPtr<FArmyHardware> ModifyingHardware, TSharedPtr<FArmyHardware> ResultHardware)
    {
        TSharedPtr<T> HardwareA = StaticCastSharedPtr<T>(ModifyingHardware);
        TSharedPtr<T> TheHardware = StaticCastSharedPtr<T>(ResultHardware);
        TSharedPtr<T> HardwareB = MakeShareable(new T(TheHardware.Get()));
        //COMMAND_MODIFY(LocalManager.Pin(), T, HardwareA, HardwareB);
        HardwareA->SetState(OS_Selected);
    }

protected:
	struct FDoubleWallCaptureInfo CaptureWallInfo;
	float HardwareWidth = 100;
	bool CaptureDoubleLine(const FVector& Pos);
	void CaculateHardwareDirectionAndPos(const FVector& Pos);

    /** 
     * 墙角捕捉信息
     */
    struct FWallCornerCaputureInfo
    {
        FWallCornerCaputureInfo() :
            LeftWallDirection(FVector(-1, 0, 0)),
            RightWallDirection(FVector(0, 1, 0)),
            LeftWallWidth(24),
            RightWallWidth(24),
            Pos(FVector::ZeroVector),
            FirstLine(nullptr),
            SecondLine(nullptr)
        {}

        FVector LeftWallDirection;
        FVector RightWallDirection;
        FVector Pos;
        float LeftWallWidth;
        float RightWallWidth;
        FVector LeftWallDimPos;
        FVector RightWallDimPos;

        TSharedPtr<FArmyLine> FirstLine;
        TSharedPtr<FArmyLine> SecondLine;
    };

    struct FWallCornerCaputureInfo CaptureWallCornerInfo;
    bool CaptureWallCorner(const FVector& Pos, FWallCornerCaputureInfo& OutInfo);
    bool FindIntersectionPoint(TSharedPtr<FArmyLine> Wall1, TSharedPtr<FArmyLine> Wall2, TSharedPtr<FArmyLine> Wall3, FVector& Pos);
    FVector FindWallSidePoint(TSharedPtr<FArmyLine> Wall1, TSharedPtr<FArmyLine> Wall2, FVector& Pos);
    void SetWallCornerProperty(const FVector& Pos);

	TSharedPtr<FArmyRoom> GetRelatedRoomByLine(TSharedPtr<FArmyLine> CheckedLine, EModelType InModelType = E_HomeModel);

    void SetHardwareLinkedInfo(TSharedPtr<FArmyHardware> Hardware);

private:
    TSharedPtr<FArmyHardware> TempHardware;
    TSharedPtr<FArmyHardware> SelectedHardware;

    TSharedPtr<FArmyPrimitive> OperationPoint;

	EObjectType HardwareType;

	bool bShouldAdd = false;

	FVector DragPrePos;
	FVector2D MousePreDragPos;

	//记录修改前操作点位置
	FVector PrePos;
	FVector PreStartPos;
	FVector PreEndPos;

	TArray<struct WallLineInfo> Lines;
};