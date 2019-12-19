#include "ArmyPass.h"
#include "ArmyPolyline.h"
#include "ArmyRect.h"
#include "ArmyShapeBoardActor.h"
#include "ArmyStyle.h"
#include "ArmyEngineModule.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "ArmySingleDoor.h"
#include "ArmySlidingDoor.h"
#include "ArmyExtrusion/Public/XRExtrusionActor.h"
#include "ArmyHttpModule.h"
#include "Http.h"
#include "ArmyViewportClient.h"
#include "ArmyMath.h"
#include "ArmyGameInstance.h"
#include "ArmyActorConstant.h"
#include "ArmyRulerLine.h"
#include "ResManager.h"
#include "ArmyResourceModule.h"
#include  "ArmyConstructionManager.h"

FArmyPass::FArmyPass()
	: FArmyHardware()
{
	ObjectType = OT_Pass;
	SetPropertyFlag(FLAG_COLLISION, true);	

	StartPass = MakeShareable(new FArmyPolyline());
	StartPass->SetLineType(FArmyPolyline::LineStrip);
	StartPass->SetLineWidth(WALLLINEWIDTH + 0.1f);
	EndPass = MakeShareable(new FArmyPolyline());
	EndPass->SetLineType(FArmyPolyline::LineStrip);
	EndPass->SetLineWidth(WALLLINEWIDTH + 0.1f);

	Spacing = MakeShareable(new FArmyRect());

	LeftAddWallRect = MakeShareable(new FArmyRect());
	LeftAddWallRect->SetCalcUVs(true);
	LeftAddWallRect->Width = 0;
	LeftAddWallRect->Height = 0;
	LeftAddWallRect->LineThickness = WALLLINEWIDTH + 0.1F;

	FillPassRect = MakeShareable(new FArmyRect());
	FillPassRect->SetCalcUVs(true);
	FillPassRect->Width = 0;
	FillPassRect->Height = 0;
	FillPassRect->LineThickness = WALLLINEWIDTH + 0.1F;

	UMaterial* AddMaterial = FArmyEngineModule::Get().GetEngineResource()->GetAddWallMaterial();
	MI_AddWall = UMaterialInstanceDynamic::Create(AddMaterial, nullptr);
	MI_AddWall->AddToRoot();
	MI_AddWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor::Green);
	LeftAddWallRect->MaterialRenderProxy = MI_AddWall->GetRenderProxy(false);
	LeftAddWallRect->bIsFilled = true;
	LeftAddWallRect->SetBaseColor(FLinearColor::Green);

	UMaterial* FillMaterial = FArmyEngineModule::Get().GetEngineResource()->GetAddWallMaterial();
	MI_FillPass = UMaterialInstanceDynamic::Create(FillMaterial, nullptr);
	MI_FillPass->AddToRoot();
	MI_FillPass->SetVectorParameterValue(TEXT("MainColor"), FLinearColor::Green);
	FillPassRect->MaterialRenderProxy = MI_FillPass->GetRenderProxy(false);
	FillPassRect->bIsFilled = true;
	FillPassRect->SetBaseColor(FLinearColor::Green);

	LeftModifyWallRect = MakeShareable(new FArmyRect());
	LeftModifyWallRect->SetCalcUVs(true);
	LeftModifyWallRect->Width = 0;
	LeftModifyWallRect->Height = 0;
	LeftModifyWallRect->LineThickness = WALLLINEWIDTH + 0.1F;
	UMaterial* ModifyMaterial = FArmyEngineModule::Get().GetEngineResource()->GetModifyWallMaterial();
	MI_ModifyWall = UMaterialInstanceDynamic::Create(ModifyMaterial, nullptr);
	MI_ModifyWall->AddToRoot();
	MI_ModifyWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor(FColor(0xFFE305E5)));
	LeftModifyWallRect->MaterialRenderProxy = MI_ModifyWall->GetRenderProxy(false);
	LeftModifyWallRect->bIsFilled = true;
	LeftModifyWallRect->SetBaseColor(FLinearColor(FColor(0xFFE305E5)));

	RightAddWallRect = MakeShareable(new FArmyRect());
	RightAddWallRect->SetCalcUVs(true);
	RightAddWallRect->Width = 0;
	RightAddWallRect->Height = 0;
	RightAddWallRect->LineThickness = WALLLINEWIDTH + 0.1F;

	MI_AddWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor::Green);
	RightAddWallRect->MaterialRenderProxy = MI_AddWall->GetRenderProxy(false);
	RightAddWallRect->bIsFilled = false;
	RightAddWallRect->SetBaseColor(FLinearColor::Green);
	RightAddWallRect->LineThickness = WALLLINEWIDTH + 0.1;

	RightModifyWallRect = MakeShareable(new FArmyRect());
	RightModifyWallRect->SetCalcUVs(true);
	RightModifyWallRect->Width = 0;
	RightModifyWallRect->Height = 0;
	RightModifyWallRect->MaterialRenderProxy = MI_ModifyWall->GetRenderProxy(false);
	RightModifyWallRect->bIsFilled = true;
	RightModifyWallRect->SetBaseColor(FLinearColor(FColor(0xFFE305E5)));
	RightModifyWallRect->LineThickness = WALLLINEWIDTH + 0.1F;

	TopViewRectImagePanel = MakeShareable(new FArmyRect());
	TopViewStartPass = MakeShareable(new FArmyPolyline());
	TopViewEndPass = MakeShareable(new FArmyPolyline());
	TopViewSpacing = MakeShareable(new FArmyRect());

	TopViewRectImagePanel->SetBaseColor(FLinearColor::Black);
	TopViewStartPass->SetBaseColor(FLinearColor::Black);
	TopViewStartPass->SetLineType(FArmyPolyline::LineStrip);
	TopViewEndPass->SetBaseColor(FLinearColor::Black);
	TopViewEndPass->SetLineType(FArmyPolyline::LineStrip);
	TopViewSpacing->SetBaseColor(FLinearColor::Black);

	PostModifyStartLine = MakeShareable(new FArmyLine);
	PostModifyEndLine = MakeShareable(new FArmyLine);
	PostModifyTopLine = MakeShareable(new FArmyLine);
	PostModifyBotLine = MakeShareable(new FArmyLine);

	PostModifyStartLine->SetLineWidth(WALLLINEWIDTH + 0.1);
	PostModifyEndLine->SetLineWidth(WALLLINEWIDTH + 0.1);

	PostModifyTopLine->SetLineWidth(WALLLINEWIDTH + 0.1);
	PostModifyBotLine->SetLineWidth(WALLLINEWIDTH + 0.1);

	PostModifyStartLine->DepthPriority = SDPG_Foreground;
	PostModifyEndLine->DepthPriority = SDPG_Foreground;
	PostModifyTopLine->DepthPriority = SDPG_Foreground;
	PostModifyBotLine->DepthPriority = SDPG_Foreground;
	PostModifyTopLine->SetBaseColor(GVC->GetBackgroundColor());
	PostModifyBotLine->SetBaseColor(GVC->GetBackgroundColor());

	UMaterialInstanceDynamic* MID_BaseColor = FArmyEngineModule::Get().GetEngineResource()->GetUnStageBaseColorMaterial();
	MID_BaseColor->AddToRoot();
	MID_BaseColor->SetVectorParameterValue("BaseColor", FLinearColor(FColor(0xFFE6E6E6)));
	TopViewRectImagePanel->MaterialRenderProxy = MID_BaseColor->GetRenderProxy(false);
	TopViewRectImagePanel->bIsFilled = true;

	//@ 门洞在户型中显示存在填充，需要新创建材质
	UMaterial * M_Pass = FArmyEngineModule::Get().GetEngineResource()->GetPassColorMesh();
	UMaterialInstanceDynamic * MI_PassColor = UMaterialInstanceDynamic::Create(M_Pass, nullptr);
	MI_PassColor->SetVectorParameterValue("PassColor", GVC->GetBackgroundColor());
	MI_PassColor->AddToRoot();
	RectImagePanel->MaterialRenderProxy = MI_PassColor->GetRenderProxy(false);

	OriginRect = MakeShareable(new FArmyRect());
	OriginRect->MaterialRenderProxy = MI_PassColor->GetRenderProxy(false);
	OriginStartPass = MakeShareable(new FArmyPolyline());
	OriginEndPass = MakeShareable(new FArmyPolyline());

	SetName(TEXT("门洞")/* + GetUniqueID().ToString()*/);
	Height = 200;
	Length = 90;
	Width = 12;
	Thickness = 3.5f;
	OuterWidth = 2.f;
	SetState(OS_Normal);

	DirectionPoint->SetState(FArmyPrimitive::OPS_Disable);
	ExtrusionActor = NULL;

	ConstructionItemData = MakeShareable(new FArmyConstructionItemInterface);
}

FArmyPass::FArmyPass(FArmyPass* Copy)
{
	ObjectType = Copy->ObjectType;
	SetName(Copy->GetName());
	bBoundingBox = Copy->bBoundingBox;
	State = Copy->GetState();
	PropertyFlag = Copy->PropertyFlag;

	DirectionPoint = MakeShareable(new FArmyEditPoint(Copy->DirectionPoint.Get()));
	AttachModelName = Copy->AttachModelName;

	StartPass = MakeShareable(new FArmyPolyline(Copy->StartPass.Get()));
	EndPass = MakeShareable(new FArmyPolyline(Copy->EndPass.Get()));
	Spacing = MakeShareable(new FArmyRect(Copy->Spacing.Get()));

	RectImagePanel = MakeShareable(new FArmyRect(Copy->RectImagePanel.Get()));
	RectImagePanel->SetBaseColor(GVC->GetBackgroundColor());
	RectImagePanel->LineThickness = Copy->RectImagePanel->LineThickness;

	LeftAddWallRect = MakeShareable(new FArmyRect());
	LeftAddWallRect->SetCalcUVs(Copy->LeftAddWallRect->GetIfCalcUVs());
	LeftAddWallRect->Width = 0;
	LeftAddWallRect->Height = 0;
	LeftAddWallRect->LineThickness = Copy->LeftAddWallRect->LineThickness;

	FillPassRect = MakeShareable(new FArmyRect());
	FillPassRect->SetCalcUVs(true);
	FillPassRect->Width = 0;
	FillPassRect->Height = 0;
	FillPassRect->LineThickness = Copy->LeftAddWallRect->LineThickness;

	UMaterial* FillMaterial = FArmyEngineModule::Get().GetEngineResource()->GetAddWallMaterial();
	MI_FillPass = UMaterialInstanceDynamic::Create(FillMaterial, nullptr);
	MI_FillPass->AddToRoot();
	MI_FillPass->SetVectorParameterValue(TEXT("MainColor"), FLinearColor::Green);
	FillPassRect->MaterialRenderProxy = MI_FillPass->GetRenderProxy(false);
	FillPassRect->bIsFilled = true;
	FillPassRect->SetBaseColor(FLinearColor::Green);

	/*LeftAddWallRect->XUVRepeatDist = 80.f;
	LeftAddWallRect->YUVRepeatDist = 30.f;*/
	UMaterial* AddMaterial = FArmyEngineModule::Get().GetEngineResource()->GetAddWallMaterial();
	MI_AddWall = UMaterialInstanceDynamic::Create(AddMaterial, nullptr);
	MI_AddWall->AddToRoot();
	MI_AddWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor::Green);
	LeftAddWallRect->MaterialRenderProxy = MI_AddWall->GetRenderProxy(false);
	LeftAddWallRect->bIsFilled = true;
	LeftAddWallRect->SetBaseColor(FLinearColor::Green);

	LeftModifyWallRect = MakeShareable(new FArmyRect());
	LeftModifyWallRect->SetCalcUVs(Copy->LeftModifyWallRect->GetIfCalcUVs());
	LeftModifyWallRect->Width = 0;
	LeftModifyWallRect->Height = 0;
	LeftModifyWallRect->LineThickness = Copy->LeftModifyWallRect->LineThickness;
	/*LeftModifyWallRect->XUVRepeatDist = 80.f;
	LeftModifyWallRect->YUVRepeatDist = 30.f;*/
	//LeftModifyWallRect->SetBaseColor(FLinearColor::Blue);
	UMaterial* ModifyMaterial = FArmyEngineModule::Get().GetEngineResource()->GetModifyWallMaterial();
	MI_ModifyWall = UMaterialInstanceDynamic::Create(ModifyMaterial, nullptr);
	MI_ModifyWall->AddToRoot();
	MI_ModifyWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor(FColor(0xFFE305E5)));
	LeftModifyWallRect->MaterialRenderProxy = MI_ModifyWall->GetRenderProxy(false);
	LeftModifyWallRect->bIsFilled = true;
	LeftModifyWallRect->SetBaseColor(FLinearColor(FColor(0xFFE305E5)));

	RightAddWallRect = MakeShareable(new FArmyRect());
	RightAddWallRect->SetCalcUVs(Copy->RightAddWallRect->GetIfCalcUVs());
	RightAddWallRect->Width = 0;
	RightAddWallRect->Height = 0;
	RightAddWallRect->LineThickness = Copy->RightAddWallRect->LineThickness;
	/*RightAddWallRect->XUVRepeatDist = 80.f;
	RightAddWallRect->YUVRepeatDist = 30.f;*/

	MI_AddWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor::Green);
	RightAddWallRect->MaterialRenderProxy = MI_AddWall->GetRenderProxy(false);
	RightAddWallRect->bIsFilled = false;
	RightAddWallRect->SetBaseColor(FLinearColor::Green);

	RightModifyWallRect = MakeShareable(new FArmyRect());
	RightModifyWallRect->SetCalcUVs(Copy->RightModifyWallRect->GetIfCalcUVs());
	RightModifyWallRect->Width = 0;
	RightModifyWallRect->Height = 0;
	RightModifyWallRect->LineThickness = Copy->RightModifyWallRect->LineThickness;
	/*RightModifyWallRect->XUVRepeatDist = 80.f;
	RightModifyWallRect->YUVRepeatDist = 30.f;*/
	RightModifyWallRect->MaterialRenderProxy = MI_ModifyWall->GetRenderProxy(false);
	RightModifyWallRect->bIsFilled = true;
	RightModifyWallRect->SetBaseColor(FLinearColor(FColor(0xFFE305E5)));

	TopViewRectImagePanel = MakeShareable(new FArmyRect(Copy->TopViewRectImagePanel.Get()));
	TopViewStartPass = MakeShareable(new FArmyPolyline(Copy->TopViewStartPass.Get()));
	TopViewEndPass = MakeShareable(new FArmyPolyline(Copy->TopViewStartPass.Get()));
	TopViewSpacing = MakeShareable(new FArmyRect(Copy->TopViewSpacing.Get()));

	PostModifyStartLine = MakeShareable(new FArmyLine);
	PostModifyEndLine = MakeShareable(new FArmyLine);
	PostModifyTopLine = MakeShareable(new FArmyLine);
	PostModifyBotLine = MakeShareable(new FArmyLine);

	PostModifyStartLine->SetLineWidth(WALLLINEWIDTH + 0.1);
	PostModifyEndLine->SetLineWidth(WALLLINEWIDTH + 0.1);

	PostModifyTopLine->SetLineWidth(WALLLINEWIDTH + 0.1);
	PostModifyBotLine->SetLineWidth(WALLLINEWIDTH + 0.1);

	PostModifyTopLine->SetBaseColor(GVC->GetBackgroundColor());
	PostModifyBotLine->SetBaseColor(GVC->GetBackgroundColor());

	SetPos(Copy->GetPos());
	Direction = Copy->Direction;
	Length = Copy->Length;
	Width = Copy->Width;
	Height = Copy->Height;
	Thickness = Copy->Thickness;
	OuterWidth = Copy->GetOuterWidth();

	LinkFirstLine = Copy->LinkFirstLine;
	LinkSecondLine = Copy->LinkSecondLine;
	FirstRelatedRoom = Copy->FirstRelatedRoom;
	SecondRelatedRoom = Copy->SecondRelatedRoom;
	ExtrusionActor = Copy->ExtrusionActor;

	ConstructionItemData = MakeShareable(new FArmyConstructionItemInterface);

	OriginRect = MakeShareable(new FArmyRect(Copy->RectImagePanel.Get()));
	OriginStartPass = MakeShareable(new FArmyPolyline(Copy->StartPass.Get()));
	OriginEndPass = MakeShareable(new FArmyPolyline(Copy->EndPass.Get()));

	Update();
}

