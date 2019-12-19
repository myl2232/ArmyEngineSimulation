#include "ArmyAddWall.h"
#include "ArmyPolygon.h"
#include "ArmyEngineModule.h"
#include "ArmyStyle.h"
#include "ArmyRulerLine.h"
#include "ArmySceneData.h"
#include "ArmyAxisRuler.h"
#include "ArmyHttpModule.h"
#include "ArmyViewportClient.h"
#include "ArmyGameInstance.h"

FArmyAddWall::FArmyAddWall()
	: Polygon(MakeShareable(new FArmyPolygon()))
	, Width(12.f)
	, Height(FArmySceneData::WallHeight)
{
	UMaterial* LineMaterial = FArmyEngineModule::Get().GetEngineResource()->GetAddWallMaterial();
	Color = FLinearColor::Green;
	MI_Line = UMaterialInstanceDynamic::Create(LineMaterial, NULL);
	MI_Line->AddToRoot();
	MI_Line->SetVectorParameterValue(TEXT("MainColor"), Color);
	Polygon->MaterialRenderProxy = MI_Line->GetRenderProxy(false);
	ObjectType = OT_AddWall;
	SetPropertyFlag(FLAG_COLLISION, true);
	SetName(TEXT("新增墙"));

	SelectedRulerLine = MakeShareable(new FArmyRulerLine());
	SelectedRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyAddWall::OnLineInputBoxCommittedCommon));

	AxisRuler = MakeShareable(new FArmyAxisRuler());
	AxisRuler->Init(GVC->ViewportOverlayWidget);

	//@ 添加墙输入框数值越界后输入框内容重置操作
	AxisRuler->AxisOperationDelegate.BindRaw(this, &FArmyAddWall::PreCalTransformIllegalCommon);

	/**@欧石楠 请求材质商品信息*/
	ReqMaterialSaleID();

	ConstructionItemData = MakeShareable(new FArmyConstructionItemInterface);

}

FArmyAddWall::FArmyAddWall(FArmyAddWall* Copy)
{

}

bool FArmyAddWall::Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		return Polygon->IsSelected(Pos, InViewportClient);
	}
	return false;
}
void FArmyAddWall::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FLAG_VISIBILITY))
	{

		if (FArmyObject::GetDrawModel(MODE_CEILING) || FArmyObject::GetDrawModel(MODE_TOPVIEW))
		{
			if (bIsHalfWall)
			{
				TArray<FVector> outverts;
				GetVertexes(outverts);

				int number = outverts.Num();
				for (int i = 0; i < number; i++)
				{
					PDI->DrawLine(outverts[i%number] + FVector(0, 0, FArmySceneData::WallHeight + 21.0f), outverts[(i + 1) % number] + FVector(0, 0, FArmySceneData::WallHeight + 21.0f), FLinearColor::Black, 0);
				}
				Polygon->MaterialRenderProxy = FArmyEngineModule::Get().GetEngineResource()->GetFloorMaterial()->GetRenderProxy(false);
				Polygon->Draw(PDI, View);
			}
		}
		else if (FArmyObject::GetDrawModel(MODE_ADDWALLPOST))
		{
			for (auto line : PolyLines)
			{
				FArmyLine PostLine(line.Get());
				PostLine.DepthPriority = SDPG_World;
				PostLine.SetBaseColor(FLinearColor::White);
				PostLine.Draw(PDI, View);
			}
		}
		else
		{
			if (!FArmySceneData::Get()->bIsDisplayDismantle && bIsHalfWall)
			{
				Polygon->MaterialRenderProxy = nullptr;
				for (auto line : PolyLines)
				{
					line->SetBaseColor(FLinearColor::White);
				}
			}
			else if (FArmySceneData::Get()->bIsDisplayDismantle)
				Polygon->MaterialRenderProxy = MI_Line->GetRenderProxy(false);

			if (State == OS_Selected)
			{
				TArray<TWeakPtr<FArmyObject>> objects;
				FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, objects);
				AxisRuler->Update(GVC, objects);
			}

			bool bSelectedLine = false;
			for (auto line : PolyLines)
			{
				if (line->GetState() == FArmyPrimitive::EOPState::OPS_Selected)
				{
					SelectedLine = line;
					UpdateSeletedRulerLine();
					SelectedRulerLine->Draw(PDI, View);
					bSelectedLine = true;
				}

				if (FArmyObject::GetDrawModel(MODE_MODIFYADD))//新增墙模式
				{
					line->DepthPriority = SDPG_Foreground;
					if (FArmyObject::GetDrawModel(MODE_NORAMAL))
					{
						line->DepthPriority = SDPG_World;
					}
				}
				line->Draw(PDI, View);
			}
			if (!bSelectedLine)
				SelectedRulerLine->ShowInputBox(false);

			Polygon->SetPolygonOffset(0.2);
			Polygon->Draw(PDI, View);
			AxisRuler->Draw(PDI, View);
		}
	}
}
void FArmyAddWall::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	JsonWriter->WriteArrayStart(TEXT("vertexes"));
	TArray<FVector>& Vertexes = Polygon->Vertices;
	for (auto& Vertex : Vertexes)
	{
		JsonWriter->WriteValue(Vertex.ToString());
	}
	JsonWriter->WriteArrayEnd();
	JsonWriter->WriteValue("MatType", GetMatType());
	JsonWriter->WriteValue("Height", GetHeight());
	JsonWriter->WriteValue("bOffsetGenerate", bOffsetGenerate);
	JsonWriter->WriteValue("bIsHalfWall", bIsHalfWall);
	JsonWriter->WriteValue("bInternalExtrusion", bInternalExtrusion);
	/**@欧石楠 存储施工项*/
	ConstructionItemData->SerializeToJson(JsonWriter);

	SERIALIZEREGISTERCLASS(JsonWriter, FArmyAddWall)
}
void FArmyAddWall::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);

	bDeserialization = true;

	TArray<FVector> Vertexes;
	TArray<FString> VertexesStrArray;
	InJsonData->TryGetStringArrayField("vertexes", VertexesStrArray);
	for (auto& ArrayIt : VertexesStrArray)
	{
		FVector Vertex;
		Vertex.InitFromString(ArrayIt);
		Vertexes.Add(Vertex);
	}
	SetVertexes(Vertexes, true);
	SetMatType(InJsonData->GetStringField(TEXT("MatType")));
	SetHeight(InJsonData->GetNumberField(TEXT("Height")));
	bOffsetGenerate = InJsonData->GetBoolField(TEXT("bOffsetGenerate"));
	bIsHalfWall = InJsonData->GetBoolField(TEXT("bIsHalfWall"));
	InJsonData->TryGetBoolField("bInternalExtrusion", bInternalExtrusion);
	/**@欧石楠 读取施工项*/
	ConstructionItemData->Deserialization(InJsonData);

	bDeserialization = false;
}
void FArmyAddWall::SetState(EObjectState InState)
{
	State = InState;

	switch (InState)
	{
	case OS_Normal:
		Color = FLinearColor::Green;
		break;
	case OS_Selected:
		Color = FLinearColor(FColor(0xFFFF9800));
		break;
	default:
		break;
	}
	MI_Line->SetVectorParameterValue(TEXT("MainColor"), Color);
	for (auto line : PolyLines)
	{
		line->SetBaseColor(Color);
		line->SetNormalColor(Color);
	}
	if (State == OS_Selected)
		AxisRuler->SetBoundObj(this->FArmyObject::AsShared());
	else
		AxisRuler->SetBoundObj(nullptr);
}
bool FArmyAddWall::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		return Polygon->IsSelected(Pos, InViewportClient);
	}
	return false;
}

