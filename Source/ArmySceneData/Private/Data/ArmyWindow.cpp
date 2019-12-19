#include "ArmyWindow.h"
#include "ArmyMath.h"
#include "ArmyShapeBoardActor.h"
#include "Primitive2D/XRRect.h"
#include "Primitive2D/XRLine.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyStyle.h"
#include "ArmyEngineModule.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "ArmyWindowActor.h"
#include "ResManager.h"
#include "ArmyResourceModule.h"
#include "ArmyGameInstance.h"
#include "ArmyActorConstant.h"
#include "ArmyWallLine.h"
#include "ArmyConstructionManager.h"
#include "ArmyConstructionQuantity.h"

#define WindowModelLength 150
#define WindowModelWidth 17
#define GlassWidth 2
#define WindowModelHeight 150
#define WindowSillWidth 18
#define WindowSillHeight 5
#define WindowSillLength 243
#define WINDOW_HEIGHT_TO_FLOOR 90

FArmyWindow::FArmyWindow()
	: FArmyHardware()
{
	ObjectType = OT_Window;
	SetPropertyFlag(FLAG_COLLISION, true);

	Length = WindowModelLength;
	Width = WindowModelWidth;
	Height = WindowModelHeight;
	HeightToFloor = WINDOW_HEIGHT_TO_FLOOR;

	Direction = FVector(1, 0, 0);
	SillThickness = WindowSillHeight;
	//RectImagePanel = MakeShareable(new FArmyRect());
	WindowGlass1 = MakeShareable(new FArmyLine());
	WindowGlass2 = MakeShareable(new FArmyLine());
	CenterLine = MakeShareable(new FArmyLine);
	RectImagePanel->SetBaseColor(FLinearColor::White);
	WindowGlass1->SetBaseColor(FLinearColor::White);
	WindowGlass2->SetBaseColor(FLinearColor::White);
	CenterLine->SetBaseColor(FLinearColor::White);

	TopViewWindowFrame = MakeShareable(new FArmyRect());
	TopViewWindowGlass1 = MakeShareable(new FArmyLine());
	TopViewWindowGlass2 = MakeShareable(new FArmyLine());
	TopCenterLine = MakeShareable(new FArmyLine);
	TopViewWindowFrame->SetBaseColor(FLinearColor::Black);
	TopViewWindowGlass1->SetBaseColor(FLinearColor::Black);
	TopViewWindowGlass2->SetBaseColor(FLinearColor::Black);
	TopCenterLine->SetBaseColor(FLinearColor::Black);

	UMaterialInstanceDynamic* MID_BaseColor = FArmyEngineModule::Get().GetEngineResource()->GetUnStageBaseColorMaterial();
	MID_BaseColor->AddToRoot();
	MID_BaseColor->SetVectorParameterValue("BaseColor", FLinearColor(FColor(0xFFE6E6E6)));
	TopViewWindowFrame->MaterialRenderProxy = MID_BaseColor->GetRenderProxy(false);
	TopViewWindowFrame->bIsFilled = true;


	SetName(TEXT("标准窗")/* + GetUniqueID().ToString()*/);


	WindowType = 3;
	HoleWindow = NULL;
	DirectionPoint->SetState(FArmyPrimitive::OPS_Disable);
}

FArmyWindow::FArmyWindow(FArmyWindow* Copy)
{
	SillThickness = Copy->SillThickness;
	bBoundingBox = Copy->bBoundingBox;
	State = Copy->GetState();
	PropertyFlag = Copy->PropertyFlag;
	WindowType = Copy->WindowType;
	WindowGlass1 = MakeShareable(new FArmyLine(Copy->WindowGlass1.Get()));
	WindowGlass2 = MakeShareable(new FArmyLine(Copy->WindowGlass2.Get()));
	CenterLine = MakeShareable(new FArmyLine(Copy->CenterLine.Get()));
	StartPoint = MakeShareable(new FArmyEditPoint(Copy->StartPoint.Get()));
	EndPoint = MakeShareable(new FArmyEditPoint(Copy->EndPoint.Get()));
	Point = MakeShareable(new FArmyEditPoint(Copy->Point.Get()));
	DirectionPoint = MakeShareable(new FArmyEditPoint(Copy->DirectionPoint.Get()));
	HeightToFloor = Copy->HeightToFloor;
	Direction = Copy->Direction;
	SetName(Copy->GetName());
	Length = Copy->Length;
	Width = Copy->Width;
	Height = Copy->Height;
	ObjectType = Copy->ObjectType;
	HoleWindow = Copy->HoleWindow;
	LinkFirstLine = Copy->LinkFirstLine;
	LinkSecondLine = Copy->LinkSecondLine;
	FirstRelatedRoom = Copy->FirstRelatedRoom;
	SecondRelatedRoom = Copy->SecondRelatedRoom;
	AttachModelName = Copy->AttachModelName;
	RectImagePanel = MakeShareable(new FArmyRect(Copy->RectImagePanel.Get()));
	RectImagePanel->SetBaseColor(GVC->GetBackgroundColor());
	RectImagePanel->LineThickness = WALLLINEWIDTH + 0.1f;

	TopViewWindowFrame = MakeShareable(new FArmyRect(Copy->TopViewWindowFrame.Get()));
	TopViewWindowGlass1 = MakeShareable(new FArmyLine(Copy->TopViewWindowGlass1.Get()));
	TopViewWindowGlass2 = MakeShareable(new FArmyLine(Copy->TopViewWindowGlass2.Get()));
	TopCenterLine = MakeShareable(new FArmyLine(Copy->TopCenterLine.Get()));


	ContentItem = Copy->ContentItem;

	Update();
}

FArmyWindow::~FArmyWindow()
{
	//DestroyWindowModel();
}

