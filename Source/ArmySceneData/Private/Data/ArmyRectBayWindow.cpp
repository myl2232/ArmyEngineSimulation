#include "ArmyRectBayWindow.h"
#include "ArmyShapeBoardActor.h"
#include "ArmyEditPoint.h"
#include "ArmyEngineModule.h"
#include "ArmyMath.h"
#include "ArmySceneData.h"
#include "ArmyWindowActor.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "ResManager.h"
#include "ArmyResourceModule.h"
#include "ArmyRect.h"
#include "ArmyStyle.h"
#include "ArmyGameInstance.h"
#include "ArmyPolygon.h"
#include "ArmyWallLine.h"
#include "ArmyRulerLine.h"
#include "ArmyActorConstant.h"
#include "ArmyConstructionManager.h"
#include "ArmyConstructionQuantity.h"

#define DefaultBayWindowHoleDepth 60
#define DefaultLeftOffset 0
#define DefaultRightOffset 0
#define DefaultBayWindowHoleLength 150
#define DefaultBayWindowHoleUpLength 150
#define DefaultBayWindowOffsetHeight 50
#define FirstGlassWith 3.5
#define SecondGlassWidth 7
#define DefaultWindowHeight 190
#define DefaultOffsetGroundHeight 50

FArmyRectBayWindow::FArmyRectBayWindow() :
	WindowType(DoubleSideBoard),
	BayWindowHoleDepth(DefaultBayWindowHoleDepth),
	LeftOffset(DefaultLeftOffset),
	RightOffset(DefaultRightOffset),
	BayWindowHoleLength(DefaultBayWindowHoleLength),
	//HeightToFloor(DefaultOffsetGroundHeight),
	//Height(DefaultWindowHeight),
	WindowWall(NULL),
	HoleWindow(NULL),
	WindowBottomExtrudeLength(6),
	WindowBottomHeight(3),
	OutWindowLength(DefaultBayWindowHoleLength),


	MaskedPolygon(MakeShareable(new FArmyPolygon)),
	MaskedLine(MakeShareable(new FArmyLine())),

	LeftLine_1(MakeShareable(new FArmyLine())),
	LeftLine_2(MakeShareable(new FArmyLine())),
	LeftLine_3(MakeShareable(new FArmyLine())),
	LeftLine_4(MakeShareable(new FArmyLine())),

	RightLine_1(MakeShareable(new FArmyLine())),
	RightLine_2(MakeShareable(new FArmyLine())),
	RightLine_3(MakeShareable(new FArmyLine())),
	RightLine_4(MakeShareable(new FArmyLine())),

	TopLine_1(MakeShareable(new FArmyLine())),
	TopLine_2(MakeShareable(new FArmyLine())),
	TopLine_3(MakeShareable(new FArmyLine())),
	TopLine_4(MakeShareable(new FArmyLine())),

	LeftBoardLine_1(MakeShareable(new FArmyLine())),
	LeftBoardLine_2(MakeShareable(new FArmyLine())),
	LeftBoardLine_3(MakeShareable(new FArmyLine())),
	LeftBoardLine_4(MakeShareable(new FArmyLine())),

	RightBoardLine_1(MakeShareable(new FArmyLine())),
	RightBoardLine_2(MakeShareable(new FArmyLine())),
	RightBoardLine_3(MakeShareable(new FArmyLine())),
	RightBoardLine_4(MakeShareable(new FArmyLine())),

	LeftBreakLine(MakeShareable(new FArmyLine())),
	RightBreakLine(MakeShareable(new FArmyLine())),

	OtherModeLeftLine_1(MakeShareable(new FArmyLine())),
	OtherModeLeftLine_2(MakeShareable(new FArmyLine())),
	OtherModeLeftLine_3(MakeShareable(new FArmyLine())),
	OtherModeLeftLine_4(MakeShareable(new FArmyLine())),

	OtherModeRightLine_1(MakeShareable(new FArmyLine())),
	OtherModeRightLine_2(MakeShareable(new FArmyLine())),
	OtherModeRightLine_3(MakeShareable(new FArmyLine())),
	OtherModeRightLine_4(MakeShareable(new FArmyLine())),

	OtherModeTopLine_1(MakeShareable(new FArmyLine())),
	OtherModeTopLine_2(MakeShareable(new FArmyLine())),
	OtherModeTopLine_3(MakeShareable(new FArmyLine())),
	OtherModeTopLine_4(MakeShareable(new FArmyLine())),

	OtherModeLeftBoardLine_1(MakeShareable(new FArmyLine())),
	OtherModeLeftBoardLine_2(MakeShareable(new FArmyLine())),
	OtherModeLeftBoardLine_3(MakeShareable(new FArmyLine())),
	OtherModeLeftBoardLine_4(MakeShareable(new FArmyLine())),

	OtherModeRightBoardLine_1(MakeShareable(new FArmyLine())),
	OtherModeRightBoardLine_2(MakeShareable(new FArmyLine())),
	OtherModeRightBoardLine_3(MakeShareable(new FArmyLine())),
	OtherModeRightBoardLine_4(MakeShareable(new FArmyLine())),

	OtherModeLeftBreakLine(MakeShareable(new FArmyLine())),
	OtherModeRightBreakLine(MakeShareable(new FArmyLine())),
	OtherModeHalfLine(MakeShareable(new FArmyLine()))

{
	SetName(TEXT("标准飘窗")/* + GetUniqueID().ToString()*/);
	ArcAngle = FMath::DegreesToRadians(90.0f);
	Length = DefaultBayWindowHoleLength;
	ObjectType = OT_RectBayWindow;
	HeightToFloor = DefaultOffsetGroundHeight;
	Height = DefaultWindowHeight;

	//RectImagePanel = MakeShareable(new FArmyRect());
	RectImagePanel->SetBaseColor(FLinearColor::White/*GVC->GetBackgroundColor()*/);
	RectImagePanel->LineThickness = WALLLINEWIDTH + 0.1f;

	MaskedLine->SetBaseColor(GVC->GetBackgroundColor());
	MaskedLine->SetLineWidth(WALLLINEWIDTH + 0.1f);

	UMaterialInstanceDynamic *MI_MaksedColor = FArmyEngineModule::Get().GetEngineResource()->GetUnStageBaseColorMaterial();
	MI_MaksedColor->AddToRoot();
	MI_MaksedColor->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(FColor(0xFFE6E6E6)));
	MaskedPolygon->MaterialRenderProxy = MI_MaksedColor->GetRenderProxy(false);

	LeftLine_1->SetLineWidth(WALLLINEWIDTH + 0.1f);
	RightLine_1->SetLineWidth(WALLLINEWIDTH + 0.1f);
	TopLine_1->SetLineWidth(WALLLINEWIDTH + 0.1f);

	LeftLine_4->SetLineWidth(WALLLINEWIDTH + 0.1f);
	RightLine_4->SetLineWidth(WALLLINEWIDTH + 0.1f);
	TopLine_4->SetLineWidth(WALLLINEWIDTH + 0.1f);

	LeftBoardLine_1->SetLineWidth(WALLLINEWIDTH + 0.1f);
	LeftBoardLine_2->SetLineWidth(WALLLINEWIDTH + 0.1f);
	LeftBoardLine_3->SetLineWidth(WALLLINEWIDTH + 0.1f);
	LeftBoardLine_4->SetLineWidth(WALLLINEWIDTH + 0.1f);

	RightBoardLine_1->SetLineWidth(WALLLINEWIDTH + 0.1f);
	RightBoardLine_2->SetLineWidth(WALLLINEWIDTH + 0.1f);
	RightBoardLine_3->SetLineWidth(WALLLINEWIDTH + 0.1f);
	RightBoardLine_4->SetLineWidth(WALLLINEWIDTH + 0.1f);

	LeftBreakLine->SetLineWidth(WALLLINEWIDTH + 0.1f);
	RightBreakLine->SetLineWidth(WALLLINEWIDTH + 0.1f);

	OtherModeLeftLine_1->SetBaseColor(FLinearColor::Black);
	OtherModeLeftLine_2->SetBaseColor(FLinearColor::Black);
	OtherModeLeftLine_3->SetBaseColor(FLinearColor::Black);
	OtherModeLeftLine_4->SetBaseColor(FLinearColor::Black);

	OtherModeRightLine_1->SetBaseColor(FLinearColor::Black);
	OtherModeRightLine_2->SetBaseColor(FLinearColor::Black);
	OtherModeRightLine_3->SetBaseColor(FLinearColor::Black);
	OtherModeRightLine_4->SetBaseColor(FLinearColor::Black);

	OtherModeTopLine_1->SetBaseColor(FLinearColor::Black);
	OtherModeTopLine_2->SetBaseColor(FLinearColor::Black);
	OtherModeTopLine_3->SetBaseColor(FLinearColor::Black);
	OtherModeTopLine_4->SetBaseColor(FLinearColor::Black);

	OtherModeLeftBoardLine_1->SetBaseColor(FLinearColor::Black);
	OtherModeLeftBoardLine_2->SetBaseColor(FLinearColor::Black);
	OtherModeLeftBoardLine_3->SetBaseColor(FLinearColor::Black);
	OtherModeLeftBoardLine_4->SetBaseColor(FLinearColor::Black);

	OtherModeRightBoardLine_1->SetBaseColor(FLinearColor::Black);
	OtherModeRightBoardLine_2->SetBaseColor(FLinearColor::Black);
	OtherModeRightBoardLine_3->SetBaseColor(FLinearColor::Black);
	OtherModeRightBoardLine_4->SetBaseColor(FLinearColor::Black);

	OtherModeLeftBreakLine->SetBaseColor(FLinearColor::Black);
	OtherModeRightBreakLine->SetBaseColor(FLinearColor::Black);
	OtherModeHalfLine->SetBaseColor(FLinearColor::Black);

	OtherModeLeftLine_1->SetLineWidth(WALLLINEWIDTH + 0.1f);
	OtherModeTopLine_1->SetLineWidth(WALLLINEWIDTH + 0.1f);
	OtherModeRightLine_1->SetLineWidth(WALLLINEWIDTH + 0.1f);

	OtherModeLeftLine_4->SetLineWidth(WALLLINEWIDTH + 0.1f);
	OtherModeTopLine_4->SetLineWidth(WALLLINEWIDTH + 0.1f);
	OtherModeRightLine_4->SetLineWidth(WALLLINEWIDTH + 0.1f);

	OtherModeLeftBoardLine_1->SetLineWidth(WALLLINEWIDTH + 0.1f);
	OtherModeLeftBoardLine_2->SetLineWidth(WALLLINEWIDTH + 0.1f);
	OtherModeLeftBoardLine_3->SetLineWidth(WALLLINEWIDTH + 0.1f);
	OtherModeLeftBoardLine_4->SetLineWidth(WALLLINEWIDTH + 0.1f);

	OtherModeRightBoardLine_1->SetLineWidth(WALLLINEWIDTH + 0.1f);
	OtherModeRightBoardLine_2->SetLineWidth(WALLLINEWIDTH + 0.1f);
	OtherModeRightBoardLine_3->SetLineWidth(WALLLINEWIDTH + 0.1f);
	OtherModeRightBoardLine_4->SetLineWidth(WALLLINEWIDTH + 0.1f);

	OtherModeLeftBreakLine->SetLineWidth(WALLLINEWIDTH + 0.1f);
	OtherModeRightBreakLine->SetLineWidth(WALLLINEWIDTH + 0.1f);
	OtherModeHalfLine->SetLineWidth(WALLLINEWIDTH + 0.1f);

	DirectionPoint->SetState(FArmyPrimitive::OPS_Disable);

	SetState(OS_Selected);
}