void FArmyAddWall::GetVertexes(TArray<FVector>& OutVertexes)
{
	OutVertexes.Append(Polygon->Vertices);
}
void FArmyAddWall::GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{
	OutLines = PolyLines;
}
const FBox FArmyAddWall::GetBounds()
{
	return FBox(Polygon->Vertices);
}

TSharedPtr<FArmyLine> FArmyAddWall::HoverLine(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	if (!bOffsetGenerate)
		return nullptr;

	for (auto It : PolyLines)
	{
		//排除掉小于250mm的线段
		if (It->IsSelected(Pos, InViewportClient))
		{
			return It;
		}
	}
	return NULL;
}

void FArmyAddWall::ApplyTransform(const FTransform& Trans)
{
	TArray<FVector> Points = Polygon->Vertices;
	for (auto& It : Points)
	{
		It = Trans.TransformPosition(It);
	}

	//@ 计算变换是否越界
	if (!CalInternalWallIntersection(Points))
	{
		SetVertexes(Points);
	}
	else
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("输入的数值超出范围"));
	}
}

void FArmyAddWall::Delete()
{
	FArmyObject::Delete();
	SelectedRulerLine->ShowInputBox(false);

	DeleteRelevanceHardwares();
}

void FArmyAddWall::GetAttachHoleWallInfo(TArray<struct FBSPWallInfo>& OutAttachWallInfo)
{
	TArray<FVector> OutLineVerts;
	GetVertexes(OutLineVerts);
	if (FArmyMath::IsClockWise(OutLineVerts))
		FArmyMath::ReversePointList(OutLineVerts);

	TArray<TSharedPtr<FArmyHardware>> RelatedHardwares;
	TArray<FObjectWeakPtr> HardWareObjects;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Pass, HardWareObjects);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Door, HardWareObjects);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_NewPass, HardWareObjects);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_SlidingDoor, HardWareObjects);


	for (auto ItObject : HardWareObjects)
	{
		TSharedPtr<FArmyHardware> HWObject = StaticCastSharedPtr<FArmyHardware>(ItObject.Pin());
		FVector tempCenterPos = FVector(HWObject->GetPos().X, HWObject->GetPos().Y, 0.0f);
		float length = HWObject->GetLength();
		float width = HWObject->GetWidth();
		FVector horiztonalDir = HWObject->GetHorizontalDirecton();
		FVector verticalDir = HWObject->GetDirection();
		TArray<FVector> ClippBox = { tempCenterPos - horiztonalDir / 2 * length + verticalDir / 2 * width,
			tempCenterPos + horiztonalDir / 2 * length + verticalDir / 2 * width,
			tempCenterPos + horiztonalDir / 2 * length - verticalDir / 2 * width,
			tempCenterPos - horiztonalDir / 2 * length - verticalDir / 2 * width };

		if (FArmyMath::ArePolysOverlap(OutLineVerts, ClippBox))
		{
			RelatedHardwares.Emplace(HWObject);
		}
	}

	int number = OutLineVerts.Num();
	TArray<FVector> newOutVerts = FArmyMath::ExturdePolygon(OutLineVerts, FArmySceneData::FinishWallThick, false);
	for (int i = 0; i < number; i++)
	{
		const FVector& orginalStart = OutLineVerts[i%number];
		const FVector& orginalEnd = OutLineVerts[(i + 1) % number];

		const FVector& finishStart = newOutVerts[i%number];
		const FVector& finishEnd = newOutVerts[(i + 1) % number];
		TArray<TArray<FVector>> RelatedHoles;
		TArray<FString> RelatedModelName;
		for (TSharedPtr<FArmyHardware> iter : RelatedHardwares)
		{
			FVector TempStart = iter->GetStartPos();
			FVector TempEnd = iter->GetEndPos();
			if (FArmyMath::AreLinesParallel(finishStart, finishEnd, TempStart, TempEnd))
			{
				float height = iter->GetHeight();
				TArray<FVector> Holes = { TempStart ,TempStart + FVector(0,0,height),TempEnd + FVector(0,0,height) ,TempEnd };
				RelatedHoles.Emplace(Holes);
				RelatedModelName.Emplace(iter->GetAttachModelName());
			}
		}
		FBSPWallInfo singalWallOrignialInfo, singalWallFinishInfo;

		TArray<FVector> orignalVerts = { orginalStart ,orginalStart + FVector(0,0,Height + FArmySceneData::FinishWallThick),
			orginalEnd + FVector(0,0,Height + FArmySceneData::FinishWallThick), orginalEnd };
		TArray<FVector> finishVerts = { finishStart ,finishStart + FVector(0,0,Height + FArmySceneData::FinishWallThick),
			finishEnd + FVector(0,0,Height + FArmySceneData::FinishWallThick),
			finishEnd };
		FVector wallNormal = (orginalEnd - orginalStart).RotateAngleAxis(-90, FVector(0, 0, 1)).GetSafeNormal();
		singalWallOrignialInfo.PolyVertices.Push(orignalVerts);
		singalWallFinishInfo.PolyVertices.Push(finishVerts);
		singalWallFinishInfo.AttachWindowOrDoorName = RelatedModelName;
		for (TArray<FVector>& iter0 : RelatedHoles)
		{
			singalWallOrignialInfo.PolyVertices.Emplace(iter0);
			singalWallFinishInfo.PolyVertices.Emplace(iter0);
		}
		singalWallOrignialInfo.Normal = singalWallFinishInfo.Normal = wallNormal;
		singalWallFinishInfo.GenerateFromObjectType = singalWallOrignialInfo.GenerateFromObjectType = ObjectType;
		singalWallOrignialInfo.UniqueIdForRoomOrHardware = singalWallFinishInfo.UniqueIdForRoomOrHardware = GetUniqueID().ToString();

		FString IndexName = FString::Printf(TEXT("%d"), i);
		singalWallFinishInfo.UniqueId = singalWallOrignialInfo.UniqueId = GetUniqueID().ToString() + IndexName;

		OutAttachWallInfo.Emplace(singalWallOrignialInfo);
		OutAttachWallInfo.Emplace(singalWallFinishInfo);
	}
	if (Height < FArmySceneData::WallHeight)
	{
		for (FVector& iter : OutLineVerts)
		{
			iter = FVector(iter.X, iter.Y, Height);
		}
		for (FVector& iter : newOutVerts)
		{
			iter = FVector(iter.X, iter.Y, Height + FArmySceneData::FinishWallThick);
		}

		FBSPWallInfo topOrgin, topFinish;
		topOrgin.PolyVertices.Emplace(OutLineVerts);
		topFinish.PolyVertices.Emplace(newOutVerts);
		topOrgin.GenerateFromObjectType = topFinish.GenerateFromObjectType = ObjectType;
		topOrgin.Normal = topFinish.Normal = FVector(0, 0, 1);
		topOrgin.UniqueIdForRoomOrHardware = topFinish.UniqueIdForRoomOrHardware = GetUniqueID().ToString();
		topOrgin.UniqueId = topFinish.UniqueId = GetUniqueID().ToString() + TEXT("TopSurface");
		for (TSharedPtr < FArmyHardware> iter : RelatedHardwares)
		{
			if (iter.IsValid() && iter->GetHeight() > Height)
			{
				TArray<FVector> clibbox;
				iter->GetVertexes(clibbox);
				topOrgin.PolyVertices.Emplace(clibbox);
				topFinish.PolyVertices.Emplace(clibbox);
			}
		}
		OutAttachWallInfo.Emplace(topOrgin);
		OutAttachWallInfo.Emplace(topFinish);


	}
}


