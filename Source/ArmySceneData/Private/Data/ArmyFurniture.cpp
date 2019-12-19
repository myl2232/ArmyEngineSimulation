#include "ArmyFurniture.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "ArmyEditPoint.h"
#include "ArmyPolyline.h"
#include "BatchedElements.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyFurnitureActor.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/BodySetup.h"
#include "ArmySceneData.h"
#include "ArmyMath.h"
#include "ArmyRulerLine.h"
#include "ArmyRoom.h"
#include "ArmyRectArea.h"
#include "ArmyCircleArea.h"
#include "ArmyCommonTypes.h"
#include "ArmyViewportClient.h"
#include "ArmyGameInstance.h"
#include "../Plugins/2D/Paper2D/Source/Paper2D/Classes/PaperSpriteComponent.h"
#include "../Plugins/2D/Paper2D/Source/Paper2D/Classes/PaperSprite.h"
#include "ArmyEditorEngine.h"
#include "ArmyActor/XRShapeBoardActor.h"
#include "ArmyResourceModule.h"

TMap<FString, int32> FArmyFurniture::TypeNameMap;
FArmyFurniture::FArmyFurniture() :

	PivotPoint(FVector::ZeroVector)
	, FurnitureId(0)
	, bReform(false)
	, ComponentType(OT_None)
	, NowLocatoin(FVector::ZeroVector)
	, bTransverse(false)
	, bUseCustomColor(false)
	, CustomoColor(FLinearColor::Red)
	, eSockType(ESF_Normal)
	, FirstRulerLine(MakeShareable(new FArmyRulerLine()))
	, SecondRulerLine(MakeShareable(new FArmyRulerLine()))
{
	FurniturePro = MakeShareable<FFurnitureProperty>(new FFurnitureProperty(EResourceType::Hydropower));
	FurniturePro->SetModeType(EModeIdent::MI_HydropowerMode);
	SetState(OS_Normal);
	LocalTransform = FTransform::Identity;
	FurnitureModelActor = NULL;
	BeamLength = 100;
	BeamWidth = 40;
	BeamThickness = 40;
	BeamModel = NULL;

	FirstRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyFurniture::OnFirstInputBoxCommitted));
	SecondRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyFurniture::OnSecondInputBoxCommitted));

	static bool TypeNameInit = false;
	if (!TypeNameInit)
	{
		TypeNameMap.Add("OT_Component_None", OT_Component_None);
		TypeNameMap.Add("OT_ComponentBase", OT_ComponentBase);
		TypeNameMap.Add("OT_Socket_Point", OT_Socket_Point);
		TypeNameMap.Add("OT_Socket_Five_Point", OT_Socket_Five_Point);
		TypeNameMap.Add("OT_Socket_Five_On_Point", OT_Socket_Five_On_Point);
		TypeNameMap.Add("OT_Socket_Three_On_Point", OT_Socket_Three_On_Point);
		TypeNameMap.Add("OT_Socket_Four_Point", OT_Socket_Four_Point);
		TypeNameMap.Add("OT_Socket_Four_On_Point", OT_Socket_Four_On_Point);
		TypeNameMap.Add("OT_Socket_IT", OT_Socket_IT);
		TypeNameMap.Add("OT_Socket_IT_TV", OT_Socket_IT_TV);
		TypeNameMap.Add("OT_Socket_TP", OT_Socket_TP);
		TypeNameMap.Add("OT_Socket_TV", OT_Socket_TV);
		TypeNameMap.Add("OT_EleBoxL_Point", OT_EleBoxL_Point);
		TypeNameMap.Add("OT_EleBoxH_Point", OT_EleBoxH_Point);
		TypeNameMap.Add("OT_ElectricWire", OT_ElectricWire);
		TypeNameMap.Add("OT_Switch3O2_Point", OT_Switch3O2_Point);
		TypeNameMap.Add("OT_Switch3O1_Point", OT_Switch3O1_Point);
		TypeNameMap.Add("OT_Switch2O2_Point", OT_Switch2O2_Point);
		TypeNameMap.Add("OT_Switch2O1_Point", OT_Switch2O1_Point);
		TypeNameMap.Add("OT_Switch1O2_Point", OT_Switch1O2_Point);
		TypeNameMap.Add("OT_Switch1O1_Point", OT_Switch1O1_Point);
		TypeNameMap.Add("OT_Lamp_Pendant_Point", OT_Lamp_Pendant_Point);
		TypeNameMap.Add("OT_Lamp_Flower_Point", OT_Lamp_Flower_Point);
		TypeNameMap.Add("OT_Lamp_Down_Point", OT_Lamp_Down_Point);
		TypeNameMap.Add("OT_Lamp_Ceiling_Point", OT_Lamp_Ceiling_Point);
		TypeNameMap.Add("OT_Water_Hot_Point", OT_Water_Hot_Point);
		TypeNameMap.Add("OT_Water_Chilled_Point", OT_Water_Chilled_Point);
		TypeNameMap.Add("OT_FloorDrain_Point", OT_FloorDrain_Point);
		TypeNameMap.Add("OT_Dewatering_Point", OT_Dewatering_Point);
		TypeNameMap.Add("OT_Furniture_TVStand", OT_Furniture_TVStand);
		TypeNameMap.Add("OT_Furniture_TV", OT_Furniture_TV);
		TypeNameMap.Add("OT_Furniture_TeaTable", OT_Furniture_TeaTable);
		TypeNameMap.Add("OT_Furniture_SquattingPan", OT_Furniture_SquattingPan);
		TypeNameMap.Add("OT_Furniture_SofaSet", OT_Furniture_SofaSet);
		TypeNameMap.Add("OT_Furniture_Sofas", OT_Furniture_Sofas);
		TypeNameMap.Add("OT_Furniture_Sink", OT_Furniture_Sink);
		TypeNameMap.Add("OT_Furniture_SingleBed", OT_Furniture_SingleBed);
		TypeNameMap.Add("OT_Furniture_ShowerHead", OT_Furniture_ShowerHead);
		TypeNameMap.Add("OT_Furniture_PedestalPan", OT_Furniture_PedestalPan);
		TypeNameMap.Add("OT_Furniture_FloorCabinet", OT_Furniture_FloorCabinet);
		TypeNameMap.Add("OT_Furniture_DoubleSofa", OT_Furniture_DoubleSofa);
		TypeNameMap.Add("OT_Furniture_DoubleBed18X20", OT_Furniture_DoubleBed18X20);
		TypeNameMap.Add("OT_Furniture_DoubleBed15X19", OT_Furniture_DoubleBed15X19);
		TypeNameMap.Add("OT_Furniture_DiningTable", OT_Furniture_DiningTable);
		TypeNameMap.Add("OT_Furniture_CommonCabinet", OT_Furniture_CommonCabinet);
		TypeNameMap.Add("OT_Furniture_Closet", OT_Furniture_Closet);
		TypeNameMap.Add("OT_Furniture_Chair", OT_Furniture_Chair);
		TypeNameMap.Add("OT_Furniture_Botany", OT_Furniture_Botany);
		TypeNameMap.Add("OT_Furniture_BedSideTable", OT_Furniture_BedSideTable);
		TypeNameMap.Add("OT_Furniture_BedChair", OT_Furniture_BedChair);
		TypeNameMap.Add("OT_Furniture_Bathtub", OT_Furniture_Bathtub);
		TypeNameMap.Add("OT_Furniture_BathroomCabinet", OT_Furniture_BathroomCabinet);
		TypeNameMap.Add("OT_Furniture_Armchair", OT_Furniture_Armchair);
		TypeNameMap.Add("OT_Funiture_GasAppliance", OT_Funiture_GasAppliance);
		TypeNameMap.Add("OT_Furniture_CasualSofa", OT_Furniture_CasualSofa);
		TypeNameMap.Add("OT_Furniture_CornerTable", OT_Furniture_CornerTable);
		TypeNameMap.Add("OT_Furniture_AirConditioner", OT_Furniture_AirConditioner);
		TypeNameMap.Add("OT_Furniture_Curtains", OT_Furniture_Curtains);
		TypeNameMap.Add("OT_Furniture_WritingDesk", OT_Furniture_WritingDesk);
		TypeNameMap.Add("OT_Furniture_WritingChair", OT_Furniture_WritingChair);
		TypeNameMap.Add("OT_Furniture_SingleSink", OT_Furniture_SingleSink);
		TypeNameMap.Add("OT_Furniture_Fridge", OT_Furniture_Fridge);
		TypeNameMap.Add("OT_Furniture_WashingMachine", OT_Furniture_WashingMachine);
		TypeNameMap.Add("OT_OtherComponent", OT_OtherComponent);
		TypeNameMap.Add("OT_Gas_Meter", OT_Gas_Meter);
		TypeNameMap.Add("OT_Gas_MainPipe", OT_Gas_MainPipe);
		TypeNameMap.Add("OT_CurtainBox", OT_CurtainBox);
		TypeNameMap.Add("OT_Water_Normal_Point", OT_Water_Normal_Point);
		TypeNameMap.Add("OT_Water_Separator_Point", OT_Water_Separator_Point);
		TypeNameMap.Add("OT_Basin", OT_Basin);
		TypeNameMap.Add("OT_EleBoxL_Point", OT_EleBoxL_Point);
		/**
		* 水路点位特殊标记，其他是OT_None
		*/
		//TypeNameMap.Add("OT_Switch", OT_Switch);
		/*TypeNameMap.Add("OT_Socket_Strong", OT_Socket_Strong);
		TypeNameMap.Add("OT_EleBox_Point", OT_EleBox_Point);
		TypeNameMap.Add("OT_Socket_Weak", OT_Socket_Weak);
		TypeNameMap.Add("OT_Water_Point", OT_Water_Point);
		TypeNameMap.Add("OT_Drain_Point", OT_Drain_Point);
		TypeNameMap.Add("OT_Closestool", OT_Closestool);*/

		TypeNameInit = true;
	}
}

FArmyFurniture::~FArmyFurniture()
{

}

