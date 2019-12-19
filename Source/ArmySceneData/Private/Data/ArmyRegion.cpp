/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRRegion.cpp
* @Description 区域
*
* @Author 常远
* @Date 2018年12月17日
* @Version 1.0
*/
#include "ArmyRegion.h"
#include "ArmyEngineModule.h"
#include "ArmyWallLine.h"
#include "ArmySplitLine.h"
#include "ArmyStyle.h"
#include "ArmyMath.h"
#include "ArmyPolygon.h"
#include "ArmyEditPoint.h"
#include "ArmyPolygonTool.h"
#include "ArmySceneData.h"
#include "ArmyRoom.h"



FArmyRegion::FArmyRegion()
{
	ObjectType = OT_Region;
	SetPropertyFlag(FLAG_PICKPRIMITIVE, true);
	SetPropertyFlag(FLAG_COLLISION, true);
	SetName(TEXT("区域")/* + GetUniqueID().ToString()*/);

	RegionName = TEXT("未命名");
	RegionId = 1;

	SelectedRoomSpace = MakeShareable(new FArmyPolygon);
	SelectedRoomSpace->FillColor = FLinearColor(FColor(0xCCFFFFFF));
	SelectedRoomSpace->MaterialRenderProxy = FArmyEngineModule::Get().GetEngineResource()->GetRoomSpaceMaterial()->GetRenderProxy(false);

	IsVirtual = false;
}

FArmyRegion::FArmyRegion(FArmyRegion* Copy)
{

}

void FArmyRegion::GetSplitAreaAndPerimeter(float& OutArea, float& OutPerimeter)
{
	/** 区域不闭合 则周长和面积默认为0.0 */
	if (!IsClosed())
	{
		OutArea = 0.0f;
		OutPerimeter = 0.0f;
		return;
	}
	/** 计算面积 */
	TArray<FVector> TempPoints= EditPointToVector();
	OutArea = FArmyMath::CalcPolyArea(TempPoints) * 0.0001f;

	/** 清空周长 */
	OutPerimeter = 0;
	/** 计算周长 */
	TArray<TSharedPtr<class FArmyLine>> TempLines= GetRegionLines();
	for (auto& It : TempLines)
	{
		OutPerimeter += It->Size();
	}
	OutPerimeter /= 100.f;
}

// 设置是否虚拟区域
void FArmyRegion::SetIsVirtual(bool InIsVirtual)
{
	IsVirtual = InIsVirtual;
}