void FArmyAddWall::GetRelatedHardwares(TArray<TWeakPtr<FArmyObject>>& relatedHardwares)
{
	TArray<TWeakPtr<FArmyObject>> allHardwares;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_Pass, allHardwares);
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_SlidingDoor, allHardwares);
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_NewPass, allHardwares);
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_Door, allHardwares);


	for (TWeakPtr<FArmyObject> iter : allHardwares)
	{
		TArray<FVector> clippBox;
		iter.Pin()->GetVertexes(clippBox);

		if (FArmyMath::ArePolysOverlap(clippBox, Polygon->Vertices))
		{
			relatedHardwares.Emplace(iter);
		}
	}

}

bool FArmyAddWall::IsPointInObj(const FVector & Pos)
{
	return FArmyMath::IsPointInOrOnPolygon2D(Pos, Polygon->Vertices);
}

void FArmyAddWall::SetVertexes(TArray<FVector>& Vertexes, bool bIsDeserialization)
{
	if (Vertexes.Num() == 0)
		return;

	FBox Box(Vertexes);
	CenterPos = Box.GetCenter();

	PolyLines.Empty();
	TSharedPtr<FArmyEditPoint> PrePoint = MakeShareable(new FArmyEditPoint(Vertexes[0]));
	TSharedPtr<FArmyEditPoint> FirstPoint = PrePoint;
	for (int i = 1; i < Vertexes.Num(); i++)
	{
		TSharedPtr<FArmyEditPoint> CurrentPoint = MakeShareable(new FArmyEditPoint(Vertexes[i]));
		TSharedPtr<FArmyLine> CurrentLine = MakeShareable(new FArmyLine);
		CurrentLine->SetStartPointer(PrePoint);
		CurrentLine->SetEndPointer(CurrentPoint);
		CurrentLine->SetLineWidth(WALLLINEWIDTH);
		PrePoint = CurrentPoint;
		PolyLines.Push(CurrentLine);
	}
	TSharedPtr<FArmyLine> EndLine = MakeShareable(new FArmyLine);
	EndLine->SetStartPointer(PrePoint);
	EndLine->SetEndPointer(FirstPoint);
	EndLine->SetLineWidth(WALLLINEWIDTH);
	PolyLines.Push(EndLine);

	//@欧石楠 把所有的线的基础颜色设为规定颜色
	for (auto line : PolyLines)
	{
		line->SetBaseColor(Color);
		line->SetNormalColor(Color);
		line->SetHoverColor(FLinearColor(FColor(0xFFF55200)));
	}

	UpdatePolyVertices(bIsDeserialization);
}
void FArmyAddWall::Generate(UWorld* InWorld)
{
}

void FArmyAddWall::SetMatType(FString InType)
{
	MatType = InType;
	/**@欧石楠 因为后台使用前台写死数据 暂时硬处理*/
	if (InType == TEXT("红砖"))
	{
		SelectedMaterialID = 1;
	}
	else if (InType == TEXT("轻体砖"))
	{
		SelectedMaterialID = 2;
	}
	else if (InType == TEXT("轻体砖保温填充材质"))
	{
		SelectedMaterialID = 3;
	}
	else if (InType == TEXT("保温板+石膏板保温"))
	{
		SelectedMaterialID = 4;
	}
	else if (InType == TEXT("轻钢龙骨隔墙"))
	{
		SelectedMaterialID = 5;
	}
	else if (InType == TEXT("大芯板"))
	{
		SelectedMaterialID = 6;
	}


	UMaterial* LineMaterial = FArmyEngineModule::Get().GetEngineResource()->GetAddWallMaterial();
	if (InType == TEXT("轻体砖") || InType == TEXT("红砖") || InType == TEXT("轻体砖保温填充材质"))
	{
		LineMaterial = FArmyEngineModule::Get().GetEngineResource()->GetAddWallMaterial();

	}
	else if (InType == TEXT("轻钢龙骨隔墙"))
	{
		LineMaterial = FArmyEngineModule::Get().GetEngineResource()->GetAWLightgageSteelJoistMaterial();
	}
	else if (InType == TEXT("保温板+石膏板保温"))
	{
		LineMaterial = FArmyEngineModule::Get().GetEngineResource()->GetLineMaterial();
	}

	Color = FLinearColor::Green;
	MI_Line = UMaterialInstanceDynamic::Create(LineMaterial, NULL);
	MI_Line->AddToRoot();
	MI_Line->SetVectorParameterValue(TEXT("MainColor"), Color);
	Polygon->MaterialRenderProxy = MI_Line->GetRenderProxy(false);

	/**@欧石楠 请求材质商品信息*/
	ReqMaterialSaleID();
}

