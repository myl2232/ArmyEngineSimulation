#include "ArmyCornerBayWindow.h"
#include "ArmyShapeBoardActor.h"
#include "ArmyEditPoint.h"
#include "ArmyMath.h"
#include "ArmyEngineModule.h"
#include "ArmyResourceModule.h"
#include "ArmyGameInstance.h"

#define FirstGlassWith 3.5
#define SecondGlassWidth 7
#define DefaultWindowHoleDepth 100
#define DefaultLeftWindowLength 180
#define DefaultRightWindowLength 180
#define DefaultLeftWindowOffset 0
#define DefaultRightWindowOffset 0
#define DefaultWindowHeight 200
#define DefaultOffsetGroundHeight 50

FArmyCornerBayWindow::FArmyCornerBayWindow() :
	LeftWindowLength(DefaultLeftWindowLength),
	RightWindowLength(DefaultRightWindowLength),
	LeftWalllOffset(DefaultLeftWindowOffset),
	RightWallOffset(DefaultRightWindowOffset),
	BayWindowHoleDepth(DefaultWindowHoleDepth),
	//WindowHeight(DefaultWindowHeight),
	//HeightToFloor(DefaultOffsetGroundHeight),
	WindowBoard(NULL),
	WindowGlass(NULL),
	WindowPillar(NULL),
	WindowWall(NULL),
	CornerWindowType(CornerLeftBoard),
	WindowBottomExtrudeLength(3),
	WindowBottomHeight(5)
{
    SetName(TEXT("杞椋樼獥") + GetUniqueID().ToString());
	ObjectType = OT_CornerBayWindow;
	Height = DefaultWindowHeight;
	HeightToFloor = DefaultOffsetGroundHeight;
	SetPropertyFlag(FLAG_COLLISION, true);
	LeftDirection = FVector(-1, 0, 0);
	RightDirection = FVector(0, 1, 0);
	LeftWallWidth = 24.0f;
	RightWallWidth = 24.0f;
	InnearCenterPos = FVector(0, 0, 0);
}


FArmyCornerBayWindow::FArmyCornerBayWindow(FArmyCornerBayWindow* Copy)
{
    SetName(GetName());
	WindowBoard = Copy->WindowBoard;
	WindowGlass = Copy->WindowGlass;
	WindowPillar = Copy->WindowPillar;
	WindowWall = Copy->WindowWall;
	LocalViewPortClient = Copy->LocalViewPortClient;
	CornerWindowType = Copy->CornerWindowType;
	WindowBottomExtrudeLength = Copy->WindowBottomExtrudeLength;
	WindowBottomHeight = Copy->WindowBottomHeight;
	StartPoint = MakeShareable(new FArmyEditPoint(Copy->StartPoint.Get()));
	EndPoint = MakeShareable(new FArmyEditPoint(Copy->EndPoint.Get()));
	Point = MakeShareable(new FArmyEditPoint(Copy->Point.Get()));
	ObjectType = Copy->ObjectType;
	LeftDirection = Copy->LeftDirection;
	RightDirection = Copy->RightDirection;
	LeftWallDimPos = Copy->LeftWallDimPos;
	RightWallDimPos = Copy->RightWallDimPos;
	LeftWallWidth = Copy->LeftWallWidth;
	RightWallWidth = Copy->RightWallWidth;
	InnearCenterPos = Copy->InnearCenterPos;
	LeftWalllOffset = Copy->LeftWalllOffset;
	RightWallOffset = Copy->RightWallOffset;
	LeftWindowLength = Copy->LeftWindowLength;
	RightWindowLength = Copy->RightWindowLength;
	BayWindowHoleDepth = Copy->BayWindowHoleDepth;
	Height = Copy->Height;
	HeightToFloor = Copy->HeightToFloor;
	LeftUpPointPos = Copy->LeftUpPointPos;
	RightUpPointPos = Copy->RightUpPointPos;
	OutWindowVerticeList = Copy->OutWindowVerticeList;
	MiddleWindowVerticeList = Copy->MiddleWindowVerticeList;
	InnearWindowVerticeList = Copy->InnearWindowVerticeList;
	BottomWindowVerticeList = Copy->BottomWindowVerticeList;

	RightBoardMiddleList = Copy->RightBoardMiddleList;
	RightBoardInnearList = Copy->RightBoardInnearList;
	RightBoardRightList = Copy->RightBoardRightList;

	LeftBoardMiddleList = Copy->LeftBoardMiddleList;
	LeftBoardInnearList = Copy->LeftBoardInnearList;
	LeftBoardLeftList = Copy->LeftBoardLeftList;

	DoubleBoardLeftList = Copy->DoubleBoardLeftList;
	DoubleBoardRightList = Copy->DoubleBoardRightList;
	DoubleBoardMiddleList = Copy->DoubleBoardMiddleList;
	DoubleBoardInnearList = Copy->DoubleBoardInnearList;
}

FArmyCornerBayWindow::~FArmyCornerBayWindow()
{
}

void FArmyCornerBayWindow::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyHardware::SerializeToJson(JsonWriter);
	FArmyObject::SerializeToJson(JsonWriter);

	JsonWriter->WriteValue("LeftDirection", LeftDirection.ToString());
	JsonWriter->WriteValue("RightDirection", RightDirection.ToString());
	JsonWriter->WriteValue("LeftWallDimPos", LeftWallDimPos.ToString());
	JsonWriter->WriteValue("RightWallDimPos", RightWallDimPos.ToString());
	JsonWriter->WriteValue("RightWallWidth", RightWallWidth);
	JsonWriter->WriteValue("LeftWallWidth", LeftWallWidth);
	JsonWriter->WriteValue("InnearCenterPos", *(InnearCenterPos.ToString()));
	JsonWriter->WriteValue("LeftWalllOffset", GetLeftWindowOffset());
	JsonWriter->WriteValue("RightWallOffset", GetRightWindowOffset());
	JsonWriter->WriteValue("LeftWindowLength", GetLeftWindowLength());
	JsonWriter->WriteValue("RightWindowLength", GetRightWindowLength());
	JsonWriter->WriteValue("BayWindowHoleDepth", GetWindowHoleDepth());
	//JsonWriter->WriteValue("Height", GetWindowHeight());
	JsonWriter->WriteValue("WindowBottomHeight", GetWindowBottomBoardHeight());
	JsonWriter->WriteValue("WindowBottomExtrudeLength", GetWindowBottomBoardExtrudeLength());
	//JsonWriter->WriteValue("OffsetGroundHeight", GetHeightToFloor());
	JsonWriter->WriteValue("CornerBayWindowType", (int32)GetWindowCornerType());
	if (WindowBoard)
	{
		UXRResourceManager * ResMg = FArmyResourceModule::Get().GetResourceManager();
		//TSharedPtr<FContentItemSpace::FContentItem> item = ResMg->GetContentItemFromID(HoleWindow->GetUniqueID());
		TSharedPtr<FContentItemSpace::FContentItem> ResultSynData = ResMg->GetContentItemFromID(WindowBoard->GetSynID());
		if (ResultSynData.IsValid())
		{
			ResultSynData->SerializeToJson(JsonWriter);
			GGI->DesignEditor->SaveFileList.AddUnique(ResultSynData->ID);
		}
	}
	SERIALIZEREGISTERCLASS(JsonWriter, FArmyCornerBayWindow)
}

