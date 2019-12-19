#include "ArmyRegionOperation.h"
#include "ArmyAddWall.h"
#include "ArmyMath.h"
#include "ArmyPolygon.h"
#include "ArmyRoom.h"
#include "ArmyEngine/Public/XREngineModule.h"
#include "ArmySceneData.h"
#include "ArmyToolsModule.h"
#include "ArmyMouseCapture.h"
#include "ArmyRectSelect.h"
#include "ArmyGameInstance.h"
#include "SArmyAddWallSetting.h"
#include "ArmyRulerLine.h"
#include "ArmyClipper.h"
#include "ArmyWallLine.h"

FArmyRegionOperation::FArmyRegionOperation(EModelType InBelongModel)
	: FArmyOperation(InBelongModel),
	SolidLine(MakeShareable(new FArmyLine()))
{
	Color = FLinearColor(FColor(0xFFFFFFFF));
	/** 初始化标尺线,设置输入线长回调 */
	RulerLine = MakeShareable(new FArmyRulerLine());
	RulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyRegionOperation::OnLineInputBoxCommitted));
}

// 开始区域划分操作 
void FArmyRegionOperation::BeginOperation(XRArgument InArg)
{
	/** 设置鼠标捕捉功能 */
	CurrentArg = InArg;
	FArmyOperation::BeginOperation(InArg);
	if (InArg._ArgInt32 != 0) // 删除时不清空选中状态
	{
		FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	}
	FArmyToolsModule::Get().GetRectSelectTool()->End();
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = true;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_Auxiliary;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_Ortho;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_LineRange;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_Closest;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_IntersectionPoint;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_Perpendicular;

}

// 结束区域划分操作
void FArmyRegionOperation::EndOperation()
{
	RulerLine->ShowInputBox(false);
	FArmyOperation::EndOperation();
	SolidLine->SetStart(FVector::ZeroVector);
	SolidLine->SetEnd(FVector::ZeroVector);
	ClearCacheData();
}

void FArmyRegionOperation::ReDraw()
{
	RulerLine->ShowInputBox(false);
	SolidLine->SetStart(FVector::ZeroVector);
	SolidLine->SetEnd(FVector::ZeroVector);
	ClearCacheData();
}

// 绘制缓存线 
void FArmyRegionOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (CachedPoints.Num() > 0)
	{
		SolidLine->bIsDashLine = true;
		SolidLine->SetBaseColor(Color);
		SolidLine->SetLineWidth(SPlITLINE_WIDTH);
		SolidLine->Draw(PDI, View);
	}
		

	if (CachedPoints.Num() > 1)
	{
		for (int i = 0; i < CachedPoints.Num() - 1; ++i)
		{
			TSharedPtr<FArmyLine> line = MakeShareable(new FArmyLine(CachedPoints[i], CachedPoints[i + 1]));
			line->bIsDashLine = true;
			line->SetBaseColor(Color);
			line->SetLineWidth(SPlITLINE_WIDTH);
			line->Draw(PDI, View);
			//PDI->DrawLine(CachedPoints[i], CachedPoints[i + 1], FLinearColor(0.5, 0.5, 0.2), 0);
		}
	}


}

// 鼠标移动 
void FArmyRegionOperation::MouseMove(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y)
{
	static int32 OldX = X, OldY = Y;
	if (OldX == X && OldY == Y)
	{
		return;
	}
	else
	{
		OldX = X;
		OldY = Y;
	}
	if (SolidLine.IsValid() && !(SolidLine->GetStart().Equals(SolidLine->GetEnd())))
	{
		RulerLine->Update(SolidLine->GetStart(), SolidLine->GetEnd());
		RulerLine->SetInputBoxFocus(true);
	}
}

// 选中的构件
void FArmyRegionOperation::SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive)
{	
	SelectedSplitLine = StaticCastSharedPtr<FArmySplitLine>(Object);
	RelativeSplitLines = GetRelativeSplitLines(SelectedSplitLine);
	if (RelativeSplitLines.Num() > 0)
	{
		GGI->Window->PresentModalDialog(TEXT("删除此分割线，将删除与其关联的分割线。是否继续？"),
			FSimpleDelegate::CreateRaw(this, &FArmyRegionOperation::DeleteExecute));
	}
	else
	{
		DeleteExecute();
	}
}

// 删除分割线
void FArmyRegionOperation::DeleteExecute()
{
	DeleteSplitLineAndSort(SelectedSplitLine);
	for (auto ref : RelativeSplitLines)
	{
		DeleteSplitLineAndSort(ref);
	}
	UpdateRegionList(SelectedSplitLine->SpaceObjectId);

	RelativeSplitLines.Reset();
	SelectedSplitLine = nullptr;
	EndOperation();
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	FArmySceneData::LayOutMode_ModifyMultiDelegate.Broadcast();
}

// 每帧捕捉Slide Line 
void FArmyRegionOperation::Tick()
{
	FArmyOperation::Tick();
	if (CachedPoints.Num() > 0)
	{
		//每帧设置实体线
		SolidLine->SetStart(CachedPoints.Last());
		FVector CapturePoint(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0);
		SolidLine->SetEnd(CapturePoint);
	}
}

// 处理输入
bool FArmyRegionOperation::InputKey(UArmyEditorViewportClient * InViewPortClient, FViewport * InViewPort, FKey Key, EInputEvent Event)
{
	//左键操作相应
	if (Key == EKeys::LeftMouseButton &&Event == EInputEvent::IE_Pressed)
	{
		FVector CapturePoint(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0);	 
		//添加缓存点
		if (AddCachePoint(CapturePoint))
		{
			FArmyToolsModule::Get().GetMouseCaptureTool()->SetCurrentPoint(FVector2D(CapturePoint));
		}
		return true;
	}
	return false;
}

