#include "ArmySlidingDoor.h"
#include "ArmyEngineModule.h"
#include "Classes/Engine/StaticMeshActor.h"
#include "ArmyRect.h"
#include "ArmyStyle.h"
#include "ArmyShapeBoardActor.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "ArmyPass.h"
#include "ArmyGameInstance.h"
#include "ArmyResourceModule.h"
#include "ArmyActorConstant.h"
#include "ArmyMath.h"
#include  "ArmyConstructionManager.h"
#define SLIDING_DOOR_FRAME_LENGTH   94
#define SLIDING_DOOR_FRAME_HEIGHT   110
#define SLIDING_DOOR_FRAME_WIDTH    8
#define SLIDING_DOOR_LENGTH         44
#define SLIDING_DOOR_HEIGHT         105
#define SLIDING_DOOR_L_WIDTH        6
#define SLIDING_DOOR_R_WIDTH        1

FArmySlidingDoor::FArmySlidingDoor()
{
	ObjectType = OT_SlidingDoor;
	SetPropertyFlag(FLAG_COLLISION, true);

	StartDoorPlank = MakeShareable(new FArmyRect);
	EndDoorPlank = MakeShareable(new FArmyRect);
	LeftLine = MakeShareable(new FArmyLine);
	RightLine = MakeShareable(new FArmyLine);
	SlidingDoorActor = nullptr;

	//RectImagePanel = MakeShareable(new FArmyRect());
	UMaterial* SlidingMaterial = FArmyEngineModule::Get().GetEngineResource()->GetPassColorMesh();
	MI_RectModifyWall = UMaterialInstanceDynamic::Create(SlidingMaterial, nullptr);
	MI_RectModifyWall->AddToRoot();
	MI_RectModifyWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor::Red);

	RectImagePanel->SetBaseColor(GVC->GetBackgroundColor());
	RectImagePanel->LineThickness = WALLLINEWIDTH + 0.1f;
	RectImagePanel->SetCalcUVs(true);
	RectImagePanel->XUVRepeatDist = 80.f;
	RectImagePanel->YUVRepeatDist = 30.f;
	RectImagePanel->MaterialRenderProxy = MI_RectModifyWall->GetRenderProxy(false);
	RectImagePanel->bIsFilled = false;

	BrokenWallLeftLine = MakeShareable(new FArmyLine());
	BrokenWallRightLine = MakeShareable(new FArmyLine());

	TopViewRectImagePanel = MakeShareable(new FArmyRect());
	TopViewStartDoorPlank = MakeShareable(new FArmyRect());
	TopViewEndDoorPlank = MakeShareable(new FArmyRect());
	TopViewLeftLine = MakeShareable(new FArmyLine());
	TopViewRightLine = MakeShareable(new FArmyLine());

	TopViewRectImagePanel->SetBaseColor(FLinearColor::Black);
	TopViewStartDoorPlank->SetBaseColor(FLinearColor::Black);
	TopViewEndDoorPlank->SetBaseColor(FLinearColor::Black);
	TopViewLeftLine->SetBaseColor(FLinearColor::Black);
	TopViewRightLine->SetBaseColor(FLinearColor::Black);


	UMaterialInstanceDynamic* MID_BaseColor = FArmyEngineModule::Get().GetEngineResource()->GetUnStageBaseColorMaterial();
	MID_BaseColor->AddToRoot();
	MID_BaseColor->SetVectorParameterValue("BaseColor", FLinearColor(FColor(0xFFE6E6E6)));
	TopViewRectImagePanel->MaterialRenderProxy = MID_BaseColor->GetRenderProxy(false);
	TopViewRectImagePanel->bIsFilled = true;

	Height = 210;
	Length = 140;
	Width = 12;
	SetName(TEXT("推拉门")/* + GetUniqueID().ToString()*/);

	ModifyWallRect = MakeShareable(new FArmyRect(RectImagePanel.Get()));
	ModifyWallRect->SetCalcUVs(true);
	ModifyWallRect->LineThickness = WALLLINEWIDTH + 0.1F;
	UMaterial* ModifyMaterial = FArmyEngineModule::Get().GetEngineResource()->GetModifyWallMaterial();
	MI_ModifyWall = UMaterialInstanceDynamic::Create(ModifyMaterial, nullptr);
	MI_ModifyWall->AddToRoot();
	MI_ModifyWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor(FColor(0xFFE305E5)));
	ModifyWallRect->MaterialRenderProxy = MI_ModifyWall->GetRenderProxy(false);
	ModifyWallRect->bIsFilled = true;
	ModifyWallRect->SetBaseColor(FLinearColor(FColor(0xFFE305E5)));


	DirectionPoint->SetState(FArmyPrimitive::OPS_Disable);

	Update();
	ConstructionItemData = MakeShareable(new FArmyConstructionItemInterface);
}