void FArmyCornerBayWindow::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyHardware::Deserialization(InJsonData);
	FArmyObject::Deserialization(InJsonData);

	FVector LeftDirection, RightDirection, InnearCenterPos, LeftWallDimPos, RightWallDimPos;
	float leftWallWidth, rightWallWidth;
	LeftDirection.InitFromString(InJsonData->GetStringField("LeftDirection"));
	RightDirection.InitFromString(InJsonData->GetStringField("RightDirection"));
	InnearCenterPos.InitFromString(InJsonData->GetStringField("InnearCenterPos"));
	LeftWallDimPos.InitFromString(InJsonData->GetStringField("LeftWallDimPos"));
	RightWallDimPos.InitFromString(InJsonData->GetStringField("RightWallDimPos"));
	leftWallWidth = InJsonData->GetNumberField("LeftWallWidth");
	rightWallWidth = InJsonData->GetNumberField("RightWallWidth");
	LeftWallDimPos = LeftWallDimPos;
	RightWallDimPos = RightWallDimPos;
	SetProperty(InnearCenterPos, LeftDirection, RightDirection, leftWallWidth, rightWallWidth);
	SetLeftWindowOffset(InJsonData->GetNumberField("LeftWalllOffset"));
	SetRightWindowOffset(InJsonData->GetNumberField("RightWallOffset"));
	SetLeftWindowLength(InJsonData->GetNumberField("LeftWindowLength"));
	SetRightWindowLength(InJsonData->GetNumberField("RightWindowLength"));
	SetWindowHoleDepth(InJsonData->GetNumberField("BayWindowHoleDepth"));
	//SetWindowHeight(InJsonData->GetNumberField("Height"));
	SetWindowBottomBoardHeight(InJsonData->GetNumberField("WindowBottomHeight"));
	SetWindowBottomBoardExtrudeLength(InJsonData->GetNumberField("WindowBottomExtrudeLength"));
	//SetWindowOffsetGroundHeight(InJsonData->GetNumberField("OffsetGroundHeight"));
	SetWindowCornerType(FArmyCornerBayWindow::ECornerBayWindowType(InJsonData->GetIntegerField("CornerBayWindowType")));
	TSharedPtr<FContentItemSpace::FContentItem> ContentItem = MakeShareable(new FContentItemSpace::FContentItem());
	ContentItem->ResObjArr[0] = MakeShareable(new FContentItemSpace::FModelRes());
	ContentItem->Deserialization(InJsonData);
	UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
	UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
	if (mat)
	{
		bDefaultMaterial = false;
		WindowStoneMat = mat;
		//HoleWindow->SetWindowBoardMaterial(mat);
	}
}
void FArmyCornerBayWindow::SetState(EObjectState InState)
{
	State = InState;
	switch (InState)
	{
	case OS_Normal:
		DeselectPoints();
		break;
	case OS_Hovered:
		break;
	case OS_Selected:
		break;
	case OS_Disable:
		break;
	default:
		break;
	}
}

void FArmyCornerBayWindow::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if ((!FArmyObject::GetDrawModel(MODE_MODIFYADD) && InWallType == 1) ||//鏂藉伐鍥句笉鏄剧ず鏂板澧欙紝骞朵笖璇ュ灜鍙ｅ湪鏂板澧欎笂鏃讹紝涓嶇粯鍒?
			(FArmyObject::GetDrawModel(MODE_DELETEWALLPOST) && !bGenerate3D))//鎷嗛櫎澧欎笂甯︿簡绐楁埗锛屽苟涓旀妸闂ㄦ礊涔熸媶闄や簡锛屽湪鎷嗛櫎鍚庣殑鏁堟灉涓笉瑕佹樉绀洪棬娲?
		{
			return;
		}

        if (State == OS_Selected)
        {
            FArmyHardware::Draw(PDI, View);
        }

		DrawAntiBayWindow(PDI, View);
	}
}

bool FArmyCornerBayWindow::IsSelected(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
        bShowDashLine = true;

		TArray<FVector> tempOutLine = OutWindowVerticeList;
		tempOutLine.Push(InnearCenterPos);
		return FArmyMath::IsPointInOrOnPolygon2D(Pos, tempOutLine);
	}
	return false;
}

bool FArmyCornerBayWindow::Hover(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	return false;
}

void FArmyCornerBayWindow::SetProperty(const FVector& InPos, const FVector& InleftWallDirection, const FVector& InrightWallDirection, const float InleftWallWidth, const float InrightWallWidth)
{
	InnearCenterPos = InPos;
	LeftDirection = InleftWallDirection;
	RightDirection = InrightWallDirection;
	LeftWallWidth = InleftWallWidth;
	RightWallWidth = InrightWallWidth;

    Update();
}

TArray<FVector> FArmyCornerBayWindow::GetBoundingBox()
{
	TArray<FVector> results;
	return results;
}


TArray<struct FLinesInfo> FArmyCornerBayWindow::GetFacadeBox()
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