//清理数据
void FArmyRegionOperation::ClearCacheData()
{
	CachedPoints.Empty();
	RulerLine->ShowInputBox(false);
	SolidLine->SetStart(FVector::ZeroVector);
	SolidLine->SetEnd(FVector::ZeroVector);
}

//添加缓存点
bool FArmyRegionOperation::AddCachePoint(const FVector& Pos)
{
	//获取所有的虚线,和区域
	TArray<TWeakPtr<FArmyObject>>	  RegionList;
	TArray<TWeakPtr<FArmyObject>>	  RoomList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Region, RegionList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
	//第一次操作,没有任何区域,把所有的区域添加到
	if (!CachedPoints.Num())
	{
		//@~1 落点在区域内
		bool  isOnLine = false;
		for (int i = 0; i < RegionList.Num(); ++i)
		{
			TSharedPtr<FArmyRegion> temp = StaticCastSharedPtr<FArmyRegion>(RegionList[i].Pin());
			if (IsPointOnRegionLine(Pos, temp))
			{
				isOnLine = true;
			}
		}
		//@ ~2 落在房间内
		if (!isOnLine)
		{
			for (int i = 0; i < RoomList.Num(); ++i)
			{
				TSharedPtr<FArmyRoom> temp = StaticCastSharedPtr<FArmyRoom>(RoomList[i].Pin());
				if (IsPointOnRoomLines(Pos, temp))
				{
					isOnLine = true;
				}
			}
		}
		if (!isOnLine)
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("请在内墙上/分割线上进行绘制 "));
			return false;
		}
		else
		{
			CachedPoints.Add(Pos);
			return true;
		}
	}
	else
	{
		//拿到区域信息
		TSharedPtr<FArmyRegion> LastRegion = SearchRegionByCachePoint(Pos, RegionList);
		TSharedPtr<FArmyRoom>   LastRoom = SearchRoomByCachePoint(Pos, RoomList);
		auto RoomLines = LastRoom->GetWallLines();
		for (auto line : RoomLines)
		{
			FVector CurrentIntersect;
			if (FArmyMath::CalculateLinesCross(line->GetCoreLine()->GetStart(), line->GetCoreLine()->GetEnd(), Pos, CachedPoints.Last(), CurrentIntersect))
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("请在当前空间进行绘制"));
				return false;
			}
		}
		// 自相交
		if (IsSelfIntersection(Pos))
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("不允许与分割线/内墙相交"));
			return false;
		}
		// 自重叠
		if (this->IsOverlayWithSelf(Pos)) {
			GGI->Window->ShowMessage(MT_Warning, TEXT("不允许与分割线/内墙重合"));
			return false;
		}
		// 两点一样
		if (this->IsSameWithLastPoint(Pos)) {
			GGI->Window->ShowMessage(MT_Warning, TEXT("不允许与分割线/内墙交叉"));
			return false;
		}
		//处理点击在区域上
		if (LastRegion.IsValid())
		{
			// 与区域重叠
			if (IsOverlayWithRegion(Pos, LastRegion)) {
				GGI->Window->ShowMessage(MT_Warning, TEXT("不允许与分割线/内墙重合"));
				return false;
			}
			// 与区域相交
			auto RegionLines = LastRegion->GetRegionLines();
			for (auto line : RegionLines)
			{
				FVector CurrentIntersect;
				if (FArmyMath::CalculateLinesCross(line->GetStart(), line->GetEnd(), Pos, CachedPoints.Last(), CurrentIntersect))
				{
					GGI->Window->ShowMessage(MT_Warning, L"不允许与分割线/内墙交叉");
					return false;
				}
			}
			CachedPoints.Add(Pos);
			//结束绘制
			if (IsPointOnRegionLine(Pos, LastRegion)|| IsPointOnRoomLines(Pos, LastRoom))
			{
			    ProduceSplitLine(LastRoom->GetUniqueID().ToString());
				RealTimeFlushRegionList(LastRoom);
				ReDraw();
				return true;
			}
			return true;
		}
		//处理点击在房间内,区域为NULL
		bool spanRegion = IsSpanRegion(Pos, RegionList);
		if (spanRegion)
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("不允许与分割线/内墙交叉"));
			return false;
		}
		if (LastRoom.IsValid()&&!LastRegion.IsValid())
		{
			// 与房间重叠
			if (IsOverlayWithRegion(Pos, LastRoom)) {
				GGI->Window->ShowMessage(MT_Warning, TEXT("不允许与分割线/内墙重合"));
				return false;
			}	
			CachedPoints.Add(Pos);
			//结束绘制
			if (IsPointOnRoomLines(Pos, LastRoom))
			{
				ProduceSplitLine(LastRoom->GetUniqueID().ToString());
				RealTimeFlushRegionList(LastRoom);
				ReDraw();
				return true;
			}
			return true;
		}
		//如果两个都是无效的,跨区域
		if (!LastRegion.IsValid() && !LastRoom.IsValid())
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("请在当前空间进行绘制"));
			return false;
		}
	}
	return false;
}

//根据缓存点查询区域
TSharedPtr<FArmyRegion> FArmyRegionOperation::SearchRegionByCachePoint(const FVector& vector, TArray<TWeakPtr<FArmyObject>> &RegionList)
{
	TSharedPtr<FArmyRegion> LastRegion;
	for (int i = 0; i < RegionList.Num(); ++i)
	{
		TSharedPtr<FArmyRegion> temp = StaticCastSharedPtr<FArmyRegion>(RegionList[i].Pin());
		auto midPoint = (CachedPoints.Last() + vector) / 2;
		if (IsPointInRegion(midPoint, temp))
			LastRegion = temp;
	}
	return LastRegion;
}