FArmyPass::~FArmyPass()
{

}
void FArmyPass::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	Update();
	FArmyHardware::SerializeToJson(JsonWriter);

	JsonWriter->WriteValue("ObjectType", ObjectType == OT_Pass ? TEXT("OT_Pass") : TEXT("OT_DoorHole"));

	JsonWriter->WriteValue("MatType", GetMatType());

	JsonWriter->WriteValue("pos", GetPos().ToString());
	JsonWriter->WriteValue("direction", GetDirection().ToString());
	JsonWriter->WriteValue("length", GetLength());
	JsonWriter->WriteValue("width", GetWidth());
	JsonWriter->WriteValue("height", GetHeight());
	JsonWriter->WriteValue("outerWidth", GetOuterWidth());
	JsonWriter->WriteValue("thickness", GetThickness());
	JsonWriter->WriteValue("bRightOpen", IsRightOpen());

	JsonWriter->WriteValue("leftAddPos", LeftAddWallRect->Pos.ToString());
	JsonWriter->WriteValue("leftAddXDirection", LeftAddWallRect->XDirection.ToString());
	JsonWriter->WriteValue("leftAddYDirection", LeftAddWallRect->YDirection.ToString());
	JsonWriter->WriteValue("leftAddWidth", LeftAddWallRect->Width);
	JsonWriter->WriteValue("leftAddHeight", LeftAddWallRect->Height);

	JsonWriter->WriteValue("leftModifyPos", LeftModifyWallRect->Pos.ToString());
	JsonWriter->WriteValue("leftModifyXDirection", LeftModifyWallRect->XDirection.ToString());
	JsonWriter->WriteValue("leftModifyYDirection", LeftModifyWallRect->YDirection.ToString());
	JsonWriter->WriteValue("leftModifyWidth", LeftModifyWallRect->Width);
	JsonWriter->WriteValue("leftModifyHeight", LeftModifyWallRect->Height);

	JsonWriter->WriteValue("rightAddPos", RightAddWallRect->Pos.ToString());
	JsonWriter->WriteValue("rightAddXDirection", RightAddWallRect->XDirection.ToString());
	JsonWriter->WriteValue("rightAddYDirection", RightAddWallRect->YDirection.ToString());
	JsonWriter->WriteValue("rightAddWidth", RightAddWallRect->Width);
	JsonWriter->WriteValue("rightAddHeight", RightAddWallRect->Height);

	JsonWriter->WriteValue("rightModifyPos", RightModifyWallRect->Pos.ToString());
	JsonWriter->WriteValue("rightModifyXDirection", RightModifyWallRect->XDirection.ToString());
	JsonWriter->WriteValue("rightModifyYDirection", RightModifyWallRect->YDirection.ToString());
	JsonWriter->WriteValue("rightModifyWidth", RightModifyWallRect->Width);
	JsonWriter->WriteValue("rightModifyHeight", RightModifyWallRect->Height);

	JsonWriter->WriteValue("bModifyWall", bModify);

	JsonWriter->WriteValue("AddWallType", AddDoorHoleMatType.ToString());
	JsonWriter->WriteValue("ModifyWallType", ModifyDoorHoleMatType.ToString());

	JsonWriter->WriteValue("bRectIsFilled", RectImagePanel->bIsFilled);

	/**@欧石楠 存储施工项*/
	ConstructionItemData->SerializeToJson(JsonWriter);

	JsonWriter->WriteValue("bIfGenerateModel", bIfGenerateModel);
	JsonWriter->WriteValue("bIfFillPass", bIfFillPass);
	JsonWriter->WriteValue("SaleID", SaleID);
    JsonWriter->WriteValue("thumbnailUrl", ThumbnailUrl);

	JsonWriter->WriteValue("originLength", OriginLength);
	JsonWriter->WriteValue("originHeight", OriginHeight);
	JsonWriter->WriteValue("originPoint", OriginPoint.ToString());
	JsonWriter->WriteValue("originStartPoint", OriginStartPoint.ToString());
	JsonWriter->WriteValue("originEndPoint", OriginEndPoint.ToString());

	JsonWriter->WriteValue("OriginRectbIsFilled", OriginRect->bIsFilled);
	JsonWriter->WriteValue("OriginRectDrawBoundray", OriginRect->DrawBoundray);
	
	JsonWriter->WriteValue("OriginRectPos", OriginRect->Pos.ToString());
	JsonWriter->WriteValue("OriginRectInCenterPos", OriginRect->InCenterPos.ToString());
	JsonWriter->WriteValue("OriginRectWidth", OriginRect->Width);
	JsonWriter->WriteValue("OriginRectHeight", OriginRect->Height);

	JsonWriter->WriteValue("OriginRectFillColor", OriginRect->FillColor.ToString());
	JsonWriter->WriteValue("OriginRectXDirection", OriginRect->XDirection.ToString());
	JsonWriter->WriteValue("OriginRectYDirection", OriginRect->YDirection.ToString());

	JsonWriter->WriteValue("OriginRectLineThickness", OriginRect->LineThickness);
	JsonWriter->WriteValue("OriginRectXUVRepeatDist", OriginRect->XUVRepeatDist);
	JsonWriter->WriteValue("OriginRectYUVRepeatDist", OriginRect->YUVRepeatDist);

	JsonWriter->WriteObjectStart("OriginStartPass");
	JsonWriter->WriteArrayStart("OriginStartPassVertices");
	for (auto& V : OriginStartPass->GetAllVertices())
	{
		JsonWriter->WriteValue(V.ToString());
	}
	JsonWriter->WriteArrayEnd();
	JsonWriter->WriteValue("OriginStartELineType", OriginStartPass->GetLineType());
	JsonWriter->WriteValue("OriginStartBasePoint", OriginStartPass->GetBasePoint().ToString());
	JsonWriter->WriteValue("OriginStartTransform", OriginStartPass->GetTransform().ToString());
	JsonWriter->WriteValue("OriginStartLineWidth", OriginStartPass->GetLineWidth());
	JsonWriter->WriteValue("OriginStartColor", OriginStartPass->GetColor().ToString());
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteObjectStart("OriginEndPass");
	JsonWriter->WriteArrayStart("OriginEndPassELineType");
	for (auto& V : OriginStartPass->GetAllVertices())
	{
		JsonWriter->WriteValue(V.ToString());
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteValue("OriginEndPassELineType", OriginStartPass->GetLineType());
	JsonWriter->WriteValue("OriginEndPassBasePoint", OriginStartPass->GetBasePoint().ToString());
	JsonWriter->WriteValue("OriginEndPassTransform", OriginStartPass->GetTransform().ToString());
	JsonWriter->WriteValue("OriginEndPassLineWidth", OriginStartPass->GetLineWidth());
	JsonWriter->WriteValue("OriginEndPassColor", OriginStartPass->GetColor().ToString());
	JsonWriter->WriteObjectEnd();


	if (ExtrusionActor)
	{
		UXRResourceManager * ResMg = FArmyResourceModule::Get().GetResourceManager();
		//TSharedPtr<FContentItemSpace::FContentItem> item = ResMg->GetContentItemFromID(HoleWindow->GetUniqueID());
		TSharedPtr<FContentItemSpace::FContentItem> ResultSynData = ResMg->GetContentItemFromID(SaleID);
		if (ResultSynData.IsValid())
		{
			//ResultSynData->SerializeToJson(JsonWriter);
			GGI->DesignEditor->SaveFileList.AddUnique(SaleID);
		}
	}

	//@打扮家 XRLightmass 序列化LightMapID
	if (ExtrusionActor && ExtrusionActor->MeshComponent->bBuildStaticLighting)
	{
		LightMapID = ExtrusionActor->GetLightMapGUIDFromSMC();
		JsonWriter->WriteValue("LightMapID", LightMapID.ToString());
	}

	SERIALIZEREGISTERCLASS(JsonWriter, FArmyPass)
}

