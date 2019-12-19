#include "ArmySingleDoor.h"
#include "ArmyCircle.h"
#include "ArmyPoint.h"
#include "ArmyPolygon.h"
#include "ArmyRect.h"
#include "ArmyMath.h"
#include "ArmyStyle.h"
#include "Engine/StaticMeshActor.h"
#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "ArmyEditPoint.h"
#include "ArmyShapeBoardActor.h"
#include "ArmyEngineModule.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "ArmyRulerLine.h"
#include "ArmyCommonTypes.h"
#include "ArmyActorConstant.h"
#include "ArmyViewportClient.h"
#include "ArmySceneData.h"
#include "ArmyPass.h"
#include "ArmyResourceModule.h"
#include  "ArmyConstructionManager.h"
#include "ArmyConstructionQuantity.h"
#include "ArmyRoom.h"
#define DoorModelLength 77
#define DoorModelWidth 5
#define DoorModelHeight 208

using namespace FContentItemSpace;

FArmySingleDoor::FArmySingleDoor()
	: FArmyHardware()
	, DoorActor(nullptr)
{
	ObjectType = OT_Door;
	SetPropertyFlag(FLAG_COLLISION, true);

	// 设置拆改墙填充材质
	UMaterial* ModifyMaterial = FArmyEngineModule::Get().GetEngineResource()->GetPassColorMesh();
	MI_RectModifyWall = UMaterialInstanceDynamic::Create(ModifyMaterial, nullptr);
	MI_RectModifyWall->AddToRoot();
	MI_RectModifyWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor::Red);

	// 户型绘制数据构建
	//RectImagePanel = MakeShareable(new FArmyRect());
	RectImagePanel->SetBaseColor(GVC->GetBackgroundColor());
	RectImagePanel->LineThickness = WALLLINEWIDTH + 0.1f;
	RectImagePanel->SetCalcUVs(true);
	RectImagePanel->XUVRepeatDist = 80.f;
	RectImagePanel->YUVRepeatDist = 30.f;
	RectImagePanel->MaterialRenderProxy = MI_RectModifyWall->GetRenderProxy(false);
	RectImagePanel->bIsFilled = false;

	DoorPlank = MakeShareable(new FArmyRect());

	DoorTrack = MakeShareable(new FArmyCircle());
	DoorTrack->ArcAngle = 90;
	DoorTrack->MDrawDashCircle = true;

	// 平面视图绘制数据构建
	TopViewFillingRect = MakeShareable(new FArmyRect);
	TopViewFillingRect->SetBaseColor(FLinearColor::Black);
	UMaterialInstanceDynamic* MID_BaseColor = FArmyEngineModule::Get().GetEngineResource()->GetUnStageBaseColorMaterial();
	MID_BaseColor->AddToRoot();
	MID_BaseColor->SetVectorParameterValue("BaseColor", FLinearColor(FColor(0xFFE6E6E6)));
	TopViewFillingRect->MaterialRenderProxy = MID_BaseColor->GetRenderProxy(false);
	TopViewFillingRect->bIsFilled = true;

	TopViewTrackPolygon = MakeShareable(new FArmyPolygon);
	TopViewTrackPolygon->MaterialRenderProxy = FArmyEngineModule::Get().GetEngineResource()->GetTranslucentWhiteMaterial()->GetRenderProxy(false);

	TopViewRectImagePanel = MakeShareable(new FArmyRect);
	TopViewRectImagePanel->SetBaseColor(FLinearColor::Black);

	TopViewDoorPlank = MakeShareable(new FArmyRect);
	TopViewDoorPlank->SetBaseColor(FLinearColor::Black);

	TopViewDoorTrack = MakeShareable(new FArmyCircle);
	TopViewDoorTrack->SetBaseColor(FLinearColor::Black);
	TopViewDoorTrack->ArcAngle = 90;
	TopViewDoorTrack->MDrawDashCircle = true;

	// 初始化门的材质
	UMaterial* M_SingleDoor = FArmyEngineModule::Get().GetEngineResource()->GetSingleDoorMaterial();
	MI_SingleDoor = UMaterialInstanceDynamic::Create(M_SingleDoor, nullptr);
	MI_SingleDoor->AddToRoot();

	BrokenWallLeftLine = MakeShareable(new FArmyLine());
	BrokenWallRightLine = MakeShareable(new FArmyLine());

	DoorType = 1;

	Length = 85;
	Height = DoorModelHeight;
	Width = 2;
	SetName(TEXT("标准门")/* + GetUniqueID().ToString()*/);

	ModifyWallRect = MakeShareable(new FArmyRect(RectImagePanel.Get()));
	ModifyWallRect->SetCalcUVs(true);
	ModifyWallRect->LineThickness = WALLLINEWIDTH + 0.1F;
	UMaterial* ModifyWallMaterial = FArmyEngineModule::Get().GetEngineResource()->GetModifyWallMaterial();
	MI_ModifyWall = UMaterialInstanceDynamic::Create(ModifyWallMaterial, nullptr);
	MI_ModifyWall->AddToRoot();
	MI_ModifyWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor(FColor(0xFFE305E5)));
	ModifyWallRect->MaterialRenderProxy = MI_ModifyWall->GetRenderProxy(false);
	ModifyWallRect->bIsFilled = true;
	ModifyWallRect->SetBaseColor(FLinearColor(FColor(0xFFE305E5)));

	Update();
	ConstructionItemData = MakeShareable(new FArmyConstructionItemInterface);
}