// 序列化json数据
void FArmyRegion::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	/**
	* 区域分割 区域的序列化
	* @纪仁泽
	*/
	FArmyObject::SerializeToJson(JsonWriter);

	/** 房间编号，区域名称，区域ID */
	JsonWriter->WriteValue(TEXT("SpaceObjectId"), SpaceObjectId);
	JsonWriter->WriteValue(TEXT("RegionName"), RegionName);
	JsonWriter->WriteValue("RegionId", RegionId);
	if (IsVirtual)
	{
		JsonWriter->WriteValue("is_virtual", 1);
	}
	else
	{
		JsonWriter->WriteValue("is_virtual", 0);
	}
	/** 组成区域的分割线 */
	JsonWriter->WriteArrayStart(TEXT("SplitLineIdList"));
	for (auto& SplitLine : SplitLineList)
	{
		/** 分割线ID */
		JsonWriter->WriteValue(SplitLine->GetUniqueID().ToString());
	}
	JsonWriter->WriteArrayEnd();

	/** 组成区域的虚线 */
	JsonWriter->WriteArrayStart(TEXT("VirtualLineList"));
	for (auto& VirtualLine : VirtualLineList)
	{
		JsonWriter->WriteObjectStart();
		/** 虚线开始点，虚线结束点 */
		JsonWriter->WriteValue(TEXT("LineStart"), VirtualLine->GetStart().ToString());
		JsonWriter->WriteValue(TEXT("LineEnd"), VirtualLine->GetEnd().ToString());
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	/** 组成区域的点的集合 */
	JsonWriter->WriteArrayStart(TEXT("PointList"));
	for (auto& Point : PointList)
	{
		JsonWriter->WriteValue(Point->GetPos().ToString());
	}
	JsonWriter->WriteArrayEnd();

	/** 组成区域的墙的集合 */
	JsonWriter->WriteArrayStart(TEXT("WallList"));
	for (auto& Wall : WallList)
	{
		JsonWriter->WriteObjectStart();
		/** 墙线ID，墙线开始点，墙线结束点 */
		JsonWriter->WriteValue(TEXT("WallLineID"), Wall->GetUniqueID().ToString());
		JsonWriter->WriteValue(TEXT("WallLineStart"), Wall->GetCoreLine()->GetStart().ToString());
		JsonWriter->WriteValue(TEXT("WallLineEnd"), Wall->GetCoreLine()->GetEnd().ToString());
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	SERIALIZEREGISTERCLASS(JsonWriter, FArmyRegion)
}

// 反序列化为对象
void FArmyRegion::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	/**
	* 区域分割的反序列化
	* @纪仁泽
	*/

	/** 清空现有的数据 */
	WallList.Empty();
	SplitLineList.Empty();
	VirtualLineList.Empty();
	PointList.Empty();
	WallList.Empty();

	FArmyObject::Deserialization(InJsonData);

	/** 房间编号，区域名称，区域ID */
	SpaceObjectId = InJsonData->GetStringField(TEXT("SpaceObjectId"));
	RegionName = InJsonData->GetStringField(TEXT("RegionName"));
	RegionId = InJsonData->GetIntegerField("RegionId");
	int32 InIsVirtual = InJsonData->GetIntegerField("IsVirtual");
	if (InIsVirtual == 1)
	{
		IsVirtual = true;
	}
	else
	{
		IsVirtual = false;
	}

	/** 组成区域的分割线ID */
	TArray<FString> SplitLineIdArray;
	InJsonData->TryGetStringArrayField("SplitLineIdList", SplitLineIdArray);
	for (auto& SplitLineId : SplitLineIdArray)
	{
		/** set分割线ID */
		FGuid LineGUID;
		FGuid::Parse(SplitLineId, LineGUID);
		SplitLineIdList.Emplace(LineGUID);
	}

	/** 组成区域的虚线 */
	TArray<TSharedPtr<FJsonValue> > VirtualLinesJson = InJsonData->GetArrayField("VirtualLineList");
	for (auto& VirtualLine : VirtualLinesJson)
	{
		const TSharedPtr<FJsonObject> VirtualJsonObj = VirtualLine->AsObject();

		/** string 虚线开始点，虚线结束点 */
		FString VirtualLineStart = VirtualJsonObj->GetStringField(TEXT("LineStart"));
		FString VirtualLineEnd = VirtualJsonObj->GetStringField(TEXT("LineEnd"));
		TSharedPtr<FArmyLine> TempVirtualNewLine = MakeShareable(new FArmyLine);

		/** set开始点，结束点 */
		FVector CurrentStartPoint;
		CurrentStartPoint.InitFromString(VirtualLineStart);
		FVector CurrentEndPoint;
		CurrentEndPoint.InitFromString(VirtualLineEnd);
		TempVirtualNewLine->SetStart(CurrentStartPoint);
		TempVirtualNewLine->SetEnd(CurrentEndPoint);
		VirtualLineList.Emplace(TempVirtualNewLine);
	}

	/** 组成区域的点的集合 */
	TArray<FString> VertexesStrArray;
	InJsonData->TryGetStringArrayField("PointList", VertexesStrArray);
	for (auto& ArrayIt : VertexesStrArray)
	{
		TSharedPtr<FArmyEditPoint> TempEditPoint = MakeShareable(new FArmyEditPoint);
		/** set 点坐标 */
		FVector CurrentPoint;
		CurrentPoint.InitFromString(ArrayIt);
		TempEditPoint->SetPos(CurrentPoint);
		PointList.Emplace(TempEditPoint);
	}
	/** 组成区域的墙的集合 */
	TArray<TSharedPtr<FJsonValue> > WallLinesJson = InJsonData->GetArrayField("WallList");
	for (auto& WallLine : WallLinesJson)
	{
		const TSharedPtr<FJsonObject> WallJsonObj = WallLine->AsObject();

		/** String 墙线ID，墙线开始点，墙线结束点 */
		FString WallLineId = WallJsonObj->GetStringField(TEXT("WallLineID"));
		FString WallLineStart = WallJsonObj->GetStringField(TEXT("WallLineStart"));
		FString WallLineEnd = WallJsonObj->GetStringField(TEXT("WallLineEnd"));

		/** 构造临时新的墙线 */
		TSharedPtr<FArmyWallLine> TempWallNewLine = MakeShareable(new FArmyWallLine);
		TempWallNewLine->SetColor(FLinearColor::Gray);
		TempWallNewLine->SetWidth(WALLLINEWIDTH);

		/** set墙线ID */
		FGuid LineGUID;
		FGuid::Parse(WallLineId, LineGUID);
		TempWallNewLine->SetUniqueID(LineGUID);

		/** set开始点，结束点 */
		FVector CurrentStartPoint;
		CurrentStartPoint.InitFromString(WallLineStart);
		FVector CurrentEndPoint;
		CurrentEndPoint.InitFromString(WallLineEnd);
		TempWallNewLine->GetCoreLine()->SetStart(CurrentStartPoint);
		TempWallNewLine->GetCoreLine()->SetEnd(CurrentEndPoint);
		WallList.Emplace(TempWallNewLine);
	}
}

