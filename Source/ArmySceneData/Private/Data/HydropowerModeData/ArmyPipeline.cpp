#include "ArmyPipeline.h"
#include "ArmyEditPoint.h"
#include "ArmyFurniture.h"
#include "CoreMinimal.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyStyle.h"
#include "ArmyMath.h"
#include "ArmyPipelineActor.h"
#include "ArmyFurnitureActor.h"
#include "SceneManagement.h"
#include "ArmyViewportClient.h"
#include "ArmySceneData.h"
#include "EngineUtils.h"
#include "ArmyActorConstant.h"

FArmyPipeline::FArmyPipeline ()
	:
	StartPoint (MakeShareable (new FArmyPipePoint))
	,EndPoint (MakeShareable (new FArmyPipePoint))
	,
	OffsetGroundHeight (0.f)
	,bShowPoint (true)
	,bReform (false)
	,DrainType(EDrainType::DT_None)
{
	SetPropertyFlag(FLAG_COLLISION,true);
    SetName(TEXT("水管") +GetUniqueID ().ToString());
	SetState(OS_Normal);
	ObjectType = OT_Pipe_Begin;
	bBoundingBox = false;
	PipeLineActor = NULL;
	Radius = 20.f;
	UMaterialInstance* MaterialInterface = FArmyEngineModule::Get ().GetEngineResource ()->GetLineMaterialIns ();
	MaterialInsDy = UMaterialInstanceDynamic::Create (MaterialInterface,nullptr);
	if (MaterialInsDy->IsValidLowLevel())
	{
		MaterialInsDy->AddToRoot();
	}
}

FArmyPipeline::FArmyPipeline (FArmyPipeline* Copy)
{
	if(Copy)
	{
		bBoundingBox = Copy->bBoundingBox;
		PropertyFlag = Copy->PropertyFlag;
		StartPoint = MakeShareable (new FArmyPipePoint (Copy->StartPoint.Get ()));
		EndPoint = MakeShareable (new FArmyPipePoint (Copy->EndPoint.Get ()));
		OffsetGroundHeight = Copy->OffsetGroundHeight;
		ObjectType = Copy->ObjectType;
		SetName (GetName ());
		SetState (OS_Normal);
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.Template = Copy->PipeLineActor;
		PipeLineActor = GVC->GetWorld ()->SpawnActor<AXRPipelineActor> (SpawnInfo);
		Radius = Copy->Radius;
		MaterialInsDy = Copy->MaterialInsDy;
		bReform = Copy->bReform;
		DrainType = Copy->DrainType;
	}

}

FArmyPipeline::FArmyPipeline (TSharedPtr<FArmyPipePoint>& InStart,TSharedPtr<FArmyPipePoint>& InEnd,EObjectType InObjectType)
	:StartPoint(InStart),
	EndPoint(InEnd),
	OffsetGroundHeight (0.f),
	bShowPoint (true),
	bReform (false),
	DrainType (EDrainType::DT_None)
{
	SetPropertyFlag (FLAG_COLLISION,true);
	SetName (TEXT ("水管")+GetUniqueID ().ToString ());
	SetState (OS_Normal);
	bBoundingBox = false;
	PipeLineActor = NULL;
	Radius = 20.f;
	UMaterialInstance* MaterialInterface = FArmyEngineModule::Get ().GetEngineResource ()->GetLineMaterialIns ();
	MaterialInsDy = UMaterialInstanceDynamic::Create (MaterialInterface,nullptr);
	if(MaterialInsDy->IsValidLowLevel ())
	{
		MaterialInsDy->AddToRoot ();
	}
}

FArmyPipeline::~FArmyPipeline ()
{

	if(MaterialInsDy->IsValidLowLevel ())
	{
		MaterialInsDy->RemoveFromRoot();
	}
	if (PipeLineActor && PipeLineActor->IsValidLowLevel())
	{
		PipeLineActor->Destroy();
	}
	
}

void FArmyPipeline::SerializeToJson (TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson (JsonWriter);

	JsonWriter->WriteValue("Color",Color.ToString());
	JsonWriter->WriteValue ("RefromColor",RefromColor.ToString ());
	JsonWriter->WriteValue ("Radius",Radius);
	JsonWriter->WriteValue ("Diameter",Diameter);

	JsonWriter->WriteObjectStart("StartPoint");
	StartPoint->SerializeToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteObjectStart("EndPoint");
	EndPoint->SerializeToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();


	switch(ObjectType)
	{
		case OT_StrongElectricity_25:            //强电2.5
			JsonWriter->WriteValue ("Type",TEXT ("OT_StrongElectricity_25"));
			break;
		case 	OT_StrongElectricity_4:            //强电4
			JsonWriter->WriteValue ("Type",TEXT ("OT_StrongElectricity_4"));
			break;
		case OT_StrongElectricity_Single:            //强电单控
			JsonWriter->WriteValue ("Type",TEXT ("OT_StrongElectricity_Single"));
			break;
		case OT_StrongElectricity_Double:            //强电双控
			JsonWriter->WriteValue ("Type",TEXT ("OT_StrongElectricity_Double"));
			break;
		case OT_WeakElectricity_TV: //弱电 电视线
			JsonWriter->WriteValue ("Type",TEXT ("OT_WeakElectricity_TV"));
			break;
		case OT_WeakElectricity_Net: //弱电， 网线
			JsonWriter->WriteValue ("Type",TEXT ("OT_WeakElectricity_Net"));
			break;
		case OT_WeakElectricity_Phone: //弱电，电话线
			JsonWriter->WriteValue ("Type",TEXT ("OT_WeakElectricity_Phone"));
			break;
		case OT_ColdWaterTube: //冷水管
			JsonWriter->WriteValue ("Type",TEXT ("OT_ColdWaterTube"));
			break;
		case OT_HotWaterTube: //热水管
			JsonWriter->WriteValue ("Type",TEXT ("OT_HotWaterTube"));
			break;
		case OT_Drain: //排水管
			JsonWriter->WriteValue ("Type",TEXT ("OT_Drain"));
			break;
		default:
			break;
	}

	JsonWriter->WriteValue("ProductID",ProductID);

    SERIALIZEREGISTERCLASS(JsonWriter, FArmyPipeline)
}