FArmySlidingDoor::FArmySlidingDoor(FArmySlidingDoor* Copy)
{
	bBoundingBox = Copy->bBoundingBox;
	State = Copy->GetState();
	PropertyFlag = Copy->PropertyFlag;
	StartDoorPlank = MakeShareable(new FArmyRect(Copy->StartDoorPlank.Get()));
	EndDoorPlank = MakeShareable(new FArmyRect(Copy->EndDoorPlank.Get()));
	LeftLine = MakeShareable(new FArmyLine(Copy->LeftLine.Get()));
	RightLine = MakeShareable(new FArmyLine(Copy->RightLine.Get()));
	AttachModelName = Copy->AttachModelName;

	DirectionPoint = MakeShareable(new FArmyEditPoint(Copy->DirectionPoint.Get()));

	RectImagePanel = MakeShareable(new FArmyRect(Copy->RectImagePanel.Get()));
	RectImagePanel->SetBaseColor(GVC->GetBackgroundColor());
	RectImagePanel->LineThickness = WALLLINEWIDTH + 0.1f;

	TopViewRectImagePanel = MakeShareable(new FArmyRect(Copy->TopViewRectImagePanel.Get()));
	TopViewStartDoorPlank = MakeShareable(new FArmyRect(Copy->TopViewStartDoorPlank.Get()));
	TopViewEndDoorPlank = MakeShareable(new FArmyRect(Copy->TopViewEndDoorPlank.Get()));
	TopViewLeftLine = MakeShareable(new FArmyLine(Copy->TopViewLeftLine.Get()));
	TopViewRightLine = MakeShareable(new FArmyLine(Copy->TopViewRightLine.Get()));
	TopViewRectImagePanel->LineThickness = 1.1f;

	Point->SetPos(Copy->GetPos());
	Direction = Copy->Direction;
	Length = Copy->Length;
	Width = Copy->Width;
	Height = Copy->Height;
	ObjectType = Copy->ObjectType;
	SetName(Copy->GetName());
	SaleID = Copy->SaleID;
	SlidingDoorActor = Copy->SlidingDoorActor;
	LinkFirstLine = Copy->LinkFirstLine;
	LinkSecondLine = Copy->LinkSecondLine;
	FirstRelatedRoom = Copy->FirstRelatedRoom;
	SecondRelatedRoom = Copy->SecondRelatedRoom;

	ModifyWallRect = MakeShareable(new FArmyRect(RectImagePanel.Get()));
	ModifyWallRect->SetCalcUVs(true);
	ModifyWallRect->LineThickness = WALLLINEWIDTH + 0.1F;
	UMaterial* ModifyMaterial = FArmyEngineModule::Get().GetEngineResource()->GetModifyWallMaterial();
	MI_ModifyWall = UMaterialInstanceDynamic::Create(ModifyMaterial, nullptr);
	MI_ModifyWall->AddToRoot();
	MI_ModifyWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor(FColor(0xFFE305E5)));
	ModifyWallRect->MaterialRenderProxy = MI_ModifyWall->GetRenderProxy(false);
	ModifyWallRect->bIsFilled = true;
	ModifyWallRect->SetBaseColor(FLinearColor(FColor(0xFFE305E5)));

	//只有点击到墙体上的门才有
	BrokenWallLeftLine = MakeShareable(new FArmyLine());
	BrokenWallRightLine = MakeShareable(new FArmyLine());

	bIsCopy = true;

	Update();
	ConstructionItemData = MakeShareable(new FArmyConstructionItemInterface);
}

FArmySlidingDoor::~FArmySlidingDoor()
{
}
void FArmySlidingDoor::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyHardware::SerializeToJson(JsonWriter);

	JsonWriter->WriteValue("pos", GetPos().ToString());
	JsonWriter->WriteValue("direction", GetDirection().ToString());
	JsonWriter->WriteValue("length", GetLength());
	JsonWriter->WriteValue("width", GetWidth());
	JsonWriter->WriteValue("height", GetHeight());
	JsonWriter->WriteValue("bRightOpen", IsRightOpen());
	JsonWriter->WriteValue("SaleID", SaleID);
	JsonWriter->WriteValue("thumbnailUrl", ThumbnailUrl);
	JsonWriter->WriteValue("bHasConnectPass", bHasConnectPass);
	JsonWriter->WriteValue("openDirectionID", OpenDirectionID);
	JsonWriter->WriteValue("MatType", GetMatType());
	/**@欧石楠 存储施工项*/
	ConstructionItemData->SerializeToJson(JsonWriter);

	if (SlidingDoorActor)
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


	SERIALIZEREGISTERCLASS(JsonWriter, FArmySlidingDoor)
}