TArray<FVector> FArmyCornerBayWindow::GetClipingBox()
{
	TArray<FVector> results;
	FVector tempOffset = FVector(0, 0, HeightToFloor + Height / 2.0f);
	FVector tempLeftVertical = LeftDirection.RotateAngleAxis(90, FVector(0, 0, 1));
	FVector tempRightVertical = RightDirection.RotateAngleAxis(-90, FVector(0, 0, 1));
	FVector first = LeftUpPointPos + tempLeftVertical * 10.0f;
	FVector third = RightUpPointPos + tempRightVertical*10.0f;
	FVector tempSecond0 = first - tempLeftVertical * 20.0f;
	FVector tempSecond1 = third - tempRightVertical*20.0f;
	FVector2D second;
	FArmyMath::Line2DIntersection(FVector2D(first), FVector2D(tempSecond0), FVector2D(third), FVector2D(tempSecond1), second);
	FVector tempLast0 = first - LeftDirection * 20.0f;
	FVector tempLast1 = third - RightDirection *20.0f;
	FVector2D last;
	FArmyMath::Line2DIntersection(FVector2D(first), FVector2D(tempLast0), FVector2D(third), FVector2D(tempLast1), last);

	results.Push(first + tempOffset);
	results.Push(FVector(second, 0.0f) + tempOffset);
	results.Push(third + tempOffset);
	results.Push(FVector(last, 0.0f) + tempOffset);

	return results;
}

void FArmyCornerBayWindow::SetLeftWindowLength(float InLength)
{
	if (LeftWindowLength != InLength)
	{
		LeftWindowLength = InLength;
		//UpateCornerBayWindow();
		//UpdateOperationPointPos();
	}
}

void FArmyCornerBayWindow::SetRightWindowLength(float InLength)
{
	if (RightWindowLength != InLength)
	{
		RightWindowLength = InLength;
		//UpateCornerBayWindow();
		//UpdateOperationPointPos();
	}
}

void FArmyCornerBayWindow::SetLeftWindowOffset(float InOffset)
{
	if (LeftWalllOffset != InOffset)
	{
		LeftWalllOffset = InOffset;
		//UpateCornerBayWindow();
		//UpdateOperationPointPos();
	}
}

void FArmyCornerBayWindow::SetRightWindowOffset(float InOffset)
{
	if (RightWallOffset != InOffset)
	{
		RightWallOffset = InOffset;
		//UpateCornerBayWindow();
		//UpdateOperationPointPos();
	}
}

void FArmyCornerBayWindow::SetWindowHoleDepth(float InDepth)
{
	if (BayWindowHoleDepth != InDepth)
	{
		BayWindowHoleDepth = InDepth;
		//UpateCornerBayWindow();
		//UpdateOperationPointPos();
	}
}

//void FArmyCornerBayWindow::SetWindowHeight(float InHeight)
//{
//	Height = InHeight;
//}

void FArmyCornerBayWindow::SetHeightToFloor(float InHeight)
{
	HeightToFloor = InHeight;
	if (HeightToFloor != InHeight)
	{
		HeightToFloor = InHeight;
		//UpateCornerBayWindow();
		//UpdateOperationPointPos();
	}
}

void FArmyCornerBayWindow::GenerateWindowModel(UWorld* world)
{
	if (WindowBoard == NULL)
	{
		WindowBoard = world->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass());

		if (bDefaultMaterial)
		{
			WindowBoard->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWindowBaseMaterial());
		}
		else
		{
			WindowBoard->SetMaterial(WindowStoneMat);
		}
		WindowGlass = world->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass());

		WindowGlass->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetGlassMaterial());
		WindowPillar = world->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass());
		WindowPillar->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWindowPillarMaterial());

		WindowWall = world->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass());
		WindowWall->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial());
	}

	TArray<FVector> WorldPoints;
	TArray<FVector> tempBottom = BottomWindowVerticeList;
	FArmyMath::ReversePointList(tempBottom);
	TArray<FVector> TempWordPoint = OutWindowVerticeList;
	TempWordPoint.Append(tempBottom);

	if (BottomWindowVerticeList.Num() == 7)
	{
		TArray<FVector> tempExtrudeBottomPoints;
		FVector leftVerticalInnear = LeftDirection.RotateAngleAxis(-90, FVector(0, 0, 1));
		FVector rightVerticalInear = RightDirection.RotateAngleAxis(90, FVector(0, 0, 1));
		FVector LeftStart = BottomWindowVerticeList[2] + leftVerticalInnear * WindowBottomExtrudeLength;
		FVector LeftEnd = LeftStart - LeftDirection * 10.0f;
		FVector RightStart = BottomWindowVerticeList[4] + rightVerticalInear * WindowBottomExtrudeLength;
		FVector RightEnd = RightStart - RightDirection * 10.f;

		FVector2D IntersectionPoint;
		FArmyMath::Line2DIntersection(FVector2D(LeftStart), FVector2D(LeftEnd), FVector2D(RightStart), FVector2D(RightEnd), IntersectionPoint);

		tempExtrudeBottomPoints.Append(OutWindowVerticeList);
		tempExtrudeBottomPoints.Push(BottomWindowVerticeList[6]);
		tempExtrudeBottomPoints.Push(BottomWindowVerticeList[5]);
		tempExtrudeBottomPoints.Push(BottomWindowVerticeList[4]);
		tempExtrudeBottomPoints.Push(BottomWindowVerticeList[4] + RightDirection * 6.0f);
		tempExtrudeBottomPoints.Push(BottomWindowVerticeList[4] + RightDirection * 6.0f + rightVerticalInear * WindowBottomExtrudeLength);

		tempExtrudeBottomPoints.Push(FVector(IntersectionPoint, 0.0f));
		tempExtrudeBottomPoints.Push(BottomWindowVerticeList[2] + LeftDirection * 6.0f + leftVerticalInnear * WindowBottomExtrudeLength);
		tempExtrudeBottomPoints.Push(BottomWindowVerticeList[2] + LeftDirection * 6.0f);
		tempExtrudeBottomPoints.Push(BottomWindowVerticeList[2]);
		tempExtrudeBottomPoints.Push(BottomWindowVerticeList[1]);
		tempExtrudeBottomPoints.Push(BottomWindowVerticeList[0]);
		for (int i = 0; i < tempExtrudeBottomPoints.Num(); i++)
		{
			WorldPoints.Push(tempExtrudeBottomPoints[i] + FVector(0, 0, HeightToFloor));
		}
	}

	TArray<FVector> TopPoints;
	int  number = TempWordPoint.Num();
	for (int i = 0; i < number; i++)
	{
		TopPoints.Push(TempWordPoint[i] + FVector(0, 0, Height - 5.0f + HeightToFloor));
	}
	WindowBoard->ResetMeshTriangles();
	WindowBoard->AddBoardPoints(WorldPoints, WindowBottomHeight);
	WindowBoard->AddBoardPoints(TopPoints, 5.0f);
	WindowBoard->UpdateAllVetexBufferIndexBuffer();

	GenerateWindowGlassAndPillars();
	CombineWindowWall();
}