void FArmyFurniture::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);

	switch (ObjectType)
	{
	case OT_Component_None:
		JsonWriter->WriteValue("Type", TEXT("OT_Component_None"));
		break;

	case OT_ComponentBase:
		JsonWriter->WriteValue("Type", TEXT("OT_ComponentBase"));
		break;
	case OT_Socket_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_Socket_Point"));
		break;
	case OT_Socket_Five_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_Socket_Five_Point"));
		break;
	case OT_Socket_Five_On_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_Socket_Five_On_Point"));
		break;
	case OT_Socket_Three_On_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_Socket_Three_On_Point"));
		break;
	case OT_Socket_Four_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_Socket_Four_Point")); break;
	case OT_Socket_Four_On_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_Socket_Four_On_Point")); break;
	case OT_Socket_IT:
		JsonWriter->WriteValue("Type", TEXT("OT_Socket_IT")); break;
	case OT_Socket_IT_TV:
		JsonWriter->WriteValue("Type", TEXT("OT_Socket_IT_TV")); break;
	case OT_Socket_TP:
		JsonWriter->WriteValue("Type", TEXT("OT_Socket_TP")); break;
	case OT_Socket_TV:
		JsonWriter->WriteValue("Type", TEXT("OT_Socket_TV")); break;
	case OT_EleBoxL_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_EleBoxL_Point")); break;
	case OT_EleBoxH_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_EleBoxH_Point")); break;
	case OT_ElectricWire:
		JsonWriter->WriteValue("Type", TEXT("OT_ElectricWire")); break;
	case OT_Switch3O2_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_Switch3O2_Point")); break;
	case OT_Switch3O1_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_Switch3O1_Point")); break;
	case OT_Switch2O2_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_Switch2O2_Point")); break;
	case OT_Switch2O1_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_Switch2O1_Point")); break;
	case OT_Switch1O2_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_Switch1O2_Point")); break;
	case OT_Switch1O1_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_Switch1O1_Point")); break;
	case OT_Lamp_Pendant_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_Lamp_Pendant_Point")); break;
	case OT_Lamp_Flower_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_Lamp_Flower_Point")); break;
	case OT_Lamp_Down_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_Lamp_Down_Point")); break;
	case OT_Lamp_Ceiling_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_Lamp_Ceiling_Point")); break;
	case OT_Water_Hot_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_Water_Hot_Point")); break;
	case OT_Water_Chilled_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_Water_Chilled_Point")); break;
	case OT_FloorDrain_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_FloorDrain_Point")); break;
	case OT_Dewatering_Point:
		JsonWriter->WriteValue("Type", TEXT("OT_Dewatering_Point")); break;
	case OT_Furniture_TVStand:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_TVStand")); break;
	case OT_Furniture_TV:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_TV")); break;
	case OT_Furniture_TeaTable:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_TeaTable")); break;
	case OT_Furniture_SquattingPan:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_SquattingPan")); break;
	case OT_Furniture_SofaSet:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_SofaSet")); break;
	case OT_Furniture_Sofas:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_Sofas")); break;
	case OT_Furniture_Sink:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_Sink")); break;
	case OT_Furniture_SingleBed:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_SingleBed")); break;
	case OT_Furniture_ShowerHead:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_ShowerHead")); break;
	case OT_Furniture_PedestalPan:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_PedestalPan")); break;
	case OT_Furniture_FloorCabinet:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_FloorCabinet")); break;
	case OT_Furniture_DoubleSofa:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_DoubleSofa")); break;
	case OT_Furniture_DoubleBed18X20:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_DoubleBed18X20")); break;
	case OT_Furniture_DoubleBed15X19:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_DoubleBed15X19")); break;
	case OT_Furniture_DiningTable:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_DiningTable")); break;
	case OT_Furniture_CommonCabinet:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_CommonCabinet")); break;
	case OT_Furniture_Closet:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_Closet")); break;
	case OT_Furniture_Chair:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_Chair")); break;
	case OT_Furniture_Botany:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_Botany")); break;
	case OT_Furniture_BedSideTable:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_BedSideTable")); break;
	case OT_Furniture_BedChair:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_BedChair")); break;
	case OT_Furniture_Bathtub:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_Bathtub")); break;
	case OT_Furniture_BathroomCabinet:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_BathroomCabinet")); break;
	case OT_Furniture_Armchair:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_Armchair")); break;
	case OT_Funiture_GasAppliance:
		JsonWriter->WriteValue("Type", TEXT("OT_Funiture_GasAppliance")); break;
	case OT_Furniture_CasualSofa:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_CasualSofa")); break;
	case OT_Furniture_CornerTable:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_CornerTable")); break;
	case OT_Furniture_AirConditioner:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_AirConditioner")); break;
	case OT_Furniture_Curtains:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_Curtains")); break;
	case OT_Furniture_WritingDesk:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_WritingDesk")); break;
	case OT_Furniture_WritingChair:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_WritingChair")); break;
	case OT_Furniture_SingleSink:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_SingleSink")); break;
	case OT_Furniture_Fridge:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_Fridge")); break;
	case OT_Furniture_WashingMachine:
		JsonWriter->WriteValue("Type", TEXT("OT_Furniture_WashingMachine")); break;
	case OT_OtherComponent:
		JsonWriter->WriteValue("Type", TEXT("OT_OtherComponent")); break;
	case OT_CurtainBox:
		JsonWriter->WriteValue("Type", TEXT("OT_CurtainBox")); break;
	case OT_Component_End:
		JsonWriter->WriteValue("Type", TEXT("OT_Component_End")); break;
	case OT_Space:
		JsonWriter->WriteValue("Type", TEXT("OT_Space")); break;
	default:
		break;
	}

	JsonWriter->WriteValue("ComponentType", (int32)ComponentType);
	JsonWriter->WriteValue("BelongClass", (int32)BelongClass);
	JsonWriter->WriteValue("Refrom", bReform);

	JsonWriter->WriteValue("escocktype", (int32)eSockType);

	//JsonWriter->WriteValue("Altitude", Altitude);
	/*JsonWriter->WriteValue("PointPositionLength", PointPositionLength);
	JsonWriter->WriteValue("PointPositionWidth", PointPositionWidth);
	JsonWriter->WriteValue("PointPositionHeigth", PointPositionHeigth);
	JsonWriter->WriteValue("PointPositionAltitude", PointPositionAltitude);*/
	//JsonWriter->WriteValue("PipeRadius", PipeRadius);

	JsonWriter->WriteValue("bUseCustomColor", bUseCustomColor);
	JsonWriter->WriteValue("CustomoColor", CustomoColor.ToString());

	JsonWriter->WriteValue("LocalTransform", LocalTransform.ToString());

	//@郭子阳 输出构件的坐标，供AI使用，无需反序列化
	JsonWriter->WriteValue("Position", LocalTransform.GetLocation().ToString());

	JsonWriter->WriteValue("EPlaceLocationFlag", (int32)EPlaceLocationFlag);

	JsonWriter->WriteArrayStart("Altitudes");
	for (auto& L : Altitudes)
	{
		JsonWriter->WriteValue(L);
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteArrayStart("PolyLines");
	for (auto& L : Polys)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteArrayStart("Vertices");
		for (auto& V : L->GetAllVertices())
		{
			JsonWriter->WriteValue(V.ToString());
		}
		JsonWriter->WriteArrayEnd();

		JsonWriter->WriteValue("ELineType", L->GetLineType());
		JsonWriter->WriteValue("BasePoint", L->GetBasePoint().ToString());
		JsonWriter->WriteValue("Transform", L->GetTransform().ToString());
		JsonWriter->WriteValue("LineWidth", L->GetLineWidth());
		JsonWriter->WriteValue("Color", L->GetColor().ToString());
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteArrayStart("Points");
	for (auto& P : Points)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("Sprite", P->bSprite);
		JsonWriter->WriteValue("Pos", P->Pos.ToString());
		JsonWriter->WriteValue("PivotPosition", P->PivotPosition.ToString());
		JsonWriter->WriteValue("PropertyFlag", int32(P->PropertyFlag));
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteArrayStart("RelatedFurnitureMap");
	for (auto It = RelatedFurnitureMap.CreateIterator(); It; ++It)
	{
		int32 switchIndex = It->Key;
		FGuid ObjectId = It->Value;
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("index", switchIndex);
		JsonWriter->WriteValue("objectId", ObjectId.ToString());
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteArrayStart("RelatedExtrusionLightMap");
	for (auto It = RelatedExtrusionLightMap.CreateIterator(); It; ++It)
	{
		int32 switchIndex = It->Key;
		FGuid ObjectId = It->Value;
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("index", switchIndex);
		JsonWriter->WriteValue("objectId", ObjectId.ToString());
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	

	JsonWriter->WriteValue(TEXT("SwitchCoupletNum"), SwitchCoupletNum);

	//断点改造相关

	//断点改造类型
	JsonWriter->WriteValue<int32>(TEXT("BRReformType"), (int32)BreakPointType);
	//是否有过替换
	bool isreplaced = IsReplaced();
	JsonWriter->WriteValue<bool>(TEXT("IsReplaced"), isreplaced);
	//替换后的构件
	if (RelativeFurniture.IsValid())
	{
		JsonWriter->WriteValue<FString>(TEXT("SubstituteID"), RelativeFurniture.Pin()->ObjID.ToString());
	}
	//停用
	JsonWriter->WriteValue<bool>(TEXT("IsStopped"), this->IsBPStopped());
	//断点改造
	JsonWriter->WriteValue<bool>(TEXT("IsReforming"), bIsBPreforming);

	//原始位置
	JsonWriter->WriteValue<bool>(TEXT("IsNativePosSet"), isNativeLocationSet);
	JsonWriter->WriteValue<FString>(TEXT("NativePos"), this->NativeLocation.ToString());

	//断点改造结束

	FurniturePro->SerializeToJson(JsonWriter);
	SERIALIZEREGISTERCLASS(JsonWriter, FArmyFurniture)
}

void FArmyFurniture::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);

	const TArray< TSharedPtr<FJsonValue> > PolyLinesJson = InJsonData->GetArrayField("PolyLines");
	for (auto& L : PolyLinesJson)
	{
		const TSharedPtr<FJsonObject> LJsonObj = L->AsObject();
		TSharedPtr<FArmyPolyline> PolyLine = MakeShareable(new FArmyPolyline);
		const TArray< TSharedPtr<FJsonValue> > VJsonArray = LJsonObj->GetArrayField("Vertices");
		for (auto& V : VJsonArray)
		{
			FVector PV;
			PV.InitFromString(V->AsString());
			PolyLine->AddVertice(PV);
		}
		PolyLine->SetLineType(FArmyPolyline::ELineType(LJsonObj->GetIntegerField("ELineType")));
		FVector PV;
		PV.InitFromString(LJsonObj->GetStringField("BasePoint"));
		PolyLine->SetBasePoint(PV);
		FTransform Trans;
		Trans.InitFromString(LJsonObj->GetStringField("Transform"));
		PolyLine->SetTransform(Trans);
		PolyLine->SetLineWidth(LJsonObj->GetNumberField("LineWidth"));
		FLinearColor color;
		color.InitFromString(LJsonObj->GetStringField("Color"));
		PolyLine->SetColor(color);
		AddPoly(PolyLine);
	}

	const TArray< TSharedPtr<FJsonValue> > PointsJson = InJsonData->GetArrayField("Points");
	for (auto& P : PointsJson)
	{
		TSharedPtr<FArmyPoint> Point = MakeShareable(new FArmyPoint);

		TSharedPtr<FJsonObject>PjsonObj = P->AsObject();

		Point->bSprite = PjsonObj->GetBoolField("Sprite");
		FVector V;
		V.InitFromString(PjsonObj->GetStringField("Pos"));
		Point->Pos = V;
		V.InitFromString(PjsonObj->GetStringField("PivotPosition"));
		Point->PivotPosition = V;
		Point->PropertyFlag = PjsonObj->GetIntegerField("PropertyFlag");
		AddPoint(Point);
	}
	InitOperationPoint();

	FString TypeStr;
	InJsonData->TryGetStringField("Type", TypeStr);
	ObjectType = EObjectType(TypeNameMap.FindRef(TypeStr));
	InJsonData->TryGetNumberField("BelongClass", BelongClass);
	FString ComponentTypeStr;
	InJsonData->TryGetStringField("ComponentType", ComponentTypeStr);
	ComponentType = EObjectType(InJsonData->GetIntegerField("ComponentType"));
	InJsonData->TryGetBoolField("Refrom", bReform);
	//Altitude = InJsonData->GetNumberField("Altitude");
	FString TransformStr;
	InJsonData->TryGetStringField("LocalTransform", TransformStr);
	FTransform TempTransForm;
	TempTransForm.InitFromString(TransformStr);
	SetTransform(TempTransForm);

	eSockType = (ESocketFlag)InJsonData->GetIntegerField("escocktype");
	/*PointPositionLength = InJsonData->GetNumberField("PointPositionLength");
	PointPositionWidth = InJsonData->GetNumberField("PointPositionWidth");
	PointPositionHeigth = InJsonData->GetNumberField("PointPositionHeigth");
	PointPositionAltitude = InJsonData->GetNumberField("PointPositionAltitude");*/
	//PipeRadius = InJsonData->GetNumberField("PipeRadius");

	EPlaceLocationFlag = (EPointLocationFlag)InJsonData->GetIntegerField("EPlaceLocationFlag");

	bUseCustomColor = InJsonData->GetBoolField("bUseCustomColor");
	CustomoColor.InitFromString(InJsonData->GetStringField("CustomoColor"));

	Altitudes.Empty();
	const TArray< TSharedPtr<FJsonValue> > AltitudesJson = InJsonData->GetArrayField("Altitudes");
	for (int32 i = 0; i < AltitudesJson.Num(); i++)
	{
		Altitudes.Add(AltitudesJson[i]->AsNumber());
	}

	RelatedFurnitureMap.Empty();
	const TArray< TSharedPtr<FJsonValue> > RelatedFurnitureMapJson = InJsonData->GetArrayField("RelatedFurnitureMap");
	for (auto& L : RelatedFurnitureMapJson)
	{
		const TSharedPtr<FJsonObject> LJsonObj = L->AsObject();
		int32 index = LJsonObj->GetNumberField("index");
		FString guid = LJsonObj->GetStringField("objectId");
		FGuid objectID;
		FGuid::Parse(guid, objectID);
		RelatedFurnitureMap.Add(index, objectID);
	}

	const TArray< TSharedPtr<FJsonValue> >* RelatedExtrusionLightMapJson=nullptr;
	if (InJsonData->TryGetArrayField("RelatedExtrusionLightMap", RelatedExtrusionLightMapJson))
		//const TArray< TSharedPtr<FJsonValue> > RelatedFurnitureMapJson = InJsonData->GetArrayField("RelatedFurnitureMap");
	{
		for (auto& L : *RelatedExtrusionLightMapJson)
		{
			const TSharedPtr<FJsonObject> LJsonObj = L->AsObject();
			int32 index = LJsonObj->GetNumberField("index");
			FString guid = LJsonObj->GetStringField("objectId");
			FGuid objectID;
			FGuid::Parse(guid, objectID);
			RelatedExtrusionLightMap.Add(index, objectID);
		}
	}

	SwitchCoupletNum = InJsonData->GetNumberField(TEXT("SwitchCoupletNum"));

	//断点改造相关

	//断点改造类型
	////JsonWriter->WriteValue<int32>(TEXT("BRReformType"), (int32)BreakPointType);
	
	int32 BreakPointTypeNumber;
	//是不是大于1.2的版本
	bool NewPlan = InJsonData->TryGetNumberField(TEXT("BRReformType"), BreakPointTypeNumber);
	
	if (NewPlan)
	{
		BreakPointType = (EBreakPointType)BreakPointTypeNumber;

		//停用
		this->StopUsing = InJsonData->GetBoolField(TEXT("IsStopped"));
		//断点改造
		this->bIsBPreforming = InJsonData->GetBoolField(TEXT("IsReforming"));

		//是否有过替换
		bisReplaced = InJsonData->GetBoolField(TEXT("IsReplaced"));

		//设置替换构件
		if (bisReplaced)
		{
			FGuid Relative;
			FGuid::Parse(InJsonData->GetStringField(TEXT("SubstituteID")), Relative);

			TWeakPtr<FArmyObject> TempRelativeFurniture = nullptr;
			if (BreakPointType == NativePoint)
			{
				TempRelativeFurniture = FArmySceneData::Get()->GetObjectByGuid(EModelType::E_LayoutModel, Relative);
			}
			else if (BreakPointType == SubstitutePoint)
			{
				TempRelativeFurniture = FArmySceneData::Get()->GetObjectByGuid(EModelType::E_LayoutModel, Relative);
			}
			if (TempRelativeFurniture.IsValid())
			{
				//与之关联的furnature
				TWeakPtr<FArmyFurniture> furniture = StaticCastSharedRef<FArmyFurniture>(TempRelativeFurniture.Pin().Get()->AsShared());
				RelativeFurniture = furniture;
				furniture.Pin()->RelativeFurniture = StaticCastSharedRef<FArmyFurniture>(this->AsShared());
			}
		}

		//原始位置
		isNativeLocationSet = InJsonData->GetBoolField(TEXT("IsNativePosSet"));
		this->NativeLocation.InitFromString(InJsonData->GetStringField(TEXT("NativePos")));

		//断点改造结束
	}
	FurniturePro->Deserialization(InJsonData);

	CalcRelatedRoom();
}


void FArmyFurniture::Generate(UWorld* InWorld)
{

}

void FArmyFurniture::Destroy()
{
    if (BeamModel->IsValidLowLevel())
    {
        BeamModel->Destroy();
        BeamModel = NULL;
    }
    if (FurnitureModelActor->IsValidLowLevel())
    {
        FurnitureModelActor->Destroy();
        FurnitureModelActor = NULL;
    }

	if (!GEngine &&HaveMoveHandle)
	{
		GEngine->OnActorMoved().Remove(MoveHandle);
	}
}

//////////////////////////////////////////////////////////////////////////
FVector FArmyFurniture::Vertexe(const FVector& InV, bool Invert) const
{
	if (Invert)
	{
		return LocalTransform.InverseTransformPosition(InV) + PivotPoint;
	}
	else
	{
		return LocalTransform.TransformPosition(InV - PivotPoint);
	}
}

