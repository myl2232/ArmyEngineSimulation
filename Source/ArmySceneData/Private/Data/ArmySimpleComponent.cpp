#include "ArmySimpleComponent.h"
#include "ArmyEditPoint.h"
#include "ArmyRoom.h"
#include "ArmySceneData.h"
#include "ArmyStyle.h"
#include "ArmyMath.h"
#include "ArmyRulerLine.h"
#include "ArmyGameInstance.h"
#include "ArmyEngineModule.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "ArmyPolygon.h"

FArmySimpleComponent::FArmySimpleComponent()
	: LeftPoint(MakeShareable(new FArmyEditPoint()))
	, RightPoint(MakeShareable(new FArmyEditPoint()))
	, TopPoint(MakeShareable(new FArmyEditPoint()))
	, BottomPoint(MakeShareable(new FArmyEditPoint()))
	, LeftTopPoint(MakeShareable(new FArmyEditPoint()))
	, LeftBottomPoint(MakeShareable(new FArmyEditPoint()))
	, RightTopPoint(MakeShareable(new FArmyEditPoint()))
	, RightBottomPoint(MakeShareable(new FArmyEditPoint()))
	, Point(MakeShareable(new FArmyEditPoint()))
	, DirectionPoint(MakeShareable(new FArmyEditPoint()))
	, TopLeftLine(MakeShareable(new FArmyLine()))
	, BottomLeftLine(MakeShareable(new FArmyLine()))
	, TopRightLine(MakeShareable(new FArmyLine()))
	, BottomRightLine(MakeShareable(new FArmyLine()))
	, LeftTopLine(MakeShareable(new FArmyLine()))
	, LeftBottomLine(MakeShareable(new FArmyLine()))
	, RightTopLine(MakeShareable(new FArmyLine()))
	, RightBottomLine(MakeShareable(new FArmyLine()))
	, OtherModeTopLeftLine(MakeShareable(new FArmyLine()))
	, OtherModeBottomLeftLine(MakeShareable(new FArmyLine()))
	, OtherModeTopRightLine(MakeShareable(new FArmyLine()))
	, OtherModeBottomRightLine(MakeShareable(new FArmyLine()))
	, OtherModeLeftTopLine(MakeShareable(new FArmyLine()))
	, OtherModeLeftBottomLine(MakeShareable(new FArmyLine()))
	, OtherModeRightTopLine(MakeShareable(new FArmyLine()))
	, OtherModeRightBottomLine(MakeShareable(new FArmyLine()))
	, Length(100)
	, Width(40)
	, Height(40)
	, Direction(FVector(1, 0, 0))
	, HorizontalDirection(ForceInitToZero)
	, TopRulerLine(MakeShareable(new FArmyRulerLine()))
	, BottomRulerLine(MakeShareable(new FArmyRulerLine()))
	, LeftRulerLine(MakeShareable(new FArmyRulerLine()))
	, RightRulerLine(MakeShareable(new FArmyRulerLine()))
	, OtherModePolygon(MakeShareable(new FArmyPolygon()))
{
	SetPos(FVector(-999999.f, -999999.f, 0.f));	

	MI_PolygonMat = FArmyEngineModule::Get().GetEngineResource()->GetUnStageBaseColorMaterial();
	MI_PolygonMat->AddToRoot();	
	MI_PolygonMat->SetVectorParameterValue("BaseColor", FLinearColor(FColor(0xFFE6E6E6)));
	OtherModePolygon->MaterialRenderProxy = MI_PolygonMat->GetRenderProxy(false);

	Direction.RotateAngleAxis(Angle, FVector(0, 0, 1));	

	//源绘制数据
	TopLeftLine->SetBaseColor(FLinearColor::White);
	BottomLeftLine->SetBaseColor(FLinearColor::White);
	TopRightLine->SetBaseColor(FLinearColor::White);
	BottomRightLine->SetBaseColor(FLinearColor::White);
	LeftTopLine->SetBaseColor(FLinearColor::White);
	LeftBottomLine->SetBaseColor(FLinearColor::White);
	RightTopLine->SetBaseColor(FLinearColor::White);
	RightBottomLine->SetBaseColor(FLinearColor::White);
	TopLeftLine->SetLineWidth(2.f);
	BottomLeftLine->SetLineWidth(2.f);
	TopRightLine->SetLineWidth(2.f);
	BottomRightLine->SetLineWidth(2.f);
	LeftTopLine->SetLineWidth(2.f);
	LeftBottomLine->SetLineWidth(2.f);
	RightTopLine->SetLineWidth(2.f);
	RightBottomLine->SetLineWidth(2.f);	
	//设置绘制数据偏移
	TopLeftLine->SetDrawDataOffset(FVector(0, 0, 3.f));
	BottomLeftLine->SetDrawDataOffset(FVector(0, 0, 3.f));
	TopRightLine->SetDrawDataOffset(FVector(0, 0, 3.f));
	BottomRightLine->SetDrawDataOffset(FVector(0, 0, 3.f));
	LeftTopLine->SetDrawDataOffset(FVector(0, 0, 3.f));
	LeftBottomLine->SetDrawDataOffset(FVector(0, 0, 3.f));
	RightTopLine->SetDrawDataOffset(FVector(0, 0, 3.f));
	RightBottomLine->SetDrawDataOffset(FVector(0, 0, 3.f));

	//其他模式绘制数据
	OtherModeTopLeftLine->SetBaseColor(FLinearColor::White);
	OtherModeBottomLeftLine->SetBaseColor(FLinearColor::White);
	OtherModeTopRightLine->SetBaseColor(FLinearColor::White);
	OtherModeBottomRightLine->SetBaseColor(FLinearColor::White);	
	OtherModeLeftTopLine->SetBaseColor(FLinearColor::White);
	OtherModeLeftBottomLine->SetBaseColor(FLinearColor::White);
	OtherModeRightTopLine->SetBaseColor(FLinearColor::White);
	OtherModeRightBottomLine->SetBaseColor(FLinearColor::White);
	OtherModeTopLeftLine->SetLineWidth(2.f);
	OtherModeBottomLeftLine->SetLineWidth(2.f);
	OtherModeTopRightLine->SetLineWidth(2.f);
	OtherModeBottomRightLine->SetLineWidth(2.f);
	OtherModeLeftTopLine->SetLineWidth(2.f);
	OtherModeLeftBottomLine->SetLineWidth(2.f);
	OtherModeRightTopLine->SetLineWidth(2.f);
	OtherModeRightBottomLine->SetLineWidth(2.f);	
	
	LeftPoint->SetState(FArmyPrimitive::OPS_Disable);
	RightPoint->SetState(FArmyPrimitive::OPS_Disable);
	LeftTopPoint->SetState(FArmyPrimitive::OPS_Disable);
	LeftBottomPoint->SetState(FArmyPrimitive::OPS_Disable);
	RightTopPoint->SetState(FArmyPrimitive::OPS_Disable);
	RightBottomPoint->SetState(FArmyPrimitive::OPS_Disable);
	DirectionPoint->SetState(FArmyPrimitive::OPS_Disable);

	TopRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmySimpleComponent::OnTopInputBoxCommitted));
	BottomRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmySimpleComponent::OnBottomInputBoxCommitted));
	LeftRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmySimpleComponent::OnLeftInputBoxCommitted));
	RightRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmySimpleComponent::OnRightInputBoxCommitted));

	/*TopRulerLine->SetUpdateShowInDraw(false);
	BottomRulerLine->SetUpdateShowInDraw(false);
	LeftRulerLine->SetUpdateShowInDraw(false);
	RightRulerLine->SetUpdateShowInDraw(false);*/

	TopRulerLine->ShowInputBox(false);
	BottomRulerLine->ShowInputBox(false);
	LeftRulerLine->ShowInputBox(false);
	RightRulerLine->ShowInputBox(false);
}