void FArmyCornerBayWindow::DestroyWindowModel()
{
	if (WindowBoard->IsValidLowLevel())
	{
		WindowBoard->Destroy();
		WindowBoard = NULL;
	}
	if (WindowGlass->IsValidLowLevel())
	{
		WindowGlass->Destroy();
		WindowGlass = NULL;
	}
	if (WindowPillar->IsValidLowLevel())
	{
		WindowPillar->Destroy();
		WindowPillar = NULL;
	}
	if (WindowWall->IsValidLowLevel())
	{
		WindowWall->Destroy();
		WindowWall = NULL;
	}
}

void FArmyCornerBayWindow::SetWindowCornerType(int i)
{
	CornerWindowType = (ECornerBayWindowType)i;
	//UpateCornerBayWindow();
}

const void FArmyCornerBayWindow::GetAlonePoints(TArray< TSharedPtr<FArmyPoint> >& OutPoints)
{
	for (auto vertice : BottomWindowVerticeList)
	{
		OutPoints.Push(MakeShareable(new FArmyPoint(vertice)));
	}
	if (OutWindowVerticeList.Num() >= 2)
	{
		OutPoints.Push(MakeShareable(new FArmyPoint(OutWindowVerticeList[0])));
		OutPoints.Push(MakeShareable(new FArmyPoint(OutWindowVerticeList[OutWindowVerticeList.Num() - 1])));
	}
}

#define PillarWidth 1
#define PillarLength 1
#define SmallPillarWidth 2
#define SmallPillarLength 2
#define SingleWindowMaxLength 100
#define SingleWindowMinLength 80

void FArmyCornerBayWindow::Update()
{
	if (CornerWindowType == CornerAntiBoard)
	{
		UpateAntiBoardCornerBayWindow();
	}
	else if (CornerWindowType == CornerLeftBoard)
	{
		UpdateLeftBoardCornerWindow();
	}
	else if (CornerWindowType == CornerRightBoard)
	{
		UpdateRightBoardCornerWindow();
	}
	else if (CornerWindowType == CornerDoubleSideBoard)
	{
		UpdateDoubleBoardCornerWindow();
	}

    UpdateOperationPointPos();
}

void FArmyCornerBayWindow::UpdateStartPoint()
{
    float Angle = FMath::Acos(FVector::DotProduct(LeftDirection, RightDirection));
    float Length = BayWindowHoleDepth / FMath::Sin(Angle / 2.0f);
    FVector UpDir = ((LeftDirection + RightDirection) / 2).GetSafeNormal();
    FVector UpCenterPos = InnearCenterPos - Length*UpDir;
    FVector LeftVertical = LeftDirection.RotateAngleAxis(90, FVector(0, 0, 1));
    FVector First = GetStartPos() + LeftWallWidth / 2 * LeftVertical;
    FVector Seccond = First + LeftWalllOffset * LeftDirection;
    FVector Third = Seccond + LeftVertical * (BayWindowHoleDepth - LeftWallWidth);
    LeftWindowLength = (Third - UpCenterPos).Size();
    
    Update();
}

void FArmyCornerBayWindow::UpdateEndPoint()
{
    float Angle = FMath::Acos(FVector::DotProduct(LeftDirection, RightDirection));
    float Length = BayWindowHoleDepth / FMath::Sin(Angle / 2.0f);
    FVector UpDir = ((LeftDirection + RightDirection) / 2).GetSafeNormal();
    FVector UpCenterPos = InnearCenterPos - Length*UpDir;
    FVector RightVertical = RightDirection.RotateAngleAxis(-90, FVector(0, 0, 1));
    FVector First = GetEndPos() + RightWallWidth / 2 * RightVertical;
    FVector Second = First + RightWallOffset * RightDirection;
    FVector Third = Second + RightVertical * (BayWindowHoleDepth - RightWallWidth);
    RightWindowLength = (Third - UpCenterPos).Size();

    Update();
}

void FArmyCornerBayWindow::UpateAntiBoardCornerBayWindow()
{
	InnearWindowVerticeList.Empty();
	UpateInnearVertice(InnearWindowVerticeList);
	float leftOffset = LeftWalllOffset + FirstGlassWith;
	float rightOffset = RightWallOffset + FirstGlassWith;
	MiddleWindowVerticeList.Empty();
	UpdateMiddleOrOutVertice(MiddleWindowVerticeList, BayWindowHoleDepth + FirstGlassWith, leftOffset, rightOffset);
	leftOffset = LeftWalllOffset + FirstGlassWith + SecondGlassWidth;
	rightOffset = RightWallOffset + FirstGlassWith + SecondGlassWidth;
	OutWindowVerticeList.Empty();
	UpdateMiddleOrOutVertice(OutWindowVerticeList, BayWindowHoleDepth + FirstGlassWith + SecondGlassWidth, leftOffset, rightOffset);
	BottomWindowVerticeList.Empty();
	UpdateBottomVertice(BottomWindowVerticeList);
}