void FArmyPass::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	bDeserialization = true;
	FArmyHardware::Deserialization(InJsonData);

	ObjectType = (InJsonData->GetStringField("ObjectType") == "OT_Pass") ? OT_Pass : OT_DoorHole;

	FVector Pos, TempDirection;
	Pos.InitFromString(InJsonData->GetStringField("pos"));
	TempDirection.InitFromString(InJsonData->GetStringField("direction"));
	SetPos(Pos);
	SetDirection(TempDirection);
	SetLength(InJsonData->GetNumberField("length"));
	SetWidth(InJsonData->GetNumberField("Width"));
	SetHeight(InJsonData->GetNumberField("height"));
	SetOuterWidth(InJsonData->GetNumberField("outerWidth"));
	SetThickness(InJsonData->GetNumberField("thickness"));
	SetRightOpen(InJsonData->GetBoolField("bRightOpen"));

	SetMatType(InJsonData->GetStringField(TEXT("MatType")));

	bModify = InJsonData->GetBoolField("bModifyWall");

	LeftAddWallRect->Pos.InitFromString(InJsonData->GetStringField("leftAddPos"));
	LeftAddWallRect->XDirection.InitFromString(InJsonData->GetStringField("leftAddXDirection"));
	LeftAddWallRect->YDirection.InitFromString(InJsonData->GetStringField("leftAddYDirection"));
	LeftAddWallRect->Width = InJsonData->GetNumberField("leftAddWidth");
	LeftAddWallRect->Height = InJsonData->GetNumberField("leftAddHeight");

	LeftModifyWallRect->Pos.InitFromString(InJsonData->GetStringField("leftModifyPos"));
	LeftModifyWallRect->XDirection.InitFromString(InJsonData->GetStringField("leftModifyXDirection"));
	LeftModifyWallRect->YDirection.InitFromString(InJsonData->GetStringField("leftModifyYDirection"));
	LeftModifyWallRect->Width = InJsonData->GetNumberField("leftModifyWidth");
	LeftModifyWallRect->Height = InJsonData->GetNumberField("leftModifyHeight");

	RightAddWallRect->Pos.InitFromString(InJsonData->GetStringField("rightAddPos"));
	RightAddWallRect->XDirection.InitFromString(InJsonData->GetStringField("rightAddXDirection"));
	RightAddWallRect->YDirection.InitFromString(InJsonData->GetStringField("rightAddYDirection"));
	RightAddWallRect->Width = InJsonData->GetNumberField("rightAddWidth");
	RightAddWallRect->Height = InJsonData->GetNumberField("rightAddHeight");

	RightModifyWallRect->Pos.InitFromString(InJsonData->GetStringField("rightModifyPos"));
	RightModifyWallRect->XDirection.InitFromString(InJsonData->GetStringField("rightModifyXDirection"));
	RightModifyWallRect->YDirection.InitFromString(InJsonData->GetStringField("rightModifyYDirection"));
	RightModifyWallRect->Width = InJsonData->GetNumberField("rightModifyWidth");
	RightModifyWallRect->Height = InJsonData->GetNumberField("rightModifyHeight");

	SetAddDoorHoleMatType(FText::FromString(InJsonData->GetStringField("AddWallType")));
	SetModifyDoorHoleMatType(FText::FromString(InJsonData->GetStringField("ModifyWallType")));

	RectImagePanel->bIsFilled = InJsonData->GetBoolField("bRectIsFilled");

	/**@欧石楠 读取施工项*/
	ConstructionItemData->Deserialization(InJsonData);

	OriginLength = InJsonData->GetNumberField("originLength");
	OriginHeight = InJsonData->GetNumberField("originHeight");
	UpdateVerticalState();
	OriginPoint.InitFromString(InJsonData->GetStringField("originPoint"));
	OriginStartPoint.InitFromString(InJsonData->GetStringField("originStartPoint"));
	OriginEndPoint.InitFromString(InJsonData->GetStringField("originEndPoint"));

	OriginRect->bIsFilled = InJsonData->GetBoolField("OriginRectbIsFilled");
	OriginRect->DrawBoundray = InJsonData->GetBoolField("OriginRectDrawBoundray");
	OriginRect->Pos.InitFromString(InJsonData->GetStringField("OriginRectPos"));
	OriginRect->InCenterPos.InitFromString(InJsonData->GetStringField("OriginRectInCenterPos"));
	OriginRect->Width = InJsonData->GetNumberField("OriginRectWidth");
	OriginRect->Height = InJsonData->GetNumberField("OriginRectHeight");
	OriginRect->FillColor.InitFromString(InJsonData->GetStringField("OriginRectFillColor"));
	OriginRect->XDirection.InitFromString(InJsonData->GetStringField("OriginRectXDirection"));
	OriginRect->YDirection.InitFromString(InJsonData->GetStringField("OriginRectYDirection"));
	OriginRect->LineThickness = InJsonData->GetNumberField("OriginRectLineThickness");
	OriginRect->XUVRepeatDist = InJsonData->GetNumberField("OriginRectXUVRepeatDist");
	OriginRect->YUVRepeatDist = InJsonData->GetNumberField("OriginRectYUVRepeatDist");

	const TSharedPtr<FJsonObject> OriginStartPassJObj = InJsonData->GetObjectField("OriginStartPass");
	if (OriginStartPassJObj.IsValid())
	{
		const TArray< TSharedPtr<FJsonValue> > VJsonArray = OriginStartPassJObj->GetArrayField("OriginStartPassVertices");
		for (auto& V : VJsonArray)
		{
			FVector PV;
			PV.InitFromString(V->AsString());
			OriginStartPass->AddVertice(PV);
		}
		OriginStartPass->SetLineType(FArmyPolyline::ELineType(OriginStartPassJObj->GetIntegerField("OriginStartPassELineType")));
		FVector PV;
		PV.InitFromString(OriginStartPassJObj->GetStringField("OriginStartPassBasePoint"));
		OriginStartPass->SetBasePoint(PV);
		FTransform Trans;
		Trans.InitFromString(OriginStartPassJObj->GetStringField("OriginStartPassTransform"));
		OriginStartPass->SetTransform(Trans);
		OriginStartPass->SetLineWidth(OriginStartPassJObj->GetNumberField("OriginStartPassLineWidth"));
		FLinearColor color;
		color.InitFromString(OriginStartPassJObj->GetStringField("OriginStartPassColor"));
		OriginStartPass->SetColor(color);
	}
	const TSharedPtr<FJsonObject> OriginEndPassJObj = InJsonData->GetObjectField("OriginEndPass");
	if (OriginEndPassJObj.IsValid())
	{
		const TArray< TSharedPtr<FJsonValue> > OriginEndPassVJsonArray = OriginEndPassJObj->GetArrayField("OriginEndPassVertices");
		for (auto& V : OriginEndPassVJsonArray)
		{
			FVector PV;
			PV.InitFromString(V->AsString());
			OriginStartPass->AddVertice(PV);
		}
		OriginEndPass->SetLineType(FArmyPolyline::ELineType(OriginEndPassJObj->GetIntegerField("OriginEndPassELineType")));
		FVector PV;
		PV.InitFromString(OriginEndPassJObj->GetStringField("OriginEndPassBasePoint"));
		OriginEndPass->SetBasePoint(PV);
		FTransform Trans;
		Trans.InitFromString(OriginEndPassJObj->GetStringField("OriginEndPassTransform"));
		OriginEndPass->SetTransform(Trans);
		OriginEndPass->SetLineWidth(OriginEndPassJObj->GetNumberField("OriginEndPassLineWidth"));
		FLinearColor color;
		color.InitFromString(OriginEndPassJObj->GetStringField("OriginEndPassColor"));
		OriginEndPass->SetColor(color);
	}

	bIfGenerateModel = InJsonData->GetBoolField("bIfGenerateModel");
	bIfFillPass = InJsonData->GetBoolField("bIfFillPass");
    InJsonData->TryGetNumberField("SaleID", SaleID);
    InJsonData->TryGetStringField("thumbnailUrl", ThumbnailUrl);

	SetIfFillPass(bIfFillPass);
	//bModifyType = InJsonData->GetBoolField("bModifyType");

	//@打扮家 XRLightmass 反序列化LightMapID
	FString LightGUIDStr = "";
	InJsonData->TryGetStringField("LightMapID", LightGUIDStr);
	FGuid::Parse(LightGUIDStr, LightMapID);

	Update();

	bool bCapture = CaptureDoubleLine(Pos, E_LayoutModel);

	if (SaleID != -1)
	{
		ReqModifyMaterialSaleID();
	}
	////@郭子阳 请求施工项
	//if (HasGoods() && ExtrusionActor)
	//{
	//	UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
	//	auto ContentItem = ResMgr->GetContentItemFromItemID(EResourceType::Texture, SaleID);
	//	if (ContentItem.IsValid())
	//	{
	//		ReplaceGoods(ContentItem, nullptr);
	//	}
	//	else
	//	{
	//		//@郭子阳
	//		//请求3D模式施工项
	//		XRConstructionManager::Get()->TryToFindConstructionData(this->GetUniqueID(), *(this->GetConstructionParameter().Get()), nullptr);
	//	}
	//}

	bDeserialization = false;

}
void FArmyPass::SetState(EObjectState InState)
{
	State = InState;

	switch (InState)
	{
	case OS_Normal:
		OriginStartPass->SetBaseColor(FLinearColor::White);
		OriginEndPass->SetBaseColor(FLinearColor::White);
		StartPass->SetBaseColor(FLinearColor::White);
		EndPass->SetBaseColor(FLinearColor::White);
		Spacing->SetBaseColor(FLinearColor::White);
		DeselectPoints();
		MI_AddWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor::Green);
		MI_ModifyWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor(FColor(0xFFE305E5)));

		LeftAddWallRect->SetBaseColor(FLinearColor::Green);
		RightAddWallRect->SetBaseColor(FLinearColor::Green);
		LeftModifyWallRect->SetBaseColor(FLinearColor(FColor(0xFFE305E5)));
		RightModifyWallRect->SetBaseColor(FLinearColor(FColor(0xFFE305E5)));

		if (bIfFillPass)
		{
			MI_FillPass->SetVectorParameterValue(TEXT("MainColor"), FLinearColor::Green);
			FillPassRect->SetBaseColor(FLinearColor::Green);
		}

		break;

	case OS_Selected:
		StartPass->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		EndPass->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		Spacing->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));

		MI_AddWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor(FColor(0xFFFF9800)));
		MI_ModifyWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor(FColor(0xFFFF9800)));

		LeftAddWallRect->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		RightAddWallRect->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		LeftModifyWallRect->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		RightModifyWallRect->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));

		if (bIfFillPass)
		{
			MI_FillPass->SetVectorParameterValue(TEXT("MainColor"), FLinearColor(FColor(0xFFFF9800)));
			FillPassRect->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		}

		break;

	default:
		break;
	}
}
float FArmyPass::GetHorizantalAddLength()
{
	float TotalLength = 0;
	if (!FMath::IsNearlyZero(LeftAddWallRect->Width))
	{
		TotalLength += LeftAddWallRect->Width;		
	}
	if (!FMath::IsNearlyZero(RightAddWallRect->Width))
	{
		TotalLength += RightAddWallRect->Width;
	}
	return TotalLength;
}
float FArmyPass::GetHorizantalModifyLength()
{
	float TotalLength = 0;
	if (!FMath::IsNearlyZero(LeftModifyWallRect->Width))
	{
		TotalLength += LeftModifyWallRect->Width;
	}
	if (!FMath::IsNearlyZero(RightModifyWallRect->Width))
	{
		TotalLength += RightModifyWallRect->Width;
	}
	return TotalLength;
}
void FArmyPass::DrawOrigin(FPrimitiveDrawInterface* PDI, const FSceneView* View, bool InEdge)
{
	OriginRect->Draw(PDI, View);
	if (InEdge)
	{
		OriginStartPass->Draw(PDI, View);
		OriginEndPass->Draw(PDI, View);
	}
}
void FArmyPass::DrawNormal(FPrimitiveDrawInterface* PDI, const FSceneView* View,bool InEdge)
{
	RectImagePanel->Draw(PDI, View);
	if (InEdge)
	{
		StartPass->Draw(PDI, View);
		EndPass->Draw(PDI, View);
	}

	//FArmyHardware::Draw(PDI, View);
}
void FArmyPass::DrawTopView(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	/**@欧石楠 只有在没有填补门洞的时候，平面视图的绘制才显示*/
	if (!bIfFillPass)
	{
		TopViewRectImagePanel->Draw(PDI, View);
		TopViewStartPass->Draw(PDI, View);
		TopViewEndPass->Draw(PDI, View);
		TopViewSpacing->Draw(PDI, View);
	}
}
void FArmyPass::DrawOnlyDelete(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetType() == OT_DoorHole)
	{
		DrawNormal(PDI,View);
	}
	else
	{
		DrawOrigin(PDI, View);
	}

	if (LeftModifyWallRect->Width > 0)
	{
		LeftModifyWallRect->Draw(PDI, View);
	}
	if (RightModifyWallRect->Width > 0)
	{
		RightModifyWallRect->Draw(PDI, View);
	}
}
void FArmyPass::DrawOnlyAdd(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	DrawPost(PDI,View);
	if (LeftAddWallRect->Width > 0)
		LeftAddWallRect->Draw(PDI, View);
	if (RightAddWallRect->Width > 0)
		RightAddWallRect->Draw(PDI, View);

	if (bIfFillPass)
	{
		FillPassRect->Draw(PDI, View);
	}
}
void FArmyPass::DrawAll(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (bIfFillPass)
	{
		FillPassRect->Draw(PDI, View);
	}
	else
	{
		DrawNormal(PDI, View);
		if (LeftAddWallRect->Width > 0)
			LeftAddWallRect->Draw(PDI, View);
		if (RightAddWallRect->Width > 0)
			RightAddWallRect->Draw(PDI, View);

		if (LeftModifyWallRect->Width > 0)
		{
			LeftModifyWallRect->Draw(PDI, View);
		}
		if (RightModifyWallRect->Width > 0)
		{
			RightModifyWallRect->Draw(PDI, View);
		}
	}
}
void FArmyPass::DrawPost(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	//修改深度偏移
	if (GetPropertyFlag(FLAG_LAYOUT) && GetType() != OT_DoorHole)
	{
		DrawNormal(PDI,View);
	}
	else
	{
		PostModifyStartLine->DepthPriority = SDPG_MAX;
		PostModifyEndLine->DepthPriority = SDPG_MAX;
		PostModifyTopLine->DepthPriority = SDPG_MAX;
		PostModifyBotLine->DepthPriority = SDPG_MAX;

		PostModifyStartLine->Draw(PDI, View);
		PostModifyEndLine->Draw(PDI, View);
		PostModifyTopLine->Draw(PDI, View);
		PostModifyBotLine->Draw(PDI, View);
	}
}

//获取施工项查询Key
TSharedPtr<ObjectConstructionKey> FArmyPass::GetConstructionKey()
{

	auto Key = MakeShared<ObjectConstructionKey>(GetUniqueID(), *GetConstructionParameter().Get());
	//ObjectConstructionKey Key(GetUniqueID(), P);
	return Key;

}

void FArmyPass::ConstructionData(TArray<struct FArmyGoods>& ArtificialData)
{
	if (!HasGoods() || !bIfGenerateModel)
	{
		return;
	}
	FArmyGoods goods;
	goods.GoodsId = SaleID;
	goods.Type = 1;

	// 施工项
	TSharedPtr<FArmyConstructionItemInterface> tempConstructoinData = XRConstructionManager::Get()->GetSavedCheckedData(GetUniqueID(), *GetConstructionParameter());
	for (auto It : tempConstructoinData->CheckedId)
	{
		int32 Quotaindex = goods.QuotaData.Add(FArmyQuota());
		FArmyQuota &Quota = goods.QuotaData[Quotaindex];
		Quota.QuotaId = It.Key;
		for (auto PropertyIt : It.Value)
		{
			Quota.QuotaProperties.Add(FArmyPropertyValue(PropertyIt.Key, PropertyIt.Value));
		}
	}


	//FArmySpaceMsg SpaceMsg;
	if (FirstRelatedRoom.IsValid() && FirstRelatedRoom->GetSpaceNameLabel().IsValid())
	{
		goods.SpaceMsg.SpaceName1 = FirstRelatedRoom->GetSpaceName();
	}
	if (SecondRelatedRoom.IsValid() && SecondRelatedRoom->GetSpaceNameLabel().IsValid())
	{
		goods.SpaceMsg.SpaceName2 = SecondRelatedRoom->GetSpaceName();
	}

	goods.DoorMsg.DoorWidth = GetLength() / 100.f;
	goods.DoorMsg.DoorHeight = GetHeight() / 100.f;
	goods.DoorMsg.WallThickness = GetWidth() / 100.f;
	//goods.DoorMsg.SwitchingDirection = GetOpenDirectionID();

	int32 GoodsIndex;
	//垭口暂不合并，如果要合并需要考虑DoorMsg的数据的合并规则
	//if (!ArtificialData.Find(goods, GoodsIndex))
	//{
	GoodsIndex = ArtificialData.Add(goods);
	//}
	FArmyGoods& Goods = ArtificialData[GoodsIndex];
	//合并施工项数量
	for (int32 i = 0; i < ArtificialData[GoodsIndex].QuotaData.Num(); i++)
	{
		ArtificialData[GoodsIndex].QuotaData[i].Dosage += 1;
	}

	Goods.Param.D += 1;
	Goods.Param.L += Goods.DoorMsg.DoorHeight * 2 + Goods.DoorMsg.DoorWidth;
}

