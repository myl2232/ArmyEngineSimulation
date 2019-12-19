#include "ArmyNewPass.h"
#include "ArmyPolyline.h"
#include "ArmyRect.h"
#include "ArmyEngineModule.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "ArmyGameInstance.h"
#include "ArmyExtrusion/Public/XRExtrusionActor.h"
#include "ArmyActorConstant.h"
#include "ArmyResourceModule.h"
#include "ArmySceneData.h"
#include  "ArmyConstructionManager.h"
FArmyNewPass::FArmyNewPass()
	: FArmyHardware()
{
	ObjectType = OT_NewPass;
	SetPropertyFlag(FLAG_COLLISION, true);	

	StartPass = MakeShareable(new FArmyPolyline());
	StartPass->SetLineType(FArmyPolyline::LineStrip);
	StartPass->SetLineWidth(WALLLINEWIDTH + 0.1f);
	EndPass = MakeShareable(new FArmyPolyline());
	EndPass->SetLineType(FArmyPolyline::LineStrip);
	EndPass->SetLineWidth(WALLLINEWIDTH + 0.1f);

	Spacing = MakeShareable(new FArmyRect());	

	ModifyRect = MakeShareable(new FArmyRect());
	ModifyRect->SetCalcUVs(true);
	ModifyRect->Width = 0;
	ModifyRect->Height = 0;
	ModifyRect->LineThickness = WALLLINEWIDTH + 0.1F;

	UMaterial* ModifyMaterial = FArmyEngineModule::Get().GetEngineResource()->GetModifyWallMaterial();
	MI_ModifyWall = UMaterialInstanceDynamic::Create(ModifyMaterial, nullptr);
	MI_ModifyWall->AddToRoot();
	MI_ModifyWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor(FColor(0xFFE305E5)));
	ModifyRect->MaterialRenderProxy = MI_ModifyWall->GetRenderProxy(false);
	ModifyRect->bIsFilled = true;
	ModifyRect->SetBaseColor(FLinearColor(FColor(0xFFE305E5)));

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

	SetName(TEXT("门洞"));
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

FArmyNewPass::FArmyNewPass(FArmyNewPass * Copy)
{
	bIsCopy = true;
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

	UMaterial* ModifyMaterial = FArmyEngineModule::Get().GetEngineResource()->GetModifyWallMaterial();
	MI_ModifyWall = UMaterialInstanceDynamic::Create(ModifyMaterial, nullptr);
	MI_ModifyWall->AddToRoot();
	MI_ModifyWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor(FColor(0xFFE305E5)));

	ModifyRect = MakeShareable(new FArmyRect(Copy->ModifyRect.Get()));
	ModifyRect->MaterialRenderProxy = MI_ModifyWall->GetRenderProxy(false);
	ModifyRect->bIsFilled = true;
	ModifyRect->SetCalcUVs(true);
	ModifyRect->SetBaseColor(FLinearColor(FColor(0xFFE305E5)));

	TopViewRectImagePanel = MakeShareable(new FArmyRect(Copy->TopViewRectImagePanel.Get()));
	TopViewStartPass = MakeShareable(new FArmyPolyline(Copy->TopViewStartPass.Get()));
	TopViewEndPass = MakeShareable(new FArmyPolyline(Copy->TopViewStartPass.Get()));
	TopViewSpacing = MakeShareable(new FArmyRect(Copy->TopViewSpacing.Get()));

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

	Update();

	ConstructionItemData = MakeShareable(new FArmyConstructionItemInterface);
}

FArmyNewPass::~FArmyNewPass()
{
}

void FArmyNewPass::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyHardware::SerializeToJson(JsonWriter);	

	JsonWriter->WriteValue("pos", GetPos().ToString());
	JsonWriter->WriteValue("direction", GetDirection().ToString());
	JsonWriter->WriteValue("length", GetLength());
	JsonWriter->WriteValue("width", GetWidth());
	JsonWriter->WriteValue("height", GetHeight());
	JsonWriter->WriteValue("outerWidth", GetOuterWidth());
	JsonWriter->WriteValue("thickness", GetThickness());
	JsonWriter->WriteValue("bRightOpen", IsRightOpen());
	
	JsonWriter->WriteValue("MatType", GetMatType());

	JsonWriter->WriteValue("ModifyWallType", ModifyDoorHoleMatType.ToString());

	JsonWriter->WriteValue("bRectIsFilled", RectImagePanel->bIsFilled);

	JsonWriter->WriteValue("bIfGenerateModel", bIfGenerateModel);

	JsonWriter->WriteValue("SaleID", SaleID);
    JsonWriter->WriteValue("thumbnailUrl", ThumbnailUrl);



	if (ExtrusionActor &&SaleID>0)
	{
		GGI->DesignEditor->SaveFileList.AddUnique(SaleID);
	}

	/**@欧石楠 存储施工项*/
	ConstructionItemData->SerializeToJson(JsonWriter);

	//@打扮家 XRLightmass 序列化LightMapID
	if (ExtrusionActor && ExtrusionActor->MeshComponent->bBuildStaticLighting)
	{
		LightMapID = ExtrusionActor->GetLightMapGUIDFromSMC();
		JsonWriter->WriteValue("LightMapID", LightMapID.ToString());
	}

	SERIALIZEREGISTERCLASS(JsonWriter, FArmyNewPass)
}