void FArmyCornerBayWindow::UpdateLeftBoardCornerWindow()
{
	LeftBoardInnearList.Empty();
	UpateInnearVertice(LeftBoardInnearList);
	LeftBoardInnearList.RemoveAt(0);
	LeftBoardLeftList.Empty();
	FVector leftStartPoint = LeftUpPointPos + LeftDirection * LeftWalllOffset;
	FVector LeftEndPoint = leftStartPoint + LeftDirection.RotateAngleAxis(90, FVector(0, 0, 1))* (BayWindowHoleDepth - LeftWallWidth + FirstGlassWith + SecondGlassWidth);
	LeftBoardLeftList.Push(leftStartPoint);
	LeftBoardLeftList.Push(LeftEndPoint);
	float leftOffset = LeftWalllOffset + FirstGlassWith;
	float rightOffset = RightWallOffset + FirstGlassWith;

	TArray<FVector> tempLeftBoardMiddle;
	tempLeftBoardMiddle.Empty();
	LeftBoardMiddleList.Empty();
	UpdateMiddleOrOutVertice(tempLeftBoardMiddle, BayWindowHoleDepth + FirstGlassWith, leftOffset, rightOffset);
	FVector LeftBoardStartPoint = leftStartPoint + LeftDirection.RotateAngleAxis(90, FVector(0, 0, 1))*(BayWindowHoleDepth - LeftWallWidth + FirstGlassWith);
	LeftBoardMiddleList.Push(LeftBoardStartPoint);
	for (int i = 2; i < tempLeftBoardMiddle.Num(); i++)
	{
		LeftBoardMiddleList.Push(tempLeftBoardMiddle[i]);
	}

	leftOffset = LeftWalllOffset + FirstGlassWith + SecondGlassWidth;
	rightOffset = RightWallOffset + FirstGlassWith + SecondGlassWidth;
	OutWindowVerticeList.Empty();
	UpdateMiddleOrOutVertice(OutWindowVerticeList, BayWindowHoleDepth + FirstGlassWith + SecondGlassWidth, leftOffset, rightOffset);
	BottomWindowVerticeList.Empty();
	UpdateBottomVertice(BottomWindowVerticeList);
}

void FArmyCornerBayWindow::UpdateRightBoardCornerWindow()
{
	RightBoardInnearList.Empty();
	UpateInnearVertice(RightBoardInnearList);
	RightBoardInnearList.RemoveAt(RightBoardInnearList.Num() - 1);

	float leftOffset = LeftWalllOffset + FirstGlassWith;
	float rightOffset = RightWallOffset + FirstGlassWith;
	RightBoardRightList.Empty();
	FVector startPoint = RightUpPointPos + RightWallOffset * RightDirection;
	FVector endPoint = startPoint + RightDirection.RotateAngleAxis(-90, FVector(0, 0, 1)) * (BayWindowHoleDepth - RightWallWidth + FirstGlassWith + SecondGlassWidth);
	RightBoardRightList.Push(startPoint);
	RightBoardRightList.Push(endPoint);

	FVector tempMiddleEnd = startPoint + RightDirection.RotateAngleAxis(-90, FVector(0, 0, 1))*(BayWindowHoleDepth - RightWallWidth + FirstGlassWith);

	TArray<FVector> tempRightBoardMiddle;
	tempRightBoardMiddle.Empty();
	RightBoardMiddleList.Empty();
	UpdateMiddleOrOutVertice(tempRightBoardMiddle, BayWindowHoleDepth + FirstGlassWith, leftOffset, rightOffset);
	for (int i = 0; i < tempRightBoardMiddle.Num() - 2; i++)
	{
		RightBoardMiddleList.Push(tempRightBoardMiddle[i]);
	}
	RightBoardMiddleList.Push(tempMiddleEnd);
	leftOffset = LeftWalllOffset + FirstGlassWith + SecondGlassWidth;
	rightOffset = RightWallOffset + FirstGlassWith + SecondGlassWidth;
	OutWindowVerticeList.Empty();
	UpdateMiddleOrOutVertice(OutWindowVerticeList, BayWindowHoleDepth + FirstGlassWith + SecondGlassWidth, leftOffset, rightOffset);
	BottomWindowVerticeList.Empty();
	UpdateBottomVertice(BottomWindowVerticeList);
}

void FArmyCornerBayWindow::UpdateDoubleBoardCornerWindow()
{
	TArray<FVector> tempDoubleInnearList;
	tempDoubleInnearList.Empty();
	DoubleBoardInnearList.Empty();
	UpateInnearVertice(tempDoubleInnearList);
	for (int i = 1; i < tempDoubleInnearList.Num() - 1; i++)
	{
		DoubleBoardInnearList.Push(tempDoubleInnearList[i]);
	}
	DoubleBoardLeftList.Empty();
	FVector leftStartPoint = LeftUpPointPos + LeftDirection * LeftWalllOffset;
	FVector LeftEndPoint = leftStartPoint + LeftDirection.RotateAngleAxis(90, FVector(0, 0, 1))* (BayWindowHoleDepth - LeftWallWidth + FirstGlassWith + SecondGlassWidth);
	DoubleBoardLeftList.Push(leftStartPoint);
	DoubleBoardLeftList.Push(LeftEndPoint);
	FVector doubleBoardStart = leftStartPoint + LeftDirection.RotateAngleAxis(90, FVector(0, 0, 1))*(BayWindowHoleDepth - LeftWallWidth + FirstGlassWith);
	DoubleBoardRightList.Empty();
	FVector rightstartPoint = RightUpPointPos + RightWallOffset * RightDirection;
	FVector rightendPoint = rightstartPoint + RightDirection.RotateAngleAxis(-90, FVector(0, 0, 1)) * (BayWindowHoleDepth - RightWallWidth + FirstGlassWith + SecondGlassWidth);
	DoubleBoardRightList.Push(rightstartPoint);
	DoubleBoardRightList.Push(rightendPoint);
	FVector doubleBoardEnd = rightstartPoint + RightDirection.RotateAngleAxis(-90, FVector(0, 0, 1))*(BayWindowHoleDepth - RightWallWidth + FirstGlassWith);
	float leftOffset = LeftWalllOffset + FirstGlassWith;
	float rightOffset = RightWallOffset + FirstGlassWith;
	TArray<FVector> tempDoubleBoard;
	tempDoubleBoard.Empty();
	DoubleBoardMiddleList.Empty();
	DoubleBoardMiddleList.Push(doubleBoardStart);
	UpdateMiddleOrOutVertice(tempDoubleBoard, BayWindowHoleDepth + FirstGlassWith, leftOffset, rightOffset);
	for (int i = 2; i < tempDoubleBoard.Num() - 2; i++)
	{
		DoubleBoardMiddleList.Push(tempDoubleBoard[i]);
	}
	DoubleBoardMiddleList.Push(doubleBoardEnd);
	leftOffset = LeftWalllOffset + FirstGlassWith + SecondGlassWidth;
	rightOffset = RightWallOffset + FirstGlassWith + SecondGlassWidth;
	OutWindowVerticeList.Empty();
	UpdateMiddleOrOutVertice(OutWindowVerticeList, BayWindowHoleDepth + FirstGlassWith + SecondGlassWidth, leftOffset, rightOffset);
	BottomWindowVerticeList.Empty();
	UpdateBottomVertice(BottomWindowVerticeList);
}