TSharedPtr<FArmyLine> FArmyAddWall::GetParallelLine() const
{
	if (!SelectedLine.IsValid())
		return nullptr;

	FVector SelectedLineCenter = FArmyMath::GetLineCenter(SelectedLine->GetStart(), SelectedLine->GetEnd());

	//@ 计算标尺线反方向与选择线最近的相交线
	float BoxMaxLength = 100000;
	TSharedPtr<FArmyLine> OutLine = nullptr;
	if (SelectedLine.IsValid())
	{
		float Distance = FLT_MAX;
		for (auto It : PolyLines)
		{
			if (It != SelectedLine && FArmyMath::CalculateLine2DIntersection(It->GetStart(), It->GetEnd(), SelectedLineCenter, SelectedLineCenter + (-SelectedRulerDir) * BoxMaxLength))
			{
				float CurrentDis = FArmyMath::CalcPointToLineDistance(SelectedLineCenter, It->GetStart(), It->GetEnd());
				if (CurrentDis < Distance)
				{
					Distance = CurrentDis;
					OutLine = It;
				}
			}
		}
	}

	return OutLine;
}

void FArmyAddWall::UpdatePolyVertices(bool bIsDeserialization)
{
	TArray<FVector> Points;
	for (int i = 0; i < PolyLines.Num(); i++)
	{
		Points.Push(PolyLines[i]->GetStart());
	}
	Polygon->SetVertices(Points);

	if (!bIsDeserialization)
		FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();
}

void FArmyAddWall::UpdateSeletedRulerLine()
{
	bIsStartOrEndLine = false;
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_OutRoom, RoomList);

	FVector SelectedLineCenter = FArmyMath::GetLineCenter(SelectedLine->GetStart(), SelectedLine->GetEnd());
	FVector SelectedLineDir = FArmyMath::GetLineDirection(SelectedLine->GetStart(), SelectedLine->GetEnd());
	SelectedRulerDir = SelectedLineDir.RotateAngleAxis(90, FVector(0, 0, 1));

	if (FArmyMath::IsPointInOrOnPolygon2D(SelectedLineCenter + SelectedRulerDir * 5, Polygon->Vertices))
	{
		SelectedRulerDir *= -1;
	}

	//@ 计算选择线是否为末端线
	float VecNum = Polygon->Vertices.Num();
	for (int32 i = 0; i < VecNum; ++i)
	{
		FVector StartLineCenter = FArmyMath::GetLineCenter(Polygon->Vertices[0], Polygon->Vertices[VecNum - 1]);
		FVector EndLineCenter = FArmyMath::GetLineCenter(Polygon->Vertices[VecNum / 2 - 1], Polygon->Vertices[VecNum / 2]);
		if (FVector::Distance(SelectedLineCenter, StartLineCenter) < 1 || FVector::Distance(SelectedLineCenter, EndLineCenter) < 1)
		{
			bIsStartOrEndLine = true;
			break;
		}
	}

	TSharedPtr<FArmyLine> NeighborLine;
	if (bIsStartOrEndLine)
	{
		//@ 获取选择线的邻接线
		for (auto L : PolyLines)
		{
			if (L.IsValid() && L != SelectedLine && (L->GetStart() == SelectedLine->GetStart() || L->GetEnd() == SelectedLine->GetStart()))
			{
				NeighborLine = L;
				break;
			}
		}

		//计算标尺线的方向，此计算考虑到末端线段可能与两侧线不垂直，标尺线需要跟两侧线的方向一致
		FVector NeighborLineCenter = FArmyMath::GetLineCenter(NeighborLine->GetStart(), NeighborLine->GetEnd());
		if (NeighborLine->GetStart() == SelectedLine->GetStart() || NeighborLine->GetEnd() == SelectedLine->GetStart())
		{
			SelectedRulerDir = (SelectedLine->GetStart() - NeighborLineCenter).GetSafeNormal();
		}
		else if (NeighborLine->GetStart() == SelectedLine->GetEnd() || NeighborLine->GetEnd() == SelectedLine->GetEnd())
		{
			SelectedRulerDir = (SelectedLine->GetEnd() - NeighborLineCenter).GetSafeNormal();
		}
	}

	float MinRoomLineDis = MAX_flt;
	TSharedPtr<FArmyLine> MinDisRoomLine;
	FVector SelectedLineInRoomInter = FVector::ZeroVector;
	float MaxDis = 100000;
	//遍历查找所有房间中距离选择线最近的内墙线
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (Room.IsValid() && Room->IsClosed()/* && FArmyMath::IsPointInOrOnPolygon2D(CenterPos, Room->GetWorldPoints(true))*/)
		{
			float dis = MAX_flt;
			TArray<TSharedPtr<FArmyLine>> RoomLines;
			Room->GetLines(RoomLines);
			for (auto Line : RoomLines)
			{
				if (Line.IsValid())
				{
					FVector posInter;
					FVector LDir = (Line->GetStart() - Line->GetEnd()).GetSafeNormal();
					//在标尺线的方向上与墙线求交，避免标尺线穿过新建墙自身标注
					if (FArmyMath::CalculateLinesIntersection(SelectedLineCenter, SelectedLineCenter + SelectedRulerDir * MaxDis, Line->GetStart()/*  + LDir * FArmySceneData::OutWallThickness*/, Line->GetEnd() /*+ (-LDir * FArmySceneData::OutWallThickness)*/, posInter))
					{
						dis = FVector::Distance(SelectedLineCenter, posInter);
						if (dis < MinRoomLineDis)
						{
							SelectedLineInRoomInter = posInter;
							MinRoomLineDis = dis;
							MinDisRoomLine = Line;
						}
					}
				}
			}
		}
	}
	if (SelectedRulerLine.IsValid() && SelectedLineInRoomInter != FVector::ZeroVector)
	{
		SelectedRulerLine->Update(SelectedLineInRoomInter, SelectedLineCenter);
	}
}