TSharedPtr<ConstructionPatameters> FArmyPass::GetConstructionParameter()
{
	auto  P = MakeShared<ConstructionPatameters>();
	P->SetNormalGoodsInfo(GetSaleID());
	return P;
}
void FArmyPass::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (FArmyObject::GetDrawModel(MODE_CONSTRUCTION))
		{
			if ((!FArmyObject::GetDrawModel(MODE_MODIFYADD) && InWallType == 1) ||//施工图不显示新增墙，并且该垭口在新增墙上时，不绘制
				(FArmyObject::GetDrawModel(MODE_DELETEWALLPOST) && !bGenerate3D)||//拆除墙上带了门洞，并且把门洞也拆除了，在拆除后的效果中不要显示门洞
				(GetPropertyFlag(FArmyObject::FLAG_LAYOUT) && FArmyObject::GetDrawModel(MODE_ADDWALLPOST) && bIfFillPass))
			{
				return;
			}
			else if (FArmyObject::GetDrawModel(MODE_ADDWALLPOST) && FArmyObject::GetDrawModel(MODE_DELETEWALLPOST))
			{
				DrawPost(PDI,View);
			}
			else if (FArmyObject::GetDrawModel(MODE_MODIFYADD) && !FArmyObject::GetDrawModel(MODE_ADDWALLPOST))
			{
				DrawOnlyAdd(PDI, View);
			}
			else if (FArmyObject::GetDrawModel(MODE_MODIFYDELETE) && !FArmyObject::GetDrawModel(MODE_DELETEWALLPOST))
			{
				DrawOnlyDelete(PDI,View);
			}
			else
			{
				DrawPost(PDI, View);
			}
		}
		else
		{
			if (GetPropertyFlag(FLAG_LAYOUT) && (bIfFillPass || GetType() == OT_DoorHole))
			{
				return;
			}
			else if (FArmyObject::GetDrawModel(MODE_TOPVIEW) || FArmyObject::GetDrawModel(MODE_CEILING))
			{
				DrawTopView(PDI,View);
			}
			if (GetPropertyFlag(FLAG_LAYOUT))
			{
				DrawNormal(PDI,View);
			}
			else if (FArmyObject::GetDrawModel(MODE_MODIFYADD) && FArmyObject::GetDrawModel(MODE_MODIFYDELETE))
			{
				DrawAll(PDI,View);
			}
		}
		FArmyHardware::Draw(PDI, View);

		///**@欧石楠 如果是拆改后被填补的门洞，则不进行绘制*/		
		//if (GetPropertyFlag(FArmyObject::FLAG_LAYOUT) && bIfFillPass)
		//{
		//	/** 不能影响图纸模式下的绘制*/
		//	if (!(FArmyObject::GetDrawModel(MODE_CONSTRUCTION) && FArmyObject::GetDrawModel(MODE_MODIFYADD)))
		//	{
		//		return;
		//	}
		//}


		//if (FArmyObject::GetDrawModel(MODE_TOPVIEW) || FArmyObject::GetDrawModel(MODE_CEILING))
		//{
		//	/**@欧石楠 只有在没有填补门洞的时候，平面视图的绘制才显示*/
		//	if (!bIfFillPass)
		//	{
		//		TopViewRectImagePanel->Draw(PDI, View);
		//		TopViewStartPass->Draw(PDI, View);
		//		TopViewEndPass->Draw(PDI, View);
		//		TopViewSpacing->Draw(PDI, View);
		//	}			
		//}
		//else
		//{
		//	if (FArmyObject::GetDrawModel(MODE_ADDWALLPOST) && FArmyObject::GetDrawModel(MODE_DELETEWALLPOST) && GetDrawModel(MODE_CONSTRUCTION))
		//	{
		//		//修改深度偏移
		//		PostModifyStartLine->DepthPriority = SDPG_MAX;
		//		PostModifyEndLine->DepthPriority = SDPG_MAX;
		//		PostModifyTopLine->DepthPriority = SDPG_MAX;
		//		PostModifyBotLine->DepthPriority = SDPG_MAX;

		//		PostModifyStartLine->Draw(PDI, View);
		//		PostModifyEndLine->Draw(PDI, View);
		//		PostModifyTopLine->Draw(PDI, View);
		//		PostModifyBotLine->Draw(PDI, View);
		//	}
		//	else
		//	{
		//		if (FArmyObject::GetDrawModel(MODE_DELETEWALLPOST) && (LeftModifyWallRect->Width > 0 || RightModifyWallRect->Width > 0) && GetDrawModel(MODE_CONSTRUCTION))
		//		{
		//			PostModifyStartLine->Draw(PDI, View);
		//			PostModifyEndLine->Draw(PDI, View);
		//			PostModifyTopLine->Draw(PDI, View);
		//			PostModifyBotLine->Draw(PDI, View);
		//		}
		//		else
		//		{
		//			RectImagePanel->Draw(PDI, View);

		//			//Spacing->Draw(PDI, View);//这是什么？

		//			StartPass->Draw(PDI, View);
		//			EndPass->Draw(PDI, View);

		//			if (FArmyObject::GetDrawModel(MODE_MODIFYDELETE) && (!GetPropertyFlag(FLAG_LAYOUT)))
		//			{
		//				if (LeftModifyWallRect->Width > 0)
		//				{
		//					LeftModifyWallRect->Draw(PDI, View);
		//					if (ConnectHardwareObj.IsValid() && ConnectHardwareObj->BrokenWallRightLine.IsValid())
		//					{
		//						ConnectHardwareObj->BrokenWallRightLine->SetShouldDraw(true);
		//					}
		//				}
		//				else
		//				{
		//					if (ConnectHardwareObj.IsValid() && ConnectHardwareObj->BrokenWallRightLine.IsValid())
		//					{
		//						ConnectHardwareObj->BrokenWallRightLine->SetShouldDraw(false);
		//					}
		//				}
		//				if (RightModifyWallRect->Width > 0)
		//				{
		//					RightModifyWallRect->Draw(PDI, View);
		//					if (ConnectHardwareObj.IsValid() && ConnectHardwareObj->BrokenWallLeftLine.IsValid())
		//					{
		//						ConnectHardwareObj->BrokenWallLeftLine->SetShouldDraw(true);
		//					}
		//				}
		//				else
		//				{
		//					if (ConnectHardwareObj.IsValid() && ConnectHardwareObj->BrokenWallLeftLine.IsValid())
		//					{
		//						ConnectHardwareObj->BrokenWallLeftLine->SetShouldDraw(false);
		//					}
		//				}
		//			}

		//			FArmyHardware::Draw(PDI, View);

		//			if (bIfFillPass)
		//			{
		//				/**@如果是图纸的拆除模式下用到拆改中的数据，则不绘制填补*/
		//				if (!(GetPropertyFlag(FArmyObject::FLAG_MODIFY) && !FArmyObject::GetDrawModel(MODE_MODIFYADD)))
		//				{
		//					FillPassRect->Draw(PDI, View);
		//				}						
		//			}
		//		}

		//		if (FArmyObject::GetDrawModel(MODE_MODIFYADD) && (!GetPropertyFlag(FLAG_LAYOUT)))
		//		{
		//			if (LeftAddWallRect->Width > 0)
		//				LeftAddWallRect->Draw(PDI, View);
		//			if (RightAddWallRect->Width > 0)
		//				RightAddWallRect->Draw(PDI, View);
		//		}
		//	}
		//}
	}
}

bool FArmyPass::IsSelected(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (bModify)
		{
			bShowDashLine = false;
			bShowEditPoint = false;
		}
		else
		{
			bShowDashLine = true;
			bShowEditPoint = true;
		}

		if (bIfGenerateModel)
		{
			/** @欧石楠 如果对包边门洞直接进行拆补，则额外判断拆补前后的选中部分*/
			if (bPassHasAdd || bPassHasModify)
			{
				return RectImagePanel->IsSelected(Pos, InViewportClient) || StartPass->IsSelected(Pos, InViewportClient) 
					|| EndPass->IsSelected(Pos, InViewportClient) || OriginRect->IsSelected(Pos, InViewportClient);
			}
			else
			{
				return RectImagePanel->IsSelected(Pos, InViewportClient) || StartPass->IsSelected(Pos, InViewportClient) || EndPass->IsSelected(Pos, InViewportClient);
			}			
		}
		else
		{
			/** @欧石楠 如果在门洞上放门变成doorhole类型，则额外判断自动拆改的部分*/
			if (bHasAdd || bHasModify)
			{
				return RectImagePanel->IsSelected(Pos, InViewportClient) || LeftAddWallRect->IsSelected(Pos, InViewportClient) 
					|| LeftModifyWallRect->IsSelected(Pos, InViewportClient) || RightAddWallRect->IsSelected(Pos, InViewportClient) || RightModifyWallRect->IsSelected(Pos, InViewportClient);
			}
			/** @欧石楠 如果对门洞直接进行拆补，则额外判断拆补前后的选中部分*/
			else if (bPassHasAdd || bPassHasModify)
			{
				return RectImagePanel->IsSelected(Pos, InViewportClient) || OriginRect->IsSelected(Pos, InViewportClient);
			}
			else
			{
				return RectImagePanel->IsSelected(Pos, InViewportClient);
			}			
		}				
	}

	return false;
}

bool FArmyPass::Hover(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		return IsSelected(Pos, InViewportClient);
	}
	return false;
}

const FBox FArmyPass::GetBounds()
{
	FVector TempHorizontalDirection = FArmyMath::GetLineDirection(GetStartPos(), GetEndPos());
	FVector VerticalDirection = TempHorizontalDirection.RotateAngleAxis(90, FVector(0, 0, 1));

	TArray<FVector> BoundingBox;
	BoundingBox.Push(GetStartPos() + VerticalDirection * (Width) / 2.0f);
	BoundingBox.Push(GetStartPos() - VerticalDirection* (Width) / 2.0f);
	BoundingBox.Push(GetEndPos() + VerticalDirection * (Width) / 2.0f);
	BoundingBox.Push(GetEndPos() - VerticalDirection* (Width) / 2.0f);
	return FBox(BoundingBox);
}


void FArmyPass::Destroy()
{
	if (ExtrusionActor->IsValidLowLevel())
	{
		ExtrusionActor->Destroy();
		ExtrusionActor = nullptr;
	}
}

void FArmyPass::SetPropertyFlag(PropertyType InType, bool InUse)
{
	FArmyObject::SetPropertyFlag(InType, InUse);
	if (InType == FArmyObject::FLAG_MODIFY && InUse)
	{		
		DirectionPoint->SetState(FArmyPrimitive::OPS_Disable);

		OriginLength = Length;
		OriginHeight = Height;
		OriginPoint = GetPos();
		OriginStartPoint = GetStartPos();
		OriginEndPoint = GetEndPos();
		OriginRect = MakeShareable(new FArmyRect(RectImagePanel.Get()));
		OriginStartPass = MakeShareable(new FArmyPolyline(StartPass.Get()));
		OriginEndPass = MakeShareable(new FArmyPolyline(EndPass.Get()));
	}
}

void FArmyPass::Update()
{
	FArmyHardware::Update();

	//StartPoint->Pos.Z = 2;
	//EndPoint->Pos.Z = 2;

	RectImagePanel->Pos = GetStartPos() + Width / 2 * Direction;
	RectImagePanel->Pos.Z = 1.f;//为了让门挡住墙体线
	RectImagePanel->XDirection = HorizontalDirection;
	RectImagePanel->YDirection = -Direction;
	RectImagePanel->Width = Length;
	RectImagePanel->Height = Width;

	FillPassRect->Pos = GetStartPos() + Width / 2 * Direction;
	FillPassRect->Pos.Z = 1.f;//为了让门挡住墙体线
	FillPassRect->XDirection = HorizontalDirection;
	FillPassRect->YDirection = -Direction;
	FillPassRect->Width = Length;
	FillPassRect->Height = Width;

	FVector Temp_Pos = GetPos();
	Temp_Pos.Z = 2.f;
	FVector V0 = Temp_Pos - HorizontalDirection * (Length / 2.f + Thickness + 3.5f) + Direction * (GetTotalWidth() / 2.f + 1.5f);
	FVector V1 = V0 + HorizontalDirection * (7.f + Thickness);
	FVector V2 = V1 - Direction * (GetTotalWidth() + 3.f);
	FVector V3 = V0 - Direction * (GetTotalWidth() + 3.f);
	FVector V4 = V3 + Direction * (OuterWidth + 1.5f);
	FVector V5 = V4 + HorizontalDirection * 7.f;
	FVector V6 = V5 + Direction * Width;
	FVector V7 = V6 - HorizontalDirection * 7.f;

	TArray<FVector> Vertexes0;
	if (bIfGenerateModel)
	{
		Vertexes0.Add(V0);
		Vertexes0.Add(V1);
		Vertexes0.Add(V2);
		Vertexes0.Add(V3);
		Vertexes0.Add(V4);
	}
	Vertexes0.Add(V5);
	Vertexes0.Add(V6);
	if (bIfGenerateModel)
	{
		Vertexes0.Add(V7);
		Vertexes0.Add(V0);
	}
	StartPass->SetVertices(Vertexes0);

	V0 = Temp_Pos + HorizontalDirection * (Length / 2.f + Thickness + 3.5f) + Direction * (GetTotalWidth() / 2.f + 1.5f);
	V1 = V0 - HorizontalDirection * (7.f + Thickness);
	V2 = V1 - Direction * (GetTotalWidth() + 3.f);
	V3 = V0 - Direction * (GetTotalWidth() + 3.f);
	V4 = V3 + Direction * (OuterWidth + 1.5f);
	V5 = V4 - HorizontalDirection * 7.f;
	V6 = V5 + Direction * Width;
	V7 = V6 + HorizontalDirection * 7.f;

	TArray<FVector> Vertexes1;
	if (bIfGenerateModel)
	{
		Vertexes1.Add(V0);
		Vertexes1.Add(V1);
		Vertexes1.Add(V2);
		Vertexes1.Add(V3);
		Vertexes1.Add(V4);
	}
	Vertexes1.Add(V5);
	Vertexes1.Add(V6);
	if (bIfGenerateModel)
	{
		Vertexes1.Add(V7);
		Vertexes1.Add(V0);
	}
	EndPass->SetVertices(Vertexes1);

	Spacing->Pos = GetPos() - HorizontalDirection * Length / 2.f + Direction * Width / 2.f;
	Spacing->XDirection = HorizontalDirection;
	Spacing->YDirection = -Direction;
	Spacing->Width = Length;
	Spacing->Height = Width;

	TopViewRectImagePanel->Pos = RectImagePanel->Pos;
	TopViewRectImagePanel->Pos.Z = 301.f;
	TopViewRectImagePanel->XDirection = RectImagePanel->XDirection;
	TopViewRectImagePanel->YDirection = RectImagePanel->YDirection;
	TopViewRectImagePanel->Width = RectImagePanel->Width;
	TopViewRectImagePanel->Height = RectImagePanel->Height;

	TArray<FVector> TempVertexes = Vertexes0;
	for (int i = 0; i < TempVertexes.Num(); ++i)
	{
		TempVertexes[i].Z = 302.f;
	}
	TopViewStartPass->SetVertices(TempVertexes);

	TempVertexes = Vertexes1;
	for (int i = 0; i < TempVertexes.Num(); ++i)
	{
		TempVertexes[i].Z = 302.f;
	}
	TopViewEndPass->SetVertices(TempVertexes);

	TopViewSpacing->Pos = Spacing->Pos;
	TopViewSpacing->Pos.Z = 302.f;
	TopViewSpacing->XDirection = Spacing->XDirection;
	TopViewSpacing->YDirection = Spacing->YDirection;
	TopViewSpacing->Width = Spacing->Width;
	TopViewSpacing->Height = Spacing->Height;

	//@ 设置包边的线宽
	if (bIfGenerateModel)
	{
		StartPass->SetLineWidth(WALLLINEWIDTH - 1.0F);
		EndPass->SetLineWidth(WALLLINEWIDTH - 1.0F);
	}
	else
	{
		StartPass->SetLineWidth(WALLLINEWIDTH + 0.1F);
		EndPass->SetLineWidth(WALLLINEWIDTH + 0.1F);
	}

	/**@欧石楠 拆改模式下修改数据后计算门洞拆补 只针对原始门洞移动拆改，不对放门拆改生效*/
	if (GetPropertyFlag(FLAG_MODIFY) && !bIsAutoCalcPassState && GetType() != OT_DoorHole)
	{
		UpdateModifyPass();
	}	

	/**@欧石楠 如果没被填补 才可以更新与门的拆补*/
	if (!bIfFillPass)
	{
		UpdatePostWall();
	}	


		
}