void FArmySlidingDoor::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	bDeserialization = true;
	FArmyHardware::Deserialization(InJsonData);

	FVector Pos, Direction;
	Pos.InitFromString(InJsonData->GetStringField("pos"));
	Direction.InitFromString(InJsonData->GetStringField("direction"));
	SetPos(Pos);
	SetDirection(Direction);
	SetLength(InJsonData->GetNumberField("length"));
	SetWidth(InJsonData->GetNumberField("Width"));
	SetHeight(InJsonData->GetNumberField("height"));
	SetRightOpen(InJsonData->GetBoolField("bRightOpen"));
	InJsonData->TryGetNumberField("SaleID", SaleID);
	InJsonData->TryGetStringField("thumbnailUrl", ThumbnailUrl);
	bHasConnectPass = InJsonData->GetBoolField("bHasConnectPass");
	InJsonData->TryGetNumberField("openDirectionID", OpenDirectionID);
	SetMatType(InJsonData->GetStringField(TEXT("MatType")));

	ModifyWallRect = MakeShareable(new FArmyRect(RectImagePanel.Get()));
	ModifyWallRect->SetCalcUVs(true);
	ModifyWallRect->LineThickness = WALLLINEWIDTH + 0.1F;
	UMaterial* ModifyMaterial = FArmyEngineModule::Get().GetEngineResource()->GetModifyWallMaterial();
	MI_ModifyWall = UMaterialInstanceDynamic::Create(ModifyMaterial, nullptr);
	MI_ModifyWall->AddToRoot();
	MI_ModifyWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor(FColor(0xFFE305E5)));
	ModifyWallRect->MaterialRenderProxy = MI_ModifyWall->GetRenderProxy(false);
	ModifyWallRect->bIsFilled = true;
	ModifyWallRect->SetBaseColor(FLinearColor(FColor(0xFFE305E5)));

	/**@欧石楠 读取施工项*/
	ConstructionItemData->Deserialization(InJsonData);

	bool bCapture = CaptureDoubleLine(Pos, E_LayoutModel);
	UpdateDoorHole(E_ModifyModel);
	bDeserialization = false;
}

void FArmySlidingDoor::SetState(EObjectState InState)
{
	State = InState;

	switch (InState)
	{
	case OS_Normal:
		StartDoorPlank->SetBaseColor(FLinearColor::White);
		EndDoorPlank->SetBaseColor(FLinearColor::White);
		LeftLine->SetBaseColor(FLinearColor::White);
		RightLine->SetBaseColor(FLinearColor::White);
		DeselectPoints();
		if (BrokenWallLeftLine.IsValid())
		{
			BrokenWallLeftLine->SetBaseColor(FLinearColor::White);
			BrokenWallRightLine->SetBaseColor(FLinearColor::White);
		}
		break;

	case OS_Hovered:
	case OS_Selected:
	case OS_Disable:
		StartDoorPlank->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		EndDoorPlank->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		LeftLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		RightLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		if (BrokenWallLeftLine.IsValid())
		{
			BrokenWallLeftLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
			BrokenWallRightLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		}
		break;

	default:
		break;
	}
}

void FArmySlidingDoor::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (FArmyObject::GetDrawModel(MODE_TOPVIEW) || FArmyObject::GetDrawModel(MODE_CEILING))
		{
			TopViewRectImagePanel->Draw(PDI, View);
			TopViewStartDoorPlank->Draw(PDI, View);
			TopViewEndDoorPlank->Draw(PDI, View);
			TopViewLeftLine->Draw(PDI, View);
			TopViewRightLine->Draw(PDI, View);
		}
		else if (FArmyObject::GetDrawModel(MODE_MODIFYDELETE) && FArmyObject::GetDrawModel(MODE_CONSTRUCTION) && !bHasConnectPass && InWallType == 0)
		{
			ModifyWallRect->Draw(PDI, View);
		}
		else
		{
			if (!(FArmyObject::GetDrawModel(MODE_DOORPASS) && bHasConnectPass))
			{
				//RectImagePanel->bIsFilled = true;
				RectImagePanel->Draw(PDI, View);
				/**@欧石楠 控制门直接放置显示拆改*/
				if (!FArmyObject::GetDrawModel(MODE_CONSTRUCTION) && InWallType == 0 && !GetPropertyFlag(FArmyObject::FLAG_LAYOUT) && !bHasConnectPass && bIsCopy)
				{
					ModifyWallRect->Draw(PDI, View);
				}
				if (!(FArmyObject::GetDrawModel(MODE_MODIFYDELETE) && FArmyObject::GetDrawModel(MODE_CONSTRUCTION) && InWallType == 1))
				{
					FArmyHardware::Draw(PDI, View);
				}
			}
			if (!FArmyObject::GetDrawModel(MODE_DOORPASS))
			{
				StartDoorPlank->Draw(PDI, View);
				EndDoorPlank->Draw(PDI, View);
				LeftLine->Draw(PDI, View);
				RightLine->Draw(PDI, View);
			}
		}
	}
}

