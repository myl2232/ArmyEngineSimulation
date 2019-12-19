/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRSplitLine.cpp
* @Description 分割线
*
* @Author 常远
* @Date 2018年12月17日
* @Version 1.0
*/
#include "ArmySplitLine.h"
#include "ArmySceneData.h"
#include "Math/XRMath.h"
#include "ArmyEditorViewportClient.h"

FArmySplitLine::FArmySplitLine()
{
	Color = FLinearColor(FColor(0xFFFFFFFF));
	ObjectType = OT_SplitLine;
	SetPropertyFlag(FLAG_COLLISION, true);
    SetName(TEXT("分割线")/* + GetUniqueID().ToString()*/);
}

FArmySplitLine::FArmySplitLine(FArmySplitLine* Copy)
{

}

void FArmySplitLine::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	/**
	* 区域分割 分割线的序列化
	* @纪仁泽
	*/
	FArmyObject::SerializeToJson(JsonWriter);

	/** 序列化房间编号 */
	JsonWriter->WriteValue(TEXT("SpaceObjectId"),SpaceObjectId);

	/** 序列化组成分割线的线段 */
	JsonWriter->WriteArrayStart(TEXT("LineList"));
	for (auto& LineSegment : LineList)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue(TEXT("LineStart"), LineSegment->GetStart().ToString());
		JsonWriter->WriteValue(TEXT("LineEnd"), LineSegment->GetEnd().ToString());
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	/** 序列化分割线依附的分割线 */
	JsonWriter->WriteArrayStart(TEXT("DependentSplitLineIdList"));
	for (auto& DependentSplitLine:DependentList)
	{
		JsonWriter->WriteValue(DependentSplitLine->GetUniqueID().ToString());
	}
	JsonWriter->WriteArrayEnd();

	/** 分割线索引 */
	JsonWriter->WriteValue("SplitLineIndex", SplitLineIndex);

	SERIALIZEREGISTERCLASS(JsonWriter, FArmySplitLine)
}

// 反序列化为对象
void FArmySplitLine::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	/**
	* 区域分割 分割线的反序列化
	* @纪仁泽
	*/
	FArmyObject::Deserialization(InJsonData);
	/** 房间编号 */
	SpaceObjectId = InJsonData->GetStringField(TEXT("SpaceObjectId"));

	/** 组成分割线的线段 */
	TArray<TSharedPtr<FJsonValue> > SplitLineSegmentJson = InJsonData->GetArrayField("LineList");
	for (auto& SplitLineSegment : SplitLineSegmentJson)
	{
		const TSharedPtr<FJsonObject> LineSegmentJsonObj = SplitLineSegment->AsObject();

		/** string 线段的开始点，结束点 */
		FString LineSegmentStart = LineSegmentJsonObj->GetStringField(TEXT("LineStart"));
		FString LineSegmentEnd = LineSegmentJsonObj->GetStringField(TEXT("LineEnd"));
		TSharedPtr<FArmyLine> TempSplitLineSegment = MakeShareable(new FArmyLine);

		/** set开始点，结束点 */
		FVector CurrentStartPoint;
		CurrentStartPoint.InitFromString(LineSegmentStart);
		FVector CurrentEndPoint;
		CurrentEndPoint.InitFromString(LineSegmentEnd);
		TempSplitLineSegment->SetStart(CurrentStartPoint);
		TempSplitLineSegment->SetEnd(CurrentEndPoint);
		LineList.Emplace(TempSplitLineSegment);
	}

	/** 依附的分割线 */
	TArray<FString> DependentSpliteLineIdArray;
	InJsonData->TryGetStringArrayField("DependentSplitLineIdList", DependentSpliteLineIdArray);
	for (auto& DependentSpliteLineId : DependentSpliteLineIdArray)
	{
		/** set 依附的分割线ID */
		FGuid LineGUID;
		FGuid::Parse(DependentSpliteLineId, LineGUID);
		DependentIdList.Emplace(LineGUID);
	}
	/** 分割线索引 */
	SplitLineIndex = InJsonData->GetIntegerField("SplitLineIndex");
}

// 重载绘制
void FArmySplitLine::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	for (auto line : LineList)
	{
		line->bIsDashLine = true;
		line->SetLineWidth(SPlITLINE_WIDTH);
		line->Draw(PDI, View);
	}
}

// 设置分割线状态
void FArmySplitLine::SetState(EObjectState InState)
{
	State = InState;

	switch (InState)
	{
	case OS_Normal:
		Color = FLinearColor(FColor(0xFFFFFFFF));
		break;
	case OS_Selected:
		Color = FLinearColor(FColor(0xFFFF9800));
		break;
	default:
		break;
	}

	for (auto &line : LineList)
	{
		line->SetBaseColor(Color);
		line->SetNormalColor(Color);
	}
}

void FArmySplitLine::GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{
	  for (auto id : LineList)
	  {
		  OutLines.Add(id);
	  }
}

// 选中
bool FArmySplitLine::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	for (auto line : LineList)
	{
		FVector ClosePoint = FMath::ClosestPointOnLine(line->GetStart(), line->GetEnd(),Pos);
		FVector2D PixPos, TempPixPos;
		InViewportClient->WorldToPixel(Pos, PixPos);
		InViewportClient->WorldToPixel(ClosePoint, TempPixPos);
		if ((PixPos - TempPixPos).Size() < 10)
		{
			return true;
		}	
	}
	return false;
}

// 设置依附的分割线id
void FArmySplitLine::SetDependentIdList()
{
	DependentIdList.Reset();
	for (auto SplitLine : DependentList)
	{
		DependentIdList.Add(SplitLine->GetUniqueID());
	}
}