void FArmyWindow::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyHardware::SerializeToJson(JsonWriter);

	JsonWriter->WriteValue("pos", GetPos().ToString());
	JsonWriter->WriteValue("direction", GetDirection().ToString());
	JsonWriter->WriteValue("length", GetLength());
	JsonWriter->WriteValue("width", GetWidth());
	JsonWriter->WriteValue("height", GetHeight());
	JsonWriter->WriteValue("heightToFloor", HeightToFloor);
	JsonWriter->WriteValue("bRightOpen", IsRightOpen());
	JsonWriter->WriteValue("bRectIsFilled", RectImagePanel->bIsFilled);
	JsonWriter->WriteValue("bGenerateWindowStone", bIfGenerateWindowStone);
	JsonWriter->WriteValue("thumbnailUrl", ThumbnailUrl);

	JsonWriter->WriteValue("SaleID", SaleID);



	if (HoleWindow)
	{

		UXRResourceManager * ResMg = FArmyResourceModule::Get().GetResourceManager();
		//TSharedPtr<FContentItemSpace::FContentItem> item = ResMg->GetContentItemFromID(HoleWindow->GetUniqueID());
		TSharedPtr<FContentItemSpace::FContentItem> ResultSynData = ResMg->GetContentItemFromID(HoleWindow->GetSynID());
		if (ResultSynData.IsValid())
		{
			//ResultSynData->SerializeToJson(JsonWriter);
			GGI->DesignEditor->SaveFileList.AddUnique(ResultSynData->ID);
		}

		//@打扮家 XRLightmass 序列化LightMapID
		if (HoleWindow)
			LightMapID = HoleWindow->GetLightMapGUIDFromSMC();
		JsonWriter->WriteValue("LightMapID", LightMapID.ToString());
	}
	SERIALIZEREGISTERCLASS(JsonWriter, FArmyWindow)



}

void FArmyWindow::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyHardware::Deserialization(InJsonData);

	FVector TempPos, TempDirection;
	TempPos.InitFromString(InJsonData->GetStringField("pos"));
	TempDirection.InitFromString(InJsonData->GetStringField("direction"));
	SetPos(TempPos);
	SetDirection(TempDirection);
	SetLength(InJsonData->GetNumberField("length"));
	SetWidth(InJsonData->GetNumberField("Width"));
	SetHeight(InJsonData->GetNumberField("height"));
	SetRightOpen(InJsonData->GetBoolField("bRightOpen"));
	RectImagePanel->bIsFilled = InJsonData->GetBoolField("bRectIsFilled");
	bIfGenerateWindowStone = InJsonData->GetBoolField("bGenerateWindowStone");
	HeightToFloor = InJsonData->GetNumberField("heightToFloor");
	InJsonData->TryGetStringField("thumbnail", ThumbnailUrl);
	InJsonData->TryGetNumberField("SaleID", SaleID);

	//@郭子阳
	//兼容V1.7旧方案窗台石材质
	const TSharedPtr<FJsonObject>* JData = nullptr;
	if (InJsonData->TryGetObjectField("ContenItem", JData))
	{
		SaleID = (*JData)->GetIntegerField("id");
	}


	//加载窗台石材质
	if (SaleID != -1)
	{
		UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
		auto ContentItem = ResMgr->GetContentItemFromItemID(EResourceType::Texture, SaleID);
		if (ContentItem.IsValid())
		{
			SetContentItem(ContentItem);
		}
		else
		{
			//资源加载错误，不应该获取不到ContentItem
			SaleID = -1;
		}
	}

	
	
	
	//TSharedPtr<FContentItemSpace::FContentItem> TempContentItem = MakeShareable(new FContentItemSpace::FContentItem());
	//	//ContentItem->ResObjArr.Add(MakeShareable(new FContentItemSpace::FModelRes()));
	//	TempContentItem->Deserialization(InJsonData);
	//	if (TempContentItem->ResObjArr.Num() > 0 && !TempContentItem->ResObjArr[0]->FilePath.IsEmpty())
	//	{
	//		UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
	//		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(TempContentItem);
	//		if (mat)
	//		{
	//			SetContentItem(TempContentItem);
	//		//HoleWindow->SetWindowBoardMaterial(mat);
	//		}
	//	}
	


	//@打扮家 XRLightmass 反序列化LightMapID
	FString LightGUIDStr = "";
	InJsonData->TryGetStringField("LightMapID", LightGUIDStr);
	FGuid::Parse(LightGUIDStr, LightMapID);

	bool bCapture = CaptureDoubleLine(TempPos, E_LayoutModel);
}
void FArmyWindow::SetState(EObjectState InState)
{
	State = InState;

	switch (InState)
	{
	case OS_Normal:
		RectImagePanel->SetBaseColor(FLinearColor::White);
		WindowGlass1->SetBaseColor(FLinearColor::White);
		WindowGlass2->SetBaseColor(FLinearColor::White);
		CenterLine->SetBaseColor(FLinearColor::White);
		DeselectPoints();
		break;

	case OS_Hovered:
	case OS_Selected:
		RectImagePanel->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		WindowGlass1->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		WindowGlass2->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		CenterLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		break;

	case OS_Disable:
		RectImagePanel->SetBaseColor(FLinearColor::Gray);
		WindowGlass1->SetBaseColor(FLinearColor::Gray);
		WindowGlass2->SetBaseColor(FLinearColor::Gray);
		CenterLine->SetBaseColor(FLinearColor::Gray);
		break;

	default:
		break;
	}
}

void FArmyWindow::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if ((!FArmyObject::GetDrawModel(MODE_MODIFYADD) && InWallType == 1) ||//施工图不显示新增墙，并且该垭口在新增墙上时，不绘制
			(FArmyObject::GetDrawModel(MODE_DELETEWALLPOST) && !bGenerate3D))//拆除墙上带了窗戶，并且把门洞也拆除了，在拆除后的效果中不要显示门洞
		{
			return;
		}

		if (State == OS_Selected)
		{
			FArmyHardware::Draw(PDI, View);
		}
		if (FArmyObject::GetDrawModel(MODE_TOPVIEW) || FArmyObject::GetDrawModel(MODE_CEILING))
		{
			TopViewWindowFrame->Draw(PDI, View);
			TopViewWindowGlass1->Draw(PDI, View);
			TopViewWindowGlass2->Draw(PDI, View);
			if (GetType() == OT_FloorWindow)
				TopCenterLine->Draw(PDI, View);
		}
		else
		{
			RectImagePanel->Draw(PDI, View);
			WindowGlass1->Draw(PDI, View);
			WindowGlass2->Draw(PDI, View);
			if (GetType() == OT_FloorWindow)
				CenterLine->Draw(PDI, View);
		}
	}
}

bool FArmyWindow::IsSelected(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		bShowDashLine = true;
		return RectImagePanel->IsSelected(Pos, InViewportClient);
	}
	return false;
}

bool FArmyWindow::Hover(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		return RectImagePanel->IsSelected(Pos, InViewportClient);
	}
	return false;
}

void FArmyWindow::SetHeightToFloor(float InValue)
{
	if (HeightToFloor != InValue)
	{
		HeightToFloor = InValue;
		Update();
	}
}