void FArmyNewPass::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyHardware::Deserialization(InJsonData);
	
	FVector Pos, Direction;
	Pos.InitFromString(InJsonData->GetStringField("pos"));
	Direction.InitFromString(InJsonData->GetStringField("direction"));
	SetPos(Pos);
	SetDirection(Direction);
	SetLength(InJsonData->GetNumberField("length"));
	SetWidth(InJsonData->GetNumberField("Width"));
	SetHeight(InJsonData->GetNumberField("height"));
	SetOuterWidth(InJsonData->GetNumberField("outerWidth"));
	SetThickness(InJsonData->GetNumberField("thickness"));
	SetRightOpen(InJsonData->GetBoolField("bRightOpen"));
	
	SetModifyDoorHoleMatType(FText::FromString(InJsonData->GetStringField("ModifyWallType")));

	SetMatType(InJsonData->GetStringField(TEXT("MatType")));

	RectImagePanel->bIsFilled = InJsonData->GetBoolField("bRectIsFilled");

	bIfGenerateModel = InJsonData->GetBoolField("bIfGenerateModel");

    InJsonData->TryGetNumberField("SaleID", SaleID);
    InJsonData->TryGetStringField("thumbnailUrl", ThumbnailUrl);

	/**@欧石楠 读取施工项*/
	ConstructionItemData->Deserialization(InJsonData);

	//@打扮家 XRLightmass 反序列化LightMapID
	FString LightGUIDStr = "";
	InJsonData->TryGetStringField("LightMapID", LightGUIDStr);
	FGuid::Parse(LightGUIDStr, LightMapID);

	Update();

	bool bCapture = CaptureDoubleLine(Pos, E_LayoutModel);	

	if (GetPropertyFlag(FLAG_LAYOUT))
	{
		StartPass->SetBaseColor(FLinearColor::White);
		EndPass->SetBaseColor(FLinearColor::White);
		Spacing->SetBaseColor(FLinearColor::White);
	}


}

void FArmyNewPass::SetState(EObjectState InState)
{
	State = InState;

	switch (InState)
	{
	case OS_Normal:
		StartPass->SetBaseColor(FLinearColor(FColor(0xFFFF700)));
		EndPass->SetBaseColor(FLinearColor(FColor(0xFFFF700)));
		Spacing->SetBaseColor(FLinearColor(FColor(0xFFFF700)));
		DeselectPoints();		
		MI_ModifyWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor(FColor(0xFFE305E5)));	

		break;

	case OS_Selected:
		StartPass->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		EndPass->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		Spacing->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		
		//MI_ModifyWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor(FColor(0xFFFF9800)));
		break;

	default:
		break;
	}
}

void FArmyNewPass::Draw(FPrimitiveDrawInterface * PDI, const FSceneView * View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{				
		if (FArmyObject::GetDrawModel(MODE_TOPVIEW) || FArmyObject::GetDrawModel(MODE_CEILING))
		{
			TopViewRectImagePanel->Draw(PDI, View);
			TopViewStartPass->Draw(PDI, View);
			TopViewEndPass->Draw(PDI, View);
			TopViewSpacing->Draw(PDI, View);
		}
		else
		{
			RectImagePanel->Draw(PDI, View);
			StartPass->Draw(PDI, View);
			EndPass->Draw(PDI, View);

			if (InWallType == 0 && bIsCopy && !GetPropertyFlag(FLAG_LAYOUT))
			{
				ModifyRect->Draw(PDI, View);
			}

			FArmyHardware::Draw(PDI, View);
		}
	}
}

bool FArmyNewPass::IsSelected(const FVector & Pos, UArmyEditorViewportClient * InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{		
		if (bIfGenerateModel)
		{			
			if (RectImagePanel->IsSelected(Pos, InViewportClient) || StartPass->IsSelected(Pos, InViewportClient) || EndPass->IsSelected(Pos, InViewportClient))
			{
				bShowDashLine = true;
				bShowEditPoint = true;
				return true;
			}
		}
		else
		{
			if (RectImagePanel->IsSelected(Pos, InViewportClient))
			{
				bShowDashLine = true;
				bShowEditPoint = true;
				return true;
			}
		}						
	}

	return false;
}