void FArmyFurniture::UpdateSimpleElementVertex()
{
	LineVertices.Empty();
	for (auto iter : Polys)
	{
		TArray<FVector>& vertices = iter->GetAllVertices();
		if (iter->GetLineType() == FArmyPolyline::LineStrip)
		{

			for (int i = 1; i < vertices.Num(); i++)
			{
				FSimpleElementVertex tempVertex;
				if (bUseCustomColor)
				{
					tempVertex.Color = CustomoColor;
				}
				else
				{
					tempVertex.Color = iter->GetBaseColor();
				}
				tempVertex.TextureCoordinate = FVector2D::ZeroVector;
				tempVertex.Position = Vertexe(vertices[i - 1]);
				LineVertices.Push(tempVertex);
				tempVertex.Position = Vertexe(vertices[i]);
				LineVertices.Push(tempVertex);
			}
		}
		else if (iter->GetLineType() == FArmyPolyline::Lines)
		{
			for (int i = 1; i < vertices.Num(); i += 2)
			{
				FSimpleElementVertex tempVertex;
				if (bUseCustomColor)
				{
					tempVertex.Color = CustomoColor;
				}
				else
				{
					tempVertex.Color = iter->GetBaseColor();
				}
				tempVertex.TextureCoordinate = FVector2D::ZeroVector;
				tempVertex.Position = Vertexe(vertices[i - 1]);
				LineVertices.Push(tempVertex);
				tempVertex.Position = Vertexe(vertices[i]);
				LineVertices.Push(tempVertex);
			}
		}

	}
}

bool FArmyFurniture::Culler(const FSceneView* InView)
{
	FVector2D minObject, maxObject;
	InView->WorldToPixel(Vertexe(BondingBox.Min), minObject);
	InView->WorldToPixel(Vertexe(BondingBox.Max), maxObject);
	FBox2D ObjectBox(minObject, maxObject);
	FBox2D ViewBox(InView->ViewRect.Min, InView->ViewRect.Max);

	if (ViewBox.Intersect(ObjectBox))
		return true;
	return false;
}

void FArmyFurniture::GetRelatedLine(TSharedPtr<FArmyLine> & OutLine)
{
	if (RelatedRoom.IsValid() && BaseEditPoint.IsValid())
	{
		TArray<TSharedPtr<FArmyLine>> Lines;
		RelatedRoom.Pin()->GetLines(Lines);
		//计算点所在的线
		for (auto & It : Lines)
		{
			if (It.IsValid() && FArmyMath::IsPointOnLine(BaseEditPoint->GetPos(), It->GetStart(), It->GetEnd()))
			{
				OutLine = It;
				break;
			}
		}
	}
}


void FArmyFurniture::ModifyFurniturePro_LayoutData(uint8 InType, float InValue)
{
	//找到对应的拆改模式的Furniture
	if (FArmySceneData::Get()->GetObjectByGuid(E_HomeModel, GetUniqueID()).Pin().IsValid())
	{
		TArray<TWeakPtr<FArmyObject>> CompoentList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_ComponentBase, CompoentList);
		for (auto It : CompoentList)
		{
			TSharedPtr<FArmyFurniture> ModifyFurniture = StaticCastSharedPtr<FArmyFurniture>(It.Pin());
			if (ModifyFurniture.IsValid())
			{
				if (PivotPoint.Equals(ModifyFurniture->GetPivortPoint()))
				{
					TSharedPtr<FFurnitureProperty> CurrentPro = ModifyFurniture->GetFurniturePro();
					if (CurrentPro.IsValid())
					{
						TSharedPtr<FArmyFurniture> LayoutFurniture = StaticCastSharedPtr<FArmyFurniture>
							(FArmySceneData::Get()->GetObjectByGuid(E_LayoutModel, ModifyFurniture->GetUniqueID()).Pin());
						if (LayoutFurniture.IsValid())
						{
							switch (InType)
							{
							case 0:
								CurrentPro->SetLength(InValue);
								LayoutFurniture->GetFurniturePro()->SetLength(InValue);
								break;
							case 1:
								CurrentPro->SetWidth(InValue);
								LayoutFurniture->GetFurniturePro()->SetWidth(InValue);
								break;
							case 2:
								CurrentPro->SetHeight(InValue);
								LayoutFurniture->GetFurniturePro()->SetHeight(InValue);
								break;
							case 3:
								CurrentPro->SetAltitude(InValue);
								LayoutFurniture->GetFurniturePro()->SetAltitude(InValue);
								break;
							case 4:
								CurrentPro->SetRadius(InValue);
								LayoutFurniture->GetFurniturePro()->SetRadius(InValue);
								break;
							default:
								break;
							}

							FArmySceneData::Get()->GeneratePointPositionModel();
							break;
						}
							
					}
				}
			}
		}
	}
}

bool FArmyFurniture::ModifyRelatedActorTransform(AActor * _RelatedActor)
{
	AActor * TempRelatedActor = RelevanceActor && RelevanceActor->IsValidLowLevel() ? RelevanceActor : _RelatedActor;
	if (TempRelatedActor && TempRelatedActor->IsValidLowLevel())
	{
		//构件的变换矩阵
		FVector Trans = LocalTransform.GetLocation();
		FRotator Rot = LocalTransform.Rotator();
		FVector Scale = LocalTransform.GetScale3D();

		Trans.Z = 0;
		Trans.Z += (FurniturePro->GetAltitude());
		//点位模型的位置与点位保持一致
		TempRelatedActor->SetActorLocation(Trans);

		//创建的Actor按照属性值的长宽高进行缩放
		FVector ProScale;
		FVector ProBox;
		ProBox.X = FurniturePro->GetLength() > 0 ? FurniturePro->GetLength() : 0;
		ProBox.Y = FurniturePro->GetWidth() > 0 ? FurniturePro->GetWidth() : 0;
		ProBox.Z = FurniturePro->GetHeight() > 0 ? FurniturePro->GetHeight() : 0;

		ProBox = Rot.RotateVector(ProBox);

		FBox FurActorBox = GVC->GetActorCollisionBox(TempRelatedActor);
		FVector Temp = FurActorBox.GetSize();

		ProScale.X = FMath::Abs(ProBox.X) != 0 ? FMath::Abs(ProBox.X) / Temp.X : 1;
		ProScale.Y = FMath::Abs(ProBox.Y) != 0 ? FMath::Abs(ProBox.Y) / Temp.Y : 1;
		ProScale.Z = FMath::Abs(ProBox.Z) != 0 ? FMath::Abs(ProBox.Z) / Temp.Z : 1;

		//缩放和旋转根据属性值的变化进行修改
		//FurActor->SetActorRotation(ProRot);
		TempRelatedActor->SetActorScale3D(ProScale);
		return true;
	}
	return false;
}

bool FArmyFurniture::ModifyRelatedFurnitureActorTransform()
{
	AActor * PipeActor = nullptr;
	TSharedPtr<FArmyPipeline> PipeLine;
	for (auto& ObjectPtr : Children)
	{
		if (ObjectPtr->GetType() >= OT_Preload_Begin && ObjectPtr->GetType() <= OT_Preload_End)
		{
			PipeLine = StaticCastSharedPtr<FArmyPipeline>(ObjectPtr);
			PipeActor = PipeLine->GetRelevanceActor();
		}
	}

	if (PipeActor && PipeActor->IsValidLowLevel())
	{
		//构件的变换矩阵
		FVector Trans = LocalTransform.GetLocation();

		Trans.Z = 0;
		FVector EndTrans = Trans;
		EndTrans.Z += (FurniturePro->GetHeight() > 0 ? FurniturePro->GetHeight() : 0) * 10;

		PipeLine->StartPoint->Location = Trans + (FurniturePro->GetAltitude() > 0 ? FurniturePro->GetAltitude() : 0) * 10;
		PipeLine->EndPoint->Location = EndTrans;
		PipeLine->UpdateWithPoints(true);
		return true;
	}
	return false;
}

bool FArmyFurniture::PreCalTransformIllegal(FTransform trans, int32 Flag)
{
	for (auto PIt : BaseEditPoints)
	{
		FVector PrePos = Vertexe(PIt.Value->GetPos(), true);
		PIt.Value->SetPos(PrePos);
	}
	LocalTransform.Accumulate(trans);
	for (auto PIt : BaseEditPoints)
	{
		FVector PrePos = Vertexe(PIt.Value->GetPos());
		PIt.Value->SetPos(PrePos);
	}

	//@ 如果变换后构件超出所在房间，则变换无效，恢复到变换前
	if (RelatedRoom.IsValid() && EPlaceLocationFlag == EPLF_ROOMSPACE)
	{
		const FBox ComBox = GetBounds();
		FVector Center = ComBox.GetCenter();
		FVector BoxSize = ComBox.GetSize();
		FVector LeftTop(Center.X - BoxSize.X / 2, Center.Y - BoxSize.Y / 2, 0);
		FVector RightTop(Center.X + BoxSize.X / 2, Center.Y - BoxSize.Y / 2, 0);
		FVector RightBottom(Center.X + BoxSize.X / 2, Center.Y + BoxSize.Y / 2, 0);
		FVector LeftBottom(Center.X - BoxSize.X / 2, Center.Y + BoxSize.Y / 2, 0);

		//放置在空间的点位，变换时外包几何不能超出所在房间
		if (!RelatedRoom.Pin()->IsPointInRoom(LeftTop)
			|| !RelatedRoom.Pin()->IsPointInRoom(RightTop)
			|| !RelatedRoom.Pin()->IsPointInRoom(RightBottom)
			|| !RelatedRoom.Pin()->IsPointInRoom(LeftBottom))
		{
			for (auto PIt : BaseEditPoints)
			{
				FVector PrePos = Vertexe(PIt.Value->GetPos(), true);
				PIt.Value->SetPos(PrePos);
			}
			LocalTransform.Accumulate(trans.Inverse());
			for (auto PIt : BaseEditPoints)
			{
				FVector PrePos = Vertexe(PIt.Value->GetPos());
				PIt.Value->SetPos(PrePos);
			}
			GGI->Window->ShowMessage(MT_Warning, TEXT("输入的数值超出范围"));
			return true;
		}
	}

	//恢复数据，保证数据跟传入状态一致
	for (auto PIt : BaseEditPoints)
	{
		FVector PrePos = Vertexe(PIt.Value->GetPos(), true);
		PIt.Value->SetPos(PrePos);
	}
	LocalTransform.Accumulate(trans.Inverse());
	for (auto PIt : BaseEditPoints)
	{
		FVector PrePos = Vertexe(PIt.Value->GetPos());
		PIt.Value->SetPos(PrePos);
	}
	return false;
}

void FArmyFurniture::SetRelatedActorSelected(bool bIsSelected)
{
	AActor * FurActor = GetRelevanceActor();
	AActor * PipeActor = nullptr;
	bool bIsExist = false;
	TArray<FObjectPtr> Objs = GetChildren();
	TSharedPtr<FArmyPipeline> PipeLine;
	for (auto& ObjectPtr : Objs)
	{
		if (ObjectPtr->GetType() >= OT_Preload_Begin && ObjectPtr->GetType() <= OT_Preload_End)
		{
			PipeLine = StaticCastSharedPtr<FArmyPipeline>(ObjectPtr);
			PipeActor = PipeLine->GetRelevanceActor();
			if (PipeActor && PipeActor->IsValidLowLevel())
			{
				PipeActor->bIsSelectable = bIsSelected;
			}
		}
	}
	if (FurActor && FurActor->IsValidLowLevel())
	{
		FurActor->bIsSelectable = bIsSelected;
	}
}

TSharedPtr<FArmyEditPoint> FArmyFurniture::GetOperationPoint(uint32 Index) const
{
	if (Index < (uint32)BaseEditPoints.Num() && Index >= 0)
		return BaseEditPoints[Index];
	else
		return NULL;
}

uint32 FArmyFurniture::GetOperationIndex(TSharedPtr<FArmyEditPoint> InPoint) const
{
	return *(BaseEditPoints.FindKey(InPoint));
}

void FArmyFurniture::SetBeamWidth(float InWith)
{
	if (!GetPropertyFlag(FLAG_POINTOBJ))
	{
		float WScale = InWith / BeamWidth;
		BeamWidth = InWith;
		FVector WVector(1, WScale, 1);
		FTransform TempTran;
		TempTran.SetScale3D(WVector);
		ApplyTransform(TempTran);

	}
}

void FArmyFurniture::SetBeamLength(float InLength)
{
	if (!GetPropertyFlag(FLAG_POINTOBJ))
	{
		float LScale = InLength / BeamLength;
		BeamLength = InLength;
		FVector HVector(LScale, 1, 1);
		FTransform TempTran;
		TempTran.SetScale3D(HVector);
		ApplyTransform(TempTran);


	}
}

void FArmyFurniture::SetThickness(float InThickness)
{
	BeamThickness = InThickness;
}

void FArmyFurniture::RebuildLenghtWidth()
{
	if (!GetPropertyFlag(FLAG_POINTOBJ) && BaseEditPoints.Contains(1) && BaseEditPoints.Contains(3) && BaseEditPoints.Contains(7))
	{
		FVector WidthVector = GetOperationPoint(7)->GetPos() - GetOperationPoint(1)->GetPos();
		FVector LenVector = GetOperationPoint(3)->GetPos() - GetOperationPoint(1)->GetPos();
		BeamLength = LenVector.Size();
		BeamWidth = WidthVector.Size();
	}
}

void FArmyFurniture::GenerateBeamModel(UWorld* InWorld)
{
	BuildModel(BeamThickness, BeamThickness, InWorld);
}

void FArmyFurniture::BuildModel(float InBeamThickness, float Height, UWorld* InWorld)
{
	if (BeamModel == NULL)
	{
		BeamModel = InWorld->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass());
		BeamModel->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial());
	}
}

const TArray<TSharedPtr<FArmyPolyline>>& FArmyFurniture::GetPolys() const
{
	return Polys;
}

void FArmyFurniture::SetFurnitureModelActor(class AXRFurnitureActor* NewActor)
{
	FurnitureModelActor = NewActor;
}

class AActor* FArmyFurniture::GetActor()
{
	return Cast<AActor>(FurnitureModelActor);
}

bool FArmyFurniture::IsGasMeter()
{
	switch (ComponentType)
	{
	case EC_Gas_Meter:
	//case EC_EleBoxL_Point:
		return true;
	default:
		return false;
	}
	return false;
}

bool FArmyFurniture::IsWaterSeparator()
{
	switch (ComponentType)
	{
	case EC_Water_Separator_Point:
	//case EC_EleBoxL_Point:
		return true;
	default:
		return false;
	}
	return false;
}

bool FArmyFurniture::IsGasMainPipe()
{
	switch (ComponentType)
	{
	case EC_Gas_MainPipe:
		return true;
	default:
		return false;
	}
	return false;
}

bool FArmyFurniture::IsDownWarterMainPipe()
{
	switch (ComponentType)
	{
	case EC_Drain_Point:
		return true;
	default:
		return false;
	}
	return false;
}

