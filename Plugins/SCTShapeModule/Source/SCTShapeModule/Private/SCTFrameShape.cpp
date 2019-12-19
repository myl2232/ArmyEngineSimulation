#include "SCTFrameShape.h"
#include "SCTBoardShape.h"
#include "SCTSpaceShape.h"
#include "SCTShapeData.h"
#include "SCTOutline.h"
#include "SCTShapeActor.h"
#include "SCTShapeBoxActor.h"
#include "SCTCabinetShape.h"
#include "SCTShapeManager.h"
#include "SCTDoorGroup.h"

FFrameShape::FFrameShape()
{
	SetShapeType(ST_Frame);
	SetShapeName(TEXT("柜体外框"));
}

FFrameShape::~FFrameShape()
{
	FrameBoardInfos.Empty();
}

void FFrameShape::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//解析型录基本信息
	FSCTShape::ParseFromJson(InJsonObject);

	//2、框体板件及板件添加顺序
	const TArray<TSharedPtr<FJsonValue>>* BoardInfoList = nullptr;
	InJsonObject->TryGetArrayField(TEXT("frameBoardInfos"), BoardInfoList);
	const TArray<TSharedPtr<FJsonValue>>* ChildList = nullptr;
	InJsonObject->TryGetArrayField(TEXT("children"), ChildList);
	if (BoardInfoList != nullptr && ChildList != nullptr)
	{
		check((*BoardInfoList).Num() == (*ChildList).Num());
		for (int32 i = 0; i < (*BoardInfoList).Num(); ++i)
		{
			TSharedPtr<FJsonObject> BoardInfoObject = (*BoardInfoList)[i]->AsObject();
			FrameBoardInfo NewBoardInfo;
			NewBoardInfo.BoardType = BoardInfoObject->GetIntegerField(TEXT("boardType"));
			NewBoardInfo.BoardIndex = BoardInfoObject->GetIntegerField(TEXT("boardIndex"));
			TSharedPtr<FJsonObject> ChildObject = (*ChildList)[i]->AsObject();
			int32 ShapeType = ChildObject->GetNumberField(TEXT("type"));
			int64 ShapeId = ChildObject->GetNumberField(TEXT("id"));
			TSharedPtr<FSCTShape> CurBoardShape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ShapeType, ShapeId);
			TSharedPtr<FSCTShape> NewCopyShape = FSCTShapeManager::Get()->CopyShapeToNew(CurBoardShape);
			NewCopyShape->ParseAttributesFromJson(ChildObject);
			check(NewCopyShape->GetShapeType() == ST_Board);
			NewBoardInfo.FrameBoardData = StaticCastSharedPtr<FBoardShape>(NewCopyShape);

			AddFrameBoardInfo(NewBoardInfo);
		}
	}
}

void FFrameShape::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//型录类型和Id
	int32 CurrentType = InJsonObject->GetNumberField(TEXT("type"));
	int32 CurrentId = InJsonObject->GetNumberField(TEXT("id"));
	check(CurrentType == ShapeType && CurrentId == ShapeId);

	//更新型录基本信息
	FSCTShape::ParseAttributesFromJson(InJsonObject);

	//2、框体板件及板件添加顺序
	const TArray<TSharedPtr<FJsonValue>>* BoardInfoList = nullptr;
	InJsonObject->TryGetArrayField(TEXT("frameBoardInfos"), BoardInfoList);
	const TArray<TSharedPtr<FJsonValue>>* ChildList = nullptr;
	InJsonObject->TryGetArrayField(TEXT("children"), ChildList);
	if (BoardInfoList != nullptr && ChildList != nullptr)
	{
		check((*BoardInfoList).Num() == (*ChildList).Num());
		for (int32 i = 0; i < (*BoardInfoList).Num(); ++i)
		{
			TSharedPtr<FJsonObject> BoardInfoObject = (*BoardInfoList)[i]->AsObject();
			FrameBoardInfo NewBoardInfo;
			NewBoardInfo.BoardType = BoardInfoObject->GetIntegerField(TEXT("boardType"));
			NewBoardInfo.BoardIndex = BoardInfoObject->GetIntegerField(TEXT("boardIndex"));
			TSharedPtr<FJsonObject> ChildObject = (*ChildList)[i]->AsObject();
			int32 ShapeType = ChildObject->GetNumberField(TEXT("type"));
			int64 ShapeId = ChildObject->GetNumberField(TEXT("id"));
			TSharedPtr<FSCTShape> CurBoardShape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ShapeType, ShapeId);
			TSharedPtr<FSCTShape> NewCopyShape = FSCTShapeManager::Get()->CopyShapeToNew(CurBoardShape);
			NewCopyShape->ParseAttributesFromJson(ChildObject);
			check(NewCopyShape->GetShapeType() == ST_Board);
			NewBoardInfo.FrameBoardData = StaticCastSharedPtr<FBoardShape>(NewCopyShape);

			AddFrameBoardInfo(NewBoardInfo);
		}
	}
}

void FFrameShape::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseShapeFromJson(InJsonObject);
}

void FFrameShape::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseContentFromJson(InJsonObject);
}

void FFrameShape::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//1、型录基本信息
	FSCTShape::SaveAttriToJson(JsonWriter);

	//2、板件添加顺序
	JsonWriter->WriteArrayStart(TEXT("frameBoardInfos"));
	for (auto it : FrameBoardInfos)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue(TEXT("boardType"), it.BoardType);
		JsonWriter->WriteValue(TEXT("boardIndex"), it.BoardIndex);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//2、框体板件
	JsonWriter->WriteArrayStart(TEXT("children"));
	for (auto it : FrameBoardInfos)
	{
		JsonWriter->WriteObjectStart();
		it.FrameBoardData->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//End children
	JsonWriter->WriteArrayEnd();

	//3、保存Shapes信息
	//统计当前型录所有层级的子型录
	FSCTShapeManager::Get()->CountInheritChildShapes(this);
	JsonWriter->WriteArrayStart(TEXT("shapes"));
	for (int32 i = ST_None; i < ST_Max; ++i)
	{
		TMap<int64, TSharedPtr<FSCTShape>>& CurTypeChildShapes = FSCTShapeManager::Get()->GetChildrenShapesByType(i);
		for (auto it = CurTypeChildShapes.CreateIterator(); it; ++it)
		{
			TSharedPtr<FSCTShape> CurShape = it->Value;
			JsonWriter->WriteObjectStart();
			CurShape->SaveShapeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
	}
	//End Shapes
	JsonWriter->WriteArrayEnd();
	//清空当前型录所有层级的子型录缓存
	FSCTShapeManager::Get()->ClearAllChildrenShapes();
}

void FFrameShape::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//1、型录基本信息
	FSCTShape::SaveAttriToJson(JsonWriter);

	//2、板件添加顺序
	JsonWriter->WriteArrayStart(TEXT("frameBoardInfos"));
	for (auto it : FrameBoardInfos)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue(TEXT("boardType"), it.BoardType);
		JsonWriter->WriteValue(TEXT("boardIndex"), it.BoardIndex);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//2、框体板件(注意：需要按照板件添加顺序进行保存)
	JsonWriter->WriteArrayStart(TEXT("children"));
	for (auto it : FrameBoardInfos)
	{
		JsonWriter->WriteObjectStart();
		it.FrameBoardData->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//End children
	JsonWriter->WriteArrayEnd();
}

void FFrameShape::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveShapeToJson(JsonWriter);
}

void FFrameShape::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveContentToJson(JsonWriter);
}

void FFrameShape::CopyTo(FSCTShape* OutShape)
{
	//基类信息拷贝
	FSCTShape::CopyTo(OutShape);

	//柜体框信息拷贝
	FFrameShape* OutOutlineShape = StaticCast<FFrameShape*>(OutShape);

	//所属的柜体空间

	//框体板件添加顺序和板件
	for (auto it : FrameBoardInfos)
	{
		TSharedPtr<FBoardShape> CurBoardShape = it.FrameBoardData;
		FBoardShape* NewBoardShape = new FBoardShape();
		CurBoardShape->CopyTo(NewBoardShape);
		FrameBoardInfo NewBoardInfo;
		NewBoardInfo.BoardType = it.BoardType;
		NewBoardInfo.BoardIndex = it.BoardIndex;
		NewBoardInfo.FrameBoardData = MakeShareable(NewBoardShape);
		OutOutlineShape->AddFrameBoardInfo(NewBoardInfo);
	}
}