FArmyRectBayWindow::FArmyRectBayWindow(FArmyRectBayWindow* Copy)
{

	WindowWall = Copy->WindowWall;
	HoleWindow = Copy->HoleWindow;
	WindowBottomExtrudeLength = Copy->WindowBottomExtrudeLength;
	WindowBottomHeight = Copy->WindowBottomHeight;
	AttachModelName = Copy->AttachModelName;

	RectImagePanel = MakeShareable(new FArmyRect(Copy->RectImagePanel.Get()));
	RectImagePanel->SetBaseColor(FLinearColor::White/*GVC->GetBackgroundColor()*/);
	RectImagePanel->LineThickness = WALLLINEWIDTH + 0.1f;

	MaskedLine = (MakeShareable(new FArmyLine(Copy->MaskedLine.Get())));
	MaskedLine->SetBaseColor(GVC->GetBackgroundColor());
	MaskedLine->SetLineWidth(WALLLINEWIDTH + 0.1f);

	MaskedPolygon = MakeShareable(new FArmyPolygon(Copy->MaskedPolygon.Get()));

	LeftLine_1 = (MakeShareable(new FArmyLine(Copy->LeftLine_1.Get())));
	LeftLine_2 = (MakeShareable(new FArmyLine(Copy->LeftLine_2.Get())));
	LeftLine_3 = (MakeShareable(new FArmyLine(Copy->LeftLine_3.Get())));
	LeftLine_4 = (MakeShareable(new FArmyLine(Copy->LeftLine_4.Get())));

	RightLine_1 = (MakeShareable(new FArmyLine(Copy->RightLine_1.Get())));
	RightLine_2 = (MakeShareable(new FArmyLine(Copy->RightLine_2.Get())));
	RightLine_3 = (MakeShareable(new FArmyLine(Copy->RightLine_3.Get())));
	RightLine_4 = (MakeShareable(new FArmyLine(Copy->RightLine_4.Get())));

	TopLine_1 = (MakeShareable(new FArmyLine(Copy->TopLine_1.Get())));
	TopLine_2 = (MakeShareable(new FArmyLine(Copy->TopLine_2.Get())));
	TopLine_3 = (MakeShareable(new FArmyLine(Copy->TopLine_3.Get())));
	TopLine_4 = (MakeShareable(new FArmyLine(Copy->TopLine_4.Get())));

	LeftBoardLine_1 = (MakeShareable(new FArmyLine(Copy->LeftBoardLine_1.Get())));
	LeftBoardLine_2 = (MakeShareable(new FArmyLine(Copy->LeftBoardLine_2.Get())));
	LeftBoardLine_3 = (MakeShareable(new FArmyLine(Copy->LeftBoardLine_3.Get())));
	LeftBoardLine_4 = (MakeShareable(new FArmyLine(Copy->LeftBoardLine_4.Get())));

	RightBoardLine_1 = (MakeShareable(new FArmyLine(Copy->RightBoardLine_1.Get())));
	RightBoardLine_2 = (MakeShareable(new FArmyLine(Copy->RightBoardLine_2.Get())));
	RightBoardLine_3 = (MakeShareable(new FArmyLine(Copy->RightBoardLine_3.Get())));
	RightBoardLine_4 = (MakeShareable(new FArmyLine(Copy->RightBoardLine_4.Get())));

	LeftBreakLine = (MakeShareable(new FArmyLine(Copy->LeftBreakLine.Get())));
	RightBreakLine = (MakeShareable(new FArmyLine(Copy->RightBreakLine.Get())));

	OtherModeLeftLine_1 = (MakeShareable(new FArmyLine(Copy->OtherModeLeftLine_1.Get())));
	OtherModeLeftLine_2 = (MakeShareable(new FArmyLine(Copy->OtherModeLeftLine_2.Get())));
	OtherModeLeftLine_3 = (MakeShareable(new FArmyLine(Copy->OtherModeLeftLine_3.Get())));
	OtherModeLeftLine_4 = (MakeShareable(new FArmyLine(Copy->OtherModeLeftLine_4.Get())));

	OtherModeRightLine_1 = (MakeShareable(new FArmyLine(Copy->OtherModeRightLine_1.Get())));
	OtherModeRightLine_2 = (MakeShareable(new FArmyLine(Copy->OtherModeRightLine_2.Get())));
	OtherModeRightLine_3 = (MakeShareable(new FArmyLine(Copy->OtherModeRightLine_3.Get())));
	OtherModeRightLine_4 = (MakeShareable(new FArmyLine(Copy->OtherModeRightLine_4.Get())));

	OtherModeTopLine_1 = (MakeShareable(new FArmyLine(Copy->OtherModeTopLine_1.Get())));
	OtherModeTopLine_2 = (MakeShareable(new FArmyLine(Copy->OtherModeTopLine_2.Get())));
	OtherModeTopLine_3 = (MakeShareable(new FArmyLine(Copy->OtherModeTopLine_3.Get())));
	OtherModeTopLine_4 = (MakeShareable(new FArmyLine(Copy->OtherModeTopLine_4.Get())));

	OtherModeLeftBoardLine_1 = (MakeShareable(new FArmyLine(Copy->OtherModeLeftBoardLine_1.Get())));
	OtherModeLeftBoardLine_2 = (MakeShareable(new FArmyLine(Copy->OtherModeLeftBoardLine_2.Get())));
	OtherModeLeftBoardLine_3 = (MakeShareable(new FArmyLine(Copy->OtherModeLeftBoardLine_3.Get())));
	OtherModeLeftBoardLine_4 = (MakeShareable(new FArmyLine(Copy->OtherModeLeftBoardLine_4.Get())));

	OtherModeRightBoardLine_1 = (MakeShareable(new FArmyLine(Copy->OtherModeRightBoardLine_1.Get())));
	OtherModeRightBoardLine_2 = (MakeShareable(new FArmyLine(Copy->OtherModeRightBoardLine_2.Get())));
	OtherModeRightBoardLine_3 = (MakeShareable(new FArmyLine(Copy->OtherModeRightBoardLine_3.Get())));
	OtherModeRightBoardLine_4 = (MakeShareable(new FArmyLine(Copy->OtherModeRightBoardLine_4.Get())));

	OtherModeLeftBreakLine = (MakeShareable(new FArmyLine(Copy->OtherModeLeftBreakLine.Get())));
	OtherModeRightBreakLine = (MakeShareable(new FArmyLine(Copy->OtherModeRightBreakLine.Get())));
	OtherModeHalfLine = (MakeShareable(new FArmyLine(Copy->OtherModeHalfLine.Get())));

	StartPoint = MakeShareable(new FArmyEditPoint(Copy->StartPoint.Get()));
	EndPoint = MakeShareable(new FArmyEditPoint(Copy->EndPoint.Get()));
	Point = MakeShareable(new FArmyEditPoint(Copy->Point.Get()));
	DirectionPoint = MakeShareable(new FArmyEditPoint(Copy->DirectionPoint.Get()));
	HeightToFloor = Copy->HeightToFloor;
	Height = Copy->Height;
	Direction = Copy->Direction;
	HorizontalDirection = Copy->HorizontalDirection;
	State = Copy->State;
	ArcAngle = Copy->ArcAngle;
	PropertyFlag = Copy->PropertyFlag;
	SetName(Copy->GetName());
	ObjectType = Copy->ObjectType;
	Length = Copy->Length;
	Width = Copy->Width;
	WindowType = Copy->WindowType;
	BayWindowHoleLength = Copy->BayWindowHoleLength;
	LeftOffset = Copy->LeftOffset;
	RightOffset = Copy->RightOffset;
	OutWindowLength = Copy->OutWindowLength;
	BayWindowHoleDepth = Copy->BayWindowHoleDepth;
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

	bRightOpen = Copy->bRightOpen;

	BrokenWallLeftLine = MakeShareable(new FArmyLine());
	BrokenWallRightLine = MakeShareable(new FArmyLine());

	ContentItem = Copy->ContentItem;

	Update();
}

FArmyRectBayWindow::~FArmyRectBayWindow()
{
}
void FArmyRectBayWindow::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyHardware::SerializeToJson(JsonWriter);

	JsonWriter->WriteValue("pos", GetPos().ToString());
	JsonWriter->WriteValue("direction", GetDirection().ToString());
	JsonWriter->WriteValue("length", GetLength());
	JsonWriter->WriteValue("width", GetWidth());
	//JsonWriter->WriteValue("OffsetGroundHeight", GetOffGoundDist());
	//JsonWriter->WriteValue("Height", GetWindowHeight());
	JsonWriter->WriteValue("bRightOpen", IsRightOpen());

	JsonWriter->WriteValue("BayWindowHoleLength", GetWindowHoleLength());
	JsonWriter->WriteValue("LeftOffset", GetWidowLeftDist());
	JsonWriter->WriteValue("RightOffset", GetWindowRightDist());
	JsonWriter->WriteValue("BayWindowHoleDepth", GetWindowHoleDepth());

	JsonWriter->WriteValue("WindowBottomHeight", GetWindowBottomBoardTickness());
	JsonWriter->WriteValue("WindowBottomExtrudeLength", GetWindowBottomBoardExtrudeLength());

	JsonWriter->WriteValue("OutWindowLength", GetOutWindowLength());

	JsonWriter->WriteValue("WindowType", GetWindowType());
	JsonWriter->WriteValue("BayType", (int32)GetType());

	JsonWriter->WriteValue("bGenerateWindowStone", bIfGenerateWindowStone);
	JsonWriter->WriteValue("SaleID", SaleID);
	//if (HoleWindow&& ContentItem.IsValid())
	//{
	//	JsonWriter->WriteObjectStart("ContenItem");
	//	ContentItem->SerializeToJson(JsonWriter);
	//	JsonWriter->WriteObjectEnd();
	//}


	if (HoleWindow)
	{
		UXRResourceManager * ResMg = FArmyResourceModule::Get().GetResourceManager();
		//TSharedPtr<FContentItemSpace::FContentItem> item = ResMg->GetContentItemFromID(HoleWindow->GetUniqueID());
		TSharedPtr<FContentItemSpace::FContentItem> ResultSynData = ResMg->GetContentItemFromID(HoleWindow->GetSynID());
		if (ResultSynData.IsValid())
		{
			ResultSynData->SerializeToJson(JsonWriter);
			GGI->DesignEditor->SaveFileList.AddUnique(ResultSynData->ID);
		}

		//@打扮家 XRLightmass 序列化LightMapID
		if (HoleWindow)
			LightMapID = HoleWindow->GetLightMapGUIDFromSMC();
		JsonWriter->WriteValue("LightMapID", LightMapID.ToString());
	}
	SERIALIZEREGISTERCLASS(JsonWriter, FArmyRectBayWindow)
}

void FArmyRectBayWindow::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyHardware::Deserialization(InJsonData);

	FVector Pos, TempDirection;
	Pos.InitFromString(InJsonData->GetStringField("pos"));
	TempDirection.InitFromString(InJsonData->GetStringField("direction"));
	SetPos(Pos);
	SetLength(InJsonData->GetNumberField("length"));
	SetWidth(InJsonData->GetNumberField("Width"));
	SetDirection(TempDirection);
	//SetWindowHeight(InJsonData->GetNumberField("Height"));
	SetRightOpen(InJsonData->GetBoolField("bRightOpen"));
	//SetOffGroundDist(InJsonData->GetNumberField("OffsetGroundHeight"));
	SetWindowHoleLength(InJsonData->GetNumberField("BayWindowHoleLength"));
	SetWindowLeftDist(InJsonData->GetNumberField("LeftOffset"));
	SetWindowRightDist(InJsonData->GetNumberField("RightOffset"));
	SetWindowHoleDepth(InJsonData->GetNumberField("BayWindowHoleDepth"));
	SetWindowBottomBoardThickness(InJsonData->GetNumberField("WindowBottomHeight"));
	SetWindowBottomBoardExtrudeLength(InJsonData->GetNumberField("WindowBottomExtrudeLength"));
	SetOutWindowLength(InJsonData->GetNumberField("OutWindowLength"));
	SetWindowType(InJsonData->GetNumberField("WindowType"));
	InJsonData->TryGetNumberField("SaleID", SaleID);
	//bIfGenerateWindowStone = InJsonData->GetBoolField("bGenerateWindowStone");
	InJsonData->TryGetBoolField("bGenerateWindowStone", bIfGenerateWindowStone);
	
	//TSharedPtr<FJsonObject> itemObject = InJsonData->GetObjectField("ContenItem");
	//if (itemObject.IsValid() && itemObject->Values.Num() > 0)
	//{
	//	//if (itemObject->Values.Num() > 0)
	//	//{
	//		ContentItem = MakeShareable(new FContentItemSpace::FContentItem());
	//		ContentItem->Deserialization(itemObject);
	//	//}
	//		UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
	//		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
	//		if (mat)
	//		{
	//			bDefaultMaterial = false;
	//			WindowStoneMat = mat;
	//			//HoleWindow->SetWindowBoardMaterial(mat);
	//		}
	//}

	
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
	

	SetState(OS_Normal);

	//@打扮家 XRLightmass 反序列化LightMapID
	FString LightGUIDStr = "";
	InJsonData->TryGetStringField("LightMapID", LightGUIDStr);
	FGuid::Parse(LightGUIDStr, LightMapID);

	bool bCapture = CaptureDoubleLine(Pos, E_LayoutModel);
}
void FArmyRectBayWindow::SetState(EObjectState InState)
{
	State = InState;
	switch (InState)
	{
	case OS_Normal:
		DrawColor = FLinearColor::White;
		DeselectPoints();
		break;
	case OS_Hovered:
	case OS_Selected:
		DrawColor = FLinearColor(FColor(0xFFFF9800));
		break;
	case OS_Disable:
		DrawColor = FLinearColor::Gray;
		break;
	default:
		break;
	}
	//RectImagePanel->SetBaseColor(DrawColor);
	if (BrokenWallLeftLine.IsValid())
	{
		BrokenWallLeftLine->SetBaseColor(DrawColor);
		BrokenWallRightLine->SetBaseColor(DrawColor);
	}

	LeftLine_1->SetBaseColor(DrawColor);
	LeftLine_2->SetBaseColor(DrawColor);
	LeftLine_3->SetBaseColor(DrawColor);
	LeftLine_4->SetBaseColor(DrawColor);

	RightLine_1->SetBaseColor(DrawColor);
	RightLine_2->SetBaseColor(DrawColor);
	RightLine_3->SetBaseColor(DrawColor);
	RightLine_4->SetBaseColor(DrawColor);

	TopLine_1->SetBaseColor(DrawColor);
	TopLine_2->SetBaseColor(DrawColor);
	TopLine_3->SetBaseColor(DrawColor);
	TopLine_4->SetBaseColor(DrawColor);

	LeftBoardLine_1->SetBaseColor(DrawColor);
	LeftBoardLine_2->SetBaseColor(DrawColor);
	LeftBoardLine_3->SetBaseColor(DrawColor);
	LeftBoardLine_4->SetBaseColor(DrawColor);

	RightBoardLine_1->SetBaseColor(DrawColor);
	RightBoardLine_2->SetBaseColor(DrawColor);
	RightBoardLine_3->SetBaseColor(DrawColor);
	RightBoardLine_4->SetBaseColor(DrawColor);

	LeftBreakLine->SetBaseColor(DrawColor);
	RightBreakLine->SetBaseColor(DrawColor);
}

void FArmyRectBayWindow::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if ((!FArmyObject::GetDrawModel(MODE_MODIFYADD) && InWallType == 1) ||//施工图不显示新增墙，并且该垭口在新增墙上时，不绘制
			(FArmyObject::GetDrawModel(MODE_DELETEWALLPOST) && !bGenerate3D))//拆除墙上带了窗戶，并且把门洞也拆除了，在拆除后的效果中不要显示门洞
		{
			return;
		}

		if (FArmyObject::GetDrawModel(MODE_TOPVIEW) || FArmyObject::GetDrawModel(MODE_CEILING))
		{
			MaskedPolygon->Draw(PDI, View);
			DrawTopViewWindow(PDI, View);
		}
		else
		{
			//DrawWindow(PDI, View);
			//RectImagePanel->Draw(PDI, View);
			FArmyHardware::Draw(PDI, View);
			DrawWindowData(PDI, View);
		}
	}
}

bool FArmyRectBayWindow::IsSelected(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		bShowDashLine = true;
		FVector LeftTop = GetPos() - Length / 2 * HorizontalDirection + (Width) / 2 * Direction;
		FVector RightBottom = GetPos() + Length / 2 * HorizontalDirection - (Width) / 2 * Direction;
		TArray<FVector> tempBottomBox = { LeftTop,RightBottom };
		FBox box = FBox(tempBottomBox);
		FBox box1 = FBox(OutWindowVerticeList);
		FVector TempPos = Pos;
		TempPos.Z += 2;
		bool TempBool = box1.IsInsideOrOn(TempPos) || box1.IsInsideOrOn(Pos);
		if (box.IsInsideOrOn(Pos) || TempBool || RectImagePanel->IsSelected(Pos, InViewportClient))
		{
			return true;
		}
		return false;
	}
	return false;
}

