#include "ArmyAirFlue.h"
#include "ArmyEditPoint.h"
#include "ArmyRoom.h"
#include "ArmySceneData.h"
#include "ArmyStyle.h"
#include "ArmyMath.h"
#include "ArmyRulerLine.h"
#include "ArmyGameInstance.h"

FArmyAirFlue::FArmyAirFlue()
	: FArmySimpleComponent()
{
	ObjectType = OT_AirFlue;
	SetPropertyFlag(FLAG_COLLISION, true);
	SetName(TEXT("风道"));
	Length = 40;
	Width = 40;
	Height = FArmySceneData::WallHeight;

	bAutoAdapt = false;	
	bAlwaysShowDashLine = true;

	RightCenterToRightTopLine = MakeShareable(new FArmyLine());
	LeftBottomToRightCenterLine = MakeShareable(new FArmyLine());

	OtherModeRightCenterToRightTopLine = MakeShareable(new FArmyLine());
	OtherModeLeftBottomToRightCenterLine = MakeShareable(new FArmyLine());

	LeftPoint->SetState(FArmyPrimitive::OPS_Normal);
	RightPoint->SetState(FArmyPrimitive::OPS_Normal);

	MI_PolygonMat->SetVectorParameterValue("BaseColor", FLinearColor(FColor(0xFFE6E6E6)));
	OtherModePolygon->MaterialRenderProxy = MI_PolygonMat->GetRenderProxy(false);

	OtherModeRightCenterToRightTopLine->bIsDashLine = true;
	OtherModeLeftBottomToRightCenterLine->bIsDashLine = true;
	OtherModeTopLeftLine->bIsDashLine = true;
	OtherModeBottomLeftLine->bIsDashLine = true;
	OtherModeTopRightLine->bIsDashLine = true;
	OtherModeBottomRightLine->bIsDashLine = true;
	OtherModeLeftTopLine->bIsDashLine = true;
	OtherModeLeftBottomLine->bIsDashLine = true;
	OtherModeRightTopLine->bIsDashLine = true;
	OtherModeRightBottomLine->bIsDashLine = true;

	OtherModeRightCenterToRightTopLine->DashSize = 5.f;
	OtherModeLeftBottomToRightCenterLine->DashSize = 5.f;
	OtherModeBottomLeftLine->DashSize = 5.f;
	OtherModeTopLeftLine->DashSize = 5.f;
	OtherModeBottomRightLine->DashSize = 5.f;
	OtherModeTopRightLine->DashSize = 5.f;
	OtherModeLeftTopLine->DashSize = 5.f;
	OtherModeLeftBottomLine->DashSize = 5.f;
	OtherModeRightTopLine->DashSize = 5.f;
	OtherModeRightBottomLine->DashSize = 5.f;

	OtherModeTopLeftLine->SetLineWidth(3.f);
	OtherModeBottomLeftLine->SetLineWidth(3.f);
	OtherModeTopRightLine->SetLineWidth(3.f);
	OtherModeBottomRightLine->SetLineWidth(3.f);
	OtherModeLeftTopLine->SetLineWidth(3.f);
	OtherModeLeftBottomLine->SetLineWidth(3.f);
	OtherModeRightTopLine->SetLineWidth(3.f);
	OtherModeRightBottomLine->SetLineWidth(3.f);
}

FArmyAirFlue::FArmyAirFlue(FArmyAirFlue * Copy)
	: FArmySimpleComponent(Copy)
{
	bAutoAdaptDir = Copy->bAutoAdaptDir;
	RightCenterToRightTopLine = MakeShareable(new FArmyLine(Copy->RightCenterToRightTopLine.Get()));
	LeftBottomToRightCenterLine = MakeShareable(new FArmyLine(Copy->LeftBottomToRightCenterLine.Get()));

	OtherModeRightCenterToRightTopLine = MakeShareable(new FArmyLine(Copy->OtherModeRightCenterToRightTopLine.Get()));
	OtherModeLeftBottomToRightCenterLine = MakeShareable(new FArmyLine(Copy->OtherModeLeftBottomToRightCenterLine.Get()));

	OtherModeRightCenterToRightTopLine->bIsDashLine = true;
	OtherModeLeftBottomToRightCenterLine->bIsDashLine = true;
	OtherModeTopLeftLine->bIsDashLine = true;
	OtherModeBottomLeftLine->bIsDashLine = true;
	OtherModeTopRightLine->bIsDashLine = true;
	OtherModeBottomRightLine->bIsDashLine = true;
	OtherModeLeftTopLine->bIsDashLine = true;
	OtherModeLeftBottomLine->bIsDashLine = true;
	OtherModeRightTopLine->bIsDashLine = true;
	OtherModeRightBottomLine->bIsDashLine = true;

	OtherModeRightCenterToRightTopLine->DashSize = 5.f;
	OtherModeLeftBottomToRightCenterLine->DashSize = 5.f;
	OtherModeBottomLeftLine->DashSize = 5.f;
	OtherModeTopLeftLine->DashSize = 5.f;
	OtherModeBottomRightLine->DashSize = 5.f;
	OtherModeTopRightLine->DashSize = 5.f;
	OtherModeLeftTopLine->DashSize = 5.f;
	OtherModeLeftBottomLine->DashSize = 5.f;
	OtherModeRightTopLine->DashSize = 5.f;
	OtherModeRightBottomLine->DashSize = 5.f;
}