FArmySingleDoor::FArmySingleDoor(FArmySingleDoor* Copy)
{
	DoorType = Copy->DoorType;
	bBoundingBox = Copy->bBoundingBox;
	State = Copy->GetState();
	PropertyFlag = Copy->PropertyFlag;
	DoorPlank = MakeShareable(new FArmyRect(Copy->DoorPlank.Get()));
	DoorTrack = MakeShareable(new FArmyCircle(Copy->DoorTrack.Get()));
	RectImagePanel = MakeShareable(new FArmyRect(Copy->RectImagePanel.Get()));
	StartPoint = MakeShareable(new FArmyEditPoint(Copy->StartPoint.Get()));
	EndPoint = MakeShareable(new FArmyEditPoint(Copy->EndPoint.Get()));
	Point = MakeShareable(new FArmyEditPoint(Copy->Point.Get()));
	DirectionPoint = MakeShareable(new FArmyEditPoint(Copy->DirectionPoint.Get()));
	AttachModelName = Copy->AttachModelName;

	RectImagePanel->SetBaseColor(GVC->GetBackgroundColor());
	RectImagePanel->LineThickness = WALLLINEWIDTH + 0.1f;
	RectImagePanel->XUVRepeatDist = 80.f;
	RectImagePanel->YUVRepeatDist = 30.f;
	RectImagePanel->SetCalcUVs(true);
	UMaterial* ModifyWallMaterial = FArmyEngineModule::Get().GetEngineResource()->GetPassColorMesh();
	MI_RectModifyWall = UMaterialInstanceDynamic::Create(ModifyWallMaterial, nullptr);
	MI_RectModifyWall->AddToRoot();
	MI_RectModifyWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor::Red);
	RectImagePanel->MaterialRenderProxy = MI_RectModifyWall->GetRenderProxy(false);
	RectImagePanel->bIsFilled = Copy->RectImagePanel->bIsFilled;

	UMaterial* M_SingleDoor = FArmyEngineModule::Get().GetEngineResource()->GetSingleDoorMaterial();
	MI_SingleDoor = UMaterialInstanceDynamic::Create(M_SingleDoor, nullptr);
	MI_SingleDoor->AddToRoot();

	Direction = Copy->Direction;
	bRightOpen = Copy->bRightOpen;
	Length = Copy->Length;
	Width = Copy->Width;
	Height = Copy->Height;
	ObjectType = Copy->ObjectType;
	SetName(Copy->GetName());
	DoorActor = Copy->DoorActor;
	LinkFirstLine = Copy->LinkFirstLine;
	LinkSecondLine = Copy->LinkSecondLine;
	FirstRelatedRoom = Copy->FirstRelatedRoom;
	SecondRelatedRoom = Copy->SecondRelatedRoom;

	//只有点击到墙体上的门才有
	BrokenWallLeftLine = MakeShareable(new FArmyLine());
	BrokenWallRightLine = MakeShareable(new FArmyLine());

	TopViewFillingRect = MakeShareable(new FArmyRect(Copy->TopViewFillingRect.Get()));
	TopViewTrackPolygon = MakeShareable(new FArmyPolygon(Copy->TopViewTrackPolygon.Get()));
	TopViewRectImagePanel = MakeShareable(new FArmyRect(Copy->TopViewRectImagePanel.Get()));
	TopViewDoorPlank = MakeShareable(new FArmyRect(Copy->TopViewDoorPlank.Get()));
	TopViewDoorTrack = MakeShareable(new FArmyCircle(Copy->TopViewDoorTrack.Get()));

	ModifyWallRect = MakeShareable(new FArmyRect(RectImagePanel.Get()));
	ModifyWallRect->SetCalcUVs(true);
	ModifyWallRect->LineThickness = WALLLINEWIDTH + 0.1F;
	UMaterial* ModifyWallMat = FArmyEngineModule::Get().GetEngineResource()->GetModifyWallMaterial();
	MI_ModifyWall = UMaterialInstanceDynamic::Create(ModifyWallMat, nullptr);
	MI_ModifyWall->AddToRoot();
	MI_ModifyWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor(FColor(0xFFE305E5)));
	ModifyWallRect->MaterialRenderProxy = MI_ModifyWall->GetRenderProxy(false);
	ModifyWallRect->bIsFilled = true;
	ModifyWallRect->SetBaseColor(FLinearColor(FColor(0xFFE305E5)));

	bIsCopy = true;

	Update();
	ConstructionItemData = MakeShareable(new FArmyConstructionItemInterface);
}