bool FArmyRectBayWindow::Hover(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		return false;
	}
	return false;
}

void FArmyRectBayWindow::ApplyTransform(const FTransform& Trans)
{

}

void FArmyRectBayWindow::Update()
{
	FArmyHardware::Update();

	if (Length < 30.f)
	{
		Length = 30.f;
	}
	SetWindowHoleLength(Length);

	StartPoint->Pos.Z = 2;
	EndPoint->Pos.Z = 2;

	RectImagePanel->Pos = GetStartPos() + Width / 2 * Direction;
	RectImagePanel->Pos.Z = 2.f;//为了让门挡住墙体线
	RectImagePanel->XDirection = HorizontalDirection;
	RectImagePanel->YDirection = -Direction;
	RectImagePanel->Width = Length;
	RectImagePanel->Height = Width;

	if (WindowType == AntiBoard)
	{
		UpdateAntiBoardWindow();
	}
	else if (WindowType == LeftBoard)
	{
		UpdateLeftBoardWindow();
	}
	else if (WindowType == RightBoard)
	{
		UpdateRightBoardWindow();
	}
	else if (WindowType == DoubleSideBoard)
	{
		UpdateDoubleBoardWindow();
	}

	UpdateWindowData();
}

TArray<FVector> FArmyRectBayWindow::GetBoundingBox()
{
	TArray<FVector> results;
	return results;
}

TArray<struct FLinesInfo> FArmyRectBayWindow::GetFacadeBox()
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

const FBox FArmyRectBayWindow::GetBounds()
{
	FBox Box(GetClipingBox());
	Box += (FBox(LeftBoardLeftList));
	Box += (FBox(RightBoardRightList));
	Box += (FBox(DoubleBoardLeftList));
	Box += (FBox(DoubleBoardRightList));
	return Box;
}

void FArmyRectBayWindow::GetCadLineArray(TArray<TPair<FVector, FVector>>& OutLineList) const
{
	FArmyHardware::GetCadLineArray(OutLineList);

	OutLineList.Append({
		TPair<FVector,FVector>(LeftBoardLine_1->GetStart(),LeftBoardLine_1->GetEnd()),
		TPair<FVector,FVector>(LeftBoardLine_2->GetStart(),LeftBoardLine_2->GetEnd()),
		TPair<FVector,FVector>(LeftBoardLine_3->GetStart(),LeftBoardLine_3->GetEnd()),
		TPair<FVector,FVector>(LeftBoardLine_4->GetStart(),LeftBoardLine_4->GetEnd()),

		TPair<FVector,FVector>(RightBoardLine_1->GetStart(),RightBoardLine_1->GetEnd()),
		TPair<FVector,FVector>(RightBoardLine_2->GetStart(),RightBoardLine_2->GetEnd()),
		TPair<FVector,FVector>(RightBoardLine_3->GetStart(),RightBoardLine_3->GetEnd()),
		TPair<FVector,FVector>(RightBoardLine_4->GetStart(),RightBoardLine_4->GetEnd()),

		TPair<FVector,FVector>(TopLine_1->GetStart(),TopLine_1->GetEnd()),
		TPair<FVector,FVector>(TopLine_2->GetStart(),TopLine_2->GetEnd()),
		TPair<FVector,FVector>(TopLine_3->GetStart(),TopLine_3->GetEnd()),
		TPair<FVector,FVector>(TopLine_4->GetStart(),TopLine_4->GetEnd()),

		TPair<FVector,FVector>(LeftBreakLine->GetStart(),LeftBreakLine->GetEnd()),
		TPair<FVector,FVector>(RightBreakLine->GetStart(),RightBreakLine->GetEnd()),
	});
}

void FArmyRectBayWindow::Destroy()
{
	if (WindowWall && WindowWall->IsValidLowLevel())
	{
		WindowWall->Destroy();
		WindowWall = nullptr;
	}
	if (HoleWindow && HoleWindow->IsValidLowLevel())
	{
		HoleWindow->Destroy();
		HoleWindow = nullptr;
	}
}

void FArmyRectBayWindow::SetDirection(const FVector & InDirection)
{
	if (!Direction.Equals(InDirection, KINDA_SMALL_NUMBER))
	{
		Direction = InDirection;
	}

	if (FirstRelatedRoom.IsValid())
	{
		if (FirstRelatedRoom->IsPointInRoom(LinkFirstLine->GetLineEditPoint()->GetPos() + 5 * Direction))
		{
			Direction *= -1;
		}
	}
	else if (SecondRelatedRoom.IsValid())
	{
		if (SecondRelatedRoom->IsPointInRoom(LinkSecondLine->GetLineEditPoint()->GetPos() + 5 * Direction))
		{
			Direction *= -1;
		}
	}
	else
	{
		TArray<TWeakPtr<FArmyObject>> RoomList;
		FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
		FArmySceneData::Get()->GetObjects(E_HomeModel, OT_OutRoom, RoomList);

		TSharedPtr<FArmyRoom> Room;
		for (auto It : RoomList)
		{
			Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
			TArray<TSharedPtr<FArmyLine>> RoomLines;
			if (Room.IsValid())
			{
				if (Room->IsPointInRoom(GetPos() + Width * Direction))
				{
					Direction *= -1;
					break;
				}
			}
		}
	}

	Update();
}

TArray<FVector> FArmyRectBayWindow::GetClipingBox()
{
	StartPoint->Pos.Z = 0;
	EndPoint->Pos.Z = 0;

	TArray<FVector> BoudingBox;
	FVector TempHorizontalDirection = FArmyMath::GetLineDirection(StartPoint->GetPos(), EndPoint->GetPos());
	FVector VerticalDirection = TempHorizontalDirection.RotateAngleAxis(90, FVector(0, 0, 1));
	FVector LeftTop = GetPos() - Length / 2 * TempHorizontalDirection + (Width + SubTracteThicknessError) / 2 * VerticalDirection;
	FVector LeftBottom = GetPos() - Length / 2 * TempHorizontalDirection - (Width + SubTracteThicknessError) / 2 * VerticalDirection;
	FVector RightBottom = GetPos() + Length / 2 * TempHorizontalDirection - (Width + SubTracteThicknessError) / 2 * VerticalDirection;
	FVector RightTop = GetPos() + Length / 2 * TempHorizontalDirection + (Width + SubTracteThicknessError) / 2 * VerticalDirection;
	//LeftTop.Z = LeftBottom.Z = RightBottom.Z = RightTop.Z = HeightToFloor;
	FVector offset = FVector(0, 0, HeightToFloor + Height / 2.0f);
	BoudingBox.Push((LeftBottom + offset));
	BoudingBox.Push((LeftTop + offset));
	BoudingBox.Push((RightTop + offset));
	BoudingBox.Push((RightBottom + offset));
	return BoudingBox;
}

void FArmyRectBayWindow::SetWindowHoleLength(float HoleLength)
{
	float TempLength = HoleLength;
	if (BayWindowHoleLength != HoleLength)
	{
		float TempValue = TempLength - BayWindowHoleLength;
		/*if (TempValue < 0)
		{
			TempValue = 0;
		}*/
		BayWindowHoleLength = TempLength;
		SetOutWindowLength(GetOutWindowLength() + TempValue);
		Update();
	}

}

void FArmyRectBayWindow::SetWindowHoleDepth(float Depth)
{
	float TempDepth = (Depth - Width);
	if (BayWindowHoleDepth != TempDepth)
	{
		if (TempDepth < 15)
		{
			TempDepth = 15;
		}
		BayWindowHoleDepth = TempDepth;
		Update();
	}
}

void FArmyRectBayWindow::SetWindowLeftDist(float leftDist)
{
	if (LeftOffset != leftDist)
	{
		LeftOffset = leftDist;
		Update();
	}
}

void FArmyRectBayWindow::SetWindowRightDist(float rightDist)
{
	if (RightOffset != rightDist)
	{
		RightOffset = rightDist;
		Update();
	}
}

void FArmyRectBayWindow::SetWindowType(int mType)
{
	WindowType = BayWindowType(mType);
	Update();
}

void FArmyRectBayWindow::SetOutWindowLength(float InOutLength)
{
	if (InOutLength < BayWindowHoleLength)
	{
		OutWindowLength = BayWindowHoleLength;
	}
	else
	{
		OutWindowLength = InOutLength;
		float TempDist = (OutWindowLength - BayWindowHoleLength) / 2;
		SetWindowLeftDist(TempDist);
		SetWindowRightDist(TempDist);
	}
}

#define PillarWidth 1
#define PillarLength 1
#define SmallPillarWidth 2
#define SmallPillarLength 2
#define SingleWindowMaxLength 100
#define SingleWindowMinLength 80

const void FArmyRectBayWindow::GetAlonePoints(TArray< TSharedPtr<FArmyPoint> >& OutPoints)
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

void FArmyRectBayWindow::GetWindowWallVertices(TArray<FBSPWallInfo>& OutWallVertices)
{
	if (WindowType == LeftBoard)
	{
		FBSPWallInfo LeftOrignalInfo, LeftFinishInfo;
		FVector leftFirst = LeftBoardLeftList[0] + FVector(0, 0, HeightToFloor);
		FVector leftSecond = leftFirst + FVector(0, 0, 1) * Height;
		FVector leftThird = LeftBoardLeftList[1] + FVector(0, 0, HeightToFloor) + FVector(0, 0, 1) * Height;
		FVector leftFourth = LeftBoardLeftList[1] + FVector(0, 0, HeightToFloor);
		LeftOrignalInfo.Normal = LeftFinishInfo.Normal = (leftFourth - leftFirst).RotateAngleAxis(-90, FVector(0, 0, 1)).GetSafeNormal();

		TArray<FVector> leftVertices = { leftFirst,leftSecond,leftThird,leftFourth };
		leftFirst = LeftBoardLeftList[0] - LeftOrignalInfo.Normal * (FArmySceneData::Get()->FinishWallThick + 0.2f) + FVector(0, 0, HeightToFloor);
		leftSecond = leftFirst + FVector(0, 0, 1) * Height;
		leftThird = LeftBoardLeftList[1] - LeftOrignalInfo.Normal * (FArmySceneData::Get()->FinishWallThick + 0.2f) + FVector(0, 0, HeightToFloor + Height);
		leftFourth = LeftBoardLeftList[1] - LeftOrignalInfo.Normal * (FArmySceneData::Get()->FinishWallThick + 0.2f) + FVector(0, 0, HeightToFloor);
		TArray<FVector> LeftFinishVerts = { leftFirst,leftSecond,leftThird,leftFourth };
		LeftOrignalInfo.PolyVertices.Add(leftVertices);
		LeftFinishInfo.PolyVertices.Add(LeftFinishVerts);
		LeftOrignalInfo.UniqueIdForRoomOrHardware = LeftFinishInfo.UniqueIdForRoomOrHardware = GetUniqueID().ToString();
		LeftOrignalInfo.GenerateFromObjectType = LeftFinishInfo.GenerateFromObjectType = ObjectType;
		LeftOrignalInfo.UniqueId = LeftFinishInfo.UniqueId = GetUniqueID().ToString() + TEXT("LeftBoard");
		OutWallVertices.Push(LeftOrignalInfo);
		OutWallVertices.Push(LeftFinishInfo);
	}
	else if (WindowType == RightBoard)
	{
		FBSPWallInfo RightOriginalInfo, RightFinishInfo;
		FVector rightFirst = RightBoardRightList[0] + FVector(0, 0, HeightToFloor);
		FVector rightSecond = rightFirst + FVector(0, 0, 1) * Height;
		FVector rightThird = RightBoardRightList[1] + FVector(0, 0, HeightToFloor) + FVector(0, 0, 1) * Height;
		FVector rightFourth = RightBoardRightList[1] + FVector(0, 0, HeightToFloor);
		RightOriginalInfo.Normal = RightFinishInfo.Normal = (rightFourth - rightFirst).RotateAngleAxis(90, FVector(0, 0, 1)).GetSafeNormal();
		TArray<FVector> rightVertices = { rightFirst,rightSecond,rightThird,rightFourth };
		rightFirst = RightBoardRightList[0] - RightOriginalInfo.Normal * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, HeightToFloor);
		rightSecond = rightFirst + FVector(0, 0, 1) * Height;
		rightThird = RightBoardRightList[1] - RightOriginalInfo.Normal * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, HeightToFloor + Height);
		rightFourth = RightBoardRightList[1] - RightOriginalInfo.Normal * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, HeightToFloor);
		TArray<FVector> RightFinishVerts = { rightFirst,rightSecond,rightThird,rightFourth };

		RightOriginalInfo.PolyVertices.Add(rightVertices);
		RightFinishInfo.PolyVertices.Add(RightFinishVerts);
		RightOriginalInfo.UniqueId = RightFinishInfo.UniqueId = GetUniqueID().ToString() + TEXT("RightBoard");
		RightOriginalInfo.UniqueIdForRoomOrHardware = RightFinishInfo.UniqueIdForRoomOrHardware = GetUniqueID().ToString();
		RightOriginalInfo.GenerateFromObjectType = RightFinishInfo.GenerateFromObjectType = ObjectType;
		OutWallVertices.Push(RightOriginalInfo);
		OutWallVertices.Push(RightFinishInfo);
	}
	else if (WindowType == DoubleSideBoard)
	{
		FBSPWallInfo leftInfo, leftFinishInfo, rightInfo, rightFinishInfo;
		FVector leftFirst = DoubleBoardLeftList[0] + FVector(0, 0, HeightToFloor);
		FVector leftSecond = leftFirst + FVector(0, 0, 1) * Height;
		FVector leftThird = DoubleBoardLeftList[1] + FVector(0, 0, HeightToFloor + Height);
		FVector leftFourth = DoubleBoardLeftList[1] + FVector(0, 0, HeightToFloor);
		leftInfo.Normal = leftFinishInfo.Normal = (leftFourth - leftFirst).RotateAngleAxis(-90, FVector(0, 0, 1)).GetSafeNormal();

		TArray<FVector> leftVertices = { leftFirst,leftSecond,leftThird,leftFourth };
		leftFirst = DoubleBoardLeftList[0] - leftInfo.Normal * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, HeightToFloor);
		leftSecond = leftFirst + FVector(0, 0, 1)*Height;
		leftThird = DoubleBoardLeftList[1] - leftInfo.Normal * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, HeightToFloor + Height);
		leftFourth = DoubleBoardLeftList[1] - leftInfo.Normal * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, HeightToFloor);
		TArray<FVector> leftFinishVerts = { leftFirst,leftSecond,leftThird,leftFourth };
		leftInfo.PolyVertices.Add(leftVertices);
		leftFinishInfo.PolyVertices.Add(leftFinishVerts);
		leftInfo.UniqueId = leftFinishInfo.UniqueId = GetUniqueID().ToString() + TEXT("LeftBoard");
		leftInfo.GenerateFromObjectType = leftFinishInfo.GenerateFromObjectType = ObjectType;
		leftInfo.UniqueIdForRoomOrHardware = leftFinishInfo.UniqueIdForRoomOrHardware = GetUniqueID().ToString();
		OutWallVertices.Push(leftInfo);
		OutWallVertices.Push(leftFinishInfo);

		FVector rightFirst = DoubleBoardRightList[0] + FVector(0, 0, HeightToFloor);
		FVector rightSecond = DoubleBoardRightList[0] + FVector(0, 0, HeightToFloor + Height);
		FVector rightThird = DoubleBoardRightList[1] + FVector(0, 0, HeightToFloor + Height);
		FVector rightFourth = DoubleBoardRightList[1] + FVector(0, 0, HeightToFloor);
		rightInfo.Normal = rightFinishInfo.Normal = (rightFourth - rightFirst).RotateAngleAxis(90, FVector(0, 0, 1)).GetSafeNormal();

		TArray<FVector> rightVertices = { rightFirst,rightSecond,rightThird,rightFourth };
		rightFirst = DoubleBoardRightList[0] - rightInfo.Normal * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, HeightToFloor);
		rightSecond = DoubleBoardRightList[0] - rightInfo.Normal * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, HeightToFloor + Height);
		rightThird = DoubleBoardRightList[1] - rightInfo.Normal  * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, HeightToFloor + Height);
		rightFourth = DoubleBoardRightList[1] - rightInfo.Normal * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, HeightToFloor);
		TArray<FVector> rightFinishVerts = { rightFirst,rightSecond,rightThird,rightFourth };
		rightInfo.PolyVertices.Add(rightVertices);
		rightFinishInfo.PolyVertices.Add(rightFinishVerts);
		rightInfo.UniqueId = rightFinishInfo.UniqueId = GetUniqueID().ToString() + TEXT("RightBoard");
		rightInfo.GenerateFromObjectType = rightFinishInfo.GenerateFromObjectType = ObjectType;
		rightInfo.UniqueIdForRoomOrHardware = rightFinishInfo.UniqueIdForRoomOrHardware = GetUniqueID().ToString();
		OutWallVertices.Push(rightInfo);
		OutWallVertices.Push(rightFinishInfo);
	}

	/** @欧石楠 飘窗顶面部分精确化*/
	TArray<FVector> TempPoints;	
	float leftDist = BayWindowHoleLength / 2 + LeftOffset;
	float rightDist = BayWindowHoleLength / 2 + RightOffset;
	float height = BayWindowHoleDepth;
	float upLength = (leftDist + rightDist - 2 * height / FMath::Tan(ArcAngle));
	CaculateVertice(TempPoints, leftDist, height, upLength);

	int	number = TempPoints.Num();
	TArray<FVector> TopPoints;	
	for (int i = 0; i < number; i++)
	{
		TopPoints.Push(TempPoints[i] + FVector(0, 0, Height + HeightToFloor));
	}
	TArray<FVector> TopFinishVerts;
	TopFinishVerts.AddUninitialized(number);
	for (int i = 0; i < number; i++)
	{
		TopFinishVerts[i] = TopPoints[i] + FVector(0, 0, -1) * FArmySceneData::Get()->FinishWallThick;
	}
	FBSPWallInfo topInfo, TopFinishInfo;
	topInfo.PolyVertices.Add(TopPoints);
	TopFinishInfo.PolyVertices.Add(TopFinishVerts);
	topInfo.Normal = TopFinishInfo.Normal = FVector(0, 0, -1);
	topInfo.UniqueId = TopFinishInfo.UniqueId = GetUniqueID().ToString() + TEXT("TopBoard");
	topInfo.GenerateFromObjectType = TopFinishInfo.GenerateFromObjectType = ObjectType;
	topInfo.UniqueIdForRoomOrHardware = TopFinishInfo.UniqueIdForRoomOrHardware = GetUniqueID().ToString();
	OutWallVertices.Push(topInfo);
	OutWallVertices.Push(TopFinishInfo);
}