bool FArmyFurniture::IsWaterNormalPoint()
{
	switch (ComponentType)
	{
	case EC_Water_Normal_Point:
		return true;
	default:
		return false;
	}
	return false;
}

bool FArmyFurniture::IsWaterSupply()
{
	switch (ComponentType)
	{
	case EC_Water_Supply:
		return true;
	default:
		return false;
	}
	return false;
}

bool FArmyFurniture::IsBasin()
{
	switch (ComponentType)
	{
	case EC_Basin:
		return true;
	default:
		return false;
	}
	return false;
}

bool FArmyFurniture::IsWaterBasin()
{
	switch (ComponentType)
	{
	case EC_Water_Basin:
		return true;
	default:
		return false;
	}
	return false;
}

bool FArmyFurniture::IsClosestool()
{
	switch (ComponentType)
	{
	case EC_Closestool:
		return true;
	default:
		return false;
	}
	return false;
}

void FArmyFurniture::SetState(EObjectState InState)
{
	State = InState;
	if (InState == OS_Selected)
	{
		bUseCustomColor = true;
		CustomoColor = FLinearColor(FColor(0xFFFF9800));

		UpdateDashLine();

		if (FirstRulerLine.IsValid() && SecondRulerLine.IsValid() && !bCreateOperation)
		{
			FirstRulerLine->SetEnableInputBox(true);
			SecondRulerLine->SetEnableInputBox(true);
			FirstRulerLine->ShowInputBox(true);
			SecondRulerLine->ShowInputBox(true);
		}
	}
	else
	{
		if (FirstRulerLine.IsValid() && SecondRulerLine.IsValid())
		{
			FirstRulerLine->SetEnableInputBox(false);
			SecondRulerLine->SetEnableInputBox(false);
			FirstRulerLine->ShowInputBox(false);
			SecondRulerLine->ShowInputBox(false);
			bUseCustomColor = false;
		}
	}

	//@  更新构件绘制颜色
	UpdateSimpleElementVertex();
}

bool FArmyFurniture::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FLAG_VISIBILITY))
	{
		FBox Box = GetBounds();

		return FBox2D(FVector2D(Box.Min), FVector2D(Box.Max)).IsInside(FVector2D(Pos));
	}
	return false;
}

bool FArmyFurniture::Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	return IsSelected(Pos, InViewportClient);
}

const FBox FArmyFurniture::GetBounds()
{
	FBox V(ForceInitToZero);
	V += Vertexe(BondingBox.Min);
	V += Vertexe(BondingBox.Max);
	V += Vertexe(FVector(BondingBox.Min.X, BondingBox.Max.Y, 0));
	V += Vertexe(FVector(BondingBox.Max.X, BondingBox.Min.Y, 0));
	return V;
}
const FBox FArmyFurniture::GetPreBounds()
{
	return BondingBox;
}
const FVector FArmyFurniture::GetBasePos()
{
	return Vertexe(PivotPoint);
}

const void FArmyFurniture::GetAlonePoints(TArray< TSharedPtr<FArmyPoint> >& OutPoints)
{
	if (BaseEditPoint.IsValid() && BaseEditPoint->GetState() == OS_Selected) return;//处于编辑状态不不参与端点捕捉

	for (auto PIt : BaseEditPoints)
	{
		TSharedPtr<FArmyPoint> CapturePoint = MakeShareable(new FArmyPoint());
		CapturePoint->Pos = PIt.Value->GetPos();
		OutPoints.Add(CapturePoint);
	}
}

TSharedPtr<FArmyEditPoint> FArmyFurniture::SelectPoint(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	TSharedPtr<FArmyEditPoint> SelectP = NULL;
	if (GetPropertyFlag(FLAG_VISIBILITY))
	{
		if (GetState() == OS_Selected)
		{
			for (auto PIt : BaseEditPoints)
			{
				if (PIt.Value->IsSelected(Pos, InViewportClient))
				{
					PIt.Value->SetState(FArmyEditPoint::OPS_Selected);
					SelectP = PIt.Value;
				}
				else
				{
					PIt.Value->SetState(FArmyEditPoint::OPS_Normal);
				}
			}
		}
	}
	return SelectP;
}

void FArmyFurniture::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (FurnitureModelActor && FurnitureModelActor->IsValidLowLevel())
	{
		PDI->DrawPoint(NowLocatoin, FLinearColor::Red, 20, SDPG_World);
	}
	
	if ((!FArmyObject::GetDrawModel(MODE_CONSTRUCTION)) 
		&&BreakPointType==EBreakPointType::NewPoint 
		)
	{
		return;
	}

	if ( BreakPointType == EBreakPointType::SubstitutePoint)
	{
		if (!(RelativeFurniture.IsValid()
			&& RelativeFurniture.Pin()->RelativeFurniture.IsValid()
			&& RelativeFurniture.Pin()->RelativeFurniture.Pin().Get() == this))
		{
			return;
		}
	}



	if (GetPropertyFlag(FLAG_VISIBILITY) && (!GetPropertyFlag(FLAG_STOPPED)))
	{
		if (Culler(View))
		{
			PDI->DrawLines(LineVertices, 1);
			if (State == OS_Selected)
			{
				for (auto PIt : BaseEditPoints)
				{
					if (FArmyObject::GetDrawModel(MODE_CONSTRUCTION))
					{
						//取消家家具在选中状态下绘制编辑点
						break;
					}

					PIt.Value->Draw(PDI, View);
				}

				if (EPlaceLocationFlag == EPLF_INNERNALWALLLINE && RelatedRoom.IsValid() && !bCreateOperation)
				{

					UpdateDashLine();
					DrawDashLine(PDI, View);
				}
				else
				{
					FirstRulerLine->SetEnableInputBox(false);
					SecondRulerLine->SetEnableInputBox(false);
					FirstRulerLine->ShowInputBox(false);
					SecondRulerLine->ShowInputBox(false);
				}

			}
		}

	}
}

void FArmyFurniture::ApplyTransform(const FTransform& Trans)
{
	for (auto PIt : BaseEditPoints)
	{
		FVector PrePos = Vertexe(PIt.Value->GetPos(), true);
		PIt.Value->SetPos(PrePos);
	}
	LocalTransform.Accumulate(Trans);
	for (auto PIt : BaseEditPoints)
	{
		FVector PrePos = Vertexe(PIt.Value->GetPos());
		PIt.Value->SetPos(PrePos);
	}

	//@ 如果变换后构件超出所在房间，则变换无效，恢复到变换前
	if (RelatedRoom.IsValid() && EPlaceLocationFlag == EPLF_ROOMSPACE)
	{
		const FBox ComBox = GetBounds();
		FVector Center = ComBox.GetCenter();
		FVector BoxSize = ComBox.GetSize();
		FVector LeftTop(Center.X - BoxSize.X / 2, Center.Y - BoxSize.Y / 2, 0);
		FVector RightTop(Center.X + BoxSize.X / 2, Center.Y - BoxSize.Y / 2, 0);
		FVector RightBottom(Center.X + BoxSize.X / 2, Center.Y + BoxSize.Y / 2, 0);
		FVector LeftBottom(Center.X - BoxSize.X / 2, Center.Y + BoxSize.Y / 2, 0);

		//放置在空间的点位，变换时外包几何不能超出所在房间
		if (!RelatedRoom.Pin()->IsPointInRoom(LeftTop)
			|| !RelatedRoom.Pin()->IsPointInRoom(RightTop)
			|| !RelatedRoom.Pin()->IsPointInRoom(RightBottom)
			|| !RelatedRoom.Pin()->IsPointInRoom(LeftBottom))
		{
			for (auto PIt : BaseEditPoints)
			{
				FVector PrePos = Vertexe(PIt.Value->GetPos(), true);
				PIt.Value->SetPos(PrePos);
			}
			LocalTransform.Accumulate(Trans.Inverse());
			for (auto PIt : BaseEditPoints)
			{
				FVector PrePos = Vertexe(PIt.Value->GetPos());
				PIt.Value->SetPos(PrePos);
			}
		}
	}

	RebuildLenghtWidth();
	UpdateSimpleElementVertex();

}

TSharedPtr<FArmyEditPoint> FArmyFurniture::HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	for (auto& It : BaseEditPoints)
	{
		if (It.Value->GetState() != FArmyEditPoint::OPS_Selected && It.Value->IsSelected(Pos, InViewportClient))
		{
			return It.Value;
		}
	}

	return NULL;
}

void FArmyFurniture::SetPosition(const FVector& InPos)
{
	for (auto PIt : BaseEditPoints)
	{
		FVector PrePos = Vertexe(PIt.Value->GetPos(), true);
		PIt.Value->SetPos(PrePos);
	}
	LocalTransform.SetTranslation(InPos);
	for (auto PIt : BaseEditPoints)
	{
		PIt.Value->SetPos(Vertexe(PIt.Value->GetPos()));
	}
	UpdateSimpleElementVertex();
	if (!bCreateOperation)
	{
		UpdateDashLine();
	}

	//找到对应的拆改模式的Furniture
	/*if (FArmySceneData::Get()->GetObjectByGuid(E_HomeModel, GetUniqueID()).Pin().IsValid())
	{
		TArray<TWeakPtr<FArmyObject>> CompoentList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_ComponentBase, CompoentList);
		for (auto It : CompoentList)
		{
			TSharedPtr<FArmyFurniture> ModifyFurniture = StaticCastSharedPtr<FArmyFurniture>(It.Pin());
			if (ModifyFurniture.IsValid())
			{
				if (PivotPoint.Equals(ModifyFurniture->GetPivortPoint()))
				{
					ModifyFurniture->SetPosition(InPos);
					TSharedPtr<FArmyFurniture> LayoutFurniture = StaticCastSharedPtr<FArmyFurniture>
						(FArmySceneData::Get()->GetObjectByGuid(E_LayoutModel, ModifyFurniture->GetUniqueID()).Pin());
					if (LayoutFurniture.IsValid())
						LayoutFurniture->SetPosition(InPos);

					FArmySceneData::Get()->GeneratePointPositionModel();
					break;
				}
			}
		}
	}*/
}

void FArmyFurniture::SetRotation(const FRotator& InRot)
{
	for (auto PIt : BaseEditPoints)
	{
		FVector PrePos = Vertexe(PIt.Value->GetPos(), true);
		PIt.Value->SetPos(PrePos);
	}
	LocalTransform.SetRotation(InRot.Quaternion());
	for (auto PIt : BaseEditPoints)
	{
		PIt.Value->SetPos(Vertexe(PIt.Value->GetPos()));
	}
	UpdateSimpleElementVertex();

	//找到对应的拆改模式的Furniture
	/*if (FArmySceneData::Get()->GetObjectByGuid(E_HomeModel, GetUniqueID()).Pin().IsValid())
	{
		TArray<TWeakPtr<FArmyObject>> CompoentList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_ComponentBase, CompoentList);
		for (auto It : CompoentList)
		{
			TSharedPtr<FArmyFurniture> ModifyFurniture = StaticCastSharedPtr<FArmyFurniture>(It.Pin());
			if (ModifyFurniture.IsValid())
			{
				if (PivotPoint.Equals(ModifyFurniture->GetPivortPoint()))
				{
					ModifyFurniture->SetRotation(InRot);
					TSharedPtr<FArmyFurniture> LayoutFurniture = StaticCastSharedPtr<FArmyFurniture>
						(FArmySceneData::Get()->GetObjectByGuid(E_LayoutModel, ModifyFurniture->GetUniqueID()).Pin());
					if (LayoutFurniture.IsValid())
						LayoutFurniture->SetRotation(InRot);

					FArmySceneData::Get()->GeneratePointPositionModel();
					break;
				}
			}
		}
	}*/
}

void FArmyFurniture::SetScale(const FVector& InScale)
{
	for (auto PIt : BaseEditPoints)
	{
		FVector PrePos = Vertexe(PIt.Value->GetPos(), true);
		PIt.Value->SetPos(PrePos);
	}
	LocalTransform.SetScale3D(InScale);
	for (auto PIt : BaseEditPoints)
	{
		PIt.Value->SetPos(Vertexe(PIt.Value->GetPos()));
	}
	UpdateSimpleElementVertex();

}

void FArmyFurniture::SetTransform(const FTransform& InTrans)
{
	for (auto PIt : BaseEditPoints)
	{
		FVector PrePos = Vertexe(PIt.Value->GetPos(), true);
		PIt.Value->SetPos(PrePos);
	}
	LocalTransform = InTrans;
	for (auto PIt : BaseEditPoints)
	{
		PIt.Value->SetPos(Vertexe(PIt.Value->GetPos()));
	}
	UpdateSimpleElementVertex();

}

void FArmyFurniture::GetTransForm(FRotator& InRot, FVector& InTran, FVector& InScale) const
{
	InTran = LocalTransform.GetLocation();
	InRot = LocalTransform.Rotator();
	InScale = LocalTransform.GetScale3D();
}
FVector FArmyFurniture::TranfromVertex(const FVector& InV) const
{
	return Vertexe(InV);
}
void FArmyFurniture::SetPolys(TArray<TSharedPtr<FArmyPolyline>>& InPolys)
{
	Polys.Empty();
	Polys = InPolys;

	//更新包围盒大小
	BondingBox.Init();
	TSharedPtr<FArmyPolyline> TempPoly;
	for (auto P : InPolys)
	{
		if (P.IsValid())
		{
			BondingBox += FBox(P->GetAllVertices());
		}

	}


}

void FArmyFurniture::SetPoints(TArray<TSharedPtr<FArmyPoint>>& InPoints)
{
	BondingBox = FBox(ForceInitToZero);
	Points.Empty();
	Points = InPoints;

	BaseEditPoints.Empty();
	for (auto P : Points)
	{
		bool IsEditPoint = false;
		TSharedPtr<FArmyEditPoint>EP = MakeShareable(new FArmyEditPoint());
		EP->SetPos(P->Pos);
		FColor UserFlag(P->PropertyFlag);
		if (UserFlag.R & 1)
		{
			IsEditPoint = true;
			BaseEditPoint = EP;
			PivotPoint = P->Pos;
			EP->OperationType = XROP_MOVE;
		}
		if (UserFlag.R & 2)
		{
			IsEditPoint = true;
			EP->OperationType = XROP_ROTATE;
		}
		if (UserFlag.R & 4)
		{
			IsEditPoint = true;
			EP->OperationType = XROP_SCALE;
		}
		if (UserFlag.R & 8)
		{
			IsEditPoint = true;
			EP->OperationType = XROP_NONE;
			//EP->SetPropertyFlag(FArmyObject::FLAG_POINTCAPTUREABLE, true);
		}
		if (IsEditPoint)
		{
			BaseEditPoints.Add(UserFlag.G, EP);
		}
		else
		{
			BondingBox += P->Pos;
			BaseSizeChanged();
		}
	}
	InitOperationPoint();
}

