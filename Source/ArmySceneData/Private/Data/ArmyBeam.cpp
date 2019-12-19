#include "ArmyBeam.h"
#include "ArmyEditPoint.h"
#include "ArmyRoom.h"
#include "ArmySceneData.h"
#include "ArmyStyle.h"
#include "ArmyMath.h"
#include "ArmyRulerLine.h"
#include "ArmyGameInstance.h"


FArmyBeam::FArmyBeam()
	: FArmySimpleComponent()
{
	ObjectType = OT_Beam;
	SetPropertyFlag(FLAG_COLLISION, true);
	SetName(TEXT("梁"));

	LeftTopLine->SetBaseColor(FLinearColor(FColor(0xFF00FFFF)));
	LeftBottomLine->SetBaseColor(FLinearColor(FColor(0xFF00FFFF)));
	RightTopLine->SetBaseColor(FLinearColor(FColor(0xFF00FFFF)));
	RightBottomLine->SetBaseColor(FLinearColor(FColor(0xFF00FFFF)));
	TopLeftLine->SetBaseColor(FLinearColor(FColor(0xFF00FFFF)));
	BottomLeftLine->SetBaseColor(FLinearColor(FColor(0xFF00FFFF)));
	TopRightLine->SetBaseColor(FLinearColor(FColor(0xFF00FFFF)));
	BottomRightLine->SetBaseColor(FLinearColor(FColor(0xFF00FFFF)));

	OtherModeTopLeftLine->bIsDashLine = true;
	OtherModeBottomLeftLine->bIsDashLine = true;
	OtherModeTopRightLine->bIsDashLine = true;
	OtherModeBottomRightLine->bIsDashLine = true;
	OtherModeLeftTopLine->bIsDashLine = true;
	OtherModeLeftBottomLine->bIsDashLine = true;
	OtherModeRightTopLine->bIsDashLine = true;
	OtherModeRightBottomLine->bIsDashLine = true;

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

FArmyBeam::FArmyBeam(FArmyBeam * Copy)
	: FArmySimpleComponent(Copy)
{
	OtherModeTopLeftLine->bIsDashLine = true;
	OtherModeBottomLeftLine->bIsDashLine = true;
	OtherModeTopRightLine->bIsDashLine = true;
	OtherModeBottomRightLine->bIsDashLine = true;
	OtherModeLeftTopLine->bIsDashLine = true;
	OtherModeLeftBottomLine->bIsDashLine = true;
	OtherModeRightTopLine->bIsDashLine = true;
	OtherModeRightBottomLine->bIsDashLine = true;

	OtherModeBottomLeftLine->DashSize = 5.f;
	OtherModeTopLeftLine->DashSize = 5.f;
	OtherModeBottomRightLine->DashSize = 5.f;
	OtherModeTopRightLine->DashSize = 5.f;
	OtherModeLeftTopLine->DashSize = 5.f;
	OtherModeLeftBottomLine->DashSize = 5.f;
	OtherModeRightTopLine->DashSize = 5.f;
	OtherModeRightBottomLine->DashSize = 5.f;
}

FArmyBeam::~FArmyBeam()
{
}

void FArmyBeam::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmySimpleComponent::SerializeToJson(JsonWriter);

	SERIALIZEREGISTERCLASS(JsonWriter, FArmyBeam)
}

void FArmyBeam::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	bDeserialization = true;
	FArmySimpleComponent::Deserialization(InJsonData);

	if (!bAutoAdapt && !GetPropertyFlag(FArmyObject::FLAG_MODIFY))
	{
		LeftPoint->SetState(FArmyPrimitive::OPS_Normal);
		RightPoint->SetState(FArmyPrimitive::OPS_Normal);
	}
}