//横跨区域
bool FArmyRegionOperation::IsSpanRegion(const FVector& vector, TArray<TWeakPtr<FArmyObject>> &RegionList)
{
	for (int i = 0; i < RegionList.Num(); ++i)
	{
		TSharedPtr<FArmyRegion> temp = StaticCastSharedPtr<FArmyRegion>(RegionList[i].Pin());
		auto midPoint = (CachedPoints.Last() + vector) / 2;
		if (IsPointInRegion(midPoint, temp) && (!IsPointInRegion(CachedPoints.Last(), temp) || !IsPointInRegion(vector, temp)))
			return true;
	}
	return false;
}

//根据缓存点查询房间
TSharedPtr<FArmyRoom> FArmyRegionOperation::SearchRoomByCachePoint(const FVector& vector, TArray<TWeakPtr<FArmyObject>> &RoomList)
{
	TSharedPtr<FArmyRoom> Room;
	for (int i = 0; i < RoomList.Num(); ++i)
	{
		TSharedPtr<FArmyRoom> temp = StaticCastSharedPtr<FArmyRoom>(RoomList[i].Pin());
		auto midPoint = (CachedPoints.Last()+ vector)/2;
		if (IsPointOnInternelRoom(midPoint, temp))
		{
			Room = temp;
		}
	}
	return Room;
}

//产生分割线
void FArmyRegionOperation::ProduceSplitLine(FString spaceId)
{
	TSharedPtr<FArmySplitLine> SplitLine = MakeShareable(new FArmySplitLine());
	for (int i = 0; i < CachedPoints.Num() - 1; ++i)
	{
		int j = i + 1;
		SplitLine->LineList.Add(MakeShareable(new FArmyLine(CachedPoints[i], CachedPoints[j])));
	}
	SplitLine->SpaceObjectId = spaceId;
	//获取线段索引
	AddSplitLine(SplitLine, spaceId);

}

//填充区域边界
void FArmyRegionOperation::FillRegionLinesToArray(TArray<TPair<FArmyLine, bool>>&LinesArray, TSharedPtr<FArmyRegion> room)
{
 
	for (int i = 0; i < room->WallList.Num(); ++i)
	{
		auto line = room->WallList[i]->GetCoreLine();
		//添加实体墙
		LinesArray.Add(TPair<FArmyLine, bool>(FArmyLine(line->GetStart(), line->GetEnd()), false));
	}


	for (int i = 0; i < room->VirtualLineList.Num(); ++i)
	{
		auto line = room->VirtualLineList[i];
		//添加虚墙
		LinesArray.Add(TPair<FArmyLine, bool>(FArmyLine(line->GetStart(), line->GetEnd()), true));
	}
	
}

//填充区域边界
void FArmyRegionOperation::FillRegionLinesToArray(TArray<TPair<FArmyLine, bool>>&LinesArray, TSharedPtr<FArmyRoom> room)
{
	auto WallLines = room->GetWallLines();
	for (int i = 0; i < WallLines.Num(); ++i)
	{
		auto line = WallLines[i]->GetCoreLine();
		//添加实体墙
		LinesArray.Add(TPair<FArmyLine, bool>(FArmyLine(line->GetStart(), line->GetEnd()), false));
	}
}
 
//回调
void FArmyRegionOperation::OnLineInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		/** 添加一条固定长度的直线 */
		const int32 LineLength = FCString::Atoi(*InText.ToString());
		if (LineLength == 0)
			return;

		SCOPE_TRANSACTION(TEXT("移动线"));

		FVector LineDirection = FArmyMath::GetLineDirection(SolidLine->GetStart(), SolidLine->GetEnd());
		FVector Offset = (LineLength / 10.f) * LineDirection;
		FVector LineEndPos = SolidLine->GetStart() + Offset;
		if (AddCachePoint(LineEndPos))
		{
			FArmyToolsModule::Get().GetMouseCaptureTool()->SetCurrentPoint(FVector2D(LineEndPos));
		}
			
	}
}

//在区域内
bool FArmyRegionOperation::IsPointInRegion(FVector vector, TSharedPtr<FArmyRegion> region)
{
	TArray<FVector> StartPoint;
	TArray<FVector> EndPoint;

	for (int i = 0; i < region->WallList.Num(); ++i)
	{
		auto line = region->WallList[i]->GetCoreLine();
		StartPoint.Add(line->GetStart());
		EndPoint.Add(line->GetEnd());
	}

	for (int i = 0; i < region->VirtualLineList.Num(); ++i)
	{
		auto line = region->VirtualLineList[i];
		StartPoint.Add(line->GetStart());
		EndPoint.Add(line->GetEnd());
	}

	bool result = false;
	for (int i = 0; i < StartPoint.Num(); ++i)
	{
		FVector p1 = StartPoint[i];
		FVector p2 = EndPoint[i];
		if (FArmyMath::IsPointOnLineSegment2D(FVector2D(vector), FVector2D(p1), FVector2D(p2)))
			return true;
		if (p1.X < vector.X && p2.X >= vector.X || p2.X < vector.X && p1.X >= vector.X)
		{
			if (p1.Y + (vector.X - p1.X) / (p2.X - p1.X) * (p2.Y - p1.Y) < vector.Y)
			{
				result = !result;
			}
		}
	}
	return result;
}
 
//在区域线上
bool FArmyRegionOperation::IsPointOnRegionLine(FVector vector, TSharedPtr<FArmyRegion> region)
{

	TArray<FVector> StartPoint;
	TArray<FVector> EndPoint;

	for (int i = 0; i < region->WallList.Num(); ++i)
	{
		auto line = region->WallList[i]->GetCoreLine();
		StartPoint.Add(line->GetStart());
		EndPoint.Add(line->GetEnd());
	}

	for (int i = 0; i < region->VirtualLineList.Num(); ++i)
	{
		auto line = region->VirtualLineList[i];
		StartPoint.Add(line->GetStart());
		EndPoint.Add(line->GetEnd());
	}

	for (int i = 0; i < StartPoint.Num(); ++i)
	{
		FVector p1 = StartPoint[i];
		FVector p2 = EndPoint[i];
		if (FArmyMath::IsPointOnLineSegment2D(FVector2D(vector), FVector2D(p1), FVector2D(p2)))
			return true;
	}
	return false;
}

