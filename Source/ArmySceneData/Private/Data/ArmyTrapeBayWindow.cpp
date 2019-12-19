#include "ArmyTrapeBayWindow.h"
#include "ArmyEngineModule.h"
#include "ArmyWindowActor.h"
#include "ArmySceneData.h"
#include "ArmyPolygon.h"
#include "ArmyActorConstant.h"
#include "ArmyResourceModule.h"

#define FirstGlassWith 3.5
#define SecondGlassWidth 7
#define DefaultOffsetGroundHeight 50

FArmyTrapeBayWindow::FArmyTrapeBayWindow() :FArmyRectBayWindow()
{
	HeightToFloor = DefaultOffsetGroundHeight;
	BayWindowHoleUpLength = BayWindowHoleLength - 40;
	OutWindowLength = BayWindowHoleLength;
	float bottom = LeftOffset + RightOffset + BayWindowHoleLength;
	float x = (bottom - BayWindowHoleUpLength) / 2;
	ArcAngle = FMath::Atan(BayWindowHoleDepth / x);
	Length = 150;
	ObjectType = OT_TrapeBayWindow;
	SetName(TEXT("梯形飘窗")/* + GetUniqueID().ToString()*/);
}

FArmyTrapeBayWindow::FArmyTrapeBayWindow(FArmyTrapeBayWindow* Copy) :FArmyRectBayWindow(Copy)
{
	BayWindowHoleUpLength = Copy->BayWindowHoleUpLength;
}

FArmyTrapeBayWindow::~FArmyTrapeBayWindow()
{

}

void FArmyTrapeBayWindow::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyHardware::SerializeToJson(JsonWriter);

	JsonWriter->WriteValue("pos", GetPos().ToString());
	JsonWriter->WriteValue("direction", GetDirection().ToString());
	JsonWriter->WriteValue("length", GetLength());
	JsonWriter->WriteValue("width", GetWidth());
	//JsonWriter->WriteValue("HeightToFloor", GetOffGoundDist());
	//JsonWriter->WriteValue("WindowHeight", GetWindowHeight());
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
	JsonWriter->WriteValue("BayWindowHoleUpLength", GetUpHoleLength());
	JsonWriter->WriteValue("bGenerateWindowStone", bIfGenerateWindowStone);

	if (HoleWindow&& ContentItem.IsValid())
	{
		JsonWriter->WriteObjectStart("ContenItem");
		ContentItem->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}



	if (HoleWindow)
	{
		//@打扮家 XRLightmass 序列化LightMapID
		if (HoleWindow)
			LightMapID = HoleWindow->GetLightMapGUIDFromSMC();
		JsonWriter->WriteValue("LightMapID", LightMapID.ToString());
	}

	SERIALIZEREGISTERCLASS(JsonWriter, FArmyTrapeBayWindow)
}

void FArmyTrapeBayWindow::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyHardware::Deserialization(InJsonData);

	FVector TempPos, TempDirection;
	TempPos.InitFromString(InJsonData->GetStringField("pos"));
	TempDirection.InitFromString(InJsonData->GetStringField("direction"));
	SetPos(TempPos);
	SetLength(InJsonData->GetNumberField("length"));
	SetWidth(InJsonData->GetNumberField("Width"));
	SetDirection(TempDirection);
	//SetWindowHeight(InJsonData->GetNumberField("WindowHeight"));
	SetRightOpen(InJsonData->GetBoolField("bRightOpen"));
	//SetOffGroundDist(InJsonData->GetNumberField("HeightToFloor"));
	SetWindowHoleLength(InJsonData->GetNumberField("BayWindowHoleLength"));
	SetWindowLeftDist(InJsonData->GetNumberField("LeftOffset"));
	SetWindowRightDist(InJsonData->GetNumberField("RightOffset"));
	SetWindowHoleDepth(InJsonData->GetNumberField("BayWindowHoleDepth"));
	SetWindowBottomBoardThickness(InJsonData->GetNumberField("WindowBottomHeight"));
	SetWindowBottomBoardExtrudeLength(InJsonData->GetNumberField("WindowBottomExtrudeLength"));
	SetUpHoleLength(InJsonData->GetNumberField("BayWindowHoleUpLength"));
	SetOutWindowLength(InJsonData->GetNumberField("OutWindowLength"));
	SetWindowType(InJsonData->GetNumberField("WindowType"));
	InJsonData->TryGetBoolField("bGenerateWindowStone", bIfGenerateWindowStone);


	TSharedPtr<FJsonObject> itemObject = InJsonData->GetObjectField("ContenItem");
	if (itemObject.IsValid() && itemObject->Values.Num() > 0)
	{

		ContentItem = MakeShareable(new FContentItemSpace::FContentItem());
		ContentItem->Deserialization(itemObject);

		UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		if (mat)
		{
			bDefaultMaterial = false;
			WindowStoneMat = mat;
		}
	}

	SetState(OS_Normal);

	//@打扮家 XRLightmass 反序列化LightMapID
	FString LightGUIDStr = "";
	InJsonData->TryGetStringField("LightMapID", LightGUIDStr);
	FGuid::Parse(LightGUIDStr, LightMapID);

	bool bCapture = CaptureDoubleLine(TempPos, E_LayoutModel);

}