void FArmyFurniture::AddPoly(TSharedPtr<FArmyPolyline> InPoly)
{
	Polys.Push(InPoly);
	BondingBox += FBox(InPoly->GetAllVertices());
	BaseSizeChanged();
}

void FArmyFurniture::BaseSizeChanged()
{
	BeamWidth = BondingBox.GetSize().Y;
	BeamLength = BondingBox.GetSize().X;
}

void FArmyFurniture::AddPoint(TSharedPtr<FArmyPoint> InPoint)
{
	//1 移动 2 旋转  4缩放 8捕捉
	Points.Push(InPoint);

	bool IsEditPoint = false;
	TSharedPtr<FArmyEditPoint>EP = MakeShareable(new FArmyEditPoint());
	EP->SetPos(InPoint->Pos);
	FColor UserFlag(InPoint->PropertyFlag);
	if (UserFlag.R & 1)
	{
		IsEditPoint = true;
		BaseEditPoint = EP;
		PivotPoint = InPoint->Pos;

		EP->OperationType = XROP_MOVE;
	}
	if (UserFlag.R & 2)
	{
		IsEditPoint = true;
		EP->OperationType = XROP_ROTATE;
	}
	if (UserFlag.R & 4)
	{
		IsEditPoint = true;
		EP->OperationType = XROP_SCALE;
	}
	if (UserFlag.R & 8)
	{
		IsEditPoint = true;
		//EP->OperationType = XROP_NONE;
	}
	if (IsEditPoint)
	{
		BaseEditPoints.Add(UserFlag.G, EP);
	}
	else
	{
		BondingBox += InPoint->Pos;
	}
}

void FArmyFurniture::InitOperationPoint()
{
	bool ContainRotaPoint = false;
	for (auto PIt : BaseEditPoints)
	{
		if (!ContainRotaPoint && PIt.Value->OperationType == XROP_ROTATE)
		{
			ContainRotaPoint = true;
		}
		PIt.Value->SetPos(PIt.Value->GetPos() - PivotPoint);
	}
	if (!ContainRotaPoint)
	{
		SetPropertyFlag(FLAG_POINTOBJ, true); //点位构件
	}
	else
	{
		SetPropertyFlag(FArmyObject::FLAG_COLLISION, true);
	}
}

FVector FArmyFurniture::GetLocation(FVector& InLocation)
{
	if (FurnitureModelActor)
	{
		FTransform Transform = FurnitureModelActor->GetTransform();
		return Transform.InverseTransformPosition(InLocation);
	}
	return FVector::ZeroVector;
}

float FArmyFurniture::GetAltitude()
{
	return FurniturePro->GetAltitude();
}

void FArmyFurniture::SetAltitude(float InAltitude)
{
	FurniturePro->SetAltitude(InAltitude);
}

void FArmyFurniture::Generate3DModel(const FString& InModelFilePath, UWorld* InWorld)
{
	bool bNotInit = FurnitureModelActor == NULL;
	//if (bNotInit)
	//{
	//	FurnitureModelActor = InWorld->SpawnActor<AXRFurnitureActor>(AXRFurnitureActor::StaticClass());
	//	FurnitureModelActor->bRunningUserConstructionScript = true;
	//	UStaticMesh* mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, InModelFilePath.GetCharArray().GetData(), NULL, LOAD_None, NULL));
	//	FurnitureModelActor->GetStaticMeshComponent()->SetStaticMesh(mesh);
	//	UCurveFloat* Float = NewObject<UCurveFloat>(); 
	//	FurnitureModelActor->GetMaterialBillboardComponent()->AddElement(mesh->GetMaterial(0),Float,false,8,8,Float);
	//	FurnitureModelActor->XROwner = this->AsShared();
	//}
	UpdateData();
}

void FArmyFurniture::UpdateData()
{

	FRotator furnitureRotator;

	if (RelevanceActor)
	{
		AStaticMeshActor* SMA = Cast<AStaticMeshActor>(RelevanceActor);
		//GetPointMap (SMA->GetStaticMeshComponent(),EditPoints3D);
	}
}

void FArmyFurniture::ConversionLocation(UStaticMeshComponent* InComponent, FVector& InRelativeLocation, FRotator& furnitureRotator, FVector& OutLocation)
{
	AActor* InActor = InComponent->GetOwner();
	FTransform transForm = InComponent->GetRelativeTransform();
	TArray<USceneComponent*> Parents;
	InComponent->GetParentComponents(Parents);
	bool bRoot = Parents.Num() == 0;
	const FMatrix CompLS = transForm.ToMatrixNoScale();
	FRotator DefRotator(0, 0, 0);
	//本地空间到世界空间
	const FMatrix LocalToWorld = InActor->ActorToWorld().ToMatrixWithScale();
	//构造附着点相对于actor的本地空间矩阵 
	const FMatrix DefMeshLS = FRotationTranslationMatrix(DefRotator, InRelativeLocation);
	FMatrix NowLocationLS = bRoot ? DefMeshLS * CompLS : DefMeshLS * CompLS * LocalToWorld;

	OutLocation = NowLocationLS.GetOrigin();
}

TSharedPtr<FArmyFurniture> FArmyFurniture::CopyFurniture()
{
	TSharedPtr<FArmyFurniture> CopyResult = MakeShareable(new FArmyFurniture());

	UpdateSimpleElementVertex();

	TArray<TSharedPtr<FArmyPoint>> CopyPoints;
	TArray<TSharedPtr<FArmyPolyline>> CopyPolys;
	for (auto PIt : Points)
	{
		CopyPoints.Push(MakeShareable(new FArmyPoint(PIt.Get())));
	}
	for (auto PIt : Polys)
	{
		CopyPolys.Push(MakeShareable(new FArmyPolyline(PIt.Get())));
	}

	CopyResult->LocalTransform = FTransform(LocalTransform);
	CopyResult->SetPoints(CopyPoints);
	CopyResult->SetName(GetName());
	CopyResult->SetPolys(CopyPolys);
	//CopyResult->Altitude = Altitude;
	CopyResult->FurnitureId = FurnitureId;
	CopyResult->PivotPoint = PivotPoint;
	CopyResult->BeamLength = BeamLength;
	CopyResult->BeamWidth = BeamWidth;
	CopyResult->BeamThickness = BeamThickness;
	CopyResult->BeamModel = BeamModel;
	CopyResult->SetType(GetType());
	CopyResult->CapturePoints3D.Append(CapturePoints3D);
	CopyResult->ComponentType = ComponentType;
	CopyResult->bReform = bReform;
	CopyResult->bTransverse = bTransverse;
	CopyResult->LineVertices = LineVertices;
	CopyResult->EPlaceLocationFlag = EPlaceLocationFlag;
	CopyResult->FurniturePro = MakeShareable(new FFurnitureProperty(FurniturePro));
	
	//@郭子阳
	CopyResult->BreakPointType = BreakPointType;
	
	
	/*CopyResult->PipeRadius = PipeRadius;
	CopyResult->PointPositionLength = PointPositionLength;
	CopyResult->PointPositionWidth = PointPositionWidth;
	CopyResult->PointPositionHeigth = PointPositionHeigth;
	CopyResult->PointPositionAltitude = PointPositionAltitude;*/
	TSharedPtr<FArmyEditPoint> TempEditPos = nullptr;
	for (auto P : BaseEditPoints)
	{
		TempEditPos = MakeShareable(new FArmyEditPoint(P.Value.Get()));
		CopyResult->BaseEditPoints.Add(P.Key, TempEditPos);
		if (P.Value->OperationType == XROP_MOVE)
		{
			CopyResult->BaseEditPoint = TempEditPos;
		}
	}
	CopyResult->RelatedRoom = RelatedRoom;
	CopyResult->BelongClass = BelongClass;
	CopyResult->bUseCustomColor = bUseCustomColor;
	CopyResult->CustomoColor = CustomoColor;
	CopyResult->SetPropertyFlag(FLAG_ORIGINALPOINT ,GetPropertyFlag(FLAG_ORIGINALPOINT) == true ? true : false);
	CopyResult->UpdateSimpleElementVertex();
	return CopyResult;
}


bool FArmyFurniture::IsEleBox()
{
	switch (ComponentType)
	{
	case EC_EleBoxH_Point:
	case EC_EleBoxL_Point:
		return true;
	default:
		return false;
	}
	return false;
}


bool FArmyFurniture::IsSwitch()
{
	switch (ComponentType)
	{
	case EC_Switch_Defualt: 
	case	EC_Switch_BathHeater://浴霸开关
	case	EC_Switch_Double://双键无线开关
	case	EC_Switch_Single://单键无线开关
	case	EC_Switch4O2_Point://四联双控开关
	case	EC_Switch4O1_Point://四联单控开关
	case	EC_Switch3O2_Point://三联双控开关
	case	EC_Switch3O1_Point://三联单控开关
	case	EC_Switch2O2_Point://双联双控开关
	case	EC_Switch2O1_Point://双联单控开关
	case	EC_Switch1O2_Point://单连双控开关
	case	EC_Switch1O1_Point://单连单控开关
		return true;
	default:
		return false;
	}
}

bool FArmyFurniture::IsSocket()
{
	switch (ComponentType)
	{
	case EC_Socket_H_Defualt:
	case 	EC_Socket_Air_Conditioner:
	case EC_Socket_L_Defualt:
	case  EC_Socket_Five_Point://五孔插座
	case  EC_Socket_Five_On_Point://五孔开关插座
	case  EC_Socket_Five_On_Dislocation://五孔错位插座
	case  EC_Socket_Five_Point_Dislocation://五孔错位开关插座
	case  EC_Socket_Four_Point: //四孔插座
	case  EC_Socket_Four_On_Point://四孔开关插座
	case  EC_Socket_Three_On_Point: //三孔开关插座
	case  EC_Socket_Three: //三孔插座
	case  EC_Socket_Three_On_Point_3A: //16A三孔开关插座
	case  EC_Socket_Three_3A: //16A三孔插座
	case  EC_Socket_White://白板
	case  EC_Socket_Spatter://防溅盒
	case  EC_Socket_IT://网线插座
	case  EC_Socket_IT_TV: //电视电脑插座
	case  EC_Socket_TV: //电视插座
	case  EC_Socket_Phone: //电话插座
	case  EC_Socket_IT_Phone: //电脑电话插座
	case  EC_Socket_TV_Phone: //电视电话插座
	case  EC_Socket_TP: //厨房橱柜专用联排插座
	case  EC_Socket_TV_Couplet: //电视墙专用联排插座
		return true;
	default:
		return false;
	}
}

bool FArmyFurniture::IsToilet()
{
	if (ComponentType == EC_Water_Hot_Point ||
		ComponentType == EC_Water_Chilled_Point ||
		ComponentType == EC_Water_Hot_Chilled_Point)
	{
		return true;
	}
	return false;
}

bool FArmyFurniture::IsPreLoad()
{
	switch (ComponentType)
	{
	case EC_Water_Defualt:
	case  EC_Water_Supply: // 给水点位,
	case  EC_Drain_Point: //下水主管道
	case  EC_Basin: //地漏下水
	case  EC_Water_Basin: //水盆下水
	case  EC_Water_Hot_Point: //热水点位
	case  EC_Closestool: //马桶下水
		return true;
	default:
		return false;
	}
}

bool FArmyFurniture::IsStrongElectirc()
{
	switch (ComponentType)
	{

	case EC_EleBoxH_Point:
	case EC_Socket_Five_Point://五孔插座
	case EC_Socket_Five_On_Point://五孔开关插座
	case EC_Socket_Five_On_Dislocation://五孔错位插座
	case EC_Socket_Five_Point_Dislocation://五孔错位开关插座
	case EC_Socket_Four_Point: //四孔插座
	case EC_Socket_Four_On_Point://四孔开关插座
	case EC_Socket_Three_On_Point: //三孔开关插座
	case EC_Socket_Three: //三孔插座
	case EC_Socket_Three_On_Point_3A: //16A三孔开关插座
	case EC_Socket_Three_3A: //16A三孔插座
	case EC_Socket_White://白板
	case EC_Socket_Spatter://防溅盒
	case EC_Socket_TP: //厨房橱柜专用联排插座
	case EC_Socket_TV_Couplet: //电视墙专用联排插座
	case EC_Switch_BathHeater: //浴霸开关
	case EC_Switch_Double: //双键无线开关
	case EC_Switch_Single: //单键无线开关
	case EC_Switch4O2_Point: //四联双控开关
	case EC_Switch4O1_Point: //四联单控开关
	case EC_Switch3O2_Point: //三联双控开关
	case EC_Switch3O1_Point: //三联单控开关
	case EC_Switch2O2_Point: //双联双控开关
	case EC_Switch2O1_Point://双联单控开关
	case EC_Switch1O2_Point://单连双控开关
	case EC_Switch1O1_Point: //单连单控开关
		return true;
	default:
		return false;
	}
	return false;
}

bool FArmyFurniture::IsWeakElectirc()
{
	switch (ComponentType)
	{
	case EC_Socket_IT://网线插座
	case EC_Socket_IT_TV: //电视电脑插座
	case EC_Socket_TV: //电视插座
	case EC_Socket_Phone: //电话插座
	case EC_Socket_IT_Phone: //电脑电话插座
	case EC_EleBoxL_Point:
	case EC_Socket_TV_Phone:
		return true;
	default:
		return false;
	}
	return false;
}

bool FArmyFurniture::IsHotWater()
{
	switch (ComponentType)
	{
	case EC_Water_Hot_Chilled_Point://
	case EC_Water_Hot_Point: //
		return true;
	default:
		return false;
	}
	return false;
}

bool FArmyFurniture::IsColdWater()
{
	switch (ComponentType)
	{
	case EC_Water_Hot_Chilled_Point://
	case EC_Water_Chilled_Point: //
		return true;
	default:
		return false;
	}
	return false;
}

bool FArmyFurniture::IsDrain()
{
	switch (ComponentType)
	{
	case EC_Dewatering_Point://排水点位
	case EC_Closestool://马桶下水
	case EC_Basin://地漏下水
		return true;
	default:
		return false;
	}
	return false;
}