//在房间内
bool FArmyRegionOperation::IsPointOnInternelRoom(FVector vector, TSharedPtr<FArmyRoom> region)
{
	return region->IsPointInRoom(vector);
}

//在房间线上
bool  FArmyRegionOperation::IsPointOnRoomLines(FVector vector, TSharedPtr<FArmyRoom> region)
{

	auto PointsList = region->GetWorldPoints();
	for (int i = 0; i < PointsList.Num(); ++i)
	{
		int j = (i + 1) % PointsList.Num();
		if (FArmyMath::IsPointOnLine(vector, PointsList[i], PointsList[j]))
			return true;
	}
	return false;
}

//自相交
bool FArmyRegionOperation::IsSelfIntersection(const FVector&Vector)
{
	if (CachedPoints.Num() >= 2)
	{
		auto this_point = Vector;
		auto last_point = CachedPoints.Last();
		for (int i = 0; i < CachedPoints.Num()-2; ++i)
		{
			FVector out;
			if (FArmyMath::SegmentIntersection2D(CachedPoints[i], CachedPoints[1 + i], this_point, last_point, out))
				return true;
		}
	}
	return false;
}
 
//与区域重叠
bool FArmyRegionOperation::IsOverlayWithRegion(const FVector&Vector, TSharedPtr<FArmyRegion> region)
{
	TArray<FVector> StartPoint;
	TArray<FVector> EndPoint;

	for (int i = 0; i < region->WallList.Num(); ++i)
	{
		auto line = region->WallList[i]->GetCoreLine();
		StartPoint.Add(line->GetStart());
		EndPoint.Add(line->GetEnd());
	}


	for (int i = 0; i < region->VirtualLineList.Num(); ++i)
	{
		auto line = region->VirtualLineList[i];
		StartPoint.Add(line->GetStart());
		EndPoint.Add(line->GetEnd());
	}

	for (int i = 0; i < StartPoint.Num(); ++i)
	{
		FVector p1 = StartPoint[i];
		FVector p2 = EndPoint[i];
		if (FArmyMath::IsPointOnLineSegment2D(FVector2D(Vector), FVector2D(p1), FVector2D(p2))
			&& FArmyMath::IsPointOnLineSegment2D(FVector2D(CachedPoints.Last()), FVector2D(p1), FVector2D(p2)))
			return true;
	}
	return false;
}
 
//与房间重叠
bool FArmyRegionOperation::IsOverlayWithRegion(const FVector&Vector, TSharedPtr<FArmyRoom> room)
{
	TArray<FVector> pointList = room->GetWorldPoints();
	for (int i = 0; i < pointList.Num(); ++i)
	{
		int j = (i + 1) % pointList.Num();
		FVector beg = pointList[i];
		FVector end = pointList[j];
	 
		if (FArmyMath::IsPointOnLine(Vector, beg, end)&& FArmyMath::IsPointOnLine(CachedPoints.Last(), beg, end))
			return true;
	}
	return false;
}

//与上次点重合
bool FArmyRegionOperation::IsSameWithLastPoint(const FVector&Vector)
{
	return CachedPoints.Last() == Vector;
}
 
//回线重合
bool FArmyRegionOperation::IsOverlayWithSelf(const FVector&Vector)
{
	if (CachedPoints.Num() >= 2)
	{
		if (FArmyMath::IsPointOnLine(Vector, CachedPoints.Last(), CachedPoints.Last(1)))
			return true;
	}
	return false;
}
 
//计算某区域下分割线的个数
int  FArmyRegionOperation::CalculateNumberOfSplitLinesOnRegion(FString SpaceId)
{
	int Count = 0;
	//获取所有切割线
	TArray<TWeakPtr<FArmyObject>>	  ObjectArray;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_SplitLine, ObjectArray);

	//如果对象id 等于参数id,添加到同区域切割线
	TArray<TSharedPtr<FArmySplitLine>>	SplitLineWithSameId;
	for (int i = 0; i < ObjectArray.Num(); ++i)
	{
		TSharedPtr<FArmySplitLine> Temp = StaticCastSharedPtr<FArmySplitLine>(ObjectArray[i].Pin());
		if (Temp->SpaceObjectId == SpaceId)
			++Count;
	}
	return Count;
}

//收集相同唯一标识符号的所有分割线并根据分割线的索引排序,拿到有顺序的分割
TArray<TSharedPtr<FArmySplitLine>> FArmyRegionOperation::SearchSplitLinesWithSameSpaceIdOrderByIndex(FString SpaceId)
{
	//获取所有切割线
	TArray<TWeakPtr<FArmyObject>>	  ObjectArray;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_SplitLine, ObjectArray);
	

	//如果对象id 等于参数id,添加到同区域切割线
	TArray<TSharedPtr<FArmySplitLine>>	SplitLineWithSameId;
	for (int i = 0; i < ObjectArray.Num(); ++i)
	{
		TSharedPtr<FArmySplitLine> Temp = StaticCastSharedPtr<FArmySplitLine>(ObjectArray[i].Pin());
		if (Temp->SpaceObjectId == SpaceId)
			SplitLineWithSameId.Add(Temp);
	}

	//对区域进行排序
	SplitLineWithSameId.Sort([](const TSharedPtr<FArmySplitLine>& A, const TSharedPtr<FArmySplitLine>& B) {
		return  A->SplitLineIndex< B->SplitLineIndex;
	});
	return SplitLineWithSameId;
}

