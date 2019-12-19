#include "HardModeData/DrawStyle/XRBaseEditStyle.h"
#include "ArmyEngineModule.h"

#include "HardModeData/GridSystemCore/XRBrickUnit.h"
#include "../Mathematics/EarcutTesselator.h"
#include "Math/XRMath.h"
#include "ResManager.h"
#include "ArmyResourceModule.h"
#include "SArmyMulitCategory.h"
#include "ArmySceneData.h"

IMPLEMENT_HIT_PROXY(HXRBaseAreaProxy, HHitProxy);

FArmyBaseEditStyle::FArmyBaseEditStyle()
    : RotationAngle(0.0f)
    , HeightToFloor(0.0f)
    , InternalDist(0.2f)
    , InFillColor(FColor::Black)
    , BNeedWallPaintColor(false)
    , WallPaintColor(FLinearColor(0.3f, 0.3f, 0.3f, 1.f))
    , LayingPointPos(FVector::ZeroVector)
	, ApplyBrickNum(0)
{
	UMaterial* LineMaterial = FArmyEngineModule::Get().GetEngineResource()->GetDefaultFloorTextureMat();
	MI_FloorTextureMat = UMaterialInstanceDynamic::Create(LineMaterial, NULL);
	MI_FloorTextureMat->AddToRoot();
	SecondTextureMat = UMaterialInstanceDynamic::Create(LineMaterial, NULL);
	SecondTextureMat->AddToRoot();

	ConstructionItemData = MakeShareable(new FArmyConstructionItemInterface);
}


void FArmyBaseEditStyle::CopyFrom(TSharedPtr <FArmyBaseEditStyle > OtherStyle)
{

	
	CurrentTextureNum = OtherStyle->CurrentTextureNum;

	 M_AlignmentType = OtherStyle->M_AlignmentType;
	 EdityType = OtherStyle->EdityType;

	// HeightToFloor = OtherStyle->HeightToFloor;
	 HasGoods = OtherStyle->HasGoods;
	 InFillColor = OtherStyle->InFillColor;
	 XDirOffset = OtherStyle->XDirOffset;
	 YDirOffset = OtherStyle->YDirOffset;
	 BodaAreaTextWidth = OtherStyle->BodaAreaTextWidth;
	 BodaAreaTextHeight = OtherStyle->BodaAreaTextHeight;

	 MainTextureUrl = OtherStyle->MainTextureUrl;
	 BNeedWallPaintColor = OtherStyle->BNeedWallPaintColor;

	 if (BNeedWallPaintColor)
	 {
		 SetWallPaintColor(OtherStyle->WallPaintColor);
	 }

	 CategoryID = OtherStyle->CategoryID;
	 GoodsID = OtherStyle->GoodsID;
	 SecondGoodsID = OtherStyle->SecondGoodsID;

	 BodaAreaTextHeight = OtherStyle->BodaAreaTextHeight;
	 BodaAreaTextHeight = OtherStyle->BodaAreaTextHeight;
	 ContentItem = OtherStyle->ContentItem;

	 DefaultGapColor = OtherStyle->DefaultGapColor;
	 GapColor = OtherStyle->GapColor;

	 CalculateClipperBricks(ClipperAreas, InnerHoles);
	 SetInternalDist(OtherStyle->GetInternalDist());
	 SetRotationAngle(OtherStyle->GetRotationAngle());
	 SetMainTexture(MainTexture);
	// IsDefaultStyle = OtherStyle->IsDefaultStyle;
	 
}


FArmyBaseEditStyle::~FArmyBaseEditStyle()
{

}

void FArmyBaseEditStyle::SetInternalDist(float dist)
{
	if (InternalDist != dist)
	{
		InternalDist = dist;
		CalculateClipperBricks(ClipperAreas, InnerHoles);

		StyleDirty.ExecuteIfBound();
	}
}


void FArmyBaseEditStyle::SetRotationAngle(float InAngle)
{
	if (RotationAngle != InAngle)
	{
		RotationAngle = InAngle - FMath::FloorToInt(InAngle / 360.0f) * 360;
		CalculateClipperBricks(ClipperAreas, InnerHoles);

		StyleDirty.ExecuteIfBound();

	}
}


void FArmyBaseEditStyle::SetStyleType(const EStyleType type)
{
	EdityType = type;

}
void FArmyBaseEditStyle::SetXDirOffset(const float InXOffset)
{
	if (XDirOffset != InXOffset)
	{
		XDirOffset = InXOffset;
		CalculateClipperBricks(ClipperAreas, InnerHoles);

		StyleDirty.ExecuteIfBound();

	}
}

