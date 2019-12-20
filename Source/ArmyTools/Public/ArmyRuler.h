#pragma once
#include "CoreMinimal.h"
#include "ArmyObject.h"
#include "ArmyEditableLabelSlate.h"
#include "Engine.h"

/**
* Copyright 2019 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File ArmyConstructionManager.h
* @Description 水电模式标尺
*
* @Author 刘克祥
* @Date 2019年1月26日
* @Version 1.0
*/





//轴的类型
enum EAxisType
{
		XFront=0,
		XBack,
		YFront,
		YBack,
		ZFront,
		ZBack
};

//点位信息
struct HAdsorbDetect
{
	//是否吸附上墙面
	bool bAdsorbDetect = false;

	//吸附点坐标
	FVector AdsPoint;
	//墙面法线
	FVector WallNormal;
	//墙面方向
	FVector WallDirection;

	//是否在两个墙面的交线上
	bool bCornerDetect = false;
	//第二面墙墙面法线
	FVector WallNormal2;
	//第二面墙墙面方向
	FVector WallDirection2;

	//在不在地板上
	bool OnFloor=false;
	//在不在房顶
	bool OnTop = false;

};


//轴信息
class RulerAxis :public FArmyLine
{
public:
	RulerAxis(EAxisType type);

	//轴方向
	FVector Direction;

	bool ShouldShow();

	void SetShouldDraw(bool bDraw) override;

	//偏置方向
	FVector OffsetDirection;

	TWeakPtr<SSArmyEditableLabel> LengthLabel;
	//包裹LengthLabel，以限制LengthLabel的大小
	TWeakPtr<SBox> LengthLabelWrapper;
	const EAxisType Type;

	//刷新文本窗口位置
	void UpdateTextWidgetLocation(const FSceneView*  InView);
	//刷新线宽
	void UpdateLineWidth(const FSceneView*  InView);

	//输入完文字发生的事件
	void OnTextCommitted(const FText& test, const SWidget* widget);

	virtual void SetStart(const FVector& InV) override;
	virtual void SetEnd(const FVector& InV) override;
	virtual FVector GetStart() const override;
	virtual FVector GetEnd() const override;

	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	//刷新文字 
	/*
	*@ForceUpdate 强制刷新
	*/
	void UpdateText(bool ForceUpdate=false);

	//是不是Z轴
	 bool IsZ();
	
private:
	//上一次的长度
	int32 PreLength = 0;
	//是否应当绘制
	bool bShouldDraw=false;
	FVector StartPostion;
	FVector EndPostion;

	//是否自动调整到轴线起点的距离
	 bool AdjustDistanceToStartPostion = true;
	//自动调整线宽
	 bool AdjustLinewidth = false;
};

DECLARE_DELEGATE_RetVal_OneParam(bool, FAxisRulerChanged, FVector);

class ARMYTOOLS_API FArmyRuler 
{
public:
	static	TSharedPtr<FArmyRuler>& Get();

	//更新标尺数据
	/*@CaptureInfo 检测信息
	*@ IgnoreOverlappedWall 忽略重叠的墙面，把发生重叠的墙面视为无墙面
	*/
	void Update(HAdsorbDetect CaptureInfo,bool In2DModle=false,bool IgnoreOverlappedWall=true,FRotator rotator=FRotator(0,0,0));

	//绘制
	void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	void Show(bool Visible);

	bool ShouldShow() {return bVisible;}
	
	bool Is2DModule() {return b2DModle;}
private:	
	//隐藏无限长的轴
	bool HideInfinateAxis=true;
	//2d模式
	bool b2DModle=false;   

	bool bVisible = false;

	//标尺整体的旋转
	FRotator Rotator= FRotator(0, 0, 0);

	//射线最远距离
	float MaxDistance=1000000.0f;
	//射线偏置距离
	float Offset = 0.5f;

	//捕捉信息
	 HAdsorbDetect CaptureInfo;

	//所有轴数据
	TArray<TSharedPtr<RulerAxis>> Axises;
	
	//探测一个轴
	void DetectAxis(TSharedPtr<RulerAxis> Axis);

	//探测所有轴
	void DetectAxises();
	
	// 忽略重叠的墙面，把发生重叠的墙面视为无墙面
	bool IgnoreOverlappedWall=true;

	//重叠墙面的最大距离,法线相反的两个墙面的距离小于这个数就被视为重叠
	float MaxOverlapDistance = 1.0f;
public:
	//绘制时使用偏移
	float DrawWithOffset = true;

	//标尺跟随的AActor
	//AActor *AActorToFollow = nullptr;
public:
	//标尺UI数字修改代理
	FAxisRulerChanged AxisOperationDelegate;

private:
	FArmyRuler();
public:
	~FArmyRuler();
};