void FArmyAddWall::DeleteRelevanceHardwares()
{
	for (auto ItLine : PolyLines)
	{
		TArray<FObjectWeakPtr> HardWareObjects;
		FArmySceneData::Get()->GetHardWareObjects(HardWareObjects, E_ModifyModel);
		for (auto ItObject : HardWareObjects)
		{
			TSharedPtr<FArmyHardware> HWObject = StaticCastSharedPtr<FArmyHardware>(ItObject.Pin());
			if (HWObject->LinkFirstLine == ItLine || HWObject->LinkSecondLine == ItLine)
			{
				FArmySceneData::Get()->Delete(HWObject);
			}
		}
	}
}

bool FArmyAddWall::CalInternalWallIntersection(TArray<FVector> Points)
{
	if (Points.Num() < 4)
		return false;

	TArray<TWeakPtr<FArmyObject>> OutRoomList;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_OutRoom, OutRoomList);
	for (auto It : OutRoomList)
	{
		TSharedPtr<FArmyRoom> CurrentRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (CurrentRoom.IsValid())
		{
			for (auto ItPoint : Points)
			{
				if (!FArmyMath::IsPointInOrOnPolygon2D(ItPoint, CurrentRoom->GetWorldPoints(true)))
				{
					return true;
				}
			}
		}
	}

	TArray<TWeakPtr<FArmyObject>> AddWallList;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_InternalRoom, AddWallList);
	for (auto It : AddWallList)
	{
		TSharedPtr<FArmyObject> AddWall = StaticCastSharedPtr<FArmyObject>(It.Pin());
		if (AddWall.IsValid())
		{
			TArray<TSharedPtr<FArmyLine>> AddWallLines;
			AddWall->GetLines(AddWallLines);

			for (auto ItLine : AddWallLines)
			{
				for (int32 i = 0; i < Points.Num(); i++)
				{
					FVector PrePoint = Points[i];
					FVector CurrentPoint = Points[i == Points.Num() - 1 ? 0 : i + 1];
					FVector CurrentIntersect;
					if (FArmyMath::CalculateLinesCross(ItLine->GetStart(), ItLine->GetEnd(),
						PrePoint, CurrentPoint, CurrentIntersect))
					{
						//GGI->Window->ShowMessage(MT_Warning, TEXT("新建墙体不允许和内墙线交叉"));
						return true;
					}
				}

			}
		}
	}

	//计算转换是否与其他新建墙相交
	TArray<TWeakPtr<FArmyObject>> AddWall;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_AddWall, AddWall);
	TArray<FVector> WallPoints;
	for (auto W : AddWall)
	{
		TSharedPtr<FArmyAddWall> wall = StaticCastSharedPtr<FArmyAddWall>(W.Pin());
		WallPoints.Empty();
		wall->GetVertexes(WallPoints);
		if ((wall.IsValid() && wall != this->FArmyObject::AsShared())
			&& (FArmyMath::IsPolygonInPolygon(WallPoints, Points)
				|| FArmyMath::IsPolygonInPolygon(Points, WallPoints)
				|| FArmyMath::ArePolysIntersection(Points, WallPoints)))
		{
			return true;
		}
	}

	//@ 计算是否自相交
	TArray<TSharedPtr<FArmyLine>> OldLines;
	Polygon->GetLines(OldLines);

	TArray<TSharedPtr<FArmyLine>> NewLines = PolyLines;
	if (OldLines.Num() == PolyLines.Num())
	{
		FVector OldNormal = FVector::ZeroVector;
		FVector NewNoraml = FVector::ZeroVector;
		for (int i = 0; i < OldLines.Num(); ++i)
		{
			OldNormal = FArmyMath::GetLineDirection(OldLines[i]->GetStart(), OldLines[i]->GetEnd()).GetSafeNormal();
			NewNoraml = FArmyMath::GetLineDirection(PolyLines[i]->GetStart(), PolyLines[i]->GetEnd()).GetSafeNormal();

			//修改后线方向逆向改变视为不合理操作
			float Result = FVector::DotProduct(OldNormal, NewNoraml);
			if (Result < 0)
			{
				return true;//表示相交
			}
		}

		FVector pos;
		for (auto L1 : PolyLines)
		{
			for (auto L2 : PolyLines)
			{
				//修改后线段长度为0也视为操作无效
				if (FMath::Abs(FVector::Dist2D(L1->GetStart(), L1->GetEnd())) < 0.001
					|| FMath::Abs(FVector::Dist2D(L2->GetStart(), L2->GetEnd())) < 0.001)
				{
					return true;
				}

				//存在自相交视为无效操作
				if (FArmyMath::CalculateLinesCross(L1->GetStart(), L1->GetEnd(), L2->GetStart(), L2->GetEnd(), pos))//存在相交，退出循环
				{
					return true;
				}
			}
		}
	}
	else
	{
		return true;
	}

	return false;
}

void FArmyAddWall::OnLineInputBoxCommitted(float InLength)
{
	float CachedLength = SelectedRulerLine->GetInputBoxCachedLength();
	FVector RulerDir = SelectedRulerDir;
	float Distance = FMath::Abs(CachedLength - InLength);
	if (CachedLength < InLength)
		RulerDir *= -1;

	//@  末端线仅延伸线的长度，不修改其他线的位置
	TSharedPtr<FArmyLine> OtherLine;
	if (!bIsStartOrEndLine)
	{
		OtherLine = GetParallelLine();
		if (OtherLine.IsValid())
		{
			FVector OtherOffsetStart = OtherLine->GetStart() + RulerDir * Distance;
			FVector OtherOffsetEnd = OtherLine->GetEnd() + RulerDir * Distance;
			OtherLine->SetStart(OtherOffsetStart);
			OtherLine->SetEnd(OtherOffsetEnd);
		}
	}

	FVector SelectedOffsetStart = SelectedLine->GetStart() + RulerDir * Distance;
	FVector SelectedOffsetEnd = SelectedLine->GetEnd() + RulerDir * Distance;
	SelectedLine->SetStart(SelectedOffsetStart);
	SelectedLine->SetEnd(SelectedOffsetEnd);

	//@ 如果尺寸修改后墙线出现自相交，则此次操作无效，恢复操作前状态TArray<FVector> Points;
	TArray<FVector> Points;
	for (int i = 0; i < PolyLines.Num(); i++)
	{
		Points.Push(PolyLines[i]->GetStart());
	}

	if (CalInternalWallIntersection(Points))
	{
		FVector TempSelectedOffsetStart = SelectedLine->GetStart() - RulerDir * Distance;
		FVector TempSelectedOffsetEnd = SelectedLine->GetEnd() - RulerDir * Distance;
		SelectedLine->SetStart(TempSelectedOffsetStart);
		SelectedLine->SetEnd(TempSelectedOffsetEnd);

		//@ 末端线仅延伸线的长度，不修改其他线的位置
		if (!bIsStartOrEndLine)
		{
			if (OtherLine.IsValid())
			{
				FVector OtherOffsetStart = OtherLine->GetStart() - RulerDir * Distance;
				FVector OtherOffsetEnd = OtherLine->GetEnd() - RulerDir * Distance;
				OtherLine->SetStart(OtherOffsetStart);
				OtherLine->SetEnd(OtherOffsetEnd);
			}
		}

		SelectedRulerLine->GetWidget()->SetInputText(FText::FromString(FString::FromInt(FMath::CeilToInt(CachedLength) * 10) + TEXT(" mm")));

		GGI->Window->ShowMessage(MT_Warning, TEXT("超出新建墙体尺寸范围"));

	}
	else
	{
		UpdatePolyVertices();
	}

	GGI->Window->DismissModalDialog();
	DeleteRelevanceHardwares();
	bModified = true;
}