void FArmyBaseEditStyle::SetYDirOffset(const float InYOffset)
{
	if (YDirOffset != InYOffset)
	{
		YDirOffset = InYOffset;
		CalculateClipperBricks(ClipperAreas, InnerHoles);

		StyleDirty.ExecuteIfBound();

	}
}

void FArmyBaseEditStyle::SetAlignType(AlignmentType InAlignType)
{
	if (M_AlignmentType != InAlignType)
	{
		M_AlignmentType = InAlignType;
		CalculateClipperBricks(ClipperAreas, InnerHoles);
		StyleDirty.ExecuteIfBound();

	}
}

void FArmyBaseEditStyle::SetMainTexture(UTexture2D* InTexture)
{
	if (InTexture == nullptr)
		return;
	MainTexture = InTexture;
	MainTexWidth = MainTexture->GetSizeX();
	MainTexHeight = MainTexture->GetSizeY();

	MI_FloorTextureMat->SetTextureParameterValue("MainTex", MainTexture);
}




void FArmyBaseEditStyle::SetMainTextureLocalPath(FString mainTexturePath)
{
	MainTextureUrl = mainTexturePath;
}

void FArmyBaseEditStyle::SetMainTextureInfo(UTexture2D* InTexture, int32 InWidth, int32 InLength)
{
	MainTexture = InTexture;
	MainTexWidth = InLength;
	MainTexHeight = InWidth;
	MI_FloorTextureMat->SetTextureParameterValue("MainTex", MainTexture);
}

void FArmyBaseEditStyle::SetMainMaterial(UMaterialInstanceDynamic * MID, int32 InWidth, int32 InLength)
{
	MainTexWidth = InLength;
	MainTexHeight = InWidth;
	MI_FloorTextureMat = MID;
}

void FArmyBaseEditStyle::SetCategoryID(int32 _categoryID)
{
	CategoryID = _categoryID;
}

int32 FArmyBaseEditStyle::GetCategoryID()
{
	return CategoryID;
}

void FArmyBaseEditStyle::SetGoodsID(int32 _GoodsID)
{
	HasGoods = true;
	GoodsID = _GoodsID;
}

void FArmyBaseEditStyle::SetSecondGoodID(int32 _GoodsID)
{
	HasGoods = true;
	SecondGoodsID = _GoodsID;
}

void FArmyBaseEditStyle::SetWallPaintColor(const FLinearColor& InColor)
{
	BNeedWallPaintColor = true;
	WallPaintColor = InColor;
	MI_FloorTextureMat->SetVectorParameterValue("Diffuse_Color", InColor);

}

int32 FArmyBaseEditStyle::GetGoodsID()
{

	if (ContentItem.IsValid())
	{
		return 	ContentItem->ID;
	}
	return GoodsID;
}

void FArmyBaseEditStyle::SetPlaneInfo(const FVector& PlaneOrginPos, const FVector& xDir, const FVector& yDir)
{
	PlaneCenterPos = PlaneOrginPos;
	PlaneXDir = xDir;
	PlaneYDir = yDir;
}

void FArmyBaseEditStyle::SetStylePlaneOffset(float InOffset)
{
	HeightToFloor = InOffset == 0.0f ? 0.25f : InOffset;
	for (auto& iter : ClipperBricks)
	{
		for (auto& iter0 : iter.Value)
		{
			iter0->SetPlaneOffset(InOffset);
		}
	}
	StyleDirty.ExecuteIfBound();

}

void FArmyBaseEditStyle::SetCurrentItem(const TSharedPtr< FContentItemSpace::FContentItem> item)
{
	TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = item->GetResObjNoComponent();
	if (resArr.Num() >= 1)
	{
		TSharedPtr<FContentItemSpace::FResObj> ResObj = resArr[0];
		TSharedPtr<FContentItemSpace::FArmyHardModeRes> Res = StaticCastSharedPtr<FContentItemSpace::FArmyHardModeRes>(ResObj);
		CurrentTextureNum = Res->UseTextureNum;
	}
	ContentItem = item;
}

int32 FArmyBaseEditStyle::GetBrickNum() const
{
	int32 brickNum = 0;
	for (auto& iter : ClipperBricks)
	{
		brickNum += iter.Value.Num();
	}
	return brickNum;
}