FArmySimpleComponent::FArmySimpleComponent(FArmySimpleComponent * Copy)
{
	ObjectType = Copy->ObjectType; 
	PropertyFlag = Copy->PropertyFlag;
		
	OtherModePolygon = MakeShareable(new FArmyPolygon(Copy->OtherModePolygon.Get()));

	LeftPoint = MakeShareable(new FArmyEditPoint(Copy->LeftPoint.Get()));
	RightPoint = MakeShareable(new FArmyEditPoint(Copy->RightPoint.Get()));
	TopPoint = MakeShareable(new FArmyEditPoint(Copy->TopPoint.Get()));
	BottomPoint = MakeShareable(new FArmyEditPoint(Copy->BottomPoint.Get()));
	LeftTopPoint = MakeShareable(new FArmyEditPoint(Copy->LeftTopPoint.Get()));
	LeftBottomPoint = MakeShareable(new FArmyEditPoint(Copy->LeftBottomPoint.Get()));
	RightTopPoint = MakeShareable(new FArmyEditPoint(Copy->RightTopPoint.Get()));
	RightBottomPoint = MakeShareable(new FArmyEditPoint(Copy->RightBottomPoint.Get()));
	Point = MakeShareable(new FArmyEditPoint(Copy->Point.Get()));
	DirectionPoint = MakeShareable(new FArmyEditPoint(Copy->DirectionPoint.Get()));

	TopLeftLine = MakeShareable(new FArmyLine(Copy->TopLeftLine.Get()));
	BottomLeftLine = MakeShareable(new FArmyLine(Copy->BottomLeftLine.Get()));
	TopRightLine = MakeShareable(new FArmyLine(Copy->TopRightLine.Get()));
	BottomRightLine = MakeShareable(new FArmyLine(Copy->BottomRightLine.Get()));
	LeftTopLine = MakeShareable(new FArmyLine(Copy->LeftTopLine.Get()));
	LeftBottomLine = MakeShareable(new FArmyLine(Copy->LeftBottomLine.Get()));
	RightTopLine = MakeShareable(new FArmyLine(Copy->RightTopLine.Get()));
	RightBottomLine = MakeShareable(new FArmyLine(Copy->RightBottomLine.Get()));

	OtherModeTopLeftLine = MakeShareable(new FArmyLine(Copy->OtherModeTopLeftLine.Get()));
	OtherModeBottomLeftLine = MakeShareable(new FArmyLine(Copy->OtherModeBottomLeftLine.Get()));
	OtherModeTopRightLine = MakeShareable(new FArmyLine(Copy->OtherModeTopRightLine.Get()));
	OtherModeBottomRightLine = MakeShareable(new FArmyLine(Copy->OtherModeBottomRightLine.Get()));	
	OtherModeLeftTopLine = MakeShareable(new FArmyLine(Copy->OtherModeLeftTopLine.Get()));
	OtherModeLeftBottomLine = MakeShareable(new FArmyLine(Copy->OtherModeLeftBottomLine.Get()));
	OtherModeRightTopLine = MakeShareable(new FArmyLine(Copy->OtherModeRightTopLine.Get()));
	OtherModeRightBottomLine = MakeShareable(new FArmyLine(Copy->OtherModeRightBottomLine.Get()));

	TopRulerLine = (MakeShareable(new FArmyRulerLine()));
	BottomRulerLine = (MakeShareable(new FArmyRulerLine()));
	LeftRulerLine = (MakeShareable(new FArmyRulerLine()));
	RightRulerLine = (MakeShareable(new FArmyRulerLine()));
	TopRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmySimpleComponent::OnTopInputBoxCommitted));
	BottomRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmySimpleComponent::OnBottomInputBoxCommitted));
	LeftRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmySimpleComponent::OnLeftInputBoxCommitted));
	RightRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmySimpleComponent::OnRightInputBoxCommitted));
	TopRulerLine->SetUpdateShowInDraw(false);
	BottomRulerLine->SetUpdateShowInDraw(false);
	LeftRulerLine->SetUpdateShowInDraw(false);
	RightRulerLine->SetUpdateShowInDraw(false);

	//设置绘制数据偏移
	TopLeftLine->SetDrawDataOffset(FVector(0, 0, 3.f));
	BottomLeftLine->SetDrawDataOffset(FVector(0, 0, 3.f));
	TopRightLine->SetDrawDataOffset(FVector(0, 0, 3.f));
	BottomRightLine->SetDrawDataOffset(FVector(0, 0, 3.f));
	LeftTopLine->SetDrawDataOffset(FVector(0, 0, 3.f));
	LeftBottomLine->SetDrawDataOffset(FVector(0, 0, 3.f));
	RightTopLine->SetDrawDataOffset(FVector(0, 0, 3.f));
	RightBottomLine->SetDrawDataOffset(FVector(0, 0, 3.f));

	Length = Copy->Length;
	Width = Copy->Width;
	Height = Copy->Height;
	Direction = Copy->Direction;
	HorizontalDirection = Copy->HorizontalDirection;
	bAutoAdapt = Copy->bAutoAdapt;
	bAlwaysShowDashLine = Copy->bAlwaysShowDashLine;
	RelatedRoom = Copy->RelatedRoom;
	Angle = Copy->Angle;

	bIsCopy = true;

	SetName(Copy->GetName());


	Update();
}

FArmySimpleComponent::~FArmySimpleComponent()
{
}

void FArmySimpleComponent::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);

	JsonWriter->WriteValue("pos", GetPos().ToString());
	JsonWriter->WriteValue("LeftToppos", GetLeftTopPos().ToString());
	JsonWriter->WriteValue("LeftBottompos", GetLeftBottomPos().ToString());
	JsonWriter->WriteValue("RightToppos", GetRightTopPos().ToString());
	JsonWriter->WriteValue("RightBottompos", GetRightBottomPos().ToString());
	JsonWriter->WriteValue("Toppos", GetTopPos().ToString());
	JsonWriter->WriteValue("Bottompos", GetBottomPos().ToString());
	JsonWriter->WriteValue("Leftpos", GetLeftPos().ToString());
	JsonWriter->WriteValue("Rightpos", GetRightPos().ToString());
	JsonWriter->WriteValue("direction", GetDirection().ToString());
	JsonWriter->WriteValue("length", GetLength());
	JsonWriter->WriteValue("width", GetWidth());
	JsonWriter->WriteValue("height", GetHeight());
	JsonWriter->WriteValue("angle", GetAngle());
	JsonWriter->WriteValue("bAutoAdapt", GetAutoAdapt());	
}

