#include "ArmyEntrance.h"
#include "ArmyPolygon.h"
#include "Materials/Material.h"
#include "ArmyEditorViewportClient.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ArmyStyle.h"
#include "ArmyEngineModule.h"

FArmyEntrance::FArmyEntrance()
{
	ObjectType = EObjectType::OT_Entrance;
	Init();
}

FArmyEntrance::~FArmyEntrance(){}

//��ʼ��
void FArmyEntrance::Init()
{
	WhiteArrows = MakeShareable(new FArmyPolygon);
	OriginWhiteArrows = MakeShareable(new FArmyPolygon);
	WhiteVertices = {
		CH.Top,
		CH.Long_Left,
		CH.Middle_Left,
		CH.Long_BottomLeft,
		CH.Bottom,
	};
	WhiteArrows->SetVertices(WhiteVertices);
	OriginWhiteArrows->SetVertices(WhiteVertices);
	WhiteArrows->MaterialRenderProxy = new FColoredMaterialRenderProxy(UMaterial::GetDefaultMaterial(MD_Surface)->GetRenderProxy(false), FLinearColor(1, 1, 1), FName("SelectionColor"));

	BlackArrows = MakeShareable(new FArmyPolygon);
	OriginBlackArrows = MakeShareable(new FArmyPolygon);
	BlackVertices = {
		CH.Top,
		CH.Long_Right,
		CH.Middle_Right,
		CH.Long_BottomRight,
		CH.Bottom,
	};
	BlackArrows->SetVertices(BlackVertices);
	OriginBlackArrows->SetVertices(BlackVertices);
	//��ȡ�Զ������
	UMaterial* M_BaseColor = FArmyEngineModule::Get().GetEngineResource()->GetBaseColorMat();
	BlackArrows->MaterialRenderProxy = new FColoredMaterialRenderProxy(M_BaseColor->GetRenderProxy(false), FLinearColor(0, 0, 0), FName("SelectionColor"));

	EditPoint = MakeShareable(new FArmyEditPoint);
	EditPoint->OperationType = XROP_MOVE;
}

//PDI����
void FArmyEntrance::Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))//ȫ��OBJ��ʾ״̬ģʽ
	{
		WhiteArrows->Draw(PDI, View);
		BlackArrows->Draw(PDI, View);

		if (GetState() == OS_Selected)
		{
			EditPoint->Draw(PDI, View);
		}
		for (TSharedPtr<FArmyLine> Line : BoundList)
		{
			Line->Draw(PDI, View);
		}
	}
}

//HUD����
void FArmyEntrance::DrawHUD(UArmyEditorViewportClient * InViewPortClient, FViewport * Viewport, const FSceneView * View, FCanvas * Canvas)
{}

//���л�
void FArmyEntrance::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);

	JsonWriter->WriteValue("Type", TEXT("OT_Entrance"));
	JsonWriter->WriteValue("EntranceWorldPosition", WorldPosition.ToString());
	JsonWriter->WriteValue("EntranceWorldRotate", WorldRotate.ToString());
	
	SERIALIZEREGISTERCLASS(JsonWriter, FArmyEntrance)
}

//�����л�
void FArmyEntrance::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);

	WorldPosition.InitFromString(InJsonData->GetStringField("EntranceWorldPosition"));
	WorldRotate.InitFromString(InJsonData->GetStringField("EntranceWorldRotate"));

	SetWorldTransform(WorldPosition, WorldRotate);
}


void FArmyEntrance::Refresh()
{
	if (GetState() == OS_Selected)
	{
		WorldPosition = EditPoint->GetPos();
		SetWorldTransform(WorldPosition, WorldRotate);
	}
}

TSharedPtr<FArmyEditPoint> FArmyEntrance::SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (GetState() == OS_Selected)
		{
			if (EditPoint->IsSelected(Pos, InViewportClient)) return EditPoint;
		}
	}
	return NULL;
}

void FArmyEntrance::SetState(EObjectState InState)
{
	State = InState;

	switch (InState)
	{
	case OS_Normal:
	case OS_Selected:
	case OS_Disable:
		EditPoint->SetState(FArmyPrimitive::EOPState::OPS_Normal);
		break;
	default:
		break;
	}
}