void FArmyBaseEditStyle::GetVertexInfo(TArray<FDynamicMeshVertex>& AllVertexs)
{
	if (ClipperBricks.Num() == 2)
		return;
	for (auto iter : ClipperBricks)
	{

		TArray < TSharedPtr<FArmyBrickUnit>>& TotalBricks = iter.Value;
		int number = TotalBricks.Num();
		for (int i = 0; i < number; i++)
		{
			AllVertexs.Append(TotalBricks[i]->GetVertices());
		}
	}
}

void FArmyBaseEditStyle::GetGapVertsInfo(TArray<FDynamicMeshVertex>& AllVerts)
{
	FBox2D box(GapVerts);

	float width = (box.Max - box.Min).X;
	float height = (box.Max - box.Min).Y;
	FVector normal = PlaneXDir ^PlaneYDir;
	FVector tangent = PlaneXDir;
	for (const FVector2D& iter : GapVerts)
	{
		float u = (iter - box.Min).X / width;
		float v = (iter - box.Min).X / height;

		AllVerts.Emplace(FDynamicMeshVertex(PlaneXDir * iter.X + PlaneYDir * iter.Y + PlaneCenterPos + normal * HeightToFloor, tangent, normal, FVector2D(u, v), FColor::White));
	}
}

 bool FArmyBaseEditStyle::GetIsNonePaveMethod() {

	TSharedPtr<FContentItemSpace::FContentItem> GoodItem = GetCurrentItem();
	EStyleType Styletype = GetEditType();
	if (!GoodItem.IsValid())
		return false;
	TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = GoodItem->GetResObjNoComponent();
	if (resArr.Num() < 1)
		return false;
	TSharedPtr<FContentItemSpace::FArmyHardModeRes> tempRes = StaticCastSharedPtr<FContentItemSpace::FArmyHardModeRes>(resArr[0]);
	if (!tempRes.IsValid())
		return false;

	return  tempRes->PavingMethodArr.Num()==0;

	return false;
}

TSharedPtr<class FArmyBrickUnit> FArmyBaseEditStyle::SelectedBrick(const FVector& pos, class UArmyEditorViewportClient* InViewportClient)
{
	for (auto& iter : ClipperBricks)
	{
		for (auto& iter0 : iter.Value)
		{
			if (iter0->IsSelected(pos, InViewportClient))
				return iter0;
		}
	}
	return NULL;
}


void FArmyBaseEditStyle::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	JsonWriter->WriteValue("edityType", int32(EdityType));
	JsonWriter->WriteValue("alignmentType", int32(M_AlignmentType));
	JsonWriter->WriteValue("xDirOffset", XDirOffset);
	JsonWriter->WriteValue("yDirOffset", YDirOffset);
	JsonWriter->WriteValue("internalDist", InternalDist);
	JsonWriter->WriteValue("rotationAngle", RotationAngle);
	JsonWriter->WriteValue("heightToFloor", HeightToFloor);
	JsonWriter->WriteValue("mainTexWidth", MainTexWidth);
	JsonWriter->WriteValue("mainTexHeight", MainTexHeight);
	JsonWriter->WriteValue("urlTexture", MainTextureUrl);//TODO
	JsonWriter->WriteValue("firstGoodID", GoodsID);
	JsonWriter->WriteValue("SecondGoodsID", SecondGoodsID);
	JsonWriter->WriteValue("HasGoods", HasGoods);
	JsonWriter->WriteValue("CurrentTextureNum", CurrentTextureNum);
	JsonWriter->WriteValue("BNeedWallPaintColor", BNeedWallPaintColor);
	JsonWriter->WriteValue("WallPaintColor", WallPaintColor.ToString());
//	JsonWriter->WriteValue<bool>("IsDefaultStyle", IsDefaultStyle);
	//@郭子阳
	//缝隙颜色
	JsonWriter->WriteValue<bool>("DefaultGapColor", DefaultGapColor);
	JsonWriter->WriteValue("GapColor", GapColor.ToString());
	//缝隙颜色，仅用于复制材质,并不是真正的缝隙颜色
	//FLinearColor GapColor;

	//@郭子阳 不保存施工项 V1.5
	/*if (!ConstructionItemData->bHasSetted && GoodsID != -1)
	{
		TSharedPtr<SArmyMulitCategory> ConstructionDetail = MakeShareable(new SArmyMulitCategory);
		ConstructionDetail->Init(FArmySceneData::Get()->GetJsonDataByItemID(GoodsID));
		ConstructionItemData->SetConstructionItemCheckedId(ConstructionDetail->GetCheckedData());
		ConstructionItemData->bHasSetted = true;
	}*/
	//ConstructionItemData->SerializeToJson(JsonWriter);
	//@郭子阳 保存施工项版本
	JsonWriter->WriteValue("ConstructionDataVer", (int32)ConstructionVersion);

	JsonWriter->WriteObjectStart("ContenItem");
	if (ContentItem.IsValid())
	{
		ContentItem->SerializeToJson(JsonWriter);
	}
	JsonWriter->WriteObjectEnd();

}