void FArmyPipeline::Deserialization (const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization (InJsonData);
	TSharedPtr<FArmyObject> LineObj=this->AsShared();
	TSharedPtr<FArmyPipeline> Line=StaticCastSharedPtr<FArmyPipeline>(LineObj);
	{
		TSharedPtr<FJsonObject> PointData = InJsonData->GetObjectField("StartPoint");
		FGuid PointID;
		FGuid::Parse(PointData->GetStringField("objectId"),PointID);
		bool bSame = true;
		if(StartPoint.IsValid())
			bSame = (PointID == StartPoint->GetUniqueID());
		StartPoint = StaticCastSharedPtr<FArmyPipePoint>(FArmySceneData::Get()->GetObjectByGuid(E_HydropowerModel, PointID).Pin());
		if (!StartPoint.IsValid())
		{
			StartPoint=MakeShareable(new FArmyPipePoint());
			StartPoint->Deserialization(PointData);
			FArmySceneData::Get()->Add(StartPoint,XRArgument(1).ArgUint32 (E_HydropowerModel),nullptr,true);
		}
		else
			StartPoint->Deserialization(PointData);
		
	}
	
	{
		TSharedPtr<FJsonObject> PointData=InJsonData->GetObjectField("EndPoint");
		FGuid PointID;
		FGuid::Parse(PointData->GetStringField("objectId"),PointID);
		EndPoint =StaticCastSharedPtr<FArmyPipePoint>(FArmySceneData::Get()->GetObjectByGuid(E_HydropowerModel,PointID).Pin());
		if(!EndPoint.IsValid())
		{
			EndPoint=MakeShareable(new FArmyPipePoint());
			EndPoint->Deserialization(PointData);
			FArmySceneData::Get()->Add(EndPoint,XRArgument(1).ArgUint32(E_HydropowerModel),nullptr,true);
		}
		else
			EndPoint->Deserialization(PointData);
	}

	FString PropertyRes;
	InJsonData->TryGetStringField ("Type",PropertyRes);
	if(PropertyRes.Equals ("OT_StrongElectricity_25"))
		SetType (OT_StrongElectricity_25);
	else if(PropertyRes.Equals ("OT_StrongElectricity_4"))
		SetType (OT_StrongElectricity_4);
	else if(PropertyRes.Equals ("OT_StrongElectricity_Single"))
		SetType (OT_StrongElectricity_Single);
	else if(PropertyRes.Equals ("OT_StrongElectricity_Double"))
		SetType (OT_StrongElectricity_Double);
	else if(PropertyRes.Equals ("OT_WeakElectricity_TV"))
		SetType (OT_WeakElectricity_TV);
	else if(PropertyRes.Equals ("OT_WeakElectricity_Net"))
		SetType (OT_WeakElectricity_Net);
	else if(PropertyRes.Equals ("OT_WeakElectricity_Phone"))
		SetType (OT_WeakElectricity_Phone);
	else if(PropertyRes.Equals ("OT_ColdWaterTube"))
		SetType (OT_ColdWaterTube);
	else if(PropertyRes.Equals ("OT_HotWaterTube"))
		SetType (OT_HotWaterTube);
	else if(PropertyRes.Equals ("OT_Drain"))
		SetType (OT_Drain);

	InJsonData->TryGetNumberField ("Radius",Radius);
	InJsonData->TryGetNumberField ("Diameter",Diameter);
	InJsonData->TryGetStringField ("Color",PropertyRes);
	Color.InitFromString (PropertyRes);
	InJsonData->TryGetStringField ("RefromColor",PropertyRes);
	RefromColor.InitFromString (PropertyRes);
	InJsonData->TryGetNumberField("ProductID",ProductID);
	
}

void FArmyPipeline::Draw(FPrimitiveDrawInterface* PDI,const FSceneView* View)
{
	
}

bool FArmyPipeline::IsSelected(const FVector& Pos,class UArmyEditorViewportClient* InViewportClient)
{
	if(GetPropertyFlag (FArmyObject::FLAG_VISIBILITY))
	{

	}
	return false;
}

TSharedPtr<FArmyEditPoint> FArmyPipeline::SelectPoint (const FVector& Pos,class UArmyEditorViewportClient* InViewportClient)
{

	TSharedPtr<FArmyEditPoint> ResultPoint = NULL;

	return ResultPoint;
}