bool FArmyNewPass::Hover(const FVector & Pos, UArmyEditorViewportClient * InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		return RectImagePanel->IsSelected(Pos, InViewportClient);
	}
	return false;
}

const FBox FArmyNewPass::GetBounds()
{
	FVector HorizontalDirection = FArmyMath::GetLineDirection(GetStartPos(), GetEndPos());
	FVector VerticalDirection = HorizontalDirection.RotateAngleAxis(90, FVector(0, 0, 1));

	TArray<FVector> BoundingBox;
	BoundingBox.Push(GetStartPos() + VerticalDirection * (Width) / 2.0f);
	BoundingBox.Push(GetStartPos() - VerticalDirection* (Width) / 2.0f);
	BoundingBox.Push(GetEndPos() + VerticalDirection * (Width) / 2.0f);
	BoundingBox.Push(GetEndPos() - VerticalDirection* (Width) / 2.0f);
	return FBox(BoundingBox);
}

void FArmyNewPass::Destroy()
{
	if (ExtrusionActor->IsValidLowLevel())
	{
		ExtrusionActor->Destroy();
		ExtrusionActor = nullptr;
	}
}

void FArmyNewPass::SetPropertyFlag(PropertyType InType, bool InUse)
{
	FArmyHardware::SetPropertyFlag(InType, InUse);
	if (InType == FArmyObject::FLAG_LAYOUT && InUse)
	{
		StartPass->SetBaseColor(FLinearColor::White);
		EndPass->SetBaseColor(FLinearColor::White);
		Spacing->SetBaseColor(FLinearColor::White);
	}
}

void FArmyNewPass::Update()
{
	FArmyHardware::Update();

	RectImagePanel->Pos = GetStartPos() + Width / 2 * Direction;
	RectImagePanel->Pos.Z = 1.f;//为了让门挡住墙体线
	RectImagePanel->XDirection = HorizontalDirection;
	RectImagePanel->YDirection = -Direction;
	RectImagePanel->Width = Length;
	RectImagePanel->Height = Width;	

	ModifyRect->Pos = GetStartPos() + Width / 2 * Direction;
	ModifyRect->Pos.Z = 1.f;//为了让门挡住墙体线
	ModifyRect->XDirection = HorizontalDirection;
	ModifyRect->YDirection = -Direction;
	ModifyRect->Width = Length;
	ModifyRect->Height = Width;

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
}

void FArmyNewPass::Generate(UWorld * InWorld)
{
	if (!bGenerate3D)
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

void FArmyNewPass::ReplaceGoods(FContentItemPtr InContentItem, UWorld* InWorld)
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

		// 更新拆改模式垭口的数据
		TWeakPtr<FArmyObject> ModifyPass = FArmySceneData::Get()->GetObjectByGuid(E_ModifyModel, GetUniqueID());
		if (ModifyPass.IsValid())
		{
			TSharedPtr<FArmyNewPass> ModifyNewPass = StaticCastSharedPtr<FArmyNewPass>(ModifyPass.Pin());
			if (ModifyNewPass.IsValid())
			{
				ModifyNewPass->SaleID = InContentItem->ID;
				ModifyNewPass->ThumbnailUrl = InContentItem->ThumbnailURL;
			}
		}


		//@郭子阳
		//设置原始户型\拆改后中的的SaleID
		auto HomeObject = FArmySceneData::Get()->GetObjectByGuid(EModelType::E_HomeModel, GetUniqueID());
		auto HomeFArmyNewPass = StaticCastSharedPtr<FArmyNewPass>(HomeObject.Pin());
		if (HomeFArmyNewPass.IsValid())
		{
			HomeFArmyNewPass->SaleID = SaleID;
		}
		auto LayoutObject = FArmySceneData::Get()->GetObjectByGuid(EModelType::E_LayoutModel, GetUniqueID());
		auto LayoutFArmyNewPass = StaticCastSharedPtr<FArmyNewPass>(LayoutObject.Pin());
		if (LayoutFArmyNewPass.IsValid())
		{
			LayoutFArmyNewPass->SaleID = SaleID;
		}


		//@郭子阳 请求施工项
		if (HasGoods())
		{


			//@郭子阳
			//请求3D模式施工项
			XRConstructionManager::Get()->TryToFindConstructionData(this->GetUniqueID(), *(this->GetConstructionParameter().Get()), nullptr);
		}
	}
}