void FArmyRectBayWindow::GetAttachHoleWallInfo(TArray<struct FBSPWallInfo>& OutAttachWallInfo)
{
	TArray<TSharedPtr<FArmyWallLine>> attWallLines;
	FArmySceneData::Get()->GetHardwareRelateWalllines(this->AsShared(), attWallLines);

    // @欧石楠 如果在放置窗户后修改墙体厚度，当厚度超过一定范围，会出现窗户关联不上墙体的情况
    if (attWallLines.Num() == 0)
    {
        return;
    }

	// 洞口左边面片信息
	FVector first = FVector(GetStartPos().X, GetStartPos().Y, 0.0f) + Width / 2 * Direction + FVector(0, 0, HeightToFloor + Height);
	FVector second = FVector(GetStartPos().X, GetStartPos().Y, 0.0f) + Width / 2 * Direction + FVector(0, 0, HeightToFloor);
	FVector third = FVector(GetStartPos().X, GetStartPos().Y, 0.0f) - Width / 2 * Direction + FVector(0, 0, HeightToFloor);
	FVector fourth = FVector(GetStartPos().X, GetStartPos().Y, 0.0f) - Width / 2 * Direction + FVector(0, 0, HeightToFloor + Height);
	float outRoomOffset = 0;
	if (attWallLines.Num() == 2)
	{
		outRoomOffset = FArmySceneData::Get()->FinishWallThick;
	}
	FVector InnerNormal = attWallLines[0]->GetNormal();

	FBSPWallInfo LeftOrignalWallInfo, LeftFinishWallInfo;
	TArray<FVector> LeftVerts = { first,second,third,fourth };
	first = FVector(GetStartPos().X, GetStartPos().Y, 0.0f) + HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, Height - FArmySceneData::Get()->FinishWallThick) + (Width / 2 + FArmySceneData::Get()->FinishWallThick)*InnerNormal + FVector(0, 0, HeightToFloor);
	second = FVector(GetStartPos().X, GetStartPos().Y, 0.0f) + HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, FArmySceneData::Get()->FinishWallThick) + (Width / 2 + FArmySceneData::Get()->FinishWallThick)*InnerNormal + FVector(0, 0, HeightToFloor);
	third = FVector(GetStartPos().X, GetStartPos().Y, 0.0f) + HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, FArmySceneData::Get()->FinishWallThick) + (Width / 2 + outRoomOffset) * (-InnerNormal) + FVector(0, 0, HeightToFloor);
	fourth = FVector(GetStartPos().X, GetStartPos().Y, 0.0f) + HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, Height - FArmySceneData::Get()->FinishWallThick) + (Width / 2 + outRoomOffset) * (-InnerNormal) + FVector(0, 0, HeightToFloor);
	TArray<FVector> LeftWallFinishVerts = { first,second,third,fourth };
	LeftOrignalWallInfo.PolyVertices.Push(LeftVerts);
	LeftFinishWallInfo.PolyVertices.Push(LeftWallFinishVerts);
	LeftOrignalWallInfo.Normal = LeftFinishWallInfo.Normal = -HorizontalDirection;
	LeftOrignalWallInfo.UniqueIdForRoomOrHardware = LeftFinishWallInfo.UniqueIdForRoomOrHardware = GetUniqueID().ToString();
	LeftOrignalWallInfo.UniqueId = LeftFinishWallInfo.UniqueId = GetUniqueID().ToString() + TEXT("LeftWall");
	LeftOrignalWallInfo.GenerateFromObjectType = LeftFinishWallInfo.GenerateFromObjectType = ObjectType;
	// 洞口右边面片信息
	first = FVector(GetEndPos().X, GetEndPos().Y, 0.0f) + Width / 2 * Direction + FVector(0, 0, HeightToFloor + Height);
	second = FVector(GetEndPos().X, GetEndPos().Y, 0.0f) + Width / 2 * Direction + FVector(0, 0, HeightToFloor);
	third = FVector(GetEndPos().X, GetEndPos().Y, 0.0f) - Width / 2 * Direction + FVector(0, 0, HeightToFloor);
	fourth = FVector(GetEndPos().X, GetEndPos().Y, 0.0f) - Width / 2 * Direction + FVector(0, 0, HeightToFloor + Height);
	TArray<FVector> RightVerts = { first,second,third,fourth };
	first = FVector(GetEndPos().X, GetEndPos().Y, 0.0f) - HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, Height - FArmySceneData::Get()->FinishWallThick) + (Width / 2 + FArmySceneData::Get()->FinishWallThick)*InnerNormal + FVector(0, 0, HeightToFloor);
	second = FVector(GetEndPos().X, GetEndPos().Y, 0.0f) - HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, FArmySceneData::Get()->FinishWallThick) + (Width / 2 + FArmySceneData::Get()->FinishWallThick)*InnerNormal + FVector(0, 0, HeightToFloor);
	third = FVector(GetEndPos().X, GetEndPos().Y, 0.0f) - HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, FArmySceneData::Get()->FinishWallThick) + (Width / 2 + outRoomOffset) * (-InnerNormal) + FVector(0, 0, HeightToFloor);
	fourth = FVector(GetEndPos().X, GetEndPos().Y, 0.0f) - HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, Height - FArmySceneData::Get()->FinishWallThick) + (Width / 2 + outRoomOffset) * (-InnerNormal) + FVector(0, 0, HeightToFloor);
	TArray<FVector> RighFinishVerts = { first,second,third,fourth };
	FBSPWallInfo RightOrignalWallInfo, RightFinishWallInfo;
	RightOrignalWallInfo.PolyVertices.Add(RightVerts);
	RightFinishWallInfo.PolyVertices.Add(RighFinishVerts);
	RightOrignalWallInfo.Normal = RightFinishWallInfo.Normal = HorizontalDirection;
	RightOrignalWallInfo.UniqueIdForRoomOrHardware = RightFinishWallInfo.UniqueIdForRoomOrHardware = GetUniqueID().ToString();
	RightOrignalWallInfo.UniqueId = RightFinishWallInfo.UniqueId = GetUniqueID().ToString() + TEXT("RightWall");
	RightOrignalWallInfo.GenerateFromObjectType = RightFinishWallInfo.GenerateFromObjectType = ObjectType;
	// 洞地信息		
	first = FVector(GetStartPos().X, GetStartPos().Y, 0.0f) + Width / 2 * Direction + FVector(0, 0, HeightToFloor);
	second = FVector(GetEndPos().X, GetEndPos().Y, 0.0f) + Width / 2 * Direction + FVector(0, 0, HeightToFloor);
	third = FVector(GetEndPos().X, GetEndPos().Y, 0.0f) - Width / 2 * Direction + FVector(0, 0, HeightToFloor);
	fourth = FVector(GetStartPos().X, GetStartPos().Y, 0.0f) - Width / 2 * Direction + FVector(0, 0, HeightToFloor);
	TArray<FVector> BottomVerts;
	BottomVerts = { first,second,third,fourth };
	TArray<FVector> BottomFinishVerts;
	first = FVector(GetStartPos().X, GetStartPos().Y, 0.0f) + HorizontalDirection * FArmySceneData::Get()->FinishWallThick + (Width / 2.0f + FArmySceneData::Get()->FinishWallThick)*InnerNormal + FVector(0, 0, FArmySceneData::Get()->FinishWallThick + HeightToFloor);
	second = FVector(GetEndPos().X, GetEndPos().Y, 0.0f) - HorizontalDirection * FArmySceneData::Get()->FinishWallThick + (Width / 2.0f + FArmySceneData::Get()->FinishWallThick) * InnerNormal + FVector(0, 0, FArmySceneData::Get()->FinishWallThick + HeightToFloor);
	third = FVector(GetEndPos().X, GetEndPos().Y, 0.0f) - HorizontalDirection * FArmySceneData::Get()->FinishWallThick - (Width / 2.0f + FArmySceneData::Get()->FinishWallThick) * InnerNormal + FVector(0, 0, FArmySceneData::Get()->FinishWallThick + HeightToFloor);
	fourth = FVector(GetStartPos().X, GetStartPos().Y, 0.0f) + HorizontalDirection * FArmySceneData::Get()->FinishWallThick - (Width / 2.0f + FArmySceneData::Get()->FinishWallThick)*InnerNormal + FVector(0, 0, FArmySceneData::Get()->FinishWallThick + HeightToFloor);
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

	
	//@郭子阳 生成被窗台石盖住的面，不管有没有窗台石,因为被遮住也存在原始墙面施工项
	{
		TArray<FVector> TempPoints;
		TArray<FVector> OutSideBottomPoints;
		float leftDist = BayWindowHoleLength / 2 + LeftOffset;
		float rightDist = BayWindowHoleLength / 2 + RightOffset;
		float height = BayWindowHoleDepth;
		float upLength = (leftDist + rightDist - 2 * height / FMath::Tan(ArcAngle));
		CaculateVertice(TempPoints, leftDist, height, upLength);

		int	number = TempPoints.Num();
		//for (int i = number-1; i >=0; --i)
		for (int i = 0; i <number; ++i)
		{
			OutSideBottomPoints.Push(TempPoints[i] + FVector(0, 0, HeightToFloor));
		}

		TArray<FVector> OutSideBottomFinishPoints;
		OutSideBottomFinishPoints.AddUninitialized(number);
		for (int i = 0; i < number; i++)
		{
			OutSideBottomFinishPoints[i] = OutSideBottomPoints[i] + FVector(0, 0, 1) * FArmySceneData::Get()->FinishWallThick;
		}

		FBSPWallInfo OutSideInfo, OutSideFinishInfo;
		OutSideInfo.PolyVertices.Add(OutSideBottomPoints);
		OutSideFinishInfo.PolyVertices.Add(OutSideBottomFinishPoints);
		OutSideInfo.Normal = OutSideFinishInfo.Normal = FVector(0, 0, 1);
		OutSideInfo.UniqueId = OutSideFinishInfo.UniqueId = GetUniqueID().ToString() + TEXT("OutSideBottomWall");
		OutSideInfo.GenerateFromObjectType = OutSideFinishInfo.GenerateFromObjectType = ObjectType;
		OutSideInfo.UniqueIdForRoomOrHardware = OutSideFinishInfo.UniqueIdForRoomOrHardware = GetUniqueID().ToString();
		OutAttachWallInfo.Push(OutSideInfo);
		OutAttachWallInfo.Push(OutSideFinishInfo);
	}	
}