TArray<FVector> FArmyWindow::GetBoundingBox()
{
	TArray<FVector> BoudingBox;
	FVector LeftTop = GetPos() - Length / 2 * HorizontalDirection + Width / 2 * Direction;
	FVector LeftBottom = GetPos() - Length / 2 * HorizontalDirection - Width / 2 * Direction;
	FVector RightBottom = GetPos() + Length / 2 * HorizontalDirection - Width / 2 * Direction;
	FVector RightTop = GetPos() + Length / 2 * HorizontalDirection + Width / 2 * Direction;
	BoudingBox.Push((LeftBottom));
	BoudingBox.Push((LeftTop));
	BoudingBox.Push((RightTop));
	BoudingBox.Push((RightBottom));
	return BoudingBox;
}

// 获得立面投影box
TArray<struct FLinesInfo> FArmyWindow::GetFacadeBox()
{
	/** 全部的框 */
	TArray<struct FLinesInfo> AllCircle;
	const int32 limit = 11;
	FVector base_z = FVector(0, 0, 1);

	/** 外层框 */
	FLinesInfo OutsideCircle;
	OutsideCircle.DrawType = 0;
	OutsideCircle.Color = FLinearColor(0.0f, 1.0f, 1.0f);
	FVector LeftBottomOutside = GetPos() - Length * 0.5f * HorizontalDirection - Width * 0.5f * Direction + HeightToFloor*base_z;
	FVector RightBottomOutside = GetPos() + Length * 0.5f * HorizontalDirection - Width * 0.5f * Direction + HeightToFloor*base_z;
	FVector LeftTopOutside = LeftBottomOutside + Height*base_z;
	FVector RightTopOutside = RightBottomOutside + Height*base_z;

	OutsideCircle.Vertices.Emplace(LeftBottomOutside);
	OutsideCircle.Vertices.Emplace(LeftTopOutside);
	OutsideCircle.Vertices.Emplace(RightTopOutside);
	OutsideCircle.Vertices.Emplace(RightBottomOutside);
	AllCircle.Emplace(OutsideCircle);

	/** 大于limit限制，生成内层框 */
	if (Length > limit)
	{
		/** 内层框 */
		FLinesInfo InsideCircle;
		InsideCircle.DrawType = 0;
		InsideCircle.Color = FLinearColor(0.0f, 1.0f, 1.0f);
		FVector LeftBottomInside = LeftBottomOutside + 5 * HorizontalDirection + 5 * base_z;
		FVector RightBottomInside = RightBottomOutside - 5 * HorizontalDirection + 5 * base_z;
		FVector LeftTopInside = LeftBottomInside + (Height - 10)*base_z;
		FVector RightTopInside = RightBottomInside + (Height - 10)*base_z;

		InsideCircle.Vertices.Emplace(LeftBottomInside);
		InsideCircle.Vertices.Emplace(LeftTopInside);
		InsideCircle.Vertices.Emplace(RightTopInside);
		InsideCircle.Vertices.Emplace(RightBottomInside);
		AllCircle.Emplace(InsideCircle);

	}
	return AllCircle;
}

TArray<FVector> FArmyWindow::GetClipingBox()
{
	TArray<FVector> BoudingBox;
	FVector TempHorizontalDirection = FArmyMath::GetLineDirection(StartPoint->GetPos(), EndPoint->GetPos());
	FVector VerticalDirection = TempHorizontalDirection.RotateAngleAxis(90, FVector(0, 0, 1));
	FVector LeftTop = GetPos() - Length / 2 * TempHorizontalDirection + (Width + SubTracteThicknessError) / 2 * VerticalDirection;
	FVector LeftBottom = GetPos() - Length / 2 * TempHorizontalDirection - (Width + SubTracteThicknessError) / 2 * VerticalDirection;
	FVector RightBottom = GetPos() + Length / 2 * TempHorizontalDirection - (Width + SubTracteThicknessError) / 2 * VerticalDirection;
	FVector RightTop = GetPos() + Length / 2 * TempHorizontalDirection + (Width + SubTracteThicknessError) / 2 * VerticalDirection;
	FVector offset = FVector(0, 0, HeightToFloor + Height / 2.0f);
	BoudingBox.Push(LeftBottom + offset);
	BoudingBox.Push(LeftTop + offset);
	BoudingBox.Push(RightTop + offset);
	BoudingBox.Push(RightBottom + offset);
	return BoudingBox;
}

const FBox FArmyWindow::GetBounds()
{
	FVector LeftTop = GetPos() - Length / 2 * HorizontalDirection + (Width) / 2 * Direction;
	FVector RightBottom = GetPos() + Length / 2 * HorizontalDirection - (Width) / 2 * Direction;
	return FBox(LeftTop, RightBottom);
}

/*获取户型模式下Draw的线段集合*/
void FArmyWindow::GetCadLineArray(TArray<TPair<FVector, FVector>>& OutLineList) const
{
	FArmyHardware::GetCadLineArray(OutLineList);

	OutLineList.Append({
		TPair<FVector,FVector>(this->WindowGlass1->GetStart(),this->WindowGlass1->GetEnd()),
		TPair<FVector,FVector>(this->WindowGlass2->GetStart(),this->WindowGlass2->GetEnd()),
	});
	if (GetType() == OT_FloorWindow)
		OutLineList.AddUnique(TPair<FVector, FVector>(this->CenterLine->GetStart(), this->CenterLine->GetEnd()));
}

void FArmyWindow::Destroy()
{
	if (HoleWindow && HoleWindow->IsValidLowLevel())
	{
		HoleWindow->Destroy();
		HoleWindow = NULL;
	}
}