TArray<struct FLinesInfo> FArmyNewPass::GetFacadeBox()
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

TArray<FVector> FArmyNewPass::GetClipingBox()
{
	StartPoint->Pos.Z = 0;
	EndPoint->Pos.Z = 0;
	
	FVector HorizontalDirection = FArmyMath::GetLineDirection(GetStartPos(), GetEndPos());
	FVector VerticalDirection = HorizontalDirection.RotateAngleAxis(90, FVector(0, 0, 1));
	FVector LeftTop = GetStartPos() + VerticalDirection * (Width + SubTracteThicknessError) / 2.0f;
	FVector LeftBootom = GetStartPos() - VerticalDirection* (Width + SubTracteThicknessError) / 2.0f;
	FVector RightTop = GetEndPos() + VerticalDirection * (Width + SubTracteThicknessError) / 2.0f;
	FVector RightBottom = GetEndPos() - VerticalDirection* (Width + SubTracteThicknessError) / 2.0f;
	FVector Offset = FVector(0, 0, Height / 2);
	TArray<FVector> BoundingBox = { (LeftBootom + Offset),(LeftTop + Offset),(RightTop + Offset),(RightBottom + Offset) };
	return BoundingBox;
}

void FArmyNewPass::SetThickness(const float InThickness)
{
	if (Thickness != InThickness)
	{
		Thickness = InThickness;
		Update();
	}
}

void FArmyNewPass::SetOuterWidth(const float InOuterWidth)
{
	if (OuterWidth != InOuterWidth)
	{
		OuterWidth = InOuterWidth;
		Update();
	}
}

const void FArmyNewPass::GetAlonePoints(TArray<TSharedPtr<FArmyPoint>>& OutPoints)
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

void FArmyNewPass::SetIfGeneratePassModel(bool bGenerate)
{
	bIfGenerateModel = bGenerate;
	Update();
	FArmySceneData::Get()->ModifyMode_ModifyMultiDelegate.Broadcast();
}

void FArmyNewPass::SetModifyDoorHoleMatType(FText InStr)
{
}

void FArmyNewPass::ReqModifyMaterialSaleID()
{
}

void FArmyNewPass::ResModifyMaterialSaleID(TSharedPtr<FJsonObject> Data)
{
}

void FArmyNewPass::GetModifyWallAreaAndPerimeter(float & OutArea, float & OutPerimeter)
{
    OutArea = Length * Height * 0.0001f;
    OutPerimeter = (Length + Height * 2) * 0.01f;
}

void FArmyNewPass::GetOriginPassArea(float & OutArea)
{
	OutArea = 0;
	OutArea = (Height * 2 + Length) * Width * 0.0001f;
}
//获取施工项查询Key
TSharedPtr<ObjectConstructionKey> FArmyNewPass::GetConstructionKey()
{

	auto Key = MakeShared<ObjectConstructionKey>(GetUniqueID(), *GetConstructionParameter().Get());
	//ObjectConstructionKey Key(GetUniqueID(), P);
	return Key;

}

void FArmyNewPass::ConstructionData(TArray<struct FArmyGoods>& ArtificialData)
{
	if (!HasGoods()  || !bIfGenerateModel)
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
	Goods.Param.L += Goods.DoorMsg.DoorHeight * 2 + Goods.DoorMsg.DoorWidth;
	Goods.Param.D += 1;
}

TSharedPtr<FArmyRoom> FArmyNewPass::GetRoom()
{
	if (FirstRelatedRoom.IsValid() && SecondRelatedRoom.IsValid())
	{
		//处于墙之间的门没有空间
		return nullptr;
	}

	return FirstRelatedRoom.IsValid() ? FirstRelatedRoom : SecondRelatedRoom;
}

void FArmyNewPass::OnRoomSpaceIDChanged(int32 NewSpaceID)
{
	XRConstructionManager::Get()->TryToFindConstructionData(GetUniqueID(), *GetConstructionParameter().Get(), nullptr);
}

TSharedPtr<ConstructionPatameters> FArmyNewPass::GetConstructionParameter()
{
	auto  P = MakeShared<ConstructionPatameters>();

	int32 SpaceID = -1;
	auto Room = GetRoom();

	if (Room.IsValid())
	{
		SpaceID = Room->GetSpaceId();
	}

	P->SetNormalGoodsInfo(GetSaleID(), SpaceID);
	return P;
}

void FArmyNewPass::SetMatType(FString InType)
{
	if (InType == "")
	{
		InType = TEXT("轻体砖");
	}
	{ MatType = InType; }
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
}