ASCTShapeActor* FFrameShape::SpawnShapeActor()
{
	ASCTShapeActor* NewFrameShapeActor = FSCTShape::SpawnShapeActor();
	//框体板件
	for (auto it : FrameBoardInfos)
	{
		TSharedPtr<FBoardShape> CurBoardData = it.FrameBoardData;
		ASCTShapeActor* NewBoardShapeActor = CurBoardData->SpawnShapeActor();
		NewBoardShapeActor->AttachToActorOverride(NewFrameShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	return NewFrameShapeActor;
}

void FFrameShape::SpawnActorsForSelected(FName InProfileName)
{
	for (auto it : FrameBoardInfos)
	{
		TSharedPtr<FBoardShape> CurBoardData = it.FrameBoardData;
		CurBoardData->SpawnActorsForSelected(InProfileName);
	}
}

void FFrameShape::SetCollisionProfileName(FName InProfileName)
{
	for (auto it : FrameBoardInfos)
	{
		TSharedPtr<FBoardShape> CurBoardData = it.FrameBoardData;
		CurBoardData->SetCollisionProfileName(InProfileName);
	}
}

void FFrameShape::GetResourceUrls(TArray<FString> &OutResourceUrls)
{
	for (auto it : FrameBoardInfos)
	{
		TSharedPtr<FBoardShape> CurBoardData = it.FrameBoardData;
		CurBoardData->GetResourceUrls(OutResourceUrls);
	}
}

void FFrameShape::GetFileCachePaths(TArray<FString> &OutFileCachePaths)
{
	for (auto it : FrameBoardInfos)
	{
		TSharedPtr<FBoardShape> CurBoardData = it.FrameBoardData;
		CurBoardData->GetFileCachePaths(OutFileCachePaths);
	}
}

void FFrameShape::HiddenFrameShapeActors(bool bHidden)
{
	for (auto it : FrameBoardInfos)
	{
		TSharedPtr<FBoardShape> CurBoardData = it.FrameBoardData;
		ASCTShapeActor* CurBoardActor = CurBoardData->GetShapeActor();
		if (CurBoardActor)
		{
			CurBoardActor->SetActorHiddenInGame(bHidden);
		}
	}
}

bool FFrameShape::IsFrameBoardEmpty()
{
	return FrameBoardInfos.Num() == 0;
}

void FFrameShape::EmptyFrameBoard()
{
	FrameBoardInfos.Empty();
}

FBoardShape* FFrameShape::AddFrameBoardInfo(const FrameBoardInfo& InFrameBoardInfo)
{
	//设置父子关系
	InFrameBoardInfo.FrameBoardData->SetParentShape(this);
	AddChildShape(InFrameBoardInfo.FrameBoardData);
	FrameBoardInfos.Add(InFrameBoardInfo);
	return InFrameBoardInfo.FrameBoardData.Get();
}

FBoardShape* FFrameShape::AddFrameBoardShape(TSharedPtr<FBoardShape> InBoardShape, int32 InType, int32 InIndex/*, int32 InCount*/)
{
	TSharedPtr<FBoardShape> CurFrameBoard = GetFrameBoardShape(InType, InIndex);
	if (CurFrameBoard.IsValid())
	{
		return nullptr;
	}
	//设置板件位置和尺寸以及轮廓信息
	bool bPosDimen = CalFrameBoardPositionDimension(InBoardShape, InType, InIndex, 0.0);
	//创建板件信息
	FrameBoardInfo NewBoardInfo;
	NewBoardInfo.BoardType = InType;
	NewBoardInfo.BoardIndex = InIndex;
	NewBoardInfo.FrameBoardData = InBoardShape;
	return AddFrameBoardInfo(NewBoardInfo);
}

bool FFrameShape::ChangeFrameBoardShape(TSharedPtr<FBoardShape> InBoardShape, int32 InType, int32 InIndex)
{
	int32 CurIndex = -1;
	for (int32 i=0; i<FrameBoardInfos.Num(); ++i)
	{
		if (FrameBoardInfos[i].BoardType == InType && FrameBoardInfos[i].BoardIndex == InIndex)
		{
			CurIndex = i;
			break;
		}
	}
	if (CurIndex < 0)
	{
		return false;
	}

	//更新基类中的板件
	InBoardShape->SetParentShape(this);
	InsertChildShape(FrameBoardInfos[CurIndex].FrameBoardData.Get(), InBoardShape);
	RemoveChildShape(FrameBoardInfos[CurIndex].FrameBoardData);
	FrameBoardInfos[CurIndex].FrameBoardData = InBoardShape;

	FCabinetShape* CurCabinetShape = SpaceShapeData->GetCabinetShapeData();
	FName BoardProfileName = CurCabinetShape->GetShapeSpaceProfileName();
	//创建板件的Actor
	ASCTShapeActor* NewShapeActor = InBoardShape->SpawnShapeActor();
	InBoardShape->SetCollisionProfileName(BoardProfileName);
	NewShapeActor->AttachToActorOverride(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
	//板件包围框
	ASCTWireframeActor* WireFrameActor = InBoardShape->SpawnWireFrameActor();
	WireFrameActor->UpdateActorDimension();
	WireFrameActor->SetActorHiddenInGame(true);
	WireFrameActor->AttachToActor(NewShapeActor, FAttachmentTransformRules::KeepRelativeTransform);

	//更新所有框体板件
	UpdateAllFrameShapes();
	return true;
}

bool FFrameShape::RemoveFrameBoardShape(FBoardShape* InBoard)
{
	for (int32 i=0; i<FrameBoardInfos.Num(); ++i)
	{
		if (FrameBoardInfos[i].FrameBoardData.Get() == InBoard)
		{
			//基类中删除
			RemoveChildShape(FrameBoardInfos[i].FrameBoardData);
			FrameBoardInfos.RemoveAt(i);
			break;
		}
	}

	//更新其他框体板件
	UpdateAllFrameShapes();
	return true;
}

bool FFrameShape::RemoveFrameBoardShapeByTypeIndex(int32 InType, int32 InIndex)
{
	for (int32 i=0; i<FrameBoardInfos.Num(); ++i)
	{
		if (FrameBoardInfos[i].BoardType == InType && FrameBoardInfos[i].BoardIndex == InIndex)
		{
			//基类中删除
			RemoveChildShape(FrameBoardInfos[i].FrameBoardData);
			FrameBoardInfos.RemoveAt(i);
			break;
		}
	}

	//更新其他框体板件
	UpdateAllFrameShapes();
	return true;
}

void FFrameShape::GetUpDownLeftRightFrameBoardShapes(TArray<FBoardShape*>& OutFrameBoardShapes)
{
	for (auto it : FrameBoardInfos)
	{
		if (it.BoardType < 4)
		{
			OutFrameBoardShapes.Add(it.FrameBoardData.Get());
		}
	}
}

TSharedPtr<FBoardShape> FFrameShape::GetFrameBoardShape(int32 InType, int32 InIndex)
{
	for (auto it : FrameBoardInfos)
	{
		if (it.BoardType == InType && it.BoardIndex == InIndex)
		{
			return it.FrameBoardData;
		}
	}
	return nullptr;
}

void FFrameShape::GetFrameBoardTypeIndex(int32& InOutType, int32& InOutIndex, FBoardShape* InBoard)
{
	for (auto it : FrameBoardInfos)
	{
		if (it.FrameBoardData.Get() == InBoard)
		{
			InOutType = it.BoardType;
			InOutIndex = it.BoardIndex;
			return;
		}
	}
}

bool FFrameShape::CalFrameBoardPositionDimension(TSharedPtr<FBoardShape> InBoardShape, int32 InType, int32 InIndex, float InFrontExtension)
{
	bool bResult = false;
	TSharedPtr<FSCTOutline> Outline = SpaceShapeData->GetSpaceOutline();
	EOutlineType OutlineType = Outline->GetOutlineType();
	switch (OutlineType)
	{
		case OLT_None:
		{
			bResult = CalFrameBoardPosDimenForRectangle(InBoardShape, InType, InIndex);
			break;
		}
		case OLT_LeftGirder:
		{
			bResult = CalFrameBoardPosDimenForLeftGirder(InBoardShape, InType, InIndex);
			break;
		}
		case OLT_RightGirder:
		{
			bResult = CalFrameBoardPosDimenForRightGirder(InBoardShape, InType, InIndex);
			break;
		}
		case OLT_BackGirder:
		{
			bResult = CalFrameBoardPosDimenForBackGirder(InBoardShape, InType, InIndex);
			break;
		}
		case OLT_LeftPiller:
		{
			bResult = CalFrameBoardPosDimenForLeftPiller(InBoardShape, InType, InIndex);
			break;
		}
		case OLT_RightPiller:
		{
			bResult = CalFrameBoardPosDimenForRightPiller(InBoardShape, InType, InIndex);
			break;
		}
		case OLT_MiddlePiller:
		{
			bResult = CalFrameBoardPosDimenForMiddlePiller(InBoardShape, InType, InIndex);
			break;
		}
		default:
			check(false);
			break;
	}

	//修正前向外延值(只需要处理顶、底、左、右侧板件)
	if (InType == 0 || InType == 1 || InType == 2 || InType == 3)
	{
		InBoardShape->SetShapeDepth(InBoardShape->GetShapeDepth() + InFrontExtension);
	}

	return bResult;
}

TTuple<bool, bool, bool, bool> FFrameShape::GetFrontBoardCoverdTopBottomLeftRightBoard()
{
	bool bFrontBoardCoverdTopBaord = false;
	bool bFrontBoardCoverdBottomBoard = false;
	bool bFrontBoardCoverdLeftBoard = false;
	bool bFrontBoardCoverdRightBoard = false;
	do
	{
		TSharedPtr<FBoardShape> FrontBoard = GetFrameBoardShape(5, 0);
		if (FrontBoard.IsValid() == false) break;		
		TSharedPtr<FBoardShape> TopBoard = GetFrameBoardShape(0, 0);
		bFrontBoardCoverdTopBaord = TopBoard.IsValid() && IsBoardInSequence(FrontBoard,TopBoard);
		TSharedPtr<FBoardShape> BottomBoard = GetFrameBoardShape(1, 0);
		bFrontBoardCoverdBottomBoard = BottomBoard.IsValid() && IsBoardInSequence(FrontBoard, BottomBoard);
		TSharedPtr<FBoardShape> LeftBoard = GetFrameBoardShape(2, 0);
		bFrontBoardCoverdLeftBoard = LeftBoard.IsValid() && IsBoardInSequence(FrontBoard, LeftBoard);		
		TSharedPtr<FBoardShape> RightBoard = GetFrameBoardShape(3, 0);		
		bFrontBoardCoverdRightBoard = RightBoard.IsValid() && IsBoardInSequence(FrontBoard, RightBoard);

	} while (false);	
	return MakeTuple(bFrontBoardCoverdTopBaord, bFrontBoardCoverdBottomBoard, bFrontBoardCoverdLeftBoard, bFrontBoardCoverdRightBoard);
}

bool FFrameShape::UpdateAllFrameShapes()
{
	for (auto it : FrameBoardInfos)
	{
		CalFrameBoardPositionDimension(it.FrameBoardData, it.BoardType, it.BoardIndex, it.FrontRetract);
	}
	return true;
}

bool FFrameShape::ModifyFrameBoardRetrack(int32 InType, int32 InIndex, float InValue)
{
	TSharedPtr<FBoardShape> CurrentBoard = GetFrameBoardShape(InType, InIndex);
	if (!CurrentBoard.IsValid())
	{
		return false;
	}
	return CurrentBoard->SetShapeContraction(InValue);
}

bool FFrameShape::ModifyFrameBoardHeight(int32 InType, int32 InIndex, FString InValue)
{
	TSharedPtr<FBoardShape> CurrentBoard = GetFrameBoardShape(InType, InIndex);
	if (!CurrentBoard.IsValid())
	{
		return false;
	}
	return CurrentBoard->SetShapeHeight(InValue);
}

bool FFrameShape::AddBoardFrontExtension(FBoardShape* InBoard, float InValue)
{
	for (int32 i = 0; i < FrameBoardInfos.Num(); ++i)
	{
		if (FrameBoardInfos[i].FrameBoardData.Get() == InBoard)
		{
			//设置外延值
			if (FrameBoardInfos[i].FrontCount == 0)
			{
				FrameBoardInfos[i].FrontRetract = InValue;
			}
			//外延计数加一
			++FrameBoardInfos[i].FrontCount;
			//重新计算板件尺寸
			CalFrameBoardPositionDimension(
				FrameBoardInfos[i].FrameBoardData, 
				FrameBoardInfos[i].BoardType, 
				FrameBoardInfos[i].BoardIndex, 
				FrameBoardInfos[i].FrontRetract);
			break;
		}
	}
	return true;
}

void FFrameShape::RemoveBoardFrontExtension(FBoardShape* InBoard)
{
	for (int32 i=0; i<FrameBoardInfos.Num(); ++i)
	{
		if (FrameBoardInfos[i].FrameBoardData.Get() == InBoard)
		{
			//外延计数减一
			--FrameBoardInfos[i].FrontCount;
			//设置外延值
			if (FrameBoardInfos[i].FrontCount == 0)
			{
				FrameBoardInfos[i].FrontRetract = 0.0;
			}
			//重新计算板件尺寸
			CalFrameBoardPositionDimension(
				FrameBoardInfos[i].FrameBoardData, 
				FrameBoardInfos[i].BoardType, 
				FrameBoardInfos[i].BoardIndex, 
				FrameBoardInfos[i].FrontRetract);
			break;
		}
	}
}

bool FFrameShape::AddBoardUpExtension(FBoardShape* InBoard, float InValue)
{
	InBoard->SetShapeTopExpand(InValue);
	return true;
}
void FFrameShape::RemoveBoardUpExtension(FBoardShape* InBoard)
{
	InBoard->SetShapeTopExpand(0.0);
}

bool FFrameShape::CalInsideSpacePosDim(FVector& InsidePosition, FVector& InsideDimention)
{
	//轮廓空间尺寸
	const float SpaceWidth = SpaceShapeData->GetShapeWidth();
	const float SpaceDepth = SpaceShapeData->GetShapeDepth();
	const float SpaceHeight = SpaceShapeData->GetShapeHeight();

	//框体板件厚度
	float TopHeight = 0.0;
	float BottomHeight = 0.0;
	float LeftHeight = 0.0;
	float RightHeight = 0.0;
	float BackHeight = 0.0;
	float FrontHeight = 0.0;
	//框体板件内缩值
	float TopRetract = 0.0;
	float BottomRetract = 0.0;
	float LeftRetract = 0.0;
	float RightRetract = 0.0;
	float BackRetract = 0.0;
	float FrontRetract = 0.0;

	TSharedPtr<FBoardShape> TopBoard = GetFrameBoardShape(0, 0);
	if (TopBoard.IsValid())
	{
		TopHeight = TopBoard->GetShapeHeight();
		TopRetract = TopBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> BottomBoard = GetFrameBoardShape(1, 0);
	if (BottomBoard.IsValid())
	{
		BottomHeight = BottomBoard->GetShapeHeight();
		BottomRetract = BottomBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> LeftBoard = GetFrameBoardShape(2, 0);
	if (LeftBoard.IsValid())
	{
		LeftHeight = LeftBoard->GetShapeHeight();
		LeftRetract = LeftBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> RightBoard = GetFrameBoardShape(3, 0);
	if (RightBoard.IsValid())
	{
		RightHeight = RightBoard->GetShapeHeight();
		RightRetract = RightBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> BackBoard = GetFrameBoardShape(4, 0);
	if (BackBoard.IsValid())
	{
		BackHeight = BackBoard->GetShapeHeight();
		BackRetract = BackBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> FrontBoard = GetFrameBoardShape(5, 0);
	if (FrontBoard.IsValid())
	{
		FrontHeight = FrontBoard->GetShapeHeight();
		FrontRetract = FrontBoard->GetShapeContraction()
			+ FrontBoard->GetShapeConfluenceContraction()
			+ GetSpaceConfluenceCoverdBoardValue();
	}

	//计算内部空间的位置和尺寸
	InsidePosition.X = LeftHeight + LeftRetract;
	InsidePosition.Y = BackHeight + BackRetract;
	InsidePosition.Z = BottomHeight + BottomRetract;

	InsideDimention.X = SpaceWidth - LeftHeight - LeftRetract - RightHeight - RightRetract;
	InsideDimention.Y = SpaceDepth - BackHeight - BackRetract - FrontHeight - FrontRetract;
	InsideDimention.Z = SpaceHeight - BottomHeight - BottomRetract - TopHeight - TopRetract;

	return true;
}

bool FFrameShape::CalInsideOutlineParm(TSharedPtr<FSpaceShape> InsideSpace)
{
	bool bResult = false;
	TSharedPtr<FSCTOutline> InsideOutline = InsideSpace->GetSpaceOutline();
	TSharedPtr<FSCTOutline> Outline = SpaceShapeData->GetSpaceOutline();
	EOutlineType OutlineType = Outline->GetOutlineType();
	if (InsideOutline->GetOutlineType() != OutlineType)
	{
		check(false);
		return false;
	}

	switch (OutlineType)
	{
	case OLT_None:
	{
		bResult = true;
		break;
	}
	case OLT_LeftGirder:
	{
		//梁宽梁高
		float GWidth = Outline->GetOutlineParamValueByRefName(TEXT("GW"));
		float GHeight = Outline->GetOutlineParamValueByRefName(TEXT("GH"));

		//板件厚度
		float LeftHeight0 = 0.0;
		float LeftHeight1 = 0.0;
		float TopHeight0 = 0.0;
		float TopHeight1 = 0.0;
		//板件内移
		float LeftRetract0 = 0.0;
		float LeftRetract1 = 0.0;
		float TopRetract0 = 0.0;
		float TopRetract1 = 0.0;

		//计算板件厚度和内移值
		TSharedPtr<FBoardShape> LeftBoard0 = GetFrameBoardShape(2, 0);
		if (LeftBoard0.IsValid())
		{
			LeftHeight0 = LeftBoard0->GetShapeHeight();
			LeftRetract0 = LeftBoard0->GetShapeContraction();
		}
		TSharedPtr<FBoardShape> LeftBoard1 = GetFrameBoardShape(2, 1);
		if (LeftBoard1.IsValid())
		{
			LeftHeight1 = LeftBoard1->GetShapeHeight();
			LeftRetract1 = LeftBoard1->GetShapeContraction();
		}
		TSharedPtr<FBoardShape> TopBoard0 = GetFrameBoardShape(0, 0);
		if (TopBoard0.IsValid())
		{
			TopHeight0 = TopBoard0->GetShapeHeight();
			TopRetract0 = TopBoard0->GetShapeContraction();
		}
		TSharedPtr<FBoardShape> TopBoard1 = GetFrameBoardShape(0, 1);
		if (TopBoard1.IsValid())
		{
			TopHeight1 = TopBoard1->GetShapeHeight();
			TopRetract1 = TopBoard1->GetShapeContraction();
		}

		//计算轮廓参数
		bResult = InsideSpace->SetOutlineParamValueByRefName(TEXT("GW"), GWidth - LeftHeight0 - LeftRetract0 + LeftHeight1 + LeftRetract1);
		bResult = InsideSpace->SetOutlineParamValueByRefName(TEXT("GH"), GHeight - TopHeight0 - TopRetract0 + TopHeight1 + TopRetract1);
		break;
	}
	case OLT_RightGirder:
	{
		//梁宽梁高
		float GWidth = Outline->GetOutlineParamValueByRefName(TEXT("GW"));
		float GHeight = Outline->GetOutlineParamValueByRefName(TEXT("GH"));

		//板件厚度
		float RightHeight0 = 0.0;
		float RightHeight1 = 0.0;
		float TopHeight0 = 0.0;
		float TopHeight1 = 0.0;
		//板件内移值
		float RightRetract0 = 0.0;
		float RightRetract1 = 0.0;
		float TopRetract0 = 0.0;
		float TopRetract1 = 0.0;

		//计算板件厚度和内移值
		TSharedPtr<FBoardShape> RightBoard0 = GetFrameBoardShape(3, 0);
		if (RightBoard0.IsValid())
		{
			RightHeight0 = RightBoard0->GetShapeHeight();
			RightRetract0 = RightBoard0->GetShapeContraction();
		}
		TSharedPtr<FBoardShape> RightBoard1 = GetFrameBoardShape(3, 1);
		if (RightBoard1.IsValid())
		{
			RightHeight1 = RightBoard1->GetShapeHeight();
			RightRetract1 = RightBoard1->GetShapeContraction();
		}
		TSharedPtr<FBoardShape> TopBoard0 = GetFrameBoardShape(0, 0);
		if (TopBoard0.IsValid())
		{
			TopHeight0 = TopBoard0->GetShapeHeight();
			TopRetract0 = TopBoard0->GetShapeContraction();
		}
		TSharedPtr<FBoardShape> TopBoard1 = GetFrameBoardShape(0, 1);
		if (TopBoard1.IsValid())
		{
			TopHeight1 = TopBoard1->GetShapeHeight();
			TopRetract1 = TopBoard1->GetShapeContraction();
		}

		//计算轮廓参数
		bResult = InsideSpace->SetOutlineParamValueByRefName(TEXT("GW"), GWidth - RightHeight0 - RightRetract0 + RightHeight1 + RightRetract1);
		bResult = InsideSpace->SetOutlineParamValueByRefName(TEXT("GH"), GHeight - TopHeight0 - TopRetract0 + TopHeight1 + TopRetract1);
		break;
	}
	case OLT_BackGirder:
	{
		//梁高梁深
		float GHeight = Outline->GetOutlineParamValueByRefName(TEXT("GH"));
		float GDepth = Outline->GetOutlineParamValueByRefName(TEXT("GD"));

		//板件厚度
		float TopHeight0 = 0.0;
		float TopHeight1 = 0.0;
		float BackHeight0 = 0.0;
		float BackHeight1 = 0.0;
		//板件内移值
		float TopRetract0 = 0.0;
		float TopRetract1 = 0.0;
		float BackRetract0 = 0.0;
		float BackRetract1 = 0.0;

		//计算板件厚度和内移值
		TSharedPtr<FBoardShape> TopBoard0 = GetFrameBoardShape(0, 0);
		if (TopBoard0.IsValid())
		{
			TopHeight0 = TopBoard0->GetShapeHeight();
			TopRetract0 = TopBoard0->GetShapeContraction();
		}
		TSharedPtr<FBoardShape> TopBoard1 = GetFrameBoardShape(0, 1);
		if (TopBoard1.IsValid())
		{
			TopHeight1 = TopBoard1->GetShapeHeight();
			TopRetract1 = TopBoard1->GetShapeContraction();
		}
		TSharedPtr<FBoardShape> BackBoard0 = GetFrameBoardShape(4, 0);
		if (BackBoard0.IsValid())
		{
			BackHeight0 = BackBoard0->GetShapeHeight();
			BackRetract0 = BackBoard0->GetShapeContraction();
		}
		TSharedPtr<FBoardShape> BackBoard1 = GetFrameBoardShape(4, 1);
		if (BackBoard1.IsValid())
		{
			BackHeight1 = BackBoard1->GetShapeHeight();
			BackRetract1 = BackBoard1->GetShapeContraction();
		}
		bResult = InsideSpace->SetOutlineParamValueByRefName(TEXT("GH"), GHeight - TopHeight0 - TopRetract0 + TopHeight1 + TopRetract1);
		bResult = InsideSpace->SetOutlineParamValueByRefName(TEXT("GD"), GDepth - BackHeight0 - BackRetract0 + BackHeight1 + BackRetract1);
		break;
	}
	case OLT_LeftPiller:
	{
		//柱宽柱深
		float PWidth = Outline->GetOutlineParamValueByRefName(TEXT("PW"));
		float PDepth = Outline->GetOutlineParamValueByRefName(TEXT("PD"));

		//板件厚度
		float LeftHeight0 = 0.0;
		float LeftHeight1 = 0.0;
		float BackHeight0 = 0.0;
		float BackHeight1 = 0.0;
		//板件内移值
		float LeftRetract0 = 0.0;
		float LeftRetract1 = 0.0;
		float BackRetract0 = 0.0;
		float BackRetract1 = 0.0;

		//计算板件厚度和内移值
		TSharedPtr<FBoardShape> LeftBoard0 = GetFrameBoardShape(2, 0);
		if (LeftBoard0.IsValid())
		{
			LeftHeight0 = LeftBoard0->GetShapeHeight();
			LeftRetract0 = LeftBoard0->GetShapeContraction();
		}
		TSharedPtr<FBoardShape> LeftBoard1 = GetFrameBoardShape(2, 1);
		if (LeftBoard1.IsValid())
		{
			LeftHeight1 = LeftBoard1->GetShapeHeight();
			LeftRetract1 = LeftBoard1->GetShapeContraction();
		}
		TSharedPtr<FBoardShape> BackBoard0 = GetFrameBoardShape(0, 0);
		if (BackBoard0.IsValid())
		{
			BackHeight0 = BackBoard0->GetShapeHeight();
			BackRetract0 = BackBoard0->GetShapeContraction();
		}
		TSharedPtr<FBoardShape> BackBoard1 = GetFrameBoardShape(0, 1);
		if (BackBoard1.IsValid())
		{
			BackHeight1 = BackBoard1->GetShapeHeight();
			BackRetract1 = BackBoard1->GetShapeContraction();
		}

		//计算轮廓参数
		bResult = InsideSpace->SetOutlineParamValueByRefName(TEXT("PW"), PWidth - LeftHeight0 - LeftRetract0 + LeftHeight1 + LeftRetract1);
		bResult = InsideSpace->SetOutlineParamValueByRefName(TEXT("PD"), PDepth - BackHeight0 - BackRetract0 + BackHeight1 + BackRetract1);
		break;
	}
	case OLT_RightPiller:
	{
		//柱宽柱深
		float PWidth = Outline->GetOutlineParamValueByRefName(TEXT("PW"));
		float PDepth = Outline->GetOutlineParamValueByRefName(TEXT("PD"));

		//板件厚度
		float RightHeight0 = 0.0;
		float RightHeight1 = 0.0;
		float BackHeight0 = 0.0;
		float BackHeight1 = 0.0;
		//板件内移值
		float RightRetract0 = 0.0;
		float RightRetract1 = 0.0;
		float BackRetract0 = 0.0;
		float BackRetract1 = 0.0;

		//计算板件厚度和内移值
		TSharedPtr<FBoardShape> RightBoard0 = GetFrameBoardShape(3, 0);
		if (RightBoard0.IsValid())
		{
			RightHeight0 = RightBoard0->GetShapeHeight();
			RightRetract0 = RightBoard0->GetShapeContraction();
		}
		TSharedPtr<FBoardShape> RightBoard1 = GetFrameBoardShape(3, 1);
		if (RightBoard1.IsValid())
		{
			RightHeight1 = RightBoard1->GetShapeHeight();
			RightRetract1 = RightBoard1->GetShapeContraction();
		}
		TSharedPtr<FBoardShape> BackBoard0 = GetFrameBoardShape(0, 0);
		if (BackBoard0.IsValid())
		{
			BackHeight0 = BackBoard0->GetShapeHeight();
			BackRetract0 = BackBoard0->GetShapeContraction();
		}
		TSharedPtr<FBoardShape> BackBoard1 = GetFrameBoardShape(0, 1);
		if (BackBoard1.IsValid())
		{
			BackHeight1 = BackBoard1->GetShapeHeight();
			BackRetract1 = BackBoard1->GetShapeContraction();
		}

		//计算轮廓参数
		bResult = InsideSpace->SetOutlineParamValueByRefName(TEXT("PW"), PWidth - RightHeight0 - RightRetract0 + RightHeight1 + RightRetract1);
		bResult = InsideSpace->SetOutlineParamValueByRefName(TEXT("PD"), PDepth - BackHeight0 - BackRetract0 + BackHeight1 + BackRetract1);
		break;
	}
	case OLT_MiddlePiller:
	{
		//TODO: 当前这种类型还有些问题
		float PPosX = Outline->GetOutlineParamValueByRefName(TEXT("PX"));
		float PWidth = Outline->GetOutlineParamValueByRefName(TEXT("PW"));
		float PDepth = Outline->GetOutlineParamValueByRefName(TEXT("PD"));

		//板件厚度
		float LeftHeight1 = 0.0;
		float RightHeight1 = 0.0;
		float BackHeight0 = 0.0;
		float BackHeight1 = 0.0;
		float BackHeight2 = 0.0;
		//板件内移值
		float LeftRetract1 = 0.0;
		float RightRetract1 = 0.0;
		float BackRetract0 = 0.0;
		float BackRetract1 = 0.0;
		float BackRetract2 = 0.0;

		//计算板件厚度和内移值
		TSharedPtr<FBoardShape> LeftBoard1 = GetFrameBoardShape(2, 1);
		if (LeftBoard1.IsValid())
		{
			LeftHeight1 = LeftBoard1->GetShapeHeight();
			LeftRetract1 = LeftBoard1->GetShapeContraction();
		}
		TSharedPtr<FBoardShape> RightBoard1 = GetFrameBoardShape(3, 1);
		if (RightBoard1.IsValid())
		{
			RightHeight1 = RightBoard1->GetShapeHeight();
			RightRetract1 = RightBoard1->GetShapeContraction();
		}
		TSharedPtr<FBoardShape> BackBoard0 = GetFrameBoardShape(4, 0);
		if (BackBoard0.IsValid())
		{
			BackHeight0 = BackBoard0->GetShapeHeight();
			BackRetract0 = BackBoard0->GetShapeContraction();
		}
		TSharedPtr<FBoardShape> BackBoard1 = GetFrameBoardShape(4, 1);
		if (BackBoard1.IsValid())
		{
			BackHeight1 = BackBoard1->GetShapeHeight();
			BackRetract1 = BackBoard1->GetShapeContraction();
		}
		TSharedPtr<FBoardShape> BackBoard2 = GetFrameBoardShape(4, 2);
		if (BackBoard2.IsValid())
		{
			BackHeight2 = BackBoard2->GetShapeHeight();
			BackRetract2 = BackBoard2->GetShapeContraction();
		}

		//计算轮廓参数
		bResult = InsideSpace->SetOutlineParamValueByRefName(TEXT("PX"), PPosX - RightHeight1 - RightRetract1);
		bResult = InsideSpace->SetOutlineParamValueByRefName(TEXT("PW"), PWidth + RightHeight1 + RightRetract1 + LeftHeight1 + LeftRetract1);
		bResult = InsideSpace->SetOutlineParamValueByRefName(TEXT("PD"), PDepth - BackHeight0 - BackRetract0 + BackHeight2 + BackRetract2);
		break;
	}
	default:
		check(false);
		break;
	}

	return bResult;
}

bool FFrameShape::IsBoardInSequence(TSharedPtr<FBoardShape> InBoardShape0, TSharedPtr<FBoardShape> InBoardShape1)
{
	int32 Sequence0 = 100;
	int32 Sequence1 = 100;
	for (int32 i=0; i<FrameBoardInfos.Num(); ++i)
	{
		if (FrameBoardInfos[i].FrameBoardData == InBoardShape0)
		{
			Sequence0 = i;
		}
		if (FrameBoardInfos[i].FrameBoardData == InBoardShape1)
		{
			Sequence1 = i;
		}
		if (Sequence0 + Sequence1 < 100)
		{
			break;
		}
	}
	return Sequence1 > Sequence0;
}

float FFrameShape::GetSpaceConfluenceCoverdBoardValue() const
{
	float RetValue = 0.0f;
	FSpaceShape * SpacePtr = SpaceShapeData;
	do
	{
		RetValue += SpacePtr->GetDoorCoverRetractValue() + SpacePtr->GetDoorNestedRetractValue();
		break;
		SpacePtr = SpacePtr->GetParentSpaceShape();
	} while (SpacePtr);
	return RetValue;
}

bool FFrameShape::CalFrameBoardPosDimenForRectangle(TSharedPtr<FBoardShape> InBoardShape, int32 InType, int32 InIndex)
{
	//轮廓空间尺寸
	const float SpaceWidth = SpaceShapeData->GetShapeWidth();
	const float SpaceDepth = SpaceShapeData->GetShapeDepth();
	const float SpaceHeight = SpaceShapeData->GetShapeHeight();
	//当前板件厚度
	const float CurHeight = InBoardShape->GetShapeHeight();

	//框体板件厚度
	float TopHeight = 0.0;
	float BottomHeight = 0.0;
	float LeftHeight = 0.0;
	float RightHeight = 0.0;
	float BackHeight = 0.0;
	float FrontHeight = 0.0;
	//框体板件内移值
	float TopRetract = 0.0;
	float BottomRetract = 0.0;
	float LeftRetract = 0.0;
	float RightRetract = 0.0;
	float BackRetract = 0.0;
	float FrontRetract = 0.0;

	//根据板件添加顺序计算板件厚度和内移值
	TSharedPtr<FBoardShape> TopBoard = GetFrameBoardShape(0, 0);
	if (TopBoard.IsValid() && IsBoardInSequence(TopBoard, InBoardShape))
	{
		TopHeight = TopBoard->GetShapeHeight();
		TopRetract = TopBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> BottomBoard = GetFrameBoardShape(1, 0);
	if (BottomBoard.IsValid() && IsBoardInSequence(BottomBoard, InBoardShape))
	{
		BottomHeight = BottomBoard->GetShapeHeight();
		BottomRetract = BottomBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> LeftBoard = GetFrameBoardShape(2, 0);
	if (LeftBoard.IsValid() && IsBoardInSequence(LeftBoard, InBoardShape))
	{
		LeftHeight = LeftBoard->GetShapeHeight();
		LeftRetract = LeftBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> RightBoard = GetFrameBoardShape(3, 0);
	if (RightBoard.IsValid() && IsBoardInSequence(RightBoard, InBoardShape))
	{
		RightHeight = RightBoard->GetShapeHeight();
		RightRetract = RightBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> BackBoard = GetFrameBoardShape(4, 0);
	if (BackBoard.IsValid() && IsBoardInSequence(BackBoard, InBoardShape))
	{
		BackHeight = BackBoard->GetShapeHeight();
		BackRetract = BackBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> FrontBoard = GetFrameBoardShape(5, 0);
	if (FrontBoard.IsValid() && IsBoardInSequence(FrontBoard, InBoardShape))
	{
		FrontHeight = FrontBoard->GetShapeHeight();
		FrontRetract = FrontBoard->GetShapeContraction() + FrontBoard->GetShapeConfluenceContraction();
	}
	//计算板件位置和尺寸
	check(0 == InIndex);
	switch (InType)
	{
		case 0:
		{
			InBoardShape->SetShapePosX(LeftHeight + LeftRetract);
			InBoardShape->SetShapePosY(BackHeight + BackRetract);
			InBoardShape->SetShapePosZ(SpaceHeight - CurHeight);
			InBoardShape->SetShapeWidth(SpaceWidth - LeftHeight - LeftRetract - RightHeight - RightRetract);
			//InBoardShape->SetShapeDepth(SpaceDepth - BackHeight - BackRetract - FrontHeight - FrontRetract - GetSpaceConfluenceCoverdBoardValue());
			InBoardShape->SetShapeDepth(SpaceDepth - BackHeight - BackRetract - FrontHeight - FrontRetract);
			break;
		}
		case 1:
		{
			InBoardShape->SetShapePosX(LeftHeight + LeftRetract);
			InBoardShape->SetShapePosY(BackHeight + BackRetract);
			InBoardShape->SetShapePosZ(0.0);
			InBoardShape->SetShapeWidth(SpaceWidth - LeftHeight - LeftRetract - RightHeight - RightRetract);
			InBoardShape->SetShapeDepth(SpaceDepth - BackHeight - BackRetract - FrontHeight - FrontRetract);
			break;
		}
		case 2:
		{
			InBoardShape->SetShapePosX(CurHeight);
			InBoardShape->SetShapePosY(BackHeight + BackRetract);
			InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
			InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - TopRetract - BottomHeight - BottomRetract);
			InBoardShape->SetShapeDepth(SpaceDepth - BackHeight - BackRetract - FrontHeight - FrontRetract);
			break;
		}
		case 3:
		{
			InBoardShape->SetShapePosX(SpaceWidth);
			InBoardShape->SetShapePosY(BackHeight + BackRetract);
			InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
			InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - TopRetract - BottomHeight - BottomRetract);
			InBoardShape->SetShapeDepth(SpaceDepth - BackHeight - BackRetract - FrontHeight - FrontRetract);
			break;
		}
		case 4:
		{
			InBoardShape->SetShapePosX(LeftHeight + LeftRetract);
			InBoardShape->SetShapePosY(0.0);
			InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
			InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - TopRetract - BottomHeight - BottomRetract);
			InBoardShape->SetShapeDepth(SpaceWidth - LeftHeight - LeftRetract - RightHeight - RightRetract);
			break;
		}
		case 5:
		{
			InBoardShape->SetShapePosX(LeftHeight + LeftRetract);
			InBoardShape->SetShapePosY(SpaceDepth - CurHeight - GetSpaceConfluenceCoverdBoardValue());
			InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
			InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - TopRetract - BottomHeight - BottomRetract);
			InBoardShape->SetShapeDepth(SpaceWidth - LeftHeight - LeftRetract - RightHeight - RightRetract);
			break;
		}
		default:
			check(false);
			break;
	}
	return true;
}

bool FFrameShape::CalFrameBoardPosDimenForLeftGirder(TSharedPtr<FBoardShape> InBoardShape, int32 InType, int32 InIndex)
{
	//轮廓空间尺寸
	float SpaceWidth = SpaceShapeData->GetShapeWidth();
	float SpaceDepth = SpaceShapeData->GetShapeDepth();
	float SpaceHeight = SpaceShapeData->GetShapeHeight();

	//轮廓参数
	TSharedPtr<FSCTOutline> Outline = SpaceShapeData->GetSpaceOutline();
	float GWidth = Outline->GetOutlineParamValueByRefName(TEXT("GW"));
	float GHeight = Outline->GetOutlineParamValueByRefName(TEXT("GH"));

	//当前板件厚度
	float CurHeight = InBoardShape->GetShapeHeight();

	//框体板件厚度
	float TopHeight0 = 0.0;
	float TopHeight1 = 0.0;
	float BottomHeight = 0.0;
	float LeftHeight0 = 0.0;
	float LeftHeight1 = 0.0;
	float RightHeight = 0.0;
	float BackHeight = 0.0;
	float FrontHeight = 0.0;
	//框体板件内移值
	float TopRetract0 = 0.0;
	float TopRetract1 = 0.0;
	float BottomRetract = 0.0;
	float LeftRetract0 = 0.0;
	float LeftRetract1 = 0.0;
	float RightRetract = 0.0;
	float BackRetract = 0.0;
	float FrontRetract = 0.0;

	//根据板件添加顺序计算板件厚度和内移值
	TSharedPtr<FBoardShape> TopBoard0 = GetFrameBoardShape(0, 0);
	if (TopBoard0.IsValid() && IsBoardInSequence(TopBoard0, InBoardShape))
	{
		TopHeight0 = TopBoard0->GetShapeHeight();
		TopRetract0 = TopBoard0->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> TopBoard1 = GetFrameBoardShape(0, 1);
	if (TopBoard1.IsValid() && IsBoardInSequence(TopBoard1, InBoardShape))
	{
		TopHeight1 = TopBoard1->GetShapeHeight();
		TopRetract1 = TopBoard1->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> BottomBoard = GetFrameBoardShape(1, 0);
	if (BottomBoard.IsValid() && IsBoardInSequence(BottomBoard, InBoardShape))
	{
		BottomHeight = BottomBoard->GetShapeHeight();
		BottomRetract = BottomBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> LeftBoard0 = GetFrameBoardShape(2, 0);
	if (LeftBoard0.IsValid() && IsBoardInSequence(LeftBoard0, InBoardShape))
	{
		LeftHeight0 = LeftBoard0->GetShapeHeight();
		LeftRetract0 = LeftBoard0->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> LeftBoard1 = GetFrameBoardShape(2, 1);
	if (LeftBoard1.IsValid() && IsBoardInSequence(LeftBoard1, InBoardShape))
	{
		LeftHeight1 = LeftBoard1->GetShapeHeight();
		LeftRetract1 = LeftBoard1->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> RightBoard = GetFrameBoardShape(3, 0);
	if (RightBoard.IsValid() && IsBoardInSequence(RightBoard, InBoardShape))
	{
		RightHeight = RightBoard->GetShapeHeight();
		RightRetract = RightBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> BackBoard = GetFrameBoardShape(4, 0);
	if (BackBoard.IsValid() && IsBoardInSequence(BackBoard, InBoardShape))
	{
		BackHeight = BackBoard->GetShapeHeight();
		BackRetract = BackBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> FrontBoard = GetFrameBoardShape(5, 0);
	if (FrontBoard.IsValid() && IsBoardInSequence(FrontBoard, InBoardShape))
	{
		FrontHeight = FrontBoard->GetShapeHeight();
		FrontRetract = FrontBoard->GetShapeContraction();
	}

	//计算板件位置和尺寸
	switch (InType)
	{
	case 0:
	{
		if (InIndex == 0)
		{
			InBoardShape->SetShapePosX(GWidth + LeftHeight1 + LeftRetract1);
			InBoardShape->SetShapePosY(BackHeight + BackRetract);
			InBoardShape->SetShapePosZ(SpaceHeight - CurHeight);
			InBoardShape->SetShapeWidth(SpaceWidth - GWidth - LeftHeight1 - LeftRetract1 - RightHeight - RightRetract);
			InBoardShape->SetShapeDepth(SpaceDepth - BackHeight - BackRetract - FrontHeight - FrontRetract);
		}
		else if (InIndex == 1)
		{
			InBoardShape->SetShapePosX(LeftHeight0 + LeftRetract0);
			InBoardShape->SetShapePosY(BackHeight + BackRetract);
			InBoardShape->SetShapePosZ(SpaceHeight - GHeight - CurHeight);
			InBoardShape->SetShapeWidth(GWidth - LeftHeight0 - LeftRetract0 + LeftHeight1 + LeftRetract1);
			InBoardShape->SetShapeDepth(SpaceDepth - BackHeight - BackRetract - FrontHeight - FrontRetract);
		}
		else
		{
			check(false);
		}
		break;
	}
	case 1:
	{
		InBoardShape->SetShapePosX(LeftHeight0 + LeftRetract0);
		InBoardShape->SetShapePosY(BackHeight + BackRetract);
		InBoardShape->SetShapePosZ(0.0);
		InBoardShape->SetShapeWidth(SpaceWidth - LeftHeight0 - LeftRetract0 - RightHeight - RightRetract);
		InBoardShape->SetShapeDepth(SpaceDepth - BackHeight - BackRetract - FrontHeight - FrontRetract);
		break;
	}
	case 2:
	{
		if (0 == InIndex)
		{
			InBoardShape->SetShapePosX(CurHeight);
			InBoardShape->SetShapePosY(BackHeight + BackRetract);
			InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
			InBoardShape->SetShapeWidth(SpaceHeight - GHeight - TopHeight1 - TopRetract1 - BottomHeight - BottomRetract);
			InBoardShape->SetShapeDepth(SpaceDepth - BackHeight - BackRetract - FrontHeight - FrontRetract);
		}
		else if (1 == InIndex)
		{
			InBoardShape->SetShapePosX(GWidth + CurHeight);
			InBoardShape->SetShapePosY(BackHeight + BackRetract);
			InBoardShape->SetShapePosZ(SpaceHeight - GHeight - TopHeight1 - TopRetract1);
			InBoardShape->SetShapeWidth(GHeight - TopHeight0 - TopRetract0 + TopHeight1 + TopRetract1);
			InBoardShape->SetShapeDepth(SpaceDepth - BackHeight - BackRetract - FrontHeight - FrontRetract);
		} 
		else
		{
			check(false);
		}
		break;
	}
	case 3:
	{
		InBoardShape->SetShapePosX(SpaceWidth);
		InBoardShape->SetShapePosY(BackHeight + BackRetract);
		InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
		InBoardShape->SetShapeWidth(SpaceHeight - TopHeight0 - TopRetract0 - BottomHeight - BottomRetract);
		InBoardShape->SetShapeDepth(SpaceDepth - BackHeight - BackRetract - FrontHeight - FrontRetract);
		break;
	}
	case 4:
	{
		InBoardShape->SetShapePosX(LeftHeight0 + LeftRetract0);
		InBoardShape->SetShapePosY(0.0);
		InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
		InBoardShape->SetShapeWidth(SpaceHeight - TopHeight0 - TopRetract0 - BottomHeight - BottomRetract);
		InBoardShape->SetShapeDepth(SpaceWidth - LeftHeight0 - LeftRetract0 - RightHeight - RightRetract);
		//设置板件的轮廓信息
		FSCTOutline* CopyOutline = new FSCTOutline();
		Outline->CopyTo(CopyOutline);
		CopyOutline->SetOutlineParamValueByRefName(TEXT("GW"), GWidth - LeftHeight0 - LeftRetract0 + LeftHeight1 + LeftRetract1);
		CopyOutline->SetOutlineParamValueByRefName(TEXT("GH"), GHeight - TopHeight0 - TopRetract0 + TopHeight1 + TopRetract1);
		InBoardShape->SetBoardOutline(MakeShareable(CopyOutline));
		break;
	}
	case 5:
	{
		InBoardShape->SetShapePosX(LeftHeight0 + LeftRetract0);
		InBoardShape->SetShapePosY(SpaceDepth - CurHeight);
		InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
		InBoardShape->SetShapeWidth(SpaceHeight - TopHeight0 - TopRetract0 - BottomHeight - BottomRetract);
		InBoardShape->SetShapeDepth(SpaceWidth - LeftHeight0 - LeftRetract0 - RightHeight - RightRetract);
		//设置板件的轮廓信息
		FSCTOutline* CopyOutline = new FSCTOutline();
		Outline->CopyTo(CopyOutline);
		CopyOutline->SetOutlineParamValueByRefName(TEXT("GW"), GWidth - LeftHeight0 - LeftRetract0 + LeftHeight1 + LeftRetract1);
		CopyOutline->SetOutlineParamValueByRefName(TEXT("GH"), GHeight - TopHeight0 - TopRetract0 + TopHeight1 + TopRetract1);
		InBoardShape->SetBoardOutline(MakeShareable(CopyOutline));
		break;
	}
	default:
		check(false);
		break;
	}

	return true;
}

bool FFrameShape::CalFrameBoardPosDimenForRightGirder(TSharedPtr<FBoardShape> InBoardShape, int32 InType, int32 InIndex)
{
	//轮廓空间尺寸
	float SpaceWidth = SpaceShapeData->GetShapeWidth();
	float SpaceDepth = SpaceShapeData->GetShapeDepth();
	float SpaceHeight = SpaceShapeData->GetShapeHeight();

	//轮廓参数
	TSharedPtr<FSCTOutline> Outline = SpaceShapeData->GetSpaceOutline();
	float GWidth = Outline->GetOutlineParamValueByRefName(TEXT("GW"));
	float GHeight = Outline->GetOutlineParamValueByRefName(TEXT("GH"));

	//当前板件厚度
	float CurHeight = InBoardShape->GetShapeHeight();

	//框体板件厚度
	float TopHeight0 = 0.0;
	float TopHeight1 = 0.0;
	float BottomHeight = 0.0;
	float LeftHeight = 0.0;
	float RightHeight0 = 0.0;
	float RightHeight1 = 0.0;
	float BackHeight = 0.0;
	float FrontHeight = 0.0;
	//框体板件内移值
	float TopRetract0 = 0.0;
	float TopRetract1 = 0.0;
	float BottomRetract = 0.0;
	float LeftRetract = 0.0;
	float RightRetract0 = 0.0;
	float RightRetract1 = 0.0;
	float BackRetract = 0.0;
	float FrontRetract = 0.0;

	//根据板件添加顺序计算板件厚度和内移值
	TSharedPtr<FBoardShape> TopBoard0 = GetFrameBoardShape(0, 0);
	if (TopBoard0.IsValid() && IsBoardInSequence(TopBoard0, InBoardShape))
	{
		TopHeight0 = TopBoard0->GetShapeHeight();
		TopRetract0 = TopBoard0->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> TopBoard1 = GetFrameBoardShape(0, 1);
	if (TopBoard1.IsValid() && IsBoardInSequence(TopBoard1, InBoardShape))
	{
		TopHeight1 = TopBoard1->GetShapeHeight();
		TopRetract1 = TopBoard1->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> BottomBoard = GetFrameBoardShape(1, 0);
	if (BottomBoard.IsValid() && IsBoardInSequence(BottomBoard, InBoardShape))
	{
		BottomHeight = BottomBoard->GetShapeHeight();
		BottomRetract = BottomBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> LeftBoard = GetFrameBoardShape(2, 0);
	if (LeftBoard.IsValid() && IsBoardInSequence(LeftBoard, InBoardShape))
	{
		LeftHeight = LeftBoard->GetShapeHeight();
		LeftRetract = LeftBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> RightBoard0 = GetFrameBoardShape(3, 0);
	if (RightBoard0.IsValid() && IsBoardInSequence(RightBoard0, InBoardShape))
	{
		RightHeight0 = RightBoard0->GetShapeHeight();
		RightRetract0 = RightBoard0->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> RightBoard1 = GetFrameBoardShape(3, 1);
	if (RightBoard1.IsValid() && IsBoardInSequence(RightBoard1, InBoardShape))
	{
		RightHeight1 = RightBoard1->GetShapeHeight();
		RightRetract1 = RightBoard1->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> BackBoard = GetFrameBoardShape(4, 0);
	if (BackBoard.IsValid() && IsBoardInSequence(BackBoard, InBoardShape))
	{
		BackHeight = BackBoard->GetShapeHeight();
		BackRetract = BackBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> FrontBoard = GetFrameBoardShape(5, 0);
	if (FrontBoard.IsValid() && IsBoardInSequence(FrontBoard, InBoardShape))
	{
		FrontHeight = FrontBoard->GetShapeHeight();
		FrontRetract = FrontBoard->GetShapeContraction();
	}

	//计算板件位置和尺寸
	switch (InType)
	{
	case 0:
	{
		if (InIndex == 0)
		{
			InBoardShape->SetShapePosX(LeftHeight + LeftRetract);
			InBoardShape->SetShapePosY(BackHeight + BackRetract);
			InBoardShape->SetShapePosZ(SpaceHeight - CurHeight);
			InBoardShape->SetShapeWidth(SpaceWidth - GWidth - LeftHeight - LeftRetract - RightHeight0 - RightRetract0);
			InBoardShape->SetShapeDepth(SpaceDepth - BackHeight - BackRetract - FrontHeight - FrontRetract);
		}
		else if (InIndex == 1)
		{
			InBoardShape->SetShapePosX(SpaceWidth - GWidth - RightHeight1 - RightRetract1);
			InBoardShape->SetShapePosY(BackHeight + BackRetract);
			InBoardShape->SetShapePosZ(SpaceHeight - GHeight - CurHeight);
			InBoardShape->SetShapeWidth(GWidth - RightHeight0  - RightRetract0 + RightHeight1 + RightRetract1);
			InBoardShape->SetShapeDepth(SpaceDepth - BackHeight - BackRetract - FrontHeight - FrontRetract);
		}
		else
		{
			check(false);
		}
		break;
	}
	case 1:
	{
		InBoardShape->SetShapePosX(LeftHeight + LeftRetract);
		InBoardShape->SetShapePosY(BackHeight + BackRetract);
		InBoardShape->SetShapePosZ(0.0);
		InBoardShape->SetShapeWidth(SpaceWidth - LeftHeight - LeftRetract - RightHeight0 - RightRetract0);
		InBoardShape->SetShapeDepth(SpaceDepth - BackHeight - BackRetract - FrontHeight - FrontRetract);
		break;
	}
	case 2:
	{
		InBoardShape->SetShapePosX(CurHeight);
		InBoardShape->SetShapePosY(BackHeight + BackRetract);
		InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
		InBoardShape->SetShapeWidth(SpaceHeight - TopHeight0 - TopRetract0 - BottomHeight - BottomRetract);
		InBoardShape->SetShapeDepth(SpaceDepth - BackHeight - BackRetract - FrontHeight - FrontRetract);
		break;
	}
	case 3:
	{
		if (0 == InIndex)
		{
			InBoardShape->SetShapePosX(SpaceWidth);
			InBoardShape->SetShapePosY(BackHeight + BackRetract);
			InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
			InBoardShape->SetShapeWidth(SpaceHeight - GHeight - TopHeight1 - TopRetract1 - BottomHeight - BottomRetract);
			InBoardShape->SetShapeDepth(SpaceDepth - BackHeight - BackRetract - FrontHeight - FrontRetract);
		}
		else if (1 == InIndex)
		{
			InBoardShape->SetShapePosX(SpaceWidth - GWidth);
			InBoardShape->SetShapePosY(BackHeight + BackRetract);
			InBoardShape->SetShapePosZ(SpaceHeight - GHeight - TopHeight1 - TopRetract1);
			InBoardShape->SetShapeWidth(GHeight - TopHeight0 - TopRetract0 + TopHeight1 + TopRetract1);
			InBoardShape->SetShapeDepth(SpaceDepth - BackHeight - BackRetract - FrontHeight - FrontRetract);
		}
		else
		{
			check(false);
		}
		break;
	}
	case 4:
	{
		InBoardShape->SetShapePosX(LeftHeight + LeftRetract);
		InBoardShape->SetShapePosY(0.0);
		InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
		InBoardShape->SetShapeWidth(SpaceHeight - TopHeight0 - TopRetract0 - BottomHeight - BottomRetract);
		InBoardShape->SetShapeDepth(SpaceWidth - LeftHeight - LeftRetract - RightHeight0 - RightRetract0);
		//设置板件的轮廓信息
		FSCTOutline* CopyOutline = new FSCTOutline();
		Outline->CopyTo(CopyOutline);
		CopyOutline->SetOutlineParamValueByRefName(TEXT("GW"), GWidth - RightHeight0 - RightRetract0 + RightHeight1 + RightRetract1);
		CopyOutline->SetOutlineParamValueByRefName(TEXT("GH"), GHeight - TopHeight0 - TopRetract0 + TopHeight1 + TopRetract1);
		InBoardShape->SetBoardOutline(MakeShareable(CopyOutline));
		break;
	}
	case 5:
	{
		InBoardShape->SetShapePosX(LeftHeight + LeftRetract);
		InBoardShape->SetShapePosY(SpaceDepth - CurHeight);
		InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
		InBoardShape->SetShapeWidth(SpaceHeight - TopHeight0 - TopRetract0 - BottomHeight - BottomRetract);
		InBoardShape->SetShapeDepth(SpaceWidth - LeftHeight - LeftRetract - RightHeight0 - RightRetract0);
		//设置板件的轮廓信息
		FSCTOutline* CopyOutline = new FSCTOutline();
		Outline->CopyTo(CopyOutline);
		CopyOutline->SetOutlineParamValueByRefName(TEXT("GW"), GWidth - RightHeight0 - RightRetract0 + RightHeight1 + RightRetract1);
		CopyOutline->SetOutlineParamValueByRefName(TEXT("GH"), GHeight - TopHeight0 - TopRetract0 + TopHeight1 + TopRetract1);
		InBoardShape->SetBoardOutline(MakeShareable(CopyOutline));
		break;
	}
	default:
		check(false);
		break;
	}

	return true;
}

bool FFrameShape::CalFrameBoardPosDimenForBackGirder(TSharedPtr<FBoardShape> InBoardShape, int32 InType, int32 InIndex)
{
	//轮廓空间尺寸
	float SpaceWidth = SpaceShapeData->GetShapeWidth();
	float SpaceDepth = SpaceShapeData->GetShapeDepth();
	float SpaceHeight = SpaceShapeData->GetShapeHeight();

	//轮廓参数
	TSharedPtr<FSCTOutline> Outline = SpaceShapeData->GetSpaceOutline();
	float GDepth = Outline->GetOutlineParamValueByRefName(TEXT("GD"));
	float GHeight = Outline->GetOutlineParamValueByRefName(TEXT("GH"));

	//当前板件厚度
	float CurHeight = InBoardShape->GetShapeHeight();

	//框体板件厚度
	float TopHeight0 = 0.0;
	float TopHeight1 = 0.0;
	float BottomHeight = 0.0;
	float LeftHeight = 0.0;
	float RightHeight = 0.0;
	float BackHeight0 = 0.0;
	float BackHeight1 = 0.0;
	float FrontHeight = 0.0;
	//框体板件内移值
	float TopRetract0 = 0.0;
	float TopRetract1 = 0.0;
	float BottomRetract = 0.0;
	float LeftRetract = 0.0;
	float RightRetract = 0.0;
	float BackRetract0 = 0.0;
	float BackRetract1 = 0.0;
	float FrontRetract = 0.0;

	//根据板件添加顺序计算板件厚度和内移值
	TSharedPtr<FBoardShape> TopBoard0 = GetFrameBoardShape(0, 0);
	if (TopBoard0.IsValid() && IsBoardInSequence(TopBoard0, InBoardShape))
	{
		TopHeight0 = TopBoard0->GetShapeHeight();
		TopRetract0 = TopBoard0->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> TopBoard1 = GetFrameBoardShape(0, 1);
	if (TopBoard1.IsValid() && IsBoardInSequence(TopBoard1, InBoardShape))
	{
		TopHeight1 = TopBoard1->GetShapeHeight();
		TopRetract1 = TopBoard1->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> BottomBoard = GetFrameBoardShape(1, 0);
	if (BottomBoard.IsValid() && IsBoardInSequence(BottomBoard, InBoardShape))
	{
		BottomHeight = BottomBoard->GetShapeHeight();
		BottomRetract = BottomBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> LeftBoard = GetFrameBoardShape(2, 0);
	if (LeftBoard.IsValid() && IsBoardInSequence(LeftBoard, InBoardShape))
	{
		LeftHeight = LeftBoard->GetShapeHeight();
		LeftRetract = LeftBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> RightBoard = GetFrameBoardShape(3, 0);
	if (RightBoard.IsValid() && IsBoardInSequence(RightBoard, InBoardShape))
	{
		RightHeight = RightBoard->GetShapeHeight();
		RightRetract = RightBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> BackBoard0 = GetFrameBoardShape(4, 0);
	if (BackBoard0.IsValid() && IsBoardInSequence(BackBoard0, InBoardShape))
	{
		BackHeight0 = BackBoard0->GetShapeHeight();
		BackRetract0 = BackBoard0->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> BackBoard1 = GetFrameBoardShape(4, 1);
	if (BackBoard1.IsValid() && IsBoardInSequence(BackBoard1, InBoardShape))
	{
		BackHeight1 = BackBoard1->GetShapeHeight();
		BackRetract1 = BackBoard1->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> FrontBoard = GetFrameBoardShape(5, 0);
	if (FrontBoard.IsValid() && IsBoardInSequence(FrontBoard, InBoardShape))
	{
		FrontHeight = FrontBoard->GetShapeHeight();
		FrontRetract = FrontBoard->GetShapeContraction();
	}

	//计算板件位置和尺寸
	switch (InType)
	{
	case 0:
	{
		if (InIndex == 0)
		{
			InBoardShape->SetShapePosX(LeftHeight + LeftRetract);
			InBoardShape->SetShapePosY(GDepth + BackHeight1 + BackRetract1);
			InBoardShape->SetShapePosZ(SpaceHeight - CurHeight);
			InBoardShape->SetShapeWidth(SpaceWidth - LeftHeight - LeftRetract - RightHeight - RightRetract);
			InBoardShape->SetShapeDepth(SpaceDepth - GDepth - BackHeight1 - BackRetract1 - FrontHeight - FrontRetract);
		}
		else if (InIndex == 1)
		{
			InBoardShape->SetShapePosX(LeftHeight + LeftRetract);
			InBoardShape->SetShapePosY(BackHeight0 + BackRetract0);
			InBoardShape->SetShapePosZ(SpaceHeight - GHeight - CurHeight);
			InBoardShape->SetShapeWidth(SpaceWidth - LeftHeight - LeftRetract - RightHeight - RightRetract);
			InBoardShape->SetShapeDepth(GDepth - BackHeight0 - BackRetract0 + BackHeight1 + BackRetract1);
		} 
		else
		{
			check(false);
		}
		break;
	}
	case 1:
	{
		InBoardShape->SetShapePosX(LeftHeight + LeftRetract);
		InBoardShape->SetShapePosY(BackHeight0 + BackRetract0);
		InBoardShape->SetShapePosZ(0.0);
		InBoardShape->SetShapeWidth(SpaceWidth - LeftHeight - LeftRetract - RightHeight - RightRetract);
		InBoardShape->SetShapeDepth(SpaceDepth - BackHeight0 - BackRetract0 - FrontHeight - FrontRetract);
		break;
	}
	case 2:
	{
		InBoardShape->SetShapePosX(CurHeight);
		InBoardShape->SetShapePosY(BackHeight0 + BackRetract0);
		InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
		InBoardShape->SetShapeWidth(SpaceHeight - TopHeight0 - TopRetract0 - BottomHeight - BottomRetract);
		InBoardShape->SetShapeDepth(SpaceDepth - BackHeight0 - BackRetract0 - FrontHeight - FrontRetract);
		//设置板件的轮廓信息
		FSCTOutline* CopyOutline = new FSCTOutline();
		Outline->CopyTo(CopyOutline);
		CopyOutline->SetOutlineParamValueByRefName(TEXT("GD"), GDepth - BackHeight0 - BackRetract0 + BackHeight1 + BackRetract1);
		CopyOutline->SetOutlineParamValueByRefName(TEXT("GH"), GHeight - TopHeight0 - TopRetract0 + TopHeight1 + TopRetract1);
		InBoardShape->SetBoardOutline(MakeShareable(CopyOutline));
		break;
	}
	case 3:
	{
		InBoardShape->SetShapePosX(SpaceWidth);
		InBoardShape->SetShapePosY(BackHeight0 + BackRetract0);
		InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
		InBoardShape->SetShapeWidth(SpaceHeight - TopHeight0 - TopRetract0 - BottomHeight - BottomRetract);
		InBoardShape->SetShapeDepth(SpaceDepth - BackHeight0 - BackRetract0 - FrontHeight - FrontRetract);
		//设置板件的轮廓信息
		FSCTOutline* CopyOutline = new FSCTOutline();
		Outline->CopyTo(CopyOutline);
		CopyOutline->SetOutlineParamValueByRefName(TEXT("GD"), GDepth - BackHeight0 - BackRetract0 + BackHeight1 + BackRetract1);
		CopyOutline->SetOutlineParamValueByRefName(TEXT("GH"), GHeight - TopHeight0 - TopRetract0 + TopHeight1 + TopRetract1);
		InBoardShape->SetBoardOutline(MakeShareable(CopyOutline));
		break;
	}
	case 4:
	{
		if (0 == InIndex)
		{
			InBoardShape->SetShapePosX(LeftHeight + LeftRetract);
			InBoardShape->SetShapePosY(0.0);
			InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
			InBoardShape->SetShapeWidth(SpaceHeight - GHeight - TopHeight1 - TopRetract1 - BottomHeight - BottomRetract);
			InBoardShape->SetShapeDepth(SpaceWidth - LeftHeight - LeftRetract - RightHeight - RightRetract);
		} 
		else if (1 == InIndex)
		{
			InBoardShape->SetShapePosX(LeftHeight + LeftRetract);
			InBoardShape->SetShapePosY(GDepth);
			InBoardShape->SetShapePosZ(SpaceHeight - GHeight - TopHeight1 - TopRetract1);
			InBoardShape->SetShapeWidth(GHeight - TopHeight0 - TopRetract0 + TopHeight1 + TopRetract1);
			InBoardShape->SetShapeDepth(SpaceWidth - LeftHeight - LeftRetract - RightHeight - RightRetract);
		}
		else
		{
			check(false);
		}
		break;
	}
	case 5:
	{
		InBoardShape->SetShapePosX(LeftHeight + LeftRetract);
		InBoardShape->SetShapePosY(SpaceDepth - CurHeight);
		InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
		InBoardShape->SetShapeWidth(SpaceHeight - TopHeight0 - TopRetract0 - BottomHeight - BottomRetract);
		InBoardShape->SetShapeDepth(SpaceWidth - LeftHeight - LeftRetract - RightHeight - RightRetract);
		break;
	}
	default:
		check(false);
		break;
	}

	return true;
}

bool FFrameShape::CalFrameBoardPosDimenForLeftPiller(TSharedPtr<FBoardShape> InBoardShape, int32 InType, int32 InIndex)
{
	//轮廓空间尺寸
	float SpaceWidth = SpaceShapeData->GetShapeWidth();
	float SpaceDepth = SpaceShapeData->GetShapeDepth();
	float SpaceHeight = SpaceShapeData->GetShapeHeight();

	//轮廓参数
	TSharedPtr<FSCTOutline> Outline = SpaceShapeData->GetSpaceOutline();
	float PWidth = Outline->GetOutlineParamValueByRefName(TEXT("PW"));
	float PDepth = Outline->GetOutlineParamValueByRefName(TEXT("PD"));

	//当前板件厚度
	float CurHeight = InBoardShape->GetShapeHeight();

	//框体板件厚度
	float TopHeight = 0.0;
	float BottomHeight = 0.0;
	float LeftHeight0 = 0.0;
	float LeftHeight1 = 0.0;
	float RightHeight = 0.0;
	float BackHeight0 = 0.0;
	float BackHeight1 = 0.0;
	float FrontHeight = 0.0;
	//框体板件内移值
	float TopRetract = 0.0;
	float BottomRetract = 0.0;
	float LeftRetract0 = 0.0;
	float LeftRetract1 = 0.0;
	float RightRetract = 0.0;
	float BackRetract0 = 0.0;
	float BackRetract1 = 0.0;
	float FrontRetract = 0.0;

	//根据板件添加顺序计算板件厚度和内移值
	TSharedPtr<FBoardShape> TopBoard = GetFrameBoardShape(0, 0);
	if (TopBoard.IsValid() && IsBoardInSequence(TopBoard, InBoardShape))
	{
		TopHeight = TopBoard->GetShapeHeight();
		TopRetract = TopBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> BottomBoard = GetFrameBoardShape(1, 0);
	if (BottomBoard.IsValid() && IsBoardInSequence(BottomBoard, InBoardShape))
	{
		BottomHeight = BottomBoard->GetShapeHeight();
		BottomRetract = BottomBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> LeftBoard0 = GetFrameBoardShape(2, 0);
	if (LeftBoard0.IsValid() && IsBoardInSequence(LeftBoard0, InBoardShape))
	{
		LeftHeight0 = LeftBoard0->GetShapeHeight();
		LeftRetract0 = LeftBoard0->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> LeftBoard1 = GetFrameBoardShape(2, 1);
	if (LeftBoard1.IsValid() && IsBoardInSequence(LeftBoard1, InBoardShape))
	{
		LeftHeight1 = LeftBoard1->GetShapeHeight();
		LeftRetract1 = LeftBoard1->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> RightBoard = GetFrameBoardShape(3, 0);
	if (RightBoard.IsValid() && IsBoardInSequence(RightBoard, InBoardShape))
	{
		RightHeight = RightBoard->GetShapeHeight();
		RightRetract = RightBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> BackBoard0 = GetFrameBoardShape(4, 0);
	if (BackBoard0.IsValid() && IsBoardInSequence(BackBoard0, InBoardShape))
	{
		BackHeight0 = BackBoard0->GetShapeHeight();
		BackRetract0 = BackBoard0->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> BackBoard1 = GetFrameBoardShape(4, 1);
	if (BackBoard1.IsValid() && IsBoardInSequence(BackBoard1, InBoardShape))
	{
		BackHeight1 = BackBoard1->GetShapeHeight();
		BackRetract1 = BackBoard1->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> FrontBoard = GetFrameBoardShape(5, 0);
	if (FrontBoard.IsValid() && IsBoardInSequence(FrontBoard, InBoardShape))
	{
		FrontHeight = FrontBoard->GetShapeHeight();
		FrontRetract = FrontBoard->GetShapeContraction();
	}

	//计算板件位置和尺寸
	switch (InType)
	{
	case 0:
	{
		InBoardShape->SetShapePosX(LeftHeight0 + LeftRetract0);
		InBoardShape->SetShapePosY(BackHeight0 + BackRetract0);
		InBoardShape->SetShapePosZ(SpaceHeight - CurHeight);
		InBoardShape->SetShapeWidth(SpaceWidth - LeftHeight0 - LeftRetract0 - RightHeight - RightRetract);
		InBoardShape->SetShapeDepth(SpaceDepth - BackHeight0 - BackRetract0 - FrontHeight - FrontRetract);
		//设置板件的轮廓信息
		FSCTOutline* CopyOutline = new FSCTOutline();
		Outline->CopyTo(CopyOutline);
		CopyOutline->SetOutlineParamValueByRefName(TEXT("PW"), PWidth - LeftHeight0 - LeftRetract0 + LeftHeight1 + LeftRetract1);
		CopyOutline->SetOutlineParamValueByRefName(TEXT("PD"), PDepth - BackHeight0 - BackRetract0 + BackHeight1 + BackRetract1);
		InBoardShape->SetBoardOutline(MakeShareable(CopyOutline));
		break;
	}
	case 1:
	{
		InBoardShape->SetShapePosX(LeftHeight0 + LeftRetract0);
		InBoardShape->SetShapePosY(BackHeight0 + BackRetract0);
		InBoardShape->SetShapePosZ(0.0);
		InBoardShape->SetShapeWidth(SpaceWidth - LeftHeight0 - LeftRetract0 - RightHeight - RightRetract);
		InBoardShape->SetShapeDepth(SpaceDepth - BackHeight0 - BackRetract0 - FrontHeight - FrontRetract);
		//设置板件的轮廓信息
		FSCTOutline* CopyOutline = new FSCTOutline();
		Outline->CopyTo(CopyOutline);
		CopyOutline->SetOutlineParamValueByRefName(TEXT("PW"), PWidth - LeftHeight0 - LeftRetract0 + LeftHeight1 + LeftRetract1);
		CopyOutline->SetOutlineParamValueByRefName(TEXT("PD"), PDepth - BackHeight0 - BackRetract0 + BackHeight1 + BackRetract1);
		InBoardShape->SetBoardOutline(MakeShareable(CopyOutline));
		break;
	}
	case 2:
	{
		if (InIndex == 0)
		{
			InBoardShape->SetShapePosX(CurHeight);
			InBoardShape->SetShapePosY(PDepth + BackHeight1 + BackRetract1);
			InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
			InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - TopRetract - BottomHeight - BottomRetract);
			InBoardShape->SetShapeDepth(SpaceDepth - PDepth - BackHeight1 - BackRetract1 - FrontHeight - FrontRetract);
		}
		else if (InIndex == 1)
		{
			InBoardShape->SetShapePosX(PWidth + CurHeight);
			InBoardShape->SetShapePosY(BackHeight0 + BackRetract0);
			InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
			InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - TopRetract - BottomHeight - BottomRetract);
			InBoardShape->SetShapeDepth(PDepth - BackHeight0 - BackRetract0 + BackHeight1 + BackRetract1);
		}
		else
		{
			check(false);
		}
		break;
	}
	case 3:
	{
		InBoardShape->SetShapePosX(SpaceWidth);
		InBoardShape->SetShapePosY(BackHeight0 + BackRetract0);
		InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
		InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - TopRetract - BottomHeight - BottomRetract);
		InBoardShape->SetShapeDepth(SpaceDepth - BackHeight0 - BackRetract0 - FrontHeight - FrontRetract);
		break;
	}
	case 4:
	{
		if (0 == InIndex)
		{
			InBoardShape->SetShapePosX(PWidth + LeftHeight1 + LeftRetract1);
			InBoardShape->SetShapePosY(0.0);
			InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
			InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - TopRetract - BottomHeight - BottomRetract);
			InBoardShape->SetShapeDepth(SpaceWidth - PWidth - LeftHeight1 - LeftRetract1 - RightHeight - RightRetract);
		}
		else if (1 == InIndex)
		{
			InBoardShape->SetShapePosX(LeftHeight0 + LeftRetract0);
			InBoardShape->SetShapePosY(PDepth);
			InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
			InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - TopRetract);
			InBoardShape->SetShapeDepth(PWidth - LeftHeight0 - LeftRetract0 + LeftHeight1 + LeftRetract1);
		}
		else
		{
			check(false);
		}
		break;
	}
	case 5:
	{
		InBoardShape->SetShapePosX(LeftHeight0 + LeftRetract0);
		InBoardShape->SetShapePosY(SpaceDepth - CurHeight);
		InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
		InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - TopRetract - BottomHeight - BottomRetract);
		InBoardShape->SetShapeDepth(SpaceWidth - LeftHeight0 - LeftRetract0 - RightHeight - RightRetract);
		break;
	}
	default:
		check(false);
		break;
	}

	return true;
}

bool FFrameShape::CalFrameBoardPosDimenForRightPiller(TSharedPtr<FBoardShape> InBoardShape, int32 InType, int32 InIndex)
{
	//轮廓空间尺寸
	float SpaceWidth = SpaceShapeData->GetShapeWidth();
	float SpaceDepth = SpaceShapeData->GetShapeDepth();
	float SpaceHeight = SpaceShapeData->GetShapeHeight();

	//轮廓参数
	TSharedPtr<FSCTOutline> Outline = SpaceShapeData->GetSpaceOutline();
	float PWidth = Outline->GetOutlineParamValueByRefName(TEXT("PW"));
	float PDepth = Outline->GetOutlineParamValueByRefName(TEXT("PD"));

	//当前板件厚度
	float CurHeight = InBoardShape->GetShapeHeight();

	//框体板件厚度
	float TopHeight = 0.0;
	float BottomHeight = 0.0;
	float LeftHeight = 0.0;
	float RightHeight0 = 0.0;
	float RightHeight1 = 0.0;
	float BackHeight0 = 0.0;
	float BackHeight1 = 0.0;
	float FrontHeight = 0.0;
	//框体板件内移值
	float TopRetract = 0.0;
	float BottomRetract = 0.0;
	float LeftRetract = 0.0;
	float RightRetract0 = 0.0;
	float RightRetract1 = 0.0;
	float BackRetract0 = 0.0;
	float BackRetract1 = 0.0;
	float FrontRetract = 0.0;

	//根据板件添加顺序计算板件厚度和内移值
	TSharedPtr<FBoardShape> TopBoard = GetFrameBoardShape(0, 0);
	if (TopBoard.IsValid() && IsBoardInSequence(TopBoard, InBoardShape))
	{
		TopHeight = TopBoard->GetShapeHeight();
		TopRetract = TopBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> BottomBoard = GetFrameBoardShape(1, 0);
	if (BottomBoard.IsValid() && IsBoardInSequence(BottomBoard, InBoardShape))
	{
		BottomHeight = BottomBoard->GetShapeHeight();
		BottomRetract = BottomBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> LeftBoard = GetFrameBoardShape(2, 0);
	if (LeftBoard.IsValid() && IsBoardInSequence(LeftBoard, InBoardShape))
	{
		LeftHeight = LeftBoard->GetShapeHeight();
		LeftRetract = LeftBoard->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> RightBoard0 = GetFrameBoardShape(3, 0);
	if (RightBoard0.IsValid() && IsBoardInSequence(RightBoard0, InBoardShape))
	{
		RightHeight0 = RightBoard0->GetShapeHeight();
		RightRetract0 = RightBoard0->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> RightBoard1 = GetFrameBoardShape(3, 1);
	if (RightBoard1.IsValid() && IsBoardInSequence(RightBoard1, InBoardShape))
	{
		RightHeight1 = RightBoard1->GetShapeHeight();
		RightRetract1 = RightBoard1->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> BackBoard0 = GetFrameBoardShape(4, 0);
	if (BackBoard0.IsValid() && IsBoardInSequence(BackBoard0, InBoardShape))
	{
		BackHeight0 = BackBoard0->GetShapeHeight();
		BackRetract0 = BackBoard0->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> BackBoard1 = GetFrameBoardShape(4, 1);
	if (BackBoard1.IsValid() && IsBoardInSequence(BackBoard1, InBoardShape))
	{
		BackHeight1 = BackBoard1->GetShapeHeight();
		BackRetract1 = BackBoard1->GetShapeContraction();
	}
	TSharedPtr<FBoardShape> FrontBoard = GetFrameBoardShape(5, 0);
	if (FrontBoard.IsValid() && IsBoardInSequence(FrontBoard, InBoardShape))
	{
		FrontHeight = FrontBoard->GetShapeHeight();
		FrontRetract = FrontBoard->GetShapeContraction();
	}

	//计算板件位置和尺寸
	switch (InType)
	{
	case 0:
	{
		InBoardShape->SetShapePosX(LeftHeight + LeftRetract);
		InBoardShape->SetShapePosY(BackHeight0 + BackRetract0);
		InBoardShape->SetShapePosZ(SpaceHeight - CurHeight);
		InBoardShape->SetShapeWidth(SpaceWidth - LeftHeight - LeftRetract - RightHeight0 - RightRetract0);
		InBoardShape->SetShapeDepth(SpaceDepth - BackHeight0 - BackRetract0 - FrontHeight - FrontRetract);
		//设置板件的轮廓信息
		FSCTOutline* CopyOutline = new FSCTOutline();
		Outline->CopyTo(CopyOutline);
		CopyOutline->SetOutlineParamValueByRefName(TEXT("PW"), PWidth - RightHeight0 - RightRetract0 + RightHeight1 + RightRetract1);
		CopyOutline->SetOutlineParamValueByRefName(TEXT("PD"), PDepth - BackHeight0 - BackRetract0 + BackHeight1 + BackRetract1);
		InBoardShape->SetBoardOutline(MakeShareable(CopyOutline));
		break;
	}
	case 1:
	{
		InBoardShape->SetShapePosX(LeftHeight + LeftRetract);
		InBoardShape->SetShapePosY(BackHeight0 + BackRetract0);
		InBoardShape->SetShapePosZ(0.0);
		InBoardShape->SetShapeWidth(SpaceWidth - LeftHeight - LeftRetract - RightHeight0 - RightRetract0);
		InBoardShape->SetShapeDepth(SpaceDepth - BackHeight0 - BackRetract0 - FrontHeight - FrontRetract);
		//设置板件的轮廓信息
		FSCTOutline* CopyOutline = new FSCTOutline();
		Outline->CopyTo(CopyOutline);
		CopyOutline->SetOutlineParamValueByRefName(TEXT("PW"), PWidth - RightHeight0 - RightRetract0 + RightHeight1 + RightRetract1);
		CopyOutline->SetOutlineParamValueByRefName(TEXT("PD"), PDepth - BackHeight0 - BackRetract0 + BackHeight1 + BackRetract1);
		InBoardShape->SetBoardOutline(MakeShareable(CopyOutline));
		break;
	}
	case 2:
	{
		InBoardShape->SetShapePosX(CurHeight);
		InBoardShape->SetShapePosY(BackHeight0 + BackRetract0);
		InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
		InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - TopRetract - BottomHeight - BottomRetract);
		InBoardShape->SetShapeDepth(SpaceDepth - BackHeight0 - BackRetract0 - FrontHeight - FrontRetract);
		break;
	}
	case 3:
	{
		if (InIndex == 0)
		{
			InBoardShape->SetShapePosX(SpaceWidth);
			InBoardShape->SetShapePosY(PDepth + BackHeight1 + BackRetract1);
			InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
			InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - TopRetract - BottomHeight - BottomRetract);
			InBoardShape->SetShapeDepth(SpaceDepth - PDepth - BackHeight1 - BackRetract1 - FrontHeight - FrontRetract);
		}
		else if (InIndex == 1)
		{
			InBoardShape->SetShapePosX(SpaceWidth - PWidth);
			InBoardShape->SetShapePosY(BackHeight0 + BackRetract0);
			InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
			InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - TopRetract - BottomHeight - BottomRetract);
			InBoardShape->SetShapeDepth(PDepth - BackHeight0 - BackRetract0 + BackHeight1 + BackRetract1);
		}
		else
		{
			check(false);
		}
		break;
	}
	case 4:
	{
		if (0 == InIndex)
		{
			InBoardShape->SetShapePosX(LeftHeight + LeftRetract);
			InBoardShape->SetShapePosY(0.0);
			InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
			InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - TopRetract - BottomHeight - BottomRetract);
			InBoardShape->SetShapeDepth(SpaceWidth - PWidth - LeftHeight - LeftRetract - RightHeight1 - RightRetract1);
		}
		else if (1 == InIndex)
		{
			InBoardShape->SetShapePosX(SpaceWidth - PWidth - RightHeight1 - RightRetract1);
			InBoardShape->SetShapePosY(PDepth);
			InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
			InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - TopRetract - BottomHeight - BottomRetract);
			InBoardShape->SetShapeDepth(PWidth - RightHeight0 - RightRetract0 + RightHeight1 + RightRetract1);
		}
		else
		{
			check(false);
		}
		break;
	}
	case 5:
	{
		InBoardShape->SetShapePosX(LeftHeight + LeftRetract);
		InBoardShape->SetShapePosY(SpaceDepth - CurHeight);
		InBoardShape->SetShapePosZ(BottomHeight + BottomRetract);
		InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - TopRetract - BottomHeight - BottomRetract);
		InBoardShape->SetShapeDepth(SpaceWidth - LeftHeight - LeftRetract - RightHeight0 - RightRetract0);
		break;
	}
	default:
		check(false);
		break;
	}

	return true;
}

bool FFrameShape::CalFrameBoardPosDimenForMiddlePiller(TSharedPtr<FBoardShape> InBoardShape, int32 InType, int32 InIndex)
{
	//轮廓空间尺寸
	float SpaceWidth = SpaceShapeData->GetShapeWidth();
	float SpaceDepth = SpaceShapeData->GetShapeDepth();
	float SpaceHeight = SpaceShapeData->GetShapeHeight();

	//轮廓参数
	TSharedPtr<FSCTOutline> Outline = SpaceShapeData->GetSpaceOutline();
	float PPosX = Outline->GetOutlineParamValueByRefName(TEXT("PX"));
	float PWidth = Outline->GetOutlineParamValueByRefName(TEXT("PW"));
	float PDepth = Outline->GetOutlineParamValueByRefName(TEXT("PD"));

	//当前板件厚度
	float CurHeight = InBoardShape->GetShapeHeight();

	//框体板件厚度
	float TopHeight = 0.0;
	TSharedPtr<FBoardShape> TopBoard = GetFrameBoardShape(0, 0);
	if (TopBoard.IsValid() && IsBoardInSequence(TopBoard, InBoardShape))
	{
		TopHeight = TopBoard->GetShapeHeight();
	}
	float BottomHeight = 0.0;
	TSharedPtr<FBoardShape> BottomBoard = GetFrameBoardShape(1, 0);
	if (BottomBoard.IsValid() && IsBoardInSequence(BottomBoard, InBoardShape))
	{
		BottomHeight = BottomBoard->GetShapeHeight();
	}
	float LeftHeight0 = 0.0;
	TSharedPtr<FBoardShape> LeftBoard0 = GetFrameBoardShape(2, 0);
	if (LeftBoard0.IsValid() && IsBoardInSequence(LeftBoard0, InBoardShape))
	{
		LeftHeight0 = LeftBoard0->GetShapeHeight();
	}
	float LeftHeight1 = 0.0;
	TSharedPtr<FBoardShape> LeftBoard1 = GetFrameBoardShape(2, 1);
	if (LeftBoard1.IsValid() && IsBoardInSequence(LeftBoard1, InBoardShape))
	{
		LeftHeight1 = LeftBoard1->GetShapeHeight();
	}
	float RightHeight0 = 0.0;
	TSharedPtr<FBoardShape> RightBoard0 = GetFrameBoardShape(3, 0);
	if (RightBoard0.IsValid() && IsBoardInSequence(RightBoard0, InBoardShape))
	{
		RightHeight0 = RightBoard0->GetShapeHeight();
	}
	float RightHeight1 = 0.0;
	TSharedPtr<FBoardShape> RightBoard1 = GetFrameBoardShape(3, 1);
	if (RightBoard1.IsValid() && IsBoardInSequence(RightBoard1, InBoardShape))
	{
		RightHeight1 = RightBoard1->GetShapeHeight();
	}
	float BackHeight0 = 0.0;
	TSharedPtr<FBoardShape> BackBoard0 = GetFrameBoardShape(4, 0);
	if (BackBoard0.IsValid() && IsBoardInSequence(BackBoard0, InBoardShape))
	{
		BackHeight0 = BackBoard0->GetShapeHeight();
	}
	float BackHeight1 = 0.0;
	TSharedPtr<FBoardShape> BackBoard1 = GetFrameBoardShape(4, 1);
	if (BackBoard1.IsValid() && IsBoardInSequence(BackBoard1, InBoardShape))
	{
		BackHeight1 = BackBoard1->GetShapeHeight();
	}
	float BackHeight2 = 0.0;
	TSharedPtr<FBoardShape> BackBoard2 = GetFrameBoardShape(4, 2);
	if (BackBoard2.IsValid() && IsBoardInSequence(BackBoard2, InBoardShape))
	{
		BackHeight2 = BackBoard2->GetShapeHeight();
	}
	float FrontHeight = 0.0;
	TSharedPtr<FBoardShape> FrontBoard = GetFrameBoardShape(5, 0);
	if (FrontBoard.IsValid() && IsBoardInSequence(FrontBoard, InBoardShape))
	{
		FrontHeight = FrontBoard->GetShapeHeight();
	}

	//计算板件位置和尺寸
	switch (InType)
	{
	case 0:
	{
		InBoardShape->SetShapePosX(LeftHeight0);
		InBoardShape->SetShapePosY(BackHeight0);
		InBoardShape->SetShapePosZ(SpaceHeight - CurHeight);
		InBoardShape->SetShapeWidth(SpaceWidth - LeftHeight0 - RightHeight0);
		InBoardShape->SetShapeDepth(SpaceDepth - BackHeight0 - FrontHeight);
		//设置板件的轮廓信息
		FSCTOutline* CopyOutline = new FSCTOutline();
		Outline->CopyTo(CopyOutline);
		InBoardShape->SetBoardOutline(MakeShareable(CopyOutline));
		break;
	}
	case 1:
	{
		InBoardShape->SetShapePosX(LeftHeight0);
		InBoardShape->SetShapePosY(BackHeight0);
		InBoardShape->SetShapePosZ(0.0);
		InBoardShape->SetShapeWidth(SpaceWidth - LeftHeight0 - RightHeight0);
		InBoardShape->SetShapeDepth(SpaceDepth - BackHeight0 - FrontHeight);
		//设置板件的轮廓信息
		FSCTOutline* CopyOutline = new FSCTOutline();
		Outline->CopyTo(CopyOutline);
		InBoardShape->SetBoardOutline(MakeShareable(CopyOutline));
		break;
	}
	case 2:
	{
		if (0 == InIndex)
		{
			InBoardShape->SetShapePosX(CurHeight);
			InBoardShape->SetShapePosY(BackHeight0);
			InBoardShape->SetShapePosZ(BottomHeight);
			InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - BottomHeight);
			InBoardShape->SetShapeDepth(SpaceDepth - BackHeight0 - FrontHeight);
		}
		else if (1 == InIndex)
		{
			InBoardShape->SetShapePosX(PPosX + PWidth + CurHeight);
			InBoardShape->SetShapePosY(BackHeight1);
			InBoardShape->SetShapePosZ(BottomHeight);
			InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - BottomHeight);
			InBoardShape->SetShapeDepth(PDepth - BackHeight1 + BackHeight2);
		} 
		else
		{
			check(false);
		}
		break;
	}
	case 3:
	{
		if (InIndex == 0)
		{
			InBoardShape->SetShapePosX(SpaceWidth);
			InBoardShape->SetShapePosY(BackHeight1);
			InBoardShape->SetShapePosZ(BottomHeight);
			InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - BottomHeight);
			InBoardShape->SetShapeDepth(SpaceDepth - BackHeight1 - FrontHeight);
		}
		else if (InIndex == 1)
		{
			InBoardShape->SetShapePosX(PPosX);
			InBoardShape->SetShapePosY(BackHeight0);
			InBoardShape->SetShapePosZ(BottomHeight);
			InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - BottomHeight);
			InBoardShape->SetShapeDepth(PDepth - BackHeight0 + BackHeight2);
		}
		else
		{
			check(false);
		}
		break;
	}
	case 4:
	{
		if (0 == InIndex)
		{
			InBoardShape->SetShapePosX(LeftHeight0);
			InBoardShape->SetShapePosY(0.0);
			InBoardShape->SetShapePosZ(BottomHeight);
			InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - BottomHeight);
			InBoardShape->SetShapeDepth(PPosX - LeftHeight0 - RightHeight1);
		}
		else if (1 == InIndex)
		{
			InBoardShape->SetShapePosX(PPosX + PWidth + LeftHeight1);
			InBoardShape->SetShapePosY(0.0);
			InBoardShape->SetShapePosZ(BottomHeight);
			InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - BottomHeight);
			InBoardShape->SetShapeDepth(SpaceWidth - PPosX - PWidth - RightHeight0 - RightHeight1);
		}
		else if (2 == InIndex)
		{
			InBoardShape->SetShapePosX(PPosX - RightHeight1);
			InBoardShape->SetShapePosY(PDepth);
			InBoardShape->SetShapePosZ(BottomHeight);
			InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - BottomHeight);
			InBoardShape->SetShapeDepth(PWidth + LeftHeight1 + RightHeight1);
		}
		else
		{
			check(false);
		}
		break;
	}
	case 5:
	{
		InBoardShape->SetShapePosX(LeftHeight0);
		InBoardShape->SetShapePosY(SpaceDepth - CurHeight);
		InBoardShape->SetShapePosZ(BottomHeight);
		InBoardShape->SetShapeWidth(SpaceHeight - TopHeight - BottomHeight);
		InBoardShape->SetShapeDepth(SpaceWidth - LeftHeight0 - RightHeight0);
		break;
	}
	default:
		check(false);
		break;
	}

	return true;
}