void FArmyWindow::Update()
{
	FArmyHardware::Update();

	RectImagePanel->Pos = GetStartPos() + Width / 2 * Direction;
	RectImagePanel->Pos.Z = 1.f;
	RectImagePanel->XDirection = HorizontalDirection;
	RectImagePanel->YDirection = -Direction;
	RectImagePanel->Width = Length;
	RectImagePanel->Height = Width;

	FVector WindowPos = GetPos();
	WindowPos.Z = 1.f;
	WindowGlass1->SetStart(WindowPos - Length / 2 * HorizontalDirection + Width / 6 * Direction);
	WindowGlass1->SetEnd(WindowPos + Length / 2 * HorizontalDirection + Width / 6 * Direction);
	WindowGlass2->SetStart(WindowPos - Length / 2 * HorizontalDirection - Width / 6 * Direction);
	WindowGlass2->SetEnd(WindowPos + Length / 2 * HorizontalDirection - Width / 6 * Direction);
	CenterLine->SetStart((WindowGlass1->GetStart() + WindowGlass1->GetEnd()) / 2);
	CenterLine->SetEnd((WindowGlass2->GetStart() + WindowGlass2->GetEnd()) / 2);

	TopViewWindowFrame->Pos = RectImagePanel->Pos;
	TopViewWindowFrame->Pos.Z = 301.f;
	TopViewWindowFrame->XDirection = RectImagePanel->XDirection;
	TopViewWindowFrame->YDirection = RectImagePanel->YDirection;
	TopViewWindowFrame->Width = RectImagePanel->Width;
	TopViewWindowFrame->Height = RectImagePanel->Height;

	FVector TempPos = GetPos();
	TempPos.Z = 302.f;
	TopViewWindowGlass1->SetStart(TempPos - Length / 2 * HorizontalDirection + Width / 6 * Direction);
	TopViewWindowGlass1->SetEnd(TempPos + Length / 2 * HorizontalDirection + Width / 6 * Direction);
	TopViewWindowGlass2->SetStart(TempPos - Length / 2 * HorizontalDirection - Width / 6 * Direction);
	TopViewWindowGlass2->SetEnd(TempPos + Length / 2 * HorizontalDirection - Width / 6 * Direction);
	TopCenterLine->SetStart((TopViewWindowGlass1->GetStart() + TopViewWindowGlass1->GetEnd()) / 2);
	TopCenterLine->SetEnd((TopViewWindowGlass2->GetStart() + TopViewWindowGlass2->GetEnd()) / 2);
}

void FArmyWindow::SetWindowType(uint32 type)
{
	WindowType = type;
}