void FArmyRectBayWindow::SetIfGenerateWindowStone(bool bValue)
{
	bIfGenerateWindowStone = bValue;

	FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
}

void FArmyRectBayWindow::UpdateWindowData()
{
	float IntervalDist = 8.f;
	FVector TempPos = GetPos();
	TempPos.Z = 3.f;
	MaskedLine->SetStart(TempPos - HorizontalDirection * Length / 2.f + Direction * Width / 2.f);
	MaskedLine->SetEnd(TempPos + HorizontalDirection * Length / 2.f + Direction * Width / 2.f);

	LeftLine_1->SetStart(TempPos - HorizontalDirection * OutWindowLength / 2.f + Direction * Width / 2.f);
	LeftLine_1->SetEnd(TempPos - HorizontalDirection * OutWindowLength / 2.f + Direction * (Width / 2.f + BayWindowHoleDepth));
	LeftLine_2->SetStart(LeftLine_1->GetStart() - HorizontalDirection * IntervalDist);
	LeftLine_2->SetEnd(LeftLine_1->GetEnd() - HorizontalDirection * IntervalDist + Direction * IntervalDist);
	LeftLine_3->SetStart(LeftLine_2->GetStart() - HorizontalDirection * IntervalDist);
	LeftLine_3->SetEnd(LeftLine_2->GetEnd() - HorizontalDirection * IntervalDist + Direction * IntervalDist);
	LeftLine_4->SetStart(LeftLine_3->GetStart() - HorizontalDirection * IntervalDist);
	LeftLine_4->SetEnd(LeftLine_3->GetEnd() - HorizontalDirection * IntervalDist + Direction * IntervalDist);

	RightLine_1->SetStart(TempPos + HorizontalDirection * OutWindowLength / 2.f + Direction * Width / 2.f);
	RightLine_1->SetEnd(TempPos + HorizontalDirection * OutWindowLength / 2.f + Direction * (Width / 2.f + BayWindowHoleDepth));
	RightLine_2->SetStart(RightLine_1->GetStart() + HorizontalDirection * IntervalDist);
	RightLine_2->SetEnd(RightLine_1->GetEnd() + HorizontalDirection * IntervalDist + Direction * IntervalDist);
	RightLine_3->SetStart(RightLine_2->GetStart() + HorizontalDirection * IntervalDist);
	RightLine_3->SetEnd(RightLine_2->GetEnd() + HorizontalDirection * IntervalDist + Direction * IntervalDist);
	RightLine_4->SetStart(RightLine_3->GetStart() + HorizontalDirection * IntervalDist);
	RightLine_4->SetEnd(RightLine_3->GetEnd() + HorizontalDirection * IntervalDist + Direction * IntervalDist);

	TopLine_1->SetStart(TempPos - HorizontalDirection * OutWindowLength / 2.f + Direction * (Width / 2.f + BayWindowHoleDepth));
	TopLine_2->SetStart(TopLine_1->GetStart() - HorizontalDirection * IntervalDist + Direction * IntervalDist);
	TopLine_3->SetStart(TopLine_2->GetStart() - HorizontalDirection * IntervalDist + Direction * IntervalDist);
	TopLine_4->SetStart(TopLine_3->GetStart() - HorizontalDirection * IntervalDist + Direction * IntervalDist);
	TopLine_1->SetEnd(TempPos + HorizontalDirection * OutWindowLength / 2.f + Direction * (Width / 2.f + BayWindowHoleDepth));
	TopLine_2->SetEnd(TopLine_1->GetEnd() + HorizontalDirection * IntervalDist + Direction * IntervalDist);
	TopLine_3->SetEnd(TopLine_2->GetEnd() + HorizontalDirection * IntervalDist + Direction * IntervalDist);
	TopLine_4->SetEnd(TopLine_3->GetEnd() + HorizontalDirection * IntervalDist + Direction * IntervalDist);

	if (WindowType == LeftBoard || WindowType == DoubleSideBoard)
	{
		LeftBoardLine_1->SetStart(LeftLine_4->GetStart());
		LeftBoardLine_1->SetEnd(LeftLine_4->GetEnd());

		LeftBoardLine_2->SetStart(TopLine_4->GetStart());

		TopLine_1->SetStart(TempPos - HorizontalDirection * OutWindowLength / 2.f + Direction * (Width / 2.f + BayWindowHoleDepth));
		TopLine_2->SetStart(TopLine_1->GetStart() + Direction * IntervalDist);
		TopLine_3->SetStart(TopLine_2->GetStart() + Direction * IntervalDist);
		TopLine_4->SetStart(TopLine_3->GetStart() + Direction * IntervalDist);

		LeftBoardLine_2->SetEnd(TopLine_4->GetStart());

		LeftBoardLine_3->SetStart(TopLine_4->GetStart());
		LeftBoardLine_3->SetEnd(TopLine_1->GetStart());

		LeftBoardLine_4->SetStart(LeftLine_1->GetEnd());
		LeftBoardLine_4->SetEnd(LeftLine_1->GetStart());
	}

	if (WindowType == RightBoard || WindowType == DoubleSideBoard)
	{
		RightBoardLine_1->SetStart(RightLine_4->GetStart());
		RightBoardLine_1->SetEnd(RightLine_4->GetEnd());

		RightBoardLine_2->SetStart(RightLine_4->GetEnd());

		TopLine_1->SetEnd(TempPos + HorizontalDirection * OutWindowLength / 2.f + Direction * (Width / 2.f + BayWindowHoleDepth));
		TopLine_2->SetEnd(TopLine_1->GetEnd() + Direction * IntervalDist);
		TopLine_3->SetEnd(TopLine_2->GetEnd() + Direction * IntervalDist);
		TopLine_4->SetEnd(TopLine_3->GetEnd() + Direction * IntervalDist);

		RightBoardLine_2->SetEnd(TopLine_4->GetEnd());

		RightBoardLine_3->SetStart(TopLine_4->GetEnd());
		RightBoardLine_3->SetEnd(TopLine_1->GetEnd());

		RightBoardLine_4->SetStart(RightLine_1->GetEnd());
		RightBoardLine_4->SetEnd(RightLine_1->GetStart());
	}

	LeftBreakLine->SetStart(TempPos - HorizontalDirection * Length / 2.f - Direction * (Width / 2.f));
	LeftBreakLine->SetEnd(TempPos - HorizontalDirection * Length / 2.f + Direction * (Width / 2.f));
	LeftBreakLine->SetZForStartAndEnd(FArmySceneData::WallHeight + 23.f);

	RightBreakLine->SetStart(TempPos + HorizontalDirection * Length / 2.f - Direction * (Width / 2.f));
	RightBreakLine->SetEnd(TempPos + HorizontalDirection * Length / 2.f + Direction * (Width / 2.f));
	RightBreakLine->SetZForStartAndEnd(FArmySceneData::WallHeight + 23.f);

	OtherModeHalfLine->SetStart(TempPos - HorizontalDirection * Length / 2.f - Direction * (Width / 2.f));
	OtherModeHalfLine->SetEnd(TempPos + HorizontalDirection * Length / 2.f - Direction * (Width / 2.f));
	OtherModeHalfLine->SetZForStartAndEnd(FArmySceneData::WallHeight + 23.f);

	OtherModeLeftBreakLine->SetStart(OtherModeHalfLine->GetStart());
	OtherModeLeftBreakLine->SetEnd(TempPos - HorizontalDirection * Length / 2.f + Direction * (Width / 2.f));
	OtherModeLeftBreakLine->SetZForStartAndEnd(FArmySceneData::WallHeight + 23.f);

	OtherModeRightBreakLine->SetStart(OtherModeHalfLine->GetEnd());
	OtherModeRightBreakLine->SetEnd(TempPos + HorizontalDirection * Length / 2.f + Direction * (Width / 2.f));
	OtherModeRightBreakLine->SetZForStartAndEnd(FArmySceneData::WallHeight + 23.f);

	TArray<FVector> Points;
	FVector V0;
	FVector V1;
	FVector V2;
	FVector V3;
	FVector V4;
	FVector V5;
	FVector V6;
	FVector V7;

	V0 = TempPos - HorizontalDirection * Length / 2.f - Direction * Width / 2.f;
	V1 = TempPos - HorizontalDirection * Length / 2.f + Direction * Width / 2.f;
	V2 = LeftLine_4->GetStart();
	V3 = LeftLine_4->GetEnd();
	V4 = RightLine_4->GetEnd();
	V5 = RightLine_4->GetStart();
	V6 = TempPos + HorizontalDirection * Length / 2.f + Direction * Width / 2.f;
	V7 = TempPos + HorizontalDirection * Length / 2.f - Direction * Width / 2.f;

	Points.Add(V0);
	Points.Add(V1);
	Points.Add(V2);
	Points.Add(V3);
	Points.Add(V4);
	Points.Add(V5);
	Points.Add(V6);
	Points.Add(V7);
	MaskedPolygon->SetVertices(Points);
	MaskedPolygon->SetPolygonOffset(FArmySceneData::WallHeight + 22.f);

	float offset = 23.f;

	//--left line	
	OtherModeLeftLine_1->SetStart(LeftLine_1->GetStart());
	OtherModeLeftLine_1->SetEnd(LeftLine_1->GetEnd());
	OtherModeLeftLine_1->SetZForStartAndEnd(FArmySceneData::WallHeight + offset);

	OtherModeLeftLine_2->SetStart(LeftLine_2->GetStart());
	OtherModeLeftLine_2->SetEnd(LeftLine_2->GetEnd());
	OtherModeLeftLine_2->SetZForStartAndEnd(FArmySceneData::WallHeight + offset);

	OtherModeLeftLine_3->SetStart(LeftLine_3->GetStart());
	OtherModeLeftLine_3->SetEnd(LeftLine_3->GetEnd());
	OtherModeLeftLine_3->SetZForStartAndEnd(FArmySceneData::WallHeight + offset);

	OtherModeLeftLine_4->SetStart(LeftLine_4->GetStart());
	OtherModeLeftLine_4->SetEnd(LeftLine_4->GetEnd());
	OtherModeLeftLine_4->SetZForStartAndEnd(FArmySceneData::WallHeight + offset);

	//-- right line	
	OtherModeRightLine_1->SetStart(RightLine_1->GetStart());
	OtherModeRightLine_1->SetEnd(RightLine_1->GetEnd());
	OtherModeRightLine_1->SetZForStartAndEnd(FArmySceneData::WallHeight + offset);

	OtherModeRightLine_2->SetStart(RightLine_2->GetStart());
	OtherModeRightLine_2->SetEnd(RightLine_2->GetEnd());
	OtherModeRightLine_2->SetZForStartAndEnd(FArmySceneData::WallHeight + offset);

	OtherModeRightLine_3->SetStart(RightLine_3->GetStart());
	OtherModeRightLine_3->SetEnd(RightLine_3->GetEnd());
	OtherModeRightLine_3->SetZForStartAndEnd(FArmySceneData::WallHeight + offset);

	OtherModeRightLine_4->SetStart(RightLine_4->GetStart());
	OtherModeRightLine_4->SetEnd(RightLine_4->GetEnd());
	OtherModeRightLine_4->SetZForStartAndEnd(FArmySceneData::WallHeight + offset);

	//-- top line	
	OtherModeTopLine_1->SetStart(TopLine_1->GetStart());
	OtherModeTopLine_1->SetEnd(TopLine_1->GetEnd());
	OtherModeTopLine_1->SetZForStartAndEnd(FArmySceneData::WallHeight + offset);

	OtherModeTopLine_2->SetStart(TopLine_2->GetStart());
	OtherModeTopLine_2->SetEnd(TopLine_2->GetEnd());
	OtherModeTopLine_2->SetZForStartAndEnd(FArmySceneData::WallHeight + offset);

	OtherModeTopLine_3->SetStart(TopLine_3->GetStart());
	OtherModeTopLine_3->SetEnd(TopLine_3->GetEnd());
	OtherModeTopLine_3->SetZForStartAndEnd(FArmySceneData::WallHeight + offset);

	OtherModeTopLine_4->SetStart(TopLine_4->GetStart());
	OtherModeTopLine_4->SetEnd(TopLine_4->GetEnd());
	OtherModeTopLine_4->SetZForStartAndEnd(FArmySceneData::WallHeight + offset);

	//-- left board line	
	OtherModeLeftBoardLine_1->SetStart(LeftBoardLine_1->GetStart());
	OtherModeLeftBoardLine_1->SetEnd(LeftBoardLine_1->GetEnd());
	OtherModeLeftBoardLine_1->SetZForStartAndEnd(FArmySceneData::WallHeight + offset);

	OtherModeLeftBoardLine_2->SetStart(LeftBoardLine_2->GetStart());
	OtherModeLeftBoardLine_2->SetEnd(LeftBoardLine_2->GetEnd());
	OtherModeLeftBoardLine_2->SetZForStartAndEnd(FArmySceneData::WallHeight + offset);

	OtherModeLeftBoardLine_3->SetStart(LeftBoardLine_3->GetStart());
	OtherModeLeftBoardLine_3->SetEnd(LeftBoardLine_3->GetEnd());
	OtherModeLeftBoardLine_3->SetZForStartAndEnd(FArmySceneData::WallHeight + offset);

	OtherModeLeftBoardLine_4->SetStart(LeftBoardLine_4->GetStart());
	OtherModeLeftBoardLine_4->SetEnd(LeftBoardLine_4->GetEnd());
	OtherModeLeftBoardLine_4->SetZForStartAndEnd(FArmySceneData::WallHeight + offset);

	//-- right board line	
	OtherModeRightBoardLine_1->SetStart(RightBoardLine_1->GetStart());
	OtherModeRightBoardLine_1->SetEnd(RightBoardLine_1->GetEnd());
	OtherModeRightBoardLine_1->SetZForStartAndEnd(FArmySceneData::WallHeight + offset);

	OtherModeRightBoardLine_2->SetStart(RightBoardLine_2->GetStart());
	OtherModeRightBoardLine_2->SetEnd(RightBoardLine_2->GetEnd());
	OtherModeRightBoardLine_2->SetZForStartAndEnd(FArmySceneData::WallHeight + offset);

	OtherModeRightBoardLine_3->SetStart(RightBoardLine_3->GetStart());
	OtherModeRightBoardLine_3->SetEnd(RightBoardLine_3->GetEnd());
	OtherModeRightBoardLine_3->SetZForStartAndEnd(FArmySceneData::WallHeight + offset);

	OtherModeRightBoardLine_4->SetStart(RightBoardLine_4->GetStart());
	OtherModeRightBoardLine_4->SetEnd(RightBoardLine_4->GetEnd());
	OtherModeRightBoardLine_4->SetZForStartAndEnd(FArmySceneData::WallHeight + offset);

}

