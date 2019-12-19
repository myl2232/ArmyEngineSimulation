#pragma once

#include "ArmyObject.h"
#include "ArmyTypes.h"
#include "ArmyCommonTypes.h"
#include "../Data/ConstructionData/IArmyConstructionHelper.h"
using namespace FContentItemSpace;

class FArmyPipeline;
class FArmyPipePoint;

DECLARE_DELEGATE_TwoParams(FArmyPipePointVectorDelegate,TSharedPtr<FArmyPipePoint>,const FVector&);
DECLARE_DELEGATE_ThreeParams(FArmyUpdateInfoDelegate,int32,FString,FString);

/**
* Description
*/
enum EPipePointType
{
  	EPPT_None = 0, // 没有类型
	//水管类
	EPPT_Direct = 1,// 直通（算量）
	EPPT_Flexure = 2,//弯通（算量）
	EPPT_Tee = 3,//三通（算量）
	EPPT_BridgeBend = 4,//过桥弯（算量）
	EPPT_Trap = 5,//存水弯
	EPPT_Water_Max,
	//电线类
	EPPT_DirectE,//电线的拐弯（不算量）
	EPPT_DirectBridgeBend ,//电线的过桥弯（不算量）
	EPPT_Eriect_Max,
	EPPT_Max,
};

/**
* 弧类型
*/
enum EArcType
{
  	AT_None,
	AT_Single = 1,
	AT_Mutil = 2,
	AT_Max
};

struct FPipeLine : TSharedFromThis<FPipeLine>
{
	FGuid LineID;

	int32 Radius;

	int32 Diameter;

	FColor NormalColor;

	FColor ReformColor;

	FVector LineDirection;

	FPipeLine()
	{

	}

	FPipeLine(FGuid InGuid,FVector InLineDirection,float InRadius,float InDiameter,FColor InNormalColor,FColor InReformColor)
		: LineID(InGuid),
		LineDirection(InLineDirection),
		Radius(InRadius),
		Diameter(InDiameter),
		NormalColor(InNormalColor),
		ReformColor(InReformColor)
	{

	}
	void Update(FGuid InGuid,FVector InLineDirection,float InRadius,float InDiameter,FColor InNormalColor,FColor InReformColor)
	{
		LineID = InGuid;
		LineDirection = InLineDirection;
		Radius = InRadius;
		Diameter = InDiameter;
		NormalColor = InNormalColor;
		InReformColor = InReformColor;
	}

	void Update(TSharedPtr<FArmyPipeline>& Line);
	void SerializeToJson (TSharedRef<TJsonWriter<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
	void Deserialization (const TSharedPtr<FJsonObject>& InJsonData);
};

/**
* 水电管线 节点
*/
class ARMYSCENEDATA_API FArmyPipePoint : public FArmyObject, public IArmyConstructionHelper
{
public:
	FArmyPipePoint ();
	FArmyPipePoint (FVector InLocation);
	FArmyPipePoint (FArmyPipePoint* Copy);
	virtual ~FArmyPipePoint ();