FArmySingleDoor::~FArmySingleDoor()
{
	//DeleteDoorAndFrameModel();
	MI_SingleDoor->RemoveFromRoot();
}

void FArmySingleDoor::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyHardware::SerializeToJson(JsonWriter);

	JsonWriter->WriteValue("pos", GetPos().ToString());
	JsonWriter->WriteValue("direction", GetDirection().ToString());
	JsonWriter->WriteValue("length", GetLength());
	JsonWriter->WriteValue("width", GetWidth());
	JsonWriter->WriteValue("height", GetHeight());
	JsonWriter->WriteValue("bRightOpen", IsRightOpen());
	JsonWriter->WriteValue("SaleID", GetDoorSaleID());
	JsonWriter->WriteValue("thumbnailUrl", ThumbnailUrl);
	JsonWriter->WriteValue("DoorType", GetDoorType());
	JsonWriter->WriteValue("bHasConnectPass", bHasConnectPass);
	JsonWriter->WriteValue("openDirectionID", OpenDirectionID);
	JsonWriter->WriteValue("MatType", GetMatType());
	/**@欧石楠 存储施工项*/
	ConstructionItemData->SerializeToJson(JsonWriter);

	if (DoorActor)
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



	SERIALIZEREGISTERCLASS(JsonWriter, FArmySingleDoor)
}

void FArmySingleDoor::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
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
	SetDoorType(InJsonData->GetNumberField("DoorType") == 0 ? 1 : InJsonData->GetNumberField("DoorType"));
	bHasConnectPass = InJsonData->GetBoolField("bHasConnectPass");
	InJsonData->TryGetNumberField("openDirectionID", OpenDirectionID);
	SetMatType(InJsonData->GetStringField(TEXT("MatType")));
	ModifyWallRect = MakeShareable(new FArmyRect(RectImagePanel.Get()));
	ModifyWallRect->SetCalcUVs(true);
	ModifyWallRect->LineThickness = WALLLINEWIDTH + 0.1F;
	UMaterial* ModifyWallMaterial = FArmyEngineModule::Get().GetEngineResource()->GetModifyWallMaterial();
	MI_ModifyWall = UMaterialInstanceDynamic::Create(ModifyWallMaterial, nullptr);
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

void FArmySingleDoor::SetState(EObjectState InState)
{
	State = InState;

	switch (InState)
	{
	case OS_Normal:
		DoorPlank->SetBaseColor(FLinearColor::White);
		DoorTrack->SetBaseColor(FLinearColor::White);
		DeselectPoints();
		if (BrokenWallLeftLine.IsValid())
		{
			BrokenWallLeftLine->SetBaseColor(FLinearColor::White);
			BrokenWallRightLine->SetBaseColor(FLinearColor::White);
		}
		break;

	case OS_Hovered:
	case OS_Selected:
		DoorPlank->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		DoorTrack->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		if (BrokenWallLeftLine.IsValid())
		{
			BrokenWallLeftLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
			BrokenWallRightLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		}
		break;

	case OS_Disable:
		DoorPlank->SetBaseColor(FLinearColor::Gray);
		DoorTrack->SetBaseColor(FLinearColor::Gray);
		break;

	default:
		break;
	}
}