bool FArmyFurniture::IsNormalSocket()
{
	switch (ComponentType)
	{
	case EC_EleBoxH_Point:
	case EC_Socket_Five_Point://五孔插座
	case EC_Socket_Five_On_Point://五孔开关插座
	case EC_Socket_Five_On_Dislocation://五孔错位插座
	case EC_Socket_Five_Point_Dislocation://五孔错位开关插座
	case EC_Socket_Four_Point: //四孔插座
	case EC_Socket_Four_On_Point://四孔开关插座
	case EC_Socket_Three_On_Point: //三孔开关插座
	case EC_Socket_Three: //三孔插座
	case EC_Socket_Three_On_Point_3A: //16A三孔开关插座
	case EC_Socket_Three_3A: //16A三孔插座
		return true;
	default:
		return false;
	}
}

void FArmyFurniture::GetPointMap(UStaticMeshComponent* StaticMeshComp, TMap<uint32, FVector>& OutMap)
{

	FRotator furnitureRotator = StaticMeshComp->GetComponentRotation();

	AActor* InActor = StaticMeshComp->GetOwner();

	StaticMeshComp->GetRelativeTransform();

	FBox Box = StaticMeshComp->GetBodySetup()->AggGeom.CalcAABB(FTransform(FVector(0, 0, 0)));

	FVector MeshPos, Extent;
	Box.GetCenterAndExtents(MeshPos, Extent);
	OutMap.Empty();

	FVector v1(ForceInit), v2(ForceInit), v3(ForceInit), v4(ForceInit);

	switch (ComponentType)
	{
	case EC_Water_Hot_Point:
	case EC_Water_Chilled_Point: //	1个附着点位
	case EC_Dewatering_Point:
	case EC_Closestool:
	case EC_Basin:
	case EC_Drain_Point:
	case EC_Water_Supply:
	case EC_Water_Basin:
	{
		FVector TopV(MeshPos.X, -Extent.Y, 0);
		ConversionLocation(StaticMeshComp, MeshPos, furnitureRotator, v1);
		OutMap.Add(11, v1);
	}
	break;
	case EC_Water_Hot_Chilled_Point://2个附着点位
	{
		float StartY = Extent.Y;//*9/10;
		float StartX = -Extent.X / 2;
		for (int32 i = 0; i < 2; i++)
		{
			int32 index = i == 0 ? 0 : 2;
			FVector TopV(StartX + Extent.X / 2 * index, StartY, Extent.Z);
			TopV = TopV + MeshPos;
			ConversionLocation(StaticMeshComp, TopV, furnitureRotator, v1);
			OutMap.Add(11 + i, v1);
		}

	}
	break;
	case EC_Socket_TP: // 厨房 14附着点
	{
		float StartY = -Extent.Y;//*9/10;
		float StartX = -Extent.X * (5.f / 6);
		for (int32 i = 0; i < 6; i++)
		{
			FVector TopV(StartX + (Extent.X / 3)*i, 0, Extent.Z);
			FVector BottomV(StartX + (Extent.X / 3)*i, 0, 0);
			TopV = TopV + MeshPos;
			ConversionLocation(StaticMeshComp, TopV, furnitureRotator, v1);
			ConversionLocation(StaticMeshComp, BottomV, furnitureRotator, v2);
			OutMap.Add(11 + i, v1);
			OutMap.Add(21 + i, v2);
		}
		FVector TopV(-Extent.X, -StartY, Extent.Z);
		FVector BottomV(Extent.X, -StartY, Extent.Z);
		ConversionLocation(StaticMeshComp, TopV, furnitureRotator, v1);
		ConversionLocation(StaticMeshComp, BottomV, furnitureRotator, v2);
		OutMap.Add(10, v1);
		OutMap.Add(20, v2);
	}
	break;
	case EC_Socket_TV_Couplet://电视墙联排18
	{
		float StartY = Extent.Y;//*9/10;
		float StartX = -Extent.X * 3 / 4;
		for (int32 i = 0; i < 8; i++)
		{
			FVector TopV(StartX + Extent.X / 5 * i, 0, Extent.Z);
			FVector BottomV(StartX + Extent.X / 5 * i, 0, 0);
			TopV = TopV + MeshPos;
			ConversionLocation(StaticMeshComp, TopV, furnitureRotator, v1);
			ConversionLocation(StaticMeshComp, BottomV, furnitureRotator, v2);
			OutMap.Add(11 + i, v1);
			OutMap.Add(21 + i, v2);
		}
		FVector TopV(-Extent.X, -StartY, Extent.Z);
		FVector BottomV(Extent.X, -StartY, Extent.Z);
		ConversionLocation(StaticMeshComp, TopV, furnitureRotator, v1);
		ConversionLocation(StaticMeshComp, BottomV, furnitureRotator, v2);
		OutMap.Add(10, v1);
		OutMap.Add(20, v2);
	}
	break;
	case EC_EleBoxL_Point:
	case EC_EleBoxH_Point://强电箱，弱电箱18点位
	{
		float StartY = Extent.Y;//*9/10;
		float StartX = -Extent.X * 4 / 5;
		for (int32 i = 0; i < 9; i++)
		{
			FVector TopV(StartX + Extent.X / 5 * i, Extent.Y, Extent.Z);
			FVector BottomV(StartX + Extent.X / 5 * i, Extent.Y, 0);
			TopV = TopV + MeshPos;
			BottomV.Y += MeshPos.Y;
			TopV.Y /= 2;
			BottomV.Y /= 2;
			ConversionLocation(StaticMeshComp, TopV, furnitureRotator, v1);
			ConversionLocation(StaticMeshComp, BottomV, furnitureRotator, v2);
			OutMap.Add(11 + i, v1);
			OutMap.Add(21 + i, v2);
		}
	}
	break;
	case EC_Socket_Five_Point:
	case EC_Socket_Five_On_Point:
	case EC_Socket_Five_On_Dislocation:
	case EC_Socket_Five_Point_Dislocation:
	case EC_Socket_Four_Point:
	case EC_Socket_Four_On_Point:
	case EC_Socket_Three_On_Point:
	case EC_Socket_Three:
	case EC_Socket_Three_On_Point_3A:
	case EC_Socket_Three_3A:
	case EC_Socket_White:
	case EC_Socket_Spatter:
	case EC_Socket_IT:
	case EC_Socket_IT_TV:
	case EC_Socket_TV:
	case EC_Socket_Phone:
	case EC_Socket_IT_Phone:
	case EC_Socket_TV_Phone:
	case EC_Switch_BathHeater:
	case EC_Switch_Double:
	case EC_Switch_Single:
	case EC_Switch4O2_Point:
	case EC_Switch4O1_Point:
	case EC_Switch3O2_Point:
	case EC_Switch3O1_Point:
	case EC_Switch2O2_Point:
	case EC_Switch2O1_Point:
	case EC_Switch1O2_Point:
	case EC_Switch1O1_Point: //6个附着点位
	default:
	{
		float StartY = Extent.Y;//*9/10;
		float StartX = -Extent.X / 2;
		for (int32 i = 0; i < 2; i++)
		{
			int32 index = i == 0 ? 0 : 2;
			FVector TopV(StartX + Extent.X / 2 * index, 0, Extent.Z);
			FVector BottomV(StartX + Extent.X / 2 * index, 0, 0);
			TopV = TopV + MeshPos;
			ConversionLocation(StaticMeshComp, TopV, furnitureRotator, v1);
			ConversionLocation(StaticMeshComp, BottomV, furnitureRotator, v2);
			OutMap.Add(11 + i, v1);
			OutMap.Add(21 + i, v2);
		}
		FVector TopV(-Extent.X, -StartY, Extent.Z);
		FVector BottomV(Extent.X, -StartY, Extent.Z);
		ConversionLocation(StaticMeshComp, TopV, furnitureRotator, v1);
		ConversionLocation(StaticMeshComp, BottomV, furnitureRotator, v2);
		OutMap.Add(13, v1);
		OutMap.Add(23, v2);
	}
	break;
	}

}

TArray<FVector> FArmyFurniture::GetBottomVerticeList(EObjectType InType)
{
	float CurrentThickness = FArmySceneData::WallHeight;
	if (InType == OT_CurtainBox)
		CurrentThickness = BeamThickness;

	TArray<FVector> VerticeList;
	if (!GetOperationPoint(1).IsValid() || !GetOperationPoint(3).IsValid()
		|| !GetOperationPoint(9).IsValid() || !GetOperationPoint(7).IsValid())
		return VerticeList;
	VerticeList.Push(GetOperationPoint(1)->GetPos() + FVector(0, 0, FArmySceneData::WallHeight - CurrentThickness));
	VerticeList.Push(GetOperationPoint(3)->GetPos() + FVector(0, 0, FArmySceneData::WallHeight - CurrentThickness));
	VerticeList.Push(GetOperationPoint(9)->GetPos() + FVector(0, 0, FArmySceneData::WallHeight - CurrentThickness));
	VerticeList.Push(GetOperationPoint(7)->GetPos() + FVector(0, 0, FArmySceneData::WallHeight - CurrentThickness));

	return VerticeList;
}

void FArmyFurniture::CalcRelatedRoom()
{
	/**@欧石楠 完成位置修改时，重新取一遍关联房间*/
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		FVector TempPos = GetBasePos();
		TempPos.Z = 0;
		if (Room.IsValid() && Room->IsPointInRoom(TempPos))
		{
			SetRelatedRoom(Room);
			return;
		}
	}
	SetRelatedRoom(nullptr);
	//HideRuleLineInputBox(true);
}

void FArmyFurniture::GetPointMapes(TMap<uint32, FVector>& OutMap3, TMap<uint32, FVector>& OutMap2)
{
	AStaticMeshActor* StaticActor = Cast<AStaticMeshActor>(GetRelevanceActor());

	for (auto& ItemComp : StaticActor->GetComponents())
	{
		if (ItemComp->ComponentHasTag(TEXT("Transverse")))
		{
			GetPointMap(Cast<UStaticMeshComponent>(ItemComp), OutMap2);
			GetPointMap(StaticActor->GetStaticMeshComponent(), OutMap3);
		}
	}
}

void FArmyFurniture::GetPoint3DMap(TMap<uint32, FVector>& OutMap3)
{
	AStaticMeshActor* StaticActor = Cast<AStaticMeshActor>(GetRelevanceActor());

	for (auto& ItemComp : StaticActor->GetComponents())
	{
		if (ItemComp->ComponentHasTag(TEXT("Transverse")))
		{
			GetPointMap(StaticActor->GetStaticMeshComponent(), OutMap3);
		}
	}
}

// 别删除!!!!!!!
//EObjectType FArmyFurniture::GetComponentTypeByServiceType(uint32 InType)
//{
//	switch (InType)
//	{
//	case EC_EleBoxL_Point:
//	{
//		return OT_EleBoxL_Point;
//	}
//	case EC_EleBoxH_Point:
//	{
//		 return OT_EleBoxH_Point;
//	}
//	case EC_Water_ChilledAndHot_Point:
//	{
//		return OT_Water_ChilledAndHot_Point;
//		
//	}
//	case	EC_Socket_Point://插座
//	{
//		return OT_Socket_Point;
//		
//	}
//	case EC_Socket_Five_Point://五孔插座
//	{
//		return OT_Socket_Five_Point;
//		
//	}
//	case EC_Socket_Five_On_Point://五孔开关插座
//	{
//		return OT_Socket_Five_On_Point;
//		
//	}
//	case EC_Socket_Five_On_Dislocation://五孔错位插座
//	{
//		return OT_Socket_Five_On_Dislocation;
//		
//	}
//	case EC_Socket_Five_Point_Dislocation://五孔错位开关插座
//	{
//		return OT_Socket_Five_Point_Dislocation;
//		
//	}
//	case EC_Socket_Four_Point: //四孔插座
//	{
//		return OT_Socket_Four_Point;
//		
//	}
//	case EC_Socket_Four_On_Point://四孔开关插座
//	{
//		return OT_Socket_Four_On_Point;
//		
//	}
//	case EC_Socket_Three_On_Point: //三孔开关插座
//	{
//		return OT_Socket_Three_On_Point;
//		
//	}
//	case EC_Socket_Three: //三孔插座
//	{
//		return OT_Socket_Three;
//		
//	}
//	case EC_Socket_Three_On_Point_3A: //16A三孔开关插座
//	{
//		return OT_Socket_Three_On_Point_3A;
//		
//	}
//	case EC_Socket_Three_3A: //16A三孔插座
//	{
//		return OT_Socket_Three_3A;
//		
//	}
//	case EC_Socket_White://白板
//	{
//		return OT_Socket_White;
//		
//	}
//	case EC_Socket_Spatter://防溅盒
//	{
//		return OT_Socket_Spatter;
//		
//	}
//	case EC_Socket_IT://网线插座
//	{
//		return OT_Socket_IT;
//		
//	}
//	case EC_Socket_IT_TV: //电视电脑插座
//	{
//		return OT_Socket_IT_TV;
//		
//	}
//	case EC_Socket_TV: //电视插座
//	{
//		return OT_Socket_TV;
//		
//	}
//	case EC_Socket_Phone: //电话插座
//	{
//		return OT_Socket_Phone;
//		
//	}
//	case EC_Socket_IT_Phone: //电脑电话插座
//	{
//		return OT_Socket_IT_Phone;
//		
//	}
//	case EC_Socket_TP: //厨房橱柜专用联排插座
//	{
//		return OT_Socket_TP;
//		
//	}
//	case EC_Socket_TV_Couplet: //电视墙专用联排插座
//	{
//		return OT_Socket_TV_Couplet;
//		
//	}
//	case EC_ElectricWire: //
//	{
//		return OT_ElectricWire;
//		
//	}
//	case EC_Switch_BathHeater: //浴霸开关
//	{
//		return OT_Switch_BathHeater;
//		
//	}
//	case EC_Switch_Double: //双键无线开关
//	{
//		return OT_Switch_Double;
//		
//	}
//	case EC_Switch_Single: //单键无线开关
//	{
//		return OT_Switch_Single;
//		
//	}
//	case EC_Switch4O2_Point: //四联双控开关
//	{
//		return OT_Switch4O2_Point;
//		
//	}
//	case EC_Switch4O1_Point: //四联单控开关
//	{
//		return OT_Switch4O1_Point;
//		
//	}
//	case EC_Switch3O2_Point: //三联双控开关
//	{
//		return OT_Switch3O2_Point;
//		
//	}
//	case EC_Switch3O1_Point: //三联单控开关
//	{
//		return OT_Switch3O1_Point;
//		
//	}
//	case EC_Switch2O2_Point: //双联双控开关
//	{
//		return OT_Switch2O2_Point;
//		
//	}
//	case EC_Switch2O1_Point://双联单控开关
//	{
//		return OT_Switch2O1_Point;
//		
//	}
//	case EC_Switch1O2_Point://单连双控开关
//	{
//		return OT_Switch1O2_Point;
//		
//	}
//	case EC_Switch1O1_Point: //单连单控开关
//	{
//		return OT_Switch1O1_Point;
//		
//	}
//	case EC_Water_Hot_Point: //热水点位
//	{
//		return OT_Water_Hot_Point;
//		
//	}
//	case EC_Water_Chilled_Point: //冷水点位
//	{
//		return OT_Water_Chilled_Point;
//		
//	}
//	case EC_Water_Hot_Chilled_Point: //冷熱水
//	{
//		return OT_Water_Hot_Chilled_Point;
//		
//	}
//	case EC_Dewatering_Point: //排水点位
//	{
//		return OT_Dewatering_Point;
//		
//	}
//	case EC_FloorDrain_Point: //地漏
//	{
//		return OT_FloorDrain_Point;
//		
//	}
//	case EC_Closestool: //马桶下水
//	{
//		return OT_Closestool;
//		
//	}
//	case EC_Basin: //地漏下水
//	{
//		return OT_Basin;
//		
//	}
//	case EC_Drain_Point: //下水主管道
//	{
//		return OT_Drain_Point;
//		
//	}
//	case EC_Water_Supply: // 给水点位,
//	{
//		return OT_Water_Supply;
//		
//	}
//	default:
//		break;
//	}
//	return OT_Switch3O2_Point;
//}