void FArmyBaseEditStyle::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	int32 TypeValue = (int32)InJsonData->GetNumberField("edityType");
	switch (TypeValue)
	{
	case 1:
		EdityType = S_ContinueStyle;
		break;
	case 2:
		EdityType = S_HerringBoneStyle;
		break;
	case 3:
		EdityType = S_TrapeZoidStyle;
		break;
	case 4:
		EdityType = S_WhirlwindStyle;
		break;
	case 5:
		EdityType = S_WorkerStyle;
		break;
	case 6:
		EdityType = S_CrossStyle;
		break;
	case 7:
		EdityType = S_IntervalStyle;
		break;
	case 8:
		EdityType = S_SeamlessStyle;
		break;
	case 10:
		EdityType = S_SlopeContinueStyle;
		break;
	default:
		EdityType = OT_StyleTypeNone;
		break;
	}
	int32 tempType = (int32)InJsonData->GetNumberField("alignmentType");
	M_AlignmentType = (AlignmentType)tempType;
	InternalDist = InJsonData->GetNumberField("internalDist");
	XDirOffset = InJsonData->GetNumberField("xDirOffset");
	YDirOffset = InJsonData->GetNumberField("yDirOffset");
	RotationAngle = InJsonData->GetNumberField("rotationAngle");
	HeightToFloor = InJsonData->GetNumberField("heightToFloor");
	MainTexWidth = InJsonData->GetNumberField("mainTexWidth");
	MainTexHeight = InJsonData->GetNumberField("mainTexHeight");
	GoodsID = InJsonData->GetNumberField("firstGoodID");
	SecondGoodsID = InJsonData->GetNumberField("SecondGoodsID");
	CurrentTextureNum = InJsonData->GetNumberField("CurrentTextureNum");


	//@郭子阳
	//缝隙颜色
	InJsonData->TryGetBoolField("DefaultGapColor", DefaultGapColor);
	FString GapColorString;
	if (InJsonData->TryGetStringField("GapColor", GapColorString))
	{
		GapColor.InitFromString(GapColorString);
	}
	//@郭子阳
	ConstructionItemData->Deserialization(InJsonData);
	uint32 TempInt;
	if (InJsonData->TryGetNumberField("ConstructionDataVer", TempInt))
	{
		ConstructionVersion = (EConstructionVersion)(int32)InJsonData->GetNumberField("ConstructionDataVer");
	}
	else
	{
		ConstructionVersion = EConstructionVersion::V140;
	}

	if (CurrentTextureNum == 0)
		CurrentTextureNum = 1;
	HasGoods = (bool)InJsonData->GetBoolField("HasGoods");
	InJsonData->TryGetStringField("urlTexture", MainTextureUrl);
	TSharedPtr<FJsonObject> itemObject = InJsonData->GetObjectField("ContenItem");
	if (itemObject.IsValid())
	{
		if (itemObject->Values.Num() > 0)
		{
			ContentItem = MakeShareable(new FContentItemSpace::FContentItem());
			ContentItem->Deserialization(itemObject);
		}
	}
	BNeedWallPaintColor = (bool)InJsonData->GetBoolField("BNeedWallPaintColor");
	/** @������ ��Ʒ .pak */
	if (MainTextureUrl.EndsWith(TEXT(".pak")))
	{
		UXRResourceManager * RM = FArmyResourceModule::Get().GetResourceManager();
		UMaterialInterface * MI = RM->CreateContentItemMaterial(ContentItem);
		if (!MI)
			MI = RM->CreateCustomMaterial(MainTextureUrl);
		MI_FloorTextureMat = dynamic_cast<UMaterialInstanceDynamic*>(MI);
		if (!MI_FloorTextureMat)
		{
			FVector planeNormal = PlaneXDir ^PlaneYDir;
			if (planeNormal.Equals(FVector(0, 0, 1)))
			{
				MI_FloorTextureMat = FArmyEngineModule::Get().GetEngineResource()->GetFloorMaterial();
				MI_FloorTextureMat->AddToRoot();
			}
			else
			{
				MI_FloorTextureMat = FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial();
				MI_FloorTextureMat->AddToRoot();
			}
		}
		if (BNeedWallPaintColor)
		{
			WallPaintColor.InitFromString(InJsonData->GetStringField("WallPaintColor"));
			MI_FloorTextureMat->SetVectorParameterValue("Diffuse_Color", WallPaintColor);
		}
	}
	/** @������ Ĭ�ϲ��� .uasset ����Ĭ�ϲ��ʵ�·�������л�*/
	else
	{
		/* ����Texture2D��·�������л�*/
		/* �ڴ������pak�ļ��и����ļ����ֶ�ȡ�ļ�*/
		MainTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *MainTextureUrl, NULL, LOAD_None, NULL));
		MI_FloorTextureMat->SetTextureParameterValue("MainTex", MainTexture);//������

		/* ����Material��·���ķ����л�*/
		if (MainTexture == NULL)
		{
			MI_FloorTextureMat = FArmyEngineModule::Get().GetEngineResource()->GetMaterialByURL(MainTextureUrl);
			MI_FloorTextureMat->AddToRoot();
		}
	}
}