void FArmySimpleComponent::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);
	bIsCopy = true;
	
	FVector Pos, Direction;
	Pos.InitFromString(InJsonData->GetStringField("pos"));
	Direction.InitFromString(InJsonData->GetStringField("direction"));

	Point->SetPos(Pos);
	this->Direction = Direction;
	Length = InJsonData->GetNumberField("length");
	Width = InJsonData->GetNumberField("Width");
	Height = InJsonData->GetNumberField("height");
	Angle = InJsonData->GetNumberField("angle");

	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(bIsCopyData ? E_ModifyModel : E_HomeModel, OT_InternalRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		TArray<TSharedPtr<FArmyLine>> RoomLines;
		if (Room.IsValid())
		{
			if (Room->IsPointInRoom(Pos))
			{
				SetRelatedRoom(Room);
				break;
			}
		}
	}

	bAutoAdapt = InJsonData->GetBoolField("bAutoAdapt");

	Pos.InitFromString(InJsonData->GetStringField("LeftToppos"));
	SetLeftTopPos(Pos);
	Pos.InitFromString(InJsonData->GetStringField("LeftBottompos"));
	SetLeftBottomPos(Pos);
	Pos.InitFromString(InJsonData->GetStringField("RightToppos"));
	SetRightTopPos(Pos);
	Pos.InitFromString(InJsonData->GetStringField("RightBottompos"));
	SetRightBottomPos(Pos);
	Pos.InitFromString(InJsonData->GetStringField("Toppos"));
	SetTopPos(Pos);
	Pos.InitFromString(InJsonData->GetStringField("Bottompos"));
	SetBottomPos(Pos);
	Pos.InitFromString(InJsonData->GetStringField("Leftpos"));
	SetLeftPos(Pos);
	Pos.InitFromString(InJsonData->GetStringField("Rightpos"));
	SetRightPos(Pos);

	Update();
	//AutoAdapt();

	if (GetPropertyFlag(FArmyObject::FLAG_MODIFY))
	{
		TopRulerLine->SetEnableInputBox(false);
		BottomRulerLine->SetEnableInputBox(false);
		LeftRulerLine->SetEnableInputBox(false);
		RightRulerLine->SetEnableInputBox(false);

		LeftPoint->SetState(FArmyPrimitive::OPS_Disable);
		RightPoint->SetState(FArmyPrimitive::OPS_Disable);
		TopPoint->SetState(FArmyPrimitive::OPS_Disable);
		BottomPoint->SetState(FArmyPrimitive::OPS_Disable);
		Point->SetState(FArmyPrimitive::OPS_Disable);
		LeftTopPoint->SetState(FArmyPrimitive::OPS_Disable);
		LeftBottomPoint->SetState(FArmyPrimitive::OPS_Disable);
		RightTopPoint->SetState(FArmyPrimitive::OPS_Disable);
		RightBottomPoint->SetState(FArmyPrimitive::OPS_Disable);
	}

	TopRulerLine->SetUpdateShowInDraw(false);
	BottomRulerLine->SetUpdateShowInDraw(false);
	LeftRulerLine->SetUpdateShowInDraw(false);
	RightRulerLine->SetUpdateShowInDraw(false);
}

void FArmySimpleComponent::SetState(EObjectState InState)
{
	State = InState;

	switch (InState)
	{
	case OS_Normal:
		LeftTopLine->SetBaseColor(FLinearColor::White);
		LeftBottomLine->SetBaseColor(FLinearColor::White);
		RightTopLine->SetBaseColor(FLinearColor::White);
		RightBottomLine->SetBaseColor(FLinearColor::White);
		TopLeftLine->SetBaseColor(FLinearColor::White);
		BottomLeftLine->SetBaseColor(FLinearColor::White);
		TopRightLine->SetBaseColor(FLinearColor::White);
		BottomRightLine->SetBaseColor(FLinearColor::White);
		DeselectPoints();
		break;

	case OS_Hovered:
	case OS_Selected:
		LeftTopLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		LeftBottomLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		RightTopLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		RightBottomLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		TopLeftLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		BottomLeftLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));		
		TopRightLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		BottomRightLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		UpdateDashLine();
		break;

	case OS_Disable:
		LeftTopLine->SetBaseColor(FLinearColor::Gray);
		LeftBottomLine->SetBaseColor(FLinearColor::Gray);
		RightTopLine->SetBaseColor(FLinearColor::Gray);
		RightBottomLine->SetBaseColor(FLinearColor::Gray);
		TopLeftLine->SetBaseColor(FLinearColor::Gray);
		BottomRightLine->SetBaseColor(FLinearColor::Gray);
		TopLeftLine->SetBaseColor(FLinearColor::Gray);
		BottomRightLine->SetBaseColor(FLinearColor::Gray);
		break;

	default:
		break;
	}
}

void FArmySimpleComponent::Draw(FPrimitiveDrawInterface * PDI, const FSceneView * View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		TopLeftLine->Draw(PDI, View);
		BottomLeftLine->Draw(PDI, View);
		TopRightLine->Draw(PDI, View);
		BottomRightLine->Draw(PDI, View);
		LeftTopLine->Draw(PDI, View);
		LeftBottomLine->Draw(PDI, View);
		RightTopLine->Draw(PDI, View);
		RightBottomLine->Draw(PDI, View);
		if (State == OS_Selected)
		{										
			LeftPoint->Draw(PDI, View);
			RightPoint->Draw(PDI, View);
			TopPoint->Draw(PDI, View);
			BottomPoint->Draw(PDI, View);
			Point->Draw(PDI, View);
			LeftTopPoint->Draw(PDI, View);
			LeftBottomPoint->Draw(PDI, View);
			RightTopPoint->Draw(PDI, View);
			RightBottomPoint->Draw(PDI, View);

			UpdateDashLine();
			DrawDashLine(PDI, View);
		}		
	}
}

bool FArmySimpleComponent::IsSelected(const FVector & Pos, UArmyEditorViewportClient * InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{		
		FVector TempPos = Pos;
		TempPos.Z = 0;
		TArray<FVector> Points;
		Points.Add(GetLeftTopPos());
		Points.Add(GetRightTopPos());
		Points.Add(GetRightBottomPos());
		Points.Add(GetLeftBottomPos());
		return FArmyMath::IsPointInOrOnPolygon2D(TempPos, Points);
	}
	return false;
}

