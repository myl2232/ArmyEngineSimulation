#pragma once
#include "CoreMinimal.h"
#include "ArmyObject.h"
#include "ArmyTypes.h"

class ARMYTOOLS_API FArmyRectSelect :
	public TSharedFromThis<FArmyRectSelect>
{
public:
	enum UnitType
	{
		Unit_Vertex,
		Unit_Primitive,
		Unit_Object
	};
	struct ObjectInfo
	{
		ObjectInfo(FObjectWeakPtr InObj,bool InHoverLine):Obj(InObj),NeedHoverLine(InHoverLine){}
		FObjectWeakPtr Obj;
		bool NeedHoverLine = false;

		bool operator<(ObjectInfo const& InOther) const
		{
			return true;
		}
		bool operator==(const ObjectInfo& InOther) const
		{
			return Obj == InOther.Obj;
		}
	};
	struct ARMYTOOLS_API FPrimitiveInfo
	{
		FPrimitiveInfo(FObjectWeakPtr InObj, TSharedPtr<"FArmyPrimitive> InPrimitive) :Obj(InObj), Primitive(InPrimitive) {}
		FObjectWeakPtr Obj;
		TSharedPtr<"FArmyPrimitive> Primitive;

		bool operator==(const FPrimitiveInfo& InOther) const
		{
			return Obj == InOther.Obj && Primitive == InOther.Primitive;
		}
	};
	struct OperationInfo
	{
		bool IsValid()
		{
			return CurrentOperationObject.IsValid() && (CurrentOperationPoint.IsValid() || CurrentOperationLine.IsValid());
		}
		void Reset()
		{
			if (CurrentOperationPoint.IsValid())
			{
				if (CurrentOperationPoint->GetState() == "FArmyPrimitive::OPS_Hovered)
				{
					CurrentOperationPoint->SetState("FArmyPrimitive::OPS_Normal);
				}
			}
			if (CurrentOperationLine.IsValid())
			{
				if (CurrentOperationLine->GetState() == "FArmyPrimitive::OPS_Hovered)
				{
					CurrentOperationLine->SetState("FArmyPrimitive::OPS_Normal);
				}
			}
			if (CurrentOperationObject.IsValid())
			{
				if (CurrentOperationObject.Pin()->GetState() == EObjectState::OS_Hovered)
				{
					CurrentOperationObject.Pin()->SetState(EObjectState::OS_Normal);
				}
			}
			CurrentOperationObject = NULL;
			CurrentOperationPoint = NULL;
			CurrentOperationLine = NULL;
		}
		const OperationInfo& operator =(const OperationInfo& Other)
		{
			CurrentOperationObject = Other.CurrentOperationObject;
			CurrentOperationPoint = Other.CurrentOperationPoint;
			CurrentOperationLine = Other.CurrentOperationLine;
			return *this;
		}
		FObjectWeakPtr CurrentOperationObject;
		TSharedPtr<"FArmyEditPoint> CurrentOperationPoint;
		TSharedPtr<"FArmyLine> CurrentOperationLine;
	};

	FArmyRectSelect();
	~FArmyRectSelect();
	void Draw(UArmyEditorViewportClient* InViewPortClient, FViewport* InViewport, FCanvas* SceneCanvas);
	void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	bool MouseMove(class UArmyEditorViewportClient* InViewPortClient,int32 X, int32 Y, const TArray<ObjectInfo>& InCanHoverObjects);
	bool MouseDrag(FViewport* Viewport, FKey Key);
	bool LeftMouseButtonPress(class UArmyEditorViewportClient* InViewPortClient, int32 X, int32 Y, const TArray<FObjectWeakPtr>& InObjectArray, bool IsPressCtl = false);

	void Start();
	void End();

	void Clear();

	void AddFiltType(EObjectType InObjType);

	void SetSelectUnit(UnitType InUnit);

	void EnableMultipleSelect(bool InV) { MultipleSelect = InV; };

	int32 GetState() { return CurrentState; }
	void SetState(int32 State) { CurrentState = State; }

	const TArray<EObjectType>& GetFiltArray()const { return FiltArray; }

	const TArray<FVector>& GetSelectedVertexs() const;
	const TArray<FPrimitiveInfo>& GetSelectedPrimitives() const;
	const TArray<FObjectWeakPtr>& GetSelectedObjects() const;
	const FArmyRectSelect::OperationInfo& GetCurrentOperationInfo() const;
	/**	设置捕捉平面，该平面的物体的点线信息都是相对该平面的坐标值*/
	void SetRefCoordinateSystem(const FVector& InBasePos, const FVector& InXDir, const FVector& InYDir, const FVector& InNormal);

	/**	获取捕捉基面*/
	const FPlane& GetPlane() const;

	//选中Object后，会发出此代理
	"FArmyObjectesDelegate SelectedObjectsDelegate;

	void AddToSelectedObject(FObjectWeakPtr InObj);

	void SetPickOne(bool IsPickOne) { PickOne = IsPickOne; }

	/**@刘克祥 控制全局hover是否启用*/
	void SetEnableGlobalHover(bool bEnable);

	///*@刘克祥 hover可高亮端点的obj接口**/
	//void AddCanHoverObjects(ObjectInfo InObjInfo);
	//void RemoveCanHoverObjects(ObjectInfo InObjInfo);
	/*@刘克祥 是否处于图纸模式，图纸模式下不需要捕获墙体*/
	bool BIsConstruction;
	//@ 布局模式需要hover空间名称对象
	bool BIsLayOut = false;
private:
	bool PickSelect(int32 X, int32 Y, UArmyEditorViewportClient* InViewPortClient, const TArray<FObjectWeakPtr>& InObjectArray,bool Subtract = false);
	void LeftSelect(const FBox2D& InBox, UArmyEditorViewportClient* InViewPortClient, const TArray<FObjectWeakPtr>& InObjectArray, bool Subtract = false);
	void RightSelect(const FBox2D& InBox, UArmyEditorViewportClient* InViewPortClient, const TArray<FObjectWeakPtr>& InObjectArray, bool Subtract = false);

	FVector ToBasePlane(const FVector& InV);
	FVector FromBasePlane(const FVector& InV);
	/**
	 * @ hover标注相关对象
	 * @return bool - true表示hover上，否则没有
	 */
	//bool HoverTextLabelObj(UArmyEditorViewportClient* InViewPortClient, int32 X, int32 Y, const TArray<ObjectInfo>& InCanHoverObjects);
private:
	bool MultipleSelect = false;
	bool IntersectRect = false;
	bool PickOne = true;
	int32 CurrentState;//0 end 1 start 2 first press
	/**@刘克祥 控制全局hover是否启用*/
	bool bEnableGlobalHover = true;

	FVector BasePoint;
	FVector BaseNormal;
	FVector BaseXDir;
	FVector BaseYDir;
	FPlane BasePlane;//基面

	UnitType Unit;
	TArray<EObjectType> FiltArray;

	/**	当前操作的拣选信息,包含当前的操作点，和当前的操作对象*/
	OperationInfo CurrentOperationInfo;

	FVector2D RectFirstPoint = FVector2D(ForceInitToZero);
	FVector2D RectSecondPoint = FVector2D(ForceInitToZero);

	TArray<FVector> TempSelectedVertexs;
	TArray<FPrimitiveInfo> TempSelectedPrimitives;
	TArray<FObjectWeakPtr> TempSelectedObjects;

	/*@刘克祥 全局hover的端点**/
	TSharedPtr<"FArmyEditPoint> CurrnetHoverPoint;
	//TArray<ObjectInfo> AllCanHoverObjects;

	TArray<FVector> SelectedVertexs;
	TMap<FVector, TSharedPtr<"FArmyPoint>> SelectedDrawPoints;
	TArray<FPrimitiveInfo> SelectedPrimitives;
	TArray<FObjectWeakPtr> SelectedObjects;
};