bool FArmySlidingDoor::IsSelected(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	bShowDashLine = true;

	return StartDoorPlank->IsSelected(Pos, InViewportClient) || EndDoorPlank->IsSelected(Pos, InViewportClient);
}

bool FArmySlidingDoor::Hover(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	return IsSelected(Pos, InViewportClient);
}

const FBox FArmySlidingDoor::GetBounds()
{
	FVector LeftTop = GetPos() - Length / 2 * HorizontalDirection + (Width) / 2 * Direction;
	FVector RightBottom = GetPos() + Length / 2 * HorizontalDirection - (Width) / 2 * Direction;
	TArray<FVector> boundingBox;
	boundingBox.Push(LeftTop);
	boundingBox.Push(RightBottom);
	return FBox(boundingBox);
}

void FArmySlidingDoor::Destroy()
{
	if (SlidingDoorActor && SlidingDoorActor->IsValidLowLevel())
	{
		SlidingDoorActor->Destroy();
		SlidingDoorActor = nullptr;
	}
}

void FArmySlidingDoor::Update()
{
	FArmyHardware::Update();

	FVector CurrentStart = StartPoint->Pos;
	FVector CurrentEnd = EndPoint->Pos;
	CurrentStart.Z = 2;
	CurrentEnd.Z = 2;

	RectImagePanel->Pos = CurrentStart + Width / 2 * Direction;
	RectImagePanel->Pos.Z = 1.f;//为了让门挡住墙体线
	RectImagePanel->XDirection = HorizontalDirection;
	RectImagePanel->YDirection = -Direction;
	RectImagePanel->Width = Length;
	RectImagePanel->Height = Width;

	ModifyWallRect->Pos = CurrentStart + Width / 2 * Direction;
	ModifyWallRect->Pos.Z = 1.f;//为了让门挡住墙体线
	ModifyWallRect->XDirection = HorizontalDirection;
	ModifyWallRect->YDirection = -Direction;
	ModifyWallRect->Width = Length;
	ModifyWallRect->Height = Width;

	StartDoorPlank->Pos = GetPos() - HorizontalDirection * Length / 2.f + Direction * 4.f;
	StartDoorPlank->Pos.Z = 1.2f;//@ 推拉门线条要再材质之上显示
	StartDoorPlank->XDirection = HorizontalDirection;
	StartDoorPlank->YDirection = -Direction;
	StartDoorPlank->Width = Length / 2.f + 4.f;
	StartDoorPlank->Height = 4.f;

	LeftLine->SetStart(GetPos() - HorizontalDirection*Length / 2.f + Direction * Width / 2);
	LeftLine->SetEnd(GetPos() - HorizontalDirection*Length / 2.f - Direction * Width / 2);
	RightLine->SetStart(GetPos() + HorizontalDirection *Length / 2.f + Direction * Width / 2);
	RightLine->SetEnd(GetPos() + HorizontalDirection *Length / 2.f - Direction * Width / 2);

	EndDoorPlank->Pos = GetPos() - HorizontalDirection * 4.f;
	EndDoorPlank->Pos.Z = 1.2f;//@ 推拉门线条要再材质之上显示
	EndDoorPlank->XDirection = HorizontalDirection;
	EndDoorPlank->YDirection = -Direction;
	EndDoorPlank->Width = Length / 2.f + 4.f;
	EndDoorPlank->Height = 4.f;

	TopViewRectImagePanel->Pos = RectImagePanel->Pos;
	TopViewRectImagePanel->Pos.Z = 301.f;
	TopViewRectImagePanel->XDirection = RectImagePanel->XDirection;
	TopViewRectImagePanel->YDirection = RectImagePanel->YDirection;
	TopViewRectImagePanel->Width = RectImagePanel->Width;
	TopViewRectImagePanel->Height = RectImagePanel->Height;

	TopViewStartDoorPlank->Pos = StartDoorPlank->Pos;
	TopViewStartDoorPlank->Pos.Z = 302.f;
	TopViewStartDoorPlank->XDirection = StartDoorPlank->XDirection;
	TopViewStartDoorPlank->YDirection = StartDoorPlank->YDirection;
	TopViewStartDoorPlank->Width = StartDoorPlank->Width;
	TopViewStartDoorPlank->Height = StartDoorPlank->Height;

	FVector TempPos = GetPos();
	TempPos.Z = 302.f;

	TopViewLeftLine->SetStart(TempPos - HorizontalDirection*Length / 2.f + Direction * Width / 2);
	TopViewLeftLine->SetEnd(TempPos - HorizontalDirection*Length / 2.f - Direction * Width / 2);
	TopViewRightLine->SetStart(TempPos + HorizontalDirection *Length / 2.f + Direction * Width / 2);
	TopViewRightLine->SetEnd(TempPos + HorizontalDirection *Length / 2.f - Direction * Width / 2);

	TopViewEndDoorPlank->Pos = EndDoorPlank->Pos;
	TopViewEndDoorPlank->Pos.Z = 302.f;
	TopViewEndDoorPlank->XDirection = EndDoorPlank->XDirection;
	TopViewEndDoorPlank->YDirection = EndDoorPlank->YDirection;
	TopViewEndDoorPlank->Width = EndDoorPlank->Width;
	TopViewEndDoorPlank->Height = EndDoorPlank->Height;
}