void FArmyCornerBayWindow::GenerateWindowGlassAndPillars()
{
	TArray<FVector> GlassPoints;
	TArray<FVector> UniMiddleWindowVerticlist;
	TArray < FVector> UniInnerWindowVerticeList;
	if (CornerWindowType == CornerAntiBoard)
	{
		UniInnerWindowVerticeList = InnearWindowVerticeList;
		UniMiddleWindowVerticlist = MiddleWindowVerticeList;
	}
	else if (CornerWindowType == CornerLeftBoard)
	{
		UniInnerWindowVerticeList = LeftBoardInnearList;
		UniMiddleWindowVerticlist = LeftBoardMiddleList;
	}
	else if (CornerWindowType == CornerRightBoard)
	{
		UniInnerWindowVerticeList = RightBoardInnearList;
		UniMiddleWindowVerticlist = RightBoardMiddleList;
	}
	else if (CornerWindowType == CornerDoubleSideBoard)
	{
		UniInnerWindowVerticeList = DoubleBoardInnearList;
		UniMiddleWindowVerticlist = DoubleBoardMiddleList;
	}

	TArray<FVector> tempMiddlePoints = UniMiddleWindowVerticlist;
	int tempInnderNum = UniInnerWindowVerticeList.Num();
	for (int i = tempInnderNum - 1; i >= 0; i--)
	{
		tempMiddlePoints.Push(UniInnerWindowVerticeList[i]);
	}
	int GlasspointNumber = tempMiddlePoints.Num();
	for (int i = 0; i < GlasspointNumber; i++)
	{
		GlassPoints.Push(tempMiddlePoints[i] + FVector(0, 0, HeightToFloor));
	}
	WindowGlass->ResetMeshTriangles();
	WindowGlass->AddBoardPoints(GlassPoints, Height);
	WindowGlass->UpdateAllVetexBufferIndexBuffer();


	int number = UniInnerWindowVerticeList.Num();
	int number1 = UniMiddleWindowVerticlist.Num();
	if (number != number1)
		return;

	WindowPillar->ResetMeshTriangles();
	TArray<FVector> PillarCenterPoints;
	for (int i = 0; i < number; i++)
	{
		FVector pos = UniInnerWindowVerticeList[i];
		FVector pos1 = UniMiddleWindowVerticlist[i];
		TArray<FVector> BoxPoint;
		BoxPoint.Push(pos1);
		BoxPoint.Push(pos);

		FBox box(BoxPoint);
		PillarCenterPoints.Push(box.GetCenter());
		float length = FMath::Abs((box.Max - box.Min).X) + PillarLength;
		float width = FMath::Abs((box.Max - box.Min).Y) + PillarWidth;
		FVector first = box.GetCenter() - FVector(1, 0, 0) * length / 2 + FVector(0, 1, 0)*width / 2 + FVector(0, 0, HeightToFloor);
		FVector second = first - FVector(0, 1, 0)*width;
		FVector third = second + FVector(1, 0, 0)*length;
		FVector fourth = third + FVector(0, 1, 0)*width;
		TArray<FVector> VerticalPillars;
		VerticalPillars.Push(first);
		VerticalPillars.Push(second);
		VerticalPillars.Push(third);
		VerticalPillars.Push(fourth);
		WindowPillar->AddBoardPoints(VerticalPillars, Height - 1.0f);
	}
	int MainPillarNumber = PillarCenterPoints.Num();
	for (int i = 1; i < MainPillarNumber; i++)
	{
		FVector tempsartPoint = PillarCenterPoints[i - 1];
		FVector tempEndPoint = PillarCenterPoints[i];
		float dist = (tempEndPoint - tempsartPoint).Size();
		int minNumberPillar = FMath::CeilToInt(dist / SingleWindowMaxLength);
		int maxNubmerPillar = FMath::CeilToInt(dist / SingleWindowMinLength);

		if (maxNubmerPillar >= minNumberPillar&&minNumberPillar >= 2)
		{
			for (int index = 1; index < minNumberPillar; index++)
			{
				TArray<FVector> MiddlePillarVertices;
				FVector tempPos = tempsartPoint + (tempEndPoint - tempsartPoint).GetSafeNormal()* (dist / minNumberPillar)*index;
				FVector first = tempPos - FVector(1, 0, 0)* SmallPillarLength / 2 + FVector(0, 1, 0)* SmallPillarWidth / 2 + FVector(0, 0, HeightToFloor);
				FVector second = first - FVector(0, 1, 0)*SmallPillarWidth;
				FVector third = second + FVector(1, 0, 0)*SmallPillarLength;
				FVector fourth = third + FVector(0, 1, 0)*SmallPillarWidth;
				MiddlePillarVertices.Push(first);
				MiddlePillarVertices.Push(second);
				MiddlePillarVertices.Push(third);
				MiddlePillarVertices.Push(fourth);
				WindowPillar->AddBoardPoints(MiddlePillarVertices, Height - 1.0f);
			}
		}

	}
	TArray<FVector> tempMiddlePillars;
	TArray<FVector> tempUpPillars;
	TArray<FVector> tempDownPillars;
	TArray<FVector> exTrudeMiddlePoint = FArmyMath::Extrude3D(UniMiddleWindowVerticlist, -0.2f, false);
	TArray<FVector> exTrueInnearPoint = FArmyMath::Extrude3D(UniInnerWindowVerticeList, 0.2f, true);
	FArmyMath::ReversePointList(exTrueInnearPoint);
	exTrudeMiddlePoint.Append(exTrueInnearPoint);
	for (int i = 0; i < exTrudeMiddlePoint.Num(); i++)
	{
		tempMiddlePillars.Push(exTrudeMiddlePoint[i] + FVector(0, 0, Height / 3.0f + HeightToFloor));
		tempDownPillars.Push(exTrudeMiddlePoint[i] + FVector(0, 0, HeightToFloor + WindowBottomHeight));
		tempUpPillars.Push(exTrudeMiddlePoint[i] + FVector(0, 0, HeightToFloor + Height - 9.0f));
	}
	WindowPillar->AddBoardPoints(tempMiddlePillars, 2.0f);
	WindowPillar->AddBoardPoints(tempDownPillars, 4.0f);
	WindowPillar->AddBoardPoints(tempUpPillars, 4.0f);
	WindowPillar->UpdateAllVetexBufferIndexBuffer();
}