void FArmyBeam::SetState(EObjectState InState)
{
	State = InState;

	switch (InState)
	{
	case OS_Normal:
		LeftTopLine->SetBaseColor(FLinearColor(FColor(0xFF00FFFF)));
		LeftBottomLine->SetBaseColor(FLinearColor(FColor(0xFF00FFFF)));
		RightTopLine->SetBaseColor(FLinearColor(FColor(0xFF00FFFF)));
		RightBottomLine->SetBaseColor(FLinearColor(FColor(0xFF00FFFF)));
		TopLeftLine->SetBaseColor(FLinearColor(FColor(0xFF00FFFF)));
		BottomLeftLine->SetBaseColor(FLinearColor(FColor(0xFF00FFFF)));
		TopRightLine->SetBaseColor(FLinearColor(FColor(0xFF00FFFF)));
		BottomRightLine->SetBaseColor(FLinearColor(FColor(0xFF00FFFF)));
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
		BottomLeftLine->SetBaseColor(FLinearColor::Gray);
		TopRightLine->SetBaseColor(FLinearColor::Gray);
		BottomRightLine->SetBaseColor(FLinearColor::Gray);
		break;

	default:
		break;
	}
}

void FArmyBeam::Draw(FPrimitiveDrawInterface * PDI, const FSceneView * View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (FArmyObject::GetDrawModel(MODE_CONSTRUCTION))
		{
			FColor ConstructionColor(0xFF666666);
			OtherModeLeftTopLine->SetBaseColor(FLinearColor(ConstructionColor));
			OtherModeLeftBottomLine->SetBaseColor(FLinearColor(ConstructionColor));
			OtherModeRightTopLine->SetBaseColor(FLinearColor(ConstructionColor));
			OtherModeRightBottomLine->SetBaseColor(FLinearColor(ConstructionColor));
			OtherModeTopLeftLine->SetBaseColor(FLinearColor(ConstructionColor));
			OtherModeBottomLeftLine->SetBaseColor(FLinearColor(ConstructionColor));
			OtherModeTopRightLine->SetBaseColor(FLinearColor(ConstructionColor));
			OtherModeBottomRightLine->SetBaseColor(FLinearColor(ConstructionColor));
			OtherModeTopLeftLine->Draw(PDI, View);
			OtherModeBottomLeftLine->Draw(PDI, View);
			OtherModeTopRightLine->Draw(PDI, View);
			OtherModeBottomRightLine->Draw(PDI, View);
			OtherModeLeftTopLine->Draw(PDI, View);
			OtherModeLeftBottomLine->Draw(PDI, View);
			OtherModeRightTopLine->Draw(PDI, View);
			OtherModeRightBottomLine->Draw(PDI, View);
		}
		else if (FArmyObject::GetDrawModel(MODE_CEILING))
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
			OtherModeTopLeftLine->Draw(PDI, View);
			OtherModeBottomLeftLine->Draw(PDI, View);
			OtherModeTopRightLine->Draw(PDI, View);
			OtherModeBottomRightLine->Draw(PDI, View);
			OtherModeLeftTopLine->Draw(PDI, View);
			OtherModeLeftBottomLine->Draw(PDI, View);
			OtherModeRightTopLine->Draw(PDI, View);
			OtherModeRightBottomLine->Draw(PDI, View);
		}	
		else if (FArmyObject::GetDrawModel(MODE_TOPVIEW))
		{
			return;
		}
		else if (FArmyObject::GetDrawModel(MODE_NORAMAL))
		{
			FArmySimpleComponent::Draw(PDI, View);
		}		
	}	
}

bool FArmyBeam::IsSelected(const FVector & Pos, UArmyEditorViewportClient * InViewportClient)
{
	return FArmySimpleComponent::IsSelected(Pos, InViewportClient);
}

TSharedPtr<FArmyEditPoint> FArmyBeam::SelectPoint(const FVector & Pos, UArmyEditorViewportClient * InViewportClient)
{
	return FArmySimpleComponent::SelectPoint(Pos, InViewportClient);
}

TSharedPtr<FArmyEditPoint> FArmyBeam::HoverPoint(const FVector & Pos, UArmyEditorViewportClient * InViewportClient)
{
	return FArmySimpleComponent::HoverPoint(Pos, InViewportClient);
}