void FArmyRectBayWindow::DrawWindowData(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	MaskedLine->Draw(PDI, View);

	if (WindowType != LeftBoard && WindowType != DoubleSideBoard)
	{
		LeftLine_1->Draw(PDI, View);
		LeftLine_2->Draw(PDI, View);
		LeftLine_3->Draw(PDI, View);
		LeftLine_4->Draw(PDI, View);
	}
	else
	{
		LeftBoardLine_1->Draw(PDI, View);
		LeftBoardLine_2->Draw(PDI, View);
		LeftBoardLine_3->Draw(PDI, View);
		LeftBoardLine_4->Draw(PDI, View);
	}

	if (WindowType != RightBoard && WindowType != DoubleSideBoard)
	{
		RightLine_1->Draw(PDI, View);
		RightLine_2->Draw(PDI, View);
		RightLine_3->Draw(PDI, View);
		RightLine_4->Draw(PDI, View);
	}
	else
	{
		RightBoardLine_1->Draw(PDI, View);
		RightBoardLine_2->Draw(PDI, View);
		RightBoardLine_3->Draw(PDI, View);
		RightBoardLine_4->Draw(PDI, View);
	}

	TopLine_1->Draw(PDI, View);
	TopLine_2->Draw(PDI, View);
	TopLine_3->Draw(PDI, View);
	TopLine_4->Draw(PDI, View);

	LeftBreakLine->Draw(PDI, View);
	RightBreakLine->Draw(PDI, View);
}

void FArmyRectBayWindow::CombineWindowPillar()
{
	float TempWindowHeight = Height + 5.f;
	TArray<FVector> GlassPoints;
	TArray<FVector> UniMiddleWindowVerticlist;
	TArray < FVector> UniInnerWindowVerticeList;
	if (WindowType == AntiBoard)
	{
		UniInnerWindowVerticeList = InnearWindowVerticeList;
		UniMiddleWindowVerticlist = MiddleWindowVerticeList;
	}
	else if (WindowType == LeftBoard)
	{
		UniInnerWindowVerticeList = LeftBoardInnearList;
		UniMiddleWindowVerticlist = LeftBoardMiddleList;
	}
	else if (WindowType == RightBoard)
	{
		UniInnerWindowVerticeList = RightBoardInnearList;
		UniMiddleWindowVerticlist = RightBoardMiddleList;
	}
	else if (WindowType == DoubleSideBoard)
	{
		UniInnerWindowVerticeList = DoubleBoardInnearList;
		UniMiddleWindowVerticlist = DoubleBoardMiddleList;
	}
	int number = UniInnerWindowVerticeList.Num();
	int number1 = UniMiddleWindowVerticlist.Num();
	if (number != number1)
		return;


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
		HoleWindow->AddPillarPoints(VerticalPillars, TempWindowHeight - 1.0f);
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
				HoleWindow->AddPillarPoints(MiddlePillarVertices, TempWindowHeight - 1.0f);
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
	/** @欧石楠 处理是否生成窗台石造成的窗户框的高度*/
	float TempWindowBottomHeight = bIfGenerateWindowStone ? WindowBottomHeight : 0;
	for (int i = 0; i < exTrudeMiddlePoint.Num(); i++)
	{
		tempMiddlePillars.Push(exTrudeMiddlePoint[i] + FVector(0, 0, TempWindowHeight / 3.0f + HeightToFloor));
		tempDownPillars.Push(exTrudeMiddlePoint[i] + FVector(0, 0, HeightToFloor + TempWindowBottomHeight));
		tempUpPillars.Push(exTrudeMiddlePoint[i] + FVector(0, 0, HeightToFloor + TempWindowHeight - 9.0f));
	}
	HoleWindow->AddPillarPoints(tempMiddlePillars, 2.0f);
	HoleWindow->AddPillarPoints(tempDownPillars, 4.0f);
	HoleWindow->AddPillarPoints(tempUpPillars, 4.0f);
}

void FArmyRectBayWindow::CombineWindowWall()
{
	FVector TempHorizontalDirection = FArmyMath::GetLineDirection(StartPoint->Pos, EndPoint->Pos);
	FVector VerticalDirection = TempHorizontalDirection.RotateAngleAxis(-90, FVector(0, 0, 1));

	TArray<FVector> TempLetBoardWall;
	TArray<FVector> TempRightBoardWall;

	WindowWall->ResetMeshTriangles();
	float HorizontalOffsetDistance = 5.0f / FMath::Sin(ArcAngle);
	if (WindowType == LeftBoard)
	{
		FVector LeftFirst = LeftBoardLeftList[0] + FVector(0, 0, HeightToFloor) - TempHorizontalDirection * 0.2f;
		FVector LeftSecond = LeftBoardLeftList[0] - TempHorizontalDirection * HorizontalOffsetDistance + FVector(0, 0, HeightToFloor) - TempHorizontalDirection * 0.2f;
		FVector LeftThird = LeftBoardLeftList[1] - TempHorizontalDirection*HorizontalOffsetDistance + FVector(0, 0, HeightToFloor) - TempHorizontalDirection * 0.2f;
		FVector LeftFourth = LeftBoardLeftList[1] + FVector(0, 0, HeightToFloor) - TempHorizontalDirection * 0.2f;
		TempLetBoardWall = { LeftFirst,LeftSecond,LeftThird,LeftFourth };
		WindowWall->AddBoardPoints(TempLetBoardWall, Height);
	}
	else if (WindowType == RightBoard)
	{
		FVector RightFirst = RightBoardRightList[0] + FVector(0, 0, HeightToFloor) + TempHorizontalDirection * 0.2f;
		FVector RightSecond = RightBoardRightList[1] + FVector(0, 0, HeightToFloor) + TempHorizontalDirection * 0.2f;
		FVector RightThird = RightBoardRightList[1] + TempHorizontalDirection * HorizontalOffsetDistance + FVector(0, 0, HeightToFloor) + TempHorizontalDirection * 0.2f;
		FVector RightFourth = RightBoardRightList[0] + TempHorizontalDirection *HorizontalOffsetDistance + FVector(0, 0, HeightToFloor) + TempHorizontalDirection * 0.2f;
		TempRightBoardWall = { RightFirst,RightSecond,RightThird,RightFourth };
		WindowWall->AddBoardPoints(TempRightBoardWall, Height);
	}
	else if (WindowType == DoubleSideBoard)
	{
		FVector LeftFirst = DoubleBoardLeftList[0] + FVector(0, 0, HeightToFloor) - TempHorizontalDirection * 0.2f;
		FVector LeftSecond = DoubleBoardLeftList[0] - TempHorizontalDirection * HorizontalOffsetDistance + FVector(0, 0, HeightToFloor) - TempHorizontalDirection * 0.2f;
		FVector LeftThird = DoubleBoardLeftList[1] - TempHorizontalDirection*HorizontalOffsetDistance + FVector(0, 0, HeightToFloor) - TempHorizontalDirection * 0.2f;
		FVector LeftFourth = DoubleBoardLeftList[1] + FVector(0, 0, HeightToFloor) - TempHorizontalDirection * 0.2f;
		TempLetBoardWall.Push(LeftFirst);
		TempLetBoardWall.Push(LeftSecond);
		TempLetBoardWall.Push(LeftThird);
		TempLetBoardWall.Push(LeftFourth);

		FVector RightFirst = DoubleBoardRightList[0] + FVector(0, 0, HeightToFloor) + TempHorizontalDirection * 0.2f;
		FVector RightSecond = DoubleBoardRightList[1] + FVector(0, 0, HeightToFloor) + TempHorizontalDirection * 0.2f;
		FVector RightThird = DoubleBoardRightList[1] + TempHorizontalDirection * HorizontalOffsetDistance + FVector(0, 0, HeightToFloor) + TempHorizontalDirection * 0.2f;
		FVector RightFourth = DoubleBoardRightList[0] + TempHorizontalDirection *HorizontalOffsetDistance + FVector(0, 0, HeightToFloor) + TempHorizontalDirection * 0.2f;
		TempRightBoardWall = { RightFirst,RightSecond,RightThird,RightFourth };

		WindowWall->AddBoardPoints(TempLetBoardWall, Height);
		WindowWall->AddBoardPoints(TempRightBoardWall, Height);
	}	
	WindowWall->UpdateAllVetexBufferIndexBuffer();
}

void FArmyRectBayWindow::DrawWindow(FPrimitiveDrawInterface*PDI, const FSceneView* View)
{
	if (WindowType == AntiBoard)
	{
		DrawWithVertice(PDI, View, MiddleWindowVerticeList, DrawColor);
		DrawWithVertice(PDI, View, InnearWindowVerticeList, DrawColor);
	}
	else if (WindowType == LeftBoard)
	{
		DrawWithVertice(PDI, View, LeftBoardLeftList, DrawColor);
		DrawWithVertice(PDI, View, LeftBoardMiddleList, DrawColor);
		DrawWithVertice(PDI, View, LeftBoardInnearList, DrawColor);
	}
	else if (WindowType == RightBoard)
	{
		DrawWithVertice(PDI, View, RightBoardInnearList, DrawColor);
		DrawWithVertice(PDI, View, RightBoardMiddleList, DrawColor);
		DrawWithVertice(PDI, View, RightBoardRightList, DrawColor);
	}
	else if (WindowType == DoubleSideBoard)
	{
		DrawWithVertice(PDI, View, DoubleBoardMiddleList, DrawColor);
		DrawWithVertice(PDI, View, DoubleBoardInnearList, DrawColor);
		DrawWithVertice(PDI, View, DoubleBoardLeftList, DrawColor);
		DrawWithVertice(PDI, View, DoubleBoardRightList, DrawColor);
	}
	DrawWithVertice(PDI, View, OutWindowVerticeList, DrawColor);
	DrawWithVertice(PDI, View, BottomWindowVerticeList, DrawColor);


}

void FArmyRectBayWindow::DrawWithVertice(FPrimitiveDrawInterface* PDI, const FSceneView* View, TArray<FVector>& InVertices, FLinearColor LineColor, float InThickness)
{
	int number = InVertices.Num();
	for (int i = 1; i < number; i++)
	{
		PDI->DrawLine(InVertices[i - 1], InVertices[i], LineColor, 1, InThickness, 0.0f, true);
	}
}

void FArmyRectBayWindow::UpdateArcAngle()
{

}

void FArmyRectBayWindow::UpdateAntiBoardWindow()
{

	float leftDist, height, upLength, rightDist;
	UpdateArcAngle();
	float tempArcTant = FMath::IsNearlyEqual(ArcAngle, PI / 2, 0.01f) ? 0 : 1 / FMath::Tan(ArcAngle);
	OutWindowVerticeList.Empty();
	leftDist = BayWindowHoleLength / 2 + LeftOffset + (FirstGlassWith + SecondGlassWidth) / FMath::Sin(ArcAngle);
	rightDist = BayWindowHoleLength / 2 + RightOffset + (FirstGlassWith + SecondGlassWidth) / FMath::Sin(ArcAngle);
	height = BayWindowHoleDepth + (FirstGlassWith + SecondGlassWidth);
	upLength = (leftDist + rightDist - 2 * height / FMath::Tan(ArcAngle));
	CaculateVertice(OutWindowVerticeList, leftDist, height, upLength);
	MiddleWindowVerticeList.Empty();
	leftDist = BayWindowHoleLength / 2 + LeftOffset + (FirstGlassWith) / FMath::Sin(ArcAngle);
	rightDist = BayWindowHoleLength / 2 + RightOffset + (FirstGlassWith) / FMath::Sin(ArcAngle);
	height = BayWindowHoleDepth + FirstGlassWith;
	upLength = (leftDist + rightDist - 2 * height * tempArcTant);
	CaculateVertice(MiddleWindowVerticeList, leftDist, height, upLength);
	InnearWindowVerticeList.Empty();
	leftDist = BayWindowHoleLength / 2 + LeftOffset;
	rightDist = BayWindowHoleLength / 2 + RightOffset;
	height = BayWindowHoleDepth;
	upLength = (leftDist + rightDist - 2 * height * tempArcTant);
	CaculateVertice(InnearWindowVerticeList, leftDist, height, upLength);
	BottomWindowVerticeList.Empty();
	CaculateBottomVertices(BottomWindowVerticeList);
}