void FArmySingleDoor::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (FArmyObject::GetDrawModel(MODE_TOPVIEW) || FArmyObject::GetDrawModel(MODE_CEILING))
		{
			TopViewFillingRect->Draw(PDI, View);
			TopViewTrackPolygon->Draw(PDI, View);
			TopViewRectImagePanel->Draw(PDI, View);
			TopViewDoorPlank->Draw(PDI, View);
			TopViewDoorTrack->Draw(PDI, View);
		}
		else if (FArmyObject::GetDrawModel(MODE_MODIFYDELETE) && FArmyObject::GetDrawModel(MODE_CONSTRUCTION) && !bHasConnectPass && InWallType == 0)
		{
			ModifyWallRect->Draw(PDI, View);
		}
		else
		{
			/*if (FArmyObject::GetDrawModel(MODE_CONSTRUCTION) && FArmyObject::GetDrawModel(MODE_MODIFYDELETE))
			{
				RectImagePanel->SetBaseColor(FLinearColor::Red);
				RectImagePanel->bIsFilled = true;
				RectImagePanel->Draw(PDI, View);
			}
			else if (FArmyObject::GetDrawModel(MODE_CONSTRUCTION) && FArmyObject::GetDrawModel(MODE_DELETEWALLPOST))
			{
				RectImagePanel->bIsFilled = false;
				MI_RectModifyWall->SetVectorParameterValue(TEXT("MainColor"), FLinearColor::White);
				RectImagePanel->Draw(PDI, View);
			}
			else*/
			{
				//RectImagePanel->bIsFilled = false;
				if (!(FArmyObject::GetDrawModel(MODE_DOORPASS) && bHasConnectPass))
				{
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
					DoorPlank->Draw(PDI, View);
					DoorTrack->Draw(PDI, View);
				}
			}

		}
	}
}

bool FArmySingleDoor::IsSelected(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		bShowDashLine = true;
		return DoorTrack->IsSelected(Pos, InViewportClient) && FArmyMath::IsPointInOrOnPolygon2D(Pos, GetBoundingBox());
	}
	return false;
}

bool FArmySingleDoor::Hover(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		return IsSelected(Pos, InViewportClient);
	}
	return false;
}

void FArmySingleDoor::ApplyTransform(const FTransform& Trans)
{
	//StartEditPoint->SetPos(Trans.TransformPosition(StartEditPoint->GetPos()));
	//EndEditPoint->SetPos(Trans.TransformPosition(EndEditPoint->GetPos()));
	//DoorEditPoint->SetPos(Trans.TransformPosition(DoorEditPoint->GetPos()));
	//DoorDirectionPoint->SetPos(Trans.TransformPosition(DoorDirectionPoint->GetPos()));
}

void FArmySingleDoor::Destroy()
{
	if (DoorActor && DoorActor->IsValidLowLevel())
	{
		DoorActor->Destroy();
		DoorActor = nullptr;
	}
}

