#pragma once

#include "ArmyObject.h"
#include "ArmyCommonTypes.h"

struct FArmyHardwareParam
{
	FVector Pos;
	FVector Direction;
	float Length;
	float Width;
	float Thickness;
	bool bRightOpen = true;
};

static float SubTracteThicknessError = 0.6;//单位：厘米

/**
 * 硬装构件
 */
class ARMYSCENEDATA_API FArmyHardware : public FArmyObject
{
public:
	FArmyHardware();

	friend class FArmyHardwareOperation;

	//~ Begin FObject2D Interface
	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual TSharedPtr<FArmyEditPoint> SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual TSharedPtr<FArmyEditPoint> HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual void GetVertexes(TArray<FVector>& OutVertexes) override;
	virtual void Delete() override;
	//~ End FObject2D Interface

	/**	设置属性标识*/
	virtual void SetPropertyFlag(PropertyType InType, bool InUse) override;

	// 获得绘制CAD的数据
	virtual void GetCadLineArray(TArray<TPair<FVector, FVector>>& OutLineList) const override;

	/** 更新2D显示 */
	virtual void Update();

	/*@欧石楠单独更新start point**/
	virtual void UpdateStartPoint();

	/*@欧石楠单独更新end point**/
	virtual void UpdateEndPoint();

	/** 设置属性,用于修改撤销 */
	//virtual void SetProperty(const FArmyHardwareParam& Param);

	/** @欧石楠 当被清空时的调用，与上面的删除不一致*/
	virtual void OnDelete() override;

	virtual void Modify() override;

	/** 计算选中的物体的默认尺寸 */
	FVector CalcActorDefaultSize(AActor* InActor);

	/**
	 * @ 判断点是否在此对象内
	 * @param Pos - const FVector & - 输入要检测的点
	 * @return bool - true 表示在面内，否则不在
	 */
	virtual bool IsPointInObj(const FVector & Pos);

	/**
	 * 设置离地高度
	 * @param InDist - float - 高度
	 * @return void - 
	 */
	virtual void SetHeightToFloor(float InDist) { HeightToFloor = InDist; }

	/**
	 * 得到离地高度
	 * @return float - 返回离地高度
	 */
	virtual float GetHeightToFloor()const { return HeightToFloor; }

	const FVector& GetStartPos();
	const FVector& GetEndPos();
	const FVector& GetPos();
    const FVector& GetDirectionPointPos();

	FORCEINLINE FVector GetDirection() { return Direction; }
	FORCEINLINE FVector GetHorizontalDirecton() {
		return HorizontalDirection;
	}
	FORCEINLINE float   GetLength() { return Length; }
	FORCEINLINE float   GetWidth() { return Width; }
	FORCEINLINE float   GetHeight() { return Height; }
	FORCEINLINE bool    IsRightOpen() { return bRightOpen; }

	virtual void SetPos(const FVector& InPos);
	virtual void SetDirection(const FVector& InDirection);
	void SetLength(const float& InLength);
	void SetWidth(const float& InWidth);
	virtual void SetHeight(const float& InHeight);
	void SetRightOpen(bool InbRightOpen);

	virtual float GetWindowDeep() { return Width; }

	virtual const void GetAlonePoints(TArray< TSharedPtr<FArmyPoint> >& OutPoints)override;

	uint8 GetOperationPointType() const;

	void SetOperationPointType(uint8 Type);

	bool GetIsRightOpen() const { return bRightOpen; }

	bool IsDirty = true;

	TSharedPtr<class FArmyRoom> GetRelatedRoomByLine(TSharedPtr<FArmyLine> CheckedLine, EModelType InModelType = E_HomeModel);

	virtual const  FString& GetAttachModelName()const { return AttachModelName; }

	bool CaptureDoubleLine(const FVector& Pos, EModelType InModelType = E_HomeModel);//用于记载门窗时关联对应的墙线