void FArmyRectBayWindow::UpdateLeftBoardWindow()
{
	float leftDist, height, upLength, rightDist;
	UpdateArcAngle();
	float tempArcTant = FMath::IsNearlyEqual(ArcAngle, PI / 2, 0.01f) ? 0 : 1 / FMath::Tan(ArcAngle);
	OutWindowVerticeList.Empty();
	leftDist = BayWindowHoleLength / 2 + LeftOffset + (FirstGlassWith + SecondGlassWidth) / FMath::Sin(ArcAngle);
	rightDist = BayWindowHoleLength / 2 + RightOffset + (FirstGlassWith + SecondGlassWidth) / FMath::Sin(ArcAngle);
	height = BayWindowHoleDepth + (FirstGlassWith + SecondGlassWidth);
	upLength = (leftDist + rightDist - 2 * height *tempArcTant);
	CaculateVertice(OutWindowVerticeList, leftDist, height, upLength);
	FVector centerPoint, leftDirection, rightDirection;
	CaculateCenterAndLeftAndRightDirection(centerPoint, leftDirection, rightDirection);

	LeftBoardLeftList.Empty();
	leftDist = BayWindowHoleLength / 2 + LeftOffset;
	height = BayWindowHoleDepth + (FirstGlassWith/* + SecondGlassWidth*/);
	FVector firstPoint = -HorizontalDirection* leftDist + centerPoint;
	LeftBoardLeftList.Push(firstPoint);
	LeftBoardLeftList.Push(firstPoint + leftDirection* height / FMath::Sin(ArcAngle));

	LeftBoardInnearList.Empty();
	FVector startPoint = centerPoint - HorizontalDirection*leftDist;
	rightDist = BayWindowHoleLength / 2 + RightOffset;
	height = BayWindowHoleDepth;
	upLength = leftDist + rightDist - height* tempArcTant * 2;
	CaculateLeftBoardMiddleAndInnearVerticeList(LeftBoardInnearList, startPoint, leftDirection, rightDirection, height, upLength);

	LeftBoardMiddleList.Empty();
	rightDist = BayWindowHoleLength / 2 + RightOffset + FirstGlassWith / FMath::Sin(ArcAngle);
	height = BayWindowHoleDepth + FirstGlassWith;
	upLength = leftDist + rightDist - height* tempArcTant * 2;
	CaculateLeftBoardMiddleAndInnearVerticeList(LeftBoardMiddleList, startPoint, leftDirection, rightDirection, height, upLength);
	BottomWindowVerticeList.Empty();
	CaculateBottomVertices(BottomWindowVerticeList);
}

void FArmyRectBayWindow::UpdateRightBoardWindow()
{
	float leftDist, height, upLength, rightDist;
	UpdateArcAngle();
	float tempArcTant = FMath::IsNearlyEqual(ArcAngle, PI / 2, 0.01f) ? 0 : 1 / FMath::Tan(ArcAngle);
	OutWindowVerticeList.Empty();
	leftDist = BayWindowHoleLength / 2 + LeftOffset + (FirstGlassWith + SecondGlassWidth) / FMath::Sin(ArcAngle);
	rightDist = BayWindowHoleLength / 2 + RightOffset + (FirstGlassWith + SecondGlassWidth) / FMath::Sin(ArcAngle);
	height = BayWindowHoleDepth + (FirstGlassWith + SecondGlassWidth);
	upLength = (leftDist + rightDist - 2 * height *tempArcTant);
	CaculateVertice(OutWindowVerticeList, leftDist, height, upLength);
	FVector centerPoint, leftDirection, rightDirection;
	CaculateCenterAndLeftAndRightDirection(centerPoint, leftDirection, rightDirection);

	leftDist = BayWindowHoleLength / 2 + LeftOffset + (FirstGlassWith) / FMath::Sin(ArcAngle);
	rightDist = BayWindowHoleLength / 2 + RightOffset;
	height = BayWindowHoleDepth + FirstGlassWith;
	upLength = (leftDist + rightDist - 2 * height *tempArcTant);
	FVector startPoint = centerPoint - HorizontalDirection* leftDist;
	RightBoardMiddleList.Empty();
	CaculateLeftBoardMiddleAndInnearVerticeList(RightBoardMiddleList, startPoint, leftDirection, rightDirection, height, upLength);

	leftDist = BayWindowHoleLength / 2 + LeftOffset;
	rightDist = BayWindowHoleLength / 2 + RightOffset;
	height = BayWindowHoleDepth;
	upLength = (leftDist + rightDist - 2 * height *tempArcTant);

	startPoint = centerPoint - HorizontalDirection *leftDist;
	RightBoardInnearList.Empty();
	CaculateLeftBoardMiddleAndInnearVerticeList(RightBoardInnearList, startPoint, leftDirection, rightDirection, height, upLength);

	RightBoardRightList.Empty();
	FVector rightBordFirstPoint = centerPoint + HorizontalDirection * rightDist;
	FVector rightBordSecondPoint = rightBordFirstPoint + rightDirection * (BayWindowHoleDepth + FirstGlassWith/* + SecondGlassWidth*/) / FMath::Sin(ArcAngle);

	RightBoardRightList.Push(rightBordFirstPoint);
	RightBoardRightList.Push(rightBordSecondPoint);
	BottomWindowVerticeList.Empty();
	CaculateBottomVertices(BottomWindowVerticeList);


}

void FArmyRectBayWindow::UpdateDoubleBoardWindow()
{
	float leftDist, height, upLength, rightDist;
	UpdateArcAngle();
	float tempArcTant = FMath::IsNearlyEqual(ArcAngle, PI / 2, 0.01f) ? 0 : 1 / FMath::Tan(ArcAngle);
	OutWindowVerticeList.Empty();
	leftDist = BayWindowHoleLength / 2 + LeftOffset + (FirstGlassWith + SecondGlassWidth) / FMath::Sin(ArcAngle);
	rightDist = BayWindowHoleLength / 2 + RightOffset + (FirstGlassWith + SecondGlassWidth) / FMath::Sin(ArcAngle);
	height = BayWindowHoleDepth + (FirstGlassWith + SecondGlassWidth);
	upLength = (leftDist + rightDist - 2 * height *tempArcTant);
	CaculateVertice(OutWindowVerticeList, leftDist, height, upLength);
	FVector centerPoint, leftDirection, rightDirection;
	CaculateCenterAndLeftAndRightDirection(centerPoint, leftDirection, rightDirection);

	leftDist = BayWindowHoleLength / 2 + LeftOffset;
	rightDist = BayWindowHoleLength / 2 + RightOffset;
	height = BayWindowHoleDepth;
	upLength = (leftDist + rightDist - 2 * height *tempArcTant);
	FVector startPoint = centerPoint - HorizontalDirection*leftDist;
	FVector secondPoint = startPoint + leftDirection * (height + FirstGlassWith/* + SecondGlassWidth*/) / FMath::Sin(ArcAngle);
	DoubleBoardLeftList.Empty();
	DoubleBoardLeftList.Push(startPoint);
	DoubleBoardLeftList.Push(secondPoint);
	DoubleBoardInnearList.Empty();
	CaculateLeftBoardMiddleAndInnearVerticeList(DoubleBoardInnearList, startPoint, leftDirection, rightDirection, height, upLength);

	height = BayWindowHoleDepth + FirstGlassWith;
	upLength = (leftDist + rightDist - 2 * height *tempArcTant);
	DoubleBoardMiddleList.Empty();
	CaculateLeftBoardMiddleAndInnearVerticeList(DoubleBoardMiddleList, startPoint, leftDirection, rightDirection, height, upLength);

	DoubleBoardRightList.Empty();
	FVector rightSideStartPoint = centerPoint + HorizontalDirection * (rightDist);
	FVector rightSideEndPoint = rightSideStartPoint + (BayWindowHoleDepth + FirstGlassWith) / FMath::Sin(ArcAngle)*rightDirection;
	DoubleBoardRightList.Push(rightSideStartPoint);
	DoubleBoardRightList.Push(rightSideEndPoint);

	BottomWindowVerticeList.Empty();
	CaculateBottomVertices(BottomWindowVerticeList);
}

void FArmyRectBayWindow::CaculateVertice(TArray<FVector>& Results, float leftDist, float height, float upLength)
{
	FVector leftDirection;
	FVector rightDirection;
	FVector tempVertical;
	if (bRightOpen)
	{
		leftDirection = HorizontalDirection.RotateAngleAxis(-FMath::RadiansToDegrees(ArcAngle), FVector(0, 0, 1)).GetSafeNormal();
		rightDirection = HorizontalDirection.RotateAngleAxis(FMath::RadiansToDegrees(ArcAngle) - 180, FVector(0, 0, 1)).GetSafeNormal();
		tempVertical = HorizontalDirection.RotateAngleAxis(-90, FVector(0, 0, 1)).GetSafeNormal();

	}
	else
	{
		leftDirection = HorizontalDirection.RotateAngleAxis(FMath::RadiansToDegrees(ArcAngle), FVector(0, 0, 1)).GetSafeNormal();
		rightDirection = HorizontalDirection.RotateAngleAxis(180 - FMath::RadiansToDegrees(ArcAngle), FVector(0, 0, 1)).GetSafeNormal();
		tempVertical = HorizontalDirection.RotateAngleAxis(90, FVector(0, 0, 1)).GetSafeNormal();
	}
	FVector centerPoint = Point->Pos + Width * tempVertical / 2;
	FVector first = centerPoint - HorizontalDirection*leftDist;
	FVector second = first + leftDirection * height / FMath::Sin(ArcAngle);
	FVector third = second + HorizontalDirection * (upLength);
	FVector fourth = third - rightDirection * height / FMath::Sin(ArcAngle);
	Results.Push(first);
	Results.Push(second);
	Results.Push(third);
	Results.Push(fourth);
}

void FArmyRectBayWindow::CaculateCenterAndLeftAndRightDirection(FVector& centerPoint, FVector& leftDirection, FVector& rightDirection)
{

	FVector tempVertical;
	if (bRightOpen)
	{
		leftDirection = HorizontalDirection.RotateAngleAxis(-FMath::RadiansToDegrees(ArcAngle), FVector(0, 0, 1)).GetSafeNormal();
		rightDirection = HorizontalDirection.RotateAngleAxis(FMath::RadiansToDegrees(ArcAngle) - 180, FVector(0, 0, 1)).GetSafeNormal();
		tempVertical = HorizontalDirection.RotateAngleAxis(-90, FVector(0, 0, 1)).GetSafeNormal();

	}
	else
	{
		leftDirection = HorizontalDirection.RotateAngleAxis(FMath::RadiansToDegrees(ArcAngle), FVector(0, 0, 1)).GetSafeNormal();
		rightDirection = HorizontalDirection.RotateAngleAxis(180 - FMath::RadiansToDegrees(ArcAngle), FVector(0, 0, 1)).GetSafeNormal();
		tempVertical = HorizontalDirection.RotateAngleAxis(90, FVector(0, 0, 1)).GetSafeNormal();
	}
	centerPoint = Point->Pos + Width * tempVertical / 2;
}

void FArmyRectBayWindow::CaculateLeftBoardMiddleAndInnearVerticeList(TArray<FVector>& vertices, FVector startPoint, FVector leftDirectin, FVector rightDirection, float height, float upLength)
{
	if (WindowType == LeftBoard)
	{
		FVector first = startPoint + height / FMath::Sin(ArcAngle)* leftDirectin;
		FVector second = first + HorizontalDirection* upLength;
		FVector third = second - height / FMath::Sin(ArcAngle)*rightDirection;
		vertices.Push(first);
		vertices.Push(second);
		vertices.Push(third);
	}
	else if (WindowType == RightBoard)
	{
		FVector first = startPoint;
		FVector second = first + height / FMath::Sin(ArcAngle)*leftDirectin;
		FVector third = second + upLength*HorizontalDirection;
		vertices.Push(first);
		vertices.Push(second);
		vertices.Push(third);
	}
	else if (WindowType == DoubleSideBoard)
	{
		FVector first = startPoint + height / FMath::Sin(ArcAngle)*leftDirectin;
		FVector second = first + upLength *HorizontalDirection;
		vertices.Push(first);
		vertices.Push(second);
	}

}

void FArmyRectBayWindow::CaculateBottomVertices(TArray<FVector>& Results)
{
	/** @欧石楠 如果不生成窗台石则不计算*/
	if (!bIfGenerateWindowStone)
	{
		return;
	}

	FVector vertical;
	if (bRightOpen)
	{
		vertical = HorizontalDirection.RotateAngleAxis(-90, FVector(0, 0, 1)).GetSafeNormal();
	}
	else
	{
		vertical = HorizontalDirection.RotateAngleAxis(90, FVector(0, 0, 1)).GetSafeNormal();
	}
	FVector centerPoint = Point->Pos + Width *vertical / 2;
	FVector first = centerPoint - HorizontalDirection*(BayWindowHoleLength / 2 + LeftOffset /*+ (FirstGlassWith + SecondGlassWidth) / FMath::Sin(ArcAngle)*/);
	FVector second = centerPoint - HorizontalDirection*(BayWindowHoleLength / 2);
	FVector third = second - vertical * Width;
	FVector fourth = third + HorizontalDirection * BayWindowHoleLength;
	FVector fifth = fourth + vertical * Width;
	FVector six = fifth + HorizontalDirection * (RightOffset /*+ (FirstGlassWith + SecondGlassWidth) / FMath::Sin(ArcAngle)*/);
	Results.Push(first);
	Results.Push(second);
	Results.Push(third);
	Results.Push(fourth);
	Results.Push(fifth);
	Results.Push(six);
}

void FArmyRectBayWindow::DrawTopViewWindow(FPrimitiveDrawInterface * PDI, const FSceneView * View)
{
	if (WindowType != LeftBoard && WindowType != DoubleSideBoard)
	{
		OtherModeLeftLine_1->Draw(PDI, View);
		OtherModeLeftLine_2->Draw(PDI, View);
		OtherModeLeftLine_3->Draw(PDI, View);
		OtherModeLeftLine_4->Draw(PDI, View);
	}
	else
	{
		OtherModeLeftBoardLine_1->Draw(PDI, View);
		OtherModeLeftBoardLine_2->Draw(PDI, View);
		OtherModeLeftBoardLine_3->Draw(PDI, View);
		OtherModeLeftBoardLine_4->Draw(PDI, View);
	}

	if (WindowType != RightBoard && WindowType != DoubleSideBoard)
	{
		OtherModeRightLine_1->Draw(PDI, View);
		OtherModeRightLine_2->Draw(PDI, View);
		OtherModeRightLine_3->Draw(PDI, View);
		OtherModeRightLine_4->Draw(PDI, View);
	}
	else
	{
		OtherModeRightBoardLine_1->Draw(PDI, View);
		OtherModeRightBoardLine_2->Draw(PDI, View);
		OtherModeRightBoardLine_3->Draw(PDI, View);
		OtherModeRightBoardLine_4->Draw(PDI, View);
	}

	OtherModeTopLine_1->Draw(PDI, View);
	OtherModeTopLine_2->Draw(PDI, View);
	OtherModeTopLine_3->Draw(PDI, View);
	OtherModeTopLine_4->Draw(PDI, View);

	OtherModeLeftBreakLine->Draw(PDI, View);
	OtherModeRightBreakLine->Draw(PDI, View);
	OtherModeHalfLine->Draw(PDI, View);
}

 float FArmyRectBayWindow::GetStoneLenth()
{
	return (StartPoint->Pos - EndPoint->Pos).Size() + WindowBottomExtrudeLength * 2;
}

 float FArmyRectBayWindow::GetStoneArea()
{
	return  StoneArea;
}