TSharedPtr<FArmyEditPoint> FArmySimpleComponent::SelectPoint(const FVector & Pos, UArmyEditorViewportClient * InViewportClient)
{
	ResultPoint = NULL;

	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		DeselectPoints();
	}

	if (GetState() == OS_Selected)
	{
		if (LeftPoint->IsSelected(Pos, InViewportClient) && LeftPoint->GetState() != FArmyEditPoint::OPS_Disable)            ResultPoint = LeftPoint;
		else if (RightPoint->IsSelected(Pos, InViewportClient) && RightPoint->GetState() != FArmyEditPoint::OPS_Disable)         ResultPoint = RightPoint;
		else if (TopPoint->IsSelected(Pos, InViewportClient) && TopPoint->GetState() != FArmyEditPoint::OPS_Disable)            ResultPoint = TopPoint;
		else if (BottomPoint->IsSelected(Pos, InViewportClient) && BottomPoint->GetState() != FArmyEditPoint::OPS_Disable)   ResultPoint = BottomPoint;
		else if (Point->IsSelected(Pos, InViewportClient) && Point->GetState() != FArmyEditPoint::OPS_Disable)   ResultPoint = Point;
		else if (LeftTopPoint->IsSelected(Pos, InViewportClient) && LeftTopPoint->GetState() != FArmyEditPoint::OPS_Disable)   ResultPoint = LeftTopPoint;
		else if (LeftBottomPoint->IsSelected(Pos, InViewportClient) && LeftBottomPoint->GetState() != FArmyEditPoint::OPS_Disable)   ResultPoint = LeftBottomPoint;
		else if (RightTopPoint->IsSelected(Pos, InViewportClient) && RightTopPoint->GetState() != FArmyEditPoint::OPS_Disable)   ResultPoint = RightTopPoint;
		else if (RightBottomPoint->IsSelected(Pos, InViewportClient) && RightBottomPoint->GetState() != FArmyEditPoint::OPS_Disable)   ResultPoint = RightBottomPoint;

		if (ResultPoint.IsValid())                  ResultPoint->SetState(FArmyEditPoint::OPS_Selected);
	}

	return ResultPoint;
}

TSharedPtr<FArmyEditPoint> FArmySimpleComponent::HoverPoint(const FVector & Pos, UArmyEditorViewportClient * InViewportClient)
{
	/*@欧石楠 这里将选中的点存下来**/
	ResultPoint = NULL;

	if (TopPoint->GetState() != FArmyEditPoint::OPS_Selected && TopPoint->GetState() != FArmyEditPoint::OPS_Disable && TopPoint->IsSelected(Pos, InViewportClient))
	{
		ResultPoint = TopPoint;
		OperationPointType = ESCS_TOP;
	}
	else if (BottomPoint->GetState() != FArmyEditPoint::OPS_Selected && BottomPoint->GetState() != FArmyEditPoint::OPS_Disable && BottomPoint->IsSelected(Pos, InViewportClient))
	{
		ResultPoint = BottomPoint;
		OperationPointType = ESCS_BOTTOM;
	}
	else if (LeftPoint->GetState() != FArmyEditPoint::OPS_Selected && LeftPoint->GetState() != FArmyEditPoint::OPS_Disable && LeftPoint->IsSelected(Pos, InViewportClient))
	{
		ResultPoint = LeftPoint;
		OperationPointType = ESCS_LEFT;
	}
	else if (RightPoint->GetState() != FArmyEditPoint::OPS_Selected && RightPoint->GetState() != FArmyEditPoint::OPS_Disable && RightPoint->IsSelected(Pos, InViewportClient))
	{
		ResultPoint = RightPoint;
		OperationPointType = ESCS_RIGHT;
	}
	else if (Point->GetState() != FArmyEditPoint::OPS_Selected && Point->GetState() != FArmyEditPoint::OPS_Disable && Point->IsSelected(Pos, InViewportClient))
	{
		ResultPoint = Point;
		OperationPointType = ESCS_CENTER;
	}
	else
	{
		OperationPointType = ESCS_NONE;
	}

	return ResultPoint;
}

void FArmySimpleComponent::Delete()
{
	FArmyObject::Delete();
}

const FBox FArmySimpleComponent::GetBounds()
{
	FBox V(ForceInitToZero);

	return V;
}

void FArmySimpleComponent::GetCadLineArray(TArray<TPair<FVector, FVector>>& OutLineList) const
{
	OutLineList.Append({
		TPair<FVector,FVector>(TopLeftLine->GetStart(),TopLeftLine->GetEnd()),
		TPair<FVector,FVector>(BottomLeftLine->GetStart(),BottomLeftLine->GetEnd()),
		TPair<FVector,FVector>(TopRightLine->GetStart(),TopRightLine->GetEnd()),
		TPair<FVector,FVector>(BottomRightLine->GetStart(),BottomRightLine->GetEnd()),

		TPair<FVector,FVector>(LeftTopLine->GetStart(),LeftTopLine->GetEnd()),
		TPair<FVector,FVector>(LeftBottomLine->GetStart(),LeftBottomLine->GetEnd()),
		TPair<FVector,FVector>(RightTopLine->GetStart(),RightTopLine->GetEnd()),
		TPair<FVector,FVector>(RightBottomLine->GetStart(),RightBottomLine->GetEnd()),
	});
}

void FArmySimpleComponent::GetVertexes(TArray<FVector>& OutVertexes)
{
	FVector UpLeft = LeftTopPoint->GetPos();
	FVector Top = TopPoint->GetPos();
	FVector UpRight = RightTopPoint->GetPos();
	FVector Right = RightPoint->GetPos();
	FVector DownRight = RightBottomPoint->GetPos();
	FVector Bottom = BottomPoint->GetPos();
	FVector DownLeft = LeftBottomPoint->GetPos();
	FVector Left = LeftPoint->GetPos();

	OutVertexes.Add(UpLeft);
	OutVertexes.Add(Top);
	OutVertexes.Add(UpRight);
	OutVertexes.Add(Right);
	OutVertexes.Add(DownRight);
	OutVertexes.Add(Bottom);
	OutVertexes.Add(DownLeft);
	OutVertexes.Add(Left);
}

void FArmySimpleComponent::Get4Vertexes(TArray<FVector>& OutVertexes)
{
	FVector UpLeft = LeftTopPoint->GetPos();	
	FVector UpRight = RightTopPoint->GetPos();	
	FVector DownRight = RightBottomPoint->GetPos();	
	FVector DownLeft = LeftBottomPoint->GetPos();	

	OutVertexes.Add(UpLeft);	
	OutVertexes.Add(UpRight);	
	OutVertexes.Add(DownRight);	
	OutVertexes.Add(DownLeft);	
}

void FArmySimpleComponent::Generate(UWorld* InWorld)
{

}

bool FArmySimpleComponent::IsAttachedToRoom()
{
	if (RelatedRoom.IsValid())
	{
		TArray<TSharedPtr<FArmyLine>> OutLines;
		TArray<FVector> OutVertexes;
		RelatedRoom.Pin()->GetLines(OutLines);
		GetVertexes(OutVertexes);
		for (auto It : OutLines)
		{
			for (auto ItPoint : OutVertexes)
			{
				if (FArmyMath::IsPointOnLine(ItPoint, It->GetStart(), It->GetEnd(), 0.01f))
				{
					return true;
				}
			}			
		}
	}
	return false;
}

void FArmySimpleComponent::GetLines(TArray<TSharedPtr<FArmyLine>>& OutLines, bool IsAbs)
{	
	OutLines.Add(TopLeftLine);
	OutLines.Add(BottomLeftLine);
	OutLines.Add(TopRightLine);
	OutLines.Add(BottomRightLine);
	OutLines.Add(LeftTopLine);
	OutLines.Add(LeftBottomLine);
	OutLines.Add(RightTopLine);
	OutLines.Add(RightBottomLine);
}