//收集相同唯一标识符下的区域数据集合
TArray<TSharedPtr<FArmyRegion>>	FArmyRegionOperation::SearchRegionWithSameSpaceId(FString SpaceId)
{
	//获取所有区域
	TArray<TWeakPtr<FArmyObject>>	  ObjectArray;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Region, ObjectArray);

	//如果对象id 等于参数id,添加到同区域
	TArray<TSharedPtr<FArmyRegion>>	RegionArray;
	for (int i = 0; i < ObjectArray.Num(); ++i)
	{
		TSharedPtr<FArmyRegion> Temp = StaticCastSharedPtr<FArmyRegion>(ObjectArray[i].Pin());
		if (Temp->SpaceObjectId == SpaceId)
			RegionArray.Add(Temp);
	}

	return RegionArray;
}

//添加切割线
void FArmyRegionOperation::AddSplitLine(TSharedPtr<FArmySplitLine> SplitLine, FString SpaceId)
{
	//所属的空间
	SplitLine->SpaceObjectId = SpaceId;

	//获取此区域下所有的线
	SplitLine->SplitLineIndex =CalculateNumberOfSplitLinesOnRegion(SpaceId);
	 
	//添加到HomeData
	XRArgument Arg = XRArgument()
								.ArgFName(FName(*FString(TEXT("基础类"))))
								.ArgString(SplitLine->GetName())
								.ArgUint32(E_LayoutModel);
	FArmySceneData::Get()->Add(SplitLine, Arg);

	TArray<FObjectWeakPtr> SplitLineArry;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, EObjectType::OT_SplitLine, SplitLineArry);
}

//删除切割线
void FArmyRegionOperation::DeleteSplitLineAndSort(TSharedPtr<FArmySplitLine> SplitLine)
{
	FArmySceneData::Get()->Delete(SplitLine);

   auto	splitLineArray= SearchSplitLinesWithSameSpaceIdOrderByIndex(SplitLine->SpaceObjectId);
   for (int i = 0; i < splitLineArray.Num(); ++i)
   {
	   splitLineArray[i]->SplitLineIndex = i;
   }
}

//获取切割线相关联的线
TArray<TSharedPtr<FArmySplitLine>> FArmyRegionOperation::GetRelativeSplitLines(TSharedPtr<FArmySplitLine> SplitLine)
{
 
	TArray<TSharedPtr<FArmySplitLine>>  LinesArray = SearchSplitLinesWithSameSpaceIdOrderByIndex(SplitLine->SpaceObjectId);
	TSet<int>	LinesIndexArray;
	TArray<TSharedPtr<FArmySplitLine>>  Result;
	SearchRelativeSplitLines(LinesIndexArray, SplitLine, LinesArray);
	for (auto i : LinesIndexArray)
	{
		Result.Add(LinesArray[i]);
	}
	return Result;
}

//搜索关联的线
void FArmyRegionOperation::SearchRelativeSplitLines(TSet<int> &Lines, TSharedPtr<FArmySplitLine> SplitLine, TArray<TSharedPtr<FArmySplitLine>>&  LinesArray)
{
	auto beg = SplitLine->LineList[0]->GetStart();
	auto end = SplitLine->LineList.Last()->GetEnd();
	int index = 0;
	for (auto &ref : LinesArray)
	{
		if (ref.Get() == SplitLine.Get()) {
			index++;
			continue;
		}
		auto arrayBeg = ref->LineList[0]->GetStart();
		auto arrayEnd = ref->LineList.Last()->GetEnd();
		bool isOnline = false;
		for (auto line : SplitLine->LineList)
		{
			if ((FArmyMath::IsPointOnLine(arrayBeg, line->GetStart(), line->GetEnd()))
				|| (FArmyMath::IsPointOnLine(arrayEnd, line->GetStart(), line->GetEnd())))
				isOnline = true;
		}
		//线条的两端,不会认为是依赖关系
		if (arrayBeg == beg || arrayEnd == end|| arrayBeg== end|| arrayEnd==beg)
			isOnline = false;
		if (!isOnline) {
			index++;
			continue;
		}
		if (!Lines.Find(index))
		{	
			Lines.Add(index);
			SearchRelativeSplitLines(Lines, LinesArray[index], LinesArray);
		}
		index++;
	}

}

//刷新指定区域
void FArmyRegionOperation::RealTimeFlushRegionList(TSharedPtr<FArmyRoom> WhichRoom)
{
	//  缓存此房间下初始区域
	TArray<TSharedPtr<FArmyRegion>> LastRegionList = SearchRegionWithSameSpaceId(WhichRoom->GetUniqueID().ToString());

	//  获取此房间下所有的线段
	TArray<TSharedPtr<FArmySplitLine>> SplitLines = SearchSplitLinesWithSameSpaceIdOrderByIndex(WhichRoom->GetUniqueID().ToString());

	//新的缓存区域
	TArray<TSharedPtr<FArmyRegion>>  NewCacheRegion;
	for (int i = 0; i < SplitLines.Num(); ++i)
	{
		//第一个从房间生成
		if (i == 0)
		{
			ProduceCacheRegionByRoom(WhichRoom, NewCacheRegion, SplitLines[i]);
		}	 
		//从区域中生成
		else
		{
			ProduceCacheRegionByRegionList(NewCacheRegion, SplitLines[i]);
		}
	}
	//区域比较
	CompareSignature(LastRegionList, NewCacheRegion);
	
	//移除房间相关区域
	RemoveAllRegionInRoom(LastRegionList);

	//关联分割线
	RelativeSplitLinesWithRegions(NewCacheRegion, SplitLines);

	//添加新的区域
	for (int i = 0; i < NewCacheRegion.Num(); ++i)
	{
		XRArgument Arg = XRArgument()
			.ArgFName(FName(*FString(TEXT("基础类"))))
			.ArgString(NewCacheRegion[i]->GetName())
			.ArgUint32(E_LayoutModel);
		FArmySceneData::Get()->Add(NewCacheRegion[i], Arg);
	}
}