//#define COMBINETEXTURE

void FArmyBaseEditStyle::Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View)
{

}

void FArmyBaseEditStyle::DrawWireFrame(class FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	for (auto& iter : ClipperBricks)
	{
		for (auto& iter0 : iter.Value)
		{
			iter0->Draw(PDI, View);
		}
	}
}

void FArmyBaseEditStyle::DrawHoles(class FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	for (auto& iter : ClipperBricks)
	{
		for (auto& iter0 : iter.Value)
		{
			iter0->Draw(PDI, View);
		}
	}
}

void FArmyBaseEditStyle::SetDrawArea(const TArray<FVector>& outAreas, const TArray<TArray<FVector>>& Holes)
{
	ClipperAreas = outAreas;
	InnerHoles = Holes;
	CalculateClipperBricks(outAreas, InnerHoles);
}



void FArmyBaseEditStyle::CalculateGapVerts(const TArray<FVector>& OutAreaVertices, const TArray<TArray<FVector>>& InneraHoles)
{
	TArray<TArray<FVector>> TotalHoles;
	for (auto& p : ClipperBricks)
	{
		for (TSharedPtr<FArmyBrickUnit> iter : p.Value)
		{
			TotalHoles.Emplace(iter->GetBrickVerts());
		}
	}
	for (auto& iter : InneraHoles)
	{
		TotalHoles.Emplace(iter);
	}
	GapVerts.Empty();
	Translate::ClipperGapVertsAndTriangle(OutAreaVertices, TotalHoles, GapVerts);
}

