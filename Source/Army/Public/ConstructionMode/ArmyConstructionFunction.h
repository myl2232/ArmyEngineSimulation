#pragma once
#include "SharedPointer.h"
#include "ArmyTextlabel.h"
#include "ArmyDimensions.h"
#include "ArmySymbolLeadLine.h"
#include "ArmySymbolRectRegion.h"
#include "ArmyWHCabinetAutoMgr.h"
#include "ArmyConstructionLayerManager.h"

DECLARE_DELEGATE_RetVal_ThreeParams(TWeakPtr<FArmyLayer>, ConstructionNewLayer, const FName&, bool, int32);

enum class EAreaEnum // 平面中的类型
{
	NoneArea,
	Ceiling, // 顶面
	Wall,    // 墙面
	Floor,   // 地面
};

//索引符号编辑块结构体
struct FIndexSymbolGroup : public TSharedFromThis<FIndexSymbolGroup>
{
	TSharedPtr<FArmySymbol> SymbolLeft;
	TSharedPtr<FArmySymbol> SymbolRight;
	TSharedPtr<FArmySymbol> SymbolTop;
	TSharedPtr<FArmySymbol> SymbolBottom;

	TSharedPtr<FArmySymbolRectRegion> SymbolLeftRectRegion;
	TSharedPtr<FArmySymbolRectRegion> SymbolRightRectRegion;
	TSharedPtr<FArmySymbolRectRegion> SymbolTopRectRegion;
	TSharedPtr<FArmySymbolRectRegion> SymbolBottomRectRegion;

	TSharedPtr<FArmySymbolLeadLine> SymbolGroupLeadLine;
	TSharedPtr<class FArmyEditPoint> EditPoint;

	FIndexSymbolGroup(const FVector& InPos)
	{
		auto NewSymbolFun = [&](const FRotator& R)
		{
			FVector BaseDir(0, -30, 0);
			FVector RDir = R.RotateVector(BaseDir);
			FVector CurrentPos = InPos + RDir;
			TSharedPtr<FArmySymbol> VSymbol = MakeShareable(new FArmySymbol);
			//VSymbol->SetText(CharArray[j]);
			VSymbol->SetTextSize(5);
			//VSymbol->SetText(OwnerText, FArmySymbol::ETT_OWNERTEXT);
			VSymbol->SetTextSize(5, FArmySymbol::ETT_OWNERTEXT);
			VSymbol->SetDrawSpliteLine(true);
			VSymbol->ApplyTransform(FTransform(R, CurrentPos, FVector(1, 1, 1)));
			return VSymbol;
		};
		SymbolTop = NewSymbolFun(FRotator(0, 0 * 90, 0));
		SymbolRight = NewSymbolFun(FRotator(0, 1 * 90, 0));
		SymbolBottom = NewSymbolFun(FRotator(0, 2 * 90, 0));
		SymbolLeft = NewSymbolFun(FRotator(0, 3 * 90, 0));

		SymbolLeftRectRegion = MakeShareable(new FArmySymbolRectRegion);
		SymbolLeftRectRegion->Init(InPos, FArmySymbolRectRegion::EDT_LEFT);
		SymbolRightRectRegion = MakeShareable(new FArmySymbolRectRegion);
		SymbolRightRectRegion->Init(InPos, FArmySymbolRectRegion::EDT_RIGHT);
		SymbolTopRectRegion = MakeShareable(new FArmySymbolRectRegion);
		SymbolTopRectRegion->Init(InPos, FArmySymbolRectRegion::EDT_TOP);
		SymbolBottomRectRegion = MakeShareable(new FArmySymbolRectRegion);
		SymbolBottomRectRegion->Init(InPos, FArmySymbolRectRegion::EDT_BOTTOM);

		SymbolGroupLeadLine = MakeShareable(new FArmySymbolLeadLine);
		SymbolGroupLeadLine->SetBasePos(InPos);
		SymbolGroupLeadLine->SetTargetPos(InPos);

		EditPoint = MakeShareable(new FArmyEditPoint(InPos));
	}