void FArmySlidingDoor::GetCadLineArray(TArray<TPair<FVector, FVector>>& OutLineList) const
{
	//FArmyHardware::GetCadLineArray(OutLineList);

	auto GetLineList = [&](TArray<TPair<FVector, FVector>>& OutLineList, TSharedPtr<class FArmyRect> Rect) {
		if (Rect.IsValid())
		{
			//计算Rect各边线段
			const int32& VerticesNum = Rect->GetVertices().Num();
			for (int32 Index = 0; Index < VerticesNum; ++Index)
				OutLineList.AddUnique(TPair<FVector, FVector>(Rect->GetVertices()[Index%VerticesNum], Rect->GetVertices()[(Index + 1) % VerticesNum]));
		}
	};
	GetLineList(OutLineList, StartDoorPlank);
	GetLineList(OutLineList, EndDoorPlank);
	// 	OutLineList.Emplace(TPair<FVector, FVector>(LeftLine->GetStart(), LeftLine->GetEnd()));
	// 	OutLineList.Emplace(TPair<FVector, FVector>(RightLine->GetStart(), RightLine->GetEnd()));

}

void FArmySlidingDoor::ReplaceGoods(FContentItemPtr InContentItem, UWorld* InWorld)
{
	FArmyResourceModule::Get().GetResourceManager()->AddSyncItem(InContentItem);

	if (InContentItem->ID != SaleID)
	{
		SaleID = InContentItem->ID;
		ThumbnailUrl = InContentItem->ThumbnailURL;
		Generate(InWorld);

		// 更新拆改模式门的数据
		TWeakPtr<FArmyObject> ModifyDoor = FArmySceneData::Get()->GetObjectByGuid(E_ModifyModel, GetUniqueID());
		if (ModifyDoor.IsValid())
		{
			TSharedPtr<FArmySlidingDoor> ModifySlidingDoor = StaticCastSharedPtr<FArmySlidingDoor>(ModifyDoor.Pin());
			if (ModifySlidingDoor.IsValid())
			{
				ModifySlidingDoor->SaleID = InContentItem->ID;
				ModifySlidingDoor->ThumbnailUrl = InContentItem->ThumbnailURL;
			}
		}
	}
}

bool FArmySlidingDoor::IsGenerative() const
{
	return (SaleID != -1);
}

