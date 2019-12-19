#include "ArmyPipePoint.h"
#include "ArmyEditPoint.h"
#include "CoreMinimal.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyStyle.h"
#include "ArmyMath.h"
#include "ArmyPipePointActor.h"
#include "SceneManagement.h"
#include "ArmyPipeline.h"
#include "ArmyViewportClient.h"
#include "ArmyConstructionQuantity.h"
#include "ArmySceneData.h"
#include "ArmyActorConstant.h"

using namespace FContentItemSpace;

static uint32 PipePointIndex = 0;
TMap<FString,int32> FArmyPipePoint::PPTypeNameMap;
FArmyPipePoint::FArmyPipePoint ()
	:PipePointType(EPPT_None)
	,PreComponentID(EC_None)
	,BridgeBendRadius (0.f)
	,bReform (false)
{
	SetPropertyFlag(FLAG_COLLISION,true);
	//ObjectName = TEXT("水管");
	SetName (TEXT ("水管")+GetUniqueID().ToString());
	PipePointIndex++;
	ObjectType = OT_WaterLinker;
	PipePointActor = NULL;
	Color = FLinearColor::Red;

	static bool TypeNameInit = false;
	if(!TypeNameInit)
	{
		PPTypeNameMap.Add ("EPPT_None",EPPT_None);
		PPTypeNameMap.Add ("EPPT_Direct",EPPT_Direct);
		PPTypeNameMap.Add ("EPPT_Flexure",EPPT_Flexure);
		PPTypeNameMap.Add ("EPPT_Tee",EPPT_Tee);
		PPTypeNameMap.Add ("EPPT_BridgeBend",EPPT_BridgeBend);
		PPTypeNameMap.Add ("EPPT_DirectE",EPPT_DirectE);
		PPTypeNameMap.Add ("EPPT_DirectBridgeBend",EPPT_DirectBridgeBend);
		TypeNameInit = true;
	}
	UMaterialInstance* MaterialInterface=FArmyEngineModule::Get ().GetEngineResource ()->GetLineMaterialIns ();
	MaterialInsDy=UMaterialInstanceDynamic::Create (MaterialInterface,nullptr);
	if(MaterialInsDy->IsValidLowLevel ())
	{
		MaterialInsDy->AddToRoot ();
	}
}

FArmyPipePoint::FArmyPipePoint (FArmyPipePoint* Copy)
{
	SetPropertyFlag (FLAG_COLLISION,true);
	SetName (GetName ());
	ObjectType = Copy->ObjectType;
	SetLocation(Copy->GetLocation());
	PipePointType = Copy->PipePointType;
	PreComponentID = EC_None;
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.Template = Copy->PipePointActor;
	PipePointActor = GVC->GetWorld ()->SpawnActor<AXRPipePointActor> (SpawnInfo);
	BridgeBendRadius = Copy->BridgeBendRadius;
	PipeLines.Reset();
	PipeLines.Append(Copy->PipeLines);
	PipeLineMap.Reset();
	PipeLineMap.Append(Copy->PipeLineMap);
	MaterialInsDy = Copy->MaterialInsDy;
	
}

FArmyPipePoint::FArmyPipePoint (FVector InLocation)
	:Location(InLocation)
	,PipePointType (EPPT_None)
	,PreComponentID(EC_None)
	,BridgeBendRadius(0.f)
	,bReform(false)
{
	SetPropertyFlag (FLAG_COLLISION,true);
	SetName (GetName ());
	PipePointIndex++;
	ObjectType = OT_WaterLinker;
	PipePointActor = NULL;
	Color = FLinearColor::Red;
	UMaterialInstance* MaterialInterface=FArmyEngineModule::Get ().GetEngineResource ()->GetLineMaterialIns ();
	MaterialInsDy=UMaterialInstanceDynamic::Create (MaterialInterface,nullptr);
	if(MaterialInsDy->IsValidLowLevel ())
	{
		MaterialInsDy->AddToRoot ();
	}
}

FArmyPipePoint::~FArmyPipePoint ()
{
	PipeLineMap.Empty();
	PipeLines.Empty();
	PipeLineInfoMap.Empty();
	if ( PipePointActor &&PipePointActor->IsValidLowLevel())
	{
		PipePointActor->Destroy();
	}
	if(MaterialInsDy->IsValidLowLevel ())
	{
		MaterialInsDy->RemoveFromRoot();
	}
}

