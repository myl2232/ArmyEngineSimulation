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
	/**	���ò�׽ƽ�棬��ƽ�������ĵ�����Ϣ������Ը�ƽ�������ֵ*/
	void SetRefCoordinateSystem(const FVector& InBasePos, const FVector& InXDir, const FVector& InYDir, const FVector& InNormal);

	/**	��ȡ��׽����*/
	const FPlane& GetPlane() const;

	//ѡ��Object�󣬻ᷢ���˴���
	"FArmyObjectesDelegate SelectedObjectsDelegate;

	void AddToSelectedObject(FObjectWeakPtr InObj);

	void SetPickOne(bool IsPickOne) { PickOne = IsPickOne; }

	/**@������ ����ȫ��hover�Ƿ�����*/
	void SetEnableGlobalHover(bool bEnable);

	///*@������ hover�ɸ����˵��obj�ӿ�**/
	//void AddCanHoverObjects(ObjectInfo InObjInfo);
	//void RemoveCanHoverObjects(ObjectInfo InObjInfo);
	/*@������ �Ƿ���ͼֽģʽ��ͼֽģʽ�²���Ҫ����ǽ��*/
	bool BIsConstruction;
	//@ ����ģʽ��Ҫhover�ռ����ƶ���
	bool BIsLayOut = false;
private:
	bool PickSelect(int32 X, int32 Y, UArmyEditorViewportClient* InViewPortClient, const TArray<FObjectWeakPtr>& InObjectArray,bool Subtract = false);
	void LeftSelect(const FBox2D& InBox, UArmyEditorViewportClient* InViewPortClient, const TArray<FObjectWeakPtr>& InObjectArray, bool Subtract = false);
	void RightSelect(const FBox2D& InBox, UArmyEditorViewportClient* InViewPortClient, const TArray<FObjectWeakPtr>& InObjectArray, bool Subtract = false);

	FVector ToBasePlane(const FVector& InV);
	FVector FromBasePlane(const FVector& InV);
	/**
	 * @ hover��ע��ض���
	 * @return bool - true��ʾhover�ϣ�����û��
	 */
	//bool HoverTextLabelObj(UArmyEditorViewportClient* InViewPortClient, int32 X, int32 Y, const TArray<ObjectInfo>& InCanHoverObjects);
private:
	bool MultipleSelect = false;
	bool IntersectRect = false;
	bool PickOne = true;
	int32 CurrentState;//0 end 1 start 2 first press
	/**@������ ����ȫ��hover�Ƿ�����*/
	bool bEnableGlobalHover = true;

	FVector BasePoint;
	FVector BaseNormal;
	FVector BaseXDir;
	FVector BaseYDir;
	FPlane BasePlane;//����

	UnitType Unit;
	TArray<EObjectType> FiltArray;

	/**	��ǰ�����ļ�ѡ��Ϣ,������ǰ�Ĳ����㣬�͵�ǰ�Ĳ�������*/
	OperationInfo CurrentOperationInfo;

	FVector2D RectFirstPoint = FVector2D(ForceInitToZero);
	FVector2D RectSecondPoint = FVector2D(ForceInitToZero);

	TArray<FVector> TempSelectedVertexs;
	TArray<FPrimitiveInfo> TempSelectedPrimitives;
	TArray<FObjectWeakPtr> TempSelectedObjects;

	/*@������ ȫ��hover�Ķ˵�**/
	TSharedPtr<"FArmyEditPoint> CurrnetHoverPoint;
	//TArray<ObjectInfo> AllCanHoverObjects;

	TArray<FVector> SelectedVertexs;
	TMap<FVector, TSharedPtr<"FArmyPoint>> SelectedDrawPoints;
	TArray<FPrimitiveInfo> SelectedPrimitives;
	TArray<FObjectWeakPtr> SelectedObjects;
};