void FArmyCornerBayWindow::CombineWindowWall()
{
	FVector LeftVerticalHorizontal = LeftDirection.RotateAngleAxis(-90, FVector(0, 0, 1));
	FVector RightVerticalHorizontal = RightDirection.RotateAngleAxis(90, FVector(0, 0, 1));
	TArray<FVector> LeftWall;
	TArray<FVector> RightWall;
	FVector first = LeftUpPointPos + FVector(0, 0, HeightToFloor);
	FVector second = first + LeftVerticalHorizontal * LeftWallWidth;
	FVector third = second + FVector(0, 0, Height);
	FVector fourth = first + FVector(0, 0, Height);
	LeftWall.Push(first);
	LeftWall.Push(second);
	LeftWall.Push(third);
	LeftWall.Push(fourth);
	first = RightUpPointPos + FVector(0, 0, HeightToFloor);
	second = first + RightVerticalHorizontal * RightWallWidth;
	third = second + FVector(0, 0, Height);
	fourth = first + FVector(0, 0, Height);
	RightWall.Push(first);
	RightWall.Push(second);
	RightWall.Push(third);
	RightWall.Push(fourth);
	TArray<FVector> LeftBordWall;
	TArray<FVector> RightBoardWall;
	FVector LeftFirst = LeftUpPointPos + LeftWalllOffset * LeftDirection + FVector(0, 0, HeightToFloor);
	FVector LeftSecond = LeftFirst + LeftDirection * 5.0f;
	FVector LeftThird = LeftSecond - LeftVerticalHorizontal * (BayWindowHoleDepth - LeftWallWidth + FirstGlassWith + SecondGlassWidth);
	FVector LeftFourth = LeftFirst - LeftVerticalHorizontal * (BayWindowHoleDepth - LeftWallWidth + FirstGlassWith + SecondGlassWidth);
	LeftBordWall.Push(LeftFirst);
	LeftBordWall.Push(LeftSecond);
	LeftBordWall.Push(LeftThird);
	LeftBordWall.Push(LeftFourth);
	WindowWall->ResetMeshTriangles();
	FVector RightFirst = RightUpPointPos + RightWallOffset * RightDirection + FVector(0, 0, HeightToFloor);
	FVector RightSecond = RightFirst - RightVerticalHorizontal* (BayWindowHoleDepth - RightWallWidth + FirstGlassWith + SecondGlassWidth);
	FVector RightThird = RightSecond + RightDirection * 5.0f;
	FVector RightFourth = RightFirst + RightDirection * 5.0f;
	RightBoardWall.Push(RightFirst);
	RightBoardWall.Push(RightSecond);
	RightBoardWall.Push(RightThird);
	RightBoardWall.Push(RightFourth);
	if (CornerWindowType == CornerLeftBoard)
	{
		WindowWall->AddBoardPoints(LeftBordWall, Height);
	}
	else if (CornerWindowType == CornerRightBoard)
	{
		WindowWall->AddBoardPoints(RightBoardWall, Height);
	}
	else if (CornerWindowType == CornerDoubleSideBoard)
	{
		WindowWall->AddBoardPoints(LeftBordWall, Height);
		WindowWall->AddBoardPoints(RightBoardWall, Height);

	}
	WindowWall->AddQuatGemometry(LeftWall);
	WindowWall->AddQuatGemometry(RightWall);
	WindowWall->UpdateAllVetexBufferIndexBuffer();
}

void FArmyCornerBayWindow::UpdateOperationPointPos()
{
	FVector leftVertical = LeftDirection.RotateAngleAxis(-90, FVector(0, 0, 1));
	FVector rightVertal = RightDirection.RotateAngleAxis(90, FVector(0, 0, 1));
	FVector tempStartPoint;
	FVector tempEndPoint;
	tempStartPoint = LeftUpPointPos + leftVertical * LeftWallWidth / 2.0f;
	tempEndPoint = RightUpPointPos + rightVertal * RightWallWidth / 2.0f;
	StartPoint->Pos = tempStartPoint;
	EndPoint->Pos = tempEndPoint;
	FVector tempPoint0 = tempStartPoint + LeftDirection* 10.0f;
	FVector tempPoint1 = tempEndPoint + RightDirection * 10.0f;
	FVector2D CornerPointPos;
	if (FArmyMath::Line2DIntersection(FVector2D(tempStartPoint), FVector2D(tempPoint0), FVector2D(tempEndPoint), FVector2D(tempPoint1), CornerPointPos))
	{
        Point->Pos = FVector(CornerPointPos, 0.0f);
	}
}

void FArmyCornerBayWindow::DeselectPoints()
{
	StartPoint->SetState(FArmyEditPoint::OPS_Normal);
	EndPoint->SetState(FArmyEditPoint::OPS_Normal);
    Point->SetState(FArmyEditPoint::OPS_Normal);
}