TArray<FVector> FArmyPass::GetClipingBox()
{
	StartPoint->Pos.Z = 0;
	EndPoint->Pos.Z = 0;

	FVector TempHorizontalDirection = FArmyMath::GetLineDirection(GetStartPos(), GetEndPos());
	FVector VerticalDirection = TempHorizontalDirection.RotateAngleAxis(90, FVector(0, 0, 1));
	FVector LeftTop = GetStartPos() + VerticalDirection * (Width + SubTracteThicknessError) / 2.0f;
	FVector LeftBootom = GetStartPos() - VerticalDirection* (Width + SubTracteThicknessError) / 2.0f;
	FVector RightTop = GetEndPos() + VerticalDirection * (Width + SubTracteThicknessError) / 2.0f;
	FVector RightBottom = GetEndPos() - VerticalDirection* (Width + SubTracteThicknessError) / 2.0f;
	FVector Offset = FVector(0, 0, Height / 2);
	TArray<FVector> BoundingBox = { (LeftBootom + Offset),(LeftTop + Offset),(RightTop + Offset),(RightBottom + Offset) };
	return BoundingBox;
}

void FArmyPass::SetThickness(const float InThickness)
{
	if (Thickness != InThickness)
	{
		Thickness = InThickness;
		Update();
	}
}

void FArmyPass::SetOuterWidth(const float InOuterWidth)
{
	if (OuterWidth != InOuterWidth)
	{
		OuterWidth = InOuterWidth;
		Update();
	}
}

const void FArmyPass::GetAlonePoints(TArray< TSharedPtr<FArmyPoint> >& OutPoints)
{
	FVector	V0 = GetPos() - HorizontalDirection * (Length / 2.f + Thickness) + Direction * Width / 2.f;
	FVector	V1 = GetPos() + HorizontalDirection * (Length / 2.f + Thickness) + Direction * Width / 2.f;
	FVector	V2 = GetPos() - HorizontalDirection * (Length / 2.f + Thickness) - Direction * Width / 2.f;
	FVector	V3 = GetPos() + HorizontalDirection * (Length / 2.f + Thickness) - Direction * Width / 2.f;
	OutPoints.Push(MakeShareable(new FArmyPoint(V0)));
	OutPoints.Push(MakeShareable(new FArmyPoint(V1)));
	OutPoints.Push(MakeShareable(new FArmyPoint(V2)));
	OutPoints.Push(MakeShareable(new FArmyPoint(V3)));
}

void FArmyPass::SetIfGeneratePassModel(bool bGenerate)
{
	bIfGenerateModel = bGenerate;
	Update();
	if (!bDeserialization)
	{
		FArmySceneData::Get()->ModifyMode_ModifyMultiDelegate.Broadcast();
	}	
}

void FArmyPass::SetIfFillPass(bool bFillPass)
{
	bIfFillPass = bFillPass;
	if (bFillPass)
	{		
		SetIfGeneratePassModel(false);
		RestModifyPass();

		HardWareRulerLine->SetEnableInputBox(false);
		FirstLineLeftRulerLine->SetEnableInputBox(false);
		FirstLineRightRulerLine->SetEnableInputBox(false);
		SecondLineLeftRulerLine->SetEnableInputBox(false);
		SecondLineRightRulerLine->SetEnableInputBox(false);

		DirectionPoint->SetState(FArmyPrimitive::OPS_Disable);
		StartPoint->SetState(FArmyPrimitive::OPS_Disable);
		EndPoint->SetState(FArmyPrimitive::OPS_Disable);
		Point->SetState(FArmyPrimitive::OPS_Disable);
	}
	else
	{
		HardWareRulerLine->SetEnableInputBox(true);
		FirstLineLeftRulerLine->SetEnableInputBox(true);
		FirstLineRightRulerLine->SetEnableInputBox(true);
		SecondLineLeftRulerLine->SetEnableInputBox(true);
		SecondLineRightRulerLine->SetEnableInputBox(true);

		DirectionPoint->SetState(FArmyPrimitive::OPS_Disable);
		StartPoint->SetState(FArmyPrimitive::OPS_Normal);
		EndPoint->SetState(FArmyPrimitive::OPS_Normal);
		Point->SetState(FArmyPrimitive::OPS_Normal);
	}	
}