FArmyAirFlue::~FArmyAirFlue()
{
}

void FArmyAirFlue::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{	
	FArmySimpleComponent::SerializeToJson(JsonWriter);
	JsonWriter->WriteValue("bAutoAdaptDir", GetAutoAdaptDir());

	SERIALIZEREGISTERCLASS(JsonWriter, FArmyAirFlue)
}

void FArmyAirFlue::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmySimpleComponent::Deserialization(InJsonData);
	SetAutoAdaptDir(InJsonData->GetBoolField("bAutoAdaptDir"));
}

void FArmyAirFlue::SetState(EObjectState InState)
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
		RightCenterToRightTopLine->SetBaseColor(FLinearColor::White);
		LeftBottomToRightCenterLine->SetBaseColor(FLinearColor::White);
		DeselectPoints();
		LeftRulerLine->ShowInputBox(false);
		RightRulerLine->ShowInputBox(false);
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
		RightCenterToRightTopLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		LeftBottomToRightCenterLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		UpdateDashLine();
		break;

	case OS_Disable:
		LeftTopLine->SetBaseColor(FLinearColor::Gray);
		LeftBottomLine->SetBaseColor(FLinearColor::Gray);
		RightTopLine->SetBaseColor(FLinearColor::Gray);
		RightBottomLine->SetBaseColor(FLinearColor::Gray);
		TopLeftLine->SetBaseColor(FLinearColor::Gray);
		BottomLeftLine->SetBaseColor(FLinearColor::Gray);
		TopRightLine->SetBaseColor(FLinearColor::Gray);
		BottomRightLine->SetBaseColor(FLinearColor::Gray);
		RightCenterToRightTopLine->SetBaseColor(FLinearColor::Gray);
		LeftBottomToRightCenterLine->SetBaseColor(FLinearColor::Gray);
		break;

	default:
		break;
	}
}

void FArmyAirFlue::Draw(FPrimitiveDrawInterface * PDI, const FSceneView * View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (FArmyObject::GetDrawModel(MODE_CEILING) || FArmyObject::GetDrawModel(MODE_TOPVIEW))
		{
			OtherModePolygon->Draw(PDI, View);
			OtherModeLeftTopLine->SetBaseColor(FLinearColor(FColor(0xFF333333)));
			OtherModeLeftBottomLine->SetBaseColor(FLinearColor(FColor(0xFF333333)));
			OtherModeRightTopLine->SetBaseColor(FLinearColor(FColor(0xFF333333)));
			OtherModeRightBottomLine->SetBaseColor(FLinearColor(FColor(0xFF333333)));
			OtherModeTopLeftLine->SetBaseColor(FLinearColor(FColor(0xFF333333)));
			OtherModeBottomLeftLine->SetBaseColor(FLinearColor(FColor(0xFF333333)));
			OtherModeTopRightLine->SetBaseColor(FLinearColor(FColor(0xFF333333)));
			OtherModeBottomRightLine->SetBaseColor(FLinearColor(FColor(0xFF333333)));
			OtherModeRightCenterToRightTopLine->SetBaseColor(FLinearColor(FColor(0xFF333333)));
			OtherModeLeftBottomToRightCenterLine->SetBaseColor(FLinearColor(FColor(0xFF333333)));
			OtherModeTopLeftLine->Draw(PDI, View);
			OtherModeBottomLeftLine->Draw(PDI, View);
			OtherModeTopRightLine->Draw(PDI, View);
			OtherModeBottomRightLine->Draw(PDI, View);
			OtherModeLeftTopLine->Draw(PDI, View);
			OtherModeLeftBottomLine->Draw(PDI, View);
			OtherModeRightTopLine->Draw(PDI, View);
			OtherModeRightBottomLine->Draw(PDI, View);
			OtherModeRightCenterToRightTopLine->Draw(PDI, View);
			OtherModeLeftBottomToRightCenterLine->Draw(PDI, View);
		}
		else
		{
			FArmySimpleComponent::Draw(PDI, View);
			RightCenterToRightTopLine->Draw(PDI, View);
			LeftBottomToRightCenterLine->Draw(PDI, View);
		}
	}
}