	// FObject2D Interface Begin
	virtual void SerializeToJson (TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization (const TSharedPtr<FJsonObject>& InJsonData) override;
	virtual void Draw (FPrimitiveDrawInterface* PDI,const FSceneView* View) override;
	virtual void ApplyTransform (const FTransform& Trans) override;
	virtual const FBox GetBounds () override;
	virtual void Refresh ()override;


	void SetLocation (const FVector InLocation){Location = InLocation;}

	const FVector& GetLocation ();

	FVector GetBridgeBendLocation(TSharedPtr<FArmyPipePoint> Point);

	/** 判断当前管线是否创建ShapeActor */
	virtual bool IsCreate();

	virtual void GeneratePipePointModel (UWorld* World);

	void  EditorApplyTranslation(const FVector& deltaVector);

	void  PostEditPointes (TArray<TSharedPtr<FArmyObject>>& Objectes);

	virtual void CreateMatrial();



	void DestroyPipelineModel ();
	// FObject2D Interface End

	void AddPipeLine(TWeakPtr<FArmyPipeline> PipeLine);

	void RemovePipeLine(TWeakPtr<FArmyPipeline> PipeLine);

	void UpdateLineInfo(TWeakPtr<FArmyPipeline> PipeLine);

	void Update(TWeakPtr<FArmyPipeline> PipeLine);

	void UpdateInfo();

	void UpdatePPType();

	FString GetDiamterStr();

	/**
	* 实时更新名字
	*/
	void UpdateName(FString InName = "");

	/**
	* 实时更新管径
	*/
	void UpdateRadius();

	/**
	* 是否生成actor
	*/
	bool CanDraw();

   /**
   * 获取构件id
   */
	int32 UpdateComponentID();

	/**
	*
	*/
	void GetConnectLines(TArray<FObjectPtr>& Lines);

	/** 过桥弯 or 自动避让 */
	bool IsBridgeBend();
	/**
	* 获取过桥弯的点位数据
	*/
	void CreateBridgeVertexes(TArray<FVector>& Verties);

	void AddBridgeBendPoint(TSharedPtr<FArmyPipePoint> Point,FVector Location);
	void AddBridgeBendPoint(FGuid PointID,FVector Location);

	void MergeOtherPoint(TSharedPtr<FArmyPipePoint> OtherPoint);

	void DrawBridgeBend( TArray<FVector>& Verties );
	void DrawCylinder(TSharedPtr<FPipeLine> PipeLine,  TArray<FVector>& Verties);

	float GetRadius();

	/**设置过桥弯的半径*/
	void SetBridgeBendRadius(float Radius);

	void SetBridgeBottomPoint(FVector InPoint){ BridgeBottomPoint  = InPoint;}

	FLinearColor GetColor();

	float GetScaleClip();

	EPipePointType GetPPType(){return PipePointType; }

	void SetLineType(EObjectType InType)
	{
		LineType = InType;
	}
	
	void SetColor(FColor InColor);

	void SetReformColor (FColor InColor)
	{
		ReformColor = FLinearColor::FromSRGBColor( InColor );
	}

	bool IsElectirc();

	/**
	* 能否连接
	*/
	bool CanLink();

	/**
	* 改变管线颜色
	*/
	void ChangeColor();

	FString GetRadiusStr (){ return RadiusStr; };

	TWeakPtr<FArmyPipeline> GetLine(FGuid& objectID);
	TWeakPtr<FArmyPipeline> GetLine(int32 _index);
	TSharedPtr<FPipeLine> GetLineInfo(FGuid& LineID);

	void GetPipeLines(TArray<TWeakPtr<FArmyPipeline>>& OutLines);
	AActor* GetRelevanceActor();
public:
	// 相近的点
	TMap<FGuid,FVector> PipeLineMap;
	TArray<FGuid> PipeLines;
	TMap<FGuid,TSharedPtr<FPipeLine>> PipeLineInfoMap;

	class AXRPipePointActor* PipePointActor;

	/** 该点位置*/
	FVector Location;
	/**过桥弯的半径*/
	float BridgeBendRadius;
	/**
	* 过桥弯
	*/
	//过桥弯最低点
	FVector BridgeBottomPoint;

	/** 过桥弯的旋转法线*/
	FVector RotateNormal;

	EArcType ArcType;

	////过桥弯起始点
	//FGuid BridgeStart;
	//FGuid BridgeEnd;
	// ---- 过桥弯 end---
	FLinearColor Color;

	FLinearColor ReformColor;

	bool bReform;// 是否是断电该

	FString RadiusStr;//管径

	bool bSameDiameter;//是否是同径
	/**
	*商品代码
	*/
	int32 ProductID = -1;

	EPipePointType PipePointType;

	EPipePointType PrePipePointType;
	
	int32 PreComponentID;

	EObjectType LineType;

	/** 提交Actor变形代理 */
	FArmyObjectesDelegate PostEditDelegate;
	static TMap<FString,int32> PPTypeNameMap;
	FArmyUpdateInfoDelegate UpdateInfoDelegate;
	/** 材质*/
	class UMaterialInstanceDynamic* MaterialInsDy;

	//@郭子阳 获得所在的房间，用于施工项
	TSharedPtr<class FArmyRoom> GetRoom() override { return nullptr; };

	virtual void OnRoomSpaceIDChanged(int32 NewSpaceID) {};
};

REGISTERCLASS(FArmyPipePoint)