void FArmySingleDoor::Update()
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

	DoorPlank->Pos = CurrentEnd;
	DoorPlank->XDirection = -HorizontalDirection;
	DoorPlank->YDirection = Direction;
	DoorPlank->Height = Length + Width / 2.f;
	DoorPlank->Width = 5;

	DoorTrack->Position = CurrentEnd + Direction * Width / 2.f;
	DoorTrack->Radius = Length;
	DoorTrack->XDirection = -HorizontalDirection;
	DoorTrack->YDirection = Direction;

	// 顶视图绘制数据构建
	int32 TopViewZ0 = FArmySceneData::WallHeight + 21.f;
	int32 TopViewZ1 = TopViewZ0 + 1;

	TopViewFillingRect->Pos = TopViewRectImagePanel->Pos = RectImagePanel->Pos;
	TopViewFillingRect->Pos.Z = TopViewZ0;
	TopViewRectImagePanel->Pos.Z = TopViewZ1;
	TopViewFillingRect->XDirection = TopViewRectImagePanel->XDirection = RectImagePanel->XDirection;
	TopViewFillingRect->YDirection = TopViewRectImagePanel->YDirection = RectImagePanel->YDirection;
	TopViewFillingRect->Width = TopViewRectImagePanel->Width = RectImagePanel->Width;
	TopViewFillingRect->Height = TopViewRectImagePanel->Height = RectImagePanel->Height;

	TArray<FVector> PolygonVertexes = DoorTrack->GetDrawingVertexes();
	for (auto& It : PolygonVertexes)
	{
		It.Z = TopViewZ0;
	}
	TopViewTrackPolygon->RemoveAllVertexes();
	TopViewTrackPolygon->SetVertices(PolygonVertexes);

	TopViewDoorPlank->Pos = DoorPlank->Pos;
	TopViewDoorPlank->Pos.Z = TopViewZ1;
	TopViewDoorPlank->XDirection = DoorPlank->XDirection;
	TopViewDoorPlank->YDirection = DoorPlank->YDirection;
	TopViewDoorPlank->Height = DoorPlank->Height;
	TopViewDoorPlank->Width = DoorPlank->Width;

	TopViewDoorTrack->Position = DoorTrack->Position;
	TopViewDoorTrack->Position.Z = TopViewZ1;
	TopViewDoorTrack->Radius = DoorTrack->Radius;
	TopViewDoorTrack->XDirection = DoorTrack->XDirection;
	TopViewDoorTrack->YDirection = DoorTrack->YDirection;
}

/*获取户型模式下Draw的线段集合*/
void FArmySingleDoor::GetCadLineArray(TArray<TPair<FVector, FVector>>& OutLineList) const
{
	//FArmyHardware::GetCadLineArray(OutLineList);

	[&](TArray<TPair<FVector, FVector>>& OutLineList, TSharedPtr<class FArmyRect> Rect) {
		if (Rect.IsValid())
		{
			//计算Rect各边线段
			const int32& VerticesNum = Rect->GetVertices().Num();
			for (int32 Index = 0; Index < VerticesNum; ++Index)
				OutLineList.AddUnique(TPair<FVector, FVector>(Rect->GetVertices()[Index%VerticesNum], Rect->GetVertices()[(Index + 1) % VerticesNum]));
		}
	}(OutLineList, DoorPlank);

	// 	OutLineList.Emplace(TPair<FVector, FVector>(BrokenWallLeftLine->GetStart(), BrokenWallLeftLine->GetEnd()));
	// 	OutLineList.Emplace(TPair<FVector, FVector>(BrokenWallRightLine->GetStart(), BrokenWallRightLine->GetEnd()));
}

TArray<FVector> FArmySingleDoor::GetBoundingBox()
{
	FVector LeftBootom = GetStartPos();
	FVector LeftTop = GetStartPos() + Direction * Length;
	FVector RightTop = GetEndPos() + Direction * Length;
	FVector RightBottom = GetEndPos();
	TArray<FVector> BoundingBox = { LeftBootom,LeftTop,RightTop,RightBottom };
	return BoundingBox;
}

TArray<struct FLinesInfo> FArmySingleDoor::GetFacadeBox()
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

	/** 折线 */
	FLinesInfo BreakCicle;
	BreakCicle.DrawType = 1;
	BreakCicle.Color = FLinearColor(56.f / 255.f, 58.f / 255.f, 64.f / 255.f);
	/** 折点位置代表合页位置 */
	FVector FirstBreakPoint = FVector::ZeroVector;
	FVector SecondBreakPoint = FVector::ZeroVector;
	FVector ThirdBreakPoint = FVector::ZeroVector;

	FVector DirectionPoint = FArmyMath::GetProjectionPoint(this->GetDirectionPointPos(), GetStartPos(), GetEndPos());


	if (sqrt(pow(LeftBottomInside.X - DirectionPoint.X, 2) + pow(LeftBottomInside.Y - DirectionPoint.Y, 2)) < sqrt(pow(RightBottomInside.X - DirectionPoint.X, 2) + pow(RightBottomInside.Y - DirectionPoint.Y, 2)))
	{
		FirstBreakPoint = RightTopInside;
		SecondBreakPoint = LeftBottomInside + Height * 0.5f * base_z;
		ThirdBreakPoint = RightBottomInside;
	}
	else
	{
		FirstBreakPoint = LeftTopInside;
		SecondBreakPoint = RightBottomInside + Height * 0.5f * base_z;
		ThirdBreakPoint = LeftBottomInside;
	}
	BreakCicle.Vertices.Emplace(FirstBreakPoint);
	BreakCicle.Vertices.Emplace(SecondBreakPoint);
	BreakCicle.Vertices.Emplace(ThirdBreakPoint);
	AllCircle.Emplace(BreakCicle);

	return AllCircle;
}