void FArmyBaseEditStyle::UpdateBrickUVInfo()
{
	for (auto& iter : ClipperBricks)
	{
		TArray<TSharedPtr<FArmyBrickUnit>>& totalBricks = iter.Value;
		uint32 id = iter.Key;
		Rectf result;
		if (M_TextureAtlas->GetRectPosById(id, result))
		{
			for (auto& iter0 : totalBricks)
			{
				iter0->SetUVRects(result);
			}
		}

	}
}
void FArmyBaseEditStyle::UpdateBrickNumAndDirectionStartPosInfo(const TArray<FVector>& OutAreaVertices, FVector& InStartPos, FVector& InWidthDirection, FVector& InHeightDirection, int& xNum, int& yNum)
{
    if (OutAreaVertices.Num() > 0 && LayingPointPos == FVector::ZeroVector)
    {
        LayingPointPos = OutAreaVertices[0];
    }

	FBox ClipperAreaBox(OutAreaVertices);
	FVector2D minPoint = FVector2D(ClipperAreaBox.Min);
	FVector2D maxPoint = FVector2D(ClipperAreaBox.Max);
	float tempMainTexHeight = MainTexHeight / 10.0f;
	float tempMainTexWidth = MainTexWidth / 10.0f;
	ClipperBricks.Empty();

	float sinRotation = 0, cosRotation = 1;

	InStartPos = FVector(minPoint, 0.0);
	// ˳ʱ����ת�����Ƕ�
	InWidthDirection = FVector(1, 0, 0).RotateAngleAxis(RotationAngle, FVector(0, 0, 1));
	InHeightDirection = FVector(0, 1, 0).RotateAngleAxis(RotationAngle, FVector(0, 0, 1));
	if (RotationAngle >= 0 && RotationAngle <= 90)
	{

		FMath::SinCos(&sinRotation, &cosRotation, FMath::DegreesToRadians(RotationAngle));
		InStartPos = FVector(minPoint, 0.0) - InHeightDirection * FMath::Abs((maxPoint - minPoint).X)*sinRotation;

	}
	else if (RotationAngle > 90 && RotationAngle <= 180)
	{
		FMath::SinCos(&sinRotation, &cosRotation, FMath::DegreesToRadians(180 - RotationAngle));
		InStartPos = FVector(maxPoint.X, minPoint.Y, 0.0) - InHeightDirection * FMath::Abs((maxPoint - minPoint).Y)*cosRotation;

	}
	else if (RotationAngle > 180 && RotationAngle <= 270)
	{
		FMath::SinCos(&sinRotation, &cosRotation, FMath::DegreesToRadians(RotationAngle - 180));
		InStartPos = FVector(maxPoint, 0.0) - InHeightDirection * FMath::Abs((maxPoint - minPoint).X)*sinRotation;
	}
	else if (RotationAngle > 270 && RotationAngle <= 360)
	{

		FMath::SinCos(&sinRotation, &cosRotation, FMath::DegreesToRadians(360 - RotationAngle));
		InStartPos = FVector(minPoint.X, maxPoint.Y, 0.0) - InHeightDirection * FMath::Abs((maxPoint - minPoint).Y)*cosRotation;

	}
	float birckdirectionLength = FMath::Abs((maxPoint - minPoint).X) * cosRotation + FMath::Abs((maxPoint - minPoint).Y) * sinRotation;
	float brickDirectionHeight = FMath::Abs((maxPoint - minPoint).X) * sinRotation + FMath::Abs((maxPoint - minPoint).Y) * cosRotation;

    float TotalXDirOffset = (LayingPointPos - InStartPos).X + XDirOffset / 10.f;
    float TotalYDirOffset = (LayingPointPos - InStartPos).Y + YDirOffset / 10.f;
	InStartPos += InWidthDirection * TotalXDirOffset + InHeightDirection * TotalYDirOffset;
	int xbrickNum = FMath::Abs(FMath::CeilToInt(TotalXDirOffset / tempMainTexWidth));
	int ybrickNum = FMath::Abs(FMath::CeilToInt(TotalYDirOffset / tempMainTexHeight));
	float tempXoffset = TotalXDirOffset >= 0 ? -xbrickNum * (MainTexWidth / 10.0f + InternalDist) : 0;
	float tempYoffset = TotalYDirOffset >= 0 ? -ybrickNum * (MainTexHeight / 10.0f + InternalDist) : 0;
	InStartPos += InWidthDirection * tempXoffset * 2 + InHeightDirection * tempYoffset * 2;

	xNum = FMath::CeilToInt(birckdirectionLength / tempMainTexWidth) + xbrickNum * 2;
	yNum = FMath::CeilToInt(brickDirectionHeight / tempMainTexHeight) + ybrickNum * 2;
	UpdateAlignMentInfo(InStartPos, InWidthDirection, InHeightDirection, xNum, yNum, birckdirectionLength, brickDirectionHeight);
}