void FArmySimpleComponent::SetPropertyFlag(PropertyType InType, bool InUse)
{
	FArmyObject::SetPropertyFlag(InType, InUse);
	if (InType == FArmyObject::FLAG_MODIFY && InUse)
	{		
		TopRulerLine->SetEnableInputBox(false);
		BottomRulerLine->SetEnableInputBox(false);
		LeftRulerLine->SetEnableInputBox(false);
		RightRulerLine->SetEnableInputBox(false);

		LeftPoint->SetState(FArmyPrimitive::OPS_Disable);
		RightPoint->SetState(FArmyPrimitive::OPS_Disable);
		TopPoint->SetState(FArmyPrimitive::OPS_Disable);
		BottomPoint->SetState(FArmyPrimitive::OPS_Disable);
		Point->SetState(FArmyPrimitive::OPS_Disable);
		LeftTopPoint->SetState(FArmyPrimitive::OPS_Disable);
		LeftBottomPoint->SetState(FArmyPrimitive::OPS_Disable);
		RightTopPoint->SetState(FArmyPrimitive::OPS_Disable);
		RightBottomPoint->SetState(FArmyPrimitive::OPS_Disable);		
	}	
}

void FArmySimpleComponent::Update()
{
	HorizontalDirection = Direction.RotateAngleAxis(90, FVector(0, 0, 1));	
	
	//如果不是自适应就按照长宽来组织数据
	if (!RelatedRoom.IsValid() || !bAutoAdapt)
	{
		LeftTopPoint->SetPos(GetPos() - Direction * Length / 2.f - HorizontalDirection * Width / 2.f);
		RightTopPoint->SetPos(GetPos() + Direction * Length / 2.f - HorizontalDirection * Width / 2.f);

		LeftBottomPoint->SetPos(GetPos() - Direction * Length / 2.f + HorizontalDirection * Width / 2.f);
		RightBottomPoint->SetPos(GetPos() + Direction * Length / 2.f + HorizontalDirection * Width / 2.f);

		LeftPoint->SetPos(GetPos() - Direction * Length / 2.f);
		RightPoint->SetPos(GetPos() + Direction * Length / 2.f);
	}		
	
	TopPoint->SetPos(GetPos() - HorizontalDirection * Width / 2.0f);
	BottomPoint->SetPos(GetPos() + HorizontalDirection * Width / 2.0f);	

	TopLeftLine->SetStart(LeftTopPoint->GetPos());
	TopLeftLine->SetEnd(TopPoint->GetPos());
	TopRightLine->SetStart(TopPoint->GetPos());
	TopRightLine->SetEnd(RightTopPoint->GetPos());

	BottomLeftLine->SetStart(LeftBottomPoint->GetPos());
	BottomLeftLine->SetEnd(BottomPoint->GetPos());
	BottomRightLine->SetStart(BottomPoint->GetPos());
	BottomRightLine->SetEnd(RightBottomPoint->GetPos());

	LeftTopLine->SetStart(LeftTopPoint->GetPos());
	LeftTopLine->SetEnd(LeftPoint->GetPos());
	LeftBottomLine->SetStart(LeftPoint->GetPos());
	LeftBottomLine->SetEnd(LeftBottomPoint->GetPos());
	RightTopLine->SetStart(RightTopPoint->GetPos());
	RightTopLine->SetEnd(RightPoint->GetPos());
	RightBottomLine->SetStart(RightPoint->GetPos());
	RightBottomLine->SetEnd(RightBottomPoint->GetPos());
	
	Width = (TopPoint->GetPos() - BottomPoint->GetPos()).Size();
	Length = (GetLeftPos() - GetRightPos()).Size();
	Width = FMath::RoundToFloat(Width * 100.f) / 100.f;
	Length = FMath::RoundToFloat(Length * 100.f) / 100.f;

	//其他模式额外数据
	int32 TopViewZ0 = FArmySceneData::WallHeight + 25.f;
	FVector TempPos = LeftTopPoint->GetPos();
	TempPos.Z = TopViewZ0;
	OtherModeTopLeftLine->SetStart(TempPos);

	TempPos = GetTopPos();
	TempPos.Z = TopViewZ0;
	OtherModeTopLeftLine->SetEnd(TempPos);

	TempPos = GetTopPos();
	TempPos.Z = TopViewZ0;
	OtherModeTopRightLine->SetStart(TempPos);

	TempPos = GetRightTopPos();
	TempPos.Z = TopViewZ0;
	OtherModeTopRightLine->SetEnd(TempPos);

	TempPos = GetLeftBottomPos();
	TempPos.Z = TopViewZ0;
	OtherModeBottomLeftLine->SetStart(TempPos);

	TempPos = GetBottomPos();
	TempPos.Z = TopViewZ0;
	OtherModeBottomLeftLine->SetEnd(TempPos);

	TempPos = GetBottomPos();
	TempPos.Z = TopViewZ0;
	OtherModeBottomRightLine->SetStart(TempPos);

	TempPos = GetRightBottomPos();
	TempPos.Z = TopViewZ0;
	OtherModeBottomRightLine->SetEnd(TempPos);

	TempPos = LeftTopPoint->GetPos();
	TempPos.Z = TopViewZ0;
	OtherModeLeftTopLine->SetStart(TempPos);

	TempPos = LeftPoint->GetPos();
	TempPos.Z = TopViewZ0;
	OtherModeLeftTopLine->SetEnd(TempPos);

	TempPos = LeftPoint->GetPos();
	TempPos.Z = TopViewZ0;
	OtherModeLeftBottomLine->SetStart(TempPos);

	TempPos = LeftBottomPoint->GetPos();
	TempPos.Z = TopViewZ0;
	OtherModeLeftBottomLine->SetEnd(TempPos);

	TempPos = RightTopPoint->GetPos();
	TempPos.Z = TopViewZ0;
	OtherModeRightTopLine->SetStart(TempPos);

	TempPos = RightPoint->GetPos();
	TempPos.Z = TopViewZ0;
	OtherModeRightTopLine->SetEnd(TempPos);

	TempPos = RightPoint->GetPos();
	TempPos.Z = TopViewZ0;
	OtherModeRightBottomLine->SetStart(TempPos);

	TempPos = RightBottomPoint->GetPos();
	TempPos.Z = TopViewZ0;
	OtherModeRightBottomLine->SetEnd(TempPos);
}

void FArmySimpleComponent::UpdateTopPoint(FVector Pos)
{
	FVector ProjectionPos = FArmyMath::GetProjectionPoint(Pos, GetBottomPos(), GetTopPos());
	TopPoint->SetPos(ProjectionPos);
	HorizontalDirection = Direction.RotateAngleAxis(90, FVector(0, 0, 1));
	float TempWidth = (TopPoint->GetPos() - BottomPoint->GetPos()).Size();
	FVector TempPos = Point->GetPos() - HorizontalDirection * (TempWidth - Width) / 2.f;	
	Point->SetPos(TempPos);	
	Width = TempWidth;
	AutoAdapt();
}

void FArmySimpleComponent::UpdateBottomPoint(FVector Pos)
{
	FVector ProjectionPos = FArmyMath::GetProjectionPoint(Pos, GetBottomPos(), GetTopPos());
	BottomPoint->SetPos(ProjectionPos);
	HorizontalDirection = Direction.RotateAngleAxis(90, FVector(0, 0, 1));
	float TempWidth = (TopPoint->GetPos() - BottomPoint->GetPos()).Size();
	FVector TempPos = Point->GetPos() + HorizontalDirection * (TempWidth - Width) / 2.f;	
	Point->SetPos(TempPos);
	Width = TempWidth;
	AutoAdapt();
}