TArray<struct FLinesInfo> FArmySlidingDoor::GetFacadeBox()
{
	/** 全部的框 */
	TArray<struct FLinesInfo> AllCircle;
	const int32 limit = 30;
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

	/** 外层框 */
	FLinesInfo OutsideCircle;
	OutsideCircle.DrawType = 1;
	OutsideCircle.Color = FLinearColor(0.0f, 1.0f, 1.0f);
	FVector LeftBottomOutside = LeftBottomInside - 6 * HorizontalDirection;
	FVector RightBottomOutside = RightBottomInside + 6 * HorizontalDirection;
	FVector LeftTopOutside = LeftBottomOutside + (Height + 6)*base_z;
	FVector RightTopOutside = RightBottomOutside + (Height + 6)*base_z;

	OutsideCircle.Vertices.Emplace(LeftBottomOutside);
	OutsideCircle.Vertices.Emplace(LeftTopOutside);
	OutsideCircle.Vertices.Emplace(RightTopOutside);
	OutsideCircle.Vertices.Emplace(RightBottomOutside);
	AllCircle.Emplace(OutsideCircle);

	if (Length > limit)
	{
		/** 内部框二维平面长度 */
		float break_length = (Length - 30)*0.5f;

		FLinesInfo BreakFirstCircle;
		BreakFirstCircle.DrawType = 0;
		BreakFirstCircle.Color = FLinearColor(0.0f, 1.0f, 1.0f);
		FVector LeftBottomFirstBreak = LeftBottomInside + 10 * HorizontalDirection + 10 * base_z;
		FVector RightBottomFirstBreak = LeftBottomFirstBreak + break_length * HorizontalDirection;
		FVector LeftTopFirstBreak = LeftBottomFirstBreak + (Height - 20) * base_z;
		FVector RightTopFirstBreak = RightBottomFirstBreak + (Height - 20) * base_z;

		BreakFirstCircle.Vertices.Emplace(LeftBottomFirstBreak);
		BreakFirstCircle.Vertices.Emplace(LeftTopFirstBreak);
		BreakFirstCircle.Vertices.Emplace(RightTopFirstBreak);
		BreakFirstCircle.Vertices.Emplace(RightBottomFirstBreak);
		AllCircle.Emplace(BreakFirstCircle);


		FLinesInfo BreakSecondCircle;
		BreakSecondCircle.DrawType = 0;
		BreakSecondCircle.Color = FLinearColor(0.0f, 1.0f, 1.0f);

		FVector RightBottomSecondBreak = RightBottomInside - 10 * HorizontalDirection + 10 * base_z;
		FVector LeftBottomSecondBreak = RightBottomSecondBreak - break_length * HorizontalDirection;
		FVector RightTopSecondBreak = RightBottomSecondBreak + (Height - 20) * base_z;
		FVector LeftTopSecondBreak = LeftBottomSecondBreak + (Height - 20) * base_z;

		BreakSecondCircle.Vertices.Emplace(LeftBottomSecondBreak);
		BreakSecondCircle.Vertices.Emplace(LeftTopSecondBreak);
		BreakSecondCircle.Vertices.Emplace(RightTopSecondBreak);
		BreakSecondCircle.Vertices.Emplace(RightBottomSecondBreak);
		AllCircle.Emplace(BreakSecondCircle);

		FLinesInfo BreakThirdCircle;
		BreakThirdCircle.DrawType = 1;
		BreakThirdCircle.Color = FLinearColor(0.0f, 1.0f, 1.0f);

		FVector TopThirdBreak = RightTopFirstBreak + 10 * base_z;
		FVector BottomThirdBreak = RightBottomFirstBreak - 10 * base_z;
		BreakThirdCircle.Vertices.Emplace(TopThirdBreak);
		BreakThirdCircle.Vertices.Emplace(BottomThirdBreak);

		AllCircle.Emplace(BreakThirdCircle);
	}

	return AllCircle;
}

TArray<FVector> FArmySlidingDoor::GetClipingBox()
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