void FArmyCornerBayWindow::DrawAntiBayWindow(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	DrawWithVertices(OutWindowVerticeList, PDI, View, FLinearColor::White);
	DrawWithVertices(BottomWindowVerticeList, PDI, View, FLinearColor::Red);
	switch (CornerWindowType)
	{
	case FArmyCornerBayWindow::CornerAntiBoard:
	{
		DrawWithVertices(InnearWindowVerticeList, PDI, View, FLinearColor::Green);
		DrawWithVertices(MiddleWindowVerticeList, PDI, View, FLinearColor::Green);

	}
	break;
	case FArmyCornerBayWindow::CornerLeftBoard:
	{
		DrawWithVertices(LeftBoardInnearList, PDI, View, FLinearColor::Green);
		DrawWithVertices(LeftBoardMiddleList, PDI, View, FLinearColor::Green);
		DrawWithVertices(LeftBoardLeftList, PDI, View, FLinearColor::White);
	}
	break;
	case FArmyCornerBayWindow::CornerRightBoard:
	{
		DrawWithVertices(RightBoardInnearList, PDI, View, FLinearColor::Green);
		DrawWithVertices(RightBoardMiddleList, PDI, View, FLinearColor::Green);
		DrawWithVertices(RightBoardRightList, PDI, View, FLinearColor::White);
	}
	break;
	case FArmyCornerBayWindow::CornerDoubleSideBoard:
	{
		DrawWithVertices(DoubleBoardInnearList, PDI, View, FLinearColor::Green);
		DrawWithVertices(DoubleBoardMiddleList, PDI, View, FLinearColor::Green);
		DrawWithVertices(DoubleBoardLeftList, PDI, View, FLinearColor::White);
		DrawWithVertices(DoubleBoardRightList, PDI, View, FLinearColor::White);
	}
	break;
	default:
		break;
	}
}

void FArmyCornerBayWindow::DrawWithVertices(TArray<FVector> InVertices, FPrimitiveDrawInterface* PDI, const FSceneView* View, FLinearColor color)
{
	int number = InVertices.Num();
	FLinearColor drawColor = State == OS_Selected ? FLinearColor::Blue : color;
	for (int i = 1; i < number; i++)
	{
		PDI->DrawLine(InVertices[i - 1], InVertices[i], drawColor, 1);
	}
}

void FArmyCornerBayWindow::UpateInnearVertice(TArray<FVector>& Points)
{

	FVector tempLeft = LeftDirection.GetSafeNormal();
	FVector tempRight = RightDirection.GetSafeNormal();
	float angle = FMath::Acos(FVector::DotProduct(tempLeft, tempRight));
	float length = BayWindowHoleDepth / FMath::Sin(angle / 2.0f);
	FVector upDir = ((LeftDirection + RightDirection) / 2).GetSafeNormal();
	FVector UpCenterPos = InnearCenterPos - length*upDir;

	FVector leftUpPos = UpCenterPos + LeftWindowLength * LeftDirection;
	FVector LeftVertical = tempLeft.RotateAngleAxis(-90, FVector(0, 0, 1));
	FVector leftDownPos = leftUpPos + LeftVertical * (BayWindowHoleDepth - LeftWallWidth);

	LeftUpPointPos = leftDownPos - LeftWalllOffset * LeftDirection;

	FVector rightVertical = tempRight.RotateAngleAxis(90, FVector(0, 0, 1));
	FVector rightUpPos = UpCenterPos + RightWindowLength * RightDirection;
	FVector rightDownPos = rightUpPos + rightVertical*(BayWindowHoleDepth - RightWallWidth);

	RightUpPointPos = rightDownPos - RightWallOffset * RightDirection;

	Points.Push(leftDownPos);
	Points.Push(leftUpPos);
	Points.Push(UpCenterPos);
	Points.Push(rightUpPos);
	Points.Push(rightDownPos);
}

void FArmyCornerBayWindow::UpdateMiddleOrOutVertice(TArray<FVector>& Points, float depth, float InLeftOffset, float InRightOffset)
{
	FVector tempLeft = LeftDirection.GetSafeNormal();
	FVector tempRight = RightDirection.GetSafeNormal();
	FVector leftVertical = tempLeft.RotateAngleAxis(90, FVector(0, 0, 1));
	FVector righVertical = tempRight.RotateAngleAxis(-90, FVector(0, 0, 1));
	float angle = FMath::Acos(FVector::DotProduct(tempLeft, tempRight));
	float length = depth / FMath::Sin(angle / 2.0f);
	FVector upDir = ((LeftDirection + RightDirection) / 2.0f).GetSafeNormal();
	FVector UpCenterPos = InnearCenterPos - length*upDir;
	FVector tempLeftDowPos = LeftUpPointPos + LeftDirection*InLeftOffset;
	FVector tempLeftUpPos = (depth - LeftWallWidth)* leftVertical + tempLeftDowPos;
	FVector tempRightDownPos = RightUpPointPos + RightDirection* InRightOffset;
	FVector tempRightUpPos = (depth - RightWallWidth)*righVertical + tempRightDownPos;

	Points.Push(tempLeftDowPos);
	Points.Push(tempLeftUpPos);
	Points.Push(UpCenterPos);
	Points.Push(tempRightUpPos);
	Points.Push(tempRightDownPos);
}

void FArmyCornerBayWindow::UpdateBottomVertice(TArray<FVector>& Points)
{
	FVector tempLeft = LeftDirection.GetSafeNormal();
	FVector tempRight = RightDirection.GetSafeNormal();
	FVector leftVertical = tempLeft.RotateAngleAxis(90, FVector(0, 0, 1));
	FVector righVertical = tempRight.RotateAngleAxis(-90, FVector(0, 0, 1));
	FVector tempFirst = LeftUpPointPos + LeftDirection * LeftWalllOffset;
	FVector tempSecond = LeftUpPointPos;
	FVector tempThird = tempSecond - leftVertical * LeftWallWidth;
	FVector tempForth = InnearCenterPos;
	FVector tempSix = RightUpPointPos;
	FVector tempFive = tempSix - righVertical * RightWallWidth;
	FVector tempSeven = RightUpPointPos + RightDirection * RightWallOffset;

	Points.Push(tempFirst);
	Points.Push(tempSecond);
	Points.Push(tempThird);
	Points.Push(tempForth);
	Points.Push(tempFive);
	Points.Push(tempSix);
	Points.Push(tempSeven);
}

void FArmyCornerBayWindow::Generate(UWorld* InWorld)
{
	GenerateWindowModel(InWorld);
}