void FArmyRegionOperation::RemoveAllRegionInRoom(const TArray<TSharedPtr<FArmyRegion>> &RegionList)
{
	for (TSharedPtr<FArmyRegion> Region : RegionList)
	{
		FArmySceneData::Get()->Delete(Region);
	}
}

//从房间生成缓存区域
void FArmyRegionOperation::ProduceCacheRegionByRoom(TSharedPtr<FArmyRoom> WhichRoom, TArray<TSharedPtr<FArmyRegion>> &RegionList, TSharedPtr<FArmySplitLine> Lines)
{
	//如果是分割线,最小两个端点
	auto PointBeg = Lines->LineList[0]->GetStart();
	auto PointEnd = Lines->LineList.Last()->GetEnd();

	TArray<TPair<FArmyLine, bool>>  Wall;
	//提取一个区域的墙体
	FillRegionLinesToArray(Wall, WhichRoom);
	//查询被分开的墙段
	int  m = -1;
	for (int i = 0; i < Wall.Num(); ++i)
	{
		if (FArmyMath::IsPointOnLine(PointBeg, Wall[i].Key.GetStart(), Wall[i].Key.GetEnd()))
			m = i;
	}
	if (-1 == m)
		return;
	//如果在点和线的端点重合,不需要切开线,这里有9各种情况需要分别讨论
	if (Wall[m].Key.GetStart() != PointBeg&& Wall[m].Key.GetEnd() != PointBeg)
	{
		FArmyLine Line1 = FArmyLine(Wall[m].Key.GetStart(), PointBeg);
		FArmyLine Line2 = FArmyLine(Wall[m].Key.GetEnd(), PointBeg);
		auto remove = Wall[m];
		Wall.RemoveAt(m);

		Wall.Add(TPair<FArmyLine, bool>(Line1, remove.Value));
		Wall.Add(TPair<FArmyLine, bool>(Line2, remove.Value));
	}
	m = -1;
	for (int i = 0; i < Wall.Num(); ++i)
	{
		if (FArmyMath::IsPointOnLine(PointEnd, Wall[i].Key.GetStart(), Wall[i].Key.GetEnd()))
			m = i;
	}
	if (-1 == m)
		return;
	if (Wall[m].Key.GetStart() != PointEnd&& Wall[m].Key.GetEnd() != PointEnd)
	{
		FArmyLine Line1 = FArmyLine(Wall[m].Key.GetStart(), PointEnd);
		FArmyLine Line2 = FArmyLine(Wall[m].Key.GetEnd(), PointEnd);
		auto remove = Wall[m];
		Wall.RemoveAt(m);

		Wall.Add(TPair<FArmyLine, bool>(Line1, remove.Value));
		Wall.Add(TPair<FArmyLine, bool>(Line2, remove.Value));
	}
	//从start 出发,第一个共有点的线段

	auto tempbeg = PointBeg;
	auto tempEnd = PointEnd;
	TSet<TPair<FArmyLine, bool>*>  PointerSet;
	while (tempbeg != tempEnd)
	{
		for (int i = 0; i < Wall.Num(); ++i)
		{
			if (PointerSet.Find(&Wall[i]) != nullptr)
				continue;
	
			if (Wall[i].Key.GetStart() == tempbeg)
			{
				tempbeg = Wall[i].Key.GetEnd();
				PointerSet.Add(&Wall[i]);
				break;
			}
			if (Wall[i].Key.GetEnd() == tempbeg)
			{
				tempbeg = Wall[i].Key.GetStart();
				PointerSet.Add(&Wall[i]);
				break;
			}
		}
	}

	//生成新的区域  生成第一个区域
	TSharedPtr<FArmyRegion>  region_one = MakeShareable(new FArmyRegion());
	region_one->SpaceObjectId = WhichRoom->GetUniqueID().ToString();
	//连个区域只有一个在Set容器中
	for (auto wall : PointerSet)
	{
		if (wall->Value)
		{
			auto line = new FArmySplitLine();
			region_one->VirtualLineList.Add(MakeShareable(new FArmyLine(wall->Key.GetStart(), wall->Key.GetEnd())));
		}
		else
		{
			region_one->WallList.Add(MakeShareable(new FArmyWallLine(wall->Key.GetStart(), wall->Key.GetEnd())));
		}
	}

	//生成第二个区域
	TSharedPtr<FArmyRegion>  region_two = MakeShareable(new FArmyRegion());
	region_two->SpaceObjectId = WhichRoom->GetUniqueID().ToString();
	for (int i = 0; i < Wall.Num(); ++i)
	{
		//这个区域不再Set中,所以Continue跳过操作
		if (PointerSet.Find(&Wall[i]))
			continue;
		if (Wall[i].Value) {
			auto line = new FArmySplitLine();
			region_two->VirtualLineList.Add(MakeShareable(new FArmyLine(Wall[i].Key.GetStart(), Wall[i].Key.GetEnd())));
		}
		else
		{
			region_two->WallList.Add(MakeShareable(new FArmyWallLine(Wall[i].Key.GetStart(), Wall[i].Key.GetEnd())));
		}
	}
	//添加分割线
	for (int i = 0; i < Lines->LineList.Num(); ++i)
	{
		region_two->VirtualLineList.Add(MakeShareable(new FArmyLine(*Lines->LineList[i])));
		region_one->VirtualLineList.Add(MakeShareable(new FArmyLine(*Lines->LineList[i])));
	}
	
	// 生成闭合的点
	if (region_one->CreatePolygonPoints())
	{
		RegionList.Add(region_one);
	}

	if (region_two->CreatePolygonPoints())
	{
		RegionList.Add(region_two);
	}
	
}