void FArmySlidingDoor::UpdateDoorHole(EModelType InModelType)
{
	TArray<TWeakPtr<FArmyObject>> HoleList;
	FArmySceneData::Get()->GetObjects(InModelType, OT_Pass, HoleList);
	FArmySceneData::Get()->GetObjects(InModelType, OT_DoorHole, HoleList);
	for (auto It : HoleList)
	{
		TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(It.Pin());
		/**@欧石楠 如果是拆改后的门洞则忽略*/
		if (GetPropertyFlag(FArmyObject::FLAG_LAYOUT))
		{
			continue;
		}
		/**@欧石楠 如果门洞包边，或者填补，则不接受自动拆补门洞*/
		if (Pass->GetIfGeneratePassModel() || Pass->GetIfFillPass())
		{
			continue;
		}
		/**@欧石楠 如果门洞已经放门被自动拆补了，则不接受新的自动拆补*/
		if (Pass->ConnectHardwareObj.IsValid() && Pass->ConnectHardwareObj.Get() != this)
		{
			continue;
		}
		if (Pass->RectImagePanel->IsSelected(GetStartPos(), nullptr)
			|| Pass->RectImagePanel->IsSelected(GetEndPos(), nullptr)
			|| RectImagePanel->IsSelected(Pass->GetStartPos(), nullptr)
			|| RectImagePanel->IsSelected(Pass->GetEndPos(), nullptr)
			|| (GetStartPos().Equals(Pass->GetStartPos(), 0.01) && GetEndPos().Equals(Pass->GetEndPos(), 0.01))
			|| (GetStartPos().Equals(Pass->GetEndPos(), 0.01) && GetEndPos().Equals(Pass->GetStartPos(), 0.01)))
		{
			Pass->UpdateWallHole(this->AsShared(), true);
			if (Pass->ConnectHardwareObj.Get() == this)
			{
				bHasConnectPass = true;
				ConnectedPass = Pass;
				UpdataConnectedDoorHole();
			}
		}
		else
		{
			if (Pass->ConnectHardwareObj.Get() == this)
			{
				bHasConnectPass = false;
				UpdataConnectedDoorHole();
				ConnectedPass = nullptr;
				BrokenWallRightLine->SetShouldDraw(true);
				BrokenWallLeftLine->SetShouldDraw(true);
			}
			Pass->UpdateWallHole(this->AsShared(), false);
		}
	}
}

void FArmySlidingDoor::GetModifyWallAreaAndPerimeter(float & OutArea, float & OutPerimeter)
{
	OutArea = Length * Height * 0.0001f;
	OutPerimeter = (Length + Height * 2) * 0.01f;
}

void FArmySlidingDoor::GetOriginPassArea(float & OutArea)
{
	OutArea = 0;
	OutArea = (Height * 2 + Length) * Width * 0.0001f;
}

void FArmySlidingDoor::UpdataConnectedDoorHole()
{
	if (ConnectedPass.IsValid())
	{
		bool TempbPassHasVerticalAdd = ConnectedPass.Pin()->bPassHasVerticalAdd;
		bool bPassHasVerticalModify = ConnectedPass.Pin()->bPassHasVerticalModify;

		if (bHasConnectPass)
		{
			if (ConnectedPass.Pin()->GetHeight() < GetHeight())
			{
				ConnectedPass.Pin()->bPassHasVerticalAdd = false;
				ConnectedPass.Pin()->bPassHasVerticalModify = true;
			}
			else if (ConnectedPass.Pin()->GetHeight() > GetHeight())
			{
				ConnectedPass.Pin()->bPassHasVerticalAdd = true;
				ConnectedPass.Pin()->bPassHasVerticalModify = false;
			}
			else
			{
				ConnectedPass.Pin()->bPassHasVerticalAdd = false;
				ConnectedPass.Pin()->bPassHasVerticalModify = false;
			}
		}
		else
		{
			ConnectedPass.Pin()->bPassHasVerticalAdd = false;
			ConnectedPass.Pin()->bPassHasVerticalModify = false;
		}

		if (!bDeserialization && TempbPassHasVerticalAdd != ConnectedPass.Pin()->bPassHasVerticalAdd || bPassHasVerticalModify != ConnectedPass.Pin()->bPassHasVerticalModify)
		{
			ConnectedPass.Pin()->ConstructionItemData->bHasSetted = false;
		}
	}
}

void FArmySlidingDoor::SetMatType(FString InType)
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

void FArmySlidingDoor::OnFirstLineLInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	FArmyHardware::OnFirstLineLInputBoxCommitted(InText, InTextAction);
	UpdateDoorHole();
}

void FArmySlidingDoor::OnFirstLineRInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	FArmyHardware::OnFirstLineRInputBoxCommitted(InText, InTextAction);
	UpdateDoorHole();
}

void FArmySlidingDoor::OnSecondLineLInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	FArmyHardware::OnSecondLineLInputBoxCommitted(InText, InTextAction);
	UpdateDoorHole();
}

void FArmySlidingDoor::OnSecondLineRInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	FArmyHardware::OnSecondLineRInputBoxCommitted(InText, InTextAction);
	UpdateDoorHole();
}

TSharedPtr<struct ConstructionPatameters> FArmySlidingDoor::GetConstructionParameter()
{
	auto  P = MakeShared<ConstructionPatameters>();
	int32 SpaceID = -1;
	auto Room = GetRoom();

	if (Room.IsValid())
	{
		SpaceID = Room->GetSpaceId();
	}

	P->SetNormalGoodsInfo(GetDoorSaleID(), SpaceID);
	return P;
}

