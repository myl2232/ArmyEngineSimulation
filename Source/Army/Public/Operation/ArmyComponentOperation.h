#pragma once

#include "ArmyOperation.h"
#include "ArmyFurniture.h"

class  FArmyComponentOperation : public FArmyOperation
{
public:
	FArmyComponentOperation(EModelType InBelongModel);
	~FArmyComponentOperation();

	/** 绘制 */
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	/** 重置 */
	virtual void Reset();

	/**	鼠标移动*/
	virtual void MouseMove(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y) override;
	virtual void MouseDrag(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, FKey Key) override;

	/** 输入事件 */
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event);

	virtual void BeginOperation(XRArgument InArg = XRArgument());

	virtual void EndOperation();

	virtual void SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive);
	void ReadFromFile(const FString& InFilePath,uint32 InObjType);

	TSharedPtr<FArmyFurniture> GetSelectedComponent() { return SelectedComponent; }

	//@ 四周标尺显隐控制代理，true显示，否则不显示
	FArmyObjectDelegate ShowAxisRulerDelegate;
private:
	/**
	 * 捕捉距离墙最近的点
	 * @param InputPos - FVector - 输入点
	 * @param OutCapturePos - FVector & - 捕捉的点
	 * @param OutLine - TSharedPtr<FArmyLine> & - 捕捉的线
	 * @param OutLine - TSharedPtr<FArmyRoom> & - 线所在的房间
	 * @return bool - 捕捉是否成功
	 */
	bool CaptureClosestPoint(FVector InputPos, FVector & OutCapturePos, TSharedPtr<FArmyLine> & OutLine, TSharedPtr<FArmyRoom> & OutRoom);
	/**
	 * 计算当前点是否合法。合法指：空间点在空间内为合法，否则不合法，墙线点位在墙线上为合法，否则不合法
	 * @param CurrentPos - const FVector & - 要检测的点
	 * @return bool - true表示合法，否则不合法
	 */
	bool CalPositionIllegal(const FVector & CurrentPos );

	/**
	 * 点位自适应标注在房间内
	 * @param WorldPos - const FVector & - 当前捕捉的位置
	 * @param CaptureLine - const TSharedPtr<FArmyLine> & -当前捕捉点所在的线 
	 * @param CaptureRoom - const TSharedPtr<FArmyRoom> & - 当前捕捉点所在的面
	 * @return bool - true表示匹配成功，否则不成功
	 */
	bool PositionSelfAdaption(const FVector & WorldPos,const TSharedPtr<FArmyLine> & CaptureLine, const TSharedPtr<FArmyRoom>& CaptureRoom);
private:
	XRArgument	OperationArg;
	bool IsModifying = false;
	bool IsModified = false;
	TSharedPtr<FArmyFurniture> SelectedComponent = NULL;
	TSharedPtr<FArmyEditPoint> SelectOperationPoint = NULL;
	bool IsSelectCaptruePoint = false;
	bool bCreatFurniture =false;

	//墙线点位离开墙体后最后捕捉的墙体位置
	FVector LastPos;
};