//从区域中生成缓存区域
void FArmyRegionOperation::ProduceCacheRegionByRegionList(TArray<TSharedPtr<FArmyRegion>> &RegionList, TSharedPtr<FArmySplitLine> Lines)
{
	 //获取索引
	 int index =   SearchSplitLinesRegion(RegionList, Lines);
	 //如果是分割线,最小两个端点
	 auto PointBeg = Lines->LineList[0]->GetStart();
	 auto PointEnd = Lines->LineList.Last()->GetEnd();


	 TArray<TPair<FArmyLine, bool>>  Wall;
	 //提取一个区域的墙体
	 FillRegionLinesToArray(Wall, RegionList[index]);
	 //查询被分开的墙段
	 int  m = -1;
	 for (int i = 0; i < Wall.Num(); ++i)
	 {
		 if (FArmyMath::IsPointOnLine(PointBeg, Wall[i].Key.GetStart(), Wall[i].Key.GetEnd()))
			 m = i;
	 }
	 if (-1 == m)
		 return;
	 //如果在点和线的端点重合,不需要切开线,这里有9各种情况需要分别讨论
	 if (Wall[m].Key.GetStart() != PointBeg&& Wall[m].Key.GetEnd() != PointBeg)
	 {
		 FArmyLine Line1 = FArmyLine(Wall[m].Key.GetStart(), PointBeg);
		 FArmyLine Line2 = FArmyLine(Wall[m].Key.GetEnd(), PointBeg);
		 auto Cache = Wall[m];
		 Wall.RemoveAt(m);
		 Wall.Add(TPair<FArmyLine, bool>(Line1, Cache.Value));
		 Wall.Add(TPair<FArmyLine, bool>(Line2, Cache.Value));
	 }
	 m = -1;
	 for (int i = 0; i < Wall.Num(); ++i)
	 {
		 if (FArmyMath::IsPointOnLine(PointEnd, Wall[i].Key.GetStart(), Wall[i].Key.GetEnd()))
			 m = i;
	 }
	 if (-1 == m)
		 return;
	 if (Wall[m].Key.GetStart() != PointEnd&& Wall[m].Key.GetEnd() != PointEnd)
	 {
		 FArmyLine Line1 = FArmyLine(Wall[m].Key.GetStart(), PointEnd);
		 FArmyLine Line2 = FArmyLine(Wall[m].Key.GetEnd(), PointEnd);
		 auto Cache = Wall[m];
		 Wall.RemoveAt(m);
		 Wall.Add(TPair<FArmyLine, bool>(Line1, Cache.Value));
		 Wall.Add(TPair<FArmyLine, bool>(Line2, Cache.Value));
	 }
	 //从start 出发,第一个共有点的线段

	 auto tempbeg = PointBeg;
	 auto tempEnd = PointEnd;
	 TSet<TPair<FArmyLine, bool>*>  PointerSet;
	 while (tempbeg != tempEnd)
	 {
		 for (int i = 0; i < Wall.Num(); ++i)
		 {
			 if (PointerSet.Find(&Wall[i]) != nullptr)
				 continue;
			 if (Wall[i].Key.GetStart() == tempbeg)
			 {
				 tempbeg = Wall[i].Key.GetEnd();
				 PointerSet.Add(&Wall[i]);
				 break;
			 }
			 if (Wall[i].Key.GetEnd() == tempbeg)
			 {
				 tempbeg = Wall[i].Key.GetStart();
				 PointerSet.Add(&Wall[i]);
				 break;
			 }
		 }
	 }

	 //生成新的区域  生成第一个区域
	 TSharedPtr<FArmyRegion>  region_one = MakeShareable(new FArmyRegion());
	 region_one->SpaceObjectId = RegionList[index]->SpaceObjectId;
	 for (auto wall : PointerSet)
	 {
		 if (wall->Value) {
			 region_one->VirtualLineList.Add(MakeShareable(new FArmyLine(wall->Key.GetStart(), wall->Key.GetEnd())));
		 }
		 else
		 {
			 region_one->WallList.Add(MakeShareable(new FArmyWallLine(wall->Key.GetStart(), wall->Key.GetEnd())));
		 }
	 }
	 //生成第二个区域
	 TSharedPtr<FArmyRegion>  region_two = MakeShareable(new FArmyRegion());
	 region_two->SpaceObjectId = RegionList[index]->SpaceObjectId;
	 for (int i = 0; i < Wall.Num(); ++i)
	 {
		 if (PointerSet.Find(&Wall[i]))
			 continue;
		 if (Wall[i].Value) {
			 auto line = new FArmySplitLine();
			 region_two->VirtualLineList.Add(MakeShareable(new FArmyLine(Wall[i].Key.GetStart(), Wall[i].Key.GetEnd())));
		 }
		 else
		 {
			 region_two->WallList.Add(MakeShareable(new FArmyWallLine(Wall[i].Key.GetStart(), Wall[i].Key.GetEnd())));
		 }
	 }
	 //添加分割线
	 for (int i = 0; i < Lines->LineList.Num(); ++i)
	 {
		 region_two->VirtualLineList.Add(MakeShareable(new FArmyLine(*Lines->LineList[i])));
		 region_one->VirtualLineList.Add(MakeShareable(new FArmyLine(*Lines->LineList[i])));
	 }
	 //继承父辈的分割线
	 RegionList.RemoveAt(index);

	 // 生成闭合的点
	 
	 if (region_one->CreatePolygonPoints())
	{
		 RegionList.Add(region_one);
	}
 
	 if (region_two->CreatePolygonPoints())
	 {
		 RegionList.Add(region_two);
	 }
}