bool FArmyEntrance::IsSelected(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		FVector2D PosPix, WorldPix;
		InViewportClient->WorldToPixel(Pos, PosPix);
		InViewportClient->WorldToPixel(WorldPosition, WorldPix);
		return FBox2D(WorldPix - UP_DIS, WorldPix + UP_DIS).IsInside(PosPix);
	}
	return  false;
}

const FBox FArmyEntrance::GetBounds()
{
	return FBox(TArray<FVector>{ WorldPosition + FVector(LONG_DIS, LONG_DIS, 0), WorldPosition + FVector(LONG_DIS, -LONG_DIS, 0), WorldPosition + FVector(-LONG_DIS, -LONG_DIS, 0), WorldPosition + FVector(-LONG_DIS, LONG_DIS, 0)});
}

TSharedPtr<FArmyEditPoint> FArmyEntrance::HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	return SelectPoint(Pos, InViewportClient);
}

void FArmyEntrance::ApplyTransform(const FTransform& Trans)
{
	SetWorldTransform(Trans.TransformPosition(WorldPosition), WorldRotate);
}

void FArmyEntrance::SetWorldTransform(const FVector& Pos, const FVector& Rotate)
{
	//0����¼�ñ�ǩ��λ���뷽��
	this->WorldPosition = Pos;
	this->WorldRotate   = Rotate;

	//1�����ñ༭���λ��
	EditPoint->SetPos(Pos);

	//2����������
	Transform.SetLocation(Pos);

	//3����ת
	Transform.SetRotation(FQuat::FindBetweenVectors(FVector(0, 1, 0), Rotate));

	//4������Polygon��λ��ת��
	TArray<FVector> TempWhiteVertexes;
	TArray<FVector> TempBlackVertexes;

	for (auto& Vertex : WhiteVertices)
	{
		TempWhiteVertexes.Emplace(Transform.TransformPosition(Vertex));
	}
	for (auto& Vertex : BlackVertices)
	{
		TempBlackVertexes.Emplace(Transform.TransformPosition(Vertex));
	}

	WhiteArrows->SetVertices(TempWhiteVertexes);
	BlackArrows->SetVertices(TempBlackVertexes);

	//5��������ڱ�ǩ�߿�
	BoundList.Empty();
	GetOutLine(TempWhiteVertexes, BoundList);
	GetOutLine(TempBlackVertexes, BoundList);

	//6�����Ƶײ��߶�
	FVector FirstLine_Left   = Transform.TransformPosition(CH.FirstLine_Left);
	FVector FirstLine_Right  = Transform.TransformPosition(CH.FirstLine_Right);
	TSharedPtr<FArmyLine> FirstLine = MakeShareable(new FArmyLine(FirstLine_Left, FirstLine_Right));
	BoundList.Emplace(FirstLine);

	FVector SecondLine_Left = Transform.TransformPosition(CH.SecondLine_Left);
	FVector SecondLine_Right = Transform.TransformPosition(CH.SecondLine_Right);
	TSharedPtr<FArmyLine> SecondLine = MakeShareable(new FArmyLine(SecondLine_Left, SecondLine_Right));
	BoundList.Emplace(SecondLine);

	FVector ThreeLine_Left = Transform.TransformPosition(CH.ThreeLine_Left);
	FVector ThreeLine_Right = Transform.TransformPosition(CH.ThreeLine_Right);
	TSharedPtr<FArmyLine> ThreeLine = MakeShareable(new FArmyLine(ThreeLine_Left, ThreeLine_Right));
	BoundList.Emplace(ThreeLine);
}

void FArmyEntrance::GetOutLine(const TArray<FVector>& InVertices, TArray<TSharedPtr<FArmyLine>>& OutBoundLines)
{
	for (int32 Index = 0;Index < InVertices.Num();++Index)
	{
		TSharedPtr<FArmyLine> TempLine = MakeShareable(new FArmyLine());
		TempLine->SetStart(InVertices[Index]);
		if (Index == InVertices.Num() - 1)//���һ���߶�
		{
			TempLine->SetEnd(InVertices[0]);
		}
		else
		{
			TempLine->SetEnd(InVertices[Index + 1]);
		}
		
		OutBoundLines.Emplace(TempLine);
	}
}