TArray<FVector> FArmySingleDoor::GetClipingBox()
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

const FBox FArmySingleDoor::GetBounds()
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

void FArmySingleDoor::UpdateDoorHole(EModelType InModelType)
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
			}
			Pass->UpdateWallHole(this->AsShared(), false);
		}
	}
}

void FArmySingleDoor::GetModifyWallAreaAndPerimeter(float & OutArea, float & OutPerimeter)
{
	OutArea = Length * Height * 0.0001f;
	OutPerimeter = (Length + Height * 2) * 0.01f;
}

void FArmySingleDoor::GetOriginPassArea(float & OutArea)
{
	OutArea = 0;
	OutArea = (Height * 2 + Length) * Width * 0.0001f;
}

void FArmySingleDoor::OnFirstLineLInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	FArmyHardware::OnFirstLineLInputBoxCommitted(InText, InTextAction);
	UpdateDoorHole();
}

void FArmySingleDoor::OnFirstLineRInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	FArmyHardware::OnFirstLineRInputBoxCommitted(InText, InTextAction);
	UpdateDoorHole();
}

void FArmySingleDoor::OnSecondLineLInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	FArmyHardware::OnSecondLineLInputBoxCommitted(InText, InTextAction);
	UpdateDoorHole();
}

void FArmySingleDoor::OnSecondLineRInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	FArmyHardware::OnSecondLineRInputBoxCommitted(InText, InTextAction);
	UpdateDoorHole();
}

void FArmySingleDoor::Generate(UWorld* InWorld)
{
	Destroy();

	int32 GoodsId = (SaleID == -2) ? -2 : SaleID;
	DoorActor = FArmyResourceModule::Get().GetResourceManager()->CreateActorFromID(InWorld, GoodsId, FName(*("NOLIST-DOOR" + GetUniqueID().ToString())));
	if (!DoorActor || !DoorActor->IsValidLowLevel())
	{
		return;
	}

	DoorActor->Tags.Add(XRActorTag::Door);
	DoorActor->Tags.Add(XRActorTag::Immovable);
	DoorActor->Tags.Add(XRActorTag::CanNotDelete);

	FVector DoorOrigin, BoxExtent;
	DoorActor->GetActorBounds(true, DoorOrigin, BoxExtent);
	FVector DoorActorSize = CalcActorDefaultSize(DoorActor);

	float ScaleX = -FMath::Abs(Length / DoorActorSize.X);
	float ScaleY = FMath::Abs((Width + 5.f) / DoorActorSize.Y);

	// 计算门的位置、旋转、缩放以匹配门洞
	FVector HDirection = FArmyMath::GetLineDirection(GetEndPos(), GetStartPos());
	FVector VDirection = FArmyMath::GetLineDirection(GetEndPos(), GetDirectionPointPos());
	FVector Dir1 = (-HDirection).RotateAngleAxis(-90, FVector(0, 0, 1));
	FVector Dir2 = FArmyMath::GetLineDirection(GetEndPos(), GetDirectionPointPos());
	FQuat Rotation = HDirection.ToOrientationQuat();

	bool bIsClockwise = (HDirection ^ VDirection).Z > 0;
	bool bIsPositiveDir = (Dir1 | Dir2) >= 0;
	if (!bIsClockwise && !bIsPositiveDir)
	{
		ScaleX = -ScaleX;
		Rotation = (-HDirection).ToOrientationQuat();
	}

	float OffsetXScale = FMath::Abs(DoorOrigin.X / DoorActorSize.X);
	float OffsetYScale = FMath::Abs((DoorOrigin.Y - 2.5f) / DoorActorSize.Y);
	FVector Location = GetPos() - HDirection * OffsetXScale * Length + VDirection * OffsetYScale * (Width + 5.f);
	FVector Scale = FVector(ScaleX, ScaleY, (Height + 4.f) / DoorActorSize.Z);

	DoorActor->SetActorLocation(Location);
	DoorActor->SetActorRotation(Rotation);
	DoorActor->SetActorScale3D(Scale);

	SetRelevanceActor(DoorActor);
	DoorActor->SetActorLabel(TEXT("NOLIST-标准门_") + GetUniqueID().ToString());
	AttachModelName = DoorActor->GetActorLabel();


	if (SaleID > 0)
	{
		UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
		auto ContentItem = ResMgr->GetContentItemFromID( SaleID);
		if (ContentItem.IsValid() && DoorActor)
		{
			SaleID = -1;
			ReplaceGoods(ContentItem, nullptr);
		}
	}

	//@郭子阳
	//请求3D模式施工项
	XRConstructionManager::Get()->TryToFindConstructionData(this->GetUniqueID(), *(this->GetConstructionParameter().Get()),nullptr);
}