int FArmyRegionOperation::SearchSplitLinesRegion(TArray<TSharedPtr<FArmyRegion>> &RegionList, TSharedPtr<FArmySplitLine> Lines)
{
	int index = 0;
	for (int i = 0; i < RegionList.Num(); ++i)
	{
		bool isOnRegion = true;
		for (int j = 0; j< Lines->LineList.Num(); ++j)
		{
			if(!IsPointInRegion(Lines->LineList[j]->GetStart(), RegionList[i]))
				isOnRegion = false;
			if (!IsPointInRegion(Lines->LineList[j]->GetEnd(), RegionList[i]))
				isOnRegion = false;
			if (!IsPointInRegion((Lines->LineList[j]->GetStart()+ Lines->LineList[j]->GetEnd())/2, RegionList[i]))
				isOnRegion = false;
		}
		if (isOnRegion) {
			index = i;
			break;
		}
	}
	return index;
}

//比较两个区域的,布置名称
void FArmyRegionOperation::CompareSignature(TArray<TSharedPtr<FArmyRegion>> &Src, TArray<TSharedPtr<FArmyRegion>> &Dest)
{
	for (int i = 0; i < Dest.Num(); ++i)
	{
		for (int j = 0; j < Src.Num(); ++j)
		{
			if (IsSameRegionPrecisely(Dest[i], Src[j])) {
				Dest[i]->RegionName = Src[j]->RegionName;
				Dest[i]->RegionId = Src[j]->RegionId;
			}
		}
	}
}

// 是否相同区域
bool FArmyRegionOperation::IsSameRegion(TSharedPtr<FArmyRegion>  &Src, TSharedPtr<FArmyRegion> &Dest)
{
	if (Src->WallList.Num() != Dest->WallList.Num())
		return false;
	if (Src->VirtualLineList.Num() != Dest->VirtualLineList.Num())
		return false;
	for (int i = 0; i < Src->WallList.Num(); ++i)
	{
		if (Src->WallList[i]->GetCoreLine()->GetStart() != Dest->WallList[i]->GetCoreLine()->GetStart())
			return false;
		if (Src->WallList[i]->GetCoreLine()->GetEnd() != Dest->WallList[i]->GetCoreLine()->GetEnd())
			return false;
	}
	for (int i = 0; i < Src->VirtualLineList.Num(); ++i)
	{
		if (Src->VirtualLineList[i]->GetStart() != Dest->VirtualLineList[i]->GetStart())
			return false;
		if (Src->VirtualLineList[i]->GetEnd() != Dest->VirtualLineList[i]->GetEnd())
			return false;
	}
	return true;
}

//是否相同区域判断
bool FArmyRegionOperation::IsSameRegionPrecisely(TSharedPtr<FArmyRegion>  &Src, TSharedPtr<FArmyRegion> &Dest)
{
	if (Src->WallList.Num() != Dest->WallList.Num())
		return false;
	if (Src->VirtualLineList.Num() != Dest->VirtualLineList.Num())
		return false;


	TSet<FString>	RegionSrc;
	TSet<FString>	RegionDest;
	for (int i = 0; i < Src->WallList.Num(); ++i) {
		auto coreLine = Src->WallList[i]->GetCoreLine();
		RegionSrc.Add(coreLine->GetStart().ToString());
		RegionSrc.Add(coreLine->GetEnd().ToString());
	}
	for (int i = 0; i < Src->VirtualLineList.Num(); ++i) {
		auto coreVirtual = Src->VirtualLineList[i];
		RegionSrc.Add(coreVirtual->GetStart().ToString());
		RegionSrc.Add(coreVirtual->GetEnd().ToString());
	}

	for (int i = 0; i < Dest->WallList.Num(); ++i) {
		auto coreLine = Dest->WallList[i]->GetCoreLine();
		RegionDest.Add(coreLine->GetStart().ToString());
		RegionDest.Add(coreLine->GetEnd().ToString());
	}
	for (int i = 0; i < Dest->VirtualLineList.Num(); ++i) {
		auto coreVirtual = Dest->VirtualLineList[i];
		RegionDest.Add(coreVirtual->GetStart().ToString());
		RegionDest.Add(coreVirtual->GetEnd().ToString());
	}
	if (RegionSrc.Num() != RegionDest.Num())
		return false;
	for (auto &SrcRef : RegionSrc)
	{
		if (!RegionDest.Find(SrcRef))
			return false;
	}
	return true;
}

// 更新缓冲区数据
void FArmyRegionOperation::UpdateRegionList(FString id)
{
	TArray<TWeakPtr<FArmyObject>>	  RoomList;
	TSharedPtr<FArmyRoom> TempRoom;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
	for (auto room : RoomList)
	{
		TSharedPtr<FArmyRoom>	 pRoom = StaticCastSharedPtr<FArmyRoom>(room.Pin());
		if (pRoom->GetUniqueID().ToString() == id)
			TempRoom = pRoom;
	}
	if (TempRoom.IsValid())
		RealTimeFlushRegionList(TempRoom);
}

//处理区域与分割线的关系
void FArmyRegionOperation::RelativeSplitLinesWithRegions(TArray<TSharedPtr<FArmyRegion>>&Regions, TArray<TSharedPtr<FArmySplitLine>>&SplitLines)
{

	for (auto &region : Regions)
	{
		for (auto &splitLine : SplitLines)
		{
			bool isdependentSplitLine = false;
			for (auto &lineSeg : splitLine->LineList)
			{
				for (auto &virtualLine : region->VirtualLineList)
				{
					auto centre = (virtualLine->GetStart() + virtualLine->GetEnd()) / 2;
					if (FArmyMath::IsPointOnLine(centre, lineSeg->GetStart(), lineSeg->GetEnd())
						&&FArmyMath::AreLinesParallel(virtualLine->GetStart(), virtualLine->GetEnd(), lineSeg->GetStart(), lineSeg->GetEnd()))
						isdependentSplitLine = true;
				}
			}
			if(isdependentSplitLine)
				region->SplitLineList.Add(splitLine);
		}
	}
}

 