// 设置状态
void FArmyRegion::SetState(EObjectState InState)
{
	State = InState;

	switch (InState)
	{
	case OS_Normal:
		Color = FLinearColor::Gray;
		break;

	case OS_Hovered:
		Color = FLinearColor::Blue;
		break;

	case OS_Selected:
		Color = FLinearColor(FColor(0xFFFFFFFF));
		break;

	case OS_Disable:
		Color = FLinearColor::Gray;
		break;

	default:
		break;
	}
}
// 重绘
void FArmyRegion::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	// 如果是选中状态
	if (GetState() == OS_Selected)
	{
		// 绘制实墙
		for (auto wall : this->WallList)
		{
			wall->GetCoreLine()->Draw(PDI, View);
		}

		// 绘制虚墙
		for (auto line : this->VirtualLineList)
		{
			line->bIsDashLine = true;
			line->SetLineWidth(SPlITLINE_WIDTH);
			line->Draw(PDI, View);
		}

		// 绘制点
		/*for (auto It : PointList)
		{
			It->Draw(PDI, View);
		}*/

		// 绘制区域
		if (IsClosed())
		{
			SelectedRoomSpace->SetVertices(EditPointToVector());
			SelectedRoomSpace->Draw(PDI, View);
		}
	}
}
// 选中
bool FArmyRegion::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	if (!IsClosed())
		return false;

	// 点是否在多边形内
	return FArmyMath::IsPointInOrOnPolygon2D(Pos, EditPointToVector());

	return false;
}

//区域是否闭合
bool FArmyRegion::IsClosed()
{
	if (PointList.Num() < 3)
	{
		return false;
	}

	if (PointList.Num() == WallList.Num() + VirtualLineList.Num())
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 获得组成区域的所有线段
TArray<TSharedPtr<class FArmyLine>> FArmyRegion::GetRegionLines()
{
	// 收集所有的线段
	TArray<TSharedPtr<FArmyLine>> AllLines;

	// 虚墙段
	for (auto &virtualLine : VirtualLineList)
	{
		AllLines.Push(virtualLine);
	}

	// 墙段
	for (auto &wallLine : WallList)
	{
		AllLines.Push(wallLine->GetCoreLine());
	}

	return AllLines;
}

// 生成有效的闭合区域的点
bool FArmyRegion::CreatePolygonPoints()
{
	// 重置所有点
	TSet<FVector> Points;
	TArray<TSharedPtr<FArmyLine>> RegionLines = GetRegionLines();

	// 遍历所有的墙
	for (auto &line : RegionLines)
	{
		Points.Add(line->GetStart());
		Points.Add(line->GetEnd());
	}

	// 没有生产闭合区域的条件
	if (Points.Num() != RegionLines.Num() || Points.Num() < 3)
	{
		return false;
	}
	// 把节点进行排序
	TSharedPtr<FArmyPolygonTool> TPolygon = MakeShareable(new FArmyPolygonTool());
	// 添加节点
	for(auto point : Points)
	{
		TPolygon->AddNode(point);
	}

	// 添加边
	for (auto line : RegionLines)
	{
		TPolygon->AddEdge(line->GetStart(), line->GetEnd());
	}
	// 生成多边形
	TArray<FVector>  PolygonPoints = TPolygon->SearchSinglePolygon();
	if (PolygonPoints.Num() == Points.Num())
	{
		PointList.Reset();
		for (auto &Pos : PolygonPoints)
		{
			TSharedPtr<FArmyEditPoint> EditPoint = MakeShareable(new FArmyEditPoint(Pos));
			PointList.Add(EditPoint);
		}

		return true;
	}

	return false;
	
}

// 获得区域id
int32 FArmyRegion::GetRegionId()
{
	return RegionId;
}
//获得区域空间名称
FString FArmyRegion::GetRegionSpaceName()
{
	FString RegionSpaceName = "未命名";
	FGuid TmpID;
	if (FGuid::Parse(SpaceObjectId, TmpID))
	{
		FObjectWeakPtr RoomOPtr = FArmySceneData::Get()->GetObjectByGuid(E_LayoutModel, TmpID);
		if (RoomOPtr.IsValid())
		{
			TSharedPtr<FArmyRoom> RoomObj = StaticCastSharedPtr<FArmyRoom>(RoomOPtr.Pin());
			return RoomObj->GetSpaceName();
		}
		else
		{
			return RegionSpaceName;
		}
	}
	else
	{
		return RegionSpaceName;
	}
	
}
// 把编辑的点装换成vector
TArray<FVector> FArmyRegion::EditPointToVector()
{
	TArray<FVector> VectorList;
	if (IsClosed())
	{
		for (auto &EditPoint : PointList)
		{
			VectorList.Push(EditPoint->GetPos());
		}
	}
	return VectorList;
}

// 设置组成区域的分割线ID
void FArmyRegion::SetSplitLineIdList()
{
	SplitLineIdList.Reset();
	for (auto SplitLine : SplitLineList)
	{
		SplitLineIdList.Add(SplitLine->GetUniqueID());
	}
}