void FArmyRectBayWindow::SetContentItem(TSharedPtr<FContentItemSpace::FContentItem> Goods)
{
	ContentItem = Goods;
	bDefaultMaterial = !Goods.IsValid();
	if (Goods.IsValid())
	{

		SaleID = Goods->ID;

		UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
		ResMgr->AddSyncItem(Goods);
		//@郭子阳
		//设置原始户型\拆改后中的窗口的SaleID
		auto HomeObject = FArmySceneData::Get()->GetObjectByGuid(EModelType::E_HomeModel, GetUniqueID());
		auto HomeWindow = StaticCastSharedPtr<FArmyRectBayWindow>(HomeObject.Pin());
		if (HomeWindow.IsValid())
		{
			HomeWindow->SaleID = SaleID;
		}
		auto LayoutObject = FArmySceneData::Get()->GetObjectByGuid(EModelType::E_LayoutModel, GetUniqueID());
		auto LayoutWindow = StaticCastSharedPtr<FArmyRectBayWindow>(LayoutObject.Pin());
		if (LayoutWindow.IsValid())
		{
			LayoutWindow->SaleID = SaleID;
		}

		ConstructionPatameters P;
		P.SetNormalGoodsInfo(Goods->ID);
		XRConstructionManager::Get()->TryToFindConstructionData(GetUniqueID(), P, nullptr);

		//UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
		WindowStoneMat = ResMgr->CreateContentItemMaterial(Goods);
		if (HoleWindow &&HoleWindow->IsValidLowLevel())
		{
			HoleWindow->WindowStoneName = Goods->Name;
			HoleWindow->SetSynID(WindowStoneMat->GetSynID());
			HoleWindow->SetWindowBoardMaterial(WindowStoneMat);
		}
		//SetThumbnailUrl(ContentItem->ThumbnailURL);
	}
	else
	{
		bDefaultMaterial = true;
		WindowStoneMat = nullptr;

		if (HoleWindow &&HoleWindow->IsValidLowLevel())
		{
			HoleWindow->SetWindowBoardMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWindowBaseMaterial());
			//Window->HoleWindow->CurrentWindowType = NORMAL_WINDOW;
			HoleWindow->WindowStoneName = TEXT("");
			HoleWindow->SetSynID(-1);
		}
		//SetThumbnailUrl("");
	}


	//为了使拆改中模式的窗台池材质匹配拆改后的材质,这样拷贝数据时才会正确
	//TArray<FObjectWeakPtr> LayOutObjects;
	//FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Window, LayOutObjects);
	//FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_FloorWindow, LayOutObjects);
	//for (auto It : LayOutObjects)
	//{
	//	TSharedPtr<FArmyWindow> WindowObj = StaticCastSharedPtr<FArmyWindow>(It.Pin());
	//	if (WindowObj.IsValid() && WindowObj->HoleWindow == winActor)
	//	{
	//		TWeakPtr<FArmyObject> ModifyObj = FArmySceneData::Get()->GetObjectByGuid(E_ModifyModel, WindowObj->GetUniqueID());
	//		WindowObj = StaticCastSharedPtr<FArmyWindow>(ModifyObj.Pin());
	//		if (WindowObj.IsValid())
	//		{
	//			if (WindowObj->HoleWindow == NULL)
	//				WindowObj->Generate(GGI->GetWorld());
	//			//UMaterialInterface * mat2 = ResMgr->CreateContentItemMaterial(ContentItem);
	//			//WindowObj->HoleWindow->SetSynID(mat2->GetSynID());
	//			//WindowObj->HoleWindow->SetWindowBoardMaterial(mat2);
	//			WindowObj->SetContentItem(ContentItem);
	//			//WindowObj->SetThumbnailUrl(ContentItem->ThumbnailURL);
	//			break;
	//		}
	//	}
	//}
	//LayOutObjects.Empty();
	//FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_RectBayWindow, LayOutObjects);
	//FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_TrapeBayWindow, LayOutObjects);
	//for (auto It : LayOutObjects)
	//{
	//	TSharedPtr<FArmyRectBayWindow> WindowObj = StaticCastSharedPtr<FArmyRectBayWindow>(It.Pin());
	//	
	//	bool temp1 = WindowObj.IsValid();
	//	bool temp2 = WindowObj->GetUniqueID() == GetUniqueID();
	//	bool temp3 = WindowObj.Get() != this;
	//	bool temp4 = WindowObj->HoleWindow == HoleWindow;
	//	
	//	if (WindowObj.IsValid()
	//		&& WindowObj->GetUniqueID() == GetUniqueID()
	//		&& WindowObj.Get() != this
	//		&& WindowObj->HoleWindow == HoleWindow)
	//	{
	//		TWeakPtr<FArmyObject> ModifyObj = FArmySceneData::Get()->GetObjectByGuid(E_ModifyModel, WindowObj->GetUniqueID());
	//		WindowObj = StaticCastSharedPtr<FArmyRectBayWindow>(ModifyObj.Pin());
	//		if (WindowObj.IsValid())
	//		{
	//			if (WindowObj->HoleWindow == NULL)
	//				WindowObj->Generate(GGI->GetWorld());
	//			//UMaterialInterface * mat2 = ResMgr->CreateContentItemMaterial(ContentItem);
	//			//WindowObj->HoleWindow->SetSynID(mat2->GetSynID());
	//			//WindowObj->HoleWindow->SetWindowBoardMaterial(mat2);
	//			WindowObj->SetContentItem(ContentItem);
	//			//WindowObj->SetThumbnailUrl(ContentItem->ThumbnailURL);
	//			break;
	//		}
	//	}
	//}


}

void FArmyRectBayWindow::ConstructionData(TArray<struct FArmyGoods>& ArtificialData)
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

	if (!ContentItem.IsValid())
	{
		return;
	}

	FArmyGoods goods;
	goods.GoodsId = SaleID;
	goods.Type = 1;
	//goods.PaveID = ConvertStyleToPavingID(MatStyle);

	// 施工项
	ConstructionPatameters P;
	P.SetNormalGoodsInfo(goods.GoodsId);
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

	Goods.Param.S += GetStoneArea() / 10000.0f;
	Goods.Param.D = Goods.Param.S;

}

TSharedPtr<FArmyRoom> FArmyRectBayWindow::GetRoom()
{
	if (FirstRelatedRoom.IsValid() && FirstRelatedRoom->GetType()!=OT_OutRoom
	&& SecondRelatedRoom.IsValid() && SecondRelatedRoom->GetType() != OT_OutRoom)
	{
		//处于墙之间的门没有空间
		return nullptr;
	}

	return FirstRelatedRoom.IsValid() && FirstRelatedRoom->GetType() != OT_OutRoom ? FirstRelatedRoom : SecondRelatedRoom;
}

void FArmyRectBayWindow::Generate(UWorld* InWorld)
{
	if (!bGenerate3D)
	{
		return;
	}

	if (WindowWall == NULL)
	{
		FActorSpawnParameters SpawnParam;
		SpawnParam.Name = FName(*(TEXT("NOLIST-SHAPE") + GetUniqueID().ToString()));
		WindowWall = InWorld->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass(), SpawnParam);
		WindowWall->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial());
		WindowWall->Tags.Add(XRActorTag::Window);
		WindowWall->Tags.Add(XRActorTag::CanNotDelete);
	}
	if (HoleWindow == NULL)
	{
		FActorSpawnParameters SpawnParam;
		SpawnParam.Name = FName(*("NOLIST-WINDOW" + GetUniqueID().ToString()));

		HoleWindow = InWorld->SpawnActor<AXRWindowActor>(AXRWindowActor::StaticClass(), SpawnParam);
		//@马云龙 赋值AttachSurface
		HoleWindow->AttachSurface = StaticCastSharedRef<FArmyHardware>(this->AsShared());
		WindowWall->Tags.Add(XRActorTag::Window);
		WindowWall->Tags.Add(XRActorTag::CanNotDelete);
		if (bDefaultMaterial)
		{
			HoleWindow->SetWindowBoardMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWindowBaseMaterial());
		}
		else
		{
			HoleWindow->SetWindowBoardMaterial(WindowStoneMat);
		}
		HoleWindow->SetWindowPillarMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWindowPillarMaterial());
		HoleWindow->SetWindowGlassMaterial(FArmyEngineModule::Get().GetEngineResource()->GetGlassMaterial());

		HoleWindow->CurrentWindowType = RECTBAY_WINDOW;
		HoleWindow->WinowLength = Length;
		HoleWindow->WindowHeight = Height;
		HoleWindow->BayWindowDepth = BayWindowHoleDepth;
		HoleWindow->WindowOffsetGroundHeight = HeightToFloor;
	}
	HoleWindow->ResetMeshTriangles();
	TArray<FVector> WorldPoints;
	TArray<FVector> tempBottom = BottomWindowVerticeList;
	FArmyMath::ReversePointList(tempBottom);
	TArray<FVector> TempWordPoint = OutWindowVerticeList;
	TempWordPoint.Append(tempBottom);

	if (BottomWindowVerticeList.Num() == 6)
	{
		TArray<FVector> newCalucluteBottom;
		FVector extrudeStart = BottomWindowVerticeList[2];
		FVector extrudeEnd = BottomWindowVerticeList[3];
		extrudeStart += HorizontalDirection.RotateAngleAxis(90, FVector(0, 0, 1))*WindowBottomExtrudeLength;
		extrudeEnd += HorizontalDirection.RotateAngleAxis(90, FVector(0, 0, 1))*WindowBottomExtrudeLength;
		for (int i = 0; i < BottomWindowVerticeList.Num(); i++)
		{
			if (i == 2)
			{
				newCalucluteBottom.Push(BottomWindowVerticeList[2]);
				newCalucluteBottom.Push(BottomWindowVerticeList[2] - HorizontalDirection * 6.0f);
				newCalucluteBottom.Push(BottomWindowVerticeList[2] - HorizontalDirection*6.0f + HorizontalDirection.RotateAngleAxis(90, FVector(0, 0, 1))*WindowBottomExtrudeLength);
			}
			else if (i == 3)
			{
				newCalucluteBottom.Push(BottomWindowVerticeList[3] + HorizontalDirection*6.0f + HorizontalDirection.RotateAngleAxis(90, FVector(0, 0, 1))*WindowBottomExtrudeLength);
				newCalucluteBottom.Push(BottomWindowVerticeList[3] + HorizontalDirection *6.0f);
				newCalucluteBottom.Push(BottomWindowVerticeList[3]);
			}
			else
			{
				newCalucluteBottom.Push(BottomWindowVerticeList[i]);
			}
		}
		FArmyMath::ReversePointList(newCalucluteBottom);

		for (int i = 0; i < OutWindowVerticeList.Num(); i++)
		{
			WorldPoints.Push(FVector(OutWindowVerticeList[i].X, OutWindowVerticeList[i].Y, 0.0f) + FVector(0, 0, HeightToFloor));
		}
		for (int i = 0; i < newCalucluteBottom.Num(); i++)
		{
			WorldPoints.Push(FVector(newCalucluteBottom[i].X, newCalucluteBottom[i].Y, 0.0f) + FVector(0, 0, HeightToFloor));
		}

		//计算窗台石面积
		StoneArea = 0;
		//计算矩形部分
		float AreaLength = BayWindowHoleLength + 6.0 * 2;
		float AreaWidth = this->Width + WindowBottomExtrudeLength; //WindowBottomExtrudeLength==6;
		StoneArea += AreaLength *AreaWidth;
		//计算梯形部分
		float UpLength = (OutWindowVerticeList[2] - OutWindowVerticeList[1]).Size2D();//上底
		float DownLength = (OutWindowVerticeList[3] - OutWindowVerticeList[0]).Size2D();//下底
		float AreaHeight= BayWindowHoleDepth + (FirstGlassWith + SecondGlassWidth); //高
		StoneArea += (UpLength + DownLength)*AreaHeight / 2;
	}
	else
	{
		StoneArea = 0;
	}

	TArray<FVector> TopPoints;
	int	number = OutWindowVerticeList.Num();
	for (int i = 0; i < number; i++)
	{
		TopPoints.Push(OutWindowVerticeList[i] + FVector(0, 0, Height + HeightToFloor + 0.2f));
	}

	HoleWindow->AddWindowBoardPoints(WorldPoints, WindowBottomHeight);
	HoleWindow->AddWindowBoardPoints(TopPoints, 5.0f);
	CombineWindowPillar();
	CombineWindowWall();
	HoleWindow->UpdataAlloVertexIndexBuffer();
	SetRelevanceActor(HoleWindow);
	AttachModelName = TEXT("RectBayWindow") + GetUniqueID().ToString();
	HoleWindow->SetActorLabel(AttachModelName);
	HoleWindow->Tags.Add(XRActorTag::Immovable);
	HoleWindow->Tags.Add(XRActorTag::Window);
	HoleWindow->Tags.Add(XRActorTag::CanNotDelete);
}

void FArmyRectBayWindow::OnRoomSpaceIDChanged(int32 NewSpaceID)
{
	ConstructionPatameters P;
	P.SetNormalGoodsInfo(SaleID, GetRoomSpaceID());
	XRConstructionManager::Get()->TryToFindConstructionData(GetUniqueID(), P, nullptr);
}