void FArmyAddWall::OnLineInputBoxCommittedCancel()
{
	SelectedRulerLine->GetWidget()->SetInputText(
		FText::FromString(FString::FromInt(FMath::CeilToInt(SelectedRulerLine->GetInputBoxCachedLength()) * 10) + TEXT(" mm")));
	GGI->Window->DismissModalDialog();
}

void FArmyAddWall::OnLineInputBoxCommittedCommon(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		bool HasHardware = false;
		TArray<TWeakPtr<FArmyObject>> HardWareList;
		FArmySceneData::Get()->GetHardWareObjects(HardWareList);
		for (auto HardWareIt : HardWareList)
		{
			TSharedPtr<FArmyHardware> Hardware = StaticCastSharedPtr<FArmyHardware>(HardWareIt.Pin());
			if (Hardware.IsValid())
			{
				for (auto It : PolyLines)
				{
					if (Hardware->LinkFirstLine == It || Hardware->LinkSecondLine == It)
					{
						HasHardware = true;
						break;
					}
				}
				if (HasHardware)
					break;
			}
		}

		const float LineLength = FCString::Atof(*InText.ToString()) / 10.f;
		if (HasHardware)
		{
			GGI->Window->PresentModalDialog(TEXT("移动墙体后，相关联的门窗数据将删除，是否继续？"), /*SettingWidget->AsShared(),*/
				FSimpleDelegate::CreateRaw(this, &FArmyAddWall::OnLineInputBoxCommitted, LineLength),
				FSimpleDelegate::CreateRaw(this, &FArmyAddWall::OnLineInputBoxCommittedCancel), false);
		}
		else
			OnLineInputBoxCommitted(LineLength);
	}
}

void FArmyAddWall::SetHeight(float InValue)
{
	Height = FMath::Clamp(InValue, 10.f, FArmySceneData::WallHeight);

	if (!bDeserialization)
		FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();
}

void FArmyAddWall::PreCalTransformIllegal(TSharedPtr<FArmyObject> SelectedObj, FTransform trans, int32 Flag)
{
	bool result = true;
	if (SelectedObj->GetType() == OT_AddWall || SelectedObj->GetType() == OT_IndependentWall)
	{
		TSharedPtr<FArmyAddWall> wall = StaticCastSharedPtr<FArmyAddWall>(SelectedObj);
		if (wall.IsValid())
		{
			TArray<FVector> Points;
			TArray<FVector> TempPoints;
			TArray<FVector> TempOriginPoints = CachedPoints;
			TArray<FVector> TempFinishedOriginPoints;
			wall->GetVertexes(Points);
			for (auto It : Points)
			{
				TempPoints.Add(trans.TransformPosition(It));
			}
			for (auto It : TempOriginPoints)
			{
				TempFinishedOriginPoints.Add(trans.TransformPosition(It));
			}
			//首先判断是否和墙体分离了
			bool bShouldMove = false;
			TArray<TWeakPtr<FArmyObject>> RoomList;
			FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_InternalRoom, RoomList);
			for (auto It : RoomList)
			{
				TSharedPtr<FArmyRoom> CurrentRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
				if (CurrentRoom.IsValid() && FArmyMath::IsPointInOrOnPolygon2D(GetBounds().GetCenter(), CurrentRoom->GetWorldPoints(true)))
				{
					TArray< TSharedPtr<FArmyLine> > OutLines;
					CurrentRoom->GetLines(OutLines);
					for (auto ItLine : OutLines)
					{
						for (auto& ItPoint : TempPoints)
						{							
							if (FArmyMath::IsPointOnLine(ItPoint, ItLine->GetStart(), ItLine->GetEnd()))
							{
								bShouldMove = true;
								break;
							}
						}
						if (bShouldMove)
							break;
					}

					break;
				}
			}
			if (!bShouldMove)
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("新建墙不能和原始墙体分离"));
				return;
			}

			result = CalInternalWallIntersection(TempPoints) ? true : false;
			if (result)
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("输入的数值超出范围"));
			}
			else
			{
				DeleteRelevanceHardwares();
				wall->SetVertexes(TempPoints);
				CachedPoints = TempFinishedOriginPoints;
				bModified = true;
			}
		}
	}
}

bool FArmyAddWall::PreCalTransformIllegalCommon(TSharedPtr<FArmyObject> SelectedObj, FTransform trans, int32 Flag)
{
	bool HasHardware = false;
	TArray<TWeakPtr<FArmyObject>> HardWareList;
	FArmySceneData::Get()->GetHardWareObjects(HardWareList);
	for (auto HardWareIt : HardWareList)
	{
		TSharedPtr<FArmyHardware> Hardware = StaticCastSharedPtr<FArmyHardware>(HardWareIt.Pin());
		if (Hardware.IsValid())
		{
			for (auto It : PolyLines)
			{
				if (Hardware->LinkFirstLine == It || Hardware->LinkSecondLine == It)
				{
					HasHardware = true;
					break;
				}
			}
			if (HasHardware)
				break;
		}
	}

	if (HasHardware)
		GGI->Window->PresentModalDialog(TEXT("移动墙体后，相关联的门窗数据将删除，是否继续？"),
			FSimpleDelegate::CreateRaw(this, &FArmyAddWall::PreCalTransformIllegal, SelectedObj, trans, Flag));
	else
		PreCalTransformIllegal(SelectedObj, trans, Flag);

	return true;
}
void FArmyAddWall::ReqMaterialSaleID()
{
	FString Url = FString::Printf(TEXT("/api/brick/%d"), SelectedMaterialID);
	FArmyHttpRequestCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindRaw(this, &FArmyAddWall::ResMaterialSaleID);
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest(Url, CompleteDelegate);
	Request->ProcessRequest();
}