TSharedPtr<FArmyEditPoint> FArmyPipeline::HoverPoint (const FVector& Pos,class UArmyEditorViewportClient* InViewportClient)
{
	TSharedPtr<FArmyEditPoint> ResultPoint = NULL;
	if(GetPropertyFlag (FArmyObject::FLAG_VISIBILITY))
	{
	}
	return ResultPoint;
}

bool FArmyPipeline::Hover (const FVector& Pos,class UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
	}
	return false;
}

void FArmyPipeline::GetVertexes (TArray<FVector>& OutVertexes)
{
	OutVertexes.Add(GetStart());
	OutVertexes.Add(GetEnd());
}

void FArmyPipeline::GetLines (TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{
	
}

void FArmyPipeline::ApplyTransform (const FTransform& Trans)
{
	SetStart (Trans.TransformPosition (GetPointLocation(StartPoint)));
	SetEnd (Trans.TransformPosition (GetPointLocation(EndPoint)));
}

const FBox FArmyPipeline::GetBounds ()
{
	TArray<FVector> Points;
	Points.Add (GetStart());
	Points.Add (GetEnd());
	return FBox (Points);
}

void FArmyPipeline::Refresh ()
{
	SetStart(GetStart());
	SetEnd(GetEnd());
}

void FArmyPipeline::SetReformState(bool bInReform)
{
	bReform = bInReform;
	for (auto& PointItem:AttachmentPoints)
	{
		TSharedPtr<FArmyPipePoint> Point = PointItem;// PointItem.Value;
		Point->bReform = bInReform; 
		Point->ChangeColor();
	}
	ChangeColor();
}

bool FArmyPipeline::IsCreate ()
{
	return PipeLineActor!=NULL&&PipeLineActor->IsValidLowLevel();
}

void FArmyPipeline::GeneratePipelineModel (UWorld* World,bool InBaseCenter)
{
	if (!IsCreate())
	{
		FActorSpawnParameters SpawnInfo; 
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FVector StartP = StartPoint->GetLocation();
		FVector EndP = EndPoint->GetLocation();
		PipeLineActor = World->SpawnActor<AXRPipelineActor> (InBaseCenter ? (StartP + EndP)/2 : StartP, FRotator::ZeroRotator, SpawnInfo);
		if (GetType()>= OT_Preload_Begin && GetType()<= OT_Preload_End 
			|| GetType() >= OT_Component_Begin && GetType() <= OT_Component_End)
		{
			PipeLineActor->SetFolderPath(FArmyActorPath::GetOriginalPointPath());
			PipeLineActor->Tags.Add(XRActorTag::OriginalPoint);
		}
		else
		{
			PipeLineActor->SetFolderPath(FArmyActorPath::GetPipelinePath());
		}
		
		PipeLineActor->SetRelevanceObject (this->AsShared());
		PipeLineActor->SetActorLabel(GetName());
		PipeLineActor->Tags.Add("MoveableMeshActor");
		PipeLineActor->Tags.Add("HydropowerPipeActor");
		PipeLineActor->DeltaDelegate.BindRaw (this,&FArmyPipeline::EditorApplyTranslation);
		PipeLineActor->PostEditDelegate.BindRaw (this,&FArmyPipeline::PostEditPointes);
		ChangeColor();
		
	}
}

void FArmyPipeline::CreateMatrial ()
{
	if(PipeLineActor)
	{
		CreateElectric();
	}
}

void FArmyPipeline::Update ()
{
	if (PipeLineActor== nullptr)
		return;
	CreateMatrial();
	//PipeLineActor->SetActorRelativeLocation ((GetPointLocation (StartPoint)+GetPointLocation (EndPoint))/2);
}


void FArmyPipeline::UpdateWithPoints(bool bTrans)
{
	TSharedPtr<FArmyPipeline> Line = StaticCastSharedRef<FArmyPipeline>(this->AsShared());
	Update();
	StartPoint->Update(Line);
	EndPoint->Update(Line);
	for (auto& PointItem : AttachmentPoints)
	{
		TSharedPtr<FArmyPipePoint> Point = PointItem;//PointItem.Value;
		Point->Update(Line);
	}
	if (!bTrans)
		Modify();
}

void FArmyPipeline::Create()
{
    check(StartPoint.IsValid());
    check(EndPoint.IsValid());

    //StartPoint->Create();
    //EndPoint->Create();
	FArmyObject::Create(); 
}

void FArmyPipeline::PostModify(bool bTransaction/*=false*/)
{
	TArray<TSharedPtr<FArmyObject>> Objectes;
	Objectes.Add(StartPoint);
	Objectes.Add(EndPoint);
	if(ModifyDelegate.IsBound())
		ModifyDelegate.ExecuteIfBound(Objectes,bTransaction);
}

TSharedPtr<FArmyPipeline> FArmyPipeline::CopyPipeLine()
{
	TSharedPtr<FArmyPipeline> CopyResult = MakeShareable (new FArmyPipeline ());

	CopyResult->PostEditDelegate = PostEditDelegate;

	CopyResult->SetColor (Color);
	CopyResult->SetReformColor(RefromColor);
	CopyResult->SetPointColor(PointColor);
	CopyResult->SetPointRefromColor(PointRefromColor);
	CopyResult->SetType(ObjectType);
	CopyResult->SetRadius(Diameter);
	CopyResult->Diameter = Diameter;
	CopyResult->SetName(GetName());

	return CopyResult;
}

void FArmyPipeline::EditorApplyTranslation (const FVector& deltaVector)
{
	StartPoint->Location+=deltaVector;
	EndPoint->Location+=deltaVector;
}

void FArmyPipeline::PostEditPointes (TArray<TSharedPtr<FArmyObject>>& Objectes)
{
	if (PostEditDelegate.IsBound())
		PostEditDelegate.ExecuteIfBound(Objectes);
}

void FArmyPipeline::AddBridgeBendPoint (TSharedPtr<FArmyPipeline> Line,TSharedPtr<FArmyPipePoint> point)
{
	/*if(AttachmentPoints.Contains(Line->GetUniqueID()))
		AttachmentPoints.Remove(Line->GetUniqueID());
	AttachmentPoints.Add(Line->GetUniqueID(),point);*/
	AttachmentPoints.AddUnique(point);
}

void FArmyPipeline::AddBridgeBendPoint(TSharedPtr<FArmyPipePoint> point)
{
	AttachmentPoints.AddUnique(point);
}

void FArmyPipeline::GetBridgePointInfo(TArray<TSharedPtr<FBridgePointInfo>>& OutPointInfoes)
{
	int32 index = 0;
	EObjectType LinkerType = EObjectType(GetType()+OT_Pipe_End-OT_Pipe_End);
	for (int32 i =0;i<BridgeInfoes.Num();i++)
	{
		TSharedPtr<FBridgeOverlyLineInfo> OverlyLineInfo = BridgeInfoes[i];
		switch (ObjectType)
		{
		case OT_StrongElectricity_25:            //强电2.5
		case OT_StrongElectricity_4:            //强电4
		case OT_StrongElectricity_Single:            //强电单控
		case OT_StrongElectricity_Double:            //强电双控
		case OT_WeakElectricity_TV: //弱电 电视线
		case OT_WeakElectricity_Net: //弱电， 网线
		case OT_WeakElectricity_Phone: //弱电，电话线
		{
			bool bHas = false;
			if (OutPointInfoes.Num())
			{
				TSharedPtr<FBridgePointInfo> PointInfo = OutPointInfoes.Last();
				if (PointInfo->Contain(OverlyLineInfo))
				{
					PointInfo->LineInfoes.AddUnique(OverlyLineInfo);
					bHas = true;
				}
			}
			if(!bHas)
			{
				TSharedPtr<FBridgePointInfo> PointInfo = MakeShareable(new FBridgePointInfo(GetStart(),GetEnd(),StartPoint->GetUniqueID(),EndPoint->GetUniqueID(), Radius,PointColor,PointRefromColor,LinkerType));
				PointInfo->LineInfoes.Add(OverlyLineInfo);
				OutPointInfoes.AddUnique(PointInfo);
			}
		}
		break;
		case OT_ColdWaterTube: //冷水管
		case OT_HotWaterTube: //热水管
		case OT_Drain: //排水管
		{
			TSharedPtr<FBridgePointInfo> PointInfo = MakeShareable(new FBridgePointInfo(GetStart(),GetEnd(),StartPoint->GetUniqueID(),EndPoint->GetUniqueID(),Radius,PointColor,PointRefromColor,LinkerType));
			PointInfo->LineInfoes.Add(OverlyLineInfo);
			OutPointInfoes.AddUnique(PointInfo);
		}
		break;
		default:
			break;
		}

	}
}

void FArmyPipeline::RemoveOverlayLine(TSharedPtr<FArmyPipeline>& Line)
{
	if (OverlayLines.Num ())
		OverlayLines.Remove(Line->GetUniqueID());
}

void FArmyPipeline::GetAllBridgeBendPoints (TArray<TSharedPtr<FArmyPipePoint>>& OutArray)
{
	TArray<TSharedPtr<FArmyPipePoint>> TempArray;
	//AttachmentPoints.GenerateValueArray(TempArray);
	for (int32 i = 0 ;i<AttachmentPoints.Num();i++)
	{
		OutArray.AddUnique(AttachmentPoints[i]);
	}
	OutArray.Sort ([this](const TSharedPtr<FArmyPipePoint>& P1,const TSharedPtr<FArmyPipePoint>& P2)
	{
		float Lenght1 = (P1->GetLocation() - GetStart()).Size();
		float Lenght2 = (P2->GetLocation() - GetStart()).Size();
		return Lenght1 < Lenght2;
	});
}

void FArmyPipeline::CreateElectric ()
{
	{
		if (GetStart().Equals(GetEnd()))
			return;
        if (PipeLineActor&&PipeLineActor->IsValidLowLevel())
        {
            PipeLineActor->ResetMeshTriangles();
            TArray<FVector> Verties;
            FVector Start = CreateElectricTurn(StartPoint, Verties);
            FVector End = CreateElectricTurn(EndPoint, Verties);
            if (!CreateBridgeBend(Start, End, Verties))
            {
                DrawCylinder(End, Start, Verties);
            }
            if (Verties.Num())
            {
				FTransform Trans = PipeLineActor->GetTransform();
				for (auto & V : Verties)
				{
					V = Trans.InverseTransformPosition(V);
				}
                PipeLineActor->AddVetries(Verties);
                PipeLineActor->UpdateAllVetexBufferIndexBuffer();
            }
        }
		
	}
}

bool FArmyPipeline::CreateBridgeBend (FVector& InStart,FVector& InEnd,TArray<FVector>& Verties)
{
	TArray<TSharedPtr<FArmyPipePoint>> BridgeBendPoints;
	GetAllBridgeBendPoints(BridgeBendPoints);
	if (BridgeBendPoints.Num() == 0 )
		return false;
	TArray<FVector> Points;
	Points.Add(InStart);
	Points.Add(BridgeBendPoints[0]->GetBridgeBendLocation (StartPoint));
	int32 Lenght = BridgeBendPoints.Num ();
	for (int32 i = 1; i<Lenght;i++)
	{
		Points.Add(BridgeBendPoints[i-1]->GetBridgeBendLocation (EndPoint));
		Points.Add(BridgeBendPoints[i]->GetBridgeBendLocation (StartPoint));
	}
	Points.Add(BridgeBendPoints[Lenght - 1]->GetBridgeBendLocation (EndPoint));
	Points.Add(InEnd);

	for(int32 i = 0;i<Points.Num ();i=i+2)
	{
		FVector Start = Points[i];
		FVector End = Points[i+1];
		DrawCylinder (Start,End,Verties);
	}
	for (int32 i = 0; i<Lenght;i++)
	{
		TSharedPtr<FArmyPipePoint> Point = BridgeBendPoints[i];
		if (Point->GetPPType() == EPPT_DirectBridgeBend)
		{
			Point->CreateBridgeVertexes(Verties); 
		}
	}
	return true;
}

void FArmyPipeline::DrawCylinder (FVector InStart,FVector InEnd,TArray<FVector>& Verties)
{
	FArmyMath::DrawCylinder(InStart,InEnd,GetRadius() / 10.f,Verties);
}

FVector FArmyPipeline::CreateElectricTurn (TSharedPtr<FArmyPipePoint> Point,TArray<FVector>& Verties)
{
	if (Point->PipeLines.Num() == 2 && Point->IsElectirc() && !Point->IsBridgeBend())
	{
		TWeakPtr<FArmyPipeline> LineObj0 = Point->GetLine(0);
		TWeakPtr<FArmyPipeline> LineObj1 = Point->GetLine(1);
		if (!LineObj0.IsValid() || !LineObj1.IsValid())
			return GetPointLocation(Point);
		FVector Direction1 = GetPointLocation(Point), Direction2 = GetPointLocation(Point);
		Direction1 = LineObj0.Pin()->GetNormal(GetPointLocation(Point));
		// 终止方向向量
		Direction2 = LineObj1.Pin()->GetNormal(GetPointLocation(Point));

		FVector Other1 = LineObj0.Pin()->GetOtherPoint(GetPointLocation(Point));
		FVector Other2 = LineObj1.Pin()->GetOtherPoint(GetPointLocation(Point));

		float r = GetRadius() / 10.f; //半径

									  // 圆心所在向量
		FVector Direction = FArmyMath::GetAngularBisectorDirection(Direction1, Direction2);
		// 交点到圆心的距离
		float Length = 5.f;

		//郭子阳 1.11
		//优化弯头形状
		//Direction1 与Direction 夹角的正弦
		float SinBisectorAngle = (Direction ^ Direction1).Size();
		if (SinBisectorAngle == 0)
		{
			return GetPointLocation(Point);
		}
		//曲率半径
		float CurveRadius = 5;
		if (LineObj1.Pin()->GetLenght()<CurveRadius || LineObj0.Pin()->GetLenght()<CurveRadius)
		{
			return GetPointLocation(Point);
		}

		Length = r / (Direction ^ Direction1).Size() + CurveRadius; //单位：cm
		
		// 圆心位置
		FVector Center = GetPointLocation(Point) + Length * Direction.GetSafeNormal();

		FVector CenterProject1 = FArmyMath::GetProjectionPoint(Center, GetPointLocation(Point), Other1);
		FVector CenterProject2 = FArmyMath::GetProjectionPoint(Center, GetPointLocation(Point), Other2);

		//郭子阳 1.11
		FArmyMath::CreateArc(Center, CenterProject1, CenterProject2, r, Verties, 18);

		FVector StartVec = StartPoint->Location;
		FVector EndVec = EndPoint->Location;
		FVector Location = FArmyMath::GetProjectionPoint(Center, StartVec, EndVec);
		return Location;
	}
	return GetPointLocation(Point);
}


bool FArmyPipeline::IsPointOnLine (TSharedPtr<FArmyPipePoint> Point)
{
	return Point == EndPoint || Point == StartPoint;
}

bool FArmyPipeline::IsIntersect (TSharedPtr<FArmyPipeline>& OtherLine)
{
	if (!OtherLine.IsValid())
		return false;
	if (OtherLine->StartPoint == StartPoint || 
		OtherLine->EndPoint == StartPoint ||
		OtherLine->EndPoint == EndPoint ||
		OtherLine->StartPoint == EndPoint)
		return false;
	if (FArmyMath::AreLinesParallel(GetStart (),GetEnd (),OtherLine->GetStart (),OtherLine->GetEnd ()))
		return false;
	FVector Point;
    float Distance = FArmyMath::DistanceLineToLine(GetStart(),GetEnd(),OtherLine->GetStart(),OtherLine->GetEnd(),Point);
	float Radius = GetRadius ()/10.f+OtherLine->GetRadius ()/10.f;
	return Distance < Radius; /// 2.f//GetRadius() /  10.f + OtherLine->GetRadius() / 10.f;//
}

void FArmyPipeline::DestroyPipelineModel ()
{
	if(PipeLineActor&&PipeLineActor->IsValidLowLevel ())
	{
		PipeLineActor->Destroy ();
		PipeLineActor = NULL;
	}
}

const FVector FArmyPipeline::GetRealStart()
{
	if (StartPoint.IsValid())
	{
		if (StartPoint->PipeLines.Num()==2&&StartPoint->IsElectirc()&&!StartPoint->IsBridgeBend())
		{
			TWeakPtr<FArmyPipeline> LineObj0 = StartPoint->GetLine(0);
			TWeakPtr<FArmyPipeline> LineObj1 = StartPoint->GetLine(1);
			if (!LineObj0.IsValid()||!LineObj1.IsValid())
				return GetPointLocation(StartPoint);
			FVector Direction1 = GetPointLocation(StartPoint),Direction2 = GetPointLocation(StartPoint);
			Direction1 = LineObj0.Pin()->GetNormal(GetPointLocation(StartPoint));
			// 终止方向向量
			Direction2 = LineObj1.Pin()->GetNormal(GetPointLocation(StartPoint));

			FVector Other1 = LineObj0.Pin()->GetOtherPoint(GetPointLocation(StartPoint));
			FVector Other2 = LineObj1.Pin()->GetOtherPoint(GetPointLocation(StartPoint));

			// 圆心所在向量
			FVector Direction = FArmyMath::GetAngularBisectorDirection (Direction1,Direction2);
			// 交点道圆心的距离
			float Length = 5.f;
			// 圆心位置
			FVector Center = GetPointLocation(StartPoint)+Length * Direction.GetSafeNormal();
			FVector StartVec = StartPoint->Location;
			FVector EndVec = EndPoint->Location;
			FVector Location = FArmyMath::GetProjectionPoint(Center,StartVec,EndVec);
			return Location;
		}
	}
	
	return GetStart();
}

const FVector FArmyPipeline::GetRealEnd()
{
	if (EndPoint.IsValid())
	{
		if (EndPoint->PipeLines.Num()==2&&EndPoint->IsElectirc()&&!EndPoint->IsBridgeBend())
		{
			TWeakPtr<FArmyPipeline> LineObj0 = EndPoint->GetLine(0);
			TWeakPtr<FArmyPipeline> LineObj1 = EndPoint->GetLine(1);
			if (!LineObj0.IsValid()||!LineObj1.IsValid())
				return GetPointLocation(EndPoint);
			FVector Direction1 = GetPointLocation(EndPoint),Direction2 = GetPointLocation(EndPoint);
			Direction1 = LineObj0.Pin()->GetNormal(GetPointLocation(EndPoint));
			// 终止方向向量
			Direction2 = LineObj1.Pin()->GetNormal(GetPointLocation(EndPoint));

			FVector Other1 = LineObj0.Pin()->GetOtherPoint(GetPointLocation(EndPoint));
			FVector Other2 = LineObj1.Pin()->GetOtherPoint(GetPointLocation(EndPoint));

			// 圆心所在向量
			FVector Direction = FArmyMath::GetAngularBisectorDirection (Direction1,Direction2);
			// 交点道圆心的距离
			float Length = 5.f;
			// 圆心位置
			FVector Center = GetPointLocation(EndPoint)+Length * Direction.GetSafeNormal();
			FVector StartVec = StartPoint->Location;
			FVector EndVec = EndPoint->Location;
			FVector Location = FArmyMath::GetProjectionPoint(Center,StartVec,EndVec);
			return Location;
		}
	}
	return GetEnd();
}

void FArmyPipeline::SetColor (const FColor InColor)
{
	Color = InColor;
}

void FArmyPipeline::SetStart (const FVector Pos)
{
	if (!StartPoint.IsValid())
		return;
	if(!StartPoint->IsBridgeBend())
	{
		StartPoint->SetLocation (Pos);
	}
	CreateMatrial();
}

void FArmyPipeline::SetEnd (const FVector Pos)
{
	if(!(EndPoint->IsBridgeBend()))
	{
		EndPoint->SetLocation (Pos);
	}
	CreateMatrial ();
}

void FArmyPipeline::SetStartPoint (TSharedPtr<FArmyPipePoint>& InPoint)
{
	StartPoint = InPoint;
}

void FArmyPipeline::SetEndPoint (TSharedPtr<FArmyPipePoint>& InPoint)
{
	EndPoint = InPoint;
}

FVector FArmyPipeline::GetNormal (FVector InStart)
{
	FVector EndVector = GetPointLocation(EndPoint);
	FVector StartVector = GetPointLocation(StartPoint);
	if (InStart.Equals(GetPointLocation(StartPoint)))
	{
		FVector Direction = EndVector - InStart;
		return Direction.GetSafeNormal();
	}
	else if (InStart.Equals(GetPointLocation(EndPoint)))
	{
		FVector Direction=StartVector-InStart;
		return Direction.GetSafeNormal();
	}
	return FVector::ZeroVector;
}

FVector FArmyPipeline::GetOtherPoint (FVector InPoint)
{
	if(InPoint.Equals (GetPointLocation (StartPoint)))
	{
		return GetPointLocation (EndPoint);
	}
	else if(InPoint.Equals (GetPointLocation (EndPoint)))
	{
		return GetPointLocation (StartPoint);
	}
	return FVector::ZeroVector;
}


TSharedPtr<FArmyPipePoint> FArmyPipeline::GetOtherPoint(TSharedPtr<FArmyPipePoint> Point)
{
	if (Point == StartPoint)
		return EndPoint;
	else if (Point == EndPoint)
		return StartPoint;
	return NULL;
}

bool FArmyPipeline::ModifyStartPoint (TSharedPtr<FArmyPipePoint> NewPoint)
{
	TSharedPtr<FArmyPipeline> Line = StaticCastSharedRef<FArmyPipeline>( this->AsShared() );
	StartPoint->RemovePipeLine(Line);
	SetStartPoint(NewPoint);
	NewPoint->AddPipeLine (Line);
	return true;
}

bool FArmyPipeline::ModifyEndPoint (TSharedPtr<FArmyPipePoint> NewPoint)
{
	TSharedPtr<FArmyPipeline> Line = StaticCastSharedRef<FArmyPipeline> (this->AsShared ());
	EndPoint->RemovePipeLine (Line);
	SetEndPoint(NewPoint);
	NewPoint->AddPipeLine(Line);
	this->Modify();
	return true;
}


bool FArmyPipeline::ModifyPoint (TSharedPtr<FArmyPipePoint> OldPoint,TSharedPtr<FArmyPipePoint> NewPoint)
{
	if (OldPoint == StartPoint)
		return ModifyStartPoint(NewPoint);
	else if( OldPoint == EndPoint)
		return ModifyEndPoint(NewPoint);
	return false;
}

void FArmyPipeline::Intersect (TSharedPtr<FArmyPipeline> OtherLine,FVector& OutPoint0,FVector& OutPoint1)
{
	FArmyMath::FootLineToLine(GetStart(),GetEnd(),OtherLine->GetStart(),OtherLine->GetEnd(),OutPoint0,OutPoint1);
}


FVector FArmyPipeline::CrossProduct(TSharedPtr<FArmyPipeline>& Line1,TSharedPtr<FArmyPipeline>& Line2)
{
	FVector Direction1 = (Line1->GetEnd()- Line1->GetStart()).GetSafeNormal();
	FVector Direction2 = (Line2->GetEnd()- Line2->GetStart()).GetSafeNormal();
	return FVector::CrossProduct(Direction1,Direction2);
}

void FArmyPipeline::PostDelete()
{
	//TSharedPtr<FArmyObject> LineObj = this->AsShared();
	//TArray<TSharedPtr<FArmyObject>> DeleteObjectes;
	//DeleteObjectes.AddUnique(LineObj);
	//PostDeleteDelegate.ExecuteIfBound(DeleteObjectes);
	DeleteSelf();
}

void FArmyPipeline::DeleteSelf(bool bTrans)
{
	TArray<FGuid> Points;
	TSharedPtr<FArmyObject> LineObj=this->AsShared();
	TSharedPtr<FArmyPipeline> Line=StaticCastSharedPtr<FArmyPipeline>(LineObj);
	StartPoint->RemovePipeLine(Line);
	if(StartPoint->PipeLines.Num()==0)
	{
		if(Points.Contains(StartPoint->GetUniqueID()))
			Points.Remove(StartPoint->GetUniqueID());
		FArmySceneData::Get()->Delete(StartPoint,true);
	}
	else
		Points.Add(StartPoint->GetUniqueID());
	EndPoint->RemovePipeLine(Line);
	if(EndPoint->PipeLines.Num()==0)
	{
		if(Points.Contains(EndPoint->GetUniqueID()))
			Points.Remove(EndPoint->GetUniqueID());
		FArmySceneData::Get()->Delete(EndPoint,true);
	}
	else
		Points.Add(EndPoint->GetUniqueID());

	for (auto It = AttachmentPoints.CreateIterator();It;++It)
	{
		TSharedPtr<FArmyPipePoint> BridgePoint = *It;
		AttachmentPoints.Remove(BridgePoint);
		FArmySceneData::Get()->Delete(BridgePoint,true);
		--It;
	}

	for (auto It = BridgeInfoes.CreateIterator (); It; ++It)
	{
		TSharedPtr<FBridgeOverlyLineInfo> LineInfo = (*It);
		TSharedPtr<FArmyPipeline> InstentLine = StaticCastSharedPtr<FArmyPipeline>(FArmySceneData::Get()->GetObjectByGuid(E_HydropowerModel,LineInfo->LineID).Pin());
		if (InstentLine.IsValid())
			InstentLine->RemoveOverlayLine(Line);
	}
	
	for(auto PointID:Points)
	{
		TSharedPtr<FArmyPipePoint> Point=StaticCastSharedPtr<FArmyPipePoint>(FArmySceneData::Get()->GetObjectByGuid(E_HydropowerModel,PointID).Pin());
		if(Point.IsValid())
		{
			if(Point->IsElectirc ())
			{
				for(auto line:Point->PipeLines)
				{
					TSharedPtr<FArmyPipeline> LinePtr=Point->GetLine(line).Pin();
					if (LinePtr.IsValid())
						LinePtr->UpdateWithPoints();
				}
			}
			Point->Update(Line);
		}

	}
	DestroyPipelineModel();
}

void FArmyPipeline::ChangeColor()
{

	if(MaterialInsDy&& MaterialInsDy->IsValidLowLevel ())
	{
		FColor CurrentColor =Color;
		if (bReform)
			CurrentColor = RefromColor;
		if (ProductID == -1)
			CurrentColor = FColor::White;
		MaterialInsDy->SetVectorParameterValue(TEXT("MainColor"),FLinearColor::FromSRGBColor(CurrentColor));
		PipeLineActor->SetMaterial (MaterialInsDy);
	}
}

FVector FArmyPipeline::GetDirection ()
{
	FVector Direction = GetEnd() - GetStart();
	return Direction;
}

FVector FArmyPipeline::GetPointLocation (TSharedPtr<FArmyPipePoint> Point)
{
	if (Point.IsValid())
		return Point->GetLocation();
	return FVector::ZeroVector;
}

void FArmyPipeline::SetType (EObjectType InType)
{
	if(InType==OT_StrongElectricity_25||
		InType==OT_StrongElectricity_4||
		InType==OT_StrongElectricity_Single||
		InType==OT_StrongElectricity_Double)
	{
		PipeType = PT_StrongElectricity;
	}
	if(InType==OT_WeakElectricity_TV||
		InType==OT_WeakElectricity_Net||
		InType==OT_WeakElectricity_Phone)
	{
		PipeType = PT_WeakElectricity;
	}
	if (InType == OT_HotWaterTube)
	{
		PipeType = PT_HotWaterTube;
	}
	if (InType == OT_ColdWaterTube)
	{
		PipeType = PT_ColdWaterTube;
	}
	if (InType == OT_Drain)
	{
		PipeType = PT_Drain;
	}
	FArmyObject::SetType(InType);
}


void FArmyPipeline::GetConnectLines(TArray<FObjectPtr>& Lines)
{
	TSharedPtr<FArmyPipeline> thisObj = StaticCastSharedRef<FArmyPipeline>(this->AsShared());
	if (Lines.Contains(thisObj))
		return;
	Lines.AddUnique(this->AsShared());
	if (StartPoint.IsValid())
		StartPoint->GetConnectLines(Lines);
	if (EndPoint.IsValid())
		EndPoint->GetConnectLines(Lines);
}

AActor* FArmyPipeline::GetRelevanceActor()
{
	if(PipeLineActor&&PipeLineActor->IsValidLowLevel())
		return PipeLineActor;
	return nullptr;
}

void FArmyPipeline::DeselectPoints ()
{

}

bool FBridgePointInfo::Contain(TSharedPtr<FBridgeOverlyLineInfo>& LineInfo)
{
	FVector Direction = (Start-End).GetSafeNormal();
	FVector InStart = LineInfoes[0]->IntersectPoint;
	FVector InEnd = LineInfoes.Last()->IntersectPoint;
	return FArmyMath::IsPointInLineSegment3D(LineInfo->IntersectPoint,InStart+15*Direction,InEnd-15*Direction);
}

void FBridgePointInfo::AddBridgeOverlyLineInfo(TSharedPtr<FBridgeOverlyLineInfo>& LineInfo)
{
	LineInfoes.AddUnique(LineInfo);
	LineInfoes.Sort([&](const TSharedPtr<FBridgeOverlyLineInfo>& A,const TSharedPtr<FBridgeOverlyLineInfo>& B){
		FVector Direction1 = (A->IntersectPoint-Start);
		FVector Direction2 = (B->IntersectPoint-Start);
		return Direction1.Size()<Direction2.Size();
	});
}

FVector FBridgePointInfo::GetBottomLocation()
{
	float MaxRaduis = LineInfoes[0]->LineRaduis;
	for (auto& Info:LineInfoes)
	{
		if (MaxRaduis<Info->LineRaduis)
		{
			MaxRaduis = Info->LineRaduis;
		}
	}
	FVector RotateNormal = LineInfoes[0]->Normal;
	FVector NewStart = GetNearlyStart();
	FVector NewEnd = GetNearlyEnd();
	FVector BridgeBottomPoint = (NewStart+NewEnd)/2+RotateNormal * (OverlyLineRadius+MaxRaduis) * 0.1f;
	return BridgeBottomPoint;
}


FVector FBridgePointInfo::GetNearlyStart()
{
	FVector Direction = (Start - End).GetSafeNormal();
	float flag = 0.5f;
	if (LinkerType == OT_ColdWaterTube||
		LinkerType==OT_HotWaterTube||
		LinkerType==OT_Drain)
	{
		flag = 0.25f;
	}
	
	FVector NewStart = LineInfoes[0]->IntersectPoint+Direction*LineInfoes[0]->LineRaduis * flag; // 告警StartPoint的点 
	FVector NDirection = (Start - NewStart).GetSafeNormal();
	float Cos = FVector::DotProduct(Direction,NDirection);
	if (Cos>0)
	{
		return NewStart;
	}
	return Start;
}

FVector FBridgePointInfo::GetNearlyEnd()
{
	FVector Direction = (End-Start).GetSafeNormal();
	int32 LastIndex = LineInfoes.Num() - 1;
	float flag = 0.5f;
	if (LinkerType==OT_ColdWaterTube||
		LinkerType==OT_HotWaterTube||
		LinkerType==OT_Drain)
	{
		flag = 0.25f;
	}

	FVector NewEnd =  LineInfoes[LastIndex]->IntersectPoint+Direction *LineInfoes[LastIndex]->LineRaduis* flag; // 靠近EndPoint的点
	FVector NDirection = (End-NewEnd).GetSafeNormal();
	float Cos = FVector::DotProduct(Direction,NDirection);
	if (Cos)
	{
		return NewEnd;
	}
	return End;
}