void FArmyWindow::GetAttachHoleWallInfo(TArray<struct FBSPWallInfo>& OutAttachWallInfo)
{
	TArray<TSharedPtr<FArmyWallLine>> attWallLines;
	FArmySceneData::Get()->GetHardwareRelateWalllines(this->AsShared(), attWallLines);

	// @欧石楠 如果在放置窗户后修改墙体厚度，当厚度超过一定范围，会出现窗户关联不上墙体的情况
	if (attWallLines.Num() == 0)
	{
		return;
	}

	FVector tempStart = FVector(GetStartPos().X, GetStartPos().Y, 0.0f);
	FVector tempEnd = FVector(GetEndPos().X, GetEndPos().Y, 0.0f);
	// 洞口左边面片信息
	FVector first = tempStart + Width / 2 * Direction + FVector(0, 0, HeightToFloor + Height);
	FVector second = tempStart + Width / 2 * Direction + FVector(0, 0, HeightToFloor);
	FVector third = tempStart - Width / 2 * Direction + FVector(0, 0, HeightToFloor);
	FVector fourth = tempStart - Width / 2 * Direction + FVector(0, 0, HeightToFloor + Height);
	float outRoomOffset = 0;
	if (attWallLines.Num() == 2)
	{
		outRoomOffset = FArmySceneData::Get()->FinishWallThick;
	}
	FVector InnerNormal = attWallLines[0]->GetNormal();
	FBSPWallInfo LeftOrignalWallInfo, LeftFinishWallInfo;
	TArray<FVector> LeftVerts = { first,second,third,fourth };

	first = tempStart + HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, Height - FArmySceneData::Get()->FinishWallThick) + (Width / 2 + FArmySceneData::Get()->FinishWallThick)*InnerNormal + FVector(0, 0, HeightToFloor);
	second = tempStart + HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, FArmySceneData::Get()->FinishWallThick) + (Width / 2 + FArmySceneData::Get()->FinishWallThick)*InnerNormal + FVector(0, 0, HeightToFloor);
	third = tempStart + HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, FArmySceneData::Get()->FinishWallThick) + (Width / 2 + outRoomOffset) * (-InnerNormal) + FVector(0, 0, HeightToFloor);
	fourth = tempStart + HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, Height - FArmySceneData::Get()->FinishWallThick) + (Width / 2 + outRoomOffset) * (-InnerNormal) + FVector(0, 0, HeightToFloor);
	TArray<FVector> LeftWallFinishVerts = { first,second,third,fourth };
	LeftOrignalWallInfo.PolyVertices.Push(LeftVerts);
	LeftFinishWallInfo.PolyVertices.Push(LeftWallFinishVerts);
	LeftOrignalWallInfo.Normal = LeftFinishWallInfo.Normal = -HorizontalDirection;
	LeftOrignalWallInfo.UniqueIdForRoomOrHardware = LeftFinishWallInfo.UniqueIdForRoomOrHardware = GetUniqueID().ToString();
	LeftOrignalWallInfo.UniqueId = LeftFinishWallInfo.UniqueId = GetUniqueID().ToString() + TEXT("LeftWall");
	LeftOrignalWallInfo.GenerateFromObjectType = LeftFinishWallInfo.GenerateFromObjectType = ObjectType;
	// 洞口右边面片信息
	first = tempEnd + Width / 2 * Direction + FVector(0, 0, HeightToFloor + Height);
	second = tempEnd + Width / 2 * Direction + FVector(0, 0, HeightToFloor);
	third = tempEnd - Width / 2 * Direction + FVector(0, 0, HeightToFloor);
	fourth = tempEnd - Width / 2 * Direction + FVector(0, 0, HeightToFloor + Height);
	TArray<FVector> RightVerts = { first,second,third,fourth };
	first = tempEnd - HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, Height - FArmySceneData::Get()->FinishWallThick) + (Width / 2 + FArmySceneData::Get()->FinishWallThick)*InnerNormal + FVector(0, 0, HeightToFloor);
	second = tempEnd - HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, FArmySceneData::Get()->FinishWallThick) + (Width / 2 + FArmySceneData::Get()->FinishWallThick)*InnerNormal + FVector(0, 0, HeightToFloor);
	third = tempEnd - HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, FArmySceneData::Get()->FinishWallThick) + (Width / 2 + outRoomOffset) * (-InnerNormal) + FVector(0, 0, HeightToFloor);
	fourth = tempEnd - HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, Height - FArmySceneData::Get()->FinishWallThick) + (Width / 2 + outRoomOffset) * (-InnerNormal) + FVector(0, 0, HeightToFloor);
	TArray<FVector> RighFinishVerts = { first,second,third,fourth };
	FBSPWallInfo RightOrignalWallInfo, RightFinishWallInfo;
	RightOrignalWallInfo.PolyVertices.Add(RightVerts);
	RightFinishWallInfo.PolyVertices.Add(RighFinishVerts);
	RightOrignalWallInfo.Normal = RightFinishWallInfo.Normal = HorizontalDirection;
	RightOrignalWallInfo.UniqueIdForRoomOrHardware = RightFinishWallInfo.UniqueIdForRoomOrHardware = GetUniqueID().ToString();
	RightOrignalWallInfo.UniqueId = RightFinishWallInfo.UniqueId = GetUniqueID().ToString() + TEXT("RightWall");
	RightOrignalWallInfo.GenerateFromObjectType = RightFinishWallInfo.GenerateFromObjectType = ObjectType;
	// 洞地信息
	first = tempStart + Width / 2 * Direction + FVector(0, 0, HeightToFloor);
	second = tempEnd + Width / 2 * Direction + FVector(0, 0, HeightToFloor);
	third = tempEnd - Width / 2 * Direction + FVector(0, 0, HeightToFloor);
	fourth = tempStart - Width / 2 * Direction + FVector(0, 0, HeightToFloor);
	TArray<FVector> BottomVerts;
	BottomVerts = { first,second,third,fourth };
	TArray<FVector> BottomFinishVerts;
	first = tempStart + HorizontalDirection * FArmySceneData::Get()->FinishWallThick + (Width / 2.0f + FArmySceneData::Get()->FinishWallThick)*InnerNormal + FVector(0, 0, FArmySceneData::Get()->FinishWallThick + HeightToFloor);
	second = tempEnd - HorizontalDirection * FArmySceneData::Get()->FinishWallThick + (Width / 2.0f + FArmySceneData::Get()->FinishWallThick) * InnerNormal + FVector(0, 0, FArmySceneData::Get()->FinishWallThick + HeightToFloor);
	third = tempEnd - HorizontalDirection * FArmySceneData::Get()->FinishWallThick - (Width / 2.0f + FArmySceneData::Get()->FinishWallThick) * InnerNormal + FVector(0, 0, FArmySceneData::Get()->FinishWallThick + HeightToFloor);
	fourth = tempStart + HorizontalDirection * FArmySceneData::Get()->FinishWallThick - (Width / 2.0f + FArmySceneData::Get()->FinishWallThick)*InnerNormal + FVector(0, 0, FArmySceneData::Get()->FinishWallThick + HeightToFloor);
	BottomFinishVerts = { first,second,third,fourth };
	FBSPWallInfo BottomOrignalWallInfo, BottomFinishWallInfo;
	BottomOrignalWallInfo.PolyVertices.Add(BottomVerts);
	BottomFinishWallInfo.PolyVertices.Push(BottomFinishVerts);
	BottomOrignalWallInfo.Normal = BottomFinishWallInfo.Normal = FVector(0, 0, 1);
	BottomOrignalWallInfo.UniqueIdForRoomOrHardware = BottomFinishWallInfo.UniqueIdForRoomOrHardware = GetUniqueID().ToString();
	BottomOrignalWallInfo.UniqueId = BottomFinishWallInfo.UniqueId = GetUniqueID().ToString() + TEXT("BottomWall");
	BottomOrignalWallInfo.GenerateFromObjectType = BottomFinishWallInfo.GenerateFromObjectType = ObjectType;
	// 洞顶信息
	TArray<FVector> TopVertices;
	for (const FVector& iter : BottomVerts)
	{
		TopVertices.Push(iter + FVector(0, 0, Height));
	}
	TArray<FVector> TopFinishVerts;
	for (const FVector& iter : BottomFinishVerts)
	{
		TopFinishVerts.Push(iter + FVector(0, 0, Height - 2 * FArmySceneData::Get()->FinishWallThick));
	}
	FBSPWallInfo TopOrignalWallInfo, TopFinishWallInfo;
	TopOrignalWallInfo.PolyVertices.Push(TopVertices);
	TopFinishWallInfo.PolyVertices.Push(TopFinishVerts);
	TopOrignalWallInfo.Normal = TopFinishWallInfo.Normal = FVector(0, 0, -1);
	TopOrignalWallInfo.UniqueIdForRoomOrHardware = TopFinishWallInfo.UniqueIdForRoomOrHardware = GetUniqueID().ToString();
	TopOrignalWallInfo.UniqueId = TopFinishWallInfo.UniqueId = GetUniqueID().ToString() + TEXT("TopWall");
	TopOrignalWallInfo.GenerateFromObjectType = TopFinishWallInfo.GenerateFromObjectType = ObjectType;

	OutAttachWallInfo.Push(LeftOrignalWallInfo);
	OutAttachWallInfo.Push(LeftFinishWallInfo);
	OutAttachWallInfo.Push(RightOrignalWallInfo);
	OutAttachWallInfo.Push(RightFinishWallInfo);
	/**@欧石楠 当没有窗台石的时候加上地面的面片信息*/
	if (!bIfGenerateWindowStone)
	{
		OutAttachWallInfo.Push(BottomOrignalWallInfo);
		OutAttachWallInfo.Push(BottomFinishWallInfo);
	}

	OutAttachWallInfo.Push(TopOrignalWallInfo);
	OutAttachWallInfo.Push(TopFinishWallInfo);

}

void FArmyWindow::SetIfGenerateWindowStone(bool bValue)
{
	bIfGenerateWindowStone = bValue;

	FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();

	/*if (FArmySceneData::Get()->GetObjectByGuid(E_HomeModel, GetUniqueID()).Pin().IsValid())
	{
		TArray<TWeakPtr<FArmyObject>> WindowList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_Window, WindowList);
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_FloorWindow, WindowList);
		for (auto It : WindowList)
		{
			TSharedPtr<FArmyWindow> Window = StaticCastSharedPtr<FArmyWindow>(It.Pin());
			if (Window.IsValid())
			{
				if (GetPos().Equals(Window->GetPos(), 0.01))
				{
					Window->SetIfGenerateWindowStone(bValue);
					Window = StaticCastSharedPtr<FArmyWindow>
						(FArmySceneData::Get()->GetObjectByGuid(E_LayoutModel, Window->GetUniqueID()).Pin());
					if (Window.IsValid())
						Window->SetIfGenerateWindowStone(bValue);
					FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();

					break;
				}
			}
		}
	}*/
}