void FArmyPass::UpdateWallHole(TSharedPtr<FArmyObject> InHardwareObj, bool bIntersect)
{
	TSharedPtr<FArmyHardware> HardwareObj = StaticCastSharedPtr<FArmyHardware>(InHardwareObj);

	if (bIntersect)
	{
		ResetAddModifyWall();
		RestModifyPass();
		ConnectHardwareObj = HardwareObj;
	}
	else
	{
		if (ConnectHardwareObj == HardwareObj)//被移开
		{
			ResetAddModifyWall();
			RestModifyPass();
			bModifyType = true;
		}

		return;
	}

	bModify = true;
	if (ObjectType == OT_Pass)
	{
		ObjectType = OT_DoorHole;
		bModifyType = true;
	}

	//为了让门洞和门朝向一致
	if (!Direction.Equals(HardwareObj->GetDirection()))
	{
		Direction *= -1;
		FArmyHardware::Update();
	}

	if (RectImagePanel->IsSelected(HardwareObj->GetStartPos(), nullptr)
		&& !RectImagePanel->IsSelected(HardwareObj->GetEndPos(), nullptr))
	{
		FVector AddWallStart;
		FVector AddWallEnd;
		FVector ModifyWallStart;
		FVector ModifyWallEnd;

		FVector HardwareStart = HardwareObj->GetStartPos();
		FVector HardwareEnd = HardwareObj->GetEndPos();

		FVector LocalStart = GetStartPos();
		FVector LocalEnd = GetEndPos();

		HardwareStart.Z = 0;
		HardwareEnd.Z = 0;

		LocalStart.Z = 0;
		LocalEnd.Z = 0;

		if (HardwareObj->GetIsRightOpen())
		{
			AddWallEnd = HardwareStart;
			ModifyWallStart = HardwareEnd;
			if (HardwareObj->GetType() == OT_Door)
			{
				TSharedPtr<FArmySingleDoor> Door = StaticCastSharedPtr<FArmySingleDoor>(HardwareObj);
				if (Door.IsValid() && Door->IsPointInObj(LocalStart)/*Door->RectImagePanel->IsSelected(LocalStart, nullptr)*/)
				{
					AddWallStart = LocalEnd;
					ModifyWallEnd = LocalStart;
				}
				else if (Door.IsValid() && Door->IsPointInObj(LocalEnd)/*Door->RectImagePanel->IsSelected(LocalEnd, nullptr)*/)
				{
					AddWallStart = LocalStart;
					ModifyWallEnd = LocalEnd;
				}
			}
			if (HardwareObj->GetType() == OT_SlidingDoor)
			{
				TSharedPtr<FArmySlidingDoor> Door = StaticCastSharedPtr<FArmySlidingDoor>(HardwareObj);
				if (Door.IsValid() && Door->IsPointInObj(LocalStart)/*Door->RectImagePanel->IsSelected(LocalStart, nullptr)*/)
				{
					AddWallStart = LocalEnd;
					ModifyWallEnd = LocalStart;
				}
				else if (Door.IsValid() && Door->IsPointInObj(LocalEnd)/*Door->RectImagePanel->IsSelected(LocalEnd, nullptr)*/)
				{
					AddWallStart = LocalStart;
					ModifyWallEnd = LocalEnd;
				}
			}
		}
		else
		{
			AddWallEnd = LocalEnd;
			ModifyWallStart = LocalStart;
			if (HardwareObj->GetType() == OT_Door)
			{
				TSharedPtr<FArmySingleDoor> Door = StaticCastSharedPtr<FArmySingleDoor>(HardwareObj);
				if (Door.IsValid() && Door->IsPointInObj(LocalStart)/*Door->RectImagePanel->IsSelected(LocalStart, nullptr)*/)
				{
					AddWallStart = HardwareStart;
					ModifyWallEnd = HardwareEnd;
				}
				else if (Door.IsValid() && Door->IsPointInObj(LocalEnd)/*Door->RectImagePanel->IsSelected(LocalEnd, nullptr)*/)
				{
					AddWallStart = LocalEnd;
					ModifyWallEnd = LocalStart;
				}
			}
			if (HardwareObj->GetType() == OT_SlidingDoor)
			{
				TSharedPtr<FArmySlidingDoor> Door = StaticCastSharedPtr<FArmySlidingDoor>(HardwareObj);
				if (Door.IsValid() && Door->IsPointInObj(LocalStart)/*Door->RectImagePanel->IsSelected(LocalStart, nullptr)*/)
				{
					AddWallStart = HardwareEnd;
					ModifyWallEnd = LocalStart;
				}
				else if (Door.IsValid() && Door->IsPointInObj(LocalEnd)/*Door->RectImagePanel->IsSelected(LocalEnd, nullptr)*/)
				{
					AddWallStart = LocalStart;
					ModifyWallEnd = LocalEnd;
				}
			}
		}

		LeftAddWallRect->Pos = AddWallStart + Width / 2 * Direction;
		LeftAddWallRect->Pos.Z = 2.f;
		LeftAddWallRect->XDirection = HorizontalDirection;
		LeftAddWallRect->YDirection = -Direction;
		LeftAddWallRect->Width = (AddWallStart - AddWallEnd).Size();
		LeftAddWallRect->Height = Width;

		LeftModifyWallRect->Pos = ModifyWallEnd + Width / 2 * Direction;
		LeftModifyWallRect->Pos.Z = 2.f;
		LeftModifyWallRect->XDirection = HorizontalDirection;
		LeftModifyWallRect->YDirection = -Direction;
		LeftModifyWallRect->Width = (ModifyWallStart - ModifyWallEnd).Size();
		LeftModifyWallRect->Height = Width;
	}
	else if (!RectImagePanel->IsSelected(HardwareObj->GetStartPos(), nullptr)
		&& RectImagePanel->IsSelected(HardwareObj->GetEndPos(), nullptr))
	{
		FVector AddWallStart;
		FVector AddWallEnd;
		FVector ModifyWallStart;
		FVector ModifyWallEnd;

		if (HardwareObj->GetIsRightOpen())
		{
			AddWallEnd = GetEndPos();
			ModifyWallStart = HardwareObj->GetStartPos();
			if (HardwareObj->GetType() == OT_Door)
			{
				TSharedPtr<FArmySingleDoor> Door = StaticCastSharedPtr<FArmySingleDoor>(HardwareObj);
				if (Door.IsValid() && Door->IsPointInObj(GetStartPos()) /*Door->RectImagePanel->IsSelected(GetStartPos(), nullptr)*/)
				{
					AddWallStart = HardwareObj->GetEndPos();
					ModifyWallEnd = GetStartPos();
				}
				else if (Door.IsValid() && Door->IsPointInObj(GetEndPos())/*Door->RectImagePanel->IsSelected(GetEndPos(), nullptr)*/)
				{
					AddWallStart = GetStartPos();
					ModifyWallEnd = GetEndPos();
				}
			}
			if (HardwareObj->GetType() == OT_SlidingDoor)
			{
				TSharedPtr<FArmySlidingDoor> Door = StaticCastSharedPtr<FArmySlidingDoor>(HardwareObj);
				if (Door.IsValid() && Door->IsPointInObj(GetStartPos())/*Door->RectImagePanel->IsSelected(GetStartPos(), nullptr)*/)
				{
					AddWallStart = HardwareObj->GetEndPos();
					ModifyWallEnd = GetStartPos();
				}
				else if (Door.IsValid() && Door->IsPointInObj(GetEndPos())/*Door->RectImagePanel->IsSelected(GetEndPos(), nullptr)*/)
				{
					AddWallStart = GetStartPos();
					ModifyWallEnd = GetEndPos();
				}
			}
		}
		else
		{
			FVector HardwareStart = HardwareObj->GetStartPos();
			FVector HardwareEnd = HardwareObj->GetEndPos();

			FVector LocalStart = GetStartPos();
			FVector LocalEnd = GetEndPos();

			HardwareStart.Z = 0;
			HardwareEnd.Z = 0;

			LocalStart.Z = 0;
			LocalEnd.Z = 0;

			AddWallEnd = HardwareEnd;
			ModifyWallStart = LocalEnd;

			if (HardwareObj->GetType() == OT_Door)
			{
				TSharedPtr<FArmySingleDoor> Door = StaticCastSharedPtr<FArmySingleDoor>(HardwareObj);
				if (Door.IsValid() && Door->IsPointInObj(LocalStart)/*Door->RectImagePanel->IsSelected(LocalStart, nullptr)*/)
				{
					AddWallStart = HardwareEnd;
					ModifyWallEnd = LocalStart;
				}
				else if (Door.IsValid() && Door->IsPointInObj(LocalEnd)/*Door->RectImagePanel->IsSelected(LocalEnd, nullptr)*/)
				{
					AddWallStart = LocalStart;
					ModifyWallEnd = HardwareStart;
				}
			}
			if (HardwareObj->GetType() == OT_SlidingDoor)
			{
				TSharedPtr<FArmySlidingDoor> Door = StaticCastSharedPtr<FArmySlidingDoor>(HardwareObj);
				if (Door.IsValid() && Door->IsPointInObj(LocalStart)/*Door->RectImagePanel->IsSelected(LocalStart, nullptr)*/)
				{
					AddWallStart = HardwareEnd;
					ModifyWallEnd = LocalStart;
				}
				else if (Door.IsValid() && Door->IsPointInObj(LocalEnd)/*Door->RectImagePanel->IsSelected(LocalEnd, nullptr)*/)
				{
					AddWallStart = LocalStart;
					ModifyWallEnd = LocalEnd;
				}
			}
		}

		RightAddWallRect->Pos = AddWallStart + Width / 2 * Direction;
		RightAddWallRect->Pos.Z = 2.f;
		RightAddWallRect->XDirection = HorizontalDirection;
		RightAddWallRect->YDirection = -Direction;
		RightAddWallRect->Width = (AddWallStart - AddWallEnd).Size();
		RightAddWallRect->Height = Width;

		RightModifyWallRect->Pos = ModifyWallStart + Width / 2 * Direction;
		RightModifyWallRect->Pos.Z = 2.f;
		RightModifyWallRect->XDirection = HorizontalDirection;
		RightModifyWallRect->YDirection = -Direction;
		RightModifyWallRect->Width = (ModifyWallStart - ModifyWallEnd).Size();
		RightModifyWallRect->Height = Width;
	}
	//子集被全包
	else if (RectImagePanel->IsSelected(HardwareObj->GetStartPos(), nullptr)
		&& RectImagePanel->IsSelected(HardwareObj->GetEndPos(), nullptr))
	{
		FVector HardwareStart = HardwareObj->GetStartPos();
		FVector HardwareEnd = HardwareObj->GetEndPos();

		FVector LocalStart = GetStartPos();
		FVector LocalEnd = GetEndPos();

		HardwareStart.Z = 0;
		HardwareEnd.Z = 0;

		LocalStart.Z = 0;
		LocalEnd.Z = 0;

		FVector LeftAddWallPos = LocalStart;
		FVector RightAddWallPos = HardwareEnd;

		float LengthLeft = (LocalStart - HardwareStart).Size();
		float LengthRight = (HardwareEnd - LocalEnd).Size();

		if ((LocalStart - HardwareStart).Size() >= (LocalStart - HardwareEnd).Size())
		{
			LeftAddWallPos = LocalStart;
			RightAddWallPos = HardwareStart;
			LengthLeft = (LocalStart - HardwareEnd).Size();
			LengthRight = (HardwareStart - LocalEnd).Size();
		}

		LeftAddWallRect->Pos = LeftAddWallPos + Width / 2 * Direction;
		LeftAddWallRect->Pos.Z = 2.f;
		LeftAddWallRect->XDirection = HorizontalDirection;
		LeftAddWallRect->YDirection = -Direction;
		LeftAddWallRect->Width = LengthLeft;
		LeftAddWallRect->Height = Width;

		RightAddWallRect->Pos = RightAddWallPos + Width / 2 * Direction;
		RightAddWallRect->Pos.Z = 2.f;
		RightAddWallRect->XDirection = HorizontalDirection;
		RightAddWallRect->YDirection = -Direction;
		RightAddWallRect->Width = LengthRight;
		RightAddWallRect->Height = Width;
	}
	else//父全包其他
	{
		FVector HardwareStart = HardwareObj->GetStartPos();
		FVector HardwareEnd = HardwareObj->GetEndPos();

		FVector LocalStart = GetStartPos();
		FVector LocalEnd = GetEndPos();

		HardwareStart.Z = 0;
		HardwareEnd.Z = 0;

		LocalStart.Z = 0;
		LocalEnd.Z = 0;

		FVector LeftAddWallPos = HardwareStart;
		FVector RightAddWallPos = LocalEnd;

		float LengthLeft = (LeftAddWallPos - LocalStart).Size();
		float LengthRight = (LocalEnd - HardwareEnd).Size();

		if ((LeftAddWallPos - LocalStart).Size() >= (LeftAddWallPos - LocalEnd).Size())
		{
			LeftAddWallPos = HardwareEnd;
			RightAddWallPos = LocalEnd;
			LengthLeft = (HardwareEnd - LocalStart).Size();
			LengthRight = (LocalEnd - HardwareStart).Size();
		}

		LeftModifyWallRect->Pos = LeftAddWallPos + Width / 2 * Direction;
		LeftModifyWallRect->Pos.Z = 2.f;
		LeftModifyWallRect->XDirection = HorizontalDirection;
		LeftModifyWallRect->YDirection = -Direction;
		LeftModifyWallRect->Width = LengthLeft;
		LeftModifyWallRect->Height = Width;

		RightModifyWallRect->Pos = RightAddWallPos + Width / 2 * Direction;
		RightModifyWallRect->Pos.Z = 2.f;
		RightModifyWallRect->XDirection = HorizontalDirection;
		RightModifyWallRect->YDirection = -Direction;
		RightModifyWallRect->Width = LengthRight;
		RightModifyWallRect->Height = Width;
	}

	if (LeftAddWallRect->Width > 0.1 || RightAddWallRect->Width > 0.1)
		bHasAdd = true;
	else
		bHasAdd = false;

	if (LeftModifyWallRect->Width > 0.1 || RightModifyWallRect->Width > 0.1)
		bHasModify = true;
	else
		bHasModify = false;

	///////////////////////////////////huaen by2018-10-10////////////////////////////////////////////////////////////
	UpdatePostWall();

	if (bHasAdd)
	{
		ReqAddMaterialSaleID();
	}
	if (bHasModify)
	{
		ReqModifyMaterialSaleID();
	}
}
void FArmyPass::UpdatePostWall()
{
	{
		LastModifyStart = GetStartPos();
		LastModifyEnd = GetEndPos();
		LastModifyStart.Z = 0;
		LastModifyEnd.Z = 0;

		FVector BaseCenter = RectImagePanel->Pos + RectImagePanel->XDirection * (RectImagePanel->Width / 2) + RectImagePanel->YDirection * (RectImagePanel->Height / 2);
		FVector BaseDir = (LastModifyEnd - LastModifyStart).GetSafeNormal();
		BaseCenter.Z = 0;

		if (LeftAddWallRect->Width > 0)
		{
			FVector AddCenter = LeftAddWallRect->Pos + LeftAddWallRect->XDirection * (LeftAddWallRect->Width / 2) + LeftAddWallRect->YDirection * (LeftAddWallRect->Height / 2);
			AddCenter.Z = 0;
			FVector AddDir = (BaseCenter - AddCenter).GetSafeNormal();
			AddDir.Z = 0;
			BaseDir.Z = 0;
			if ((BaseDir - AddDir).Size() < 0.001)
			{
				LastModifyStart += (AddDir.GetSafeNormal() * LeftAddWallRect->Width);
			}
			else
			{
				LastModifyEnd += (AddDir.GetSafeNormal() * LeftAddWallRect->Width);
			}
		}
		if (RightAddWallRect->Width > 0)
		{
			FVector AddCenter = RightAddWallRect->Pos + RightAddWallRect->XDirection * (RightAddWallRect->Width / 2) + RightAddWallRect->YDirection * (RightAddWallRect->Height / 2);
			AddCenter.Z = 0;
			FVector AddDir = (BaseCenter - AddCenter).GetSafeNormal();

			AddDir.Z = 0;
			BaseDir.Z = 0;
			if ((BaseDir - AddDir).Size() < 0.001)
			{
				LastModifyStart += (AddDir.GetSafeNormal() * RightAddWallRect->Width);
			}
			else
			{
				LastModifyEnd += (AddDir.GetSafeNormal() * RightAddWallRect->Width);
			}
		}

		if (RightModifyWallRect->Width > 0)
		{
			FVector DeleteCenter = RightModifyWallRect->Pos + RightModifyWallRect->XDirection * (RightModifyWallRect->Width / 2) + RightModifyWallRect->YDirection * (RightModifyWallRect->Height / 2);
			DeleteCenter.Z = 0;
			FVector DeleteDir = (DeleteCenter - BaseCenter).GetSafeNormal();

			DeleteDir.Z = 0;
			BaseDir.Z = 0;
			if ((BaseDir - DeleteDir).Size() < 0.001)
			{
				LastModifyEnd += (DeleteDir.GetSafeNormal() * RightModifyWallRect->Width);
			}
			else
			{
				LastModifyStart += (DeleteDir.GetSafeNormal() * RightModifyWallRect->Width);
			}
		}


		if (LeftModifyWallRect->Width > 0)
		{
			FVector DeleteCenter = LeftModifyWallRect->Pos + LeftModifyWallRect->XDirection * (LeftModifyWallRect->Width / 2) + LeftModifyWallRect->YDirection * (LeftModifyWallRect->Height / 2);
			DeleteCenter.Z = 0;
			FVector DeleteDir = (DeleteCenter - BaseCenter).GetSafeNormal();

			DeleteDir.Z = 0;
			BaseDir.Z = 0;
			if ((BaseDir - DeleteDir).Size() < 0.001)
			{
				LastModifyEnd += (DeleteDir.GetSafeNormal() * LeftModifyWallRect->Width);
			}
			else
			{
				LastModifyStart += (DeleteDir.GetSafeNormal() * LeftModifyWallRect->Width);
			}
		}

		PostModifyStartLine->SetStart(LastModifyStart + RectImagePanel->YDirection * RectImagePanel->Height / 2);
		PostModifyStartLine->SetEnd(LastModifyStart - RectImagePanel->YDirection * RectImagePanel->Height / 2);
		PostModifyEndLine->SetStart(LastModifyEnd + RectImagePanel->YDirection * RectImagePanel->Height / 2);
		PostModifyEndLine->SetEnd(LastModifyEnd - RectImagePanel->YDirection * RectImagePanel->Height / 2);
		PostModifyTopLine->SetStart(PostModifyStartLine->GetStart());
		PostModifyTopLine->SetEnd(PostModifyEndLine->GetStart());
		PostModifyBotLine->SetStart(PostModifyStartLine->GetEnd());
		PostModifyBotLine->SetEnd(PostModifyEndLine->GetEnd());
	}
}
void FArmyPass::ResetAddModifyWall()
{
	LeftAddWallRect->Width = 0;
	LeftModifyWallRect->Width = 0;
	RightModifyWallRect->Width = 0;
	RightAddWallRect->Width = 0;
	ConnectHardwareObj = nullptr;
	bModify = false;
	ObjectType = OT_Pass;
}

void FArmyPass::RestVerticalState()
{
	bPassHasVerticalAdd = false;
	bPassHasVerticalModify = false;
	SetHeight(OriginHeight);
}

void FArmyPass::SetAddDoorHoleMatType(FText InStr)
{
    AddDoorHoleMatType = InStr;
	/**@欧石楠 因为后台使用前台写死数据 暂时硬处理*/
	if (AddDoorHoleMatType.EqualTo(FText::FromString(TEXT("红砖"))))
	{
		SelectedAddMaterialID = 1;
	}
	else if (AddDoorHoleMatType.EqualTo(FText::FromString(TEXT("轻体砖"))))
	{
		SelectedAddMaterialID = 2;
	}
	else if (AddDoorHoleMatType.EqualTo(FText::FromString(TEXT("轻体砖保温填充材质"))))
	{
		SelectedAddMaterialID = 3;
	}
	else if (AddDoorHoleMatType.EqualTo(FText::FromString(TEXT("保温板+石膏板保温"))))
	{
		SelectedAddMaterialID = 4;
	}
	else if (AddDoorHoleMatType.EqualTo(FText::FromString(TEXT("轻钢龙骨隔墙"))))
	{
		SelectedAddMaterialID = 5;
	}
	else if (AddDoorHoleMatType.EqualTo(FText::FromString(TEXT("大芯板"))))
	{
		SelectedAddMaterialID = 6;
	}

	UMaterial* LineMaterial = FArmyEngineModule::Get().GetEngineResource()->GetAddWallMaterial();
	MI_AddWall = UMaterialInstanceDynamic::Create(LineMaterial, nullptr);
	MI_AddWall->AddToRoot();
	MI_AddWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor::Green);

	if (LeftAddWallRect->Width > 0)
	{
		LeftAddWallRect->MaterialRenderProxy = MI_AddWall->GetRenderProxy(false);
	}
	if (RightAddWallRect->Width > 0)
	{
		RightAddWallRect->MaterialRenderProxy = MI_AddWall->GetRenderProxy(false);
	}

	if (InStr.EqualTo(FText::FromString(TEXT("轻体砖"))))
	{
		LeftAddWallRect->bIsFilled = true;
		RightAddWallRect->bIsFilled = true;
	}
	else if (InStr.EqualTo(FText::FromString(TEXT("大芯板"))))
	{
		LeftAddWallRect->bIsFilled = false;
		RightAddWallRect->bIsFilled = false;
	}

	ReqAddMaterialSaleID();
}