TSharedPtr<struct ObjectConstructionKey> FArmySlidingDoor::GetConstructionKey()
{
	auto Key = MakeShared<ObjectConstructionKey>(GetUniqueID(), *GetConstructionParameter().Get());
	return Key;
}

void FArmySlidingDoor::ConstructionData(TArray<struct FArmyGoods>& ArtificialData)
{

	if (!HasGoods())
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

	goods.DoorMsg.DoorWidth = GetLength() * 10.f;
	goods.DoorMsg.DoorHeight = GetHeight() * 10.f;
	goods.DoorMsg.WallThickness = GetWidth() * 10.f;
	goods.DoorMsg.SwitchingDirection = GetOpenDirectionID();

	int32 GoodsIndex;
	//门暂不合并，如果要合并需要考虑DoorMsg的数据的合并规则
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
}

void FArmySlidingDoor::Generate(UWorld* InWorld)
{
	Destroy();

	int32 GoodsId = (SaleID == -1) ? -1 : SaleID;
	SlidingDoorActor = FArmyResourceModule::Get().GetResourceManager()->CreateActorFromID(InWorld, GoodsId, FName(*("NOLIST-DOOR" + GetUniqueID().ToString())));
	if (!SlidingDoorActor || !SlidingDoorActor->IsValidLowLevel())
	{
		return;
	}

	SlidingDoorActor->Tags.Add(XRActorTag::Door);
	SlidingDoorActor->Tags.Add(XRActorTag::Immovable);
	SlidingDoorActor->Tags.Add(XRActorTag::CanNotDelete);

	// 计算门的位置、旋转、缩放以匹配门洞
	FVector HorizontalDirection = FArmyMath::GetLineDirection(GetStartPos(), GetEndPos());
	FVector VerticalDirection = HorizontalDirection.RotateAngleAxis(-90, FVector(0, 0, 1));
	FVRSObject* VRSObj = FArmyResourceModule::Get().GetResourceManager()->GetObjFromObjID(SlidingDoorActor->GetObjID());
	FVector DoorActorSize = CalcActorDefaultSize(SlidingDoorActor);
	FVector Location = GetPos();
	FQuat Rotation = HorizontalDirection.ToOrientationQuat();
	FVector Scale = FVector(Length / DoorActorSize.X, (Width + 2.f) / DoorActorSize.Y, (Height + 2.f) / DoorActorSize.Z);
	SlidingDoorActor->SetActorLocation(Location);
	SlidingDoorActor->SetActorRotation(Rotation);
	SlidingDoorActor->SetActorScale3D(Scale);

	SetRelevanceActor(SlidingDoorActor);

	AttachModelName = TEXT("推拉门_") + GetUniqueID().ToString();
	SlidingDoorActor->SetActorLabel(AttachModelName);

	if (SaleID > 0)
	{
		UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
		auto ContentItem = ResMgr->GetContentItemFromID( SaleID);
		if (ContentItem.IsValid() && SlidingDoorActor)
		{
			SaleID = -1;
			ReplaceGoods(ContentItem, nullptr);
		}
	}

	//@郭子阳
	//请求3D模式施工项
	XRConstructionManager::Get()->TryToFindConstructionData(this->GetUniqueID(), *(this->GetConstructionParameter().Get()), nullptr);
}

void FArmySlidingDoor::Delete()
{
	FArmyObject::Delete();

	Destroy();

	TArray<TWeakPtr<FArmyObject>> HoleList;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_Pass, HoleList);
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_DoorHole, HoleList);
	for (auto It : HoleList)
	{
		TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(It.Pin());
		if (Pass->ConnectHardwareObj == this->AsShared())
		{
			Pass->ResetAddModifyWall();
			Pass->RestModifyPass();
			Pass->RestVerticalState();
			break;
		}
	}
}

TSharedPtr<FArmyRoom> FArmySlidingDoor::GetRoom()
{
	if (FirstRelatedRoom.IsValid() && FirstRelatedRoom->GetType() != OT_OutRoom
		&& SecondRelatedRoom.IsValid() && SecondRelatedRoom->GetType() != OT_OutRoom)
	{
		//处于墙之间的门没有空间
		return nullptr;
	}

	return FirstRelatedRoom.IsValid() && FirstRelatedRoom->GetType() != OT_OutRoom ? FirstRelatedRoom : SecondRelatedRoom;
}

void FArmySlidingDoor::OnRoomSpaceIDChanged(int32 NewSpaceID)
{
	XRConstructionManager::Get()->TryToFindConstructionData(GetUniqueID(), *GetConstructionParameter().Get(), nullptr);
}