void FArmyWindow::GenerateWindowSill(UWorld* World)
{
	/**@欧石楠 判断是否需要生成窗台石*/
	if (!bIfGenerateWindowStone)
	{
		return;
	}
	FVector vertical = HorizontalDirection.RotateAngleAxis(-90, FVector(0, 0, 1)).GetSafeNormal();
	FVector horizontal = HorizontalDirection.GetSafeNormal();
	TArray<FVector> WindowSillVertices;
	//窗台石宽度 郭子阳注
	float TempWidth = Width + 8;
	WindowSillVertices.Push(StartPoint->Pos - WindowBottomExtrudeLength * horizontal + vertical* (TempWidth) / 2 + FVector(0, 0, HeightToFloor));
	WindowSillVertices.Push(EndPoint->Pos + WindowBottomExtrudeLength * horizontal + vertical* (TempWidth) / 2 + FVector(0, 0, HeightToFloor));
	WindowSillVertices.Push(EndPoint->Pos + WindowBottomExtrudeLength * horizontal - vertical * (TempWidth) / 2 + FVector(0, 0, HeightToFloor));
	WindowSillVertices.Push(StartPoint->Pos - WindowBottomExtrudeLength * horizontal - vertical* (TempWidth) / 2 + FVector(0, 0, HeightToFloor));

	HoleWindow->AddWindowBoardPoints(WindowSillVertices, SillThickness);

}

void FArmyWindow::GenerateWindowGlass(UWorld* World)
{
	FVector vertical = HorizontalDirection.RotateAngleAxis(-90, FVector(0, 0, 1)).GetSafeNormal();
	TArray<FVector> GlassVertices;
	GlassVertices.Push(StartPoint->Pos + vertical *GlassWidth / 2 + FVector(0, 0, HeightToFloor));
	GlassVertices.Push(EndPoint->Pos + vertical * GlassWidth / 2 + FVector(0, 0, HeightToFloor));
	GlassVertices.Push(EndPoint->Pos - vertical * GlassWidth / 2 + FVector(0, 0, HeightToFloor));
	GlassVertices.Push(StartPoint->Pos - vertical * GlassWidth / 2 + FVector(0, 0, HeightToFloor));
	HoleWindow->AddWindowGlassPoints(GlassVertices, Height);
}



void FArmyWindow::GenerateWindowPillars(UWorld* World)
{
	GenerateOutPillars(World);
	FVector first, second, third, fourth;
	FVector vertical = HorizontalDirection.RotateAngleAxis(-90, FVector(0, 0, 1)).GetSafeNormal();
	if (WindowType == 0 || WindowType == 1)
	{
		TArray<FVector> MiddlePillar;
		first = Point->Pos - (Length - 2 * OutPillarThiness) / 2 * HorizontalDirection - vertical * MiddlePillarThiness / 2;
		second = first + vertical * MiddlePillarThiness;
		third = second + HorizontalDirection* (Length - 2 * OutPillarThiness);
		fourth = third - vertical * MiddlePillarThiness;
		MiddlePillar.Push(first + FVector(0, 0, HeightToFloor + Height / 3));
		MiddlePillar.Push(second + FVector(0, 0, HeightToFloor + Height / 3));
		MiddlePillar.Push(third + FVector(0, 0, HeightToFloor + Height / 3));
		MiddlePillar.Push(fourth + FVector(0, 0, HeightToFloor + Height / 3));
		HoleWindow->AddPillarPoints(MiddlePillar, MiddlePillarHeight);
		if (WindowType == 1)
		{
			first = Point->Pos - vertical* MiddlePillarRectLength / 2 - HorizontalDirection * MiddlePillarRectLength / 2;
			second = first + vertical * MiddlePillarRectLength;
			third = second + HorizontalDirection * MiddlePillarRectLength;
			fourth = third - vertical * MiddlePillarRectLength;
			TArray<FVector> MiddlePillarVertices;
			MiddlePillarVertices.Push(first + FVector(0, 0, HeightToFloor + Height / 3 + MiddlePillarHeight));
			MiddlePillarVertices.Push(second + FVector(0, 0, HeightToFloor + Height / 3 + MiddlePillarHeight));
			MiddlePillarVertices.Push(third + FVector(0, 0, HeightToFloor + Height / 3 + MiddlePillarHeight));
			MiddlePillarVertices.Push(fourth + FVector(0, 0, HeightToFloor + Height / 3 + MiddlePillarHeight));
			HoleWindow->AddPillarPoints(MiddlePillarVertices, Height * 2 / 3 - MiddlePillarHeight - BottomOrTopThickness);
		}
	}
	else if (WindowType == 2)
	{
		TArray<FVector> tempVertices;
		for (int i = 1; i < 3; i++)
		{
			tempVertices.Empty();
			first = StartPoint->Pos + i * Length / 3.0f* HorizontalDirection + vertical *  MiddlePillarRectLength / 2;
			second = first + HorizontalDirection * MiddlePillarRectLength;
			third = second - vertical * MiddlePillarRectLength;
			fourth = third - HorizontalDirection * MiddlePillarRectLength;
			tempVertices.Push(first + FVector(0, 0, HeightToFloor + BottomOrTopThickness));
			tempVertices.Push(second + FVector(0, 0, HeightToFloor + BottomOrTopThickness));
			tempVertices.Push(third + FVector(0, 0, HeightToFloor + BottomOrTopThickness));
			tempVertices.Push(fourth + FVector(0, 0, HeightToFloor + BottomOrTopThickness));
			HoleWindow->AddPillarPoints(tempVertices, Height - BottomOrTopThickness * 2);

		}
		first = Point->Pos - HorizontalDirection * (Length - 2 * OutPillarThiness) / 2 + vertical * OutPillarWidth / 2;
		second = first + HorizontalDirection* (Length - 2 * OutPillarThiness);
		third = second - vertical * OutPillarWidth;
		fourth = third - HorizontalDirection * (Length - 2 * OutPillarThiness);
		tempVertices.Empty();
		tempVertices.Push(first + FVector(0, 0, HeightToFloor + Height / 3));
		tempVertices.Push(second + FVector(0, 0, HeightToFloor + Height / 3));
		tempVertices.Push(third + FVector(0, 0, HeightToFloor + Height / 3));
		tempVertices.Push(fourth + FVector(0, 0, HeightToFloor + Height / 3));
		HoleWindow->AddPillarPoints(tempVertices, OutPillarThiness);

	}
	else if (WindowType == 3)
	{
		TArray<FVector> tempVertices;
		for (int i = 1; i < 4; i++)
		{
			tempVertices.Empty();
			float tempWidth = (i == 2) ? 6 : 3;
			first = StartPoint->Pos + i * Length / 4.0f* HorizontalDirection + vertical *  tempWidth / 2;
			second = first + HorizontalDirection * tempWidth;
			third = second - vertical * tempWidth;
			fourth = third - HorizontalDirection * tempWidth;

			tempVertices.Push(first + FVector(0, 0, HeightToFloor + BottomOrTopThickness));
			tempVertices.Push(second + FVector(0, 0, HeightToFloor + BottomOrTopThickness));
			tempVertices.Push(third + FVector(0, 0, HeightToFloor + BottomOrTopThickness));
			tempVertices.Push(fourth + FVector(0, 0, HeightToFloor + BottomOrTopThickness));

			float tempHeight = (i == 2) ? (Height - 2 * BottomOrTopThickness) : (2.0f / 3.0f * Height);
			HoleWindow->AddPillarPoints(tempVertices, tempHeight);

		}
		tempVertices.Empty();

		first = Point->Pos - HorizontalDirection * (Length - 2 * OutPillarThiness) / 2.0f + vertical *MiddleHorizontalWidth / 2;
		second = first + HorizontalDirection * (Length - 2 * OutPillarThiness);
		third = second - vertical * MiddleHorizontalWidth;
		fourth = third - HorizontalDirection *(Length - 2 * OutPillarThiness);
		tempVertices.Push(first + FVector(0, 0, HeightToFloor + SillThickness + 2.0f / 3.0* Height));
		tempVertices.Push(second + FVector(0, 0, HeightToFloor + SillThickness + 2.0f / 3.0* Height));
		tempVertices.Push(third + FVector(0, 0, HeightToFloor + SillThickness + 2.0f / 3.0* Height));
		tempVertices.Push(fourth + FVector(0, 0, HeightToFloor + SillThickness + 2.0f / 3.0* Height));
		HoleWindow->AddPillarPoints(tempVertices, 4.0f);

	}

}

