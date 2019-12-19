#pragma once
/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRBearingWall.h
 * @Description 承重墙
 *
 * @Author 
 * @Date 2018年11月5日
 * @Version 1.0
 */
#include "ArmyObject.h"

class ARMYSCENEDATA_API FArmyBearingWall :public FArmyObject
{
public:
	/**
	* 双墙墙体捕捉信息
	*/
	struct FDoubleWallCaptureInfo
	{
		FDoubleWallCaptureInfo()
			: FirstLine(NULL)
			, SecondLine(NULL)
			, MinDistanceWall(NULL)
			, Pos(ForceInitToZero)
			, Direction(ForceInitToZero)
			, Thickness(24)
		{}

		TSharedPtr<FArmyLine> FirstLine;//捕捉的第一条线
		TSharedPtr<FArmyLine> SecondLine;//捕捉的第二条线
		TSharedPtr<FArmyLine> MinDistanceWall;//距离鼠标点位最短的捕捉线
		FVector Pos;//第一第二捕捉线的中点
		FVector Direction;//控件在墙中的方向
		float Thickness;//墙体厚度
	};

	FArmyBearingWall();
	~FArmyBearingWall() {}

	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	// FObject2D Interface Begin
	virtual void SetState(EObjectState InState) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	//判断点是否在该控件几何范围内
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	//得到该控件的所有顶点
	virtual void GetVertexes(TArray<FVector>& OutVertexes) override;
	//获取该控件的所有线
	virtual void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false) override;
	//获取包围盒
	virtual const FBox GetBounds();
	virtual void Delete() override;

	/**
	* @ 判断点是否在此对象内
	* @param Pos - const FVector & - 输入要检测的点
	* @return bool - true 表示在面内，否则不在
	*/
	virtual bool IsPointInObj(const FVector & Pos);

	/**	设置属性标识*/
	virtual void SetPropertyFlag(PropertyType InType, bool InUse) override;

	// FObject2D Interface End
	//设置承重墙材质
	void SetMaterial(UMaterial* InMaterial);
	//设置和更新承重墙顶点
	void SetVertexes(TArray<FVector>& Vertexes);

	//设置控件的起止点并更新双墙捕捉信息
	void SetOperationLinePos(const FVector& InStartPos, const FVector& InEndPos);
	//更新标尺线信息
	void UpdateDashLine();

	void SetLength(float InValue);
	float GetLength() { return FVector::Distance(OperationLineStartPos, OperationLineEndPos); }

	//获取控件方向
	FVector GetDir() const { return (OperationLineEndPos - OperationLineStartPos).GetSafeNormal(); }

	//获取控件的边框线
	const TArray<TSharedPtr<FArmyLine>>& GetBorderLines() const { return BorderLines; }
	//获取双墙捕捉信息
	const FDoubleWallCaptureInfo& GetWallCaptureInfo() const { return CaptureWallInfo; }

	//获取操作线的起止点在墙上的中线点位置
	FVector & GetStartPos() { return OperationLineStartPosCenter; }
	FVector & GetEndPos() { return OperationLineEndPosCenter; };

	/**
	* 检查新建或者修改是否合法。合法指：是否相交、是否包含等不符合承重墙绘制范围的操作
	* @return bool - Trur表示可以添加或者修改，否则操作无效
	*/
	bool CheckAddOrModifyLegal();

	/**
	 * 计算承重墙是否在外墙上，只要承重墙的起止点有一个点在外墙上，视该承重墙在外墙上
	 * @return bool - true表示在外墙上，否者不在
	 */
	bool CalBearingInOutwall();
public:
	TSharedPtr<class FArmyPolygon> Polygon;

private:
	//获取控件包围盒顶点信息
	TArray<FVector> GetClipingBox();
	float RulerOffsetDis;
	/**
	 * 根据输入点来计算距离该点最近的捕捉线
	 * @param Pos - const FVector & - 输入点
	 * @param OutInfo - FDoubleWallCaptureInfo & - 返回的双线捕捉信息
	 * @return bool - 捕捉是否成功
	 */
	bool CaptureDoubleLine(const FVector& Pos, FDoubleWallCaptureInfo& OutInfo);

	TSharedPtr<class FArmyRoom> FirstRelatedRoom;
	TSharedPtr<class FArmyRoom> SecondRelatedRoom;

	TSharedPtr<class FArmyRulerLine> HardWareRulerLine;
	virtual void OnHardWareInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	TSharedPtr<class FArmyRulerLine> FirstLineLeftRulerLine;
	void OnFirstLineLInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	TSharedPtr<class FArmyRulerLine> FirstLineRightRulerLine;
	void OnFirstLineRInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	TSharedPtr<class FArmyRulerLine> SecondLineLeftRulerLine;
	void OnSecondLineLInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	TSharedPtr<class FArmyRulerLine> SecondLineRightRulerLine;
	void OnSecondLineRInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	/**
	 * 更新操作线的两个端点在双线的中心位置
	 * @param OperationStart - const FVector & - 操作线起点，此点在某一条墙线上
	 * @param OperationEnd - const FVector & - 操作线终点，此点与起点在同一条墙线上
	 * @param WallInfo - const FDoubleWallCaptureInfo & - 双线捕捉信息
	 * @return bool - true表示更新成功，否则失败
	 */
	bool UpdateOperationCenterPos(const FVector & OperationStart, const FVector & OperationEnd,const FDoubleWallCaptureInfo & WallInfo);

	struct FDoubleWallCaptureInfo CaptureWallInfo;

protected:
	TArray<TSharedPtr<FArmyLine>> BorderLines;
	UMaterialInstanceDynamic* MI_Line;
	FLinearColor Color;
	//边框线颜色
	FLinearColor OutLineColor;
	int32 CurrentWallType = 0; // 0  拆改墙，1 承重墙
	TArray<TWeakPtr<FArmyLine>> AllLines;
	bool bUpdateDashLine = false;

	//此处两个点关联的边线上
	FVector OperationLineStartPos;
	FVector OperationLineEndPos;

	//此处两个点在承重墙中线点方向的端点,不在关联的边线上
	FVector OperationLineStartPosCenter;
	FVector OperationLineEndPosCenter;

	float Height;
	float FloorHeight = 0;
};

REGISTERCLASS(FArmyBearingWall)