void FArmyTrapeBayWindow::SetUpHoleLength(float length)
{
	float TempLength = length;

	float bottom = LeftOffset + RightOffset + BayWindowHoleLength;
	BayWindowHoleUpLength = TempLength;
	if (bottom >= TempLength)
	{
		float x = (bottom - TempLength) / 2;
		ArcAngle = FMath::Atan(BayWindowHoleDepth / x);
		Update();
	}
}

void FArmyTrapeBayWindow::SetOutWindowLength(float InOutLength)
{
	float TempLength = InOutLength;
	if (TempLength < 30.f)
	{
		TempLength = 30.f;
	}

	if (TempLength >= Length)
	{
		OutWindowLength = Length - 20;
		if (OutWindowLength < 30.f)
		{
			OutWindowLength = 30.f;
		}
	}
	else
	{
		OutWindowLength = TempLength;
	}
	SetUpHoleLength(OutWindowLength);
}

void FArmyTrapeBayWindow::UpdateWindowData()
{
	float IntervalDist = 8.f;
	FVector TempPos = GetPos();
	TempPos.Z = 3.f;
	MaskedLine->SetStart(TempPos - HorizontalDirection * Length / 2.f + Direction * Width / 2.f);
	MaskedLine->SetEnd(TempPos + HorizontalDirection * Length / 2.f + Direction * Width / 2.f);

	LeftLine_1->SetStart(TempPos - HorizontalDirection * Length / 2.f + Direction * Width / 2.f);
	LeftLine_1->SetEnd(TempPos - HorizontalDirection * OutWindowLength / 2.f + Direction * (Width / 2.f + BayWindowHoleDepth));
	LeftLine_2->SetStart(LeftLine_1->GetStart() - HorizontalDirection * IntervalDist);
	LeftLine_2->SetEnd(LeftLine_1->GetEnd() - HorizontalDirection * IntervalDist + Direction * IntervalDist);
	LeftLine_3->SetStart(LeftLine_2->GetStart() - HorizontalDirection * IntervalDist);
	LeftLine_3->SetEnd(LeftLine_2->GetEnd() - HorizontalDirection * IntervalDist + Direction * IntervalDist);
	LeftLine_4->SetStart(LeftLine_3->GetStart() - HorizontalDirection * IntervalDist);
	LeftLine_4->SetEnd(LeftLine_3->GetEnd() - HorizontalDirection * IntervalDist + Direction * IntervalDist);	

	RightLine_1->SetStart(TempPos + HorizontalDirection * Length / 2.f + Direction * Width / 2.f);
	RightLine_1->SetEnd(TempPos + HorizontalDirection * OutWindowLength / 2.f + Direction * (Width / 2.f + BayWindowHoleDepth));
	RightLine_2->SetStart(RightLine_1->GetStart() + HorizontalDirection * IntervalDist);
	RightLine_2->SetEnd(RightLine_1->GetEnd() + HorizontalDirection * IntervalDist + Direction * IntervalDist);
	RightLine_3->SetStart(RightLine_2->GetStart() + HorizontalDirection * IntervalDist);
	RightLine_3->SetEnd(RightLine_2->GetEnd() + HorizontalDirection * IntervalDist + Direction * IntervalDist);
	RightLine_4->SetStart(RightLine_3->GetStart() + HorizontalDirection * IntervalDist);
	RightLine_4->SetEnd(RightLine_3->GetEnd() + HorizontalDirection * IntervalDist + Direction * IntervalDist);

	TopLine_1->SetStart(TempPos - HorizontalDirection * OutWindowLength / 2.f + Direction * (Width / 2.f + BayWindowHoleDepth));
	TopLine_1->SetEnd(TempPos + HorizontalDirection * OutWindowLength / 2.f + Direction * (Width / 2.f + BayWindowHoleDepth));
	TopLine_2->SetStart(TopLine_1->GetStart() - HorizontalDirection * IntervalDist + Direction * IntervalDist);
	TopLine_2->SetEnd(TopLine_1->GetEnd() + HorizontalDirection * IntervalDist + Direction * IntervalDist);
	TopLine_3->SetStart(TopLine_2->GetStart() - HorizontalDirection * IntervalDist + Direction * IntervalDist);
	TopLine_3->SetEnd(TopLine_2->GetEnd() + HorizontalDirection * IntervalDist + Direction * IntervalDist);
	TopLine_4->SetStart(TopLine_3->GetStart() - HorizontalDirection * IntervalDist + Direction * IntervalDist);
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
	LeftBreakLine->SetEnd(LeftLine_1->GetStart());
	LeftBreakLine->SetZForStartAndEnd(FArmySceneData::WallHeight + 23.f);

	RightBreakLine->SetStart(TempPos + HorizontalDirection * Length / 2.f - Direction * (Width / 2.f));
	RightBreakLine->SetEnd(RightLine_1->GetStart());
	RightBreakLine->SetZForStartAndEnd(FArmySceneData::WallHeight + 23.f);

	OtherModeHalfLine->SetStart(TempPos - HorizontalDirection * Length / 2.f - Direction * (Width / 2.f));
	OtherModeHalfLine->SetEnd(TempPos + HorizontalDirection * Length / 2.f - Direction * (Width / 2.f));
	OtherModeHalfLine->SetZForStartAndEnd(FArmySceneData::WallHeight + 23.f);

	OtherModeLeftBreakLine->SetStart(OtherModeHalfLine->GetStart());
	OtherModeLeftBreakLine->SetEnd(LeftLine_1->GetStart());
	OtherModeLeftBreakLine->SetZForStartAndEnd(FArmySceneData::WallHeight + 23.f);

	OtherModeRightBreakLine->SetStart(OtherModeHalfLine->GetEnd());
	OtherModeRightBreakLine->SetEnd(RightLine_1->GetStart());
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
	V1 = LeftLine_1->GetStart();
	V2 = LeftLine_4->GetStart();
	V3 = LeftLine_4->GetEnd();
	V4 = RightLine_4->GetEnd();
	V5 = RightLine_4->GetStart();
	V6 = RightLine_1->GetStart();
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

void FArmyTrapeBayWindow::UpdateArcAngle()
{
	float bottom = LeftOffset + RightOffset + BayWindowHoleLength;
	float x = (bottom - BayWindowHoleUpLength) / 2;
	ArcAngle = FMath::Atan(BayWindowHoleDepth / x);
}

void FArmyTrapeBayWindow::Generate(UWorld* InWorld)
{
    if (!bGenerate3D)
    {
		return;
    }

	TArray<FVector> Vertices = GetClipingBox();
    float TempHeight = GetHeight();
    FArmyRectBayWindow::Generate(InWorld);
	AttachModelName = TEXT("TrapBayWindow") + GetUniqueID().ToString();
	HoleWindow->SetActorLabel(AttachModelName);
	HoleWindow->CurrentWindowType = TRAPEBAY_WINDOW;
	HoleWindow->WinowLength = Length;
	HoleWindow->WindowHeight = TempHeight;
	HoleWindow->BayWindowDepth = BayWindowHoleDepth;
	HoleWindow->WindowOffsetGroundHeight = HeightToFloor;
	HoleWindow->Tags.Add(XRActorTag::Immovable);
	HoleWindow->Tags.Add(XRActorTag::Window);
	HoleWindow->Tags.Add(XRActorTag::CanNotDelete);
}

void FArmyTrapeBayWindow::Update()
{
	FArmyRectBayWindow::Update();
	if (OutWindowLength >= Length)
	{
		OutWindowLength = Length - 20;
		SetUpHoleLength(OutWindowLength);
	}
	UpdateWindowData();
}