bool FArmyAirFlue::IsSelected(const FVector & Pos, UArmyEditorViewportClient * InViewportClient)
{
	return FArmySimpleComponent::IsSelected(Pos, InViewportClient);
}

TSharedPtr<FArmyEditPoint> FArmyAirFlue::SelectPoint(const FVector & Pos, UArmyEditorViewportClient * InViewportClient)
{
	return FArmySimpleComponent::SelectPoint(Pos, InViewportClient);
}

TSharedPtr<FArmyEditPoint> FArmyAirFlue::HoverPoint(const FVector & Pos, UArmyEditorViewportClient * InViewportClient)
{
	return FArmySimpleComponent::HoverPoint(Pos, InViewportClient);
}

const FBox FArmyAirFlue::GetBounds()
{
	TArray<FVector> OutVertexes;
	GetVertexes(OutVertexes);

	return FBox(OutVertexes);
}

void FArmyAirFlue::GetCadLineArray(TArray<TPair<FVector, FVector>>& OutLineList) const
{
	FArmySimpleComponent::GetCadLineArray(OutLineList);
	OutLineList.Append({
		TPair<FVector,FVector>(RightCenterToRightTopLine->GetStart(), RightCenterToRightTopLine->GetEnd()),
		TPair<FVector,FVector>(LeftBottomToRightCenterLine->GetStart(), LeftBottomToRightCenterLine->GetEnd()),
	});
}

void FArmyAirFlue::GetVertexes(TArray<FVector>& OutVertexes)
{
	FArmySimpleComponent::GetVertexes(OutVertexes);
}

void FArmyAirFlue::Update()
{
	FArmySimpleComponent::Update();

	FVector TempTurningPos = GetPos() + Direction * 0.3f * Length + HorizontalDirection * 0.3f * Width;
	LeftBottomToRightCenterLine->SetStart(GetLeftBottomPos());
	LeftBottomToRightCenterLine->SetEnd(TempTurningPos);

	RightCenterToRightTopLine->SetStart(TempTurningPos);
	RightCenterToRightTopLine->SetEnd(GetRightTopPos());

	int32 TopViewZ0 = FArmySceneData::WallHeight + 20.f;
	TArray<FVector> Vertexs;
	GetVertexes(Vertexs);
	OtherModePolygon->RemoveAllVertexes();
	OtherModePolygon->SetVertices(Vertexs);
	OtherModePolygon->SetPolygonOffset(TopViewZ0);

	FVector TempPos = RightCenterToRightTopLine->GetStart();
	TempPos.Z = TopViewZ0;
	OtherModeRightCenterToRightTopLine->SetStart(TempPos);

	TempPos = RightCenterToRightTopLine->GetEnd();
	TempPos.Z = TopViewZ0;
	OtherModeRightCenterToRightTopLine->SetEnd(TempPos);

	TempPos = LeftBottomToRightCenterLine->GetStart();
	TempPos.Z = TopViewZ0;
	OtherModeLeftBottomToRightCenterLine->SetStart(TempPos);

	TempPos = LeftBottomToRightCenterLine->GetEnd();
	TempPos.Z = TopViewZ0;
	OtherModeLeftBottomToRightCenterLine->SetEnd(TempPos);
}

void FArmyAirFlue::UpdateTopPoint(FVector Pos)
{
	FArmySimpleComponent::UpdateTopPoint(Pos);
}

void FArmyAirFlue::UpdateBottomPoint(FVector Pos)
{
	FArmySimpleComponent::UpdateBottomPoint(Pos);
}

void FArmyAirFlue::UpdateLeftPoint(FVector Pos)
{
	FArmySimpleComponent::UpdateLeftPoint(Pos);
}

void FArmyAirFlue::UpdateRightPoint(FVector Pos)
{
	FArmySimpleComponent::UpdateRightPoint(Pos);
}

void FArmyAirFlue::OnDelete()
{
	FArmySimpleComponent::OnDelete();
}