void FArmyWindow::GenerateOutPillars(UWorld* World)
{

	FVector vertical = HorizontalDirection.RotateAngleAxis(-90, FVector(0, 0, 1)).GetSafeNormal();
	TArray<FVector> LeftPillar;
	LeftPillar.Push(StartPoint->Pos - vertical* OutPillarWidth / 2 + FVector(0, 0, HeightToFloor));
	LeftPillar.Push(StartPoint->Pos + vertical * OutPillarWidth / 2 + FVector(0, 0, HeightToFloor));
	LeftPillar.Push(StartPoint->Pos + vertical*OutPillarWidth / 2 + HorizontalDirection* OutPillarThiness + FVector(0, 0, HeightToFloor));
	LeftPillar.Push(StartPoint->Pos - vertical * OutPillarWidth / 2 + HorizontalDirection * OutPillarThiness + FVector(0, 0, HeightToFloor));
	HoleWindow->AddPillarPoints(LeftPillar, Height);
	TArray<FVector> RightPillar;
	RightPillar.Push(EndPoint->Pos - vertical * OutPillarWidth / 2 + FVector(0, 0, HeightToFloor));
	RightPillar.Push(EndPoint->Pos - HorizontalDirection * OutPillarThiness - vertical* OutPillarWidth / 2 + FVector(0, 0, HeightToFloor));
	RightPillar.Push(EndPoint->Pos - HorizontalDirection * OutPillarThiness + vertical * OutPillarWidth / 2 + FVector(0, 0, HeightToFloor));
	RightPillar.Push(EndPoint->Pos + vertical* OutPillarWidth / 2 + FVector(0, 0, HeightToFloor));
	HoleWindow->AddPillarPoints(RightPillar, Height);
	TArray<FVector> BottomPillar;
	FVector first = Point->Pos - (Length - 2 * OutPillarThiness) / 2 * HorizontalDirection - vertical * OutPillarWidth / 2;
	FVector second = first + vertical * OutPillarWidth;
	FVector third = second + HorizontalDirection * (Length - 2 * OutPillarThiness);
	FVector fourth = third - vertical * OutPillarWidth;
	/**@欧石楠 通过是否生成窗台石来控制下边窗户框的位置高度*/
	float TempSillThickness = (bIfGenerateWindowStone ? SillThickness : 0);
	BottomPillar.Push(first + FVector(0, 0, TempSillThickness + HeightToFloor));
	BottomPillar.Push(second + FVector(0, 0, TempSillThickness + HeightToFloor));
	BottomPillar.Push(third + FVector(0, 0, TempSillThickness + HeightToFloor));
	BottomPillar.Push(fourth + FVector(0, 0, TempSillThickness + HeightToFloor));
	HoleWindow->AddPillarPoints(BottomPillar, BottomOrTopThickness);
	TArray<FVector> TopPillar;

	TopPillar.Push(first + FVector(0, 0, HeightToFloor + Height - BottomOrTopThickness));
	TopPillar.Push(second + FVector(0, 0, HeightToFloor + Height - BottomOrTopThickness));
	TopPillar.Push(third + FVector(0, 0, HeightToFloor + Height - BottomOrTopThickness));
	TopPillar.Push(fourth + FVector(0, 0, HeightToFloor + Height - BottomOrTopThickness));
	HoleWindow->AddPillarPoints(TopPillar, BottomOrTopThickness);
}

//@郭子阳 获取窗台石长度

 float FArmyWindow::GetStoneLenth()
 { 
	 return (StartPoint->Pos - EndPoint->Pos).Size() + WindowBottomExtrudeLength * 2;
 }

 //@郭子阳 获取窗台石面积

  float FArmyWindow::GetStoneArea() 
  {
	  float TempWidth = Width + 8;
	  return  GetStoneLenth()*TempWidth;
 
  }