	void Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View)
	{
		SymbolGroupLeadLine->Draw(PDI, View);
		EditPoint->Draw(PDI, View);

		SymbolLeft->Draw(PDI, View);
		SymbolRight->Draw(PDI, View);
		SymbolTop->Draw(PDI, View);
		SymbolBottom->Draw(PDI, View);
	}

	void Update(const FVector& InPos)
	{
		FVector BaseDir(0, -30, 0);
		EditPoint->SetPos(InPos);
		SymbolGroupLeadLine->SetTargetPos(InPos);

		FRotator R1(0, 0 * 90, 0), R2(0, 1 * 90, 0), R3(0, 2 * 90, 0), R4(0, 3 * 90, 0);

		SymbolTop->ApplyTransform(FTransform(R1, InPos + R1.RotateVector(BaseDir), FVector(1, 1, 1)));
		SymbolRight->ApplyTransform(FTransform(R2, InPos + R2.RotateVector(BaseDir), FVector(1, 1, 1)));
		SymbolBottom->ApplyTransform(FTransform(R3, InPos + R3.RotateVector(BaseDir), FVector(1, 1, 1)));
		SymbolLeft->ApplyTransform(FTransform(R4, InPos + R4.RotateVector(BaseDir), FVector(1, 1, 1)));
	}
};

class FArmyConstructionFunction :public TSharedFromThis<FArmyConstructionFunction>
{
public:
	struct PointInfo
	{
		PointInfo(int32 InType, const FVector& InV, const TSharedPtr<FArmyRoom> InRoom = NULL) :StepType(InType), Point(InV), RelationInnerRoom(InRoom) {}
		bool operator==(const PointInfo& InOther) const
		{
			//return StepType == InOther.StepType && Point == InOther.Point;
			return StepType == InOther.StepType && (Point - InOther.Point).Size() < 0.001;
		}

		int32 StepType;//1,2,3
		FVector Point;
		TSharedPtr<FArmyRoom> RelationInnerRoom; //关联房间

		bool bInnerPoint = false;
		int32 LeftInterCount = 0;
		int32 RightInterCount = 0;
		int32 TopInterCount = 0;
		int32 BottomInterCount = 0;
	};
	struct InnerLineInfo
	{
		TSharedPtr<FArmyLine> MainLine;
		TArray<FVector> DetailPoints;
		TArray<TPair<FVector, int32>> ComponentPoints;//int32 用来做构建标尺用的（判断是什么类型的构建，以做分类显示处理）
		TArray<TPair<FVector, int32>> PackPipePoints; //int32 用来判断包立管形状的类型（1 直角包立管）
		int32 UPackPipeCount = 0;//U形包立管个数
		TArray<TPair<FVector, FVector>> IgoneLines;
		TSharedPtr<FArmyRoom> RelationInnerRoom; //关联房间
		TSharedPtr<FArmyObject> RelationPillar;//关联的风道、柱子等Obj
		FVector ComponentCenter = FVector(0, 0, 0);
		//表示是否有包立管外墙点在该直线上
		bool HasPackPipe = false;
		//-1该包立管是否是直角包立管
		uint8 PackPipe = -1;
		bool operator==(const InnerLineInfo& InOther) const
		{
			return MainLine == InOther.MainLine && RelationInnerRoom == InOther.RelationInnerRoom;
		}
		void SortPoints()
		{
			FVector StartPos = MainLine->GetStart();
			DetailPoints.Sort([&](const FVector& A, const FVector& B) {
				return (A - StartPos).Size() < (B - StartPos).Size();
			});
		}
		bool CompareIgone(const TPair<FVector, FVector>& InIgone)
		{
			for (auto & I : IgoneLines)
			{
				if (((I.Key - InIgone.Key).Size() < 0.001 && (I.Value - InIgone.Value).Size() < 0.001) ||
					((I.Key - InIgone.Value).Size() < 0.001 && (I.Value - InIgone.Key).Size() < 0.001))
				{
					return true;
				}
			}
			return false;
		}
		void RemoveDetailPoint(const FVector& Point)//�Ƴ�DetailPoint
		{
			int TempIndex = INDEX_NONE;
			for (int Index = 0; Index < DetailPoints.Num(); ++Index)
			{
				if ((DetailPoints[Index] - Point).Size() < 0.001)
				{
					TempIndex = Index;
					break;
				}
			}
			if (TempIndex != INDEX_NONE)
			{
				DetailPoints.RemoveAt(TempIndex);
			}
		}
	};
	struct FConstructionPropertyInfo
	{
		FName LayerName;
		TMap<uint32, FString> AreaPropertyMap;