void FArmyAirFlue::AutoAdapt()
{
	if (OperationPointType == ESCS_CENTER)
	{
		bAutoAdapt = false;
		Update();
		return;
	}
	if (!GetRelatedRoom().IsValid() || !bAutoAdapt)
	{
		Update();
		return;
	}

	TArray<TSharedPtr<FArmyLine>> Lines;
	GetRelatedRoom().Pin()->GetLines(Lines);
	//找到横向每个方向两个到关联房间的所有线段最近的线线段的交点

	FVector TempLineDirection;
	FVector TempDestPos;
	float MaxDist = Length * 3.f / 2.f;
	float MaxAdaptDist = 50.f;

	//默认一个最大距离的交叉点
	FVector LeftTopCrossPoint = GetTopPos() + (-GetDirection()).GetSafeNormal() * MaxDist;
	FVector LeftBottomCrossPoint = GetBottomPos() + (-GetDirection()).GetSafeNormal() * MaxDist;
	FVector RightTopCrossPoint = GetTopPos() + (GetDirection()).GetSafeNormal() * MaxDist;
	FVector RightBottomCrossPoint = GetBottomPos() + (GetDirection()).GetSafeNormal() * MaxDist;

	TSharedPtr<FArmyLine> TempLeftLeftLine = nullptr;
	TSharedPtr<FArmyLine> TempLeftRightLine = nullptr;
	TSharedPtr<FArmyLine> TempRightLeftLine = nullptr;
	TSharedPtr<FArmyLine> TempRightRightLine = nullptr;

	for (auto It : Lines)
	{
		FVector TempCrossPos = FVector::ZeroVector;
		FVector TempLineStart = It->GetStart();
		FVector TempLineEnd = It->GetEnd();
		FVector TempTopPos = GetTopPos();
		FVector TempBottomPos = GetBottomPos();
		TempLineStart.Z = 0.f;
		TempLineEnd.Z = 0.f;
		TempTopPos.Z = 0.f;
		TempBottomPos.Z = 0.f;

		//left top 
		TempLineDirection = (-GetDirection()).GetSafeNormal();
		TempDestPos = GetTopPos() + TempLineDirection * MaxDist;
		TempDestPos.Z = 0.f;

		if (FArmyMath::SegmentIntersection2D(TempLineStart, TempLineEnd, TempTopPos, TempDestPos, TempCrossPos))
		{
			//找到最近的交点
			if (FVector::Distance(LeftTopCrossPoint, TempTopPos) >
				FVector::Distance(TempCrossPos, TempTopPos))
			{
				LeftTopCrossPoint = TempCrossPos;
				if (FVector::Distance(LeftTopCrossPoint, GetLeftTopPos()) <= MaxAdaptDist)
				{
					TempLeftLeftLine = It;
				}
			}
		}

		//left bottom 
		TempLineDirection = (-GetDirection()).GetSafeNormal();
		TempDestPos = GetBottomPos() + TempLineDirection * MaxDist;
		TempDestPos.Z = 0.f;
		if (FArmyMath::SegmentIntersection2D(TempLineStart, TempLineEnd, TempBottomPos, TempDestPos, TempCrossPos))
		{
			if (FVector::Distance(LeftBottomCrossPoint, TempBottomPos) >
				FVector::Distance(TempCrossPos, TempBottomPos))
			{
				LeftBottomCrossPoint = TempCrossPos;
				if (FVector::Distance(LeftBottomCrossPoint, GetLeftBottomPos()) <= MaxAdaptDist)
				{
					TempLeftRightLine = It;
				}
			}
		}

		//right top 
		TempLineDirection = (GetDirection()).GetSafeNormal();
		TempDestPos = GetTopPos() + TempLineDirection * MaxDist;
		TempDestPos.Z = 0.f;
		if (FArmyMath::SegmentIntersection2D(TempLineStart, TempLineEnd, TempTopPos, TempDestPos, TempCrossPos))
		{
			//找到最近的交点
			if (FVector::Distance(RightTopCrossPoint, TempTopPos) >
				FVector::Distance(TempCrossPos, TempTopPos))
			{
				RightTopCrossPoint = TempCrossPos;
				if (FVector::Distance(RightTopCrossPoint, GetRightTopPos()) <= MaxAdaptDist)
				{
					TempRightLeftLine = It;
				}
			}
		}

		//right bottom 
		TempLineDirection = (GetDirection()).GetSafeNormal();
		TempDestPos = GetBottomPos() + TempLineDirection * MaxDist;
		TempDestPos.Z = 0.f;
		if (FArmyMath::SegmentIntersection2D(TempLineStart, TempLineEnd, TempBottomPos, TempDestPos, TempCrossPos))
		{
			if (FVector::Distance(RightBottomCrossPoint, TempBottomPos) >
				FVector::Distance(TempCrossPos, TempBottomPos))
			{
				RightBottomCrossPoint = TempCrossPos;
				if (FVector::Distance(RightBottomCrossPoint, GetRightBottomPos()) <= MaxAdaptDist)
				{
					TempRightRightLine = It;
				}
			}
		}
	}

	if (TempLeftLeftLine.IsValid() && TempLeftRightLine.IsValid())
	{
		if (TempLeftLeftLine != TempLeftRightLine)
		{
			if (TempLeftLeftLine->GetStart() == TempLeftRightLine->GetStart() || TempLeftLeftLine->GetStart() == TempLeftRightLine->GetEnd())
			{
				SetLeftPos(TempLeftLeftLine->GetStart());
			}
			else
			{
				SetLeftPos(TempLeftLeftLine->GetEnd());
			}
		}
		else
		{
			SetLeftPos((LeftTopCrossPoint + LeftBottomCrossPoint) / 2.f);
		}

		SetLeftTopPos(LeftTopCrossPoint);
		SetLeftBottomPos(LeftBottomCrossPoint);
		//Update();
	}
	else
	{
		LeftTopPoint->SetPos(GetPos() - Direction * Length / 2.f - HorizontalDirection * Width / 2.f);
		LeftBottomPoint->SetPos(GetPos() - Direction * Length / 2.f + HorizontalDirection * Width / 2.f);
		LeftPoint->SetPos(GetPos() - Direction * Length / 2.f);
	}

	if (TempRightLeftLine.IsValid() && TempRightRightLine.IsValid())
	{
		if (TempRightLeftLine != TempRightRightLine)
		{
			if (TempRightLeftLine->GetStart() == TempRightRightLine->GetStart() || TempRightLeftLine->GetStart() == TempRightRightLine->GetEnd())
			{
				SetRightPos(TempRightLeftLine->GetStart());
			}
			else
			{
				SetRightPos(TempRightLeftLine->GetEnd());
			}
		}
		else
		{
			SetRightPos((RightTopCrossPoint + RightBottomCrossPoint) / 2.f);
		}

		SetRightTopPos(RightTopCrossPoint);
		SetRightBottomPos(RightBottomCrossPoint);
	}
	else
	{
		RightTopPoint->SetPos(GetPos() + Direction * Length / 2.f - HorizontalDirection * Width / 2.f);
		RightBottomPoint->SetPos(GetPos() + Direction * Length / 2.f + HorizontalDirection * Width / 2.f);
		RightPoint->SetPos(GetPos() + Direction * Length / 2.f);
	}
	Update();
}