void FArmyBaseEditStyle::RecaculateBrickLocation(const TArray<FVector>& OutAreaVertices, FVector& InStartPos, FVector&InWidthDirection, FVector& InHeightDirection, int& xNUm, int& yNum, TSharedPtr<FArmyBrickUnit> InBrick)
{
	FBox ClipperAreaBox(OutAreaVertices);
	FVector2D minPoint = FVector2D(ClipperAreaBox.Min);
	FVector2D maxPoint = FVector2D(ClipperAreaBox.Max);
	float tempMainTexHeight = MainTexHeight / 10.0f;
	float tempMainTexWidth = MainTexWidth / 10.0f;
	ClipperBricks.Empty();

	float sinRotation = 0, cosRotation = 1;

	InStartPos = FVector(minPoint, 0.0);
	// ˳ʱ����ת�����Ƕ�
	InWidthDirection = FVector(1, 0, 0).RotateAngleAxis(RotationAngle, FVector(0, 0, 1));
	InHeightDirection = FVector(0, 1, 0).RotateAngleAxis(RotationAngle, FVector(0, 0, 1));
	float UpDist = 0.0f;
	float DownDist = 0.0f;
	float leftDist = 0.0f;
	float rightDist = 0.0f;
	if (RotationAngle >= 0 && RotationAngle <= 90)
	{
		UpDist = FArmyMath::Distance(InBrick->GetPosition(), FVector(maxPoint.X, minPoint.Y, 0), FVector(maxPoint.X, minPoint.Y, 0) + InWidthDirection * 10);
		DownDist = FArmyMath::Distance(InBrick->GetPosition(), FVector(minPoint.X, maxPoint.Y, 0), FVector(minPoint.X, maxPoint.Y, 0) + InWidthDirection * 10);

		leftDist = FArmyMath::Distance(InBrick->GetPosition(), FVector(minPoint, 0.0), FVector(minPoint, 0) + InHeightDirection * 10);
		rightDist = FArmyMath::Distance(InBrick->GetPosition(), FVector(maxPoint, 0.0), FVector(maxPoint, 0.0) + InHeightDirection * 10.0f);


	}
	else if (RotationAngle > 90 && RotationAngle <= 180)
	{
		UpDist = FArmyMath::Distance(InBrick->GetPosition(), FVector(maxPoint, 0.0), FVector(maxPoint, 0.0) + InWidthDirection * 10.0f);
		DownDist = FArmyMath::Distance(InBrick->GetPosition(), FVector(minPoint, 0.0), FVector(minPoint, 0.0) + InWidthDirection*10.0f);

		leftDist = FArmyMath::Distance(InBrick->GetPosition(), FVector(maxPoint.X, minPoint.Y, 0.0), FVector(maxPoint.X, minPoint.Y, 0.0) + InHeightDirection * 10.0f);
		rightDist = FArmyMath::Distance(InBrick->GetPosition(), FVector(minPoint.X, maxPoint.Y, 0.0), FVector(minPoint.X, maxPoint.Y, 0.0) + InHeightDirection * 10.0f);


	}
	else if (RotationAngle > 180 && RotationAngle <= 270)
	{
		UpDist = FArmyMath::Distance(InBrick->GetPosition(), FVector(minPoint.X, maxPoint.Y, 0), FVector(minPoint.X, maxPoint.Y, 0) + InWidthDirection * 10.0f);
		DownDist = FArmyMath::Distance(InBrick->GetPosition(), FVector(maxPoint.X, minPoint.Y, 0), FVector(maxPoint.X, minPoint.Y, 0) + InWidthDirection* 10.0f);

		leftDist = FArmyMath::Distance(InBrick->GetPosition(), FVector(maxPoint, 0.0), FVector(maxPoint, 0.0) + InHeightDirection * 10.0f);
		rightDist = FArmyMath::Distance(InBrick->GetPosition(), FVector(minPoint, 0.0), FVector(minPoint, 0.0) + InHeightDirection * 10.0f);
	}

	else if (RotationAngle > 270 && RotationAngle < 360)
	{
		UpDist = FArmyMath::Distance(InBrick->GetPosition(), FVector(minPoint, 0.0), FVector(minPoint, 0.0) + InWidthDirection * 10.0f);
		DownDist = FArmyMath::Distance(InBrick->GetPosition(), FVector(maxPoint, 0.0), FVector(maxPoint, 0.0) + InWidthDirection * 10.0f);

		leftDist = FArmyMath::Distance(InBrick->GetPosition(), FVector(minPoint.X, maxPoint.Y, 0.0), FVector(minPoint.X, maxPoint.Y, 0.0) + InHeightDirection * 10.0f);
		rightDist = FArmyMath::Distance(InBrick->GetPosition(), FVector(maxPoint.X, minPoint.Y, 0.0), FVector(maxPoint.X, minPoint.Y, 0.0) + InHeightDirection * 10.0f);

	}

	int32 UpBrickNum = FMath::CeilToInt((UpDist - InBrick->GetBrickHeight() / 2) / InBrick->GetBrickHeight());
	int32 DownBrickNum = FMath::CeilToInt((DownDist - InBrick->GetBrickHeight() / 2) / InBrick->GetBrickHeight());
	yNum = UpBrickNum + DownBrickNum + 1;

	int32 LeftBrickNum = FMath::CeilToInt((leftDist - InBrick->GetBrickWidth() / 2) / InBrick->GetBrickWidth());
	int32 RightBrickNum = FMath::CeilToInt((rightDist - InBrick->GetBrickWidth() / 2) / InBrick->GetBrickWidth());
	xNUm = LeftBrickNum + RightBrickNum + 1;

	InStartPos = InBrick->GetPosition() - InWidthDirection * LeftBrickNum - InHeightDirection * UpBrickNum;
}