const FBox FArmyBeam::GetBounds()
{
	return FBox();
}

void FArmyBeam::GetCadLineArray(TArray<TPair<FVector, FVector>>& OutLineList) const
{
	OutLineList.Append({
		TPair<FVector,FVector>(OtherModeTopLeftLine->GetStart(),OtherModeTopLeftLine->GetEnd()),
		TPair<FVector,FVector>(OtherModeBottomLeftLine->GetStart(),OtherModeBottomLeftLine->GetEnd()),
		TPair<FVector,FVector>(OtherModeTopRightLine->GetStart(),OtherModeTopRightLine->GetEnd()),
		TPair<FVector,FVector>(OtherModeBottomRightLine->GetStart(),OtherModeBottomRightLine->GetEnd()),

		TPair<FVector,FVector>(OtherModeLeftTopLine->GetStart(),OtherModeLeftTopLine->GetEnd()),
		TPair<FVector,FVector>(OtherModeLeftBottomLine->GetStart(),OtherModeLeftBottomLine->GetEnd()),
		TPair<FVector,FVector>(OtherModeRightTopLine->GetStart(),OtherModeRightTopLine->GetEnd()),
		TPair<FVector,FVector>(OtherModeRightBottomLine->GetStart(),OtherModeRightBottomLine->GetEnd()),
	});
}

void FArmyBeam::GetVertexes(TArray<FVector>& OutVertexes)
{
	FArmySimpleComponent::GetVertexes(OutVertexes);
}

void FArmyBeam::Update()
{
	FArmySimpleComponent::Update();

	int32 TopViewZ0 = FArmySceneData::WallHeight + 20.f;
	TArray<FVector> Vertexs;
	GetVertexes(Vertexs);	
	OtherModePolygon->RemoveAllVertexes();
	OtherModePolygon->SetVertices(Vertexs);
	OtherModePolygon->SetPolygonOffset(TopViewZ0);
}

void FArmyBeam::UpdateTopPoint(FVector Pos)
{
	FArmySimpleComponent::UpdateTopPoint(Pos);
}

void FArmyBeam::UpdateBottomPoint(FVector Pos)
{
	FArmySimpleComponent::UpdateBottomPoint(Pos);
}

void FArmyBeam::UpdateLeftPoint(FVector Pos)
{
	FArmySimpleComponent::UpdateLeftPoint(Pos);
}

void FArmyBeam::UpdateRightPoint(FVector Pos)
{
	FArmySimpleComponent::UpdateRightPoint(Pos);
}

void FArmyBeam::OnDelete()
{
	FArmySimpleComponent::OnDelete();
}