void FArmyPass::SetModifyDoorHoleMatType(FText InStr)
{
	ModifyDoorHoleMatType = InStr;

	/**@欧石楠 因为后台使用前台写死数据 暂时硬处理*/
	if (ModifyDoorHoleMatType.EqualTo(FText::FromString(TEXT("红砖"))))
	{
		SelectedModifyMaterialID = 1;
	}
	else if (ModifyDoorHoleMatType.EqualTo(FText::FromString(TEXT("轻体砖"))))
	{
		SelectedModifyMaterialID = 2;
	}
	else if (ModifyDoorHoleMatType.EqualTo(FText::FromString(TEXT("轻体砖保温填充材质"))))
	{
		SelectedModifyMaterialID = 3;
	}
	else if (ModifyDoorHoleMatType.EqualTo(FText::FromString(TEXT("保温板+石膏板保温"))))
	{
		SelectedModifyMaterialID = 4;
	}
	else if (ModifyDoorHoleMatType.EqualTo(FText::FromString(TEXT("轻钢龙骨隔墙"))))
	{
		SelectedModifyMaterialID = 5;
	}
	else if (ModifyDoorHoleMatType.EqualTo(FText::FromString(TEXT("大芯板"))))
	{
		SelectedModifyMaterialID = 6;
	}

	ReqModifyMaterialSaleID();
}

void FArmyPass::ReqAddMaterialSaleID()
{
	FString Url = FString::Printf(TEXT("/api/brick/%d"), SelectedAddMaterialID);
    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &FArmyPass::ResAddMaterialSaleID);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest(Url, CompleteDelegate);
    Request->ProcessRequest();
}

void FArmyPass::ResAddMaterialSaleID(FArmyHttpResponse Response)
{
    if (Response.bWasSuccessful)
    {
        TSharedPtr<FJsonObject> JObject = Response.Data->GetObjectField("data");
        int32 Id = JObject->GetIntegerField("id");
        FString Name = JObject->GetStringField("name");

        AddMaterialSaleID = Id;
    }
}

void FArmyPass::ReqModifyMaterialSaleID()
{
    FString Url = FString::Printf(TEXT("/api/brick/%d"), SelectedModifyMaterialID);
    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &FArmyPass::ResModifyMaterialSaleID);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest(Url, CompleteDelegate);
    Request->ProcessRequest();
}

void FArmyPass::ResModifyMaterialSaleID(FArmyHttpResponse Response)
{
    if (Response.bWasSuccessful)
    {
        TSharedPtr<FJsonObject> JObject = Response.Data->GetObjectField("data");
        int32 Id = JObject->GetIntegerField("id");
        FString Name = JObject->GetStringField("name");

        ModifyMaterialSaleID = Id;
    }
}

void FArmyPass::GetAddWallAreaAndPerimeter(float& LeftOutArea, float& LeftOutPerimeter, float& RightOutArea, float& RightOutPerimeter)
{
	LeftOutArea = 0;
	LeftOutPerimeter = 0;
	RightOutArea = 0;
	RightOutPerimeter = 0;
	if (!FMath::IsNearlyZero(LeftAddWallRect->Width))
	{
		LeftOutPerimeter = (LeftAddWallRect->Width + LeftAddWallRect->Height) * 2.f / 100.f;
		LeftOutArea = LeftAddWallRect->Width * Height * 0.0001f;
	}
	if (!FMath::IsNearlyZero(RightAddWallRect->Width))
	{
		RightOutPerimeter += (RightAddWallRect->Width + RightAddWallRect->Height) * 2.f / 100.f;
		RightOutArea = RightAddWallRect->Width * Height * 0.0001f;
	}
}

void FArmyPass::GetAddWallAreaAndPerimeter(float & OutArea, float & OutPerimeter)
{
	OutArea = 0;
	OutPerimeter = 0;
	float TempHeight = bPassHasVerticalAdd ? OriginHeight : Height;
	if (!FMath::IsNearlyZero(LeftAddWallRect->Width))
	{
		OutPerimeter += (LeftAddWallRect->Width + LeftAddWallRect->Height) * 2.f / 100.f;
		OutArea += LeftAddWallRect->Width * TempHeight * 0.0001f;
	}
	if (!FMath::IsNearlyZero(RightAddWallRect->Width))
	{
		OutPerimeter += (RightAddWallRect->Width + RightAddWallRect->Height) * 2.f / 100.f;
		OutArea += RightAddWallRect->Width * TempHeight * 0.0001f;
	}	
}

void FArmyPass::GetModifyWallAreaAndPerimeter(float & OutArea, float & OutPerimeter)
{
	OutArea = 0;
	OutPerimeter = 0;
	if (!FMath::IsNearlyZero(LeftModifyWallRect->Width))
	{
		OutPerimeter += (LeftModifyWallRect->Width + LeftModifyWallRect->Height) * 2.f / 100.f;		
		OutArea += LeftModifyWallRect->Width * OriginHeight * 0.0001f;
	}
	if (!FMath::IsNearlyZero(RightModifyWallRect->Width))
	{
		OutPerimeter += (RightModifyWallRect->Width + RightModifyWallRect->Height) * 2.f / 100.f;		
		OutArea += RightModifyWallRect->Width * OriginHeight * 0.0001f;
	}	
}

void FArmyPass::GetOriginPassArea(float & OutArea)
{
	OutArea = 0;
	OutArea = (Height * 2 + Length) * Width * 0.0001f;
}

void FArmyPass::GetFillAreaAndPerimeter(float& OutArea, float& OutPerimeter)
{
    OutArea = Length * Height * 0.0001f;
    OutPerimeter = (Length + Height * 2) * 0.01f;
}

void FArmyPass::GetAttachHoleWallInfo(TArray<struct FBSPWallInfo>& OutAttachWallInfo)
{
	if (!bIfFillPass)
		FArmyHardware::GetAttachHoleWallInfo(OutAttachWallInfo);
}

float FArmyPass::GetModifyLength()
{
	float TotalLength = 0.f;
	if (!FMath::IsNearlyZero(LeftModifyWallRect->Width)) 
	{
		TotalLength += LeftModifyWallRect->Width;
	}
	if (!FMath::IsNearlyZero(RightModifyWallRect->Width))
	{
		TotalLength += RightModifyWallRect->Width;
	}
	return TotalLength;
}

float FArmyPass::GetAddLength()
{
	float TotalLength = 0.f;
	if (!FMath::IsNearlyZero(LeftAddWallRect->Width))
	{
		TotalLength += LeftAddWallRect->Width;
	}
	if (!FMath::IsNearlyZero(RightAddWallRect->Width))
	{
		TotalLength += RightAddWallRect->Width;
	}
	return TotalLength;
}

void FArmyPass::UpdateModifyPass()
{	
	if (bIfFillPass)
	{		
		return;
	}		
	bool TempCachedPassHasAdd = bPassHasAdd;
	bool TempCachedPassHasModify = bPassHasModify;

	bIsAutoCalcPassState = true;

	bPassHasAdd = false;
	bPassHasModify = false;
	bHasModifyPass = false;
	//如果新的起点在原始区域内部且新旧起始点距离大于1则认为进行了修补
	if (OriginRect->IsPointInRect(GetStartPos()) && FVector::Distance(OriginStartPoint, GetStartPos()) >= 1.f)
	{
		LeftAddWallRect->Pos = OriginStartPoint + Width / 2 * Direction;
		LeftAddWallRect->Pos.Z = 2.f;
		LeftAddWallRect->XDirection = HorizontalDirection;
		LeftAddWallRect->YDirection = -Direction;
		LeftAddWallRect->Width = (GetStartPos() - OriginStartPoint).Size();
		LeftAddWallRect->Height = Width;		

		LeftModifyWallRect->Width = 0;
	}
	//如果新的起点在原始区域外部且新旧起始点距离大于1则认为进行了拆改
	else if (!OriginRect->IsPointInRect(GetStartPos()) && FVector::Distance(OriginStartPoint, GetStartPos()) >= 1.f)
	{
		LeftModifyWallRect->Pos = OriginStartPoint + Width / 2 * Direction;
		LeftModifyWallRect->Pos.Z = 2.f;
		LeftModifyWallRect->XDirection = -HorizontalDirection;
		LeftModifyWallRect->YDirection = -Direction;
		LeftModifyWallRect->Width = (OriginStartPoint - GetStartPos()).Size();
		LeftModifyWallRect->Height = Width;		

		LeftAddWallRect->Width = 0;
	}	

	//如果新的终点在原始区域内部且新旧终点距离大于1则认为进行了修补
	if (OriginRect->IsPointInRect(GetEndPos()) && FVector::Distance(OriginEndPoint, GetEndPos()) >= 1.f)
	{
		RightAddWallRect->Pos = OriginEndPoint + Width / 2 * Direction;
		RightAddWallRect->Pos.Z = 2.f;
		RightAddWallRect->XDirection = -HorizontalDirection;
		RightAddWallRect->YDirection = -Direction;
		RightAddWallRect->Width = (GetEndPos() - OriginEndPoint).Size();
		RightAddWallRect->Height = Width;		

		RightModifyWallRect->Width = 0;
	}
	//如果新的终点在原始区域外部且新旧终点距离大于1则认为进行了拆改
	else if (!OriginRect->IsPointInRect(GetEndPos()) && FVector::Distance(OriginEndPoint, GetEndPos()) >= 1.f)
	{
		RightModifyWallRect->Pos = OriginEndPoint + Width / 2 * Direction;
		RightModifyWallRect->Pos.Z = 2.f;
		RightModifyWallRect->XDirection = HorizontalDirection;
		RightModifyWallRect->YDirection = -Direction;
		RightModifyWallRect->Width = (GetEndPos() - OriginEndPoint).Size();
		RightModifyWallRect->Height = Width;		

		RightAddWallRect->Width = 0;
	}	

	//在最后判断是否进行了修改，增加标记
	if (LeftModifyWallRect->Width > 0 || RightModifyWallRect->Width > 0)
	{
		bPassHasModify = true;
		bHasModifyPass = true;
	}
	if (LeftAddWallRect->Width > 0 || RightAddWallRect->Width > 0)
	{
		bPassHasAdd = true;
		bHasModifyPass = true;
	}

	//如果存在拆改且距离小于2mm，则会为其自动对齐，取消拆改和修补
	if ((bPassHasAdd || bPassHasModify) && !bDeserialization)
	{
		if (FVector::Distance(OriginStartPoint, GetStartPos()) < 2.f)
		{
			Point->SetPos(OriginPoint + (OriginLength - Length) / 2.f * (OriginStartPoint - OriginEndPoint).GetSafeNormal());

			Update();

			LeftModifyWallRect->Width = 0;
			LeftAddWallRect->Width = 0;
		}		
		
		if (FVector::Distance(OriginEndPoint, GetEndPos()) < 2.f)
		{
			//如果一侧已经无拆改信息，另一侧有拆改信息并且距离小于2mm，则直接恢复原状态
			if (LeftModifyWallRect->Width == 0 && LeftAddWallRect->Width == 0)
			{
				SetLength(OriginLength);
				SetPos(OriginPoint);
			}			
			else
			{
				Point->SetPos(OriginPoint + (OriginLength - Length) / 2.f * (OriginEndPoint - OriginStartPoint).GetSafeNormal());
			}

			Update();

			RightModifyWallRect->Width = 0;
			RightAddWallRect->Width = 0;
		}
	}

	//在最后判断是否进行了修改，增加标记
	if (LeftModifyWallRect->Width > 0 || RightModifyWallRect->Width > 0)
	{
		bPassHasModify = true;
		bHasModifyPass = true;
	}
	if (LeftAddWallRect->Width > 0 || RightAddWallRect->Width > 0)
	{
		bPassHasAdd = true;
		bHasModifyPass = true;
	}

	//发生变化时需要重新使用默认数据
	if (!bDeserialization && (TempCachedPassHasAdd != bPassHasAdd || TempCachedPassHasModify != bPassHasModify))
	{
		ConstructionItemData->bHasSetted = false;
	}

	bIsAutoCalcPassState = false;
}

void FArmyPass::RestModifyPass()
{
	LeftModifyWallRect->Width = 0;
	LeftAddWallRect->Width = 0;
	RightModifyWallRect->Width = 0;
	RightAddWallRect->Width = 0;

	bPassHasAdd = false;
	bPassHasModify = false;
	bHasModifyPass = false;
	/** @欧石楠 只有在封门洞的时候才重置高度*/
	if (bIfFillPass)
	{
		bPassHasVerticalAdd = false;
		bPassHasVerticalModify = false;
		SetHeight(OriginHeight);
	}

	SetLength(OriginLength);
	SetPos(OriginPoint);

	OriginStartPoint = GetStartPos();
	OriginEndPoint = GetEndPos();
	OriginRect = MakeShareable(new FArmyRect(RectImagePanel.Get()));
	OriginStartPass = MakeShareable(new FArmyPolyline(StartPass.Get()));
	OriginEndPass = MakeShareable(new FArmyPolyline(EndPass.Get()));
}

bool FArmyPass::CheckIfCanMoveModifyPass(FVector InPos)
{
	if (OriginRect->IsPointInRect(GetStartPos()) && OriginRect->IsPointInRect(GetEndPos()))
	{
		return true;
	}

    FVector TempHorizontalDir = bRightOpen ? Direction.RotateAngleAxis(90.f, FVector(0.f, 0.f, 1.f)) :
        Direction.RotateAngleAxis(-90.f, FVector(0.f, 0.f, 1.f));

    FVector V1 = InPos - TempHorizontalDir * Length / 2.f;
    FVector V2 = InPos + TempHorizontalDir * Length / 2.f;

    float StartPointDist = FMath::Min<float>(FVector::Distance(V1, OriginStartPoint), FVector::Distance(V2, OriginStartPoint));
    float EndPointDist = FMath::Min<float>(FVector::Distance(V1, OriginEndPoint), FVector::Distance(V2, OriginEndPoint));

	if ((StartPointDist >= Length - 0.1f && !OriginRect->IsPointInRect(V1)) ||
        (EndPointDist >= Length - 0.1f && !OriginRect->IsPointInRect(V2)))
	{
		return false;
	}

	return true;
}