		FConstructionPropertyInfo(const FName& InName) :LayerName(InName)
		{}
		bool operator==(const FConstructionPropertyInfo& InOther) const
		{
			return LayerName == InOther.LayerName;
		}
	};

	struct FCabinetFacadeData : public TSharedFromThis<FCabinetFacadeData>
	{
		enum EAlignType
		{
			EA_FACE,
			EA_LEFT,
			EA_RIGHT
		};
		FCabinetFacadeData(const TSharedPtr<FShapeInRoom> InShape, EAlignType InAlignType = EAlignType::EA_FACE) :Shape(InShape), AlignType(InAlignType) {}
		TSharedPtr<FShapeInRoom> Shape;
		EAlignType AlignType = EA_FACE;
		bool ExistDoor = true;
	};

	struct FAreaWithDepthInfo : public TSharedFromThis<FAreaWithDepthInfo>
	{
		FAreaWithDepthInfo(const TSharedPtr<FArmyRoomSpaceArea> InArea, float InDepth = MAX_FLT) :AreaObj(InArea), Depth(InDepth) {};
		float Depth = MAX_FLT;
		TSharedPtr<FArmyRoomSpaceArea> AreaObj;
		TArray<FObjectWeakPtr> OwnerObjectArray;
		TArray<FVector> ProjectOtherVArray;
		FVector StartPos;
		FVector EndPos;
		TArray<TSharedPtr<FArmyRoomSpaceArea>> CoverAreaArray;//遮挡该墙体的其他墙体

		bool VerticalToPlane = false;//是否垂直于投影面（投影成线条）
		bool Intersection = false;//是否与范围框相交
		FBox SectionBox = FBox(ForceInitToZero);//范围框构建的包围盒
	};
	struct AreaLinkGroup
	{
		float Depth = MAX_FLT;
		float AreaSize = 0;
		TArray<FVector> AllVertexArray;
		TArray<TSharedPtr<FAreaWithDepthInfo>> SpaceAreaArray;

		void operator+=(const AreaLinkGroup& InOther)
		{
			Depth < InOther.Depth ? Depth = InOther.Depth : true;
			AreaSize += InOther.AreaSize;
			AllVertexArray.Append(InOther.AllVertexArray);
			SpaceAreaArray.Append(InOther.SpaceAreaArray);
		}
	};

	FArmyConstructionFunction() {}
	~FArmyConstructionFunction() {}

	static ObjectClassType GetAnnotationClassTypeByObject(ObjectClassType InObjClass);
	static ObjectClassType GetClassTypeByObject(FObjectPtr InObj, int32 InType = 0);