void FArmyFurniture::UpdateDashLine()
{
	if (!RelatedRoom.IsValid())
		return;
	TArray<TSharedPtr<FArmyLine>> Lines;
	RelatedRoom.Pin()->GetLines(Lines);
	//点位的两个标尺线
	FVector FirstPoint(INT_MAX, 0, 0);
	FVector SecondPoint(INT_MAX, 0, 0);

	//计算点所在的线
	TSharedPtr<FArmyLine> RelatedLine;
	GetRelatedLine(RelatedLine);

	if (!RelatedLine.IsValid())
		return;

	//计算在关联线方向上距离最近的墙点
	for (auto & It : Lines)
	{
		if (It.IsValid() && RelatedLine != It)
		{
			FVector InternalPos;
			if (FArmyMath::SegmentIntersection2D(It->GetStart(), It->GetEnd(), RelatedLine->GetStart(), RelatedLine->GetEnd(), InternalPos) && FVector(INT_MAX, 0, 0) == FirstPoint)
			{
				FirstPoint = InternalPos;
			}

			if (FArmyMath::SegmentIntersection2D(It->GetStart(), It->GetEnd(), RelatedLine->GetStart(), RelatedLine->GetEnd(), InternalPos) && InternalPos != FirstPoint/* && FVector2D(INT_MAX, 0) == FVector2D(SecondPoint)*/)
			{
				SecondPoint = InternalPos;
			}

		}
	}

	//确保第一个点与起始点一致
	if (FirstPoint != RelatedLine->GetStart())
	{
		FVector TempVec = FirstPoint;
		FirstPoint = SecondPoint;
		SecondPoint = TempVec;
	}

	//设置标尺的位置
	FVector RelatedLineDirection = RelatedLine->GetDirectionNormal();
	FVector VerticalRelatedLineDirection = RelatedLineDirection.RotateAngleAxis(90, FVector(0, 0, 1));
	if (!RelatedRoom.Pin()->IsPointInRoom(BaseEditPoint->GetPos() + VerticalRelatedLineDirection * 3))
	{
		VerticalRelatedLineDirection *= -1;
	}

	float Thickness = 24.F;
	if (FirstPoint.X != INT_MAX && SecondPoint.X != INT_MAX)
	{
		FirstRulerLine->Update(BaseEditPoint->GetPos(), FirstPoint, VerticalRelatedLineDirection * (OffsetDis - Thickness / 2));
		SecondRulerLine->Update(BaseEditPoint->GetPos(), SecondPoint, VerticalRelatedLineDirection * (OffsetDis - Thickness / 2));
	}
	else
	{
		return;
	}

}

void FArmyFurniture::DrawDashLine(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (FirstRulerLine.IsValid() && SecondRulerLine.IsValid() && !bDragModify && !bCreateOperation && State == OS_Selected)
	{
		FirstRulerLine->Draw(PDI, View);
		SecondRulerLine->Draw(PDI, View);
	}
	else
	{
		FirstRulerLine->SetEnableInputBox(false);
		SecondRulerLine->SetEnableInputBox(false);
		FirstRulerLine->ShowInputBox(false);
		SecondRulerLine->ShowInputBox(false);
	}
}

void FArmyFurniture::OnFirstInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{

	if (InTextAction == ETextCommit::OnEnter)
	{
		float CachedLength = FirstRulerLine->GetInputBoxCachedLength();
		float TempInValue = FCString::Atof(*InText.ToString()) / 10.f;
		float TempLength = CachedLength - TempInValue;
		TSharedPtr<FArmyLine> RelatedLine;
		GetRelatedLine(RelatedLine);

		//点位必须在墙线上，不能超出墙线
		float LineDis = FVector::Dist2D(RelatedLine->GetStart(), RelatedLine->GetEnd());

		if (TempInValue < LineDis && TempLength != 0 && RelatedLine.IsValid())
		{
			SCOPE_TRANSACTION(TEXT("通过标尺修改构件的位置"));
			SetPosition(GetBasePos() + (RelatedLine->GetDirectionNormal() * (-TempLength)));
			Modify();
			//临时处理：后期可能移植到撤销重做统一发代理
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
		else
		{
			//更新输入框数值
			TSharedPtr<SArmyInputBox> InputBoxWid = FirstRulerLine->GetWidget();
			if (InputBoxWid.IsValid())
			{
				InputBoxWid->SetInputText(FText::FromString(FString::FromInt(CachedLength * 10) /*FString::SanitizeFloat(CachedLength * 10)*/));
			}
		}
	}

}
void FArmyFurniture::OnSecondInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		float CachedLength = SecondRulerLine->GetInputBoxCachedLength();
		float TempInValue = FCString::Atof(*InText.ToString()) / 10.f;
		float TempLength = CachedLength - TempInValue;
		TSharedPtr<FArmyLine> RelatedLine;
		GetRelatedLine(RelatedLine);

		//点位必须在墙线上，不能超出墙线
		float LineDis = FVector::Dist2D(RelatedLine->GetStart(), RelatedLine->GetEnd());

		if (TempInValue < LineDis && TempLength != 0 && RelatedLine.IsValid())
		{
			SCOPE_TRANSACTION(TEXT("通过标尺修改构件的位置"));
			SetPosition(GetBasePos() + (RelatedLine->GetDirectionNormal() * TempLength));
			Modify();
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
		else
		{
			//更新输入框数值
			TSharedPtr<SArmyInputBox> InputBoxWid = SecondRulerLine->GetWidget();
			if (InputBoxWid.IsValid())
			{
				InputBoxWid->SetInputText(FText::FromString(FString::FromInt(CachedLength * 10) /*FString::SanitizeFloat(CachedLength * 10)*/));
			}
		}
	}
}



void FArmyFurniture::SetRelevanceActor(AActor * InActor)
{
	FArmyObject::SetRelevanceActor(InActor);

	
	bool ShouldShow3DMesh = true;
	if ( bisReplaced && bIsBPreforming)
	{
		if (BreakPointType == NativePoint)
		{
			ShouldShow3DMesh = false;
		}
	}else if(BreakPointType == SubstitutePoint)
	{
		ShouldShow3DMesh = false;
	}

	if (ShouldShow3DMesh)
	{
		InActor->Tags.Remove(TEXT("BreakPointReformHide"));
		InActor->SetActorHiddenInGame(false);
	}
	else
	{
		InActor->Tags.AddUnique(TEXT("BreakPointReformHide"));
		InActor->SetActorHiddenInGame(true);
	}

	bool ShouldShowWidget = true; //是否顯示坐标轴

	if (BreakPointType == NativePoint &&
		(IsBPStopped()
		|| (!IsBPreforming())))
	{
		ShouldShowWidget = false;
	}

	if (ShouldShowWidget)
	{
		InActor->Tags.Remove(TEXT("NoWidget"));
	}
	else
	{
		InActor->Tags.AddUnique(TEXT("NoWidget"));
	}

	if (BreakPointType == NativePoint)
	{
		if (!GEngine->OnActorMoved().IsBoundToObject(this))
		{
			MoveHandle=GEngine->OnActorMoved().AddRaw(this, &FArmyFurniture::OnActorMove);
			HaveMoveHandle = true;
		}
	}


	UpdateShow();
}

void FArmyFurniture::SetBreakPointType(EBreakPointType NewType)
{
	BreakPointType = NewType;
}

 //是否已停用

  bool FArmyFurniture::IsBPStopped()
 {
	 return GetRepresentedFurniture(true)->StopUsing;
 }

void FArmyFurniture::SetBPStop(bool value)
{
	if (StopUsing != value)
	{
		StopUsing = value;
			auto Native = GetRepresentedFurniture(true);
			auto Substitute= GetRepresentedFurniture(false);
		if (value)
		{
			//清楚替换数据
			if (Native->RelativeFurniture.IsValid())
			{
				Native->RelativeFurniture.Pin()->BreakPointType = NativePoint;
				Native->RelativeFurniture.Pin()->SetPropertyFlag(FLAG_STOPPED, true);
				Native->RelativeFurniture.Pin()->GetRelevanceActor()->Destroy();
				Native->RelativeFurniture = nullptr;
				Native->bisReplaced = false;
			}

			//添加tag防止坐标轴显示
			Native->GetRelevanceActor()->Tags.AddUnique(TEXT("NoWidget"));

			//控制在3d的显示
			Native->GetRelevanceActor()->Tags.Remove(TEXT("BreakPointReformHide"));
			if (Native->RelativeFurniture.IsValid())
			{
				Native->RelativeFurniture.Pin()->GetRelevanceActor()->Tags.AddUnique(TEXT("BreakPointReformHide"));
			}
			
			
			//添加属性限制是否在2d图层显示
			Native->SetPropertyFlag(FLAG_STOPPED,true);
		


			Native->bisReplaced = false;
			//Substitute->bisReplaced = false;

			Native->bIsBPreforming = false;
			//Substitute->bIsBPreforming = false;

			//清楚偏移
			Native->SetOffset(FVector(0, 0, 0));



			//切换选择
			GXREditor->SelectNone(false, false, false);
			GXREditor->SelectActor(Native->GetRelevanceActor(), true, true);

			//绘制停用标识
			AStaticMeshActor * actor = Cast<AStaticMeshActor>(GetRepresentedFurniture(true)->GetRelevanceActor());
			UPaperSpriteComponent * StopSprite = actor->FindComponentByClass<UPaperSpriteComponent>();
			if (!StopSprite)
			{
				StopSprite = NewObject<UPaperSpriteComponent>(actor,TEXT("StopSprite1"));
				StopSprite->RegisterComponent();
                FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, false);
				StopSprite->AttachToComponent(actor->GetRootComponent(), AttachmentRules);
				UPaperSprite* Stop = LoadObject<UPaperSprite>(nullptr, TEXT("PaperSprite'/Game/Sprite/disable_red_Sprite.disable_red_Sprite'"));
				//StaticLoadObject(UObject::StaticClass(), NULL, TEXT("Material'/Game/XRCommon/Material/M_BaseColorT.M_BaseColorT'"));
				StopSprite->SetSprite(Stop);
				
			}
			FKAggregateGeom& agg = actor->GetStaticMeshComponent()->GetBodySetup()->AggGeom;
			FBox AggGeomBox = agg.CalcAABB(FTransform(FVector(0, 0, 0)));
			FVector Size= AggGeomBox.GetSize()*actor->GetActorScale3D();
			//UStaticMeshComponent::SetRelativeLocation
			FVector RelativeLocation;
			FRotator RelativeRotation(0, 0, 0);
			if (IsEleBox())
			{
				RelativeLocation.X = Size.X / 2 - 6;
				RelativeLocation.Y = 3;
				RelativeLocation.Z = 5;
			}
			else if (IsToilet() || IsWaterNormalPoint() || IsWaterSupply())
			{
				RelativeLocation.X = 0;
				RelativeLocation.Y = 5;
				RelativeLocation.Z = 0;
			}
			else if (IsBasin())
			{
				RelativeLocation.X = 0;
				RelativeLocation.Y = 0;
				RelativeLocation.Z = 2;
				RelativeRotation = FRotator(0, 0, 90);
			}
			else if (IsClosestool())
			{
				RelativeLocation.X = 0;
				RelativeLocation.Y = 0;
				RelativeLocation.Z = 5;
				RelativeRotation = FRotator(0, 0, 90);
			}
			else if (IsWaterSeparator())
			{
				RelativeLocation.X = Size.X / 2 - 40;
				RelativeLocation.Y = 15;
				RelativeLocation.Z = 4;

				////燃气表
				//bool IsGasMeter();
				////分集水器
				//bool IsWaterSeparator();
			}
			else if (IsGasMeter())
			{
				RelativeLocation.X = Size.X/2-5;
				RelativeLocation.Y = Size.Y;
				RelativeLocation.Z = 5;
			}

			StopSprite->SetRelativeLocation(RelativeLocation);
			StopSprite->SetRelativeRotation(RelativeRotation);
			StopSprite->SetRelativeScale3D(FVector(0.01f, 1.0f, 0.01f));
			StopSprite->SetVisibility(true);
		}
		else
		{
			//取消属性限制
			Native->SetPropertyFlag(FLAG_STOPPED,false);
			
			//关闭停用标识
			AActor * actor = GetRepresentedFurniture(true)->GetRelevanceActor();
			UPaperSpriteComponent * StopSprite = actor->FindComponentByClass<UPaperSpriteComponent>();
			if (StopSprite)
			{
				StopSprite->SetVisibility(false);
			}

		}
		Native->UpdateShow();
	/*	if (Native->bisReplaced)
		{
			Substitute->UpdateShow();
		}*/
	}
}