void FArmyAddWall::ResMaterialSaleID(FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful)
	{
        TSharedPtr<FJsonObject> JObject = Response.Data->GetObjectField("data");
        int32 Id = JObject->GetIntegerField("id");
        FString Name = JObject->GetStringField("name");

		MaterialSaleID = Id;
	}
}

void FArmyAddWall::GetAreaAndPerimeter(float & OutArea, float & OutPerimeter)
{
	// 计算周长
	for (auto It : PolyLines)
	{
		OutPerimeter += It->Size();
	}

	// 计算墙中线长度，周长减去两侧宽度，得到内外墙总长度，除以二得到墙中线长度
	float InnerWallLength = (OutPerimeter - Width * 2.f) / 2.f;
	OutArea = InnerWallLength * Height / 10000.f;
	OutPerimeter /= 100.f;
}

void FArmyAddWall::SetIsHalfWall(bool bValue)
{
	bIsHalfWall = bValue;
	FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();
}

void FArmyAddWall::SetCachedPoints(TArray<FVector>& InCachedPoints)
{
	CachedPoints = InCachedPoints;
}

bool FArmyAddWall::RegenerateWall()
{
	TArray<FVector> Vertexes = CachedPoints;
	
	if (Vertexes.Num() < 2)
		return false;

	FVector StartNormal = (Vertexes[1] - Vertexes[0]).GetSafeNormal() ^ FVector(0, 0, 1);
	StartNormal *= bInternalExtrusion ? 1 : -1;
	FVector EndNormal = (Vertexes[Vertexes.Num() - 1] - Vertexes[Vertexes.Num() - 2]).GetSafeNormal() ^ FVector(0, 0, 1);
	EndNormal *= bInternalExtrusion ? 1 : -1;
	FVector StartOffsetPoint = Vertexes[0] + StartNormal * Width;
	FVector EndOffsetPoint;
	if (Vertexes.Num() == 2)
	{
		//@ 计算新建墙是否在房间内，此处解决起止点在墙线上的临界条件
		TArray<TWeakPtr<FArmyObject> > WallList;
		FArmySceneData::Get()->GetObjects(EModelType::E_ModifyModel, OT_InternalRoom, WallList);
		EndOffsetPoint = Vertexes[1] + EndNormal * Width;
		FVector TestPoint = (Vertexes[0] + Vertexes[1]) / 2 + EndNormal;//测试偏移点是否符合
		bool InRoom = false;
		for (auto & W : WallList)
		{
			TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(W.Pin());
			if (Room.IsValid() && Room->IsPointInRoom(TestPoint))
			{
				InRoom = true;
				break;
			}
		}
		if (!InRoom)
		{
			//再判断点是否在删除墙体区域内
			bool bInModify = false;
			TArray<TWeakPtr<FArmyObject>> ModifyWallList;
			FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_ModifyWall, ModifyWallList);
			for (auto It : ModifyWallList)
			{
				TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(It.Pin());
				if (ModifyWall.IsValid())
				{
					TArray<FVector> ModifyPoints;
					ModifyWall->GetVertexes(ModifyPoints);
					if (FArmyMath::IsPointInOrOnPolygon2D(TestPoint, ModifyPoints))
					{
						bInModify = true;
						break;
					}
				}
			}

			if (!bInModify)
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("新建墙体不允许在原始墙体内"));
				return false;
			}
		}

		Vertexes.Push(EndOffsetPoint);
		Vertexes.Push(StartOffsetPoint);		
	}
	else
	{
		EndOffsetPoint = Vertexes[Vertexes.Num() - 1] + EndNormal * Width;
		TArray<FVector> ExtursionPoints = FArmyMath::Extrude3D(Vertexes, -Width * (bInternalExtrusion ? 1 : -1));
		ExtursionPoints.RemoveAt(0);
		ExtursionPoints.RemoveAt(ExtursionPoints.Num() - 1);
		FArmyMath::ReversePointList(ExtursionPoints);
		Vertexes.Push(EndOffsetPoint);
		Vertexes.Insert(ExtursionPoints, Vertexes.Num());
		Vertexes.Push(StartOffsetPoint);
	}		
	TSharedPtr<FArmyAddWall> TempWall = MakeShareable(new FArmyAddWall());
	TempWall->SetVertexes(Vertexes);	
	TempWall->SetWidth(Width);	
	//判断所有的点是否和墙体相交
	TArray<TSharedPtr<FArmyLine>> Lines;
	TempWall->GetLines(Lines);	
	for (auto ItAddLine : Lines)
	{
		//再判断点是否在删除墙体区域内
		bool bInModify = false;
		TArray<TWeakPtr<FArmyObject>> ModifyWallList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_ModifyWall, ModifyWallList);
		for (auto It : ModifyWallList)
		{
			TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(It.Pin());
			if (ModifyWall.IsValid())
			{
				TArray<FVector> ModifyPoints;
				ModifyWall->GetVertexes(ModifyPoints);
				if (FArmyMath::IsPointInOrOnPolygon2D(ItAddLine->GetStart(), ModifyPoints) || 
					FArmyMath::IsPointInOrOnPolygon2D(ItAddLine->GetEnd(), ModifyPoints))
				{
					bInModify = true;
					break;
				}
			}
		}

		TArray<TWeakPtr<FArmyObject>> AddWallList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AddWall, AddWallList);
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_IndependentWall, AddWallList);
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_InternalRoom, AddWallList);
		for (auto It : AddWallList)
		{
			TSharedPtr<FArmyObject> AddWall = StaticCastSharedPtr<FArmyObject>(It.Pin());
			if (AddWall.IsValid())
			{
				TArray<TSharedPtr<FArmyLine>> AddWallLines;
				AddWall->GetLines(AddWallLines);				

				for (auto ItLine : AddWallLines)
				{
					FVector CurrentIntersect;
					if (FArmyMath::CalculateLinesCross(ItLine->GetStart(), ItLine->GetEnd(),
						ItAddLine->GetEnd(), ItAddLine->GetStart(), CurrentIntersect) && !bInModify)
					{
						GGI->Window->ShowMessage(MT_Warning, TEXT("新建墙体不允许交叉绘制"));						
						return false;
					}
				}
			}
		}
	}
	SetVertexes(Vertexes);
	return true;
}

