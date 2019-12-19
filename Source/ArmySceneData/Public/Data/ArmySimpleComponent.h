/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRSimpleComponent.h
 * @Description 简单构件
 *
 * @Author 欧石楠
 * @Date 2018年10月11日
 * @Version 1.0
 */
#pragma once

#include "ArmyObject.h"

enum ESCState {
	ESCS_NONE,
	ESCS_TOP,
	ESCS_BOTTOM,
	ESCS_LEFT,
	ESCS_RIGHT,
	ESCS_CENTER,
	ESCS_DIR
};

class ARMYSCENEDATA_API FArmySimpleComponent : public FArmyObject
{
	
public:
	FArmySimpleComponent();
	FArmySimpleComponent(FArmySimpleComponent* Copy);
	virtual ~FArmySimpleComponent();	

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	virtual void SetState(EObjectState InState) override;
	//~ Begin FObject2D Interface
	//virtual void SetState(EObjectState InState) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual TSharedPtr<FArmyEditPoint> SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual TSharedPtr<FArmyEditPoint> HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	//virtual bool Hover(const FVector& Pos) override;
	//virtual void ApplyTransform(const FTransform& Trans) override;

	virtual void Delete() override;
	/** 获取包围盒*/
	virtual const FBox GetBounds() override;
	virtual void GetCadLineArray(TArray<TPair<FVector, FVector>>& OutLineList) const override;
	virtual void GetVertexes(TArray<FVector>& OutVertexes) override;
	void Get4Vertexes(TArray<FVector>& OutVertexes);
	virtual void Generate(UWorld* InWorld) override;
	//~ End FObject2D Interface

	//判断是否吸附了墙线
	bool IsAttachedToRoom();

	virtual void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false) override;

	/**	设置属性标识*/
	virtual void SetPropertyFlag(PropertyType InType, bool InUse) override;

	/** 更新2D显示 */
	virtual void Update();

	/*@欧石楠 拖拽top point时单独更新Top point**/
	virtual void UpdateTopPoint(FVector Pos);

	/*@欧石楠 拖拽bottom point时单独更新Bottom point**/
	virtual void UpdateBottomPoint(FVector Pos);

	/*@欧石楠 拖拽left point时单独更新left point**/
	virtual void UpdateLeftPoint(FVector Pos);

	/*@欧石楠 拖拽right point时单独更新right point**/
	virtual void UpdateRightPoint(FVector Pos);

	/** @欧石楠 当被清空时的调用，与上面的删除不一致*/
	virtual void OnDelete() override;	

	/**@欧石楠 自适应*/
	virtual void AutoAdapt();

	/**@欧石楠 自吸附*/
	virtual void AutoAttach(float Invalue = 0);

	const FVector& GetLeftPos();
	const FVector& GetRightPos();
	const FVector& GetTopPos();
	const FVector& GetBottomPos();
	const FVector& GetLeftTopPos();
	const FVector& GetLeftBottomPos();
	const FVector& GetRightTopPos();
	const FVector& GetRightBottomPos();
	const FVector& GetPos();

	FORCEINLINE FVector GetDirection() { return Direction; }
	FORCEINLINE float   GetLength() { return Length; }
	FORCEINLINE float   GetWidth() { return Width; }
	FORCEINLINE float   GetHeight() { return Height; }	
	FORCEINLINE float   GetAngle() { return Angle; }


	void SetLeftPos(const FVector& InPos);
	void SetRightPos(const FVector& InPos);
	void SetTopPos(const FVector& InPos);
	void SetBottomPos(const FVector& InPos);
	void SetLeftTopPos(const FVector& InPos);
	void SetLeftBottomPos(const FVector& InPos);
	void SetRightTopPos(const FVector& InPos);
	void SetRightBottomPos(const FVector& InPos);
	void SetPos(const FVector& InPos);
	void SetDirection(const FVector& InDirection);
	void SetLength(const float& InLength);
	void SetWidth(const float& InWidth);
	void SetHeight(const float& InHeight);	
	void SetAngle(const float& InAngle);

	ESCState GetOperationPointType() const;

	void SetOperationPointType(ESCState Type);
	
	/**@欧石楠 查找相关联的房间*/
	void CalcRelatedRoom();

	/**@欧石楠 设置相关联的房间*/
	void SetRelatedRoom(TWeakPtr<class FArmyRoom> InRoom) { RelatedRoom = InRoom; }

	TWeakPtr<class FArmyRoom> GetRelatedRoom() { return RelatedRoom; }

	bool GetAutoAdapt() { return bAutoAdapt; }

	virtual void SetAutoAdapt(bool bValue);

	bool GetAutoAdaptDir() { return bAutoAdaptDir; }

	void SetAutoAdaptDir(bool bValue);

	/** 取消选中操作点 */
	void DeselectPoints();

	virtual bool CheckAllPointsInRoom(FVector Pos);

	void HideRuleLineInputBox(bool bHide);

	/** @纪仁泽 获得立面投影box */
	TArray<struct FLinesInfo> GetFacadeBox(TSharedPtr<class FArmyLine> InLine);