//获取偏移

 FVector FArmyFurniture::GetOffset()
{
	if (!CanNativeBPReform())
	{
		return FVector(0, 0, 0);
	}

	if (!IsReplaced())
	{
		//return FVector(0, 0, 0);
		auto Native = GetRepresentedFurniture(true);
		if (Native->GetRelevanceActor())
		{
			return Native->GetRelevanceActor()->GetActorLocation() -
				Native->NativeLocation;
		}
		else
		{
			return  FVector(0, 0, 0);
		}
	}

	auto Native = GetRepresentedFurniture(true);
	auto Substute = GetRepresentedFurniture(false);
	return Substute->GetRelevanceActor()->GetActorLocation() -
		Native->NativeLocation;
}

  void FArmyFurniture::SetOffset(FVector NewOffset, bool InfluenceSubstitute )
 {
		 AActor * ActorTomove = this->GetRelevanceActor();
		 //移动对应的2d构件
		 if (this->GetPropertyFlag(FArmyObject::FLAG_POINTOBJ))
		 {
			 this->SetTransform(FTransform(ActorTomove->GetActorRotation(), NativeLocation + NewOffset, FVector(1,1,1)));
		 }
		 else
		 {
			 this->SetTransform(FTransform(ActorTomove->GetActorRotation(), NativeLocation + NewOffset, ActorTomove->GetActorScale3D()));
		 }
	
	     GetRelevanceActor()->SetActorLocation(NativeLocation + NewOffset);
		 SetAltitude(GetRelevanceActor()->GetActorLocation().Z);

		if (InfluenceSubstitute && IsBPreforming()&& IsReplaced() && BreakPointType == NativePoint)
		{
		   GetRepresentedFurniture(false)->SetOffset(NewOffset);
		}
	 FurnitureOffsetedDelegate.ExecuteIfBound(NewOffset);


 }


//开启断点改造

 void FArmyFurniture::StartReforming()
{
	 auto Native = GetRepresentedFurniture(true);
	 auto Substitute = GetRepresentedFurniture(false);
	 Native->bIsBPreforming = true;
	 Substitute->bIsBPreforming = true;

	 Native->bReform = Native->bIsBPreforming;
	 Substitute->bReform = Substitute->bIsBPreforming;

	 if (Native->IsReplaced())
	 {

		 Native->GetRelevanceActor()->Tags.AddUnique(TEXT("BreakPointReformHide"));
		 Substitute->GetRelevanceActor()->Tags.Remove(TEXT("BreakPointReformHide"));

		 Native->SetPropertyFlag(FLAG_STOPPED, true);
		 if (Native->IsReplaced())
		 {
			 Substitute->SetPropertyFlag(FLAG_STOPPED, false);
		 }

		 Native->UpdateShow();
		 Substitute->UpdateShow();

		 //切换选中物体
		 GXREditor->SelectNone(false, false, false);
		 GXREditor->SelectActor(Substitute->GetRelevanceActor(), true, true);

	 }
	 else
	 {
		 Native->GetRelevanceActor()->Tags.Remove(TEXT("NoWidget"));
		 Native->GetRelevanceActor()->Tags.Remove(TEXT("BreakPointReformHide"));
	 }
	
	

	if (!GEngine->OnActorMoved().IsBoundToObject(this))
	{
		 MoveHandle= GEngine->OnActorMoved().AddRaw(this, &FArmyFurniture::OnActorMove);
	}
}

void FArmyFurniture::StopReforming()
{
	auto Native = GetRepresentedFurniture(true);
	auto Substitute = GetRepresentedFurniture(false);

	Native->bIsBPreforming = false;
	Substitute->bIsBPreforming = false;

	Native->bReform = Native->bIsBPreforming;
	Substitute->bReform = Substitute->bIsBPreforming;

	Native->GetRelevanceActor()->Tags.Remove(TEXT("BreakPointReformHide"));
	Native->SetPropertyFlag(FLAG_STOPPED,false);
	if (Native->IsReplaced())
	{
		Substitute->SetPropertyFlag(FLAG_STOPPED,true);
		Substitute->GetRelevanceActor()->Tags.AddUnique(TEXT("BreakPointReformHide"));
		//切换选中物体
		GXREditor->SelectNone(false, false, false);
		GXREditor->SelectActor(Native->GetRelevanceActor(), true, true);

	}

	Native->GetRelevanceActor()->Tags.AddUnique(TEXT("NoWidget"));
	Native->SetOffset(FVector(0, 0, 0));

	Native->UpdateShow();
	Substitute->UpdateShow();

 }

 bool FArmyFurniture::IsBPreforming() { return GetRepresentedFurniture(true)->bIsBPreforming; }

  //只会在原始点位的构件里发生
  void FArmyFurniture::OnActorMove(AActor * actor)
  {
	  if(!CanBPMove())
	  {
		  return;
	  }

	  if (actor == GetRelevanceActor())
	  {
		  SetOffset(actor->GetActorLocation() - NativeLocation);
	  }
	  else if (this->bisReplaced)
	  {
		  auto substute = RelativeFurniture.Pin().ToSharedRef();
		  if (actor == substute->GetRelevanceActor())
		  {
			  SetOffset(actor->GetActorLocation() - NativeLocation);
		  }
	  }
  }

  /*获取应当显示的构件
  * @MustShowNativeFurniture 是不是一定要原本的构件
  */
   bool FArmyFurniture::IsReplaced()
  {
	  return GetRepresentedFurniture(true)->bisReplaced;
  }

    void FArmyFurniture::SetNativeLocation(FVector Location)
   {
		if (isNativeLocationSet)
		{
			return;
		}
	   NativeLocation = Location;
	   if (IsOrignalPoint() && IsReplaced())
	   {
		   GetRepresentedFurniture(false)->SetNativeLocation(Location);
	   }
	   isNativeLocationSet = true;
   }

  TSharedRef<FArmyFurniture> FArmyFurniture::GetRepresentedFurniture(bool MustShowNativeFurniture)
  {
	  if (BreakPointType == EBreakPointType::NewPoint)
	  {
		  return StaticCastSharedRef<FArmyFurniture>(this->AsShared());
	  }

	  if ((BreakPointType == EBreakPointType::NativePoint))
	  {
		  if (MustShowNativeFurniture)
		  {
			  return StaticCastSharedRef<FArmyFurniture>(this->AsShared());
		  }
		  if (RelativeFurniture.IsValid())
		  {
			  return RelativeFurniture.Pin().ToSharedRef();
		  }
		  return StaticCastSharedRef<FArmyFurniture>(this->AsShared());

	  }
	  else
	  {
		  if (!MustShowNativeFurniture)
		  {
			  return StaticCastSharedRef<FArmyFurniture>(this->AsShared());
		  }
		  if (RelativeFurniture.IsValid())
		  {
			  return RelativeFurniture.Pin().ToSharedRef();
		  }
		  return StaticCastSharedRef<FArmyFurniture>(this->AsShared());
	  }
  }


void FArmyFurniture::UpdateShow()
{
	bool NewHide = GetRelevanceActor()->Tags.Contains(TEXT("BreakPointReformHide"));
	OnFurnitureActorShowUpdated.ExecuteIfBound(StaticCastSharedRef<FArmyFurniture>(this->AsShared()), NewHide);

}


//是否可以原始点位断点改造
 bool FArmyFurniture::CanNativeBPReform()
{
	 bool Canmove = false;
		static int32 movecount = sizeof(MoveableBreakPointIDs) / sizeof(EComponentID);
		for (int32 i = 0; i < movecount; i++)
		{
			if (ComponentType == MoveableBreakPointIDs[i])
			{
				Canmove= true;
			}
		}
		bool CanReplace = false;
		static int32 Replaceablecount = sizeof(ReplaceableBreakPointIDs) / sizeof(EComponentID);
		for (int32 i = 0; i < Replaceablecount; i++)
		{
			if (ComponentType == ReplaceableBreakPointIDs[i])
			{
				CanReplace= true;
			}
		}
		return BreakPointType != EBreakPointType::NewPoint
			&& (CanReplace|| Canmove);
}

 //断点是否可移动

  bool FArmyFurniture::CanBPMove() {
	  
	  if (!(CanNativeBPReform() && bIsBPreforming))
	  {
		  return false;
	  }
	  
	  static int32 count = sizeof(MoveableBreakPointIDs) / sizeof(EComponentID);
	  for (int32 i = 0; i < count; i++)
	  {
		  if (ComponentType == MoveableBreakPointIDs[i])
		  {
			  return true;
		  }
	  }
	  return false;
  }

  //断点是否可停用

   bool FArmyFurniture::CanBPStopUsing() { return  CanNativeBPReform(); }

   //断点是否可替换

    bool FArmyFurniture::CanBPReplace()
   {
	   if (!GetRepresentedFurniture(true)->bIsBPreforming)
	   {
		   return false;
	   }
	   if (!CanNativeBPReform())
	   {
		   return false;
	   }

	   static int32 count = sizeof(ReplaceableBreakPointIDs) / sizeof(EComponentID);
	   for (int32 i = 0; i < count; i++)
	   {
		   if (ComponentType == ReplaceableBreakPointIDs[i])
		   {
			   return true;
		   }
	   }
	   return false;
   }



//替换，若果是二次替换,返回被替换掉的构件，否则返回null
 TWeakPtr<FArmyFurniture> FArmyFurniture::Replace(TWeakPtr<FArmyFurniture> NewRelativeFurniture)
{
	auto Furniture = GetRepresentedFurniture(true);
	auto ReturnFArmyFurniture = Furniture-> RelativeFurniture;

	FVector CurrentLocation;
	if (Furniture->bisReplaced && ReturnFArmyFurniture.IsValid())
	{
		CurrentLocation = ReturnFArmyFurniture.Pin()->GetRelevanceActor()->GetActorLocation();
	}
	else
	{
		CurrentLocation= Furniture->GetRelevanceActor()->GetActorLocation();
	}

	if (! NewRelativeFurniture.IsValid())
	{
		Furniture->bisReplaced = false;
		Furniture->GetRelevanceActor()->Tags.Remove(TEXT("BreakPointReformHide"));
		Furniture->SetPropertyFlag(FLAG_STOPPED, false);
		Furniture->RelativeFurniture = nullptr;
		Furniture->UpdateShow();

		if (Furniture->IsReplaced())
		{
			Furniture->RelativeFurniture.Pin()->GetRelevanceActor()->Tags.Remove(TEXT("BreakPointReformHide"));
			Furniture->RelativeFurniture.Pin()->BreakPointType = EBreakPointType::NewPoint;
			Furniture->RelativeFurniture.Pin()->SetPropertyFlag(FLAG_STOPPED, true);
		}

		return ReturnFArmyFurniture;
	}

	Furniture->SetPropertyFlag(FLAG_STOPPED,true);


	Furniture->GetRelevanceActor()->Tags.AddUnique(TEXT("BreakPointReformHide"));
	if (Furniture->RelativeFurniture.IsValid())
	{
		Furniture->RelativeFurniture.Pin()->GetRelevanceActor()->Tags.Remove(TEXT("BreakPointReformHide"));
		Furniture->RelativeFurniture.Pin()->BreakPointType = EBreakPointType::NewPoint;
		Furniture->RelativeFurniture.Pin()->SetPropertyFlag(FLAG_STOPPED, true);
	}

	Furniture->RelativeFurniture = NewRelativeFurniture;

	Furniture->bisReplaced = true;
	NewRelativeFurniture.Pin()->NativeLocation = Furniture-> NativeLocation;
	NewRelativeFurniture.Pin()->RelativeFurnitureID = Furniture->ObjID;
	Furniture->RelativeFurnitureID = NewRelativeFurniture.Pin()->ObjID;
	NewRelativeFurniture.Pin()->bisReplaced = true;
	NewRelativeFurniture.Pin()->bIsBPreforming = true;
	NewRelativeFurniture.Pin()->BreakPointType = EBreakPointType::SubstitutePoint;
	NewRelativeFurniture.Pin()->RelativeFurniture = StaticCastSharedRef<FArmyFurniture>(this->AsShared());

	Furniture->SetOffset(FVector(0, 0, 0),false);
	NewRelativeFurniture.Pin()->SetOffset(CurrentLocation- Furniture->NativeLocation, false);

	Furniture->UpdateShow();
	NewRelativeFurniture.Pin()->UpdateShow();

	return ReturnFArmyFurniture;
}

 bool FArmyFurniture::CanOutPutCalcList()
 {
	 if (BreakPointType == EBreakPointType::SubstitutePoint && !bIsBPreforming)
	 {
		 return false;
	 }

	 if (BreakPointType == EBreakPointType::NativePoint && bIsBPreforming &&bisReplaced)
	 {
		 return false;
	 }

	 if (BreakPointType == EBreakPointType::NativePoint && (!bisReplaced) && (!(GetOffset() == FVector(0,0,0))))
	 {
		 return false;
	 }
	 return true;
 }

 ConstructionPatameters FArmyFurniture::GetConstructionParameter()
 {
	 ConstructionPatameters P;
	 int32 SpaceID = -1;
	 auto Room = GetRoom();

	 if (Room.IsValid())
	 {
		 SpaceID = Room->GetSpaceId();
	 }

	 if (EBreakPointType::NativePoint==BreakPointType && GetOffset()!=FVector::ZeroVector)
	 {
		 //针对原始点位移位的施工项
		 P.SetPureOrignalOffset((EComponentID)ComponentType, SpaceID);
	 }
	 else
	 { 

		int32 GoodsID = FArmyResourceModule::Get().GetResourceManager()->GetItemIDFromActor(GetRelevanceActor());
		P.SetNormalGoodsInfo(GoodsID, SpaceID);
	 }
	 return P;
 }

 TSharedPtr<FArmyRoom> FArmyFurniture::GetRoom()
 {

	 if (!GetRelevanceActor())
	 {
		 return nullptr;
	 }

	 // 遍历所有房间
	 TArray<FObjectWeakPtr> RoomList;
	 FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);

	 for (FObjectWeakPtr It : RoomList)
	 {
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (Room->IsPointInRoom(GetRelevanceActor()->GetActorLocation()))
		{
			return Room;
		}
	 }
	 return nullptr;
 }

 void FArmyFurniture::OnRoomSpaceIDChanged(int32 NewSpaceID)
 {
	 XRConstructionManager::Get()->TryToFindConstructionData(GetUniqueID(), GetConstructionParameter(), nullptr);
 }

 