void FArmyPipePoint::SerializeToJson (TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson (JsonWriter);
	JsonWriter->WriteValue ("Position",Location.ToString());
	JsonWriter->WriteValue ("Refrom",bReform);

	JsonWriter->WriteValue ("Color",Color.ToString());
	JsonWriter->WriteValue("RefromColor",ReformColor.ToString());
	JsonWriter->WriteValue("ProductID",ProductID);
	switch(ObjectType)
	{
		case OT_StrongElectricity_25_Linker:            //强电2.5
			JsonWriter->WriteValue ("Type",TEXT ("OT_StrongElectricity_25_Linker"));
			break;
		case OT_StrongElectricity_4_Linker:            //强电4
			JsonWriter->WriteValue ("Type",TEXT ("OT_StrongElectricity_4_Linker"));
			break;
		case OT_StrongElectricity_Single_Linker:            //强电单控
			JsonWriter->WriteValue ("Type",TEXT ("OT_StrongElectricity_Single_Linker"));
			break;
		case OT_StrongElectricity_Double_Linker:            //强电双控
			JsonWriter->WriteValue ("Type",TEXT ("OT_StrongElectricity_Double_Linker"));
			break;
		case OT_WeakElectricity_TV_Linker://弱电 电视线
			JsonWriter->WriteValue ("Type",TEXT ("OT_WeakElectricity_TV_Linker"));
			break;
		case OT_WeakElectricity_Net_Linker://弱电， 网线
			JsonWriter->WriteValue ("Type",TEXT ("OT_WeakElectricity_Net_Linker"));
			break;
		case OT_WeakElectricity_Phone_Linker://弱电，电话线
			JsonWriter->WriteValue ("Type",TEXT ("OT_WeakElectricity_Phone_Linker"));
			break;
		case OT_ColdWaterTube_Linker://冷水管
			JsonWriter->WriteValue ("Type",TEXT ("OT_ColdWaterTube_Linker"));
			break;
		case OT_HotWaterTube_Linker://热水管
			JsonWriter->WriteValue ("Type",TEXT ("OT_HotWaterTube_Linker"));
			break;
		case OT_Drain_Linker://排水管
			JsonWriter->WriteValue ("Type",TEXT ("OT_Drain_Linker"));
			break;
		default:
			break;
	}

	switch(PipePointType)
	{
		case EPPT_None:
			JsonWriter->WriteValue ("PPType",TEXT ("EPPT_None"));
			break;
		case EPPT_Direct:
			JsonWriter->WriteValue ("PPType",TEXT ("EPPT_Direct"));
			break;
		case EPPT_Flexure:
			JsonWriter->WriteValue ("PPType",TEXT ("EPPT_Flexure"));
			break;
		case EPPT_Tee:
			JsonWriter->WriteValue ("PPType",TEXT ("EPPT_Tee"));
			break;
		case EPPT_BridgeBend:
			JsonWriter->WriteValue ("PPType",TEXT ("EPPT_BridgeBend"));
			break;
		case EPPT_DirectE:
			JsonWriter->WriteValue ("PPType",TEXT ("EPPT_DirectE"));
			break;
		case EPPT_DirectBridgeBend:
			JsonWriter->WriteValue ("PPType",TEXT ("EPPT_DirectBridgeBend"));
			break;
		default:
			break;
	}

	JsonWriter->WriteArrayStart ("Lines");
	for(auto& LineRef:PipeLineInfoMap)
	{
		JsonWriter->WriteObjectStart();
		TSharedPtr<FPipeLine> LineInfo = LineRef.Value;
		LineInfo->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	// 过桥弯
	JsonWriter->WriteValue("BridgeBendRadius",BridgeBendRadius);
	JsonWriter->WriteValue("BridgeBottomPoint",BridgeBottomPoint.ToString());
	//过桥弯最近点
	//JsonWriter->WriteArrayStart("PipeLineMap");
	//for(auto It = PipeLineMap.CreateConstIterator (); It; ++It)
	//{
	//	FGuid Point = It->Key;
	//	FVector Positon = It->Value;
	//	JsonWriter->WriteObjectStart();
	//	JsonWriter->WriteValue("NeareastPoint",Point.ToString());
	//	JsonWriter->WriteValue("NeareastLocation",Positon.ToString());
	//	JsonWriter->WriteObjectEnd();

	//}
	//JsonWriter->WriteArrayEnd();

    SERIALIZEREGISTERCLASS(JsonWriter, FArmyPipePoint)
}

void FArmyPipePoint::Deserialization (const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization (InJsonData);

	FString PositionStr;
	InJsonData->TryGetStringField ("Position",PositionStr);
	Location.InitFromString(PositionStr);

	InJsonData->TryGetStringField("Color",PositionStr);
	Color.InitFromString(PositionStr);
	InJsonData->TryGetNumberField("ProductID",ProductID);
	InJsonData->TryGetBoolField("Refrom",bReform);
	FString PropertyRes;
	InJsonData->TryGetStringField ("Type",PropertyRes);
	if(PropertyRes.Equals ("OT_StrongElectricity_25_Linker"))
		SetType (OT_StrongElectricity_25_Linker);
	else if(PropertyRes.Equals ("OT_StrongElectricity_4_Linker"))
		SetType (OT_StrongElectricity_4_Linker);
	else if(PropertyRes.Equals ("OT_StrongElectricity_Single_Linker"))
		SetType (OT_StrongElectricity_Single_Linker);
	else if(PropertyRes.Equals ("OT_StrongElectricity_Double_Linker"))
		SetType (OT_StrongElectricity_Double_Linker);
	else if(PropertyRes.Equals ("OT_WeakElectricity_TV_Linker"))
		SetType (OT_WeakElectricity_TV_Linker);
	else if(PropertyRes.Equals ("OT_WeakElectricity_Net_Linker"))
		SetType (OT_WeakElectricity_Net_Linker);
	else if(PropertyRes.Equals ("OT_WeakElectricity_Phone_Linker"))
		SetType (OT_WeakElectricity_Phone_Linker);
	else if(PropertyRes.Equals ("OT_ColdWaterTube_Linker"))
		SetType (OT_ColdWaterTube_Linker);
	else if(PropertyRes.Equals ("OT_HotWaterTube_Linker"))
		SetType (OT_HotWaterTube_Linker);
	else if(PropertyRes.Equals ("OT_Drain_Linker"))
		SetType (OT_Drain_Linker);

	InJsonData->TryGetNumberField ("propertyFlag",PropertyFlag);
	InJsonData->TryGetStringField ("PPType",PropertyRes);
	PipePointType = EPipePointType (PPTypeNameMap.FindRef (PropertyRes));
	BridgeBendRadius = InJsonData->GetNumberField("BridgeBendRadius");
	FString BridgeBottomPointPtr = InJsonData->GetStringField("BridgeBottomPoint");
	BridgeBottomPoint.InitFromString(BridgeBottomPointPtr);
	{
		PipeLines.Empty();
		PipeLineInfoMap.Empty();
		TArray<TSharedPtr<FJsonValue> > JsonArray = InJsonData->GetArrayField ("Lines");
		for(auto V:JsonArray)
		{
			TSharedPtr<FJsonObject> JsonObject = V->AsObject();
			TSharedPtr<FPipeLine> Line = MakeShareable(new FPipeLine);
			Line->Deserialization(JsonObject);
			PipeLines.Add(Line->LineID);
			PipeLineInfoMap.Add(Line->LineID,Line);
		}
	}
	
	/*{
		PipeLineMap.Empty();
		TArray<TSharedPtr<FJsonValue> > JsonArray = InJsonData->GetArrayField ("PipeLineMap");
		for(auto V:JsonArray)
		{
			TSharedPtr<FJsonObject> JsonObject = V->AsObject();
			FGuid PointGuid;
			FGuid::Parse (JsonObject->GetStringField("NeareastPoint"),PointGuid);
			FString Property;
			JsonObject->TryGetStringField ("NeareastLocation",Property);
			FVector location;
			location.InitFromString (Property);
			PipeLineMap.Add(PointGuid,location);
		}
	}*/

}

void FArmyPipePoint::Draw (FPrimitiveDrawInterface* PDI,const FSceneView* View)
{
	if(GetPropertyFlag (FArmyObject::FLAG_VISIBILITY))
	{

	}
}

void FArmyPipePoint::ApplyTransform (const FTransform& Trans)
{
	//SetStart (Trans.TransformPosition (Line->Start));
	//SetEnd (Trans.TransformPosition (Line->End));
}

const FBox FArmyPipePoint::GetBounds ()
{
	//TArray<FVector> Points;
	//Points.Add (Line->Start);
	//Points.Add (Line->End);
	//return FBox (Points);
	return FBox();
}

void FArmyPipePoint::Refresh ()
{

}
//
//bool operator==(const FArmyPipePoint& A,const FArmyPipePoint& B)
//{
//	return A.Location.Equals (B.Location);
//}
//
//bool operator==(TSharedPtr< FArmyPipePoint> const& InSharedPtrA,TSharedPtr< FArmyPipePoint > const& InSharedPtrB)
//{
//	if (InSharedPtrA.IsValid()&& InSharedPtrB.IsValid())
//		return InSharedPtrA->Location.Equals(InSharedPtrB->Location);
//	return false;
//}

bool FArmyPipePoint::IsBridgeBend ()
{
	EPipePointType PPType = GetPPType();
	return PPType== EPPT_BridgeBend ||PPType== EPPT_DirectBridgeBend;
}

const FVector& FArmyPipePoint::GetLocation ( )
{
	return Location;
}

FVector FArmyPipePoint::GetBridgeBendLocation (TSharedPtr<FArmyPipePoint> Point)
{
	if (Point.IsValid())
	{
		FGuid objectID = Point->GetUniqueID();
		if(PipeLineMap.Contains(objectID))
		{
			FVector pLocation=*PipeLineMap.Find(objectID);
			return pLocation;
		}
	}
	return Location;
}

bool FArmyPipePoint::IsCreate ()
{
	return PipePointActor!=NULL&&PipePointActor->IsValidLowLevel();
}

void FArmyPipePoint::GeneratePipePointModel (UWorld* World)
{
	if (PipePointActor == NULL && CanDraw())
	{
		PipePointActor = World->SpawnActor<AXRPipePointActor> (AXRPipePointActor::StaticClass ());
		PipePointActor->SetFolderPath(FArmyActorPath::GetPipelinePath());
		PipePointActor->SetRelevanceObject(this->AsShared());
		PipePointActor->DelteaDelegate.BindRaw(this,&FArmyPipePoint::EditorApplyTranslation);
		PipePointActor->PostEditDelegate.BindRaw (this,&FArmyPipePoint::PostEditPointes);
		PipePointActor->Tags.Add("MoveableMeshActor");
		PipePointActor->Tags.Add("HydropowerPipeActor");
		ChangeColor();
	}
	else if(PipePointActor&&PipePointActor->IsValidLowLevel()&&!CanDraw())
		DestroyPipelineModel();
}

void FArmyPipePoint::EditorApplyTranslation (const FVector& deltaVector)
{
	if(!IsBridgeBend ())
		Location+=deltaVector;
}


void FArmyPipePoint::PostEditPointes (TArray<TSharedPtr<FArmyObject>>& Objectes)
{
	if(PostEditDelegate.IsBound ())
		PostEditDelegate.ExecuteIfBound (Objectes);
}
void FArmyPipePoint::CreateMatrial ()
{
	if (PipePointActor&&PipePointActor->IsValidLowLevel())
	{
		TArray<FVector> Vertirs;
		bool bCanDraw = CanDraw();
		if(bCanDraw)
		{
			if(IsBridgeBend())
			{
				CreateBridgeVertexes(Vertirs);
			}
			else if (PipeLines.Num() >= 2)
			{
				for(int i = 0;i<PipeLines.Num ();i++)
					DrawCylinder(GetLineInfo(PipeLines[i]),Vertirs);
				FArmyMath::DrawWire(Location,GetRadius()/10.f+2*GetScaleClip (),Vertirs);
			}

		}
		PipePointActor->ResetMeshTriangles ();
		if(Vertirs.Num ())
		{
			PipePointActor->AddVetries (Vertirs);
			//PipePointActor->SetActorLocation (Location);
		}
		PipePointActor->UpdateAllVetexBufferIndexBuffer ();
	}
	
}

void FArmyPipePoint::CreateBridgeVertexes(TArray<FVector>& Verties)
{
	DrawBridgeBend(Verties);
	TArray<FVector> Locations;
	PipeLineMap.GenerateValueArray(Locations);
	for (int i = 0;i<Locations.Num ();i++)
		FArmyMath::DrawWire(Locations[i],BridgeBendRadius/10.f*GetScaleClip (),Verties,20);
}

void FArmyPipePoint::DestroyPipelineModel ()
{
	if(PipePointActor->IsValidLowLevel ())
	{
		PipePointActor->Destroy ();
		PipePointActor = NULL;
	}
}

void FArmyPipePoint::AddPipeLine (TWeakPtr<FArmyPipeline> PipeLine)
{
	
	if (!PipeLines.Contains(PipeLine.Pin()->GetUniqueID()))
	{
		PipeLines.Add(PipeLine.Pin()->GetUniqueID()); 
	} 
	{
		bool bHas = PipeLineInfoMap.Contains(PipeLine.Pin()->GetUniqueID());
		if (bHas)
		{
			PipeLineInfoMap[PipeLine.Pin()->GetUniqueID()]->Update(PipeLine.Pin()->GetUniqueID(),PipeLine.Pin()->GetNormal(Location),PipeLine.Pin()->GetRadius(),
						PipeLine.Pin()->GetDiameter(),PipeLine.Pin()->GetPointColor(),
						PipeLine.Pin()->GetPointRefromColor());
		}
		else
		{
			TSharedPtr<FPipeLine> Line = MakeShareable(new FPipeLine(PipeLine.Pin()->GetUniqueID(),PipeLine.Pin()->GetNormal(Location),PipeLine.Pin()->GetRadius(),
						PipeLine.Pin()->GetDiameter(),PipeLine.Pin()->GetPointColor(),
						PipeLine.Pin()->GetPointRefromColor()));
			PipeLineInfoMap.Add(PipeLine.Pin()->GetUniqueID(),Line);
		}
		
	}
}

void FArmyPipePoint::AddBridgeBendPoint (TSharedPtr<FArmyPipePoint> Point,FVector Location1)
{
	if (IsBridgeBend())
	{
		if (PipeLineMap.Contains(Point->GetUniqueID()))
			PipeLineMap.Remove(Point->GetUniqueID());
		PipeLineMap.Add (Point->GetUniqueID(),Location1);
	}
}

void FArmyPipePoint::AddBridgeBendPoint(FGuid PointID,FVector Location)
{
	if (IsBridgeBend())
	{
		if (PipeLineMap.Contains(PointID))
			PipeLineMap.Remove(PointID);
		PipeLineMap.Add (PointID,Location);
	}
}

void FArmyPipePoint::RemovePipeLine (TWeakPtr<FArmyPipeline> PipeLine)
{
	PipeLineInfoMap.Remove(PipeLine.Pin()->GetUniqueID());
	if (PipeLines.Contains(PipeLine.Pin()->GetUniqueID()))
	{
		PipeLines.Remove(PipeLine.Pin()->GetUniqueID());
		PipeLineMap.Empty ();
		if(PipeLineInfoMap.Num ()==1)
		{
			PipePointType = EPPT_None;
		}
		UpdateInfo();
		
	}
}

void FArmyPipePoint::UpdateLineInfo(TWeakPtr<FArmyPipeline> PipeLine)
{
	TSharedPtr<FArmyPipeline> Line = PipeLine.Pin();
	if (PipeLineInfoMap.Contains(Line->GetUniqueID()))
	{
		PipeLineInfoMap[Line->GetUniqueID()]->Update(Line->GetUniqueID(),Line->GetNormal(Location),Line->GetRadius(),
			Line->GetDiameter(),Line->GetPointColor(),
			Line->GetPointRefromColor());
	}
}

void FArmyPipePoint::Update(TWeakPtr<FArmyPipeline> PipeLine)
{
	UpdateLineInfo(PipeLine);
	UpdateInfo();
	CreateMatrial();
}

void FArmyPipePoint::UpdateInfo()
{
	UpdatePPType();
	UpdateRadius();
	UpdateName();
	if(PreComponentID!=UpdateComponentID()&&CanDraw())
	{
		int32 ComponentID=UpdateComponentID();
		PreComponentID=ComponentID;
		if (PipeLines.Num()>1 ||PipePointType==EPPT_BridgeBend)
		{
			FString DiaStr = GetDiamterStr();
			ProductID = -1;
			ChangeColor();
			UpdateInfoDelegate.ExecuteIfBound(ComponentID,DiaStr,GetUniqueID().ToString());
		}

	}
	GeneratePipePointModel(GVC->GetWorld());
}

void FArmyPipePoint::UpdatePPType ()
{
	if (IsBridgeBend())
		return;
	PrePipePointType = PipePointType;
	if (PipeLines.Num() == 1)
	{
		PipePointType = EPPT_None;
	}
	else
	{
		if(!IsElectirc ())
		{
			if(GetPPType ()!=EPPT_BridgeBend)
			{
				if(PipeLines.Num ()==3)
					PipePointType = EPPT_Tee;
				else if(PipeLines.Num ()==2)
				{
					FVector Direct0 = GetLineInfo(PipeLines[0])->LineDirection;
					FVector Direct1 = GetLineInfo(PipeLines[1])->LineDirection;
					float value =FMath::Abs(FVector::DotProduct(Direct0,Direct1));
					if( FMath::IsNearlyEqual(value,1.f,0.01f) )
						PipePointType = EPPT_Direct;
					else
						PipePointType = EPPT_Flexure;
				}
				else
					PipePointType = EPPT_None;
			}
		}
		else
		{
			if(GetPPType ()!=EPPT_DirectBridgeBend)
				PipePointType = EPPT_DirectE;
		}
	}
	
	
}

FString FArmyPipePoint::GetDiamterStr()
{
	FString DiaStr;
	if(PipeLines.Num()>1)
	{
		int32 ARadius=GetLineInfo(PipeLines[0])->Diameter;
		int32 BRadius= GetLineInfo(PipeLines[1])->Diameter;
		bSameDiameter=BRadius==ARadius;
		bool bIsSan=PipeLineInfoMap.Num()==3;
		if(bIsSan)
		{
			int32 CRadius= GetLineInfo(PipeLines[2])->Diameter;
			bSameDiameter=BRadius==ARadius&&ARadius==CRadius&&BRadius==CRadius;
		}
		if(bSameDiameter)
		{
			DiaStr=FString::FromInt(ARadius);
		}
		else
		{
			TArray<int32> Diamerters;
			for(int32 i=0;i<PipeLines.Num();i++)
			{
				int32 Radius= GetLineInfo(PipeLines[i])->Diameter;
				Diamerters.Add(Radius);
			}
			Diamerters.Sort();
			DiaStr = FString::FromInt(Diamerters[0]);
			for (int32 i = 1;i<Diamerters.Num();i++)
			{
				FString TempStr =FString::Printf (TEXT ("*%d"),Diamerters[i]);
				DiaStr += TempStr;
			}
		}
	}
	else if (PipePointType == EPPT_BridgeBend)
	{
		DiaStr = FString::FromInt(BridgeBendRadius*2);
	}
	return DiaStr;
}

void FArmyPipePoint::UpdateName(FString InName /*= ""*/)
{
	if (InName.IsEmpty())
	{
		TMap<EObjectType,FString> NameMap ;// {TEXT("热水管"),TEXT ("冷水管"),TEXT ("排水管")};
		NameMap.Add(OT_HotWaterTube_Linker,TEXT ("给水管"));
		NameMap.Add (OT_ColdWaterTube_Linker,TEXT ("给水管"));
		NameMap.Add (OT_Drain_Linker,TEXT ("排水管"));

		TMap<bool,FString> SameMap;
		SameMap.Add(true,TEXT("同径"));
		SameMap.Add(false,TEXT ("异径"));
		
		switch(PipePointType)
		{
			case EPPT_BridgeBend: //过桥弯
			{
				FString Name = *NameMap.Find(GetType()) + TEXT("过桥弯");
				SetName(Name);
			}
			break;
			case EPPT_Direct: //直接
			{
				FString Name0 = *SameMap.Find(bSameDiameter);
				FString Name1 = *NameMap.Find (GetType ());
				FString Name = Name0 +Name1+TEXT ("直接");
				SetName (Name);
			}
			break;
			case EPPT_Flexure://弯通（算量）
			{
				FString Name0 = *SameMap.Find (bSameDiameter);
				FString Name1 = *NameMap.Find (GetType ());
				FString Name = Name0+Name1+TEXT ("弯头");
				SetName (Name);
			}
			break;
			case EPPT_Tee://三通（算量）
			{
				FString Name0 = *SameMap.Find (bSameDiameter);
				FString Name1 = *NameMap.Find (GetType ());
				int32 ComponentID = UpdateComponentID();

				switch(ComponentID)
				{
					case EC_Supply_Same_Tee://给水管等径正三通
						SetName(TEXT("给水管等径正三通"));
						break;
					case EC_Supply_Diff_Tee://给水管异径正三通
						SetName(TEXT("给水管异径正三通"));
						break;
					case EC_Supply_Same_45_Tee://给水管等径45斜三通
						SetName(TEXT("给水管等径45°斜三通"));
						break;
					case EC_Supply_Diff_45_Tee://给水管异径45斜三通
						SetName(TEXT("给水管异径45°斜三通"));
						break;
					case EC_Supply_NoPlane_Same_Tee: //给水管立体等径三通
						SetName(TEXT("给水管立体等径三通"));
						break;
					case EC_Supply_NoPlane_Diff_Tee: //给水管立体异径三通
						SetName(TEXT("给水管立体异径三通"));
						break;
					case EC_Drain_Bottle_Tee:// 排水管瓶型三通
						SetName(TEXT("排水管瓶型三通"));
						break;
					case EC_Drain_Diff_Tee: //排水管异径正三通
						SetName(TEXT("排水管异径正三通"));
						break;
					case EC_Drain_Same_Tee: //排水管等径正三通
						SetName(TEXT("排水管等径正三通"));
						break;
					case EC_Drain_Same_45_Tee: //排水管等径45°斜三通
						SetName(TEXT("排水管等径45°斜三通"));
						break;
					case EC_Drain_Diff_45_Tee: //排水管异径45°斜三通
						SetName(TEXT("排水管异径45°斜三通"));
						break;
					default:
						break;
				}

	/*			FString Name = Name0+Name1+TEXT ("三通");
				SetName(Name);*/
			}
			break;
			default:
				RadiusStr = "";
				break;
		}
	}
	else
		SetName(InName);
	if (PipePointActor&&PipePointActor->IsValidLowLevel())
		PipePointActor->SetActorLabel(InName);
}

void FArmyPipePoint::UpdateRadius ()
{

	switch (PipePointType)
	{
		case EPPT_BridgeBend: //过桥弯
		{
			RadiusStr = FString::FromInt(BridgeBendRadius);
		}
		break;
		case EPPT_Direct: //直接
		case EPPT_Flexure ://弯通（算量）
		{
			float ARadius= GetLineInfo(PipeLines[0])->Diameter;
			float BRadius= GetLineInfo(PipeLines[1])->Diameter;
			bSameDiameter = FMath::IsNearlyEqual (BRadius,ARadius);
			if(bSameDiameter)
			{
				RadiusStr = FString::FromInt (ARadius);
			}
			else
			{
				RadiusStr = FString::Printf (TEXT ("%.fx%.f"),ARadius,BRadius);
			}
		}
		break;
		case EPPT_Tee ://三通（算量）
		{
			float ARadius = GetLineInfo(PipeLines[0])->Diameter;
			float BRadius = GetLineInfo(PipeLines[1])->Diameter;
			float CRadius = GetLineInfo(PipeLines[2])->Diameter;
			bSameDiameter = FMath::IsNearlyEqual (BRadius,ARadius) &&FMath::IsNearlyEqual (ARadius,CRadius)&&FMath::IsNearlyEqual (BRadius,CRadius);
			if(bSameDiameter)
			{
				RadiusStr = FString::FromInt (ARadius);
			}
			else
			{
				RadiusStr = FString::Printf (TEXT ("%.fx%.fx%.f"),ARadius,BRadius,CRadius);
			}
		}
		break;
		default:
			RadiusStr = "";
			break;
	}	
}

bool FArmyPipePoint::CanDraw()
{
	bool bObjectCan = (ObjectType==OT_HotWaterTube_Linker || ObjectType == OT_ColdWaterTube_Linker || ObjectType == OT_Drain_Linker || ObjectType == EPPT_BridgeBend);
	bool bPPType = (PipePointType == EPPT_Direct || PipePointType == EPPT_Flexure ||PipePointType == EPPT_Tee || PipePointType == EPPT_BridgeBend || PipePointType == EPPT_DirectBridgeBend); 
	return bObjectCan && bPPType;
}



int32 FArmyPipePoint::UpdateComponentID()
{
	int32 ComponentID = EC_Supply_Diff_Direct;
	switch(PipePointType)
	{
		case EPPT_BridgeBend: //过桥弯
		{
			ComponentID = EC_Bridge_Same;
		}
		break;
		case EPPT_Direct: //直接
		{
			if (PipeLines.Num() ==2)
			{
				float Radius1 = GetLineInfo(PipeLines[0])->Diameter;
				float Radius2 = GetLineInfo(PipeLines[1])->Diameter;
				if (FMath::IsNearlyEqual(Radius1,Radius2))
				{
					if(ObjectType==OT_ColdWaterTube_Linker || 
						ObjectType == OT_HotWaterTube_Linker)
					{
						ComponentID = EC_Supply_Same_Direct;
					}
					else if (ObjectType == OT_Drain_Linker)
					{
						ComponentID = EC_Drain_Same_Direct;
					}
				}
				else
				{
					if(ObjectType==OT_ColdWaterTube_Linker||
						ObjectType==OT_HotWaterTube_Linker)
					{
						ComponentID=EC_Supply_Diff_Direct;
					}
					else if(ObjectType==OT_Drain_Linker)
					{
						ComponentID=EC_Drain_Diff_Direct;
					}
				}
			}
		}
		break;
		case EPPT_Flexure://弯通（算量）
		{
			if(PipeLines.Num()==2)
			{
				float Radius1= GetLineInfo(PipeLines[0])->Diameter;
				float Radius2= GetLineInfo(PipeLines[1])->Diameter;
				FVector Direction1= GetLineInfo(PipeLines[0])->LineDirection;
				FVector Direction2= GetLineInfo(PipeLines[1])->LineDirection;
				float dot = FVector::DotProduct(Direction1,Direction2);

				switch(ObjectType)
				{
					case OT_ColdWaterTube_Linker:
					case OT_HotWaterTube_Linker:
					{
						if(FMath::IsNearlyEqual(Radius1,Radius2))
						{
							if (FMath::IsNearlyZero(dot,0.001f))
								ComponentID = EC_Supply_Same_90_Flexure;
							else
								ComponentID = EC_Supply_Same_45_Flexure;
						}
						else
						{
							if(FMath::IsNearlyZero(dot,0.001f))
								ComponentID=EC_Supply_Diff_90_Flexure;
							else
								ComponentID=EC_Supply_Diff_45_Flexure;
						}
					}
					break;
					case OT_Drain_Linker:
					{
						if(FMath::IsNearlyEqual(Radius1,Radius2))
						{
							if(FMath::IsNearlyZero(dot,0.001f))
								ComponentID=EC_Drain_Same_90_Flexure;
							else
								ComponentID=EC_Drain_Same_45_Flexure;
						}
						else
						{
							if(FMath::IsNearlyZero(dot,0.001f))
								ComponentID=EC_Drain_Diff_90_Flexure;
							else
								ComponentID=EC_Drain_Diff_45_Flexure;
						}
					}
					break;
					default:
						break;
				}				
			}
		}
		break;
		case EPPT_Tee://三通（算量）
		{
			if(PipeLines.Num()==3)
			{
				float Radius1= GetLineInfo(PipeLines[0])->Diameter;
				float Radius2= GetLineInfo(PipeLines[1])->Diameter;
				float Radius3= GetLineInfo(PipeLines[2])->Diameter;
				FVector Direction1=GetLineInfo(PipeLines[0])->LineDirection;
				FVector Direction2=GetLineInfo(PipeLines[1])->LineDirection;
				FVector Direction3=GetLineInfo(PipeLines[2])->LineDirection;
				float dot1=FVector::DotProduct(Direction1,Direction2);
				float dot2=FVector::DotProduct(Direction1,Direction3);
				float dot3=FVector::DotProduct(Direction3,Direction2);

				FVector Normal1=FVector::CrossProduct(Direction2,Direction1).GetSafeNormal();
				FVector Normal2=FVector::CrossProduct(Direction3,Direction1).GetSafeNormal();
				FVector Normal3=FVector::CrossProduct(Direction2,Direction3).GetSafeNormal();
				FVector Direct3Location = Location +Direction3*2;
				bool bPlane = FPlane::PointPlaneDist(Direct3Location,Location,Normal1) < 0.001f;
				if (bSameDiameter) //判断等径
				{
					if ( bPlane ) //判断是否同平面
					{
						if ((FMath::IsNearlyZero(dot1,0.001f)&&FMath::IsNearlyZero(dot2,0.001f))||
							(FMath::IsNearlyZero(dot3,0.001f)&&FMath::IsNearlyZero(dot2,0.001f))||
							(FMath::IsNearlyZero(dot1,0.001f)&&FMath::IsNearlyZero(dot3,0.001f))) //判断是是等径90°三通
						{
							switch(ObjectType)
							{
								case OT_ColdWaterTube_Linker:
								case OT_HotWaterTube_Linker:
								{
									ComponentID = EC_Supply_Same_Tee;
								}
								break;
								case OT_Drain_Linker:
								{
									ComponentID = EC_Drain_Same_Tee;
								}
								break;
							}
						}
						else   //等径 45°三通
						{
							switch(ObjectType)
							{
								case OT_ColdWaterTube_Linker:
								case OT_HotWaterTube_Linker:
								{
									ComponentID=EC_Supply_Same_45_Tee;
								}
								break;
								case OT_Drain_Linker:
								{
									ComponentID=EC_Drain_Same_45_Tee;
								}
								break;
							}
						}
					}
					else //非同面等径三通
					{
						switch(ObjectType)
						{
							case OT_ColdWaterTube_Linker:
							case OT_HotWaterTube_Linker:
							{
								ComponentID=EC_Supply_NoPlane_Same_Tee;
							}
							break;
							case OT_Drain_Linker:
							{
								ComponentID=EC_Drain_Bottle_Tee;
							}
							break;
						}
					}
				}
				else
				{
					if(bPlane) //判断是否同平面
					{
						if ((FMath::IsNearlyZero(dot1,0.001f)&&FMath::IsNearlyZero(dot2,0.001f))||
							(FMath::IsNearlyZero(dot3,0.001f)&&FMath::IsNearlyZero(dot2,0.001f))||
							(FMath::IsNearlyZero(dot1,0.001f)&&FMath::IsNearlyZero(dot3,0.001f))) //判断是是不同径90°三通
						{
							switch(ObjectType)
							{
								case OT_ColdWaterTube_Linker:
								case OT_HotWaterTube_Linker:
								{
									ComponentID=EC_Supply_Diff_Tee;
								}
								break;
								case OT_Drain_Linker:
								{
									ComponentID=EC_Drain_Diff_Tee;
								}
								break;
							}
						}
						else   //异径 45°三通
						{
							switch(ObjectType)
							{
								case OT_ColdWaterTube_Linker:
								case OT_HotWaterTube_Linker:
								{
									ComponentID=EC_Supply_Diff_45_Tee;
								}
								break;
								case OT_Drain_Linker:
								{
									ComponentID=EC_Drain_Diff_45_Tee;
								}
								break;
							}
						}
					}
					else //非同面异径三通
					{
						switch(ObjectType)
						{
							case OT_ColdWaterTube_Linker:
							case OT_HotWaterTube_Linker:
							{
								ComponentID=EC_Supply_NoPlane_Diff_Tee;
							}
							break;
							case OT_Drain_Linker:
							{
								ComponentID=EC_Drain_Bottle_Tee;
							}
							break;
						}
					}
				}

			}
		}
		break;
		case EPPT_Trap:
		{
		}
		break;
		default:
			break;
	}
	return ComponentID;
}

void FArmyPipePoint::GetConnectLines(TArray<FObjectPtr>& Lines)
{
	TArray<TSharedPtr<FArmyPipeline>> TempLines;
	for (auto& Line : Lines)
	{
		FGuid LineId = Line->GetUniqueID();
		for (auto& LineID : PipeLines)
		{
			if (LineID == LineId)
				continue;
            TWeakPtr<FArmyPipeline> Line = GetLine(LineID);
            if (Line.IsValid())
                TempLines.Add(Line.Pin());
		}
	}
	if (PipePointActor && PipePointActor->IsValidLowLevel())
		Lines.AddUnique(this->AsShared());
	if (TempLines.Num() == 0)
		return;
	for (int32 i = 0 ;i<TempLines.Num();i++)
	{
		TSharedPtr<FArmyPipeline> Line = TempLines[i];
		Line->GetConnectLines(Lines);
	}
}

void FArmyPipePoint::MergeOtherPoint (TSharedPtr<FArmyPipePoint> OtherPoint)
{
	TSharedPtr<FArmyPipePoint>  thisPtr = StaticCastSharedRef<FArmyPipePoint>(this->AsShared());
	for (int32 i = 0 ;i<OtherPoint->PipeLines.Num();i++)
	{
		TWeakPtr<FArmyPipeline> LineWPtr = OtherPoint->GetLine(i);
		TSharedPtr<FArmyPipeline> line = LineWPtr.Pin();
		if (line->ModifyPoint(OtherPoint,thisPtr))
			i--;
	}
	for (auto Line : PipeLines)
	{
		TWeakPtr<FArmyPipeline> LineWPtr = GetLine(Line);
		if (LineWPtr.IsValid())
		{
			LineWPtr.Pin()->UpdateWithPoints();
		}
	}
}

void FArmyPipePoint::DrawBridgeBend (TArray<FVector>& Verties)
{
	float r = (BridgeBendRadius/10.f); //半径

	 

	TArray<FVector> Locations;
	PipeLineMap.GenerateValueArray(Locations);

	if (Locations.Num() == 0)
		return;
	
	FVector BridgeBendLeftPosition = Locations[0];
	FVector BridgeBendRightPosition = Locations[1];
	FVector BridgeBendMiddlePosition = ( BridgeBendRightPosition + BridgeBendLeftPosition ) / 2;
	
	
	if ( ArcType == AT_Single )
	{
		FVector Center;
		double RotationRadius;

		bool bCircle = FArmyMath::Circle3D(BridgeBendLeftPosition,BridgeBendRightPosition,BridgeBottomPoint,Center,RotationRadius);
		FPlane plane(BridgeBendLeftPosition,BridgeBendRightPosition,BridgeBottomPoint);
		FVector CircleNormal = plane.GetSafeNormal();
		if (!bCircle)
			return; 
		FArmyMath::CreateArc(Center,BridgeBendLeftPosition,BridgeBendRightPosition,r,Verties,4);
	}
	else if(ArcType == AT_Mutil)
	{
		{
			FPlane plane(BridgeBendLeftPosition,BridgeBendRightPosition,BridgeBottomPoint);
			FVector PlaneNormal = plane.GetSafeNormal();
			FVector DirectionLeftRightDirection = (BridgeBendLeftPosition-BridgeBendRightPosition).GetSafeNormal();
			FVector DirectionRightLeftDirection = (BridgeBendRightPosition-BridgeBendLeftPosition).GetSafeNormal();
			FVector DirectionMiddleBottom = BridgeBottomPoint-BridgeBendMiddlePosition;
			float Height = DirectionMiddleBottom.Size()+3*r;
			float Angle = 20.f;
			float SideSize = Height/FMath::Cos(Angle / 180.f *PI);
			FVector Direction1 = DirectionLeftRightDirection.RotateAngleAxis(180-Angle,-PlaneNormal).GetSafeNormal();
			FVector Direction2 = DirectionRightLeftDirection.RotateAngleAxis(180-Angle,PlaneNormal).GetSafeNormal();
			FVector Start = BridgeBendLeftPosition + SideSize*Direction1;
			FVector End = BridgeBendRightPosition+SideSize*Direction2;
			FVector Direction = (End-Start).GetSafeNormal();

			FArmyMath::DrawCylinder(BridgeBendLeftPosition,Start,Direction,r,Verties);
			FArmyMath::DrawCylinder(Start,End,r,Verties);
			FArmyMath::DrawCylinder(End,BridgeBendRightPosition,Direction,r,Verties);
		}
		

	}

	
}

void FArmyPipePoint::DrawCylinder (TSharedPtr<FPipeLine> PipeLine,TArray<FVector>& Verties)
{
	FVector Direction1 = FVector::ZeroVector;
	FVector Start = FVector::ZeroVector;
	if (IsBridgeBend())
	{

	}
	else
	{
		Direction1 = PipeLine->LineDirection.GetSafeNormal();
		Start = Location;
	}
		
	
	FVector End = Start+ Direction1 * 20 *GetScaleClip();
	FVector DirectVector = End-Start;
	float halfLength = DirectVector.Size ()/2;
	FVector DirectNormal = DirectVector.GetSafeNormal (); // 单位法线
	FVector U (DirectNormal.Y,-DirectNormal.X,0); // 圆所在平面上的向量
	if(FVector::DistSquaredXY (U,FVector::ZeroVector)==0) //如果y*y + x*x ==0换个向量
		U = FVector (DirectNormal.Z,0,0);
	FVector UN = U.GetSafeNormal ();
	float r = PipeLine->Radius/10.f+2*GetScaleClip (); //半径
	float Angle = 360;
	int32 Count = 16; // 圆弧分成2

	float theta = Angle/Count;
	FVector Location1 = (Start+End)/2;
	for(int32 i = 0;i<Count;i++)
	{
		FVector StartDirt = UN.RotateAngleAxis (theta * i,DirectNormal).GetSafeNormal ();
		FVector Start1 = Location1+StartDirt* r;
		FVector EndDirt = UN.RotateAngleAxis (theta * (i+1),DirectNormal).GetSafeNormal ();
		FVector End1 = Location1+EndDirt* r;
		FVector SP0 = Start1-halfLength*DirectNormal;
		FVector EP0 = End1-halfLength*DirectNormal;
		FVector SP1 = Start1+halfLength*DirectNormal;
		FVector EP1 = End1+halfLength*DirectNormal;
		Verties.Push (SP0);
		Verties.Push (SP1);
		Verties.Push (EP1);

		Verties.Push (SP0);
		Verties.Push (EP1);
		Verties.Push (EP0);
	}
	
}

float FArmyPipePoint::GetRadius ()
{
	float max = 0.f;
	for (auto& LineInfoItem : PipeLineInfoMap)
	{
		TSharedPtr<FPipeLine> LineInfo = LineInfoItem.Value;
		max = max < LineInfo->Radius ? LineInfo->Radius : max; 
	}
	return max;
}

void FArmyPipePoint::SetBridgeBendRadius (float Radius)
{
	BridgeBendRadius = Radius;
	CreateMatrial();
}

FLinearColor FArmyPipePoint::GetColor ()
{
	return Color;
}

float FArmyPipePoint::GetScaleClip ()
{
	if (GetRadius())
		return GetRadius ()/110.f;
	return 1.f;
}

void FArmyPipePoint::SetColor (FColor InColor)
{
	Color = FLinearColor::FromSRGBColor(InColor) ;
}

bool FArmyPipePoint::IsElectirc ()
{
	if( ObjectType == OT_ColdWaterTube_Linker ||
		ObjectType == OT_HotWaterTube_Linker ||
		ObjectType == OT_Drain_Linker ||
		ObjectType == OT_Drain_Point)
	{
		return false;
	}
	return true;
}

bool FArmyPipePoint::CanLink ()
{
	if (IsElectirc())
		return PipeLineInfoMap.Num() == 1 && GetPPType() != EPPT_DirectBridgeBend;
	else
		return PipeLineInfoMap.Num() > 0 &&PipeLineInfoMap.Num() < 3;
	return false;
}

void FArmyPipePoint::ChangeColor()
{
	if(PipePointActor&&PipePointActor->IsValidLowLevel()&&MaterialInsDy&& MaterialInsDy->IsValidLowLevel ())
	{
		FLinearColor CurrentColor=Color;
		if ( ProductID == -1 )
			CurrentColor = FColor::White;
		else
			CurrentColor = Color;
		if (bReform)
			CurrentColor = ReformColor;
		FColor fColor = CurrentColor.ToFColor(false);
		MaterialInsDy->SetVectorParameterValue(TEXT("MainColor"),CurrentColor);
		PipePointActor->SetMaterial (MaterialInsDy);
	}
}

TWeakPtr<FArmyPipeline> FArmyPipePoint::GetLine(FGuid& objectID)
{
	return StaticCastSharedPtr<FArmyPipeline>(FArmySceneData::Get()->GetObjectByGuid(E_HydropowerModel,objectID).Pin());
}

TWeakPtr<FArmyPipeline> FArmyPipePoint::GetLine(int32 _index)
{
	FGuid _objectID = PipeLines[_index];
	return GetLine(_objectID);
}

TSharedPtr<FPipeLine> FArmyPipePoint::GetLineInfo(FGuid& LineID)
{
	return PipeLineInfoMap[LineID];
}

void FArmyPipePoint::GetPipeLines(TArray<TWeakPtr<FArmyPipeline>>& OutLines)
{
	for (int32 i = 0 ;i<PipeLines.Num();i++)
	{
		OutLines.AddUnique(GetLine(i));
	}
}

AActor* FArmyPipePoint::GetRelevanceActor()
{
	if(PipePointActor&&PipePointActor->IsValidLowLevel())
		return PipePointActor;
	return nullptr;
}

void FPipeLine::Update(TSharedPtr<FArmyPipeline>& Line)
{
	LineID = Line->GetUniqueID();
	LineDirection = Line->GetDirection();
	Radius = Line->GetRadius();
	Diameter = Line->GetDiameter();
	NormalColor = Line->GetPointColor();
	ReformColor = Line->GetPointRefromColor();
}

void FPipeLine::SerializeToJson(TSharedRef<TJsonWriter<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	JsonWriter->WriteValue ("LineID",LineID.ToString());
	JsonWriter->WriteValue ("LineDirection",LineDirection.ToString());
	JsonWriter->WriteValue ("Radius",Radius);
	JsonWriter->WriteValue("Diameter",Diameter);
	JsonWriter->WriteValue("NormalColor",NormalColor.ToString());
	JsonWriter->WriteValue("ReformColor",ReformColor.ToString());
}

void FPipeLine::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FString PropertyStr;
	InJsonData->TryGetStringField ("LineID",PropertyStr);
	FGuid::Parse(PropertyStr,LineID);
	InJsonData->TryGetStringField ("LineDirection",PropertyStr);
	LineDirection.InitFromString(PropertyStr);

	InJsonData->TryGetStringField("NormalColor",PropertyStr);
	NormalColor.InitFromString(PropertyStr);

	InJsonData->TryGetStringField("ReformColor",PropertyStr);
	ReformColor.InitFromString(PropertyStr);

	InJsonData->TryGetNumberField("Radius",Radius);
	InJsonData->TryGetNumberField("Diameter",Diameter);
}