void FArmySingleDoor::Delete()
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

void FArmySingleDoor::ReplaceGoods(FContentItemPtr InContentItem, UWorld* InWorld)
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
			TSharedPtr<FArmySingleDoor> ModifySingleDoor = StaticCastSharedPtr<FArmySingleDoor>(ModifyDoor.Pin());
			if (ModifySingleDoor.IsValid())
			{
				ModifySingleDoor->SaleID = InContentItem->ID;
				ModifySingleDoor->ThumbnailUrl = InContentItem->ThumbnailURL;
			}
		}
	}
}

bool FArmySingleDoor::IsGenerative() const
{
	return (SaleID != -1);
}

TSharedPtr<FArmyArcLine> FArmySingleDoor::GetDoorArcLine() const
{
	TSharedPtr<FArmyArcLine> ArcLine = MakeShareable(new FArmyArcLine());
	TSharedPtr<FArmyCircle> Circle = GetDoorTrack();
	if (Circle.IsValid())
	{
		ArcLine->SetCadPos(Circle->Position + Circle->Radius*Circle->XDirection, Circle->Position + Circle->Radius*Circle->YDirection, Circle->Position);
		ArcLine->SetRadius(Circle->Radius);
		ArcLine->SetClockwise(bRightOpen);//右开-->顺时针
	}

	return ArcLine;
}

void FArmySingleDoor::UpdataConnectedDoorHole()
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
void FArmySingleDoor::SetMatType(FString InType)
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
//获取施工项查询Key
TSharedPtr<ObjectConstructionKey> FArmySingleDoor::GetConstructionKey()
{

	auto Key = MakeShared<ObjectConstructionKey>(GetUniqueID(), *GetConstructionParameter().Get());
	//ObjectConstructionKey Key(GetUniqueID(), P);
	return Key;

}

void FArmySingleDoor::ConstructionData(TArray<struct FArmyGoods>& ArtificialData)
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
	if (SecondRelatedRoom.IsValid() && SecondRelatedRoom ->GetSpaceNameLabel().IsValid())
	{
		goods.SpaceMsg.SpaceName2 =SecondRelatedRoom->GetSpaceName();
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

TSharedPtr<FArmyRoom> FArmySingleDoor::GetRoom()
{
	if (FirstRelatedRoom.IsValid() && FirstRelatedRoom->GetType() != OT_OutRoom
		&& SecondRelatedRoom.IsValid() && SecondRelatedRoom->GetType() != OT_OutRoom)
	{
		//处于墙之间的门没有空间
		return nullptr;
	}

	return FirstRelatedRoom.IsValid() && FirstRelatedRoom->GetType() != OT_OutRoom ? FirstRelatedRoom : SecondRelatedRoom;
}

void FArmySingleDoor::OnRoomSpaceIDChanged(int32 NewSpaceID)
{
	XRConstructionManager::Get()->TryToFindConstructionData(GetUniqueID(), *GetConstructionParameter().Get(), nullptr);
}

TSharedPtr<ConstructionPatameters> FArmySingleDoor::GetConstructionParameter()
{
	auto  P= MakeShared<ConstructionPatameters>();

	int32 SpaceID = -1;
	auto Room = GetRoom();

	if (Room.IsValid())
	{
		SpaceID = Room->GetSpaceId();
	}

	P->SetNormalGoodsInfo(GetDoorSaleID(), SpaceID);
	return P;
}