void FArmyBaseEditStyle::UpdateAlignMentInfo(FVector& InPos, FVector& InWidthDirection, FVector& InHeight, int& xNum, int& ynum, const float boxWidth, const float boxHeight)
{
	switch (M_AlignmentType)
	{
	case LeftTop:
		break;
	case MidTop:
	{
		uint32 tempNum = FMath::CeilToInt((boxWidth - MainTexWidth / 10.0f * 2) / (2 * MainTexWidth / 10.0f)) + 1;
		FVector middletop = InPos + InWidthDirection * boxWidth / 2;
		InPos = middletop - tempNum*(MainTexWidth / 10.f + InternalDist) *InWidthDirection;
		xNum += 1;
	}
	break;
	case RightTop:
	{
		uint32 tempNum = FMath::CeilToInt((boxWidth - MainTexWidth / 10.0f * 2) / (MainTexWidth / 10.0f)) + 2;
		FVector rightTop = InPos + InWidthDirection * boxWidth;
		InPos = rightTop - tempNum*(MainTexWidth / 10.0f + InternalDist) *InWidthDirection;
		xNum += 1;
	}
	break;
	case LeftMid:
	{
		uint32 tempNum = FMath::CeilToInt((boxHeight - MainTexHeight / 10.0f * 2) / (MainTexHeight / 10.0f)) + 1;
		FVector leftMide = InPos + InHeight * (boxHeight - (MainTexHeight / 10.0 * 2));
		InPos = leftMide - tempNum*(MainTexHeight / 10.f + InternalDist)* InHeight;
		ynum += 1;
	}
	break;
	case MidMid:
	{
		uint32 tempXNum = FMath::CeilToInt((boxWidth - MainTexWidth / 10.0f * 2) / (2 * MainTexWidth / 10.0f)) + 1;
		uint32 tempYNum = FMath::CeilToInt((boxHeight - MainTexHeight / 10.0f * 2) / (2 * MainTexHeight / 10.0f)) + 1;
		FVector midmid = InPos + InHeight*boxHeight / 2 + InWidthDirection * boxWidth / 2;
		InPos = midmid - tempXNum*(MainTexWidth / 10.0f + InternalDist) * InWidthDirection - tempYNum * (MainTexHeight / 10.0f + InternalDist) * InHeight;
		xNum += 1;
		ynum += 1;
	}
	break;
	case RightMid:
	{
		uint32 tempXNum = FMath::CeilToInt((boxWidth - MainTexWidth / 10.0f * 2) / (2 * MainTexWidth / 10.0f)) + 1;
		uint32 tempYNum = FMath::CeilToInt((boxHeight - MainTexHeight / 10.0f * 2) / (2 * MainTexHeight / 10.0f)) + 1;
		FVector midmid = InPos + InHeight*boxHeight / 2 + InWidthDirection * boxWidth / 2;
		InPos = midmid - tempXNum*(MainTexWidth / 10.0f + InternalDist) * InWidthDirection - tempYNum * (MainTexHeight / 10.0f + InternalDist) * InHeight;
		xNum += 1;
		ynum += 1;
	}
	break;
	case LeftDown:
	{
		uint32 tempYNum = FMath::CeilToInt((boxHeight - MainTexHeight / 10.0f) / (MainTexHeight / 10.0f)) + 1;
		FVector leftDown = InPos + InHeight * boxHeight;
		InPos = leftDown - tempYNum * (MainTexHeight / 10.0f + InternalDist) * InHeight;
		ynum += 1;


	}
	break;
	case MidDown:
	{
		uint32 tempXNum = FMath::CeilToInt((boxWidth - MainTexWidth / 10.0f) / (2 * MainTexWidth / 10.0f)) + 1;
		uint32 tempYNum = FMath::CeilToInt((boxHeight - MainTexHeight / 10.0f) / (MainTexHeight / 10.0f)) + 1;
		FVector middown = InPos + InHeight*boxHeight + InWidthDirection * boxWidth / 2;
		InPos = middown - tempXNum*(MainTexWidth / 10.0f + InternalDist) * InWidthDirection - tempYNum * (MainTexHeight / 10.0f + InternalDist) * InHeight;
		xNum += 1;
		ynum += 1;

	}
	break;
	case RightDown:
	{
		uint32 tempXNum = FMath::CeilToInt((boxWidth - MainTexWidth / 10.0f) / (MainTexWidth / 10.0f)) + 1;
		uint32 tempYNum = FMath::CeilToInt((boxHeight - MainTexHeight / 10.0f) / (MainTexHeight / 10.0f)) + 1;
		FVector rightdown = InPos + InHeight*boxHeight + InWidthDirection * boxWidth;
		InPos = rightdown - tempXNum*(MainTexWidth / 10.0f + InternalDist) * InWidthDirection - tempYNum * (MainTexHeight / 10.0f + InternalDist) * InHeight;
		xNum += 1;
		ynum += 1;
	}
	break;
	default:
		break;
	}
}