void FArmySimpleComponent::UpdateLeftPoint(FVector Pos)
{
	FVector ProjectionPos = FArmyMath::GetProjectionPoint(Pos, GetLeftPos(), GetRightPos());
	LeftPoint->SetPos(ProjectionPos);
	float TempLength = (LeftPoint->GetPos() - RightPoint->GetPos()).Size();
	FVector TempPos = Point->GetPos() - Direction * (TempLength - Length) / 2.f;
	Point->SetPos(TempPos);
	Length = TempLength;	
	AutoAdapt();
}

void FArmySimpleComponent::UpdateRightPoint(FVector Pos)
{
	FVector ProjectionPos = FArmyMath::GetProjectionPoint(Pos, GetLeftPos(), GetRightPos());
	RightPoint->SetPos(ProjectionPos);
	float TempLength = (LeftPoint->GetPos() - RightPoint->GetPos()).Size();
	FVector TempPos = Point->GetPos() + Direction * (TempLength - Length) / 2.f;
	Point->SetPos(TempPos);
	Length = TempLength;
	AutoAdapt();
}

void FArmySimpleComponent::OnDelete()
{
	DeselectPoints();
}

void FArmySimpleComponent::AutoAdapt()
{	
	
}

void FArmySimpleComponent::AutoAttach(float Invalue)
{		
	if (!RelatedRoom.IsValid())
	{
		return;
	}
	float TempValue;
	float MaxDistance = 10.f;
	TempValue = TopRulerLine->GetInputBoxCachedLength();
	if (TempValue < MaxDistance)
	{
		OnTopInputBoxCommitted(FText::AsNumber(Invalue), ETextCommit::OnEnter);
	}
	TempValue = BottomRulerLine->GetInputBoxCachedLength();
	if (TempValue < MaxDistance)
	{
		OnBottomInputBoxCommitted(FText::AsNumber(Invalue), ETextCommit::OnEnter);
	}
	if (!bAutoAdapt)
	{
		TempValue = LeftRulerLine->GetInputBoxCachedLength();
		if (TempValue < MaxDistance)
		{
			OnLeftInputBoxCommitted(FText::AsNumber(Invalue), ETextCommit::OnEnter);
		}
		TempValue = RightRulerLine->GetInputBoxCachedLength();
		if (TempValue < MaxDistance)
		{
			OnRightInputBoxCommitted(FText::AsNumber(Invalue), ETextCommit::OnEnter);
		}
	}
	UpdateDashLine();
}

const FVector & FArmySimpleComponent::GetLeftPos()
{
	// TODO: insert return statement here
	return LeftPoint->GetPos();
}

const FVector & FArmySimpleComponent::GetRightPos()
{
	// TODO: insert return statement here
	return RightPoint->GetPos();
}

const FVector & FArmySimpleComponent::GetTopPos()
{
	// TODO: insert return statement here
	return TopPoint->GetPos();
}

const FVector & FArmySimpleComponent::GetBottomPos()
{
	// TODO: insert return statement here
	return BottomPoint->GetPos();
}

const FVector & FArmySimpleComponent::GetLeftTopPos()
{
	// TODO: insert return statement here
	return LeftTopPoint->GetPos();
}

const FVector & FArmySimpleComponent::GetLeftBottomPos()
{
	// TODO: insert return statement here
	return LeftBottomPoint->GetPos();
}

const FVector & FArmySimpleComponent::GetRightTopPos()
{
	// TODO: insert return statement here
	return RightTopPoint->GetPos();
}

const FVector & FArmySimpleComponent::GetRightBottomPos()
{
	// TODO: insert return statement here
	return RightBottomPoint->GetPos();
}

const FVector & FArmySimpleComponent::GetPos()
{
	// TODO: insert return statement here
	return Point->GetPos();
}


void FArmySimpleComponent::SetPos(const FVector & InPos)
{
	if (Point->GetPos() != InPos)
	{
		Point->SetPos(InPos);
		Update();
	}
}

void FArmySimpleComponent::SetDirection(const FVector & InDirection)
{
	if (Direction != InDirection)
	{
		Direction = InDirection;		
		Angle = FMath::RoundToInt(FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Direction, FVector(1, 0, 0)))));
		Update();
		AutoAdapt();
	}	
}

void FArmySimpleComponent::SetLength(const float & InLength)
{
	if (Length != InLength)
	{
		Length = InLength;
		Update();
	}	
}

void FArmySimpleComponent::SetWidth(const float & InWidth)
{
	if (Width != InWidth)
	{
		Width = InWidth;
		Update();
	}
}

void FArmySimpleComponent::SetHeight(const float & InHeight)
{
	if (Height != InHeight)
	{
		Height = InHeight;
	}
}

void FArmySimpleComponent::SetAngle(const float & InAngle)
{
	Direction = FVector(1, 0, 0).RotateAngleAxis(InAngle, FVector(0, 0, 1));
	Angle = InAngle;
	Update();
}

ESCState FArmySimpleComponent::GetOperationPointType() const
{
	return OperationPointType;
}

void FArmySimpleComponent::SetOperationPointType(ESCState Type)
{
	OperationPointType = Type;
	DeselectPoints();
}

void FArmySimpleComponent::CalcRelatedRoom()
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
}

void FArmySimpleComponent::SetAutoAdapt(bool bValue)
{
	{ bAutoAdapt = bValue; }
}

void FArmySimpleComponent::SetAutoAdaptDir(bool bValue)
{
	bAutoAdaptDir = bValue;
}

void FArmySimpleComponent::DeselectPoints()
{
	Point->SetState(Point->GetState() != FArmyPrimitive::OPS_Disable ? FArmyPrimitive::OPS_Normal : FArmyPrimitive::OPS_Disable);
	TopPoint->SetState(TopPoint->GetState() != FArmyPrimitive::OPS_Disable ? FArmyPrimitive::OPS_Normal : FArmyPrimitive::OPS_Disable);
	LeftPoint->SetState(LeftPoint->GetState() != FArmyPrimitive::OPS_Disable ? FArmyPrimitive::OPS_Normal : FArmyPrimitive::OPS_Disable);
	RightPoint->SetState(RightPoint->GetState() != FArmyPrimitive::OPS_Disable ? FArmyPrimitive::OPS_Normal : FArmyPrimitive::OPS_Disable);
	BottomPoint->SetState(BottomPoint->GetState() != FArmyPrimitive::OPS_Disable ? FArmyPrimitive::OPS_Normal : FArmyPrimitive::OPS_Disable);
	LeftTopPoint->SetState(LeftTopPoint->GetState() != FArmyPrimitive::OPS_Disable ? FArmyPrimitive::OPS_Normal : FArmyPrimitive::OPS_Disable);
	RightTopPoint->SetState(RightTopPoint->GetState() != FArmyPrimitive::OPS_Disable ? FArmyPrimitive::OPS_Normal : FArmyPrimitive::OPS_Disable);
	DirectionPoint->SetState(DirectionPoint->GetState() != FArmyPrimitive::OPS_Disable ? FArmyPrimitive::OPS_Normal : FArmyPrimitive::OPS_Disable);
	LeftBottomPoint->SetState(LeftBottomPoint->GetState() != FArmyPrimitive::OPS_Disable ? FArmyPrimitive::OPS_Normal : FArmyPrimitive::OPS_Disable);
	RightBottomPoint->SetState(RightBottomPoint->GetState() != FArmyPrimitive::OPS_Disable ? FArmyPrimitive::OPS_Normal : FArmyPrimitive::OPS_Disable);

	HideRuleLineInputBox(true);
}