	/**@欧石楠 检查以这个点进行移动后，两端点的投影是否在内墙线上决定是否可以移动*/
	//bool CheckPointIsProjectOnLine(FVector &Point);

protected:
	FString AttachModelName;
	/** 取消选中操作点 */
	void DeselectPoints();

	virtual void HideRulerLineInputBox(bool bHide);

	/**@欧石楠 更新虚线标注*/
	void UpdateDashLine();

	/**@欧石楠 找到关联线及其所有关联门窗端点离该标尺点最近的点*/
	void GetStartNearestPoint(TSharedPtr<FArmyLine> LinkedLine, FVector &DestPos);

	void GetEndNearestPoint(TSharedPtr<FArmyLine> LinkedLine, FVector &DestPos);

	/**@欧石楠 选中控件时的标注虚线框*/
	TSharedPtr<class FArmyRulerLine> HardWareRulerLine;
	virtual void OnHardWareInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	TSharedPtr<class FArmyRulerLine> FirstLineLeftRulerLine;
	virtual void OnFirstLineLInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	TSharedPtr<class FArmyRulerLine> FirstLineRightRulerLine;
	virtual void OnFirstLineRInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	TSharedPtr<class FArmyRulerLine> SecondLineLeftRulerLine;
	virtual void OnSecondLineLInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	TSharedPtr<class FArmyRulerLine> SecondLineRightRulerLine;
	virtual void OnSecondLineRInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

public:
	/** 操作点 */
	TSharedPtr<FArmyEditPoint> StartPoint;
	TSharedPtr<FArmyEditPoint> EndPoint;
	TSharedPtr<FArmyEditPoint> Point;
	TSharedPtr<FArmyEditPoint> DirectionPoint;
	/*@欧石楠 选中的点**/
	TSharedPtr<FArmyEditPoint> ResultPoint;

	TSharedPtr<FArmyLine> LinkFirstLine;
	TSharedPtr<FArmyLine> LinkSecondLine;

	TSharedPtr<FArmyLine> BrokenWallLeftLine;
	TSharedPtr<FArmyLine> BrokenWallRightLine;

	TSharedPtr<class FArmyRoom> FirstRelatedRoom;
	TSharedPtr<class FArmyRoom> SecondRelatedRoom;

    TSharedPtr<class FArmyRect> RectImagePanel;

	FVector AutoPos;//适配后的位置
	float AutoThickness = 0.f;//适配的墙体厚度

    bool bGenerate3D = true;

	EModelType CurrentModelType;

	//add by huaen 2018-10-26
	int32 InWallType = 0;//0: 在原始墙体上 1：在新建墙体上
	// 获取墙洞面片信息
	virtual	void GetAttachHoleWallInfo(TArray<struct FBSPWallInfo>& OutAttachWallInfo);

	/** @马云龙 存放LightMapID序列化 */
	FGuid LightMapID;

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

	/** 离地高度 */
	float HeightToFloor;

	/** 是否右开 */
	bool bRightOpen;

	uint8 OperationPointType = 0;

	/**@欧石楠 标记是否显示标示线*/
	bool bShowDashLine;

	/**@欧石楠 标尺线偏移量*/
	float RulerOffset;

	/**@欧石楠 标尺线朝向*/
	FVector DashLineUpDir = FVector::ZeroVector;

	/**@欧石楠 断尺相关数据*/
	FVector FirstDestinationStart;
	FVector FirstDestinationEnd;
	FVector SecondDestinationStart;
	FVector SecondDestinationEnd;

	bool bIsLeftToRight = true;

	bool bShowEditPoint = true;

	/**@欧石楠 是否显示外墙标尺线*/
	bool bShowOutWallDashLine = false;

	/**@欧石楠 标记该控件是点击产生的还是二次点击拷贝产生的*/
	bool bIsCopy = false;

public:
	bool bModified = true;
};
REGISTERCLASS(FArmyHardware)