	void GenerateAutoDimensions();
	void GenerateStep3Dimensions(const TArray<PointInfo>& TopArray, const TArray<PointInfo>& BottomArray, const TArray<PointInfo>& LeftArray, const TArray<PointInfo>& RightArray, EModelType ModelType = E_HomeModel);
	void GenerateStep2Dimensions(const TArray<PointInfo>& TopArray, const TArray<PointInfo>& BottomArray, const TArray<PointInfo>& LeftArray, const TArray<PointInfo>& RightArray, EModelType ModelType = E_HomeModel);
	void GenerateStep1Dimensions(const TArray<PointInfo>& TopArray, const TArray<PointInfo>& BottomArray, const TArray<PointInfo>& LeftArray, const TArray<PointInfo>& RightArray, EModelType ModelType = E_HomeModel);
	void GenerateOtherInnerDimensions(TArray<InnerLineInfo>& OtherArray);
	void GenerateModifyWallDimensions(const TArray<InnerLineInfo>& InInnerRoomOriginLines, const TArray<InnerLineInfo>& InInnerRoomLayoutLines);
	void GenerateAutoObjectDimensions(const TArray<InnerLineInfo>& InInnerRoomLines);
	void GenerateCeilingDimensions(const TArray<InnerLineInfo>& InInnerRoomLines);
	void GenerateAutoObjectRuler();
	void CreateLampControlLines(FObjectPtr InBindObj);
	void CreateObjectRuler(FObjectPtr InBindObj);
	void CreateObjectEntrance(FObjectPtr InBindObj, const FVector& Pos, const FVector& Rotate);
	TSharedPtr<FArmyTextLabel> CreateObjectTextLable(FObjectPtr InObj, int32 InTag, const FText& InText, const FVector& Pos, FArmyTextLabel::LabelType InLabelType = FArmyTextLabel::LT_None);
	TSharedPtr<FArmyTextLabel> CreateOriginObjectTextLable(FObjectPtr InObj, int32 InTag, const FText& InText, const FVector& Pos, FArmyTextLabel::LabelType InLabelType = FArmyTextLabel::LT_None, FString ArgString = TEXT("AUTO"), bool IgnoreCompare=false);
	TSharedPtr<FArmyDimensions> CreateObjectDismension(ObjectClassType mClassType, const FVector& mBaseStart, const FVector& mBaseEnd, const FVector& mDismensionStart, const FVector& mDismensionEnd, EObjectType mObjType = OT_Dimensions, int32 mTextSize = 10, int32 mAreaType = 0, bool mUseLeadLine = true);
	//标记出冗余的内墙点
	void SignInnerPoint(TArray<FArmyConstructionFunction::PointInfo>& PointInfoList) const;

	void AddLayerFrameProperty(const FName& InLayerName, const FObjectPtr InAddObj);
	/* 自动生成动态图层（例如：橱柜台面）*/
	void GenerateAutoLayers();

	void GenerateWallFacadeLayer(const FName& InLayerName, const TSharedPtr<FArmyRoom> InRoom, const FPlane& InFacePlane, const FBox& InRegionBox);
	void GenerateCupboardPlanar(const FName& InLayerName);
	void GenerateCupboardFacade(const FName& InLayerName, TMultiMap<FVector, TSharedPtr<FCabinetFacadeData>>& InFacadeDataMap, const FVector& InOffset);
	bool GenerateBoardFaceOutline(AActor* InShapeActor, const TSharedPtr<class FSCTShape> InShape, const FVector& InBasePos, const FPlane& InPlane, const FName& InLayerName, const FVector InStartPos, const FVector InEndPos, const FVector InOffset, TArray<FBox2D>& OutSheetBoxArray, TArray<FSimpleElementVertex>& OutVArray);

	void GenerateWallFacadeData(const FName& InLayerName, TArray<AreaLinkGroup>& InWallLinkGroupArray, const TArray<FObjectWeakPtr>& InWallObjectArray, const FPlane &InProjectPlane);

	// 计算不同构件的立面box
	TArray<struct FLinesInfo> GetObjFacadeBoxes(FObjectWeakPtr InObj, TSharedPtr<FArmyRoomSpaceArea> &InWallPtr);
	// 判断点位是否可以投影到当面墙，如果可以计算出中心点
	bool GetComponentCenter(FObjectWeakPtr InObj, TSharedPtr<FArmyRoomSpaceArea> &InWallPtr,FVector &OutCenter);
	// 获得点位相对于墙面的本地坐标
	FVector GetLocalPosByWallSpace(TSharedPtr<FArmyRoomSpaceArea> InWallSpace, FVector InSrcPos);
public:
	ConstructionNewLayer DelegateNewLayer;

	float TY = 0;
	float BY = 0;
	float LX = 0;
	float RX = 0;

	float ThirdDimensionOffset = 90;//cm
	float SecondDimensionOffset = 130;//cm
	float FirstDimensionOffset = 170;//cm

	FBox BoundBox;

	TArray<PointInfo> TopArray, BottomArray, LeftArray, RightArray;

	TArray<FConstructionPropertyInfo> CupboardTableLayerArray;//橱柜台面图层

	//需要更新的对象类型列表
	TArray<ObjectClassType> ClassTypeList;
};