bool FArmySimpleComponent::CheckAllPointsInRoom(FVector Pos)
{
	return true;
}

void FArmySimpleComponent::HideRuleLineInputBox(bool bHide)
{

	TopRulerLine->ShowInputBox(!bHide);
	BottomRulerLine->ShowInputBox(!bHide);
	if (!bAutoAdapt)
	{
		LeftRulerLine->ShowInputBox(!bHide);
		RightRulerLine->ShowInputBox(!bHide);
	}	
}

// 获得立面投影box
TArray<struct FLinesInfo> FArmySimpleComponent::GetFacadeBox(TSharedPtr<class FArmyLine> InLine)
{
	TArray<FLinesInfo> FacadeBox;
	if (!InLine.IsValid())
	{
		return FacadeBox;
	}
	TArray<FVector> inside_vertices;
	inside_vertices.Emplace(LeftTopPoint->GetPos());
	inside_vertices.Emplace(RightTopPoint->GetPos());
	inside_vertices.Emplace(RightBottomPoint->GetPos());
	inside_vertices.Emplace(LeftBottomPoint->GetPos());

	TArray<FVector> Intersections;
	TArray<FVector> TmpVertexes;
	// 获得墙线与包围盒的交点
	if (FArmyMath::CalLinePolygonIntersection(InLine->GetStart(), InLine->GetEnd(), inside_vertices, Intersections))
	{
		if (Intersections.Num() != 2)
		{
			return FacadeBox;
		}
		else
		{
			TmpVertexes = Intersections;
		}
	}
	else
	{
		if (FArmyMath::CalLinePolygonCoincidence(InLine->GetStart(), InLine->GetEnd(), inside_vertices, Intersections))
		{
			if (Intersections.Num() != 2)
			{
				return FacadeBox;
			}
			else
			{
				TmpVertexes = Intersections;
			}
		}
	}

	if (TmpVertexes.Num() == 2)
	{
		FLinesInfo pack_pipe;
		pack_pipe.Color = FLinearColor(0.0f, 1.0f, 1.0f);
		pack_pipe.DrawType = 0;
		pack_pipe.LineType = 0;

		FVector BaseZ = FVector(0, 0, 1);
		FVector FirstBottom = TmpVertexes[0];
		FVector SecondBottom = TmpVertexes[1];
		FVector FirstTop = FirstBottom + BaseZ*GetHeight();
		FVector SecondTop = SecondBottom + BaseZ*GetHeight();

		pack_pipe.Vertices.Emplace(FirstBottom);
		pack_pipe.Vertices.Emplace(FirstTop);
		pack_pipe.Vertices.Emplace(SecondTop);
		pack_pipe.Vertices.Emplace(SecondBottom);
		FacadeBox.Emplace(pack_pipe);
	}

	return FacadeBox;
}

void FArmySimpleComponent::UpdateDashLine()
{
	if (!RelatedRoom.IsValid())
	{
		return;
	}	
			
	TArray<TSharedPtr<FArmyLine>> Lines;
	GetRelatedRoom().Pin()->GetLines(Lines);
	//找到横向每个方向两个到关联房间的所有线段最近的线线段的交点

	FVector TempLineDirection;
	FVector TempDestPos;
	float MaxDist = 10000;

	//默认一个最大距离的交叉点
	FVector TopCrossPoint = (GetTopPos() - GetPos()).GetSafeNormal() * MaxDist;	
	FVector BottomCrossPoint = (GetBottomPos() - GetPos()).GetSafeNormal() * MaxDist;
	FVector LeftCrossPoint = (GetLeftPos() - GetPos()).GetSafeNormal() * MaxDist;	
	FVector RightCrossPoint = (GetRightPos() - GetPos()).GetSafeNormal() * MaxDist;	

	for (auto It : Lines)
	{
		FVector TempCrossPos = FVector::ZeroVector;
		FVector TempLineStart = It->GetStart();
		FVector TempLineEnd = It->GetEnd();
		FVector TempTopPos = GetTopPos();
		FVector TempBottomPos = GetBottomPos();
		FVector TempLeftPos = GetLeftPos();
		FVector TempRightPos = GetRightPos();
		TempLineStart.Z = 0.f;
		TempLineEnd.Z = 0.f;
		TempTopPos.Z = 0.f;
		TempBottomPos.Z = 0.f;
		if (!bAutoAdapt || bAlwaysShowDashLine)
		{
			//left
			TempLineDirection = (GetLeftPos() - GetPos()).GetSafeNormal();						
			TempDestPos = GetLeftPos() + TempLineDirection * MaxDist;
			TempDestPos.Z = 0.f;
			if (FArmyMath::SegmentIntersection2D(TempLineStart, TempLineEnd, TempLeftPos, TempDestPos, TempCrossPos))
			{
				//找到最近的交点
				if (FVector::Distance(LeftCrossPoint, TempLeftPos) >
					FVector::Distance(TempCrossPos, TempLeftPos))
				{
					LeftCrossPoint = TempCrossPos;
				}
			}

			//right
			TempLineDirection = (GetRightPos() - GetPos()).GetSafeNormal();
			TempDestPos = GetRightPos() + TempLineDirection * MaxDist;
			TempDestPos.Z = 0.f;
			if (FArmyMath::SegmentIntersection2D(TempLineStart, TempLineEnd, TempRightPos, TempDestPos, TempCrossPos))
			{
				if (FVector::Distance(RightCrossPoint, TempRightPos) >
					FVector::Distance(TempCrossPos, TempRightPos))
				{
					RightCrossPoint = TempCrossPos;
				}
			}
		}		

		//top 
		TempLineDirection = (GetTopPos() - GetPos()).GetSafeNormal();
		TempDestPos = GetTopPos() + TempLineDirection * MaxDist;
		TempDestPos.Z = 0.f;
		if (FArmyMath::SegmentIntersection2D(TempLineStart, TempLineEnd, TempTopPos, TempDestPos, TempCrossPos))
		{
			//找到最近的交点
			if (FVector::Distance(TopCrossPoint, TempTopPos) >
				FVector::Distance(TempCrossPos, TempTopPos))
			{
				TopCrossPoint = TempCrossPos;				
			}
		}		
		
		//bottom 
		TempLineDirection = (GetBottomPos() - GetPos()).GetSafeNormal();
		TempDestPos = GetBottomPos() + TempLineDirection * MaxDist;
		TempDestPos.Z = 0.f;
		if (FArmyMath::SegmentIntersection2D(TempLineStart, TempLineEnd, TempBottomPos, TempDestPos, TempCrossPos))
		{
			if (FVector::Distance(BottomCrossPoint, TempBottomPos) >
				FVector::Distance(TempCrossPos, TempBottomPos))
			{
				BottomCrossPoint = TempCrossPos;				
			}
		}		
	}
	if (!bAutoAdapt || bAlwaysShowDashLine)
	{
		if ((RelatedRoom.Pin()->IsPointInRoom(GetLeftPos())))
		{
			LeftRulerLine->Update(GetLeftPos(), LeftCrossPoint);
		}
		else
		{
			LeftRulerLine->ShowInputBox(false);
		}
		if (RelatedRoom.Pin()->IsPointInRoom(GetRightPos()))
		{
			RightRulerLine->Update(GetRightPos(), RightCrossPoint);			
		}
		else
		{
			RightRulerLine->ShowInputBox(false);
		}		
	}
	if (RelatedRoom.Pin()->IsPointInRoom(GetTopPos()))
	{
		TopRulerLine->Update(GetTopPos(), TopCrossPoint);		
	}	
	else
	{
		TopRulerLine->ShowInputBox(false);
	}
	if (RelatedRoom.Pin()->IsPointInRoom(GetBottomPos()))
	{
		BottomRulerLine->Update(GetBottomPos(), BottomCrossPoint);		
	}	
	else
	{
		BottomRulerLine->ShowInputBox(false);
	}
	
}