// 计算cad墙线
bool FArmyAddWall::CalCadWallsByHardwares(TArray<TSharedPtr<FArmyPolygon>>&OutArray)
{
	OutArray.Add(Polygon);
	TArray < TWeakPtr<FArmyObject>> Hardwares;
	GetRelatedHardwares(Hardwares);
	for (auto Hardware : Hardwares)
	{
		TSharedPtr <FArmyHardware>   hardWare = StaticCastSharedPtr<FArmyHardware>(Hardware.Pin());
		FVector Beg = hardWare->GetStartPos();
		FVector End = hardWare->GetEndPos();
		FVector Normal = (End - Beg).GetSafeNormal();
		Normal = FVector(-Normal.Y, Normal.X, 0);
		TSharedPtr<FArmyPolygon> intersectedPolygon;

		const float ExtendsPoint = hardWare->GetWidth() * 2;
		{
			auto BegP1 = Beg + Normal*ExtendsPoint;
			auto BegP2 = Beg - Normal*ExtendsPoint;
			TPair<TSharedPtr<FArmyLine>, FVector>   intersectionLine[2];
			TArray<TSharedPtr<FArmyLine>> Lines;
			for (TSharedPtr<FArmyPolygon> polygon : OutArray) {
				polygon->GetLines(Lines);
				int bIntersected = 0;
				for (TSharedPtr<FArmyLine> line : Lines)
				{
					if (bIntersected == 2)
						break;
					FVector intersectionPoint = FVector::ZeroVector;
					if (FMath::SegmentIntersection2D(BegP1, BegP2, line->GetStart(), line->GetEnd(), intersectionPoint))
					{
						intersectionLine[bIntersected].Key = line;
						intersectionLine[bIntersected].Value = intersectionPoint;
						bIntersected++;
					}
				}
				if (bIntersected == 2) {
					intersectedPolygon = polygon;
					break;
				}
			}
			if (!intersectedPolygon.IsValid())
				continue;

			//Get search direction
			FVector searchStart = FVector::ZeroVector;
			FVector tempDirection = intersectionLine[0].Value - intersectionLine[0].Key->GetStart();


			TArray<FVector> Points;
			Points.Add(intersectionLine[0].Value);
			if ((tempDirection | (End - Beg)) > 0)
				Points.Add(intersectionLine[0].Key->GetStart());
			else
				Points.Add(intersectionLine[0].Key->GetEnd());

			TSharedPtr<FArmyLine> CurrentLine;
			TSet < FArmyLine*> AddLines;
			AddLines.Add(intersectionLine[0].Key.Get());
			while (CurrentLine != intersectionLine[1].Key)
			{
				for (auto Line : Lines)
				{
					if (AddLines.Contains(Line.Get()))
						continue;
					if (Line->GetStart() == Points.Last())
					{
						CurrentLine = Line;
						if (Line == intersectionLine[1].Key)
							Points.Add(intersectionLine[1].Value);
						else
							Points.Add(Line->GetEnd());
						AddLines.Add(Line.Get());
						continue;
					}
					if (Line->GetEnd() == Points.Last())
					{
						CurrentLine = Line;
						if (Line == intersectionLine[1].Key)
							Points.Add(intersectionLine[1].Value);
						else
							Points.Add(Line->GetStart());
						AddLines.Add(Line.Get());
						continue;
					}
				}
			}
			TSharedPtr<FArmyPolygon> newPolygon = MakeShareable(new FArmyPolygon);
			newPolygon->SetVertices(Points);
			OutArray.Add(newPolygon);

		}

		{
			auto BegP1 = End + Normal*ExtendsPoint;
			auto BegP2 = End - Normal*ExtendsPoint;
			TPair<TSharedPtr<FArmyLine>, FVector>   intersectionLine[2];
			TArray<TSharedPtr<FArmyLine>> Lines;

			intersectedPolygon->GetLines(Lines);
			int bIntersected = 0;
			for (TSharedPtr<FArmyLine> line : Lines)
			{
				if (bIntersected == 2)
					break;
				FVector intersectionPoint = FVector::ZeroVector;
				if (FMath::SegmentIntersection2D(BegP1, BegP2, line->GetStart(), line->GetEnd(), intersectionPoint))
				{
					intersectionLine[bIntersected].Key = line;
					intersectionLine[bIntersected].Value = intersectionPoint;
					bIntersected++;
				}
			}
			if (bIntersected != 2)
			{
				continue;
			}
			//Get search direction
			FVector searchStart = FVector::ZeroVector;
			FVector tempDirection = intersectionLine[0].Value - intersectionLine[0].Key->GetStart();

			TArray<FVector> Points;
			Points.Add(intersectionLine[0].Value);
			if ((tempDirection | (Beg - End)) > 0)
				Points.Add(intersectionLine[0].Key->GetStart());
			else
				Points.Add(intersectionLine[0].Key->GetEnd());

			TSharedPtr<FArmyLine> CurrentLine;
			TSet < FArmyLine*> AddLines;
			AddLines.Add(intersectionLine[0].Key.Get());
			while (CurrentLine != intersectionLine[1].Key)
			{
				for (auto Line : Lines)
				{
					if (AddLines.Contains(Line.Get()))
						continue;
					if (Line->GetStart() == Points.Last())
					{
						CurrentLine = Line;
						if (Line == intersectionLine[1].Key)

							Points.Add(intersectionLine[1].Value);
						else
							Points.Add(Line->GetEnd());
						AddLines.Add(Line.Get());
						continue;
					}
					if (Line->GetEnd() == Points.Last())
					{
						CurrentLine = Line;
						if (Line == intersectionLine[1].Key)

							Points.Add(intersectionLine[1].Value);
						else
							Points.Add(Line->GetStart());
						AddLines.Add(Line.Get());
						continue;
					}
				}
			}
			TSharedPtr<FArmyPolygon> newPolygon = MakeShareable(new FArmyPolygon);
			newPolygon->SetVertices(Points);
			OutArray.Remove(intersectedPolygon);
			OutArray.Add(newPolygon);
		}
	}
	return true;
}