void FArmyWindow::SetContentItem(TSharedPtr<FContentItemSpace::FContentItem> Goods)
{
	
	ContentItem = Goods;
	bDefaultMaterial =! Goods.IsValid();
	if (Goods.IsValid())
	{
		SaleID = Goods->ID;

		UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
		ResMgr->AddSyncItem(Goods);
		//@郭子阳
		//设置原始户型\拆改后中的窗口的SaleID
		auto HomeObject=FArmySceneData::Get()->GetObjectByGuid(EModelType::E_HomeModel, GetUniqueID());
		auto HomeWindow = StaticCastSharedPtr<FArmyWindow>(HomeObject.Pin());
		if (HomeWindow.IsValid())
		{
			HomeWindow->SaleID = SaleID;
		}
		auto LayoutObject = FArmySceneData::Get()->GetObjectByGuid(EModelType::E_LayoutModel, GetUniqueID());
		auto LayoutWindow = StaticCastSharedPtr<FArmyWindow>(LayoutObject.Pin());
		if (LayoutWindow.IsValid())
		{
			LayoutWindow->SaleID = SaleID;
		}

		ConstructionPatameters P;
		P.SetNormalGoodsInfo(Goods->ID);
		XRConstructionManager::Get()->TryToFindConstructionData(GetUniqueID(), P, nullptr);

//		UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
		WindowStoneMat= ResMgr->CreateContentItemMaterial(Goods);
		if (HoleWindow &&HoleWindow->IsValidLowLevel())
		{
			HoleWindow->WindowStoneName = Goods->Name;
			HoleWindow->SetSynID(WindowStoneMat->GetSynID());
			HoleWindow->SetWindowBoardMaterial(WindowStoneMat);
			SetThumbnailUrl(ContentItem->ThumbnailURL);
		}
	}
	else
	{
		SaleID = -1;
		bDefaultMaterial = true;
		WindowStoneMat = nullptr;
		if (HoleWindow &&HoleWindow->IsValidLowLevel())
		{
			HoleWindow->SetWindowBoardMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWindowBaseMaterial());
			//Window->HoleWindow->CurrentWindowType = NORMAL_WINDOW;
			HoleWindow->WindowStoneName = TEXT("");
			HoleWindow->SetSynID(-1);
			SetThumbnailUrl("");
		}
	}
}
void FArmyWindow::ConstructionData(TArray<FArmyGoods>& ArtificialData)
{
	//收集窗户面上的数据
	TArray<TWeakPtr<FArmyObject>> AllRoomArea;
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, AllRoomArea);

	for (auto& AreaObj : AllRoomArea)
	{
		auto Area = StaticCastSharedPtr<FArmyRoomSpaceArea>(AreaObj.Pin());
		if (Area->AttachRoomID.Contains(GetUniqueID().ToString()))
		{
			//导出窗户墙面的施工项
			Area->GetConstructionAreas(ArtificialData);
			for (auto& iter0 : Area->GetEditAreas())
			{
				iter0->GetConstructionAreas(ArtificialData);
			}
			
		}
	}

	//收集窗台石施工项
	if (ContentItem.IsValid()) 
	{
	FArmyGoods goods;
	goods.GoodsId = SaleID;
	goods.Type = 1;
	//goods.PaveID = ConvertStyleToPavingID(MatStyle);

	// 施工项
	ConstructionPatameters P;
	P.SetNormalGoodsInfo(goods.GoodsId, GetRoomSpaceID());
	TSharedPtr<FArmyConstructionItemInterface> tempConstructoinData = XRConstructionManager::Get()->GetSavedCheckedData(GetUniqueID(), P);
//	TSharedPtr<FArmyConstructionItemInterface> tempConstructoinData = FArmySceneData::Get()->GetConstructionItemDataByExtrusionUniqueCode(ExturesionActor->UniqueCodeExtrusion);
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

	int32 GoodsIndex;
	if (!ArtificialData.Find(goods, GoodsIndex))
	{
		GoodsIndex = ArtificialData.Add(goods);
	}
	FArmyGoods& Goods = ArtificialData[GoodsIndex];
	//合并施工项数量
	for (int32 i = 0; i < ArtificialData[GoodsIndex].QuotaData.Num(); i++)
	{
		ArtificialData[GoodsIndex].QuotaData[i].Dosage += 1;
	}
	//int tempC = ExturesionActor->Circumference / 100.0f * 100;
	
	Goods.Param.C += GetStoneLenth() / 100.0f;
	Goods.Param.L += GetStoneLenth() / 100.0f;
	Goods.Param.S += GetStoneArea() / 10000.0f;
	Goods.Param.D = Goods.Param.S;
	//Goods.Param.H = M_ExtrusionHeight;
	}

}

void FArmyWindow::Generate(UWorld* InWorld)
{
	if (!bGenerate3D)
	{
		return;
	}

	AttachModelName = ObjectType == OT_Window ? TEXT("Window") + GetUniqueID().ToString() : TEXT("FloorWindow") + GetUniqueID().ToString();

	if (!HoleWindow)
	{
		FActorSpawnParameters SpawnParam;
		SpawnParam.Name = FName(*("NOLIST-WINDOW" + GetUniqueID().ToString()));

		HoleWindow = InWorld->SpawnActor<AXRWindowActor>(AXRWindowActor::StaticClass(), SpawnParam);
		//@马云龙 赋值AttachSurface
		HoleWindow->AttachSurface = StaticCastSharedRef<FArmyHardware>(this->AsShared());
		HoleWindow->SetWindowGlassMaterial(FArmyEngineModule::Get().GetEngineResource()->GetGlassMaterial());
		if (bDefaultMaterial)
		{
			HoleWindow->SetWindowBoardMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWindowBaseMaterial());
		}
		else
		{
			HoleWindow->SetWindowBoardMaterial(WindowStoneMat);
		}
		HoleWindow->SetWindowPillarMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWindowPillarMaterial());
	}
	HoleWindow->CurrentWindowType = NORMAL_WINDOW;
	HoleWindow->WinowLength = Length;
	HoleWindow->WindowHeight = Height;
	HoleWindow->WindowOffsetGroundHeight = HeightToFloor;

	HoleWindow->ResetMeshTriangles();
	GenerateWindowGlass(InWorld);
	GenerateWindowSill(InWorld);
	GenerateWindowPillars(InWorld);
	HoleWindow->UpdataAlloVertexIndexBuffer();
	SetRelevanceActor(HoleWindow);
	HoleWindow->SetActorLabel(AttachModelName);
	HoleWindow->Tags.Add(XRActorTag::Immovable);
	HoleWindow->Tags.Add(XRActorTag::Window);
	HoleWindow->Tags.Add(XRActorTag::CanNotDelete);
}


TSharedPtr<FArmyRoom> FArmyWindow::GetRoom()
{
	if (FirstRelatedRoom.IsValid() && FirstRelatedRoom->GetType() != OT_OutRoom
		&& SecondRelatedRoom.IsValid() && SecondRelatedRoom->GetType() != OT_OutRoom)
	{
		//处于墙之间的门没有空间
		return nullptr;
	}

	return FirstRelatedRoom.IsValid() && FirstRelatedRoom->GetType() != OT_OutRoom ? FirstRelatedRoom : SecondRelatedRoom;
}

void FArmyWindow::OnRoomSpaceIDChanged(int32 NewSpaceID)
{
	ConstructionPatameters P;
	P.SetNormalGoodsInfo(SaleID, GetRoomSpaceID());
	XRConstructionManager::Get()->TryToFindConstructionData(GetUniqueID(), P, nullptr);
}