void FArmyAirFlue::AutoAttach(float Invalue)
{
	FArmySimpleComponent::AutoAttach(0.01f);
}

bool FArmyAirFlue::CheckAllPointsInRoom(FVector Pos)
{
	if (RelatedRoom.IsValid())
	{
		if (RelatedRoom.Pin()->IsPointInRoom(Pos - HorizontalDirection * Width / 2.0f) && RelatedRoom.Pin()->IsPointInRoom(Pos + HorizontalDirection * Width / 2.0f) &&
			RelatedRoom.Pin()->IsPointInRoom(Pos - Direction * Length / 2.0f) && RelatedRoom.Pin()->IsPointInRoom(Pos + Direction * Length / 2.0f))
		{
			return true;
		}
	}
	return false;
}

void FArmyAirFlue::SetAutoAdapt(bool bValue)
{
	{ bAutoAdapt = bValue; }
	if (bAutoAdapt)
	{
		AutoAdapt();
		/*LeftRulerLine->ShowInputBox(false);
		RightRulerLine->ShowInputBox(false);*/
	}
	else
	{
		SetLength(FMath::Min(TopLeftLine->Size() + TopRightLine->Size(), BottomLeftLine->Size() + BottomRightLine->Size()));
		LeftPoint->SetState(FArmyPrimitive::OPS_Normal);
		RightPoint->SetState(FArmyPrimitive::OPS_Normal);
		TopPoint->SetState(FArmyPrimitive::OPS_Normal);
		BottomPoint->SetState(FArmyPrimitive::OPS_Normal);
	}
	Update();
}

void FArmyAirFlue::OnTopInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	FArmySimpleComponent::OnTopInputBoxCommitted(InText, InTextAction);
}

void FArmyAirFlue::OnBottomInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	FArmySimpleComponent::OnBottomInputBoxCommitted(InText, InTextAction);
}

void FArmyAirFlue::OnLeftInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	FArmySimpleComponent::OnLeftInputBoxCommitted(InText, InTextAction);
}

void FArmyAirFlue::OnRightInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	FArmySimpleComponent::OnRightInputBoxCommitted(InText, InTextAction);
}