void FArmyBeam::AutoAdapt()
{
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
	float MaxDist = 10000;

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
				TempLeftLeftLine = It;
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
				TempLeftRightLine = It;
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
				TempRightLeftLine = It;
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
				TempRightRightLine = It;
			}
		}
	}

	if (TempLeftLeftLine.IsValid() && TempLeftRightLine.IsValid() && TempRightLeftLine.IsValid() && TempRightRightLine.IsValid())
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

		//如果出现线段交叉，进行处理
		if (FVector::Distance(GetLeftPos(), LeftTopCrossPoint) > 2.f * GetWidth() || FVector::Distance(GetLeftPos(), LeftBottomCrossPoint) > 2.f * GetWidth())
		{
			if (FVector::Distance(GetPos(), LeftTopCrossPoint) < FVector::Distance(GetPos(), LeftBottomCrossPoint))
			{
				LeftBottomCrossPoint = FArmyMath::GetProjectionPoint(LeftTopCrossPoint, GetBottomPos(), GetBottomPos() + (-GetDirection()).GetSafeNormal() * MaxDist);
			}
			else
			{
				LeftTopCrossPoint = FArmyMath::GetProjectionPoint(LeftBottomCrossPoint, GetTopPos(), GetTopPos() + (-GetDirection()).GetSafeNormal() * MaxDist);
			}
			SetLeftPos((LeftTopCrossPoint + LeftBottomCrossPoint) / 2.f);
		}
		if (FVector::Distance(GetRightPos(), RightTopCrossPoint) > 2.f * GetWidth() || FVector::Distance(GetRightPos(), RightBottomCrossPoint) > 2.f * GetWidth())
		{
			if (FVector::Distance(GetPos(), RightTopCrossPoint) < FVector::Distance(GetPos(), RightBottomCrossPoint))
			{
				RightBottomCrossPoint = FArmyMath::GetProjectionPoint(RightTopCrossPoint, GetBottomPos(), GetBottomPos() + (GetDirection()).GetSafeNormal() * MaxDist);
			}
			else
			{
				RightTopCrossPoint = FArmyMath::GetProjectionPoint(RightBottomCrossPoint, GetTopPos(), GetTopPos() + (GetDirection()).GetSafeNormal() * MaxDist);
			}
			SetRightPos((RightTopCrossPoint + RightBottomCrossPoint) / 2.f);
		}

		SetLeftTopPos(LeftTopCrossPoint);
		SetLeftBottomPos(LeftBottomCrossPoint);
		SetRightTopPos(RightTopCrossPoint);
		SetRightBottomPos(RightBottomCrossPoint);
		Update();
	}
}

void FArmyBeam::AutoAttach(float Invalue)
{
	FArmySimpleComponent::AutoAttach(0.2f);
}

bool FArmyBeam::CheckAllPointsInRoom(FVector Pos)
{
	if (RelatedRoom.IsValid())
	{
		if (bAutoAdapt)
		{
			if (RelatedRoom.Pin()->IsPointInRoom(Pos - HorizontalDirection * Width / 2.0f) && RelatedRoom.Pin()->IsPointInRoom(Pos + HorizontalDirection * Width / 2.0f))
			{
				return true;
			}
		}
		else
		{
			if (RelatedRoom.Pin()->IsPointInRoom(Pos - HorizontalDirection * Width / 2.0f) && RelatedRoom.Pin()->IsPointInRoom(Pos + HorizontalDirection * Width / 2.0f) &&
				RelatedRoom.Pin()->IsPointInRoom(Pos - Direction * Length / 2.0f) && RelatedRoom.Pin()->IsPointInRoom(Pos + Direction * Length / 2.0f))
			{
				return true;
			}
		}		
	}
	return false;
}

void FArmyBeam::SetAutoAdapt(bool bValue)
{
	{ bAutoAdapt = bValue; }
	if (bAutoAdapt)
	{
		LeftPoint->SetState(FArmyPrimitive::OPS_Disable);
		RightPoint->SetState(FArmyPrimitive::OPS_Disable);
		LeftRulerLine->ShowInputBox(false);
		RightRulerLine->ShowInputBox(false);
		AutoAdapt();
	}
	else
	{		
		if (!bDeserialization)
		{
			SetLength(FMath::Min(TopLeftLine->Size() + TopRightLine->Size(), BottomLeftLine->Size() + BottomRightLine->Size()) / 10 * 9);
		}		
		LeftPoint->SetState(FArmyPrimitive::OPS_Normal);
		RightPoint->SetState(FArmyPrimitive::OPS_Normal);
	}
	Update();
}

void FArmyBeam::OnTopInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	FArmySimpleComponent::OnTopInputBoxCommitted(InText, InTextAction);
}

void FArmyBeam::OnBottomInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	FArmySimpleComponent::OnBottomInputBoxCommitted(InText, InTextAction);
}

void FArmyBeam::OnLeftInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	FArmySimpleComponent::OnLeftInputBoxCommitted(InText, InTextAction);
}

void FArmyBeam::OnRightInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	FArmySimpleComponent::OnRightInputBoxCommitted(InText, InTextAction);
}