void FArmyPass::UpdateVerticalState()
{
	bool TempCachedPassHasVerticalAdd = bPassHasVerticalAdd;
	bool TempCachedPassHasVerticalModify = bPassHasVerticalModify;
	if (GetPropertyFlag(PropertyType::FLAG_MODIFY) && InWallType == 0)
	{
		if (Height > OriginHeight)
		{
			bPassHasVerticalModify = true;
			bPassHasVerticalAdd = false;
		}
		else if (Height < OriginHeight)
		{
			bPassHasVerticalModify = false;
			bPassHasVerticalAdd = true;
		}
		else
		{
			bPassHasVerticalModify = false;
			bPassHasVerticalAdd = false;
		}
	}
	else if (InWallType == 1)
	{
		bPassHasVerticalModify = false;
		bPassHasVerticalAdd = false;		
	}
	//发生变化时需要重新使用默认数据
	if (!bDeserialization && (TempCachedPassHasVerticalAdd != bPassHasVerticalAdd || TempCachedPassHasVerticalModify != bPassHasVerticalModify))
	{
		ConstructionItemData->bHasSetted = false;
	}
}

void FArmyPass::SetMatType(FString InType)
{
	if (InType == "")
	{
		InType = TEXT("轻体砖");
	}
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
	
	UMaterialInstanceDynamic* MI_Line = UMaterialInstanceDynamic::Create(LineMaterial, NULL);
	MI_Line->AddToRoot();
	MI_Line->SetVectorParameterValue(TEXT("MainColor"), FLinearColor::Green);
	FillPassRect->MaterialRenderProxy = MI_Line->GetRenderProxy(false);
}

void FArmyPass::Generate(UWorld* InWorld)
{
	if (bModify || !bGenerate3D)
	{
		return;
	}

	if (bIfGenerateModel)
	{
		FString Path = FPaths::ProjectContentDir() + "/Assets/test.dxf";
		FVector pos0 = GetPos() + HorizontalDirection * Length / 2;
		FVector pos1 = pos0 + FVector(0, 0, 1)*Height;
		FVector pos2 = GetPos() - HorizontalDirection * Length / 2 + FVector(0, 0, 1)*Height;
		FVector pos3 = GetPos() - HorizontalDirection * Length / 2;
		TArray<FVector> RootPathVertices = { pos0,pos1,pos2,pos3 };
		FVector tempPlaneX = HorizontalDirection.GetSafeNormal();
		FVector tempPlaneY = FVector(0, 0, -1);
		FVector tempPlaneCenter = (pos0 + pos1 + pos2 + pos3) / 4.0f;

		for (auto& iter : RootPathVertices)
		{
			float x = FVector::DotProduct((iter - tempPlaneCenter), tempPlaneX);
			float y = FVector::DotProduct((iter - tempPlaneCenter), tempPlaneY);
			iter = FVector(x, y, 0.0);
		}
		if (!FArmyMath::IsClockWise(RootPathVertices))
			FArmyMath::ReversePointList(RootPathVertices);
		if (ExtrusionActor == NULL)
		{
            FActorSpawnParameters Params;
            Params.Name = FName(*("NOLIST-PASS-" + GetUniqueID().ToString()));
			ExtrusionActor = InWorld->SpawnActor<AArmyExtrusionActor>(AArmyExtrusionActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, Params);
			ExtrusionActor->MeshComponent->bBuildStaticLighting = true;
			ExtrusionActor->Tags.Add(XRActorTag::Immovable);
			ExtrusionActor->Tags.Add(XRActorTag::CanNotDelete);
			ExtrusionActor->GeneratePassMeshFromFilePath(Path, RootPathVertices, Width, false, tempPlaneX, tempPlaneY, tempPlaneX^tempPlaneY, tempPlaneCenter);
			ExtrusionActor->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWindowBaseMaterial());
			ExtrusionActor->SetActorLabel(TEXT("垭口_") + GetUniqueID().ToString());
			ExtrusionActor->AttachHardware = StaticCastSharedRef<FArmyHardware>(this->AsShared());
			AttachModelName = ExtrusionActor->GetActorLabel();
		}
		else
		{
			ExtrusionActor->GeneratePassMeshFromFilePath(Path, RootPathVertices, Width, false, tempPlaneX, tempPlaneY, tempPlaneX^tempPlaneY, tempPlaneCenter);
		}
		ExtrusionActor->Tags.Add(XRActorTag::Pass);

		//@郭子阳 请求施工项
		if (HasGoods())
		{
			UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
			auto ContentItem = ResMgr->GetContentItemFromItemID(EResourceType::Texture, SaleID);
			if (ContentItem.IsValid() && ExtrusionActor)
			{
				SaleID = -1;
				ReplaceGoods(ContentItem, nullptr);
			}
			else
			{
				//@郭子阳
				//请求3D模式施工项
				XRConstructionManager::Get()->TryToFindConstructionData(this->GetUniqueID(), *(this->GetConstructionParameter().Get()), nullptr);
			}
		}
	}
}

void FArmyPass::SetPos(const FVector & InPos)
{
	if ((!GetPropertyFlag(FLAG_MODIFY) && !GetPropertyFlag(FLAG_LAYOUT)) || bDeserialization)
	{
		FArmyHardware::SetPos(InPos);
	}
	else
	{
		if (Point->GetPos() != InPos)
		{
			if (CheckIfCanMoveModifyPass(InPos))
			{
				Point->SetPos(InPos);
			}
			else
			{
				GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("门洞拆改不能完全偏离原始位置!"));
			}
			Update();
		}
	}	
}

void FArmyPass::SetHeight(const float & InHeight)
{
	FArmyHardware::SetHeight(InHeight);	
}

void FArmyPass::SetDirection(const FVector & InDirection)
{
	if (!Direction.Equals(InDirection, KINDA_SMALL_NUMBER))
	{
		if (Direction.Equals(-InDirection, KINDA_SMALL_NUMBER))
		{
			FVector Temp = OriginStartPoint;
			OriginStartPoint = OriginEndPoint;
			OriginEndPoint = Temp;
		}
		else if (GetPropertyFlag(FLAG_MODIFY) && !bDeserialization)
		{
			return;
		}
		Direction = InDirection;
		Update();
	}
}

void FArmyPass::ReplaceGoods(FContentItemPtr InContentItem, UWorld* InWorld)
{
	if (InContentItem->ID != SaleID)
	{
		PassContentItem = InContentItem;
		SaleID = InContentItem->ID;
        ThumbnailUrl = InContentItem->ThumbnailURL;
		UMaterialInterface* MI = FArmyResourceModule::Get().GetResourceManager()->CreateContentItemMaterial(InContentItem);
		ExtrusionActor->SetMaterial(MI);
		
		FVector pos0 = GetPos() + HorizontalDirection * Length / 2;
		FVector pos1 = pos0 + FVector(0, 0, 1)*Height;
		FVector pos2 = GetPos() - HorizontalDirection * Length / 2 + FVector(0, 0, 1)*Height;
		FVector pos3 = GetPos() - HorizontalDirection * Length / 2;
		TArray<FVector> RootPathVertices = { pos0,pos1,pos2,pos3 };
		FVector tempPlaneX = HorizontalDirection.GetSafeNormal();
		FVector tempPlaneY = FVector(0, 0, -1);
		FVector tempPlaneCenter = (pos0 + pos1 + pos2 + pos3) / 4.0f;

		for (auto& iter : RootPathVertices)
		{
			float x = FVector::DotProduct((iter - tempPlaneCenter), tempPlaneX);
			float y = FVector::DotProduct((iter - tempPlaneCenter), tempPlaneY);
			iter = FVector(x, y, 0.0);
		}
		if (!FArmyMath::IsClockWise(RootPathVertices))
			FArmyMath::ReversePointList(RootPathVertices);

		TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = InContentItem->GetResObjNoComponent();
		if (resArr.Num() < 1)
			return;
		FString dxfFilePath;
		for (auto& iter : resArr)
		{
			if (iter->ResourceType == HardDXF)
			{
				dxfFilePath = iter->FilePath;
				break;
			}

		}

		ExtrusionActor->GeneratePassMeshFromFilePath(dxfFilePath, RootPathVertices, Width, false, tempPlaneX, tempPlaneY, tempPlaneX^tempPlaneY, tempPlaneCenter);
		ExtrusionActor->SetSynID(SaleID);
	
		// 更新拆改模式垭口的数据
		TWeakPtr<FArmyObject> ModifyPass = FArmySceneData::Get()->GetObjectByGuid(E_ModifyModel, GetUniqueID());
		if (ModifyPass.IsValid())
		{
			TSharedPtr<FArmyPass> ModifyNewPass = StaticCastSharedPtr<FArmyPass>(ModifyPass.Pin());
			if (ModifyNewPass.IsValid())
			{
				ModifyNewPass->SaleID = InContentItem->ID;
				ModifyNewPass->ThumbnailUrl = InContentItem->ThumbnailURL;
			}
		}

		//@郭子阳
		//设置原始户型\拆改后中的的SaleID
		auto HomeObject = FArmySceneData::Get()->GetObjectByGuid(EModelType::E_HomeModel, GetUniqueID());
		auto HomeFArmyPass = StaticCastSharedPtr<FArmyPass>(HomeObject.Pin());
		if (HomeFArmyPass.IsValid())
		{
			HomeFArmyPass->SaleID = SaleID;
		}
		auto LayoutObject = FArmySceneData::Get()->GetObjectByGuid(EModelType::E_LayoutModel, GetUniqueID());
		auto LayoutFArmyPass = StaticCastSharedPtr<FArmyPass>(LayoutObject.Pin());
		if (LayoutFArmyPass.IsValid())
		{
			LayoutFArmyPass->SaleID = SaleID;
		}

		//@郭子阳 请求施工项
		if (HasGoods())
		{

			{

				//@郭子阳
				//请求3D模式施工项
				XRConstructionManager::Get()->TryToFindConstructionData(this->GetUniqueID(), *(this->GetConstructionParameter().Get()), nullptr);
			}
		}
	}
}

void FArmyPass::UpdateStartPoint()
{
	if (FVector::Distance(OriginStartPoint, GetStartPos()) < 2.f)
	{
		if (bRightOpen)
		{
			HorizontalDirection = Direction.RotateAngleAxis(90, FVector(0, 0, 1));
		}
		else
		{
			HorizontalDirection = Direction.RotateAngleAxis(-90, FVector(0, 0, 1));
		}
		StartPoint->SetPos(OriginStartPoint);
		Length = (StartPoint->GetPos() - EndPoint->GetPos()).Size();
		FVector TempPos = EndPoint->GetPos() - HorizontalDirection * Length / 2.f;
		Point->SetPos(TempPos);
		Update();
	}
	else
	{
		FArmyHardware::UpdateStartPoint();
	}
}

void FArmyPass::UpdateEndPoint()
{
	if (FVector::Distance(OriginEndPoint, GetEndPos()) < 2.f)
	{
		if (bRightOpen)
		{
			HorizontalDirection = Direction.RotateAngleAxis(90, FVector(0, 0, 1));
		}
		else
		{
			HorizontalDirection = Direction.RotateAngleAxis(-90, FVector(0, 0, 1));
		}
		EndPoint->SetPos(OriginEndPoint);
		Length = (StartPoint->GetPos() - EndPoint->GetPos()).Size();
		FVector TempPos = StartPoint->GetPos() + HorizontalDirection * Length / 2.f;
		Point->SetPos(TempPos);
		Update();
	}
	else
	{
		FArmyHardware::UpdateEndPoint();
	}
}

TArray<struct FLinesInfo> FArmyPass::GetFacadeBox()
{
	/** 全部的框 */
	TArray<struct FLinesInfo> AllCircle;
	FVector base_z = FVector(0, 0, 1);

	/** 内层框 */
	FLinesInfo InsideCircle;
	InsideCircle.DrawType = 1;
	InsideCircle.Color = FLinearColor(0.0f, 1.0f, 1.0f);
	FVector LeftBottomInside = GetPos() - Length * 0.5f * HorizontalDirection - Width * 0.5f * Direction + HeightToFloor*base_z;
	FVector RightBottomInside = GetPos() + Length * 0.5f * HorizontalDirection - Width * 0.5f * Direction + HeightToFloor*base_z;
	FVector LeftTopInside = LeftBottomInside + Height*base_z;
	FVector RightTopInside = RightBottomInside + Height*base_z;

	InsideCircle.Vertices.Emplace(LeftBottomInside);
	InsideCircle.Vertices.Emplace(LeftTopInside);
	InsideCircle.Vertices.Emplace(RightTopInside);
	InsideCircle.Vertices.Emplace(RightBottomInside);
	AllCircle.Emplace(InsideCircle);

	if (bIfGenerateModel)
	{
		/** 外层框 */
		FLinesInfo OutsideCicle;
		OutsideCicle.DrawType = 1;
		OutsideCicle.Color = FLinearColor(0.0f, 1.0f, 1.0f);
		FVector LeftBottomOutside = LeftBottomInside - 6 * HorizontalDirection;
		FVector RightBottomOutside = RightBottomInside + 6 * HorizontalDirection;
		FVector LeftTopOutside = LeftBottomOutside + (Height + 6)*base_z;
		FVector RightTopOutside = RightBottomOutside + (Height + 6)*base_z;

		OutsideCicle.Vertices.Emplace(LeftBottomOutside);
		OutsideCicle.Vertices.Emplace(LeftTopOutside);
		OutsideCicle.Vertices.Emplace(RightTopOutside);
		OutsideCicle.Vertices.Emplace(RightBottomOutside);

		AllCircle.Emplace(OutsideCicle);
	}
	/** 折线 */
	FLinesInfo BreakCicle;
	BreakCicle.DrawType = 1;
	BreakCicle.Color = FLinearColor(56.f / 255.f, 58.f / 255.f, 64.f / 255.f);
	FVector FirstBreakPoint = RightTopInside;
	FVector SecondBreakPoint = LeftBottomInside + Length * 0.25f * HorizontalDirection + Height * 0.875f * base_z;
	FVector ThirdBreakPoint = LeftBottomInside;

	BreakCicle.Vertices.Emplace(FirstBreakPoint);
	BreakCicle.Vertices.Emplace(SecondBreakPoint);
	BreakCicle.Vertices.Emplace(ThirdBreakPoint);
	AllCircle.Emplace(BreakCicle);

	return AllCircle;
}