protected:	
	/**@欧石楠 更新虚线标注*/
	void UpdateDashLine();
	/**@欧石楠 绘制标尺标注*/
	void DrawDashLine(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	/**@欧石楠 选中控件时的标注虚线框*/
	TSharedPtr<class FArmyRulerLine> TopRulerLine;
	virtual void OnTopInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	TSharedPtr<class FArmyRulerLine> BottomRulerLine;
	virtual void OnBottomInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	TSharedPtr<class FArmyRulerLine> LeftRulerLine;
	virtual void OnLeftInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	TSharedPtr<class FArmyRulerLine> RightRulerLine;
	virtual void OnRightInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);
public:
	/** 操作点 */
	TSharedPtr<FArmyEditPoint> LeftPoint;
	TSharedPtr<FArmyEditPoint> RightPoint;

	TSharedPtr<FArmyEditPoint> TopPoint;
	TSharedPtr<FArmyEditPoint> BottomPoint;
	TSharedPtr<FArmyEditPoint> Point;

	TSharedPtr<FArmyEditPoint> LeftTopPoint;
	TSharedPtr<FArmyEditPoint> LeftBottomPoint;

	TSharedPtr<FArmyEditPoint> RightTopPoint;
	TSharedPtr<FArmyEditPoint> RightBottomPoint;

	TSharedPtr<FArmyEditPoint> DirectionPoint;
	/*@欧石楠 选中的点**/
	TSharedPtr<FArmyEditPoint> ResultPoint;
	
	TSharedPtr<FArmyLine> TopLeftLine;	
	TSharedPtr<FArmyLine> TopRightLine;
	TSharedPtr<FArmyLine> BottomLeftLine;
	TSharedPtr<FArmyLine> BottomRightLine;
	
	TSharedPtr<FArmyLine> LeftTopLine;
	TSharedPtr<FArmyLine> LeftBottomLine;
	TSharedPtr<FArmyLine> RightTopLine;
	TSharedPtr<FArmyLine> RightBottomLine;

public:
	//其他模式绘制需要的数据	
	TSharedPtr<FArmyLine> OtherModeTopLeftLine;
	TSharedPtr<FArmyLine> OtherModeTopRightLine;
	TSharedPtr<FArmyLine> OtherModeBottomLeftLine;
	TSharedPtr<FArmyLine> OtherModeBottomRightLine;

	TSharedPtr<FArmyLine> OtherModeLeftTopLine;
	TSharedPtr<FArmyLine> OtherModeLeftBottomLine;
	TSharedPtr<FArmyLine> OtherModeRightTopLine;
	TSharedPtr<FArmyLine> OtherModeRightBottomLine;

	TSharedPtr<class FArmyPolygon> OtherModePolygon;
	UMaterialInstanceDynamic* MI_PolygonMat;

protected:
	/** 朝向 */
	FVector Direction;

	/** 从左到右的方向 */
	FVector HorizontalDirection;

	/** 长度 */
	float Length;

	/** 厚度 */
	float Width;

	/** 高度 */
	float Height;
	
	/** 自适应*/
	bool bAutoAdapt = true;

	/** 自适应方向*/
	bool bAutoAdaptDir = true;

	/** 总是显示四向标尺*/
	bool bAlwaysShowDashLine = false;

	/** 角度*/
	float Angle = 0;

	bool bIsCopy = false;

	/**@欧石楠 关联的房间*/
	TWeakPtr<class FArmyRoom> RelatedRoom;

	ESCState OperationPointType = ESCS_NONE;
	bool bShowEditPoint = true;

public:
	bool bModified = true;
};
REGISTERCLASS(FArmySimpleComponent)