void FArmySimpleComponent::DrawDashLine(FPrimitiveDrawInterface * PDI, const FSceneView * View)
{
	if (!RelatedRoom.IsValid() || !bIsCopy)
	{
		return;
	}
	if (!bAutoAdapt || bAlwaysShowDashLine)
	{
		LeftRulerLine->Draw(PDI, View);
		RightRulerLine->Draw(PDI, View);		
	}	
	TopRulerLine->Draw(PDI, View);
	BottomRulerLine->Draw(PDI, View);
}

void FArmySimpleComponent::OnTopInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		float CachedLength = TopRulerLine->GetInputBoxCachedLength();		
		float TempInValue = FCString::Atof(*InText.ToString()) / 10.f;		
		
		float TempLength = CachedLength - TempInValue;
		if ((CachedLength < TempInValue && !FMath::IsNearlyEqual(CachedLength, TempInValue, 0.01f)) && FMath::Abs(TempLength) > BottomRulerLine->GetInputBoxCachedLength())
		{
			OnBottomInputBoxCommitted(FText::AsNumber(0.01), ETextCommit::OnEnter);
			TopRulerLine->GetWidget()->Revert();
			return;
		}
		if (!FMath::IsNearlyZero(TempLength, 0.001f))
		{
			//SCOPE_TRANSACTION(TEXT("通过标尺修改构件的位置"));
			FVector DashLineDirection = (GetTopPos() - GetPos()).GetSafeNormal();
			SetPos(GetPos() + (TempLength * DashLineDirection));
			AutoAdapt();

			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

void FArmySimpleComponent::OnBottomInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		float CachedLength = BottomRulerLine->GetInputBoxCachedLength();
		float TempInValue = FCString::Atof(*InText.ToString()) / 10.f;
		
		float TempLength = CachedLength - TempInValue;
		if ((CachedLength < TempInValue && !FMath::IsNearlyEqual(CachedLength, TempInValue, 0.01f)) && FMath::Abs(TempLength) > TopRulerLine->GetInputBoxCachedLength())
		{
			OnTopInputBoxCommitted(FText::AsNumber(0.01), ETextCommit::OnEnter);
			BottomRulerLine->GetWidget()->Revert();
			return;
		}
		if (!FMath::IsNearlyZero(TempLength, 0.001f))
		{
			//SCOPE_TRANSACTION(TEXT("通过标尺修改构件的位置"));
			FVector DashLineDirection = (GetBottomPos() - GetPos()).GetSafeNormal();
			SetPos(GetPos() + (TempLength * DashLineDirection));
			AutoAdapt();

			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

void FArmySimpleComponent::OnLeftInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		float CachedLength = LeftRulerLine->GetInputBoxCachedLength();
		float TempInValue = FCString::Atof(*InText.ToString()) / 10.f;
		
		float TempLength = CachedLength - TempInValue;
		if ((CachedLength < TempInValue && !FMath::IsNearlyEqual(CachedLength, TempInValue, 0.01f)) && FMath::Abs(TempLength) > RightRulerLine->GetInputBoxCachedLength())
		{
			OnRightInputBoxCommitted(FText::AsNumber(0.01), ETextCommit::OnEnter);
			LeftRulerLine->GetWidget()->Revert();
			return;
		}
		if (!FMath::IsNearlyZero(TempLength, 0.001f))
		{
			//SCOPE_TRANSACTION(TEXT("通过标尺修改构件的位置"));
			FVector DashLineDirection = (GetLeftPos() - GetPos()).GetSafeNormal();
			SetPos(GetPos() + (TempLength * DashLineDirection));
			AutoAdapt();

			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

void FArmySimpleComponent::OnRightInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		float CachedLength = RightRulerLine->GetInputBoxCachedLength();
		float TempInValue = FCString::Atof(*InText.ToString()) / 10.f;
		
		float TempLength = CachedLength - TempInValue;
		if ((CachedLength < TempInValue && !FMath::IsNearlyEqual(CachedLength, TempInValue, 0.01f)) && FMath::Abs(TempLength) > LeftRulerLine->GetInputBoxCachedLength())
		{
			OnLeftInputBoxCommitted(FText::AsNumber(0.01), ETextCommit::OnEnter);
			RightRulerLine->GetWidget()->Revert();
			return;
		}
		if (!FMath::IsNearlyZero(TempLength, 0.001f))
		{
			//SCOPE_TRANSACTION(TEXT("通过标尺修改构件的位置"));
			FVector DashLineDirection = (GetRightPos() - GetPos()).GetSafeNormal();
			SetPos(GetPos() + (TempLength * DashLineDirection));
			AutoAdapt();

			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

void FArmySimpleComponent::SetLeftPos(const FVector & InPos)
{
	if (LeftPoint->GetPos() != InPos)
	{
		LeftPoint->SetPos(InPos);		
	}
}

void FArmySimpleComponent::SetRightPos(const FVector & InPos)
{
	if (RightPoint->GetPos() != InPos)
	{
		RightPoint->SetPos(InPos);		
	}
}

void FArmySimpleComponent::SetTopPos(const FVector & InPos)
{
	if (TopPoint->GetPos() != InPos)
	{
		TopPoint->SetPos(InPos);		
	}
}

void FArmySimpleComponent::SetBottomPos(const FVector & InPos)
{
	if (BottomPoint->GetPos() != InPos)
	{
		BottomPoint->SetPos(InPos);		
	}
}

void FArmySimpleComponent::SetLeftTopPos(const FVector & InPos)
{
	if (LeftTopPoint->GetPos() != InPos)
	{
		LeftTopPoint->SetPos(InPos);		
	}
}

void FArmySimpleComponent::SetLeftBottomPos(const FVector & InPos)
{
	if (LeftBottomPoint->GetPos() != InPos)
	{
		LeftBottomPoint->SetPos(InPos);		
	}
}

void FArmySimpleComponent::SetRightTopPos(const FVector & InPos)
{
	if (RightTopPoint->GetPos() != InPos)
	{
		RightTopPoint->SetPos(InPos);		
	}
}

void FArmySimpleComponent::SetRightBottomPos(const FVector & InPos)
{
	if (RightBottomPoint->GetPos() != InPos)
	{
		RightBottomPoint->SetPos(InPos);		
	}
}
