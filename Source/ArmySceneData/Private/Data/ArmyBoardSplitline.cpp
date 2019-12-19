#include "ArmyBoardSplitline.h"
#include "CoreMinimal.h"
#include "ArmyStyle.h"
#include "ArmyMath.h"
#include "ArmyEditorViewportClient.h"

FArmyBoardSplitline::FArmyBoardSplitline()
{
    SetName(TEXT("标注") + GetUniqueID().ToString());
	ObjectType = OT_BoardSplitline;

	LeftStaticPoint = MakeShareable(new FArmyPoint);
	RightStaticPoint = MakeShareable(new FArmyPoint);

	MainLine = MakeShareable(new FArmyPolyline);
	MainLine->SetLineType(FArmyPolyline::LineStrip);
	MainLine->SetBaseColor(FLinearColor(FColor(0xFF999999)));
}


FArmyBoardSplitline::~FArmyBoardSplitline()
{

}
void FArmyBoardSplitline::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		//设置深度值
		ESceneDepthPriorityGroup Depth = ::SDPG_MAX;
		MainLine->Draw(PDI, View);
		MainLine->DepthPriority = Depth;
	}
}
void FArmyBoardSplitline::SetState(EObjectState InState)
{
	State = InState;
	FLinearColor LinearColor = FLinearColor::White;

	switch (InState)
	{
	case OS_Normal:
	case OS_Disable:
		LinearColor = FLinearColor(FColor(0xFF999999));
		break;

	case OS_Hovered:
	case OS_Selected:
		LinearColor = FLinearColor(FColor(0xFFFF9800));
		break;

	default:
		break;
	}
	if (MainLine.IsValid())
	{
		MainLine->SetBaseColor(LinearColor);
	}
}
void FArmyBoardSplitline::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	JsonWriter->WriteValue("Type", TEXT("OT_BoardSplitline"));
	JsonWriter->WriteValue("LeftStaticPoint", LeftStaticPoint->Pos.ToString());
	JsonWriter->WriteValue("RightStaticPoint", RightStaticPoint->Pos.ToString());
	JsonWriter->WriteValue("BaseColor",BaseColor.ToString());

	SERIALIZEREGISTERCLASS(JsonWriter, FArmyBoardSplitline)
}

void FArmyBoardSplitline::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);

	if (InJsonData->GetStringField("Type") == TEXT("OT_BoardSplitline"))
	{
		ObjectType = OT_None;
	}

	LeftStaticPoint->Pos.InitFromString(InJsonData->GetStringField("LeftStaticPoint"));
	RightStaticPoint->Pos.InitFromString(InJsonData->GetStringField("RightStaticPoint"));
	BaseColor.InitFromString(InJsonData->GetStringField("BaseColor"));

	Update();
}
void FArmyBoardSplitline::Update()
{
	//MainLine
	FVector LDir = (RightStaticPoint->Pos - LeftStaticPoint->Pos).GetSafeNormal();
	FVector VDir(-LDir.Y, LDir.X, 0);
	FVector CenterPos = (LeftStaticPoint->Pos + RightStaticPoint->Pos) / 2;

	TArray<FVector>& PolyLineVertices = MainLine->GetAllVertices();
	PolyLineVertices.Empty();
	PolyLineVertices.Add(LeftStaticPoint->Pos);
	PolyLineVertices.Add(CenterPos - LDir * 2.5);
	PolyLineVertices.Add(CenterPos + VDir * 5);
	PolyLineVertices.Add(CenterPos - VDir * 5);
	PolyLineVertices.Add(CenterPos + LDir * 2.5);
	PolyLineVertices.Add(RightStaticPoint->Pos);
}
bool FArmyBoardSplitline::IsSelected(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		FVector2D MPos;
		InViewportClient->WorldToPixel(Pos, MPos);

		FVector CurrentPos(MPos, 0);

		const TArray<FVector>& PolyLineVertices = MainLine->GetAllVertices();

		for (int32 i = 0;i < PolyLineVertices.Num() - 1;++i)
		{
			FVector2D PrePos,NextPos;
			InViewportClient->WorldToPixel(PolyLineVertices[i], PrePos);
			InViewportClient->WorldToPixel(PolyLineVertices[i + 1], NextPos);

			if (FMath::PointDistToSegment(CurrentPos, FVector(PrePos, 0), FVector(NextPos, 0)) < 10)
			{
				return true;
			}
		}
	}
	return  false;
}

bool FArmyBoardSplitline::Hover(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		return IsSelected(Pos,InViewportClient);
	}
	return  false;
}

void FArmyBoardSplitline::GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{
	
}
const void FArmyBoardSplitline::GetAlonePoints(TArray< TSharedPtr<FArmyPoint> >& OutPoints)
{
	OutPoints.Add(LeftStaticPoint);
	OutPoints.Add(RightStaticPoint);
}
void FArmyBoardSplitline::SetBaseColor(const FLinearColor& InColor)
{
	BaseColor = InColor;
	MainLine->SetBaseColor(BaseColor);
}
const FLinearColor& FArmyBoardSplitline::GetBaseColor() const
{
	return BaseColor;
}
void FArmyBoardSplitline::SetBaseStart(FVector InStartPoint)
{
	LeftStaticPoint->Pos = InStartPoint;
	Update();
}
void FArmyBoardSplitline::SetBaseEnd(FVector InEndPoint)
{
	RightStaticPoint->Pos = InEndPoint;
	Update();
}
void FArmyBoardSplitline::GetCadLineArray(TArray<TPair<FVector, FVector>>& OutLineList) const
{
	const TArray<FVector>& PolyLineVertices = MainLine->GetAllVertices();

	for (int32 i = 0;i < PolyLineVertices.Num() - 1;++i)
	{
		OutLineList.Emplace(TPair<FVector, FVector>(PolyLineVertices[i], PolyLineVertices[i + 1]));
	}
}
