#include "SCTDoorGroup.h"
#include "SCTShape.h"
#include "SCTModelShape.h"
#include "SCTBoardShape.h"
#include "SCTShapeManager.h"
#include "SCTDoorSheet.h"
#include "Engine/EngineTypes.h"
#include "SCTAttribute.h"
#include "SCTShapeData.h"
#include "SCTDoorSheet.h"
#include "SCTShapeActor.h"
#include "Actor/SCTModelActor.h"
#include "SCTSpaceShape.h"
#include "SCTShapeBoxActor.h"
#include "Engine/CollisionProfile.h"
#include "SCTShapeData.h"


//门组基类
FDoorGroup::FDoorGroup()
{

}

FDoorGroup::~FDoorGroup()
{

}

void FDoorGroup::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseFromJson(InJsonObject);
	//外延值
	UpExtensionValue = InJsonObject->GetNumberField(TEXT("upExtension"));
	DownExtensionValue = InJsonObject->GetNumberField(TEXT("downExtension"));
	LeftExtensionValue = InJsonObject->GetNumberField(TEXT("leftExtension"));
	RightExtensionValue = InJsonObject->GetNumberField(TEXT("rightExtension"));
	//ProfileName
	//BoundBoxProfileName = InJsonObject->GetStringField(TEXT("profileName"));
	TArray<int32> ChildrenIds;
	if (InJsonObject->HasField(TEXT("shapes")))
	{
		const TArray<TSharedPtr<FJsonValue>> & TempShapes = InJsonObject->GetArrayField(TEXT("shapes"));
		for (const auto & Ref : TempShapes)
		{			
			const int32 FindIndex = FilterParseMetalType.Find(StaticCast<EMetalsType>(Ref->AsObject()->GetIntegerField(TEXT("category"))));
			if (FindIndex == INDEX_NONE) continue;
			TSharedPtr<FSCTShape> Shape = FSCTShapeManager::Get()->ParseShapeFromJson(Ref->AsObject());
			FSCTShapeManager::Get()->AddChildShape(Shape);
			int32 ID = Ref->AsObject()->GetIntegerField(TEXT("id"));
			OpetionsMetalsShapesList.Add(ID, Shape);
			if (StaticCast<EShapeType>(Ref->AsObject()->GetIntegerField(TEXT("type"))) != ST_Accessory) continue;
			if (ChildrenIds.FindByKey(ID) == nullptr)
			{
				FDoorSheetShapeBase::FAccessoryToDownload TempAccessoryToDownload;
				TempAccessoryToDownload.ModelID = Ref->AsObject()->GetIntegerField(TEXT("modelId"));
				TempAccessoryToDownload.Md5 = Ref->AsObject()->GetStringField(TEXT("fileMd5"));
				TempAccessoryToDownload.Url = Ref->AsObject()->GetStringField(TEXT("fileUrl"));
				TempAccessoryToDownload.UpdateTime = Ref->AsObject()->GetNumberField(TEXT("updateTime"));
				TempAccessoryToDownload.OptimizeParam = InJsonObject->GetStringField(TEXT("optimizeParam"));
				ToDownloadAccessoryList.Emplace(MoveTemp(TempAccessoryToDownload));
			}
		}
	}

	// 五金 
	if (InJsonObject->HasField(TEXT("children")))
	{
		const TArray<TSharedPtr<FJsonValue>> & TempChildren = InJsonObject->GetArrayField(TEXT("children"));
		for (const auto & Ref : TempChildren)
		{
			const TSharedPtr<FJsonObject> & Obj = Ref->AsObject();						
			const int32 ShapeId = Obj->GetIntegerField(TEXT("id"));
			if (OpetionsMetalsShapesList.Find(ShapeId) == nullptr)
			{
				continue;
			}
			TSharedPtr<FSCTShape> NewShape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ST_Accessory, ShapeId);
			TSharedPtr<FAccessoryShape> CopyShape = StaticCastSharedPtr<FAccessoryShape>(FSCTShapeManager::Get()->CopyShapeToNew(NewShape));
			AddMetal(CopyShape);
			CopyShape->ParseAttributesFromJson(Obj);
		}
	}
	ProcessOptionsMetalType();
}

void FDoorGroup::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseAttributesFromJson(InJsonObject);
	//外延值
	UpExtensionValue = InJsonObject->GetNumberField(TEXT("upExtension"));
	DownExtensionValue = InJsonObject->GetNumberField(TEXT("downExtension"));
	LeftExtensionValue = InJsonObject->GetNumberField(TEXT("leftExtension"));
	RightExtensionValue = InJsonObject->GetNumberField(TEXT("rightExtension"));
	//ProfileName
	//BoundBoxProfileName = InJsonObject->GetStringField(TEXT("profileName"));

	// 五金 
	if (InJsonObject->HasField(TEXT("children")))
	{
		const TArray<TSharedPtr<FJsonValue>> & TempChildren = InJsonObject->GetArrayField(TEXT("children"));
		for (const auto & Ref : TempChildren)
		{
			const TSharedPtr<FJsonObject> & Obj = Ref->AsObject();			
			const int32 ShapeId = Obj->GetIntegerField(TEXT("id"));
			if (OpetionsMetalsShapesList.Find(ShapeId) == nullptr || Obj->GetIntegerField(TEXT("type")) != ST_Accessory)
			{
				continue;
			}
			TSharedPtr<FSCTShape> NewShape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ST_Accessory, ShapeId);
			TSharedPtr<FAccessoryShape> CopyShape = StaticCastSharedPtr<FAccessoryShape>(FSCTShapeManager::Get()->CopyShapeToNew(NewShape));
			AddMetal(CopyShape);
			CopyShape->ParseAttributesFromJson(Obj);
		}
	}
	ProcessOptionsMetalType();

}

int32 FDoorGroup::GetOptionMetalsIdByNameAndType(const FString & InName, const EMetalsType InType)
{
	int32 RetValue = INDEX_NONE;
	for (const auto & Ref : OpetionsMetalsShapesList)
	{
		if (Ref.Value->GetShapeCategory() == StaticCast<int32>(InType) && Ref.Value->GetShapeName() == InName)
		{
			RetValue = Ref.Key;
			break;
		}
	}
	return 	RetValue;
}

bool FDoorGroup::IsContainSpecialMetal(const int32 InShapeId)
{
	return 	GetOptionShapeMap().Contains(InShapeId);
}

void FDoorGroup::AddMetal(const TSharedPtr<FAccessoryShape>& InShape)
{
	Metals.Add(InShape);
	AddChildShape(InShape);
	InShape->SetParentShape(this);
}

void FDoorGroup::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseShapeFromJson(InJsonObject);
	TArray<int32> ChildrenIds;
	if (InJsonObject->HasField(TEXT("shapes")))
	{
		const TArray<TSharedPtr<FJsonValue>> & TempShapes = InJsonObject->GetArrayField(TEXT("shapes"));
		for (const auto & Ref : TempShapes)
		{
			const int32 FindIndex = FilterParseMetalType.Find(StaticCast<EMetalsType>(Ref->AsObject()->GetIntegerField(TEXT("category"))));
			if (FindIndex == INDEX_NONE) continue;
			TSharedPtr<FSCTShape> Shape = FSCTShapeManager::Get()->ParseShapeFromJson(Ref->AsObject());
			FSCTShapeManager::Get()->AddChildShape(Shape);
			int32 ID = Ref->AsObject()->GetIntegerField(TEXT("id"));
			OpetionsMetalsShapesList.Add(ID, Shape);
			if (StaticCast<EShapeType>(Ref->AsObject()->GetIntegerField(TEXT("type"))) != ST_Accessory) continue;
			if (ChildrenIds.FindByKey(ID) == nullptr)
			{
				FDoorSheetShapeBase::FAccessoryToDownload TempAccessoryToDownload;
				TempAccessoryToDownload.ModelID = Ref->AsObject()->GetIntegerField(TEXT("modelId"));
				TempAccessoryToDownload.Md5 = Ref->AsObject()->GetStringField(TEXT("fileMd5"));
				TempAccessoryToDownload.Url = Ref->AsObject()->GetStringField(TEXT("fileUrl"));
				TempAccessoryToDownload.UpdateTime = Ref->AsObject()->GetNumberField(TEXT("updateTime"));
				TempAccessoryToDownload.OptimizeParam = InJsonObject->GetStringField(TEXT("optimizeParam"));
				ToDownloadAccessoryList.Emplace(MoveTemp(TempAccessoryToDownload));
			}
		}
	}
}

void FDoorGroup::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseContentFromJson(InJsonObject);
}

void FDoorGroup::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveToJson(JsonWriter);
	//外延值
	JsonWriter->WriteValue(TEXT("upExtension"), UpExtensionValue);
	JsonWriter->WriteValue(TEXT("downExtension"), DownExtensionValue);
	JsonWriter->WriteValue(TEXT("leftExtension"), LeftExtensionValue);
	JsonWriter->WriteValue(TEXT("rightExtension"), RightExtensionValue);
	
}

void FDoorGroup::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveAttriToJson(JsonWriter);
	//外延值
	JsonWriter->WriteValue(TEXT("upExtension"), UpExtensionValue);
	JsonWriter->WriteValue(TEXT("downExtension"), DownExtensionValue);
	JsonWriter->WriteValue(TEXT("leftExtension"), LeftExtensionValue);
	JsonWriter->WriteValue(TEXT("rightExtension"), RightExtensionValue);
}

void FDoorGroup::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveShapeToJson(JsonWriter);	
}

void FDoorGroup::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveContentToJson(JsonWriter);
}

void FDoorGroup::CopyTo(FSCTShape* OutShape)
{
	FSCTShape::CopyTo(OutShape);
	//门组信息
	FDoorGroup* OutDoorShape = StaticCast<FDoorGroup*>(OutShape);
	//外延值
	OutDoorShape->UpExtensionValue = UpExtensionValue;
	OutDoorShape->DownExtensionValue = DownExtensionValue;
	OutDoorShape->LeftExtensionValue = LeftExtensionValue;
	OutDoorShape->RightExtensionValue = RightExtensionValue;
	OutDoorShape->bIsDoorGroupHiddlen = bIsDoorGroupHiddlen;
	
	//ProfileName
	//OutDoorShape->BoundBoxProfileName = BoundBoxProfileName;
	OutDoorShape->OpetionsMetalsShapesList = OpetionsMetalsShapesList;
	OutDoorShape->ToDownloadAccessoryList = ToDownloadAccessoryList;
	for (const auto & Ref : Metals)
	{
		TSharedPtr<FAccessoryShape> NewShape = MakeShareable(new FAccessoryShape);
		Ref->CopyTo(NewShape.Get());
		OutDoorShape->AddMetal(NewShape);
	}
}

ASCTShapeActor* FDoorGroup::SpawnShapeActor()
{
	ASCTShapeActor* NewDoorGroupActor = FSCTShape::SpawnShapeActor();
	do	
	{
		if (!NewDoorGroupActor) break;
		// 放置所有当前正在使用的五金
		for (auto & Ref : Metals)
		{
			const int32 FindIndex = FilterSpawnMetalType.Find(StaticCast<EMetalsType>(Ref->GetShapeCategory()));
			if (FindIndex == INDEX_NONE) continue;
			Ref->LoadModelMesh();
			ASCTShapeActor * Actor = Ref->SpawnShapeActor();
			Actor->AttachToActorOverride(NewDoorGroupActor, FAttachmentTransformRules::KeepRelativeTransform);
		}

	} while (false);	
	return 	NewDoorGroupActor;
}

void FDoorGroup::SpawnActorsForSelected(FName InProfileName)
{
	BoundBoxProfileName = InProfileName;
	ASCTShapeActor* CurDoorActor = GetShapeActor();
	if (!CurDoorActor)
	{
		return;
	}
	//创建型录外包框
	ASCTWireframeActor* WireFrameActor = SpawnWireFrameActor();
	WireFrameActor->UpdateActorDimension();
	WireFrameActor->SetActorHiddenInGame(true);
	WireFrameActor->AttachToActor(CurDoorActor, FAttachmentTransformRules::KeepRelativeTransform);
	//生成模型的包围盒，用于鼠标点选
	ASCTBoundingBoxActor* BoundingBoxActor = SpawnBoundingBoxActor();
	BoundingBoxActor->SetCollisionProfileName(InProfileName);
	BoundingBoxActor->SetLineAlpha(0.0f);
	BoundingBoxActor->SetRegionAlpha(0.0f);
	BoundingBoxActor->UpdateActorDimension();
	BoundingBoxActor->AttachToActor(CurDoorActor, FAttachmentTransformRules::KeepRelativeTransform);
}

void FDoorGroup::SetCollisionProfileName(FName InProfileName)
{
	FSCTShape::SetCollisionProfileName(InProfileName);
	// 放置所有当前正在使用的五金
	for (auto & Ref : Metals)
	{
		const int32 FindIndex = FilterSpawnMetalType.Find(StaticCast<EMetalsType>(Ref->GetShapeCategory()));
		if (FindIndex == INDEX_NONE) continue;
		Ref->SetCollisionProfileName(InProfileName);
	}
}

void FDoorGroup::GetResourceUrls(TArray<FString> &OutResourceUrls)
{
	FSCTShape::GetResourceUrls(OutResourceUrls);
	for (const auto & Ref : ToDownloadAccessoryList)
	{
		if (Ref.Url.IsEmpty()) continue;
		OutResourceUrls.Add(Ref.Url);
	}	
}

void FDoorGroup::GetFileCachePaths(TArray<FString> &OutFileCachePaths)
{
	FSCTShape::GetFileCachePaths(OutFileCachePaths);
	for (const auto & Ref : ToDownloadAccessoryList)
	{
		if (Ref.Url.IsEmpty()) continue;
		OutFileCachePaths.Add(FDividBlockShape::GetCacheFilePathFromIDAndMd5AndUpdateTime(Ref.ModelID, Ref.Md5, Ref.UpdateTime));
	}
}

void FDoorGroup::ShowDoorSheetsActor()
{
	if (BoundingBoxActor)
	{
		BoundingBoxActor->SetCollisionProfileName(BoundBoxProfileName);
	}
}

void FDoorGroup::HideDoorSheetsActor()
{
	if (BoundingBoxActor)
	{
		BoundingBoxActor->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	}
}

void FDoorGroup::HiddenDoorGroupActors(bool bHidden)
{
	bIsDoorGroupHiddlen = bHidden;
}

float FDoorGroup::GetDoorTotalDepth()
{
	return GetShapeDepth();
}

float FDoorGroup::GetShapeWidthRange(float& MinValue, float& MaxValue)
{
	return 0.0;
}

float FDoorGroup::GetShapeDepthRange(float& MinValue, float& MaxValue)
{
	return 0.0;
}

float FDoorGroup::GetShapeHeightRange(float& MinValue, float& MaxValue)
{
	return 0.0;
}

bool FDoorGroup::IsValidForWidth(float InValue)
{
	return false;
}

bool FDoorGroup::IsValidForDepth(float InValue)
{
	return false;
}

bool FDoorGroup::IsValidForHeight(float InValue)
{
	return false;
}


//移门门组
FSlidingDoor::FSlidingDoor()
{
	SetShapeType(ST_SlidingDoor);
	SetShapeName(TEXT("移门门组"));
	SrcJsonStr = MakeShareable(new FString);
}

FSlidingDoor::~FSlidingDoor()
{
}

void FSlidingDoor::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{	
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> SaveJsonWriter =
		TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(SrcJsonStr.Get());	
	FJsonSerializer::Serialize(InJsonObject.ToSharedRef(), SaveJsonWriter);
	SaveJsonWriter->Close();

	//解析基类信息
	FDoorGroup::ParseFromJson(InJsonObject);
	if (Depth->GetAttributeType() == SAT_NumberRange)
	{
		TSharedPtr<FNumberRangeAttri> CurDepthRange = StaticCastSharedPtr<FNumberRangeAttri>(Depth);
		CurDepthRange->SetMaxValue(200.0);
	}

	//移门门组信息
	FirstPosition = InJsonObject->GetIntegerField(TEXT("firstDoorPosition"));
	InterleavingMode = InJsonObject->GetIntegerField(TEXT("doorInterleavingMode"));
	OverlapWidth = InJsonObject->GetIntegerField(TEXT("overlapWidth"));
	SlidewayPresetValue = InJsonObject->GetNumberField(TEXT("introskipValue"));
	CorverType = InJsonObject->GetIntegerField(TEXT("coverUpWay"));

	//上下滑轨
	//可替换上下滑轨
	const TArray<TSharedPtr<FJsonValue>> & TempShapes = InJsonObject->GetArrayField(TEXT("shapes"));
	for (const auto & Ref : TempShapes)
	{
		const TSharedPtr<FJsonObject> & SlidewayObj = Ref->AsObject();
		const int32 Category = SlidewayObj->GetIntegerField(TEXT("category"));
		const bool ToReserver = (Category == StaticCast<int32>(EMetalsType::MT_SD_TOP_RAILS)) ||
			(Category == StaticCast<int32>(EMetalsType::MT_SD_BOTTOM_RAILS));
		//只收集滑轨五金
		if (ToReserver == false) continue;
		TSharedPtr<FAccessoryShape> NewSlidway =  MakeShareable(new FAccessoryShape);
		NewSlidway->ParseShapeFromJson(SlidewayObj);
		AllValidSlideways.Add(NewSlidway);
	}
	if (InJsonObject->HasField(TEXT("slideWayGroup")))
	{
		const TArray<TSharedPtr<FJsonValue>> & ArrayRef = InJsonObject->GetArrayField(TEXT("slideWayGroup"));
		for (const auto & Ref : ArrayRef)
		{			
			const TSharedPtr<FJsonObject> & ObjRef = Ref->AsObject();
			int32 TopSlidewayIndex = ObjRef->GetIntegerField(TEXT("topSlideWay"));
			int32 BottomSlidewayIndex = ObjRef->GetIntegerField(TEXT("bottomSlideWay"));
			SlidWayGroups.Emplace(TPair<int32, int32>(TopSlidewayIndex, BottomSlidewayIndex));
		}
	}
	//当前默认上下滑轨
	const TArray<TSharedPtr<FJsonValue>> & TempChildren = InJsonObject->GetArrayField(TEXT("children"));	
	for (const auto & Ref : TempChildren)
	{
		const TSharedPtr<FJsonObject> & CurSlidwayObj = Ref->AsObject();
		int32 CurShapeId = CurSlidwayObj->GetIntegerField(TEXT("id"));
		// 如果不在收集到的上下滑轨列表中,则不做任何处理
		FAccessoryShape* CurAccessoryShape = GetSlidewayShapeByID(CurShapeId);
		if (CurAccessoryShape == nullptr)
		{
			continue;
		}
		FAccessoryShape* CurCopyAccessoryShape = new FAccessoryShape;
		CurAccessoryShape->CopyTo(CurCopyAccessoryShape);
		CurCopyAccessoryShape->ParseAttributesFromJson(CurSlidwayObj);
		if (CurCopyAccessoryShape->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SD_TOP_RAILS))
		{
			SetUpSlideway(MakeShareable(CurCopyAccessoryShape));
		}
		else if (CurCopyAccessoryShape->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SD_BOTTOM_RAILS))
		{
			SetDownSlideway(MakeShareable(CurCopyAccessoryShape));
		} 
		else
		{
			check(false);
		}
	}	

	//上下垫板
	const TArray<TSharedPtr<FJsonValue>> & PlateSettingsRef = InJsonObject->GetArrayField(TEXT("plateSettings"));
	check(PlateSettingsRef.Num() == 2);
	const TSharedPtr<FJsonObject> & UpPlateBoardObj = PlateSettingsRef[0]->AsObject();
	FBoardShape* CurUpPlateBoard = ParsePlateBoardShape(UpPlateBoardObj);
	SetUpPlateBoard(MakeShareable(CurUpPlateBoard));
	const TSharedPtr<FJsonObject> & DownPlateBoardObj = PlateSettingsRef[1]->AsObject();
	FBoardShape* CurDownPlateBoard = ParsePlateBoardShape(DownPlateBoardObj);
	SetDownPlateBoard(MakeShareable(CurDownPlateBoard));

	//移门门板
	const TArray<TSharedPtr<FJsonValue>> & DoorsRef = InJsonObject->GetArrayField(TEXT("doorGoops"));
	for (const auto & Ref : DoorsRef)
	{
		TSharedPtr<FJsonObject> DoorSheetObject = Ref->AsObject();
		FSpaceDividDoorSheet* NewDoorSheet = new FSpaceDividDoorSheet;
		NewDoorSheet->ParseFromJson(DoorSheetObject);
		InsertDoorSheet(MakeShareable(NewDoorSheet));
	}
	OriginDoorCount = DoorsRef.Num();
}

void FSlidingDoor::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//型录类型和Id
	int32 CurrentType = InJsonObject->GetIntegerField(TEXT("type"));
	int32 CurrentId = InJsonObject->GetIntegerField(TEXT("id"));
	check(CurrentType == ShapeType && CurrentId == ShapeId);

	//更新型录基本信息
	FDoorGroup::ParseAttributesFromJson(InJsonObject);

	//门板定义数量
	OriginDoorCount = InJsonObject->GetNumberField(TEXT("originDoorCount"));
	//上下垫板是否使用
	bUpPlateBoardValid = InJsonObject->GetBoolField(TEXT("upPlateBoardValid"));
	bDownPlateBoardValid = InJsonObject->GetBoolField(TEXT("downPlateBoardValid"));
	//首扇门板位置
	FirstPosition = InJsonObject->GetIntegerField(TEXT("firstPosition"));
	//门板交错方式
	InterleavingMode = InJsonObject->GetIntegerField(TEXT("interleavingMode"));
	//重叠位宽度
	OverlapWidth = InJsonObject->GetNumberField(TEXT("overlapWidth"));
	//轨道预设值
	SlidewayPresetValue = InJsonObject->GetNumberField(TEXT("slidewayPresetValue"));
	//掩盖方式
	CorverType = InJsonObject->GetIntegerField(TEXT("corverType"));
	// 期望的门板数量
	if (InJsonObject->HasTypedField<EJson::Number>(TEXT("expectDoorSheetNum")))
	{
		ExpectDoorSheetNum = InJsonObject->GetIntegerField(TEXT("expectDoorSheetNum"));
	}

	//更新子级型录信息
	const TArray<TSharedPtr<FJsonValue>>* ChildList = nullptr;
	InJsonObject->TryGetArrayField(TEXT("children"), ChildList);
	if (ChildList != nullptr && (*ChildList).Num() > 0)
	{
		for (auto& It : *ChildList)
		{
			TSharedPtr<FJsonObject> ChildObject = It->AsObject();
			int32 ShapeType = ChildObject->GetNumberField(TEXT("type"));
			int64 ShapeId = ChildObject->GetNumberField(TEXT("id"));
			TSharedPtr<FSCTShape> NewShape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ShapeType, ShapeId);
			TSharedPtr<FSCTShape> CopyShape = FSCTShapeManager::Get()->CopyShapeToNew(NewShape);
			//根据型录类型添加子级对象
			EShapeType CurShapeType = CopyShape->GetShapeType();
			switch (CurShapeType)
			{
			case ST_Board:  //上下垫板
			{
				TSharedPtr<FBoardShape> CurBoardShape = StaticCastSharedPtr<FBoardShape>(CopyShape);
				//解析子级对象
				CopyShape->ParseAttributesFromJson(ChildObject);
				if (!UpPlateBoard.IsValid())
				{
					SetUpPlateBoard(CurBoardShape);
				}
				else
				{
					SetDownPlateBoard(CurBoardShape);
				}
				break;
			}
			case ST_Accessory:  //上下滑轨
			{
				TSharedPtr<FAccessoryShape> CurSlideway = StaticCastSharedPtr<FAccessoryShape>(CopyShape);
				int32 SlidewayType = CurSlideway->GetShapeCategory();
				if (SlidewayType == StaticCast<int32>(EMetalsType::MT_SD_TOP_RAILS))
				{
					SetUpSlideway(CurSlideway);
				}
				else if (SlidewayType == StaticCast<int32>(EMetalsType::MT_SD_BOTTOM_RAILS))
				{
					SetDownSlideway(CurSlideway);
				}
				else
				{
					check(false);
				}
				//解析子级对象
				CopyShape->ParseAttributesFromJson(ChildObject);
				break;
			}
			case ST_SpaceDividDoorSheet:  //移门门板
			{
				TSharedPtr<FSpaceDividDoorSheet> CurDoorSheet = StaticCastSharedPtr<FSpaceDividDoorSheet>(CopyShape);
				InsertDoorSheet(CurDoorSheet);
				//解析子级对象
				CopyShape->ParseAttributesFromJson(ChildObject);
				break;
			}
			default:
				check(false);
				break;
			}
		}
	}
}

void FSlidingDoor::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FDoorGroup::ParseShapeFromJson(InJsonObject);
	//可替换滑轨组
	if (InJsonObject->HasField(TEXT("slideWayGroup")))
	{
		const TArray<TSharedPtr<FJsonValue>> & ArrayRef = InJsonObject->GetArrayField(TEXT("slideWayGroup"));
		for (const auto & Ref : ArrayRef)
		{
			const TSharedPtr<FJsonObject> & ObjRef = Ref->AsObject();
			int32 TopSlidewayIndex = ObjRef->GetIntegerField(TEXT("topSlideWay"));
			int32 BottomSlidewayIndex = ObjRef->GetIntegerField(TEXT("bottomSlideWay"));
			SlidWayGroups.Emplace(TPair<int32, int32>(TopSlidewayIndex, BottomSlidewayIndex));
		}
	}
	//
	if (InJsonObject->HasField(TEXT("allValidSlideways")))
	{
		const TArray<TSharedPtr<FJsonValue>> & ArrayRef = InJsonObject->GetArrayField(TEXT("allValidSlideways"));
		for (const auto & Ref : ArrayRef)
		{
			const TSharedPtr<FJsonObject> & ObjRef = Ref->AsObject();
			FAccessoryShape* NewSlideway = new FAccessoryShape;
			NewSlideway->ParseShapeFromJson(ObjRef);
			AllValidSlideways.Add(MakeShareable(NewSlideway));
		}
	}
}

void FSlidingDoor::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FDoorGroup::ParseContentFromJson(InJsonObject);
}

void FSlidingDoor::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	JsonWriter->WriteRawJSONValue(*(SrcJsonStr.Get()));
}

void FSlidingDoor::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//型录基本信息
	FDoorGroup::SaveAttriToJson(JsonWriter);

	//门组定义门板数量
	JsonWriter->WriteValue(TEXT("originDoorCount"), OriginDoorCount);
	//上下垫板是否使用
	JsonWriter->WriteValue(TEXT("upPlateBoardValid"), bUpPlateBoardValid);
	JsonWriter->WriteValue(TEXT("downPlateBoardValid"), bDownPlateBoardValid);
	//首扇门板位置
	JsonWriter->WriteValue(TEXT("firstPosition"), FirstPosition);
	//门板交错方式
	JsonWriter->WriteValue(TEXT("interleavingMode"), InterleavingMode);
	//重叠位宽度
	JsonWriter->WriteValue(TEXT("overlapWidth"), OverlapWidth);
	//滑轨预设值
	JsonWriter->WriteValue(TEXT("slidewayPresetValue"), SlidewayPresetValue);
	//掩盖方式
	JsonWriter->WriteValue(TEXT("corverType"), CorverType);
	// 期望的门板数量
	if (IsExpectDoorSheetNumValid())
	{		
		JsonWriter->WriteValue(TEXT("expectDoorSheetNum"), GetExpectDoorSheetNum());
	}

	//子型录信息
	JsonWriter->WriteArrayStart(TEXT("children"));
	//上下垫板
	if (UpPlateBoard.IsValid())
	{
		JsonWriter->WriteObjectStart();
		UpPlateBoard->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	if (DownPlateBoard.IsValid())
	{
		JsonWriter->WriteObjectStart();
		DownPlateBoard->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//上下滑轨
	if (UpSlideWay.IsValid())
	{
		JsonWriter->WriteObjectStart();
		UpSlideWay->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	if (DownSlideWay.IsValid())
	{
		JsonWriter->WriteObjectStart();
		DownSlideWay->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//移门门板
	for (int32 i=0; i<DoorSheets.Num(); ++i)
	{
		JsonWriter->WriteObjectStart();
		DoorSheets[i]->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
}

void FSlidingDoor::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FDoorGroup::SaveShapeToJson(JsonWriter);

	//可替换滑轨组
	JsonWriter->WriteArrayStart(TEXT("slideWayGroup"));
	for (int32 i = 0; i < SlidWayGroups.Num(); ++i)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue(TEXT("topSlideWay"), SlidWayGroups[i].Key);
		JsonWriter->WriteValue(TEXT("bottomSlideWay"), SlidWayGroups[i].Value);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
	//
	JsonWriter->WriteArrayStart(TEXT("allValidSlideways"));
	for (int32 i=0; i<AllValidSlideways.Num(); ++i)
	{
		JsonWriter->WriteObjectStart();
		AllValidSlideways[i]->SaveShapeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
}

void FSlidingDoor::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FDoorGroup::SaveContentToJson(JsonWriter);
}

void FSlidingDoor::CopyTo(FSCTShape* OutShape)
{
	//基类操作
	FDoorGroup::CopyTo(OutShape);

	//移门门组类型
	FSlidingDoor* OutSlidingDoor = StaticCast<FSlidingDoor*>(OutShape);

	OutSlidingDoor->OriginDoorCount = OriginDoorCount;
	OutSlidingDoor->bUpPlateBoardValid = bUpPlateBoardValid;
	OutSlidingDoor->bDownPlateBoardValid = bDownPlateBoardValid;
	OutSlidingDoor->FirstPosition = FirstPosition;
	OutSlidingDoor->InterleavingMode = InterleavingMode;
	OutSlidingDoor->OverlapWidth = OverlapWidth;
	OutSlidingDoor->SlidewayPresetValue = SlidewayPresetValue;
	OutSlidingDoor->CorverType = CorverType;
	OutSlidingDoor->SrcJsonStr = SrcJsonStr;
	OutSlidingDoor->AvaliableDoorSheetNum = AvaliableDoorSheetNum;
	OutSlidingDoor->ExpectDoorSheetNum = ExpectDoorSheetNum;

	//上下垫板
	if (UpPlateBoard.IsValid())
	{
		TSharedPtr<FSCTShape> NewUpBoard = FSCTShapeManager::Get()->CopyShapeToNew(UpPlateBoard);
		TSharedPtr<FBoardShape> NewUpPlateBoard = StaticCastSharedPtr<FBoardShape>(NewUpBoard);
		OutSlidingDoor->SetUpPlateBoard(NewUpPlateBoard);
	}

	if (DownPlateBoard.IsValid())
	{
		TSharedPtr<FSCTShape> NewDownBoard = FSCTShapeManager::Get()->CopyShapeToNew(DownPlateBoard);
		TSharedPtr<FBoardShape> NewDownPlateBoard = StaticCastSharedPtr<FBoardShape>(NewDownBoard);
		OutSlidingDoor->SetDownPlateBoard(NewDownPlateBoard);
	}

	//上下滑轨
	if (UpSlideWay.IsValid())
	{
		TSharedPtr<FSCTShape> NewUpAccessory = FSCTShapeManager::Get()->CopyShapeToNew(UpSlideWay);
		TSharedPtr<FAccessoryShape> NewUpSlideway = StaticCastSharedPtr<FAccessoryShape>(NewUpAccessory);
		OutSlidingDoor->SetUpSlideway(NewUpSlideway);
	}

	if (DownSlideWay.IsValid())
	{
		TSharedPtr<FSCTShape> NewDownAccessory = FSCTShapeManager::Get()->CopyShapeToNew(DownSlideWay);
		TSharedPtr<FAccessoryShape> NewDownSlideway = StaticCastSharedPtr<FAccessoryShape>(NewDownAccessory);
		OutSlidingDoor->SetDownSlideway(NewDownSlideway);
	}

	//移门门组
	for (int32 i = 0; i < DoorSheets.Num(); ++i)
	{
		TSharedPtr<FSCTShape> NewShape = FSCTShapeManager::Get()->CopyShapeToNew(DoorSheets[i]);
		TSharedPtr<FSpaceDividDoorSheet> NewDoorSheet = StaticCastSharedPtr<FSpaceDividDoorSheet>(NewShape);
		OutSlidingDoor->InsertDoorSheet(NewDoorSheet);
	}

	//可替换滑轨组
	OutSlidingDoor->SlidWayGroups = this->SlidWayGroups;
	for (int32 i=0; i<AllValidSlideways.Num(); ++i)
	{
		TSharedPtr<FAccessoryShape> NewSlideway = MakeShareable(new FAccessoryShape);
		AllValidSlideways[i]->CopyTo(NewSlideway.Get());
		OutSlidingDoor->AllValidSlideways.Add(NewSlideway);
	}
}

ASCTShapeActor* FSlidingDoor::SpawnShapeActor()
{
	//创建移门门组型录
	ASCTShapeActor* NewDoorGroupActor = FDoorGroup::SpawnShapeActor();

	//上下垫板
	if (UpPlateBoard.IsValid())
	{
		ASCTShapeActor* UpPlateActor = UpPlateBoard->SpawnShapeActor();
		UpPlateActor->AttachToActorOverride(NewDoorGroupActor, FAttachmentTransformRules::KeepRelativeTransform);
		SetUpPlateBoardValid(bUpPlateBoardValid);
	}
	if (DownPlateBoard.IsValid())
	{
		ASCTShapeActor* DownPlateActor = DownPlateBoard->SpawnShapeActor();
		DownPlateActor->AttachToActorOverride(NewDoorGroupActor, FAttachmentTransformRules::KeepRelativeTransform);
		SetDownPlateBoardValid(bDownPlateBoardValid);
	}

	//上下滑轨
	if (UpSlideWay.IsValid())
	{
		UpSlideWay->LoadModelMesh();
		ASCTShapeActor* UpSlideActor = UpSlideWay->SpawnShapeActor();
		UpSlideActor->AttachToActorOverride(NewDoorGroupActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	if (DownSlideWay.IsValid())
	{
		DownSlideWay->LoadModelMesh();
		ASCTShapeActor* DownSlideActor = DownSlideWay->SpawnShapeActor();
		DownSlideActor->AttachToActorOverride(NewDoorGroupActor, FAttachmentTransformRules::KeepRelativeTransform);
	}

	//门板
	for (int32 i = 0; i < DoorSheets.Num(); ++i)
	{
		if (DoorSheets[i]->GetShapeActor() != nullptr) continue;
		ASCTShapeActor* NewDoorSheetActor = DoorSheets[i]->SpawnShapeActor();
		NewDoorSheetActor->AttachToActorOverride(NewDoorGroupActor, FAttachmentTransformRules::KeepRelativeTransform);
	}	
	return NewDoorGroupActor;
}

void FSlidingDoor::SetCollisionProfileName(FName InProfileName)
{
	FDoorGroup::SetCollisionProfileName(InProfileName);
	//上下垫板
	UpPlateBoard->SetCollisionProfileName(InProfileName);
	DownPlateBoard->SetCollisionProfileName(InProfileName);
	//上下滑轨
	if (UpSlideWay.IsValid())
	{
		UpSlideWay->SetCollisionProfileName(InProfileName);
	}
	if (DownSlideWay.IsValid())
	{
		DownSlideWay->SetCollisionProfileName(InProfileName);
	}
	//门板
	for (int32 i = 0; i < DoorSheets.Num(); ++i)
	{
		DoorSheets[i]->SetCollisionProfileName(InProfileName);
	}
}

void FSlidingDoor::GetResourceUrls(TArray<FString> &OutResourceUrls)
{
	FDoorGroup::GetResourceUrls(OutResourceUrls);
	//上下垫板
	UpPlateBoard->GetResourceUrls(OutResourceUrls);
	DownPlateBoard->GetResourceUrls(OutResourceUrls);
	//上下滑轨
	if (UpSlideWay.IsValid())
	{
		UpSlideWay->GetResourceUrls(OutResourceUrls);
	}
	if (DownSlideWay.IsValid())
	{
		DownSlideWay->GetResourceUrls(OutResourceUrls);
	}
	//门板
	for (int32 i=0; i<DoorSheets.Num(); ++i)
	{
		DoorSheets[i]->GetResourceUrls(OutResourceUrls);
	}
	//可替换滑轨组
	for (const auto & Ref : AllValidSlideways)
	{
		Ref->GetResourceUrls(OutResourceUrls);
	}
}

void FSlidingDoor::GetFileCachePaths(TArray<FString> &OutFileCachePaths)
{
	FDoorGroup::GetFileCachePaths(OutFileCachePaths);
	//上下垫板
	UpPlateBoard->GetFileCachePaths(OutFileCachePaths);
	DownPlateBoard->GetFileCachePaths(OutFileCachePaths);
	//上下滑轨
	if (UpSlideWay.IsValid())
	{
		UpSlideWay->GetFileCachePaths(OutFileCachePaths);
	}
	if (DownSlideWay.IsValid())
	{
		DownSlideWay->GetFileCachePaths(OutFileCachePaths);
	}
	//门板
	for (int32 i = 0; i < DoorSheets.Num(); ++i)
	{
		DoorSheets[i]->GetFileCachePaths(OutFileCachePaths);
	}
	//可替换滑轨组
	for (const auto & Ref : AllValidSlideways)
	{
		Ref->GetFileCachePaths(OutFileCachePaths);
	}
}

void FSlidingDoor::ShowDoorSheetsActor()
{
	//控制BoundingBoxActor
	FDoorGroup::ShowDoorSheetsActor();
	//空间门板Actor
	for (int32 i = 0; i < DoorSheets.Num(); ++i)
	{
		ASCTShapeActor* DoorSheetActor = DoorSheets[i]->GetShapeActor();
		if (DoorSheetActor)
		{
			DoorSheetActor->SetShapeActorHiddenInGameRecursively(false);
		}
	}
}

void FSlidingDoor::HideDoorSheetsActor()
{
	//控制BoundingBoxActor
	FDoorGroup::HideDoorSheetsActor();
	//空间门板Actor
	for (int32 i = 0; i < DoorSheets.Num(); ++i)
	{
		ASCTShapeActor* DoorSheetActor = DoorSheets[i]->GetShapeActor();
		if (DoorSheetActor)
		{
			DoorSheetActor->SetShapeActorHiddenInGameRecursively(true);
		}
	}
}

void FSlidingDoor::HiddenDoorGroupActors(bool bHidden)
{
	FDoorGroup::HiddenDoorGroupActors(bHidden);
	//上下垫板
	if (bUpPlateBoardValid && UpPlateBoard.IsValid())
	{
		ASCTShapeActor* CurBoardActor = UpPlateBoard->GetShapeActor();
		if (CurBoardActor)
		{
			CurBoardActor->SetActorHiddenInGame(bHidden);
		}
	}
	if (bUpPlateBoardValid && DownPlateBoard.IsValid())
	{
		ASCTShapeActor* CurBoardActor = DownPlateBoard->GetShapeActor();
		if (CurBoardActor)
		{
			CurBoardActor->SetActorHiddenInGame(bHidden);
		}
	}
	//上下滑轨
	if (UpSlideWay.IsValid())
	{
		ASCTShapeActor* CurBoardActor = UpSlideWay->GetShapeActor();
		if (CurBoardActor)
		{
			CurBoardActor->SetActorHiddenInGame(bHidden);
		}
	}
	if (DownSlideWay.IsValid())
	{
		ASCTShapeActor* CurBoardActor = DownSlideWay->GetShapeActor();
		if (CurBoardActor)
		{
			CurBoardActor->SetActorHiddenInGame(bHidden);
		}
	}
	//空间门板Actor
	for (int32 i = 0; i < DoorSheets.Num(); ++i)
	{
		ASCTShapeActor* DoorSheetActor = DoorSheets[i]->GetShapeActor();
		if (DoorSheetActor)
		{
			DoorSheetActor->SetShapeActorHiddenInGameRecursively(bHidden);
		}
	}
}

bool FSlidingDoor::GetDoorGoupIsHIdden() const
{
	check(const_cast<FSlidingDoor*>(this)->GetDoorSheets().Num() > 0);
	if (const_cast<FSlidingDoor*>(this)->GetDoorSheets()[0]->GetShapeActor())
	{
		return const_cast<FSlidingDoor*>(this)->GetDoorSheets()[0]->GetShapeActor()->bHidden;
	}
	else
	{
		return FDoorGroup::GetDoorGoupIsHIdden();
	}		
}

bool FSlidingDoor::SetShapeWidth(float InIntValue)
{
	return SetShapeWidth(FString::Printf(TEXT("%f"), InIntValue));
}

bool FSlidingDoor::SetShapeWidth(const FString &InStrValue)
{
	bool bResult = true;
	if (Width->GetAttributeStr() != InStrValue)
	{
		bResult = Width->SetAttributeValue(InStrValue);
		if (bResult)
		{
			UpdateDoorGroup();
			//
			if (BoundingBoxActor)
			{
				BoundingBoxActor->UpdateActorDimension();
			}
			if (WireFrameActor)
			{
				WireFrameActor->UpdateActorDimension();
			}
		}
	}
	return bResult;
}

bool FSlidingDoor::SetShapeDepth(float InIntValue)
{
	return SetShapeDepth(FString::Printf(TEXT("%f"), InIntValue));
}

bool FSlidingDoor::SetShapeDepth(const FString &InStrValue)
{
	bool bResult = true;
	if (Depth->GetAttributeStr() != InStrValue)
	{
		bResult = Depth->SetAttributeValue(InStrValue);
		if (bResult)
		{
			UpdateDoorGroup();
			//
			if (BoundingBoxActor)
			{
				BoundingBoxActor->UpdateActorDimension();
			}
			if (WireFrameActor)
			{
				WireFrameActor->UpdateActorDimension();
			}
		}
	}
	return bResult;
}

bool FSlidingDoor::SetShapeHeight(float InIntValue)
{
	return SetShapeHeight(FString::Printf(TEXT("%f"), InIntValue));
}

bool FSlidingDoor::SetShapeHeight(const FString &InStrValue)
{
	bool bResult = true;
	if (Height->GetAttributeStr() != InStrValue)
	{
		bResult = Height->SetAttributeValue(InStrValue);
		if (bResult)
		{
			UpdateDoorGroup();
			//
			if (BoundingBoxActor)
			{
				BoundingBoxActor->UpdateActorDimension();
			}
			if (WireFrameActor)
			{
				WireFrameActor->UpdateActorDimension();
			}
		}
	}
	return bResult;
}

float FSlidingDoor::GetDoorTotalDepth()
{
	return SlidewayPresetValue;
}

float FSlidingDoor::GetShapeWidthRange(float& MinValue, float& MaxValue)
{
	//单门板尺寸范围
	float MinDoorSheetWidth = 0.0;
	float MaxDoorSheetWidth = 0.0;
	float CurDoorSheetWidth = DoorSheets[0]->GetShapeWidth();
	TSharedPtr<FShapeAttribute> DoorWidthAttri = DoorSheets[0]->GetShapeWidthAttri();
	if (SAT_NumberRange == DoorWidthAttri->GetAttributeType())
	{
		TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(DoorWidthAttri);
		MinDoorSheetWidth = NumberRangeAttri->GetMinValue();
		MaxDoorSheetWidth = NumberRangeAttri->GetMaxValue();
	}
	else
	{
		MinDoorSheetWidth = CurDoorSheetWidth;
		MaxDoorSheetWidth = CurDoorSheetWidth;
	}

	//门板数量
	int32 DoorSheetCount = DoorSheets.Num();
	//重叠位总宽度
	float TotalOverlapWidth = OverlapWidth * (DoorSheetCount - 1);
	if (InterleavingMode == 2)  //对称交错的情况
	{
		//TODO：对称交错情况进行修正
		if (DoorSheetCount > 3)
		{
			TotalOverlapWidth -= OverlapWidth;
		}
	}

	//计算门组的尺寸范围
	MinValue = MinDoorSheetWidth * DoorSheetCount - TotalOverlapWidth;
	MaxValue = MaxDoorSheetWidth * DoorSheetCount - TotalOverlapWidth;


	//门组当前值
	return GetShapeWidth();
}

float FSlidingDoor::GetShapeDepthRange(float& MinValue, float& MaxValue)
{
	//注意：移门门组的深度等于轨道预设值
	//尺寸变化范围为固定值

	//计算门组的尺寸范围
	MinValue = SlidewayPresetValue;
	MaxValue = SlidewayPresetValue;

	//门组当前值
	return GetShapeDepth();
}

float FSlidingDoor::GetShapeHeightRange(float& MinValue, float& MaxValue)
{
	//单门板尺寸范围
	float MinDoorSheetHeight = 0.0;
	float MaxDoorSheetHeight = 0.0;
	float CurDoorSheetHeight = DoorSheets[0]->GetShapeHeight();
	TSharedPtr<FShapeAttribute> DoorHeightAttri = DoorSheets[0]->GetShapeHeightAttri();
	if (SAT_NumberRange == DoorHeightAttri->GetAttributeType())
	{
		TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(DoorHeightAttri);
		MinDoorSheetHeight = NumberRangeAttri->GetMinValue();
		MaxDoorSheetHeight = NumberRangeAttri->GetMaxValue();
	}
	else
	{
		MinDoorSheetHeight = CurDoorSheetHeight;
		MaxDoorSheetHeight = CurDoorSheetHeight;
	}

	//上下垫板修正
	float CurUpPlateHeight = 0.0;
	if (bUpPlateBoardValid && UpPlateBoard.IsValid())
	{
		CurUpPlateHeight = UpPlateHeight;
	}
	float CurDownPlateHeight = 0.0;
	if (bDownPlateBoardValid && DownPlateBoard.IsValid())
	{
		CurDownPlateHeight = DownPlateHeight;
	}
	//上下滑轨修正

	//计算门组的尺寸范围
	MinValue = MinDoorSheetHeight + CurUpPlateHeight + UpSlideMeasure;
	MaxValue = MaxDoorSheetHeight + CurUpPlateHeight + UpSlideMeasure;

	//门组当前值
	return GetShapeHeight();
}

bool FSlidingDoor::IsValidForWidth(float InValue)
{
	//单门板尺寸范围
	float MinDoorSheetWidth = 0.0;
	float MaxDoorSheetWidth = 0.0;
	float CurDoorSheetWidth = DoorSheets[0]->GetShapeWidth();
	TSharedPtr<FShapeAttribute> DoorWidthAttri = DoorSheets[0]->GetShapeWidthAttri();
	if (SAT_NumberRange == DoorWidthAttri->GetAttributeType())
	{
		TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(DoorWidthAttri);
		MinDoorSheetWidth = NumberRangeAttri->GetMinValue();
		MaxDoorSheetWidth = NumberRangeAttri->GetMaxValue();
	}
	else
	{
		MinDoorSheetWidth = CurDoorSheetWidth;
		MaxDoorSheetWidth = CurDoorSheetWidth;
	}

	//门板数量
	int32 DoorSheetCount = 2;
	while (DoorSheetCount < 7)
	{
		//重叠位总宽度
		float TotalOverlapWidth = OverlapWidth * (DoorSheetCount - 1);
		if (InterleavingMode == 2)  //对称交错的情况
		{
			//TODO：对称交错情况进行修正
			if (DoorSheetCount > 3)
			{
				TotalOverlapWidth -= OverlapWidth;
			}
		}

		//计算门组的尺寸范围
		float MinGroupValue = MinDoorSheetWidth * DoorSheetCount - TotalOverlapWidth;
		float MaxGroupValue = MaxDoorSheetWidth * DoorSheetCount - TotalOverlapWidth;
		if (InValue >= MinGroupValue && InValue <= MaxGroupValue)
		{
			return true;
		}
		++DoorSheetCount;
	}
	return false;
}

bool FSlidingDoor::IsValidForDepth(float InValue)
{
	float MinDepth = 0.0;
	float MaxDepth = 0.0;
	float CurDepth = GetShapeDepthRange(MinDepth, MaxDepth);
	if (InValue >= MinDepth && InValue <= MaxDepth)
	{
		return true;
	}
	return false;
}

bool FSlidingDoor::IsValidForHeight(float InValue)
{
	float MinHeight = 0.0;
	float MaxHeight = 0.0;
	float CurHeight = GetShapeHeightRange(MinHeight, MaxHeight);
	if (InValue >= MinHeight && InValue <= MaxHeight)
	{
		return true;
	}
	return false;
}

void FSlidingDoor::SetUpExtensionValue(float InValue)
{

}

void FSlidingDoor::SetDownExtensionValue(float InValue)
{

}

void FSlidingDoor::SetLeftExtensionValue(float InValue)
{

}

void FSlidingDoor::SetRightExtensionValue(float InValue)
{

}

int32 FSlidingDoor::GetFirstPosition() const
{
	return FirstPosition;
}

void FSlidingDoor::SetFirstPosition(const int32 InPosition)
{
	FirstPosition = InPosition;
	//
	UpdateDoorGroup();
}

int32 FSlidingDoor::GetInterleavingMode() const
{
	return InterleavingMode;
}

void FSlidingDoor::SetInterleavintMode(const int32 InMode)
{
	InterleavingMode = InMode;
	//
	UpdateDoorGroup();
}

void FSlidingDoor::SetOverlapWidth(const int32 InOverlapWidth)
{
	OverlapWidth = InOverlapWidth;
	//
	UpdateDoorGroup();
}

void FSlidingDoor::SetUpSlideway(TSharedPtr<FAccessoryShape> InSlideway)
{
	UpSlideWay = InSlideway;
	UpSlideWay->SetParentShape(this);
	AddChildShape(UpSlideWay);
	//计算滑轨减尺以及深度和高度
	UpSlideMeasure = UpSlideWay->GetShapeHeight();
	UpSlideDepth = UpSlideWay->GetShapeDepth();
	UpSlideHeight = UpSlideWay->GetShapeHeight();
}

void FSlidingDoor::SetDownSlideway(TSharedPtr<FAccessoryShape> InSlideway)
{
	DownSlideWay = InSlideway;
	DownSlideWay->SetParentShape(this);
	AddChildShape(DownSlideWay);
	//计算滑轨减尺以及深度和高度
	DownSlideMeasure = DownSlideWay->GetShapeHeight();
	DownSlideDepth = DownSlideWay->GetShapeDepth();
	DownSlideHeight = DownSlideWay->GetShapeHeight();
}

float FSlidingDoor::GetUpSlideMeasure()
{
	return UpSlideMeasure;
}

float FSlidingDoor::GetDownSlideMeasure()
{
	return DownSlideMeasure;
}

bool FSlidingDoor::ChangeSlidwayByTopSlidwayName(const FString & InTopSlidwayName)
{
	bool RetValue = false;
	do
	{		
		int32 TopSlidWayID = -1;
		for (const auto & Ref : AllValidSlideways)
		{
			if (Ref->GetShapeName() == InTopSlidwayName)
			{
				TopSlidWayID = Ref->GetShapeId();
				break;
			}
		}
		RetValue = TopSlidWayID != -1;
		if (RetValue == false) break;
		int32 BottomSlidWayId = -1;
		for (const auto & Ref : SlidWayGroups)
		{
			if (Ref.Key == TopSlidWayID)
			{
				BottomSlidWayId = Ref.Value;
				break;
			}
		}
		RetValue = BottomSlidWayId != -1;
		if (RetValue == false) break;
		auto ChangeAtor = [this](TSharedPtr<FAccessoryShape>& InSrcShape,TSharedPtr<FAccessoryShape> & InDesShape)
		{			
			InDesShape->SetShapeWidth(InSrcShape->GetShapeWidth());
			InDesShape->SetShapeHeight(InSrcShape->GetShapeHeight());
			InDesShape->SetShapeDepth(InSrcShape->GetShapeDepth());
			InDesShape->SetShapePosX(InSrcShape->GetShapePosX());
			InDesShape->SetShapePosY(InSrcShape->GetShapePosY());
			InDesShape->SetShapePosZ(InSrcShape->GetShapePosZ());
			this->RemoveChildShape(InSrcShape);
			InSrcShape->DestroyShapeActor();			
			InSrcShape = InDesShape;
			InSrcShape->SetParentShape(this);
			InSrcShape->LoadModelMesh();
			ASCTShapeActor * Actor = InSrcShape->SpawnShapeActor();
			Actor->AttachToActorOverride(GetShapeActor(), FAttachmentTransformRules::KeepRelativeTransform);
		};
		for (auto & Ref : AllValidSlideways)
		{
			if (Ref->GetShapeId() == TopSlidWayID)
			{
				ChangeAtor(UpSlideWay, Ref);
			}
			else if(Ref->GetShapeId() == BottomSlidWayId)
			{		
				ChangeAtor(DownSlideWay, Ref);
			}
		}
		UpdateDoorGroup();
	} while (false);

	return 	 RetValue;
}

void FSlidingDoor::GetAllOptionTopSlidWay(TArray<FString>& OutTopSlidWayNameList, FString & OutCurTopSlideWayName)
{
	const bool bIsOk = (UpSlideWay.IsValid() && DownSlideWay.IsValid());
	if (bIsOk)
	{
		OutCurTopSlideWayName = UpSlideWay->GetShapeName();
		for (const auto & Ref : AllValidSlideways)
		{
			if (Ref->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SD_TOP_RAILS))
			{
				OutTopSlidWayNameList.Add(Ref->GetShapeName());
			}
		}
	}
}

void FSlidingDoor::SetUpPlateBoard(TSharedPtr<FBoardShape> InBoard)
{
	UpPlateBoard = InBoard;
	UpPlateBoard->SetParentShape(this);
	AddChildShape(UpPlateBoard);
	//计算垫板深度和厚度
	UpPlateDepth = UpPlateBoard->GetShapeDepth();
	UpPlateHeight = UpPlateBoard->GetShapeHeight();
}

TSharedPtr<FBoardShape> FSlidingDoor::GetUpPlateBoard()
{
	return UpPlateBoard;
}

void FSlidingDoor::SetDownPlateBoard(TSharedPtr<FBoardShape> InBoard)
{
	DownPlateBoard = InBoard;
	DownPlateBoard->SetParentShape(this);
	AddChildShape(DownPlateBoard);
	//计算垫板深度和厚度
	DownPlateDepth = DownPlateBoard->GetShapeDepth();
	DownPlateHeight = DownPlateBoard->GetShapeHeight();
	//计算滑轨预设值
	SlidewayPresetValue = DownPlateDepth;

	//根据下垫板深度修改门组深度
	FString StrDepthValue = FString::Printf(TEXT("%f"), DownPlateDepth);
	if (Depth->GetAttributeStr() != StrDepthValue)
	{
		bool bResult = Depth->SetAttributeValue(StrDepthValue);
		if (bResult)
		{
			if (BoundingBoxActor)
			{
				BoundingBoxActor->UpdateActorDimension();
			}
			if (WireFrameActor)
			{
				WireFrameActor->UpdateActorDimension();
			}
		}
	}
}

TSharedPtr<FBoardShape> FSlidingDoor::GetDownPlateBoard()
{
	return DownPlateBoard;
}

bool FSlidingDoor::SetUpPlateBoardHeight(float InIndex)
{
	bool bResult = true;
	if (UpPlateBoard.IsValid())
	{
		bResult = UpPlateBoard->SetShapeHeight(InIndex);
		UpPlateHeight = UpPlateBoard->GetShapeHeight();
		UpdateDoorGroup();
	}
	return bResult;
}

bool FSlidingDoor::SetDownPlateBoardHeight(float InIndex)
{
	bool bResult = true;
	if (DownPlateBoard.IsValid())
	{
		bResult = DownPlateBoard->SetShapeHeight(InIndex);
		DownPlateHeight = DownPlateBoard->GetShapeHeight();
		UpdateDoorGroup();
	}
	return bResult;
}

bool FSlidingDoor::IsUpPlateBoardValid()
{
	return bUpPlateBoardValid;
}

void FSlidingDoor::SetUpPlateBoardValid(bool bValid/* = true*/)
{
	bUpPlateBoardValid = bValid;
	if (UpPlateBoard.IsValid() == false) return;

	{
		ASCTShapeActor* PlateBoardActor = UpPlateBoard->GetShapeActor();
		if (bValid)
		{
			if (PlateBoardActor)
			{
				PlateBoardActor->SetActorHiddenInGame(!bUpPlateBoardValid);
			}
			else
			{
				if (GetShapeActor())
				{
					ASCTShapeActor * Actor = UpPlateBoard->SpawnShapeActor();
					Actor->AttachToActorOverride(GetShapeActor(), FAttachmentTransformRules::KeepRelativeTransform);
				}
			}
		}
		else
		{
			if (PlateBoardActor)
			{
				UpPlateBoard->DestroyShapeActor();
			}
		}
	}
	UpdateDoorGroup();
}

bool FSlidingDoor::IsDownPlateBoardValid()
{
	return bDownPlateBoardValid;
}

void FSlidingDoor::SetDownPlateBoardValid(bool bValid/* = true*/)
{
	bDownPlateBoardValid = bValid;
	if (DownPlateBoard.IsValid() == false) return;
	{
		ASCTShapeActor* PlateBoardActor = DownPlateBoard->GetShapeActor();
		if (bValid)
		{
			if (PlateBoardActor)
			{
				PlateBoardActor->SetActorHiddenInGame(!bDownPlateBoardValid);
			}
			else
			{
				if (GetShapeActor())
				{
					ASCTShapeActor * Actor = DownPlateBoard->SpawnShapeActor();
					Actor->AttachToActorOverride(GetShapeActor(), FAttachmentTransformRules::KeepRelativeTransform);
				}
			}
		}
		else
		{
			if (PlateBoardActor)
			{
				DownPlateBoard->DestroyShapeActor();
			}
		}		
	}	
	UpdateDoorGroup();
}

void FSlidingDoor::InsertDoorSheet(TSharedPtr<FSpaceDividDoorSheet> InDoorSheet)
{
	InDoorSheet->SetParentShape(this);
	AddChildShape(InDoorSheet);
	DoorSheets.Add(InDoorSheet);
}

void FSlidingDoor::DeleteDoorSheetAt(int32 InIndex)
{
	TSharedPtr<FSCTShape> CurDoorSheet = DoorSheets[InIndex];
	RemoveChildShape(CurDoorSheet);
	DoorSheets.RemoveAt(InIndex);
}

void FSlidingDoor::UpdateDoorGroup()
{
	if (DoorSheets.Num() == 0)
	{
		return;
	}

	const float DoorHoleWidth = GetShapeWidth();
	const float DoorHoleDepth = GetShapeDepth();
	const float DoorHoleHeight = GetShapeHeight();

	bool bResult = false;
	//1、计算上下垫板的位置和尺寸
	//float UpBoardHeight = 0.0;
	float DownBoardHeight = 0.0;
	if (bUpPlateBoardValid && UpPlateBoard.IsValid())
	{
		//UpBoardHeight = UpPlateDepth;
		bResult = UpPlateBoard->SetShapeWidth(DoorHoleWidth);
		bResult = UpPlateBoard->SetShapePosX(0.0);
		bResult = UpPlateBoard->SetShapePosY(DownPlateDepth - UpPlateDepth);
		bResult = UpPlateBoard->SetShapePosZ(DoorHoleHeight);
	}
	if (bDownPlateBoardValid && DownPlateBoard.IsValid())
	{
		DownBoardHeight = DownPlateHeight;
		bResult = DownPlateBoard->SetShapeWidth(DoorHoleWidth);
		bResult = DownPlateBoard->SetShapePosX(0.0);
		bResult = DownPlateBoard->SetShapePosY(0.0);
		bResult = DownPlateBoard->SetShapePosZ(0.0);
	}

	//2、计算上下滑轨的位置和尺寸
	if (UpSlideWay.IsValid())
	{
		bResult = UpSlideWay->SetShapeWidth(DoorHoleWidth);
		bResult = UpSlideWay->SetShapePosX(0.0f);
		bResult = UpSlideWay->SetShapePosY((SlidewayPresetValue - UpSlideDepth) / 2.0);
		bResult = UpSlideWay->SetShapePosZ(DoorHoleHeight - UpSlideHeight);
	}
	if (DownSlideWay.IsValid())
	{
		bResult = DownSlideWay->SetShapeWidth(DoorHoleWidth);
		bResult = DownSlideWay->SetShapePosX(0.0f);
		bResult = DownSlideWay->SetShapePosY((SlidewayPresetValue - DownSlideDepth) / 2.0);
		bResult = DownSlideWay->SetShapePosZ(DownBoardHeight);
	}

	AvaliableDoorSheetNum.Empty();
	//3、计算门板的数量
	if (OriginDoorCount == 1)
	{
		TSharedPtr<FShapeAttribute> WidthAttri =  DoorSheets[0]->GetShapeWidthAttri();
		TSharedPtr<FNumberRangeAttri> WidthRange = StaticCastSharedPtr<FNumberRangeAttri>(WidthAttri);
		float MinDoorWidth = WidthRange->GetMinValue();
		float MaxDoorWidth = WidthRange->GetMaxValue();
		float MinGroupWidth = MinDoorWidth;
		float MaxGroupWidth = MaxDoorWidth;
		//计算门板数量
		int32 DoorCount = 1;		
		bool bFindMinCount = false;
		float TempMinWidth = MinGroupWidth;
		float TempMaxWidth = MaxGroupWidth;
		int32 TempDoorCount = DoorCount;
		while (TempDoorCount <= 7)
		{			
			++TempDoorCount;
			TempMinWidth = TempMinWidth + MinDoorWidth - OverlapWidth;
			TempMaxWidth = TempMaxWidth + MaxDoorWidth - OverlapWidth;
			if (TempMinWidth <= DoorHoleWidth && TempMaxWidth >= DoorHoleWidth)
			{							
				AvaliableDoorSheetNum.Emplace(TempDoorCount);
				if (!bFindMinCount)
				{
					bFindMinCount = true;
					DoorCount = TempDoorCount;
					MinGroupWidth = TempMinWidth;
					MaxGroupWidth = TempMaxWidth;
				}
			}		
		}
		check(DoorCount <= 7);
		if (IsExpectDoorSheetNumValid())
		{
			if (AvaliableDoorSheetNum.Contains(GetExpectDoorSheetNum()))
			{
				DoorCount = GetExpectDoorSheetNum();
			}
			else
			{
				ResetExpectDoorSheetnum();
			}			
		}	
		{
			for (int32 i = 1; i < DoorSheets.Num(); ++i)
			{
				DeleteDoorSheetAt(i);
			}
		}
		//复制门板
		if (DoorSheets.Num() < DoorCount)
		{
			for (int32 i = DoorSheets.Num(); i < DoorCount; ++i)
			{
				FSpaceDividDoorSheet* NewDoorSheet = new FSpaceDividDoorSheet();
				DoorSheets[0]->CopyTo(NewDoorSheet);
				InsertDoorSheet(MakeShareable(NewDoorSheet));				
			}
		} 		
	}
	else
	{
		ResetExpectDoorSheetnum();
	}

	// 没有创建Actor的门板创建Actor
	if (GetShapeActor())
	{
		for (int32 i = 0; i < DoorSheets.Num(); ++i)
		{
			if (DoorSheets[i]->GetShapeActor() != nullptr) continue;
			ASCTShapeActor* NewDoorSheetActor = DoorSheets[i]->SpawnShapeActor();
			NewDoorSheetActor->AttachToActorOverride(GetShapeActor(), FAttachmentTransformRules::KeepRelativeTransform);
			NewDoorSheetActor->SetShapeActorHiddenInGameRecursively(GetDoorGoupIsHIdden());
		}
	}

	//4、计算门板的尺寸
	const float TotalDoorsWidth = DoorHoleWidth + OverlapWidth * (DoorSheets.Num() - 1);
	const float SingleDoorWidth = TotalDoorsWidth / DoorSheets.Num();
	const float SingleDoorHeight = DoorHoleHeight - UpSlideMeasure - DownBoardHeight - DownSlideMeasure;
	for (int32 i = 0; i < DoorSheets.Num(); ++i)
	{
		bResult = DoorSheets[i]->SetShapeWidth(SingleDoorWidth);
		bResult = DoorSheets[i]->SetShapeHeight(SingleDoorHeight);
	}

	//5、计算门板的位置
 	const float DoorSheetDepth = DoorSheets[0]->GetOrCalDoorTrueDepth();
	for (int32 i = 0; i < DoorSheets.Num(); ++i)
	{
		const float DepthModification = IsDoorSheetLocateOutside(i) ? 0.0 : DoorSheetDepth;
		DoorSheets[i]->SetShapePosX((SingleDoorWidth - OverlapWidth) * i);
		DoorSheets[i]->SetShapePosY(SlidewayPresetValue / 2.0 - DepthModification);
		DoorSheets[i]->SetShapePosZ(DownBoardHeight + DownSlideMeasure);
		if (DoorSheets[i]->GetShapeActor())
		{
			DoorSheets[i]->ReCalDividBlocksPositionAndSize();
		}
	}
}

FBoardShape* FSlidingDoor::ParsePlateBoardShape(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FBoardShape* NewBoardShape = new FBoardShape;	
	//板件的厚度和深度
	float BoardDepth = StaticCast<float>(InJsonObject->GetNumberField(TEXT("depth")));
	float BoardHeight = StaticCast<float>(InJsonObject->GetNumberField(TEXT("height")));
	NewBoardShape->SetShapeDepth(BoardDepth);
	NewBoardShape->SetShapeHeight(BoardHeight);
	//初始化一个宽度和形状类型
	NewBoardShape->SetShapeWidth(100.0f);
	NewBoardShape->SetBoardShapeType(BoST_Rectangle);

	int32 SubstrateId = InJsonObject->GetIntegerField(TEXT("substrateId"));

	//材质信息
	{
		FPakMetaData TempInfo;
		const TSharedPtr<FJsonObject> & ObjRef = InJsonObject->GetObjectField(TEXT("material"));
		TempInfo.ID = ObjRef->GetIntegerField(TEXT("id"));
		TempInfo.Url = ObjRef->GetStringField(TEXT("pakUrl"));
		TempInfo.MD5 = ObjRef->GetStringField(TEXT("pakMd5"));
		TempInfo.Name = ObjRef->GetStringField(TEXT("name"));
		
		NewBoardShape->SetMaterialData(TempInfo);
	}

	//封边信息
	{
		FPakMetaData TempInfo;
		const TSharedPtr<FJsonObject> & ObjRef = InJsonObject->GetObjectField(TEXT("edgeBanding"));
		TempInfo.ID = ObjRef->GetIntegerField(TEXT("id"));
		TempInfo.Url = ObjRef->GetStringField(TEXT("pakUrl"));
		TempInfo.MD5 = ObjRef->GetStringField(TEXT("pakMd5"));
		TempInfo.Name = ObjRef->GetStringField(TEXT("name"));
		
		NewBoardShape->SetEdgeBandingData(TempInfo);
	}

	//基材信息
	{
		const TSharedPtr<FJsonObject> & ObjRef = InJsonObject->GetObjectField(TEXT("substrate"));
		int32 SubstrateID = ObjRef->GetIntegerField(TEXT("id"));
		FString SubstrateName = ObjRef->GetStringField(TEXT("name"));
		int32 SubstrateTypeInt = ObjRef->GetIntegerField(TEXT("type"));
		NewBoardShape->SetSubstrateID(SubstrateID);
		NewBoardShape->SetSubstrateName(SubstrateName);
		NewBoardShape->SetSubstrateType(StaticCast<ESubstrateType>(SubstrateTypeInt));
	}

	return NewBoardShape;
}

FAccessoryShape* FSlidingDoor::GetSlidewayShapeByID(int32 InID)
{
	for (int32 i=0; i<AllValidSlideways.Num(); ++i)
	{
		if (AllValidSlideways[i]->GetShapeId() == InID)
		{
			return AllValidSlideways[i].Get();
		}
	}
	return nullptr;
}

bool FSlidingDoor::IsDoorSheetLocateOutside(int32 InIndex)
{
	//第一扇门板
	if (InIndex == 0)
	{
		return FirstPosition == 2;
	}

	//内外交错
	if (InterleavingMode == 1)  
	{
		int32 InOutMode = InIndex % 2;
		if (InOutMode == 0)
		{
			return FirstPosition == 2;
		} 
		else
		{
			return FirstPosition != 2;
		}
	}
	//左右对称
	else if (InterleavingMode == 2)
	{
		int32 DoorSheetCount = DoorSheets.Num();
		int32 InOutMode = InIndex / 2;
		if (InIndex <= 3 || InIndex < InOutMode)
		{
			int32 InOutMode = InIndex % 2;
			if (InOutMode == 0)
			{
				return FirstPosition == 2;
			}
			else
			{
				return FirstPosition != 2;
			}
		}
		else
		{
			int32 InOutMode = InIndex % 2;
			if (InOutMode == 0)
			{
				return FirstPosition != 2;
			}
			else
			{
				return FirstPosition == 2;
			}
		}
	} 
	else
	{
		check(false);
	}

	return true;
}

//掩门门组
FSideHungDoor::FSideHungDoor()
{
	SetShapeType(ST_SideHungDoor);
	SetShapeName(TEXT("掩门门组"));
	// 明装拉手	
	FilterParseMetalType.Add(EMetalsType::MT_LATERAL_BOX_SHAKE_HAND);	
	FilterSpawnMetalType.Add(EMetalsType::MT_LATERAL_BOX_SHAKE_HAND);
}

FSideHungDoor::~FSideHungDoor()
{
}

void FSideHungDoor::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//解析型录基本信息
	FDoorGroup::ParseFromJson(InJsonObject);
	//修改门组尺寸变化范围
	TSharedPtr<FNumberRangeAttri> GroupWidthAttri = StaticCastSharedPtr<FNumberRangeAttri>(Width);
	GroupWidthAttri->SetMinValue(0.0);
	GroupWidthAttri->SetMaxValue(100000.0);
	TSharedPtr<FNumberRangeAttri> GroupDepthAttri = StaticCastSharedPtr<FNumberRangeAttri>(Depth);
	GroupDepthAttri->SetMinValue(0.0);
	GroupDepthAttri->SetMaxValue(10000.0);
	TSharedPtr<FNumberRangeAttri> GroupHeightAttri = StaticCastSharedPtr<FNumberRangeAttri>(Height);
	GroupHeightAttri->SetMinValue(0.0);
	GroupHeightAttri->SetMaxValue(10000.0);

	//四边掩盖方式
	UpCorverType = 1;
	DownCorverType = 1;
	LeftCorverType = 1;
	RightCorverType = 1;

	//门缝值
	if (InJsonObject->HasField(TEXT("doorSlotValue")))
	{
		const TArray<TSharedPtr<FJsonValue>> TempDoorSlotArray = InJsonObject->GetArrayField(TEXT("doorSlotValue"));
		check(TempDoorSlotArray.Num() >= 2);
		LeftRightGap = TempDoorSlotArray[0]->AsNumber();
		UpDownGap = TempDoorSlotArray[1]->AsNumber();
		BackGap = 1.0;
	}
	
	//掩门门板
	FVariableAreaDoorSheetForSideHungDoor* NewSideHungSheet = new FVariableAreaDoorSheetForSideHungDoor;
	NewSideHungSheet->ParseFromJson(InJsonObject);
	DoorSheets.Add(MakeShareable(NewSideHungSheet));

	//定义门板数量
	OriginDoorCount = 1;	

}

void FSideHungDoor::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//型录类型和Id
	int32 CurrentType = InJsonObject->GetIntegerField(TEXT("type"));
	int32 CurrentId = InJsonObject->GetIntegerField(TEXT("id"));
	check(CurrentType == ShapeType && CurrentId == ShapeId);
					     
	//更新型录基本信息
	FDoorGroup::ParseAttributesFromJson(InJsonObject);

	//门板尺寸比例
	const TArray<TSharedPtr<FJsonValue>>* ScalesList = nullptr;
	InJsonObject->TryGetArrayField(TEXT("doorWidthScales"), ScalesList);
	if (ScalesList != nullptr && (*ScalesList).Num() > 0)
	{
		DoorWidthScales.Empty();
		for (int32 i=0; i<(*ScalesList).Num(); ++i)
		{
			float CurScaleValue = (*ScalesList)[i]->AsNumber();
			DoorWidthScales.Add(CurScaleValue);
		}
	}
	
	//定义门板数量
	OriginDoorCount = InJsonObject->GetIntegerField(TEXT("originDoorCount"));

	//四边掩盖方式
	UpCorverType = InJsonObject->GetIntegerField(TEXT("upCoverType"));
	DownCorverType = InJsonObject->GetIntegerField(TEXT("downCoverType"));
	LeftCorverType = InJsonObject->GetIntegerField(TEXT("leftCoverType"));
	RightCorverType = InJsonObject->GetIntegerField(TEXT("rightCoverType"));

	//门缝值
	LeftRightGap = InJsonObject->GetNumberField(TEXT("leftRightGap"));
	UpDownGap = InJsonObject->GetNumberField(TEXT("upDownGap"));
	BackGap = InJsonObject->GetNumberField(TEXT("backGap"));

	//更新子级型录信息
	const TArray<TSharedPtr<FJsonValue>>* ChildList = nullptr;
	InJsonObject->TryGetArrayField(TEXT("children"), ChildList);
	if (ChildList != nullptr && (*ChildList).Num() > 0)
	{
		// 此处只解析门板		
		for (auto& It : *ChildList)
		{
			TSharedPtr<FJsonObject> ChildObject = It->AsObject();
			int32 ShapeType = ChildObject->GetNumberField(TEXT("type"));
			int64 ShapeId = ChildObject->GetNumberField(TEXT("id"));
			if (ShapeType != ST_VariableAreaDoorSheet_ForSideHungDoor)
			{
				continue;
			}
			TSharedPtr<FSCTShape> NewShape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ShapeType, ShapeId);
			TSharedPtr<FSCTShape> CopyShape = FSCTShapeManager::Get()->CopyShapeToNew(NewShape);
			//添加子级对象
			TSharedPtr<FVariableAreaDoorSheetForSideHungDoor> CurDoorSheet = StaticCastSharedPtr<FVariableAreaDoorSheetForSideHungDoor>(CopyShape);
			InsertDoorSheet(CurDoorSheet);
			//解析子级对象
			CopyShape->ParseAttributesFromJson(ChildObject);
		}
	}

	//插脚封板信息
	if (InJsonObject->HasField(TEXT("cornerPlate")))
	{
		const TSharedPtr<FJsonObject> & CornerPlateJsonObjRef = InJsonObject->GetObjectField(TEXT("cornerPlate"));
		{
			//SetFrontBoardType(StaticCast<EFrontBoardType>(CornerPlateJsonObjRef->GetIntegerField(TEXT("frontBoardType"))));
			SetFrontBoardPositionType(StaticCast<EFrontBoardPositionType>(CornerPlateJsonObjRef->GetIntegerField(TEXT("frontBoardPosType"))));
			//SetFrontBoardCornerType(StaticCast<EFrontBoardCornerType>(CornerPlateJsonObjRef->GetIntegerField(TEXT("frontBoardCorner"))));
			SetFrontBoardLinkageRule(StaticCast<EFrontBoardLinkageRule>(CornerPlateJsonObjRef->GetIntegerField(TEXT("frontBoardLinkageRule"))));
			SetFrontBoardSubstrate(CornerPlateJsonObjRef->GetIntegerField(TEXT("frontBoardSubstarte")));
			SetFrontBoardEdgeBanding(CornerPlateJsonObjRef->GetIntegerField(TEXT("frontBoardEdgeBanding")));
			SetFrontBoardWidth(StaticCast<float>(CornerPlateJsonObjRef->GetNumberField(TEXT("frontBoardWidth"))));
			//SetFrontBoardHeight(StaticCast<float>(CornerPlateJsonObjRef->GetNumberField(TEXT("frontBoardHeight"))));
			SetFrontBoardDepth(StaticCast<float>(CornerPlateJsonObjRef->GetNumberField(TEXT("frontBoardDepth"))));
			SetFrontBoardMinWidth(StaticCast<float>(CornerPlateJsonObjRef->GetNumberField(TEXT("frontBoardMinWidth"))));
			SetFrontBoardTopExtern(StaticCast<float>(CornerPlateJsonObjRef->GetNumberField(TEXT("frontBoardTopExtern"))));
			SetFrontBoardBottomExtern(StaticCast<float>(CornerPlateJsonObjRef->GetNumberField(TEXT("frontBoardBottomExtern"))));
			SetFrontBoardCornerCutWidth(StaticCast<float>(CornerPlateJsonObjRef->GetNumberField(TEXT("frontBoardCornerCutWidth"))));
			SetFrontBoardCornerCutHeight(StaticCast<float>(CornerPlateJsonObjRef->GetNumberField(TEXT("frontBoardCornerCutHeight"))));
			const TSharedPtr<FJsonObject> & FrontBoardMaterialJsonRef = CornerPlateJsonObjRef->GetObjectField(TEXT("frontBoardMaterial"));
			FDoorSheetShapeBase::FDoorPakMetaData DoorPakData;
			DoorPakData.ID = FrontBoardMaterialJsonRef->GetIntegerField(TEXT("id"));
			DoorPakData.Name = FrontBoardMaterialJsonRef->GetStringField(TEXT("name"));
			DoorPakData.Url = FrontBoardMaterialJsonRef->GetStringField(TEXT("url"));
			DoorPakData.ThumbnailUrl = FrontBoardMaterialJsonRef->GetStringField(TEXT("thumbnailUrl"));
			DoorPakData.MD5 = FrontBoardMaterialJsonRef->GetStringField(TEXT("md5"));
			DoorPakData.OptimizeParam = FrontBoardMaterialJsonRef->GetStringField(TEXT("optimizeParam"));
			SetFrontBoardMaterial(DoorPakData);

			if (CornerPlateJsonObjRef->HasField(TEXT("assistBoard")))
			{
				const TSharedPtr<FJsonObject> AssistBoardJsonRef = CornerPlateJsonObjRef->GetObjectField(TEXT("assistBoard"));
				SetAssistBoardSubstrate(AssistBoardJsonRef->GetIntegerField(TEXT("assistBoardSubstrate")));
				SetAssistBoardEdgeBanding(AssistBoardJsonRef->GetIntegerField(TEXT("assistBoardEdgeBanding")));
				SetAssistBoardWidth(StaticCast<float>(AssistBoardJsonRef->GetNumberField(TEXT("assistBoardWidth"))));
				SetAssistBoardHeight(StaticCast<float>(AssistBoardJsonRef->GetNumberField(TEXT("assistBoardHeight"))));
				SetAssistBoardDepth(StaticCast<float>(AssistBoardJsonRef->GetNumberField(TEXT("assistBoardDepth"))));
				SetAssistBoardTopExtern(StaticCast<float>(AssistBoardJsonRef->GetNumberField(TEXT("assistBoardTopExtern"))));
				SetAsssistBoardBottomExtern(StaticCast<float>(AssistBoardJsonRef->GetNumberField(TEXT("assistBoardBottomExtern"))));
				SetAssistBoardLengthToFrontBoard(StaticCast<float>(AssistBoardJsonRef->GetNumberField(TEXT("assistBoardLengthToFrontBoard"))));
				const TSharedPtr<FJsonObject> & AssistBoardMaterialJsonRef = AssistBoardJsonRef->GetObjectField(TEXT("assistBoardMaterial"));
				FDoorSheetShapeBase::FDoorPakMetaData DoorPakData;
				DoorPakData.ID = AssistBoardMaterialJsonRef->GetIntegerField(TEXT("id"));
				DoorPakData.Name = AssistBoardMaterialJsonRef->GetStringField(TEXT("name"));
				DoorPakData.Url = AssistBoardMaterialJsonRef->GetStringField(TEXT("url"));
				DoorPakData.ThumbnailUrl = AssistBoardMaterialJsonRef->GetStringField(TEXT("thumbnailUrl"));
				DoorPakData.MD5 = AssistBoardMaterialJsonRef->GetStringField(TEXT("md5"));
				DoorPakData.OptimizeParam = AssistBoardMaterialJsonRef->GetStringField(TEXT("optimizeParam"));
				SetAssistBoardMaterial(DoorPakData);
			}
			// 其他插角封板的属性设置完之后，再设置这两个属性
			SetFrontBoardType(StaticCast<EFrontBoardType>(CornerPlateJsonObjRef->GetIntegerField(TEXT("frontBoardType"))));
			SetFrontBoardCornerType(StaticCast<EFrontBoardCornerType>(CornerPlateJsonObjRef->GetIntegerField(TEXT("frontBoardCorner"))));
		}
	}


	// 拉手类型
	auto SetDefaultAuxiliaryMetalInfo = [InJsonObject](FDoorSheetShapeBase::FAuxiliaryMetalInfo & OutInfo, const FString & InJsonObjName)
	{
		if (!InJsonObject->HasField(InJsonObjName)) return;
		{
			const TSharedPtr<FJsonObject> & PosJsonObj = InJsonObject->GetObjectField(InJsonObjName);
		
			OutInfo.ID = PosJsonObj->GetIntegerField(TEXT("id"));
			OutInfo.Name = PosJsonObj->GetStringField(TEXT("name"));					
			if (PosJsonObj->HasField(TEXT("hpos")))
			{
				OutInfo.Location = MakeShareable(new FDoorHanlerDefaultLocation);
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->MetalType = StaticCast<EMetalsType>(PosJsonObj->GetIntegerField(TEXT("metalsType")));
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->Hpos = PosJsonObj->GetIntegerField(TEXT("hpos"));
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->Vpos = PosJsonObj->GetIntegerField(TEXT("vpos"));
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->RotateDirection = PosJsonObj->GetIntegerField(TEXT("rotationalDirection"));
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->HposVal = PosJsonObj->GetNumberField(TEXT("hposVal"));
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->VposVAl = PosJsonObj->GetNumberField(TEXT("vposVal"));
				if (PosJsonObj->HasTypedField<EJson::Number>(TEXT("leftExtend")))
				{
					StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->LeftExtend = PosJsonObj->GetNumberField(TEXT("leftExtend"));
				}
				if (PosJsonObj->HasTypedField<EJson::Number>(TEXT("rightExtend")))
				{
					StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->RightExtend = PosJsonObj->GetNumberField(TEXT("rightExtend"));
				}
			}
		}
	};
	SetDefaultAuxiliaryMetalInfo(CurrentHandleLocationInfo, TEXT("handleInfo"));
	if (InJsonObject->HasField(TEXT("hanleType")))
	{
		HanleType = StaticCast<EHandleMetalType>(InJsonObject->GetIntegerField(TEXT("hanleType")));
	}
}

void FSideHungDoor::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FDoorGroup::ParseShapeFromJson(InJsonObject);
}

void FSideHungDoor::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FDoorGroup::ParseContentFromJson(InJsonObject);
}

void FSideHungDoor::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//此接口暂时为了让BIM前段使用

	JsonWriter->WriteValue(TEXT("upExtension"), UpExtensionValue);
	JsonWriter->WriteValue(TEXT("downExtension"), DownExtensionValue);
	JsonWriter->WriteValue(TEXT("leftExtension"), LeftExtensionValue);
	JsonWriter->WriteValue(TEXT("rightExtension"), RightExtensionValue);

	// 获取第一个门板
	TSharedPtr<FDoorSheetShapeBase> DoorSheet = GetDoorSheets()[0];	
	{
		//1、型录类型和ID
		JsonWriter->WriteValue(TEXT("type"), (int32)GetShapeType());
		JsonWriter->WriteValue(TEXT("id"), GetShapeId());
		JsonWriter->WriteValue(TEXT("category"), GetShapeCategory());

		//2、型录基本信息
		JsonWriter->WriteValue(TEXT("name"), GetShapeName());
		JsonWriter->WriteValue(TEXT("thumbnailUrl"), GetThumbnailUrl());
		JsonWriter->WriteValue(TEXT("code"), GetShapeCode());
		JsonWriter->WriteValue(TEXT("systemName"), GetSystemName());

		//3、产品状态
		JsonWriter->WriteValue(TEXT("status"), (int32)GetProductStatus());

		//4、型录显示条件
		// 	JsonWriter->WriteObjectStart(TEXT("showCondition"));
		// 	ShowCondition->SaveToJson(JsonWriter);
		// 	JsonWriter->WriteObjectEnd();

		//5、型录尺寸信息
		JsonWriter->WriteObjectStart(TEXT("width"));
		DoorSheet->GetShapeWidthAttri()->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteObjectStart(TEXT("depth"));
		DoorSheet->GetShapeDepthAttri()->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteObjectStart(TEXT("height"));
		DoorSheet->GetShapeHeightAttri()->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		//6、型录位置信息
		JsonWriter->WriteObjectStart(TEXT("posx"));
		DoorSheet->GetShapePosXAttri()->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteObjectStart(TEXT("posy"));
		DoorSheet->GetShapePosYAttri()->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteObjectStart(TEXT("posz"));
		DoorSheet->GetShapePosZAttri()->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		//7、型录位置信息
		JsonWriter->WriteObjectStart(TEXT("rotx"));
		DoorSheet->GetShapeRotXAttri()->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteObjectStart(TEXT("roty"));
		DoorSheet->GetShapeRotYAttri()->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteObjectStart(TEXT("rotz"));
		DoorSheet->GetShapeRotZAttri()->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}

	//门缝值
	JsonWriter->WriteArrayStart(TEXT("doorSlotValue"));
	JsonWriter->WriteValue(LeftRightGap);
	JsonWriter->WriteValue(UpDownGap);
	JsonWriter->WriteArrayEnd();

	

	// 封边
	{
		JsonWriter->WriteObjectStart(TEXT("edgeBanding"));
		const FDoorSheetShapeBase::FDoorPakMetaData & PakRef = DoorSheet->GetEdgeBanding();
		JsonWriter->WriteValue(TEXT("id"), PakRef.ID);
		JsonWriter->WriteValue(TEXT("name"), PakRef.Name);
		JsonWriter->WriteValue(TEXT("pakUrl"), PakRef.Url);
		JsonWriter->WriteValue(TEXT("pakMd5"), PakRef.MD5);
		JsonWriter->WriteValue(TEXT("thumbnailUrl"), PakRef.ThumbnailUrl);
		JsonWriter->WriteObjectEnd();
	}
	// 基材
	{
		JsonWriter->WriteObjectStart(TEXT("substrate"));
		JsonWriter->WriteValue(TEXT("id"), DoorSheet->GetSubstrateID());
		JsonWriter->WriteValue(TEXT("name"), DoorSheet->GetSubstrateName());
		JsonWriter->WriteObjectEnd();
	}
	// 材质
	{
		JsonWriter->WriteObjectStart(TEXT("material"));
		const FDoorSheetShapeBase::FDoorPakMetaData & PakRef = DoorSheet->GetMaterial();
		JsonWriter->WriteValue(TEXT("id"), PakRef.ID);
		JsonWriter->WriteValue(TEXT("name"), PakRef.Name);
		JsonWriter->WriteValue(TEXT("pakUrl"), PakRef.Url);
		JsonWriter->WriteValue(TEXT("pakMd5"), PakRef.MD5);
		JsonWriter->WriteValue(TEXT("thumbnailUrl"), PakRef.ThumbnailUrl);
		JsonWriter->WriteValue(TEXT("optimizeParam"), PakRef.OptimizeParam);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteValue(TEXT("hasMaterialReplace"), DoorSheet->HasReplaceMaterial());

	// 存储门所用到的五金
	{
		JsonWriter->WriteArrayStart(TEXT("children"));
		// 门组五金
		for (const auto & Ref : Metals)
		{
			JsonWriter->WriteObjectStart();
			Ref->SaveAttriToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
		// 门板五金 
		for (const auto & Ref : DoorSheet->GetMetals())
		{
			JsonWriter->WriteObjectStart();
			Ref->SaveAttriToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}

		JsonWriter->WriteArrayEnd();
	}

	// 存储Shapes
	{
		JsonWriter->WriteArrayStart(TEXT("shapes"));
		// 门组可替换五金
		for (const auto & Ref : OpetionsMetalsShapesList)
		{
			JsonWriter->WriteObjectStart();
			Ref.Value->SaveShapeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
		// 门板可替换五金 
		for (const auto & Ref : DoorSheet->GetOptionShapeMap())
		{
			JsonWriter->WriteObjectStart();
			Ref.Value->SaveShapeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}

		JsonWriter->WriteArrayEnd();
	}

	// 存储切块
	{		

		// 存储门切块
		JsonWriter->WriteArrayStart(TEXT("divideBlocks"));

		auto WriteAttribute = [](const FString & InName, const FString & InRefName, const TSharedPtr<FShapeAttribute> & InAttri, TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
		{
			TSharedPtr<FNumberRangeAttri> RangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(InAttri);
			JsonWriter->WriteObjectStart(InName);
			JsonWriter->WriteValue(TEXT("id"), RangeAttri->GetAttributeId());
			JsonWriter->WriteValue(TEXT("type"), (int32)RangeAttri->GetAttributeType());
			JsonWriter->WriteValue(TEXT("refName"), InRefName);
			JsonWriter->WriteObjectStart(TEXT("value"));
			JsonWriter->WriteValue(TEXT("min"), RangeAttri->GetMinValue());
			JsonWriter->WriteValue(TEXT("max"), RangeAttri->GetMaxValue());
			JsonWriter->WriteObjectEnd();
			JsonWriter->WriteValue(TEXT("current"), RangeAttri->GetAttributeStr());
			JsonWriter->WriteObjectEnd();
		};

		TSharedPtr<FVariableAreaDoorSheet> VariableAreaDoorSheet = StaticCastSharedPtr<FVariableAreaDoorSheet>(DoorSheet);
		for (const auto & Ref : VariableAreaDoorSheet->GetDividBlocks())
		{
			JsonWriter->WriteObjectStart();
			WriteAttribute(TEXT("width"), TEXT("WIDTH"), Ref->GetShapeWidthAttri(), JsonWriter);
			WriteAttribute(TEXT("depth"), TEXT("DEPTH"), Ref->GetShapeDepthAttri(), JsonWriter);
			WriteAttribute(TEXT("height"), TEXT("HEIGHT"), Ref->GetShapeHeightAttri(), JsonWriter);
			WriteAttribute(TEXT("posx"), TEXT("POSX"), Ref->GetShapePosXAttri(), JsonWriter);
			WriteAttribute(TEXT("posy"), TEXT("POSY"), Ref->GetShapePosYAttri(), JsonWriter);
			WriteAttribute(TEXT("posz"), TEXT("POSZ"), Ref->GetShapePosZAttri(), JsonWriter);
			WriteAttribute(TEXT("rotx"), TEXT("ROTX"), Ref->GetShapeRotXAttri(), JsonWriter);
			WriteAttribute(TEXT("roty"), TEXT("ROTY"), Ref->GetShapeRotYAttri(), JsonWriter);
			WriteAttribute(TEXT("rotz"), TEXT("ROTZ"), Ref->GetShapeRotZAttri(), JsonWriter);
			JsonWriter->WriteValue(TEXT("pakUrl"), Ref->GetFileUrl());
			JsonWriter->WriteValue(TEXT("pakMd5"), Ref->GetFileMd5());
			JsonWriter->WriteValue(TEXT("fileName"), Ref->GetFileName());
			JsonWriter->WriteValue(TEXT("mwassetUrl"), TEXT(""));
			JsonWriter->WriteValue(TEXT("optimizeParam"), Ref->GetoptimizeParam());
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();
	}


	{	
		TSharedPtr<FVariableAreaDoorSheet> VarSheet = StaticCastSharedPtr<FVariableAreaDoorSheet>(DoorSheet);

		JsonWriter->WriteArrayStart(TEXT("openDoorDirections"));
		for (const auto & Ref : VarSheet->GetAvailableOpenDirections())
		{
			JsonWriter->WriteValue(StaticCast<int32>(Ref));
		}
		JsonWriter->WriteArrayEnd();
		JsonWriter->WriteValue(TEXT("defaultOpenDoorIndex"), StaticCast<int32>(VarSheet->GetOpenDoorDirectionIndex()));
	}

}

void FSideHungDoor::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//保存型录基本信息
	FDoorGroup::SaveAttriToJson(JsonWriter);

	//门板尺寸比例
	JsonWriter->WriteArrayStart(TEXT("doorWidthScales"));
	for (int32 i=0; i<DoorWidthScales.Num(); ++i)
	{
		JsonWriter->WriteValue(DoorWidthScales[i]);
	}
	JsonWriter->WriteArrayEnd();
	

	//定义门板数量
	JsonWriter->WriteValue(TEXT("originDoorCount"), OriginDoorCount);

	//四边掩盖方式
	JsonWriter->WriteValue(TEXT("upCoverType"), UpCorverType);
	JsonWriter->WriteValue(TEXT("downCoverType"), DownCorverType);
	JsonWriter->WriteValue(TEXT("leftCoverType"), LeftCorverType);
	JsonWriter->WriteValue(TEXT("rightCoverType"), RightCorverType);

	//门缝值
	JsonWriter->WriteValue(TEXT("leftRightGap"), LeftRightGap);
	JsonWriter->WriteValue(TEXT("upDownGap"), UpDownGap);
	JsonWriter->WriteValue(TEXT("backGap"), BackGap);

	//子型录信息
	JsonWriter->WriteArrayStart(TEXT("children"));
	//掩门门板
	for (int32 i = 0; i < DoorSheets.Num(); ++i)
	{
		JsonWriter->WriteObjectStart();
		DoorSheets[i]->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}

	for (auto & Ref : Metals)
	{
		JsonWriter->WriteObjectStart();
		Ref->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}

	JsonWriter->WriteArrayEnd();

	// 插角封板
	if (FrontBoard.IsValid())
	{
		JsonWriter->WriteObjectStart(TEXT("cornerPlate"));
		{
			JsonWriter->WriteValue(TEXT("frontBoardType"), StaticCast<int32>(GetFrontBoardType()));
			JsonWriter->WriteValue(TEXT("frontBoardPosType"), StaticCast<int32>(GetFrontBoardPositionType()));
			JsonWriter->WriteValue(TEXT("frontBoardCorner"), StaticCast<int32>(GetFrontBoardCornerType()));
			JsonWriter->WriteValue(TEXT("frontBoardLinkageRule"), StaticCast<int32>(GetFrontBoardLinkageRule()));
			JsonWriter->WriteValue(TEXT("frontBoardSubstarte"), GetFrontBoardSubstrate());
			JsonWriter->WriteValue(TEXT("frontBoardEdgeBanding"), GetFrontBoardEdgeBanding());
			JsonWriter->WriteValue(TEXT("frontBoardWidth"), GetFrontBoardWidth());
			//JsonWriter->WriteValue(TEXT("frontBoardHeight"), GetFrontBoardHeight());
			JsonWriter->WriteValue(TEXT("frontBoardDepth"), GetFrontBoardDepth());
			JsonWriter->WriteValue(TEXT("frontBoardMinWidth"), GetFrontBoardMinWidth());
			JsonWriter->WriteValue(TEXT("frontBoardTopExtern"), GetFrontBoardTopExtern());
			JsonWriter->WriteValue(TEXT("frontBoardBottomExtern"), GetFrontBoardBottomExtern());
			JsonWriter->WriteValue(TEXT("frontBoardCornerCutWidth"), GetFrontBoardCornerCutWidth());
			JsonWriter->WriteValue(TEXT("frontBoardCornerCutHeight"), GetFrontBoardCornerCutHeight());
			{
				JsonWriter->WriteObjectStart(TEXT("frontBoardMaterial"));
				JsonWriter->WriteValue(TEXT("id"), GetFrontBoardMaterial().ID);
				JsonWriter->WriteValue(TEXT("name"), GetFrontBoardMaterial().Name);
				JsonWriter->WriteValue(TEXT("url"), GetFrontBoardMaterial().Url);
				JsonWriter->WriteValue(TEXT("thumbnailUrl"), GetFrontBoardMaterial().ThumbnailUrl);
				JsonWriter->WriteValue(TEXT("md5"), GetFrontBoardMaterial().MD5);
				JsonWriter->WriteValue(TEXT("optimizeParam"), GetFrontBoardMaterial().OptimizeParam);
				JsonWriter->WriteObjectEnd();
			}
			//if (AssistBoard.IsValid())
			{
				JsonWriter->WriteObjectStart(TEXT("assistBoard"));
				JsonWriter->WriteValue(TEXT("assistBoardSubstrate"), GetAssistBoardSubstrate());
				JsonWriter->WriteValue(TEXT("assistBoardEdgeBanding"), GetAssistBoardEdgeBanding());
				JsonWriter->WriteValue(TEXT("assistBoardWidth"), GetAssistBoardWidth());
				JsonWriter->WriteValue(TEXT("assistBoardHeight"), GetAssistBoardHeight());
				JsonWriter->WriteValue(TEXT("assistBoardDepth"), GetAssistBoardDepth());
				JsonWriter->WriteValue(TEXT("assistBoardTopExtern"), GetAssistBoardTopExtern());
				JsonWriter->WriteValue(TEXT("assistBoardBottomExtern"), GetAssistBoardBottomExtern());
				JsonWriter->WriteValue(TEXT("assistBoardLengthToFrontBoard"), GetAssistBoardLengthToFrontBoard());
				{
					JsonWriter->WriteObjectStart(TEXT("assistBoardMaterial"));
					JsonWriter->WriteValue(TEXT("id"), GetAssistBoardMaterial().ID);
					JsonWriter->WriteValue(TEXT("name"), GetAssistBoardMaterial().Name);
					JsonWriter->WriteValue(TEXT("url"), GetAssistBoardMaterial().Url);
					JsonWriter->WriteValue(TEXT("thumbnailUrl"), GetAssistBoardMaterial().ThumbnailUrl);
					JsonWriter->WriteValue(TEXT("md5"), GetAssistBoardMaterial().MD5);
					JsonWriter->WriteValue(TEXT("optimizeParam"), GetAssistBoardMaterial().OptimizeParam);
					JsonWriter->WriteObjectEnd();
				}
				JsonWriter->WriteObjectEnd();
			}
		}
		JsonWriter->WriteObjectEnd();
	}

	const FDoorSheetShapeBase::FAuxiliaryMetalInfo & HandleInfo = GetCurrentHandleInfo();
	if (HandleInfo.ID != -1)
	{
		auto SetDefaultAuxiliaryMetalInfo = [JsonWriter](const FDoorSheetShapeBase::FAuxiliaryMetalInfo & InInfo, const FString & InJsonObjName, const int32 InType)
		{
			if (InInfo.ID != -1)
			{
				JsonWriter->WriteObjectStart(InJsonObjName);
				JsonWriter->WriteValue(TEXT("id"), InInfo.ID);
				JsonWriter->WriteValue(TEXT("name"), InInfo.Name);
				JsonWriter->WriteValue(TEXT("metalsType"), InType);
				if (InInfo.Location.IsValid())
				{
					JsonWriter->WriteValue(TEXT("hpos"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->Hpos);
					JsonWriter->WriteValue(TEXT("rotationalDirection"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->RotateDirection);
					JsonWriter->WriteValue(TEXT("hposVal"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->HposVal);
					JsonWriter->WriteValue(TEXT("vpos"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->Vpos);
					JsonWriter->WriteValue(TEXT("vposVal"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->VposVAl);
					JsonWriter->WriteValue(TEXT("leftExtend"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->LeftExtend);
					JsonWriter->WriteValue(TEXT("rightExtend"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->RightExtend);					
				}
				JsonWriter->WriteObjectEnd();
			}
		};

		SetDefaultAuxiliaryMetalInfo(HandleInfo, TEXT("handleInfo"), StaticCast<int32>(EMetalsType::MT_LATERAL_BOX_SHAKE_HAND));		
	}
	JsonWriter->WriteValue(TEXT("hanleType"), StaticCast<int32>(GetHanleType()));

#if 0
	// 以下代码用来检验SaveToJson的内容
	JsonWriter->WriteObjectStart(TEXT("SideHungdoorTest"));
	SaveToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();
#endif
}

void FSideHungDoor::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FDoorGroup::SaveShapeToJson(JsonWriter);
	JsonWriter->WriteArrayStart(TEXT("shapes"));
	for (const auto & Ref : OpetionsMetalsShapesList)
	{
		JsonWriter->WriteObjectStart();
		Ref.Value->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

}

void FSideHungDoor::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FDoorGroup::SaveContentToJson(JsonWriter);
}

void FSideHungDoor::CopyTo(FSCTShape* OutShape)
{
	//基类操作
	FDoorGroup::CopyTo(OutShape);

	//掩门门组
	FSideHungDoor* OutSideHungDoor = StaticCast<FSideHungDoor*>(OutShape);

	//四边掩盖方式
	OutSideHungDoor->UpCorverType = UpCorverType;
	OutSideHungDoor->DownCorverType = DownCorverType;
	OutSideHungDoor->LeftCorverType = LeftCorverType;
	OutSideHungDoor->RightCorverType = RightCorverType;

	//门缝值
	OutSideHungDoor->LeftRightGap = LeftRightGap;
	OutSideHungDoor->UpDownGap = UpDownGap;
	OutSideHungDoor->BackGap = BackGap;

	//掩门门板
	for (int32 i = 0; i < DoorSheets.Num(); ++i)
	{
		FVariableAreaDoorSheetForSideHungDoor* NewDoorSheet = new FVariableAreaDoorSheetForSideHungDoor;
		DoorSheets[i]->CopyTo(NewDoorSheet);
		OutSideHungDoor->InsertDoorSheet(MakeShareable(NewDoorSheet));
	}
	OutSideHungDoor->OriginDoorCount = OriginDoorCount;

	//封板信息
	if (FrontBoard.IsValid())
	{
		//FBoardShape* NewFrontBoard = new FBoardShape;
		//FrontBoard->CopyTo(NewFrontBoard);
		//OutSideHungDoor->FrontBoard = MakeShareable(NewFrontBoard);
		//OutSideHungDoor->FrontBoardType = FrontBoardType;		
		//OutSideHungDoor->FrontBoardCornerType = FrontBoardCornerType;
		OutSideHungDoor->FrontPositionType = FrontPositionType;
		OutSideHungDoor->FrontBoardLinkageRule = FrontBoardLinkageRule;
		OutSideHungDoor->FrontBoardSubsrate = FrontBoardSubsrate;
		OutSideHungDoor->FrontBoardEdgeBanding = FrontBoardEdgeBanding;
		OutSideHungDoor->FrontBoardWidth = FrontBoardWidth;
		OutSideHungDoor->FrontBoardMinWidth = FrontBoardMinWidth;
		//OutSideHungDoor->FrontBoardHeight = FrontBoardHeight;
		OutSideHungDoor->FrontBoardDepth = FrontBoardDepth;
		OutSideHungDoor->FrontBoardTopExtern = FrontBoardTopExtern;
		OutSideHungDoor->FrontBoardBottomExtern = FrontBoardBottomExtern;
		OutSideHungDoor->FrontBoardCornerCutWidth = FrontBoardCornerCutWidth;
		OutSideHungDoor->FrontBoardCornerCutHeight = FrontBoardCornerCutHeight;
		OutSideHungDoor->SetFrontBoardMaterial(FrontBoardMaterial);
		//if (AssistBoard.IsValid())
		{
			//FBoardShape* NewAssistBoard = new FBoardShape;
			//AssistBoard->CopyTo(NewAssistBoard);
			//OutSideHungDoor->AssistBoard = MakeShareable(NewAssistBoard);
			OutSideHungDoor->AssistBoardSubstrate = AssistBoardSubstrate;
			OutSideHungDoor->AssistBoardEdgeBanding = AssistBoardEdgeBanding;
			OutSideHungDoor->AssistBoardWith = AssistBoardWith;
			OutSideHungDoor->AssistBoardHeight = AssistBoardHeight;
			OutSideHungDoor->AsssistBoardDepth = AsssistBoardDepth;
			OutSideHungDoor->AssistBoardTopExtern = AssistBoardTopExtern;
			OutSideHungDoor->AssistBoardBottomExtern = AssistBoardBottomExtern;
			OutSideHungDoor->AssistBoardLengthToFrontBoard = AssistBoardLengthToFrontBoard;
			OutSideHungDoor->SetAssistBoardMaterial(AssistBoardMaterial);
		}
		// 其他插角封板的属性设置完之后，再设置这两个属性
		OutSideHungDoor->SetFrontBoardType(FrontBoardType);
		OutSideHungDoor->SetFrontBoardCornerType(FrontBoardCornerType);
	}
	if (CurrentHandleLocationInfo.ID != -1)
	{
		OutSideHungDoor->CurrentHandleLocationInfo.ID = CurrentHandleLocationInfo.ID;
		OutSideHungDoor->CurrentHandleLocationInfo.Name = CurrentHandleLocationInfo.Name;
		OutSideHungDoor->CurrentHandleLocationInfo.Location = MakeShareable(new FDoorHanlerDefaultLocation);
		OutSideHungDoor->CurrentHandleLocationInfo.Location->MetalType = CurrentHandleLocationInfo.Location->MetalType;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutSideHungDoor->CurrentHandleLocationInfo.Location)->Hpos = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->Hpos;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutSideHungDoor->CurrentHandleLocationInfo.Location)->HposVal = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->HposVal;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutSideHungDoor->CurrentHandleLocationInfo.Location)->Vpos = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->Vpos;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutSideHungDoor->CurrentHandleLocationInfo.Location)->VposVAl = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->VposVAl;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutSideHungDoor->CurrentHandleLocationInfo.Location)->RotateDirection = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->RotateDirection;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutSideHungDoor->CurrentHandleLocationInfo.Location)->LeftExtend = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->LeftExtend;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutSideHungDoor->CurrentHandleLocationInfo.Location)->RightExtend = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->RightExtend;

	}
	OutSideHungDoor->HandleList = HandleList;
	OutSideHungDoor->BoxHanleSubType = BoxHanleSubType;
	OutSideHungDoor->HanleType = HanleType;
}

ASCTShapeActor* FSideHungDoor::SpawnShapeActor()
{	
	//创建门组型录
	ASCTShapeActor* NewDoorGroupActor = FDoorGroup::SpawnShapeActor();
	
	//门板
	for (int32 i = 0; i < DoorSheets.Num(); ++i)
	{
		DoorSheets[i]->FilterSpawnMetalType.Empty();	
		if(GetHanleType() == EHandleMetalType::E_MT_SHAKE_HANDS)
		{			
			DoorSheets[i]->FilterSpawnMetalType.Add(EMetalsType::MT_SHAKE_HANDS);
		}
		else if (GetHanleType() == EHandleMetalType::E_MT_SEALING_SIDE_HANDLE)
		{			
			DoorSheets[i]->FilterSpawnMetalType.Add(EMetalsType::MT_SEALING_SIDE_HANDLE);
		}
		ASCTShapeActor* NewDoorSheetActor = DoorSheets[i]->SpawnShapeActor();
		NewDoorSheetActor->AttachToActorOverride(NewDoorGroupActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	// 封板
	if (FrontBoard.IsValid())
	{
		ASCTShapeActor * ShapeActor = FrontBoard->SpawnShapeActor();
		ShapeActor->AttachToActorOverride(NewDoorGroupActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	// 帮板
	if (AssistBoard.IsValid())
	{
		ASCTShapeActor * ShapeActor = AssistBoard->SpawnShapeActor();
		ShapeActor->AttachToActorOverride(NewDoorGroupActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	if (GetHanleType() != EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND)
	{
		for (auto & Ref : Metals)
		{
			if (Ref->GetShapeActor())
			{								
				Ref->DestroyShapeActor();
			}
		}
	}		
	UpdateDoorGroup();
	return NewDoorGroupActor;
}

void FSideHungDoor::SetCollisionProfileName(FName InProfileName)
{
	//门板
	for (int32 i = 0; i < DoorSheets.Num(); ++i)
	{
		DoorSheets[i]->SetCollisionProfileName(InProfileName);
	}
}

void FSideHungDoor::GetResourceUrls(TArray<FString> &OutResourceUrls)
{
	FDoorGroup::GetResourceUrls(OutResourceUrls);
	//门板
	for (int32 i = 0; i < DoorSheets.Num(); ++i)
	{
		DoorSheets[i]->GetResourceUrls(OutResourceUrls);
	}
	if (FrontBoard.IsValid())
	{		
		FrontBoard->GetResourceUrls(OutResourceUrls);
	}
	if (AssistBoard.IsValid())
	{		
		AssistBoard->GetResourceUrls(OutResourceUrls);
	}
}

void FSideHungDoor::GetFileCachePaths(TArray<FString> &OutFileCachePaths)
{
	FDoorGroup::GetFileCachePaths(OutFileCachePaths);
	//门板
	for (int32 i = 0; i < DoorSheets.Num(); ++i)
	{
		DoorSheets[i]->GetFileCachePaths(OutFileCachePaths);
	}
	if (FrontBoard.IsValid())
	{
		FrontBoard->GetFileCachePaths(OutFileCachePaths);
	}
	if (AssistBoard.IsValid())
	{
		AssistBoard->GetFileCachePaths(OutFileCachePaths);
	}
}

void FSideHungDoor::ShowDoorSheetsActor()
{
	//控制BoundingBoxActor
	FDoorGroup::ShowDoorSheetsActor();
	//空间门板Actor
	for (int32 i = 0; i < DoorSheets.Num(); ++i)
	{
		ASCTShapeActor* DoorSheetActor = DoorSheets[i]->GetShapeActor();
		if (DoorSheetActor)
		{
			DoorSheetActor->SetShapeActorHiddenInGameRecursively(false);
		}
	}
	if (FrontBoard.IsValid())
	{
		ASCTShapeActor* FrontBoardActor = FrontBoard->GetShapeActor();
		if (FrontBoardActor)
		{
			FrontBoardActor->SetShapeActorHiddenInGameRecursively(false);
		}
		if (AssistBoard.IsValid())
		{
			ASCTShapeActor* AssistActor = AssistBoard->GetShapeActor();
			if (AssistActor)
			{
				AssistActor->SetShapeActorHiddenInGameRecursively(false);
			}
		}
	}
}

void FSideHungDoor::HideDoorSheetsActor()
{
	//控制BoundingBoxActor
	FDoorGroup::HideDoorSheetsActor();
	//空间门板Actor
	for (int32 i = 0; i < DoorSheets.Num(); ++i)
	{
		ASCTShapeActor* DoorSheetActor = DoorSheets[i]->GetShapeActor();
		if (DoorSheetActor)
		{
			DoorSheetActor->SetShapeActorHiddenInGameRecursively(true);
		}
	}
	if (FrontBoard.IsValid())
	{
		ASCTShapeActor* FrontBoardActor = FrontBoard->GetShapeActor();
		if (FrontBoardActor)
		{
			FrontBoardActor->SetShapeActorHiddenInGameRecursively(true);
		}
		if (AssistBoard.IsValid())
		{
			ASCTShapeActor* AssistActor = AssistBoard->GetShapeActor();
			if (AssistActor)
			{
				AssistActor->SetShapeActorHiddenInGameRecursively(true);
			}
		}
	}
}

void FSideHungDoor::HiddenDoorGroupActors(bool bHidden)
{
	//空间门板Actor
	for (int32 i = 0; i < DoorSheets.Num(); ++i)
	{
		ASCTShapeActor* DoorSheetActor = DoorSheets[i]->GetShapeActor();
		if (DoorSheetActor)
		{
			DoorSheetActor->SetShapeActorHiddenInGameRecursively(bHidden);
		}
	}
	if (FrontBoard.IsValid())
	{
		ASCTShapeActor* FrontBoardActor = FrontBoard->GetShapeActor();
		if (FrontBoardActor)
		{
			FrontBoardActor->SetShapeActorHiddenInGameRecursively(bHidden);
		}
		if (AssistBoard.IsValid())
		{
			ASCTShapeActor* AssistActor = AssistBoard->GetShapeActor();
			if (AssistActor)
			{
				AssistActor->SetShapeActorHiddenInGameRecursively(bHidden);
			}
		}
	}
}

bool FSideHungDoor::GetDoorGoupIsHIdden() const
{
	check(const_cast<FSideHungDoor*>(this)->GetDoorSheets().Num() > 0);
	if (const_cast<FSideHungDoor*>(this)->GetDoorSheets()[0]->GetShapeActor())
	{
		return const_cast<FSideHungDoor*>(this)->GetDoorSheets()[0]->GetShapeActor()->bHidden;
	}
	else
	{
		return FDoorGroup::GetDoorGoupIsHIdden();
	}
}

bool FSideHungDoor::SetShapeWidth(float InIntValue)
{
	return SetShapeWidth(FString::Printf(TEXT("%f"), InIntValue));
}

bool FSideHungDoor::SetShapeWidth(const FString &InStrValue)
{
	bool bResult = true;
	if (Width->GetAttributeStr() != InStrValue)
	{
		bResult = Width->SetAttributeValue(InStrValue);
		if (bResult)
		{
			UpdateDoorGroup();
			//
			if (BoundingBoxActor)
			{
				BoundingBoxActor->UpdateActorDimension();
			}
			if (WireFrameActor)
			{
				WireFrameActor->UpdateActorDimension();
			}
		}
	}
	return bResult;
}

bool FSideHungDoor::SetShapeDepth(float InIntValue)
{
	return SetShapeDepth(FString::Printf(TEXT("%f"), InIntValue));
}

bool FSideHungDoor::SetShapeDepth(const FString &InStrValue)
{
	bool bResult = true;
	if (Depth->GetAttributeStr() != InStrValue)
	{
		bResult = Depth->SetAttributeValue(InStrValue);
		if (bResult)
		{
			UpdateDoorGroup();
			//
			if (BoundingBoxActor)
			{
				BoundingBoxActor->UpdateActorDimension();
			}
			if (WireFrameActor)
			{
				WireFrameActor->UpdateActorDimension();
			}
		}
	}
	return bResult;
}

bool FSideHungDoor::SetShapeHeight(float InIntValue)
{
	return SetShapeHeight(FString::Printf(TEXT("%f"), InIntValue));
}

bool FSideHungDoor::SetShapeHeight(const FString &InStrValue)
{
	bool bResult = true;
	if (Height->GetAttributeStr() != InStrValue)
	{
		bResult = Height->SetAttributeValue(InStrValue);
		if (bResult)
		{
			UpdateDoorGroup();
			//
			if (BoundingBoxActor)
			{
				BoundingBoxActor->UpdateActorDimension();
			}
			if (WireFrameActor)
			{
				WireFrameActor->UpdateActorDimension();
			}
		}
	}
	return bResult;
}

float FSideHungDoor::GetDoorTotalDepth()
{
	return GetShapeDepth() + BackGap;
}

float FSideHungDoor::GetDoorSheetWidthRange(float& MinValue, float& MaxValue)
{
	//单门板尺寸范围
	float MinDoorSheetWidth = 0.0;
	float MaxDoorSheetWidth = 0.0;
	float CurDoorSheetWidth = DoorSheets[0]->GetShapeWidth();
	TSharedPtr<FShapeAttribute> DoorWidthAttri = DoorSheets[0]->GetShapeWidthAttri();
	if (SAT_NumberRange == DoorWidthAttri->GetAttributeType())
	{
		TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(DoorWidthAttri);
		MinDoorSheetWidth = NumberRangeAttri->GetMinValue();
		MaxDoorSheetWidth = NumberRangeAttri->GetMaxValue();
	}
	else
	{
		MinDoorSheetWidth = CurDoorSheetWidth;
		MaxDoorSheetWidth = CurDoorSheetWidth;
	}

	//门板数量
	int32 DoorSheetCount = DoorSheets.Num();
	//重叠位总宽度
	float TotalGapWidth = LeftRightGap * DoorSheetCount * 2.0;

	//计算门组的尺寸范围
	MinValue = MinDoorSheetWidth * DoorSheetCount + TotalGapWidth;
	MaxValue = MaxDoorSheetWidth * DoorSheetCount + TotalGapWidth;
	
	return CurDoorSheetWidth * DoorSheetCount + TotalGapWidth;
}

float FSideHungDoor::GetShapeWidthRange(float& MinValue, float& MaxValue)
{
	//单门板尺寸范围
	float MinDoorSheetWidth = 0.0;
	float MaxDoorSheetWidth = 0.0;
	float CurDoorSheetWidth = DoorSheets[0]->GetShapeWidth();
	TSharedPtr<FShapeAttribute> DoorWidthAttri = DoorSheets[0]->GetShapeWidthAttri();
	if (SAT_NumberRange == DoorWidthAttri->GetAttributeType())
	{
		TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(DoorWidthAttri);
		MinDoorSheetWidth = NumberRangeAttri->GetMinValue();
		MaxDoorSheetWidth = NumberRangeAttri->GetMaxValue();
	}
	else
	{
		MinDoorSheetWidth = CurDoorSheetWidth;
		MaxDoorSheetWidth = CurDoorSheetWidth;
	}

	//门板数量
	int32 DoorSheetCount = DoorSheets.Num();
	//重叠位总宽度
	float TotalGapWidth = LeftRightGap * DoorSheetCount * 2.0;

	//计算门组的尺寸范围
	MinValue = MinDoorSheetWidth * DoorSheetCount + TotalGapWidth;
	MaxValue = MaxDoorSheetWidth * DoorSheetCount + TotalGapWidth;

	//根据封板尺寸修正
	if (FrontBoard.IsValid())
	{
		MinValue += FrontBoardWidth;
		MaxValue += FrontBoardWidth;
	}

	return GetShapeWidth();
}

float FSideHungDoor::GetShapeDepthRange(float& MinValue, float& MaxValue)
{
	//单门板尺寸范围
	float MinDoorSheetDepth = 0.0;
	float MaxDoorSheetDepth = 0.0;
	float CurDoorSheetDepth = DoorSheets[0]->GetShapeDepth();
	TSharedPtr<FShapeAttribute> DoorDepthAttri = DoorSheets[0]->GetShapeDepthAttri();
	if (SAT_NumberRange == DoorDepthAttri->GetAttributeType())
	{
		TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(DoorDepthAttri);
		MinDoorSheetDepth = NumberRangeAttri->GetMinValue();
		MaxDoorSheetDepth = NumberRangeAttri->GetMaxValue();
	}
	else
	{
		MinDoorSheetDepth = CurDoorSheetDepth;
		MaxDoorSheetDepth = CurDoorSheetDepth;
	}

	//计算门组的尺寸范围
	MinValue = MinDoorSheetDepth + BackGap;
	MaxValue = MaxDoorSheetDepth + BackGap;

	//门组当前值
	return GetShapeDepth();
}

float FSideHungDoor::GetShapeHeightRange(float& MinValue, float& MaxValue)
{
	//单门板尺寸范围
	float MinDoorSheetHeight = 0.0;
	float MaxDoorSheetHeight = 0.0;
	float CurDoorSheetHeight = DoorSheets[0]->GetShapeHeight();
	TSharedPtr<FShapeAttribute> DoorHeightAttri = DoorSheets[0]->GetShapeHeightAttri();
	if (SAT_NumberRange == DoorHeightAttri->GetAttributeType())
	{
		TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(DoorHeightAttri);
		MinDoorSheetHeight = NumberRangeAttri->GetMinValue();
		MaxDoorSheetHeight = NumberRangeAttri->GetMaxValue();
	}
	else
	{
		MinDoorSheetHeight = CurDoorSheetHeight;
		MaxDoorSheetHeight = CurDoorSheetHeight;
	}

	//计算门组的尺寸范围
	MinValue = MinDoorSheetHeight + UpDownGap * 2.0;
	MaxValue = MaxDoorSheetHeight + UpDownGap * 2.0;

	//门组当前值
	return GetShapeHeight();
}

bool FSideHungDoor::IsValidForSheetWidth(float InValue)
{
	if (DoorSheets.Num() == 0)
	{
		return true;
	}
	//单门板尺寸范围
	float MinDoorSheetWidth = 0.0;
	float MaxDoorSheetWidth = 0.0;
	float CurDoorSheetWidth = DoorSheets[0]->GetShapeWidth();
	TSharedPtr<FShapeAttribute> DoorWidthAttri = DoorSheets[0]->GetShapeWidthAttri();
	if (SAT_NumberRange == DoorWidthAttri->GetAttributeType())
	{
		TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(DoorWidthAttri);
		MinDoorSheetWidth = NumberRangeAttri->GetMinValue();
		MaxDoorSheetWidth = NumberRangeAttri->GetMaxValue();
	}
	else
	{
		MinDoorSheetWidth = CurDoorSheetWidth;
		MaxDoorSheetWidth = CurDoorSheetWidth;
	}

	if (OriginDoorCount == 1)
	{
		//单开门情况
		float TotalGapWidth = LeftRightGap * 2.0;
		float MinGroupValue = MinDoorSheetWidth + TotalGapWidth;
		float MaxGroupValue = MaxDoorSheetWidth + TotalGapWidth;
		if (InValue >= MinGroupValue && InValue <= MaxGroupValue)
		{
			return true;
		}
		//对开门情况
		TotalGapWidth = LeftRightGap * 2 * 2.0;
		MinGroupValue = MinDoorSheetWidth * 2 + TotalGapWidth;
		MaxGroupValue = MaxDoorSheetWidth * 2 + TotalGapWidth;
		if (InValue >= MinGroupValue && InValue <= MaxGroupValue)
		{
			return true;
		}
	}
	else
	{
		check(OriginDoorCount == 2);
		//对开门
		float TotalGapWidth = LeftRightGap * 2 * 2.0;
		float MinGroupValue = MinDoorSheetWidth * 2 + TotalGapWidth;
		float MaxGroupValue = MaxDoorSheetWidth * 2 + TotalGapWidth;
		if (InValue >= MinGroupValue && InValue <= MaxGroupValue)
		{
			return true;
		}
	}
	return false;
}

bool FSideHungDoor::IsValidForWidth(float InValue)
{
	bool bRetResult = false;
	do
	{
		bool bSingleDoorValid = false, bDoubleDoorValid = false;
		IsSingleAndDoubleValid(InValue, bSingleDoorValid, bDoubleDoorValid);
		bRetResult = bSingleDoorValid || bDoubleDoorValid;
		if (!bRetResult) break;

	} while (false);
	return 	bRetResult;
}

bool FSideHungDoor::IsValidForDepth(float InValue)
{
	float MinGroupValue = 0.0;
	float MaxGroupValue = 0.0;
	float CurGroupValue = GetShapeDepthRange(MinGroupValue, MaxGroupValue);
	if (InValue >= MinGroupValue && InValue <= MaxGroupValue)
	{
		return true;
	}
	return false;
}

bool FSideHungDoor::IsValidForHeight(float InValue)
{
	float MinGroupValue = 0.0;
	float MaxGroupValue = 0.0;
	float CurGroupValue = GetShapeHeightRange(MinGroupValue, MaxGroupValue);
	if (InValue >= MinGroupValue && InValue <= MaxGroupValue)
	{
		return true;
	}
	return false;
}

void FSideHungDoor::SetUpExtensionValue(float InValue)
{
	UpExtensionValue = InValue;
	//更新门板尺寸和位置
	UpdateDoorGroup();
}

void FSideHungDoor::SetDownExtensionValue(float InValue)
{
	DownExtensionValue = InValue;
	//更新门板尺寸和位置
	UpdateDoorGroup();
}

void FSideHungDoor::SetLeftExtensionValue(float InValue)
{
	LeftExtensionValue = InValue;
	//更新门板尺寸和位置
	UpdateDoorGroup();
}

void FSideHungDoor::SetRightExtensionValue(float InValue)
{
	RightExtensionValue = InValue;
	//更新门板尺寸和位置
	UpdateDoorGroup();
}

void FSideHungDoor::SetFrontBoardType(const EFrontBoardType InType)
{
	if (InType == FrontBoardType)  return;
	FrontBoardType = InType;
	ReSetAssistBoard();
	ReSetFrontBoard();
	if (FrontBoardType == EFrontBoardType::E_None)
	{
	}
	else if (FrontBoardType == EFrontBoardType::E_JustFrontBoard)
	{
		SetFrontBoard();
		if (ShapeActor)
		{
			ASCTShapeActor * Actor = FrontBoard->SpawnShapeActor();
			Actor->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
	else if (FrontBoardType == EFrontBoardType::E_FrontBoardAndAssitBoard)
	{		
		SetFrontBoard();
		SetAssistBoard();		
		if (ShapeActor)
		{
			ASCTShapeActor * FrontBoardActor = FrontBoard->SpawnShapeActor();
			FrontBoardActor->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
		}		
		if (ShapeActor)
		{
			ASCTShapeActor * AssistBoardActor = AssistBoard->SpawnShapeActor();
			AssistBoardActor->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
	UpdateDoorGroup();	
}

void FSideHungDoor::SetFrontBoardCornerType(const EFrontBoardCornerType InType)
{
	if (FrontBoardCornerType == InType)	return;
	FrontBoardCornerType = InType;
	ReSetFrontBoard();
	SetFrontBoard();	
	auto CheckAndFixAttri = [this]()
	{
		if (FrontBoard->GetShapeParamAttriByRefName(TEXT("TW")).IsValid() == false)
		{
			TArray<TSharedPtr<FShapeAttribute>> AttriArray;
			TSharedPtr<FShapeAttribute> TwAttri = MakeShareable(new FNumberRangeAttri(FrontBoard.Get()));
			TwAttri->SetRefName(TEXT("TW"));
			StaticCastSharedPtr<FNumberRangeAttri>(TwAttri)->SetMinValue(-10000.0f);
			StaticCastSharedPtr<FNumberRangeAttri>(TwAttri)->SetMaxValue(10000.0f);
			StaticCastSharedPtr<FNumberRangeAttri>(TwAttri)->SetAttributeValue(TEXT("10.0"));
			AttriArray.Emplace(TwAttri);
			TSharedPtr<FShapeAttribute> TdAttri = MakeShareable(new FNumberRangeAttri(FrontBoard.Get()));
			TdAttri->SetRefName(TEXT("TD"));
			StaticCastSharedPtr<FNumberRangeAttri>(TdAttri)->SetMinValue(-10000.0f);
			StaticCastSharedPtr<FNumberRangeAttri>(TdAttri)->SetMaxValue(10000.0f);
			StaticCastSharedPtr<FNumberRangeAttri>(TdAttri)->SetAttributeValue(TEXT("10.0"));
			AttriArray.Emplace(TdAttri);
			FrontBoard->AddShapeParamAttri(AttriArray);			
		}
		FrontBoard->SetShapeParamValueByRefName(TEXT("TW"), GetFrontBoardCornerCutWidth());
		FrontBoard->SetShapeParamValueByRefName(TEXT("TD"), GetFrontBoardCornerCutHeight());
	};

	switch (FrontBoardCornerType)
	{
		case EFrontBoardCornerType::E_LeftTopCorner:
		{
			FrontBoard->SetBoardShapeType(BoST_LeftCut);
			CheckAndFixAttri();
		}
		break;
		case EFrontBoardCornerType::E_RithtTopCorner:
		{					
			FrontBoard->SetBoardShapeType(BoST_RightCut);
			CheckAndFixAttri();
		}
		break;
		case EFrontBoardCornerType::E_None:
		{
			FrontBoard->SetBoardShapeType(BoST_Rectangle);
		}
		break;
		default:
		check(false);
		break;	   
	}
		
	if (ShapeActor)
	{
		ASCTShapeActor * Actor = FrontBoard->SpawnShapeActor();
		Actor->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	UpdateDoorGroup();
}

bool FSideHungDoor::SetFrontBoardWidth(const float InValue)
{
	bool bRetResult = false;	
	do
	{
		if (FrontBoard.IsValid())
		{
			bRetResult = InValue > GetFrontBoardMinWidth();
			if (!bRetResult) break;
			const float OldFrontBoardWidth = FrontBoardWidth;
			FrontBoardWidth = InValue;
			bool IsSingDoorValid = false, bIsDoubleDoorValid = false;
			IsSingleAndDoubleValid(GetShapeWidth(), IsSingDoorValid, bIsDoubleDoorValid);
			bRetResult = IsSingDoorValid || bIsDoubleDoorValid;
			if (!bRetResult)
			{
				FrontBoardWidth = OldFrontBoardWidth;
				break;
			}
			FrontBoardWidth = InValue;
		}
		else
		{
			FrontBoardWidth = InValue;
			bRetResult = true;
		}
		UpdateDoorGroup();
	}
	while (false);	
	return 	bRetResult;
}

bool FSideHungDoor::SetFrontBoardDepth(const float InValue)
{
	bool RetResult = false;
	if (FrontBoard.IsValid())
	{
		RetResult = FrontBoard->SetShapeHeight(InValue);
		if (RetResult)
		{
			FrontBoardDepth = InValue;
			if (ShapeActor)
			{
				UpdateDoorGroup();
			}
		}		
	}
	else
	{
		FrontBoardDepth = InValue;
		RetResult = true;
	}
	return 	RetResult;
	
}

bool FSideHungDoor::SetFrontBoardTopExtern(const float InValue)
{
	bool RetResult = true;
	if (FrontBoard.IsValid())
	{
		FrontBoardTopExtern = InValue;
		if (ShapeActor)
		{
			UpdateDoorGroup();
		}
	}
	else
	{
		FrontBoardTopExtern = InValue;
	}
	return 	RetResult;

}

bool FSideHungDoor::SetFrontBoardBottomExtern(const float InValue)
{
	bool RetResult = true;
	if (FrontBoard.IsValid())
	{		
		FrontBoardBottomExtern = InValue;
		if (ShapeActor)
		{
			UpdateDoorGroup();
		}
	}
	else
	{
		FrontBoardBottomExtern = InValue;		
	}
	return 	RetResult;
}

bool FSideHungDoor::SetFrontBoardCornerCutWidth(const float InValue)
{
	bool RetResult = true;
	if (FrontBoard.IsValid())
	{		
		const float OldValue = FrontBoardCornerCutWidth;
		FrontBoardCornerCutWidth = InValue;
		RetResult = FrontBoard->SetShapeParamValueByRefName(TEXT("TW"), GetFrontBoardCornerCutWidth());
		if (!RetResult)
		{
			FrontBoardCornerCutWidth = OldValue;
		}
		else
		{
			UpdateDoorGroup();
		}
	}
	else
	{
		FrontBoardCornerCutWidth = InValue;
	}
	return 	RetResult;
}

bool FSideHungDoor::SetFrontBoardCornerCutHeight(const float InValue)
{	
	bool RetResult = true;
	if (FrontBoard.IsValid())
	{		
		const float OldValue = FrontBoardCornerCutHeight;
		FrontBoardCornerCutHeight = InValue;
		RetResult = FrontBoard->SetShapeParamValueByRefName(TEXT("TD"), GetFrontBoardCornerCutHeight());
		if (!RetResult)
		{
			FrontBoardCornerCutHeight = OldValue;
		}
	}
	else
	{
		FrontBoardCornerCutHeight = InValue;
	}
	return 	RetResult;
}

bool FSideHungDoor::SetAssistBoardWidth(const float InValue)
{
	bool RetResult = false;
	if (AssistBoard.IsValid())
	{
		RetResult = AssistBoard->SetShapeDepth(InValue);
		if (RetResult)
		{
			AssistBoardWith = InValue;
			if (ShapeActor)
			{
				UpdateDoorGroup();
			}
		}
	}
	else
	{
		AssistBoardWith = InValue;
		RetResult = true;
	}
	return 	RetResult;
}

bool FSideHungDoor::SetAssistBoardHeight(const float InValue)
{
	bool RetResult = false;
	if (AssistBoard.IsValid())
	{
		RetResult = AssistBoard->SetShapeWidth(InValue);
		if (RetResult)
		{
			AssistBoardHeight = InValue;
			if (ShapeActor)
			{
				UpdateDoorGroup();
			}
		}
	}
	else
	{
		AssistBoardHeight = InValue;
		RetResult = true;
	}
	return 	RetResult;
}

bool FSideHungDoor::SetAssistBoardDepth(const float InValue)
{
	bool RetResult = false;
	if (AssistBoard.IsValid())
	{
		RetResult = AssistBoard->SetShapeHeight(InValue);
		if (RetResult)
		{
			AsssistBoardDepth = InValue;
			if (ShapeActor)
			{
				UpdateDoorGroup();
			}
		}
	}
	else
	{
		AsssistBoardDepth = InValue;
		RetResult = true;
	}
	return 	RetResult;
}

bool FSideHungDoor::SetAssistBoardTopExtern(const float InValue)
{
	bool RetResult = true;
	if (FrontBoard.IsValid())
	{
		AssistBoardTopExtern = InValue;
		if (ShapeActor)
		{
			UpdateDoorGroup();
		}
	}
	else
	{
		AssistBoardTopExtern = InValue;
	}
	return 	RetResult;
}

bool FSideHungDoor::SetAsssistBoardBottomExtern(const float InValue)
{
	bool RetResult = true;
	if (FrontBoard.IsValid())
	{
		AssistBoardBottomExtern = InValue;
		if (ShapeActor)
		{
			UpdateDoorGroup();
		}
	}
	else
	{
		AssistBoardBottomExtern = InValue;
	}
	return 	RetResult;
}

bool FSideHungDoor::SetAssistBoardLengthToFrontBoard(const float InValue)
{
	bool RetResult = true;
	if (FrontBoard.IsValid())
	{
		AssistBoardLengthToFrontBoard = InValue;
		if (ShapeActor)
		{
			UpdateDoorGroup();
		}
	}
	else
	{
		AssistBoardLengthToFrontBoard = InValue;
	}
	return 	RetResult;	
}

void FSideHungDoor::SetFrontBoardMaterial(const FDoorSheetShapeBase::FDoorPakMetaData & InMaterialData)
{
	FrontBoardMaterial = InMaterialData;	
	{
		FPakMetaData Material;
		Material.ID = FrontBoardMaterial.ID; 
		Material.Name = FrontBoardMaterial.Name;
		Material.Url = FrontBoardMaterial.Url;
		Material.MD5 = FrontBoardMaterial.MD5;
		Material.ThumbnailUrl = FrontBoardMaterial.ThumbnailUrl;
		Material.OptimizeParam = FrontBoardMaterial.OptimizeParam;
		if (FrontBoard.IsValid())
		{
			FrontBoard->SetMaterialRotateValue(90.0f);
			FrontBoard->SetMaterialData(Material);			
		}
	}
}

void FSideHungDoor::SetAssistBoardMaterial(const FDoorSheetShapeBase::FDoorPakMetaData & InMaterialData)
{
	AssistBoardMaterial = InMaterialData;	
	{
		FPakMetaData Material;
		Material.ID = AssistBoardMaterial.ID;
		Material.Name = AssistBoardMaterial.Name;
		Material.Url = AssistBoardMaterial.Url;
		Material.MD5 = AssistBoardMaterial.MD5;
		Material.ThumbnailUrl = AssistBoardMaterial.ThumbnailUrl;
		Material.OptimizeParam = AssistBoardMaterial.OptimizeParam;
		if (AssistBoard.IsValid())
		{
			AssistBoard->SetMaterialData(Material);
		}
	}
}

bool FSideHungDoor::SetDoorSheetNum(int32 InType)
{
	bool bRetResult = false;
	do
	{
		// 先决条件判断
		{
			bRetResult = (InType == 1 || InType == 2);
			if (!bRetResult) break;
			bRetResult = OriginDoorCount >= 1;
			if (!bRetResult) break;
			bRetResult = DoorSheets.Num() != InType;
			if (!bRetResult) break;
			bRetResult = (InType == 2) ? GetDoorOpenType() != EDoorOpenType::E_TipUpDoor : true;
			if (!bRetResult) break;
		}

		// 尺寸判断
		{
			const float DoorSheetWidthExtenSize = GetFrontBoardPositionType() == FSideHungDoor::EFrontBoardPositionType::E_None ? GetRightExtensionValue() + GetLeftExtensionValue() :
				(GetFrontBoardPositionType() == FSideHungDoor::EFrontBoardPositionType::E_DoorLeft ? GetRightExtensionValue() : GetLeftExtensionValue());

			float MinDooSheetWidth = 0.0, MaxDoorSheetWidth = 0.0, CurSingleDoorSheetWidth = 0.0f;
			GetSingleDoorSheetRange(MinDooSheetWidth, MaxDoorSheetWidth, CurSingleDoorSheetWidth);
			const float CurrentDoorGroupWidth = GetShapeWidth() - (GetFrontBoardPositionType() == FSideHungDoor::EFrontBoardPositionType::E_None ? 0 : GetFrontBoardWidth()) + DoorSheetWidthExtenSize;
			if (InType == 1)
			{
				const float DoorSheetsWidth = CurrentDoorGroupWidth - (2.0f * GetLeftRightGapValue());
				bRetResult = DoorSheetsWidth >= MinDooSheetWidth &&  DoorSheetsWidth <= MaxDoorSheetWidth;
				if (!bRetResult) break;
			}
			else if (InType == 2)
			{
				const float DoorSheetsWidth = (CurrentDoorGroupWidth - 2.0f * (2.0f * GetLeftRightGapValue())) / 2.0f;
				bRetResult = DoorSheetsWidth >= MinDooSheetWidth &&  DoorSheetsWidth <= MaxDoorSheetWidth;
				if (!bRetResult) break;

			}
		}
		 // 门板数量设置
		if (InType == 1)
		{
			check(DoorSheets.Num() == 2);			
			DeleteDoorSheetAt(1);			
		}
		else if (InType == 2)
		{
			check(DoorSheets.Num() == 1);
			TSharedPtr<FVariableAreaDoorSheetForSideHungDoor> NewDoorSheet = MakeShareable(new FVariableAreaDoorSheetForSideHungDoor());
			DoorSheets[0]->CopyTo(NewDoorSheet.Get());
			ASCTShapeActor* NewDoorActor = NewDoorSheet->SpawnShapeActor();
			NewDoorActor->AttachToActorOverride(GetShapeActor(), FAttachmentTransformRules::KeepRelativeTransform);
			InsertDoorSheet(NewDoorSheet);

			//修改对开门的开门方式
			auto CheckAvailableOpenDirection = [](const TArray<FVariableAreaDoorSheet::EDoorOpenDirection> & InArray, const int32 InIndex)->bool
			{
				return InArray.IsValidIndex(InIndex);
			};
			//如果后台没有给该门设置支持的开门方向，则不进行左右开门方向的设置
			if (CheckAvailableOpenDirection(DoorSheets[0]->GetAvailableOpenDirections(), StaticCast<int32>(FVariableAreaDoorSheet::EDoorOpenDirection::E_Left)))
			{
				DoorSheets[0]->SetOpenDoorDirection(FVariableAreaDoorSheet::EDoorOpenDirection::E_Left);
			}
			if (CheckAvailableOpenDirection(DoorSheets[1]->GetAvailableOpenDirections(), StaticCast<int32>(FVariableAreaDoorSheet::EDoorOpenDirection::E_Right)))
			{
				DoorSheets[1]->SetOpenDoorDirection(FVariableAreaDoorSheet::EDoorOpenDirection::E_Right);
			}

			if (GetHanleType() == EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND || GetHanleType() == EHandleMetalType::E_MT_None)
			{
				for (auto & InnerRef : DoorSheets[1]->GetMetals())
				{
					if (InnerRef->GetShapeActor())
					{
						InnerRef->DestroyShapeActor();
					}
				}
			}
			else
			{
				DoorSheets[1]->ReBuildHandle();
				DoorSheets[1]->RecalHandlePosition();
			}
		}
		//更新门组，重新计算门板的位置和尺寸
		UpdateDoorGroup();

	} while (false);
	
	return bRetResult;
}

FSideHungDoor::EDoorOpenType FSideHungDoor::GetDoorOpenType() const
{
	return GetDoorSheets()[0]->GetOpenDoorDirection() != FVariableAreaDoorSheet::EDoorOpenDirection::E_Top 
		? EDoorOpenType::E_HingedDoor : EDoorOpenType::E_TipUpDoor;
}

bool FSideHungDoor::SetDoorWidthScales(TArray<float> InWidthScales)
{
	DoorWidthScales = InWidthScales;
	//更新门板尺寸和位置
	if (DoorSheets.Num() == 2)
	{
		UpdateDoorGroup();
	}
	return true;
}

void FSideHungDoor::SetUpCorverType(int32 InType)
{
	UpCorverType = InType;
}

void FSideHungDoor::SetDownCorverType(int32 InType)
{
	DownCorverType = InType;
}

void FSideHungDoor::SetLeftCorverType(int32 InType)
{
	LeftCorverType = InType;
}

void FSideHungDoor::SetRightCorverType(int32 InType)
{
	RightCorverType = InType;
}

void FSideHungDoor::InsertDoorSheet(TSharedPtr<FVariableAreaDoorSheetForSideHungDoor> InDoorSheet)
{
	InDoorSheet->SetParentShape(this);
	AddChildShape(InDoorSheet);
	DoorSheets.Add(InDoorSheet);
}

bool FSideHungDoor::SetUpDownGapValue(float InValue)
{
	UpDownGap = InValue;
	//更新门板位置和尺寸
	UpdateDoorGroup();
	return true;
}

bool FSideHungDoor::SetLeftRightGapValue(float InValue)
{
	LeftRightGap = InValue;
	//更新门板位置和尺寸
	UpdateDoorGroup();
	return true;
}

bool FSideHungDoor::SetBackGapValue(float InValue)
{
	BackGap = InValue;
	//更新门板位置和尺寸
	UpdateDoorGroup();
	return true;
}

void FSideHungDoor::DeleteDoorSheetAt(int32 InIndex)
{
	TSharedPtr<FSCTShape> CurDoorSheet = DoorSheets[InIndex];
	RemoveChildShape(CurDoorSheet);
	DoorSheets.RemoveAt(InIndex);
}

void FSideHungDoor::UpdateDoorGroup()
{	
	//当前门组尺寸
	float DoorGroupWidth = GetShapeWidth() + LeftExtensionValue + RightExtensionValue;
	float DoorGroupDepth = GetShapeDepth();
	float DoorGroupHeight = GetShapeHeight() + UpExtensionValue + DownExtensionValue;
	//当前门组的位置
	float DoorGroupPosX = -LeftExtensionValue;
	float DoorGroupPosY = 0.0;
	float DoorGroupPosZ = -DownExtensionValue;

	auto GetCurHanleShape = [this]()->TSharedPtr<FAccessoryShape>
	{
		TSharedPtr<FAccessoryShape> RetShape = nullptr;
		const int32 Id = CurrentHandleLocationInfo.ID;
		for (auto & Ref : Metals)
		{
			if (Ref->GetShapeId() == Id)
			{
				RetShape = Ref;
				break;
			}
		}
		return 	RetShape;
	};

	// 箱体拉手
	if(GetHanleType() == EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND)
	{
		// 设置箱体拉手的空间位置以及尺寸
		auto SetHanlePositionAndSize = [this](TSharedPtr<FAccessoryShape> & Shape,const float InPosX, const float InPosY, const float InPosZ, const float InW)
		{					
			check(Shape.IsValid());
			Shape->SetShapePosX(InPosX);
			Shape->SetShapePosY(InPosY);
			Shape->SetShapePosZ(InPosZ);
			Shape->SetShapeWidth(InW);
		};

		float AccessPosX = -LeftExtensionValue;
		float AccessPosY = 0.0f;
		float AccessPosZ = 0.0f;
		float AccessW = DoorGroupWidth;		
		if (FrontBoard.IsValid())
		{
			// 处理切角
			if (FrontBoardCornerType == EFrontBoardCornerType::E_LeftTopCorner && 
				FrontPositionType == EFrontBoardPositionType::E_DoorRight )
			{
				AccessW = DoorGroupWidth - FrontBoardWidth + FrontBoardCornerCutWidth;				
			}			
			else if (FrontBoardCornerType == EFrontBoardCornerType::E_RithtTopCorner &&
				FrontPositionType == EFrontBoardPositionType::E_DoorLeft)
			{
				AccessW = DoorGroupWidth - FrontBoardWidth + FrontBoardCornerCutWidth;
				AccessPosX +=  -FrontBoardCornerCutWidth;
			}
			else
			{
				AccessW = DoorGroupWidth - FrontBoardWidth;
			}
			// 修正门板在左侧时候的，箱体拉手位置错误的bug
			if (FrontPositionType == EFrontBoardPositionType::E_DoorLeft)
			{
				AccessPosX += FrontBoardWidth;
			}
		}
		if (CurrentHandleLocationInfo.ID != -1 && CurrentHandleLocationInfo.Location.IsValid())
		{
			AccessW += StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->LeftExtend + StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->RightExtend;
			AccessPosX -= StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->LeftExtend;
		}
		// 地柜上拉手
		if (BoxHanleSubType == ELateralBoxShakeHandSubType::E_LBSH_FLOOR_CUPBOARD_HANDLE)
		{
			TSharedPtr<FAccessoryShape> DesAccessShape = GetCurHanleShape();
			AccessPosX += AccessW / 2.0f;			
			AccessPosZ += DoorGroupHeight - DownExtensionValue - DesAccessShape->GetShapeHeight() + UpExtensionValue;
			SetHanlePositionAndSize(DesAccessShape,AccessPosX, AccessPosY, AccessPosZ, AccessW);
			DoorGroupHeight = DoorGroupHeight - (DesAccessShape->GetShapeHeight() - StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->VposVAl);
		}
		//吊柜下拉手
		else if (BoxHanleSubType == ELateralBoxShakeHandSubType::E_LBSH_WALL_CUPBOARD_HANDLE)
		{
			TSharedPtr<FAccessoryShape> DesAccessShape = GetCurHanleShape();
			AccessPosX += AccessW / 2.0f;
			AccessPosZ +=  DesAccessShape->GetShapeHeight() - DownExtensionValue;
			SetHanlePositionAndSize(DesAccessShape, AccessPosX, AccessPosY, AccessPosZ, AccessW);
			DoorGroupHeight = DoorGroupHeight - (DesAccessShape->GetShapeHeight() - StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->VposVAl);
			DoorGroupPosZ += DesAccessShape->GetShapeHeight() - StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->VposVAl;
		}
		// 横装C拉手
		else if (BoxHanleSubType == ELateralBoxShakeHandSubType::LBSH_C_HANDL)
		{
			TSharedPtr<FAccessoryShape> DesAccessShape = GetCurHanleShape();
			AccessPosX += AccessW / 2.0f;						
			DoorGroupHeight = DoorGroupHeight - (DesAccessShape->GetShapeHeight() -2.0 * StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->VposVAl);
			AccessPosZ += GetShapeHeight() - (DesAccessShape->GetShapeHeight() - StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->VposVAl) + UpExtensionValue;
			SetHanlePositionAndSize(DesAccessShape, AccessPosX, AccessPosY, AccessPosZ, AccessW);
		}
	}

	// 封板
	if (FrontBoard.IsValid())
	{
		float FrontBoardPosX = 0.0f;
		float FrontBoardPosY = 0.0f;
		float FrontBoardPosZ = -GetFrontBoardBottomExtern();
		float FrontBoardWidth = GetFrontBoardWidth();
		float FrontBoardHeight = GetFrontBoardHeight() + GetFrontBoardBottomExtern() + GetFrontBoardTopExtern();
		float FrontBoardDepth = GetFrontBoardDepth();
		
		switch (GetFrontBoardPositionType())
		{
			case EFrontBoardPositionType::E_None:
			{
				FrontPositionType = EFrontBoardPositionType::E_DoorLeft;
				// 封板位置
				FrontBoardPosX = -LeftExtensionValue;
				FrontBoardWidth += LeftExtensionValue;
				// 修正门板位置
				DoorGroupPosX = FrontBoardPosX + GetFrontBoardWidth();
				DoorGroupWidth = GetShapeWidth() + RightExtensionValue - GetFrontBoardWidth();
				break;
			}
			case  EFrontBoardPositionType::E_DoorLeft:
			{
				// 封板位置
				FrontBoardPosX = -LeftExtensionValue;
				FrontBoardWidth += LeftExtensionValue;				
				// 修正门板位置
				DoorGroupPosX = FrontBoardPosX + GetFrontBoardWidth();
				DoorGroupWidth = GetShapeWidth() + RightExtensionValue - GetFrontBoardWidth();
			}
			break;
			case EFrontBoardPositionType::E_DoorRight:
			{
				// 修正门板位置				
				DoorGroupWidth = GetShapeWidth() + LeftExtensionValue - GetFrontBoardWidth();
				// 封板位置
				FrontBoardWidth += RightExtensionValue;				
				FrontBoardPosX = DoorGroupPosX + DoorGroupWidth;												
			}
			break;
			default:
			check(false);
		}
		
		FrontBoard->SetShapePosX(FrontBoardPosX);		
		FrontBoard->SetShapePosZ(FrontBoardPosZ + FrontBoardHeight);
		FrontBoard->SetShapeWidth(FrontBoardWidth);
		FrontBoard->SetShapeHeight(FrontBoardDepth);
		FrontBoard->SetShapeDepth(FrontBoardHeight);
		if (FrontBoard->GetMaterialData().ID == 0)
		{
			SetFrontBoardMaterial(GetFrontBoardMaterial());
		}
		if (AssistBoard.IsValid())
		{
			float AssistBoardPosX = FrontBoardPosX + GetAssistBoardDepth() + GetAssistBoardLengthToFrontBoard();
			if (GetFrontBoardPositionType() == EFrontBoardPositionType::E_DoorLeft)
			{
				AssistBoardPosX = FrontBoardPosX + GetFrontBoardWidth() - GetAssistBoardLengthToFrontBoard();
			}
			const float AssistBoardPosY = -GetAssistBoardWidth();
			const float AssistBoardPosZ = -GetAssistBoardBottomExtern();
			const float AssistBoardWidth = GetAssistBoardWidth();
			const float AssistBoardHeight = GetAssistBoardHeight() + GetAssistBoardTopExtern() + GetAssistBoardBottomExtern();
			const float AssistBoardDepth = GetAssistBoardDepth();
			AssistBoard->SetShapePosX(AssistBoardPosX);
			AssistBoard->SetShapePosY(AssistBoardPosY);
			AssistBoard->SetShapePosZ(AssistBoardPosZ);
			AssistBoard->SetShapeWidth(AssistBoardHeight);
			AssistBoard->SetShapeHeight(AssistBoardDepth);
			AssistBoard->SetShapeDepth(AssistBoardWidth);
			if (AssistBoard->GetMaterialData().ID == 0)
			{
				SetAssistBoardMaterial(GetAssistBoardMaterial());
			}
		}		
	}

	// 自动调整门板数量，根据当前门组的宽度
	if (AutoModifyDoorSheetCountByDoorGroupWidth() == false) return;
	int32 CurDoorCount = DoorSheets.Num();
	auto GetDoorSheetEdgeHanleShape = [this](const int32 InDoorIndex)->TSharedPtr<FAccessoryShape>
	{
		TSharedPtr<FAccessoryShape> RetShape = nullptr;
		const int32 Id = DoorSheets[InDoorIndex]->GetCurrentHandleInfo().ID;
		for (auto & Ref : DoorSheets[InDoorIndex]->GetMetals())
		{
			if (Ref->GetShapeId() == Id)
			{
				RetShape = Ref;
				break;
			}
		}
		return 	RetShape;
	};	

	//更新门板的位置和尺寸
	if (CurDoorCount == 1)
	{
		TSharedPtr<FVariableAreaDoorSheet> CurDoorSheet = DoorSheets[0];
		//设置门板的位置和尺寸
		CurDoorSheet->SetShapePosX(DoorGroupPosX + LeftRightGap);
		CurDoorSheet->SetShapePosY(DoorGroupPosY + BackGap);		
		CurDoorSheet->SetShapeWidth(DoorGroupWidth - LeftRightGap * 2);

		// 封边拉手
		if (GetHanleType() == EHandleMetalType::E_MT_SEALING_SIDE_HANDLE)
		{
			TSharedPtr<FAccessoryShape> AccessShape = GetDoorSheetEdgeHanleShape(0);
			if (AccessShape.IsValid())
			{
				const float HanleW = AccessShape->GetShapeWidth();
				const float HanleH = AccessShape->GetShapeHeight();
				float  InstallHeight = HanleH;
				for (const auto & Ref : AccessShape->GetMetalsPropertyList())
				{
					if (FMath::IsNearlyEqual(FCString::Atof(*Ref.propertyId), 12))
					{
						InstallHeight = FCString::Atof(*Ref.propertyValue);
						break;
					}
				}
				TSharedPtr<FDoorHanlerDefaultLocation> Loacation = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DoorSheets[0]->GetCurrentHandleInfo().Location);
				if (Loacation.IsValid())
				{
					AccessShape->SetShapeWidth(CurDoorSheet->GetShapeWidth());
					AccessShape->SetShapePosX(CurDoorSheet->GetShapeWidth() / 2.0f);
					AccessShape->SetShapePosY(AccessShape->GetShapeDepth() / 2.0f);
					// 封边拉手处于门板上面
					if (Loacation->Vpos == 1)
					{
						AccessShape->SetShapePosZ(DoorGroupHeight - InstallHeight - 2.0f * UpDownGap);
						AccessShape->SetShapeRotY(0.0f);
					}
					// 封边拉手处于门板下面
					else if (Loacation->Vpos == 2)
					{
						AccessShape->SetShapePosZ(0.0f);
						DoorGroupPosZ += InstallHeight;
						AccessShape->SetShapeRotY(180.0f);
					}
				}
				DoorGroupHeight = DoorGroupHeight - InstallHeight;
			}
		}
		CurDoorSheet->SetShapePosZ(DoorGroupPosZ + UpDownGap);			
		CurDoorSheet->SetShapeHeight(DoorGroupHeight - UpDownGap * 2);
	}
	else if (CurDoorCount == 2)
	{
		//计算左右门板的尺寸
		float DoorSheetWidth = DoorGroupWidth - LeftRightGap * 4;
		check(DoorWidthScales.Num() == 2);
		float TotalScales = DoorWidthScales[0] + DoorWidthScales[1];
		float LeftDoorWidth = DoorSheetWidth * DoorWidthScales[0] / TotalScales;
		float RightDoorWidth = DoorSheetWidth * DoorWidthScales[1] / TotalScales;

		TSharedPtr<FVariableAreaDoorSheet> LeftDoorSheet = DoorSheets[0];
		TSharedPtr<FVariableAreaDoorSheet> RightDoorSheet = DoorSheets[1];
		
		//设置左右门板的位置和尺寸
		LeftDoorSheet->SetShapePosX(DoorGroupPosX + LeftRightGap);
		LeftDoorSheet->SetShapePosY(DoorGroupPosY + BackGap);		
		LeftDoorSheet->SetShapeWidth(LeftDoorWidth);
		
		if (GetHanleType() == EHandleMetalType::E_MT_SEALING_SIDE_HANDLE)
		{
			TSharedPtr<FAccessoryShape> AccessShape = GetDoorSheetEdgeHanleShape(0);
			if (AccessShape.IsValid())
			{
				const float HanleW = AccessShape->GetShapeWidth();
				const float HanleH = AccessShape->GetShapeHeight();
				float InstallHeight = HanleH;
				for (const auto & Ref : AccessShape->GetMetalsPropertyList())
				{
					if (FMath::IsNearlyEqual(FCString::Atof(*Ref.propertyId), 12))
					{
						InstallHeight = FCString::Atof(*Ref.propertyValue);
						break;
					}
				}
				TSharedPtr<FDoorHanlerDefaultLocation> Loacation = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DoorSheets[0]->GetCurrentHandleInfo().Location);
				if (Loacation.IsValid())
				{
					AccessShape->SetShapePosX(LeftDoorWidth / 2.0f);
					AccessShape->SetShapeWidth(LeftDoorWidth);
					AccessShape->SetShapePosY(AccessShape->GetShapeDepth() / 2.0f);
					// 封边拉手处于门板上面
					if (Loacation->Vpos == 1)
					{
						AccessShape->SetShapePosZ(DoorGroupHeight - 2.0f * UpDownGap - InstallHeight);
						AccessShape->SetShapeRotY(0.0f);
					}
					// 封边拉手处于门板下面
					else if (Loacation->Vpos == 2)
					{
						AccessShape->SetShapePosZ(0.0f);
						DoorGroupPosZ += InstallHeight;
						AccessShape->SetShapeRotY(180.0f);
					}
				}
				DoorGroupHeight = DoorGroupHeight - InstallHeight;
			}
		}
		LeftDoorSheet->SetShapePosZ(DoorGroupPosZ + UpDownGap);
		LeftDoorSheet->SetShapeHeight(DoorGroupHeight - UpDownGap * 2);

		RightDoorSheet->SetShapePosX(DoorGroupPosX + LeftRightGap * 3 + LeftDoorWidth);
		RightDoorSheet->SetShapePosY(DoorGroupPosY + BackGap);	
		RightDoorSheet->SetShapeWidth(RightDoorWidth);	
		if (GetHanleType() == EHandleMetalType::E_MT_SEALING_SIDE_HANDLE)
		{
			TSharedPtr<FAccessoryShape> AccessShape = GetDoorSheetEdgeHanleShape(1);
			if (AccessShape.IsValid())
			{
				const float HanleW = AccessShape->GetShapeWidth();
				const float HanleH = AccessShape->GetShapeHeight();
				TSharedPtr<FDoorHanlerDefaultLocation> Loacation = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DoorSheets[0]->GetCurrentHandleInfo().Location);
				if (Loacation.IsValid())
				{
					AccessShape->SetShapePosX(RightDoorWidth / 2.0f);
					AccessShape->SetShapeWidth(RightDoorWidth);
					AccessShape->SetShapePosY(AccessShape->GetShapeDepth() / 2.0f);
					// 封边拉手处于门板上面
					if (Loacation->Vpos == 1)
					{
						AccessShape->SetShapePosZ(DoorGroupHeight - 2.0f * UpDownGap);
						AccessShape->SetShapeRotY(0.0f);
					}
					// 封边拉手处于门板下面														
					else if (Loacation->Vpos == 2)
					{
						AccessShape->SetShapePosZ(0.0f);
						AccessShape->SetShapeRotY(180.0f);
					}
				}
			}
		}
		RightDoorSheet->SetShapePosZ(DoorGroupPosZ + UpDownGap);		
		RightDoorSheet->SetShapeHeight(DoorGroupHeight - UpDownGap * 2);

		//修改对开门的开门方式
		auto CheckAvailableOpenDirection = [](const TArray<FVariableAreaDoorSheet::EDoorOpenDirection> & InArray,const int32 InIndex)->bool
		{
			return InArray.IsValidIndex(InIndex);
		};
		//如果后台没有给该门设置支持的开门方向，则不进行左右开门方向的设置
		if (CheckAvailableOpenDirection(DoorSheets[0]->GetAvailableOpenDirections(), StaticCast<int32>(FVariableAreaDoorSheet::EDoorOpenDirection::E_Left)))
		{
			DoorSheets[0]->SetOpenDoorDirection(FVariableAreaDoorSheet::EDoorOpenDirection::E_Left); 
		}
		if (CheckAvailableOpenDirection(DoorSheets[1]->GetAvailableOpenDirections(), StaticCast<int32>(FVariableAreaDoorSheet::EDoorOpenDirection::E_Right)))
		{
			DoorSheets[1]->SetOpenDoorDirection(FVariableAreaDoorSheet::EDoorOpenDirection::E_Right);
		}				
	} 
}

bool FSideHungDoor::SetHanleType(const EHandleMetalType InHanleType)
{
	bool RetValue = false;
	do
	{
		if (HanleType == InHanleType) break;
		HanleType = InHanleType;
		for (auto & Ref : DoorSheets)
		{
			for (auto & InnerRef : Ref->GetMetals())
			{
				if (InnerRef->GetShapeActor())
				{
					InnerRef->DestroyShapeActor();
				}
			}
		}
		for (auto & Ref : Metals)
		{
			if (Ref->GetShapeActor())
			{
				Ref->DestroyShapeActor();
			}
		}
		// 如果是箱体拉手
		if (HanleType == EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND)
		{			
			// 激活当前的箱体拉手
			for (auto & Ref : Metals)
			{
				check(Ref->GetShapeActor() == nullptr);
				if (Ref->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_LATERAL_BOX_SHAKE_HAND) && ShapeActor)
				{
					Ref->SpawnShapeActor();
					Ref->GetShapeActor()->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
				}
			}

			RetValue = true;
		}
		// 封边拉手
		else if (HanleType == EHandleMetalType::E_MT_SEALING_SIDE_HANDLE)
		{						
			RetValue = true;
		}
		// 如果是明装拉手
		else if (HanleType == EHandleMetalType::E_MT_SHAKE_HANDS)
		{																
			RetValue = true;
		}
		// 无拉手
		else if (HanleType == EHandleMetalType::E_MT_None)
		{
			RetValue = true;
		}

	} while (false);	

	return RetValue;
}

bool FSideHungDoor::SetHanleZValue(const float InValue)
{
	auto GetCurHanleShape = [this]()->TSharedPtr<FAccessoryShape>
	{
		TSharedPtr<FAccessoryShape> RetShape = nullptr;
		const int32 Id = CurrentHandleLocationInfo.ID;
		for (auto & Ref : Metals)
		{
			if (Ref->GetShapeId() == Id)
			{
				RetShape = Ref;
				break;
			}
		}
		return 	RetShape;
	};
	bool RetValue = false;
	do
	{
		if (GetCurHanleShape().IsValid() == false)
		{			
			break;
		}	
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->VposVAl = InValue;
		RetValue = true;
	} while (false);	
	return RetValue;
}

void FSideHungDoor::ReBuildHandle(const int32 InID)
{
	TSharedPtr<FAccessoryShape> CurMetal;
	const EMetalsType DesType = EMetalsType::MT_LATERAL_BOX_SHAKE_HAND;
	for (const auto & Ref : Metals)
	{
		if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == DesType)
		{
			CurMetal = Ref;
			break;
		}
	}
	
	check(CurMetal.IsValid());
	TSharedPtr<FAccessoryShape> AccessShape = StaticCastSharedPtr<FAccessoryShape>(*(GetOptionShapeMap().Find(InID)));
	TSharedPtr<FAccessoryShape> NewShape = StaticCastSharedPtr<FAccessoryShape>(FSCTShapeManager::Get()->CopyShapeToNew(AccessShape));
	NewShape->SetShapePosX(CurMetal->GetShapePosX());
	NewShape->SetShapePosY(CurMetal->GetShapePosY());
	NewShape->SetShapePosZ(CurMetal->GetShapePosZ());
	NewShape->LoadModelMesh(true);
	AddChildShape(NewShape);
	Metals.Add(NewShape);
	FDoorSheetShapeBase::FAuxiliaryMetalInfo & CurHandleInfo = GetCurrentHandleInfo();
	CurHandleInfo.ID = InID;
	CurHandleInfo.Name = AccessShape->GetShapeName();
	if (ShapeActor)
	{
		NewShape->SpawnShapeActor();
		NewShape->GetShapeActor()->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	Metals.Remove(CurMetal);
	RemoveChildShape(CurMetal);
	CurMetal->DestroyShapeActor();
	if (NewShape->GetMetalsPropertyList().Num() > 0)
	{
		const FString & PropertyValueIdentity = NewShape->GetMetalsPropertyList()[0].PropertyValueIdentity;
		if (PropertyValueIdentity == TEXT("c5164a"))
		{
			BoxHanleSubType = ELateralBoxShakeHandSubType::E_LBSH_WALL_CUPBOARD_HANDLE;
		}
		else if (PropertyValueIdentity == TEXT("MIQHn1"))
		{
			BoxHanleSubType = ELateralBoxShakeHandSubType::E_LBSH_FLOOR_CUPBOARD_HANDLE;
		}
		else if (PropertyValueIdentity == TEXT("0Xkx6p"))
		{
			BoxHanleSubType = ELateralBoxShakeHandSubType::LBSH_C_HANDL;
		}
	}
	for (auto & Ref : GetDoorSheets())
	{
		Ref->GetCurrentHandleInfo().ID = -1;
		Ref->GetCurrentHandleInfo().Location = nullptr;
		Ref->ReBuildHandle();
	}
}

void FSideHungDoor::ReSetFrontBoard()
{
	if (FrontBoard.IsValid())
	{
		//RemoveChildShape(FrontBoard);
		FrontBoard = nullptr;
	}	
}

void FSideHungDoor::SetFrontBoard()
{
	FrontBoard = MakeShareable(new FBoardShape);
	FrontBoard->SetBoardShapeType(BoST_Rectangle);
	FrontBoard->SetShapeCategory(BUT_BackBoard);
	FrontBoard->SetShapeRotX(90.0f);		
	FrontBoard->SetShapeWidth(GetFrontBoardWidth());
	FrontBoard->SetShapeHeight(GetFrontBoardDepth());
	FrontBoard->SetShapeDepth(GetFrontBoardHeight());
	// 设置材质
	{
		FPakMetaData Material;
		Material.ID = FrontBoardMaterial.ID;
		Material.Name = FrontBoardMaterial.Name;
		Material.Url = FrontBoardMaterial.Url;
		Material.MD5 = FrontBoardMaterial.MD5;
		Material.ThumbnailUrl = FrontBoardMaterial.ThumbnailUrl;
		Material.OptimizeParam = FrontBoardMaterial.OptimizeParam;
		FrontBoard->SetMaterialData(Material);
		FrontBoard->SetMaterialRotateValue(90.0f);
	}
}

void FSideHungDoor::ReSetAssistBoard()
{
	if (AssistBoard.IsValid())
	{
		//RemoveChildShape(AssistBoard);
		AssistBoard = nullptr;
	}
}

void FSideHungDoor::SetAssistBoard()
{
	AssistBoard = MakeShareable(new FBoardShape);
	AssistBoard->SetBoardShapeType(BoST_Rectangle);
	AssistBoard->SetShapeCategory(BUT_LeftBoard);
	AssistBoard->SetShapeRotY(90.0f);
	AssistBoard->SetParentShape(this);
	AssistBoard->SetShapeWidth(GetAssistBoardDepth());
	AssistBoard->SetShapeHeight(GetAssistBoardHeight());
	AssistBoard->SetShapeDepth(GetAssistBoardWidth());
	// 设置材质
	{
		FPakMetaData Material;
		Material.ID = FrontBoardMaterial.ID;
		Material.Name = FrontBoardMaterial.Name;
		Material.Url = FrontBoardMaterial.Url;
		Material.MD5 = FrontBoardMaterial.MD5;
		Material.ThumbnailUrl = FrontBoardMaterial.ThumbnailUrl;
		Material.OptimizeParam = FrontBoardMaterial.OptimizeParam;
		AssistBoard->SetMaterialData(Material);
	}

}

void FSideHungDoor::ProcessOptionsMetalType()
{
	for (const auto & Ref : OpetionsMetalsShapesList)
	{
		if (Ref.Value->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_LATERAL_BOX_SHAKE_HAND))
		{
			FDoorSheetShapeBase::FAuxiliaryMetalInfo TempMetalInfo;
			TempMetalInfo.ID = Ref.Key;
			TempMetalInfo.Name = Ref.Value->GetShapeName();
			HandleList.Add(TempMetalInfo);
		}		
	}
	HanleType = EHandleMetalType::E_MT_SHAKE_HANDS;
	for (const auto & Ref : Metals)
	{
		if (Ref->GetShapeType() != ST_Accessory) continue;
		
		if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_LATERAL_BOX_SHAKE_HAND)
		{
			CurrentHandleLocationInfo.ID = Ref->GetShapeId();
			CurrentHandleLocationInfo.Name = Ref->GetShapeName();
			CurrentHandleLocationInfo.Location = MakeShareable(new FDoorHanlerDefaultLocation);
			StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->Vpos = 3;
			StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->VposVAl = 20.0f;
			StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->MetalType = StaticCast<EMetalsType>(Ref->GetShapeCategory());		
			if (Ref->GetMetalsPropertyList().Num() > 0)
			{
				const FString & PropertyValueIdentity = Ref->GetMetalsPropertyList()[0].PropertyValueIdentity;
				if (PropertyValueIdentity == TEXT("c5164a"))
				{
					BoxHanleSubType = ELateralBoxShakeHandSubType::E_LBSH_WALL_CUPBOARD_HANDLE;
				}
				else if (PropertyValueIdentity == TEXT("MIQHn1"))
				{
					BoxHanleSubType = ELateralBoxShakeHandSubType::E_LBSH_FLOOR_CUPBOARD_HANDLE;
				}
				else if (PropertyValueIdentity == TEXT("0Xkx6p"))
				{
					BoxHanleSubType = ELateralBoxShakeHandSubType::LBSH_C_HANDL;
				}
			}
			HanleType = EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND;
		}		
	}
}

bool FSideHungDoor::AutoModifyDoorSheetCountByDoorGroupWidth()
{
	bool bRetResult = false;
	do
	{		
		bRetResult = ShapeActor != nullptr;
		if (!bRetResult) break;
		bool bIsSingleDoorValid = false;
		bool bIsDoubleDoorValid = false;
		IsSingleAndDoubleValid(GetShapeWidth(), bIsSingleDoorValid, bIsDoubleDoorValid);
		bRetResult = bIsSingleDoorValid || bIsDoubleDoorValid;
		if (!bRetResult) break;

		// 插角封板尺寸发生变化后，重新计算门组中门板的数量
		const bool bSingleDoor = GetDoorSheetNum() == 1;
		const bool bDoubleDoor = GetDoorSheetNum() == 2;
		// 门板数量设置
		if (bDoubleDoor && !bIsDoubleDoorValid)
		{
			DeleteDoorSheetAt(1);
		}
		else if (bSingleDoor && !bIsSingleDoorValid)
		{
			check(DoorSheets.Num() == 1);
			TSharedPtr<FVariableAreaDoorSheetForSideHungDoor> NewDoorSheet = MakeShareable(new FVariableAreaDoorSheetForSideHungDoor());
			DoorSheets[0]->CopyTo(NewDoorSheet.Get());			
			ASCTShapeActor* NewDoorActor = NewDoorSheet->SpawnShapeActor();
			NewDoorActor->AttachToActorOverride(GetShapeActor(), FAttachmentTransformRules::KeepRelativeTransform);
			InsertDoorSheet(NewDoorSheet);

			//修改对开门的开门方式
			auto CheckAvailableOpenDirection = [](const TArray<FVariableAreaDoorSheet::EDoorOpenDirection> & InArray, const int32 InIndex)->bool
			{
				return InArray.IsValidIndex(InIndex);
			};
			//如果后台没有给该门设置支持的开门方向，则不进行左右开门方向的设置
			if (CheckAvailableOpenDirection(DoorSheets[0]->GetAvailableOpenDirections(), StaticCast<int32>(FVariableAreaDoorSheet::EDoorOpenDirection::E_Left)))
			{
				DoorSheets[0]->SetOpenDoorDirection(FVariableAreaDoorSheet::EDoorOpenDirection::E_Left);
			}
			if (CheckAvailableOpenDirection(DoorSheets[1]->GetAvailableOpenDirections(), StaticCast<int32>(FVariableAreaDoorSheet::EDoorOpenDirection::E_Right)))
			{
				DoorSheets[1]->SetOpenDoorDirection(FVariableAreaDoorSheet::EDoorOpenDirection::E_Right);
			}

			if (GetHanleType() == EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND || GetHanleType() == EHandleMetalType::E_MT_None)
			{
				for (auto & InnerRef : DoorSheets[1]->GetMetals())
				{
					if (InnerRef->GetShapeActor())
					{
						InnerRef->DestroyShapeActor();
					}
				}
			}
			else
			{
				DoorSheets[1]->ReBuildHandle();
				DoorSheets[1]->RecalHandlePosition();
			}
		}

	} while (false);
	return  bRetResult;
}

void FSideHungDoor::GetSingleDoorSheetRange(float & OutMin, float & OutMax, float OutCur)
{
	OutCur = DoorSheets[0]->GetShapeWidth();
	TSharedPtr<FShapeAttribute> DoorWidthAttri = DoorSheets[0]->GetShapeWidthAttri();
	if (SAT_NumberRange == DoorWidthAttri->GetAttributeType())
	{
		TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(DoorWidthAttri);
		OutMin = NumberRangeAttri->GetMinValue();
		OutMax = NumberRangeAttri->GetMaxValue();
	}
	else
	{
		OutMin = OutCur;
		OutMax = OutCur;
	}
}

void FSideHungDoor::IsSingleAndDoubleValid(const float InW, bool & OutSingleDoorSheet, bool & OutDoubleSheet)
{
	float MinDoorSheetWidth = 0.0f, MaxDoorSheetWidth = 0.0f, CurSingleDoorSheetWidth = 0.0f;
	GetSingleDoorSheetRange(MinDoorSheetWidth, MaxDoorSheetWidth, CurSingleDoorSheetWidth);
	const float DoorSheetWidthExtenSize = GetFrontBoardPositionType() == FSideHungDoor::EFrontBoardPositionType::E_DoorLeft ? GetRightExtensionValue() : GetLeftExtensionValue();
	const float DoorSheetsWidth = InW - GetFrontBoardWidth() + DoorSheetWidthExtenSize;
	{
		const float TempDoorSheetWidth = DoorSheetsWidth - (2.0f * GetLeftRightGapValue());
		OutSingleDoorSheet = TempDoorSheetWidth >= MinDoorSheetWidth && TempDoorSheetWidth <= MaxDoorSheetWidth;
	}
	{
		const float TempDoorSheetWidth = (DoorSheetsWidth - 2.0f * (2.0f * GetLeftRightGapValue())) / 2.0f;
		OutDoubleSheet = TempDoorSheetWidth >= MinDoorSheetWidth && TempDoorSheetWidth <= MaxDoorSheetWidth;
	}
}


void FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::CopyTo(FSCTShape * OutShape)
{
	check(OutShape);
	FVariableAreaDoorSheet::CopyTo(OutShape);
	FVariableAreaDoorSheetForDrawer * OutBaseShape = StaticCast<FVariableAreaDoorSheetForDrawer*>(OutShape);
	OutBaseShape->LockList = LockList;
	OutBaseShape->HandleList = HandleList;
	OutBaseShape->DefaultHandleLocationInfo.ID = DefaultHandleLocationInfo.ID;
	OutBaseShape->DefaultHandleLocationInfo.Name = DefaultHandleLocationInfo.Name;
	if (DefaultHandleLocationInfo.Location.IsValid())
	{
		OutBaseShape->DefaultHandleLocationInfo.Location = MakeShareable(new FDoorHanlerDefaultLocation);
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutBaseShape->DefaultHandleLocationInfo.Location)->Hpos = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultHandleLocationInfo.Location)->Hpos;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutBaseShape->DefaultHandleLocationInfo.Location)->HposVal = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultHandleLocationInfo.Location)->HposVal;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutBaseShape->DefaultHandleLocationInfo.Location)->Vpos = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultHandleLocationInfo.Location)->Vpos;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutBaseShape->DefaultHandleLocationInfo.Location)->VposVAl = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultHandleLocationInfo.Location)->VposVAl;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutBaseShape->DefaultHandleLocationInfo.Location)->RotateDirection = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultHandleLocationInfo.Location)->RotateDirection;
	}
	OutBaseShape->DefaultLockLocationInfo.ID = DefaultLockLocationInfo.ID;
	OutBaseShape->DefaultLockLocationInfo.Name = DefaultLockLocationInfo.Name;
	if (DefaultLockLocationInfo.Location.IsValid())
	{
		OutBaseShape->DefaultLockLocationInfo.Location = MakeShareable(new FDoorHanlerDefaultLocation);
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutBaseShape->DefaultLockLocationInfo.Location)->Hpos = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultLockLocationInfo.Location)->Hpos;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutBaseShape->DefaultLockLocationInfo.Location)->HposVal = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultLockLocationInfo.Location)->HposVal;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutBaseShape->DefaultLockLocationInfo.Location)->Vpos = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultLockLocationInfo.Location)->Vpos;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutBaseShape->DefaultLockLocationInfo.Location)->VposVAl = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultLockLocationInfo.Location)->VposVAl;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutBaseShape->DefaultLockLocationInfo.Location)->RotateDirection = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultLockLocationInfo.Location)->RotateDirection;
	}	
}

void FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::ReCalDividBlocksPositionAndSize()
{
	FVariableAreaDoorSheet::ReCalDividBlocksPositionAndSize();	
	RecalLockPosition();
	RecalHandlePosition();
}

void FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::UpdateMaterial(UMaterialInterface * InMaterialInterface)
{
	bHasOptionalMaterial = true;
	for (const auto Ref : DividBlocks)
	{
		ASCTModelActor * ModelActor = StaticCast<ASCTModelActor *>(Ref->GetShapeActor());
		if (!ModelActor) return;
		ModelActor->ChangeMaterial(InMaterialInterface);		
	}
	ReCalDividBlocksPositionAndSize();
}

FDoorSheetShapeBase::FAuxiliaryMetalInfo FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::GetDefaultLockInfo() const
{
	FAuxiliaryMetalInfo RetValue;
	if (DefaultLockLocationInfo.ID != -1)
	{
		RetValue = DefaultLockLocationInfo;
	}
	else
	{	
		for (const auto & Ref : GetMetals())
		{
			if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_LOCK)
			{
				RetValue.ID = Ref->GetShapeId();
				RetValue.Name = Ref->GetShapeName();
				break;
			}
		}
	}
	return RetValue;
}

FDoorSheetShapeBase::FAuxiliaryMetalInfo FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::GetDefaultHandleInfo() const
{
	FAuxiliaryMetalInfo RetValue;
	if (DefaultHandleLocationInfo.ID != -1)
	{
		RetValue = DefaultHandleLocationInfo;
	}
	else
	{
		bool bHasCommonHanle = false;
		for (const auto & Ref : GetMetals())
		{
			if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_SHAKE_HANDS)
			{
				RetValue.ID = Ref->GetShapeId();
				RetValue.Name = Ref->GetShapeName();
				bHasCommonHanle = true;
				break;
			}
		}
		if (!bHasCommonHanle)
		{
			for (const auto & Ref : GetMetals())
			{
				if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_SEALING_SIDE_HANDLE)
				{
					RetValue.ID = Ref->GetShapeId();
					RetValue.Name = Ref->GetShapeName();					
					break;
				}
			}
		}
	}
	return RetValue;
}

bool FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::SetHandlePosX(const FString & InStrValue)
{
	bool bResult = true;
	if (HandleAttriX->GetAttributeStr() != InStrValue)
	{
		bResult = HandleAttriX->SetAttributeValue(InStrValue);
		if (bResult)
		{
			StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultHandleLocationInfo.Location)->HposVal = FCString::Atof(*InStrValue);
			RecalHandlePosition();
		}
	}
	return bResult;
}

bool FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::SetHandlePosZ(const FString & InStrValue)
{
	bool bResult = true;
	if (HandleAttriZ->GetAttributeStr() != InStrValue)
	{
		bResult = HandleAttriZ->SetAttributeValue(InStrValue);
		if (bResult)
		{
			StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultHandleLocationInfo.Location)->VposVAl = FCString::Atof(*InStrValue);
			RecalHandlePosition();
		}
	}
	return bResult;
}

bool FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::SetLockPosX(const FString & InStrValue)
{
	bool bResult = true;
	if (LockAttriX->GetAttributeStr() != InStrValue)
	{
		bResult = LockAttriX->SetAttributeValue(InStrValue);
		if (bResult)
		{
			RecalLockPosition();
		}
	}
	return bResult;
}

bool FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::SetLockPosZ(const FString & InStrValue)
{
	bool bResult = true;
	if (LockAttriZ->GetAttributeStr() != InStrValue)
	{
		bResult = LockAttriZ->SetAttributeValue(InStrValue);
		if (bResult)
		{
			RecalLockPosition();
		}
	}
	return bResult;
}

void FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::ReBuildHandleOrLock(const bool InIsHanle)
{	
	const int32 ShapeID = GetDefaultHandleInfo().ID;
	if (ShapeID == -1) return;
	// 先确定该ID的五金是否已经创建 	
	EMetalsType InMetalType = EMetalsType::MT_None;
	for (const auto & Ref : GetOptionShapeMap())
	{
		if (Ref.Key == ShapeID)
		{
			InMetalType = StaticCast<EMetalsType>(Ref.Value->GetShapeCategory());
			break;
		}
	}
	EMetalsType DesType = InMetalType;
	if (!InIsHanle)
	{
		DesType = EMetalsType::MT_LOCK;
	}

	// 删除互斥拉手		
	for (int32 Index = Metals.Num() - 1; Index >= 0; --Index)
	{
		if (InMetalType == EMetalsType::MT_SHAKE_HANDS && Metals[Index]->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SEALING_SIDE_HANDLE))
		{
			Metals.RemoveAt(Index);
		}
		else if (InMetalType == EMetalsType::MT_SEALING_SIDE_HANDLE && Metals[Index]->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SHAKE_HANDS))
		{
			Metals.RemoveAt(Index);
		}
	}

	TSharedPtr<FAccessoryShape> CurMetal;
	for (const auto & Ref : Metals)
	{
		if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == DesType)
		{
			CurMetal = Ref;
			break;
		}
	}
	if (CurMetal.IsValid())
	{		
		if (CurMetal->GetShapeId() != ShapeID)
		{
			TSharedPtr<FAccessoryShape> AccessShape = StaticCastSharedPtr<FAccessoryShape>(*(GetOptionShapeMap().Find(ShapeID)));
			TSharedPtr<FAccessoryShape> NewShape = StaticCastSharedPtr<FAccessoryShape>(FSCTShapeManager::Get()->CopyShapeToNew(AccessShape));
			NewShape->SetShapePosX(CurMetal->GetShapePosX());
			NewShape->SetShapePosY(CurMetal->GetShapePosY());
			NewShape->SetShapePosZ(CurMetal->GetShapePosZ());
			NewShape->SetShapeRotX(CurMetal->GetShapeRotX());
			NewShape->SetShapeRotY(CurMetal->GetShapeRotY());
			NewShape->SetShapeRotZ(CurMetal->GetShapeRotZ());
			NewShape->LoadModelMesh(true);
			AddChildShape(NewShape);
			Metals.Add(NewShape);
			FAuxiliaryMetalInfo CurHandleInfo = GetDefaultHandleInfo();
			CurHandleInfo.ID = ShapeID;
			CurHandleInfo.Name = AccessShape->GetShapeName();
			SetDefaultHandleInfo(CurHandleInfo);
			NewShape->SpawnShapeActor();
			NewShape->GetShapeActor()->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
			Metals.Remove(CurMetal);
			RemoveChildShape(CurMetal);
			CurMetal->DestroyShapeActor();
		}
		else if(CurMetal->GetShapeActor() == nullptr)
		{
			CurMetal->SpawnShapeActor();
			CurMetal->GetShapeActor()->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
	else
	{
		if (InIsHanle)
		{
			TSharedPtr<FAccessoryShape> AccessShape = StaticCastSharedPtr<FAccessoryShape>(*(GetOptionShapeMap().Find(ShapeID)));
			TSharedPtr<FAccessoryShape> NewShape = StaticCastSharedPtr<FAccessoryShape>(FSCTShapeManager::Get()->CopyShapeToNew(AccessShape));
			NewShape->LoadModelMesh(true);
			AddChildShape(NewShape);
			Metals.Add(NewShape);
			NewShape->SpawnShapeActor();
			NewShape->GetShapeActor()->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
}

void FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::RecalHandlePosition()
{
	if (DefaultHandleLocationInfo.ID == -1) return;
	if (DefaultHandleLocationInfo.Location.IsValid() == false) return;
	if (GetMetalTypeByID(DefaultHandleLocationInfo.ID) != EMetalsType::MT_SHAKE_HANDS) return;
	TSharedPtr<FAccessoryShape> OperationShape;
	for (const auto Ref : Metals)
	{
		if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_SHAKE_HANDS)
		{
			OperationShape = Ref;
			break;
		}
	}	
	if (OperationShape.IsValid() == false) return;
	switch (StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultHandleLocationInfo.Location)->Hpos)
	{
		case 1:
		{
			OperationShape->SetShapePosX(StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultHandleLocationInfo.Location)->HposVal);
		}break;
		case 2:
		{	
			OperationShape->SetShapePosX(GetShapeWidth() - StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultHandleLocationInfo.Location)->HposVal);
		}break;
		case 3:
		{
			OperationShape->SetShapePosX(GetShapeWidth() / 2.0f);		
		}break;	
	}
	switch (StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultHandleLocationInfo.Location)->Vpos)
	{
		case 1:
		{
			OperationShape->SetShapePosZ(GetShapeHeight() - StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultHandleLocationInfo.Location)->VposVAl);
		}break;
		case 2:
		{
			OperationShape->SetShapePosZ(StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultHandleLocationInfo.Location)->VposVAl);
		}break;
		case 3:
		{
			OperationShape->SetShapePosZ(GetShapeHeight() / 2.0f);		
		}break;		
	}	
	switch (StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultHandleLocationInfo.Location)->RotateDirection)
	{
		case 0:		
			OperationShape->SetShapeRotY(0.0f);
			break;
		case 1:		
			OperationShape->SetShapeRotY(90.0f);
			break;
		case 2:
			OperationShape->SetShapeRotY(-90.0f);		
			break;
		default:
			check(false);
			break;
	}
	OperationShape->SetShapePosY(GetShapeDepth());
	ShakeHanleTypePosition.HPositionType = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultHandleLocationInfo.Location)->Hpos;
	ShakeHanleTypePosition.HPositionValue = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultHandleLocationInfo.Location)->HposVal;
	ShakeHanleTypePosition.VPositionType = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultHandleLocationInfo.Location)->Vpos;
	ShakeHanleTypePosition.VPositionValue = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultHandleLocationInfo.Location)->VposVAl;
	ShakeHanleTypePosition.RotateDirection = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultHandleLocationInfo.Location)->RotateDirection;
}

void FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::RecalLockPosition()
{
	if (DefaultLockLocationInfo.ID == -1) return;
	if (DefaultLockLocationInfo.Location.IsValid() == false) return;
	TSharedPtr<FAccessoryShape> OperationShape;
	for (const auto Ref : Metals)
	{
		if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_LOCK)
		{
			OperationShape = Ref;
			break;
		}
	}
	if (OperationShape.IsValid() == false) return;
	switch (StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultLockLocationInfo.Location)->Hpos)
	{
		case 1:
		{
			OperationShape->SetShapePosX(StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultLockLocationInfo.Location)->HposVal);
		}break;
		case 2:
		{
			OperationShape->SetShapePosX(GetShapeWidth() - StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultLockLocationInfo.Location)->HposVal);
		}break;
		case 3:
		{
			OperationShape->SetShapePosX(GetShapeWidth() / 2.0f);		
		}break;
		default:
		check(false);
		break;
	}	
	switch (StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultLockLocationInfo.Location)->Vpos)
	{
		case 1:
		{
			OperationShape->SetShapePosZ(GetShapeHeight() - StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultLockLocationInfo.Location)->VposVAl);
		}break;
		case 2:
		{
			OperationShape->SetShapePosZ(StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultLockLocationInfo.Location)->VposVAl);
		}break;
		case 3:
		{
			OperationShape->SetShapePosZ(GetShapeHeight() / 2.0f);		
		}break;
		default:
		check(false);
		break;
	}		
	switch (StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DefaultLockLocationInfo.Location)->RotateDirection)
	{
	case 0:
		OperationShape->SetShapeRotY(0.0f);		
		break;
	case 1:		
		OperationShape->SetShapeRotY(90.0f);
		break;
	case 2:
		OperationShape->SetShapeRotY(-90.0f);
		break;
	default:
		check(false);
		break;
	}
}

void FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::ProcessOptionsMetalType()
{
	for (const auto & Ref : OpetionsMetalsShapesList)
	{
		if (Ref.Value->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SHAKE_HANDS) ||
			Ref.Value->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SEALING_SIDE_HANDLE))
		{
			FAuxiliaryMetalInfo TempMetalInfo;
			TempMetalInfo.ID = Ref.Key;
			TempMetalInfo.Name = Ref.Value->GetShapeName();
			HandleList.Add(TempMetalInfo);
		}
		else if (Ref.Value->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_LOCK))
		{
			FAuxiliaryMetalInfo TempMetalInfo;
			TempMetalInfo.ID = Ref.Key;
			TempMetalInfo.Name = Ref.Value->GetShapeName();
			LockList.Add(TempMetalInfo);
		}
	}

}

FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::FVariableAreaDoorSheetForDrawer()
{
	SetShapeType(ST_VariableAreaDoorSheet_ForDrawerDoor);

	FilterParseMetalType.Add(EMetalsType::MT_SHAKE_HANDS);
	FilterParseMetalType.Add(EMetalsType::MT_SEALING_SIDE_HANDLE);
	FilterParseMetalType.Add(EMetalsType::MT_LOCK);
	FilterSpawnMetalType.Add(EMetalsType::MT_SHAKE_HANDS);
	FilterSpawnMetalType.Add(EMetalsType::MT_SEALING_SIDE_HANDLE);
	FilterSpawnMetalType.Add(EMetalsType::MT_LOCK);
	HandleAttriX = MakeShareable(new FNumberRangeAttri(this));
	HandleAttriZ = MakeShareable(new FNumberRangeAttri(this));
	LockAttriX = MakeShareable(new FNumberRangeAttri(this));
	LockAttriZ = MakeShareable(new FNumberRangeAttri(this));

	HandlePosHSelectXAtt = MakeShareable(new FTextSelectAttri(this));
	HandlePosHSelectXAtt->AddSelectValue(TEXT("1"), TEXT("距左边"));
	HandlePosHSelectXAtt->AddSelectValue(TEXT("2"), TEXT("距右边"));
	HandlePosHSelectXAtt->AddSelectValue(TEXT("3"), TEXT("水平居中"));	
	HandlePosHSelectXAtt->SetAttributeValue(TEXT("3"));	
	
	HanleRotateDirection = MakeShareable(new FTextSelectAttri(this));
	HanleRotateDirection->AddSelectValue(TEXT("0"),TEXT("保持不变"));
	HanleRotateDirection->AddSelectValue(TEXT("1"), TEXT("左旋90度"));
	HanleRotateDirection->AddSelectValue(TEXT("2"), TEXT("右旋90度"));
	HanleRotateDirection->SetAttributeValue(TEXT("0"));
	
	HandlePosVSelectXAtt = MakeShareable(new FTextSelectAttri(this));
	HandlePosVSelectXAtt->AddSelectValue(TEXT("1"), TEXT("距上边"));
	HandlePosVSelectXAtt->AddSelectValue(TEXT("2"), TEXT("距下边"));
	HandlePosVSelectXAtt->AddSelectValue(TEXT("3"), TEXT("垂直居中"));	
	HandlePosVSelectXAtt->SetAttributeValue(TEXT("3"));
		
	
	LockRotateDirection = MakeShareable(new FTextSelectAttri(this));
	LockRotateDirection->AddSelectValue(TEXT("0"), TEXT("保持不变"));
	LockRotateDirection->AddSelectValue(TEXT("1"), TEXT("左旋90度"));
	LockRotateDirection->AddSelectValue(TEXT("2"), TEXT("右旋90度"));
	LockRotateDirection->SetAttributeValue(TEXT("0"));
	
	LockPosHSelectXAtt = MakeShareable(new FTextSelectAttri(this));
	LockPosHSelectXAtt->AddSelectValue(TEXT("1"), TEXT("距左边"));
	LockPosHSelectXAtt->AddSelectValue(TEXT("2"), TEXT("距右边"));
	LockPosHSelectXAtt->AddSelectValue(TEXT("3"), TEXT("水平居中"));	
	LockPosHSelectXAtt->SetAttributeValue(TEXT("3"));
		
	
	LockPosVSelectXAtt = MakeShareable(new FTextSelectAttri(this));
	LockPosVSelectXAtt->AddSelectValue(TEXT("1"), TEXT("距上边"));
	LockPosVSelectXAtt->AddSelectValue(TEXT("2"), TEXT("距下边"));
	LockPosVSelectXAtt->AddSelectValue(TEXT("3"), TEXT("垂直居中"));	
	LockPosVSelectXAtt->SetAttributeValue(TEXT("3"));
	
	HandleListSelectAtt = MakeShareable(new FTextSelectAttri(this));
	LockListSelectAtt = MakeShareable(new FTextSelectAttri(this));

	OpenDoorDirectionIndex = 3;
}

void FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FVariableAreaDoorSheet::ParseFromJson(InJsonObject);
	SetShapeType(ST_VariableAreaDoorSheet_ForDrawerDoor);
	OpenDoorDirectionIndex = 3;
}

void FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FVariableAreaDoorSheet::ParseAttributesFromJson(InJsonObject);
	// 坐标修复
#if 0
	{
		SetShapePosX(0.0f);
		SetShapePosY(0.0f);
		SetShapePosZ(0.0f);
		SetShapeRotX(0.0f);
		SetShapeRotY(0.0f);		
		SetShapeRotZ(0.0f);
	}
#endif
	auto SetDefaultAuxiliaryMetalInfo = [InJsonObject](FAuxiliaryMetalInfo & OutInfo, const FString & InJsonObjName)
	{
		if (!InJsonObject->HasField(InJsonObjName)) return;
		{
			const TSharedPtr<FJsonObject> & PosJsonObj = InJsonObject->GetObjectField(InJsonObjName);
			OutInfo.ID = PosJsonObj->GetIntegerField(TEXT("id"));
			OutInfo.Name = PosJsonObj->GetStringField(TEXT("name"));
			if (PosJsonObj->HasField(TEXT("hpos")))
			{
				OutInfo.Location = MakeShareable(new FDoorHanlerDefaultLocation);
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->Hpos = PosJsonObj->GetIntegerField(TEXT("hpos"));
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->Vpos = PosJsonObj->GetIntegerField(TEXT("vpos"));
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->RotateDirection = PosJsonObj->GetIntegerField(TEXT("rotationalDirection"));
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->HposVal = PosJsonObj->GetNumberField(TEXT("hposVal"));
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->VposVAl = PosJsonObj->GetNumberField(TEXT("vposVal"));
			}
		}
	};
	SetDefaultAuxiliaryMetalInfo(DefaultHandleLocationInfo, TEXT("handleInfo"));
	SetDefaultAuxiliaryMetalInfo(DefaultLockLocationInfo, TEXT("lockInfo"));
	OpenDoorDirectionIndex = 3;
	//auto ParseHandleOrLockList = [InJsonObject](const FString & InName, TArray<FAuxiliaryMetalInfo> & InList)
	//{
	//	if (!InJsonObject->HasField(InName)) return;
	//	const TArray<TSharedPtr<FJsonValue>> & Arrays = InJsonObject->GetArrayField(InName);
	//	for (const auto & Ref : Arrays)
	//	{
	//		FAuxiliaryMetalInfo Temp;
	//		Temp.ID = Ref->AsObject()->GetIntegerField(TEXT("id"));
	//		Temp.Name = Ref->AsObject()->GetStringField(TEXT("name"));
	//		InList.Emplace(MoveTemp(Temp));
	//	}
	//};
	//ParseHandleOrLockList(TEXT("hanleList"), HandleList);
	//ParseHandleOrLockList(TEXT("lockList"), LockList);
	
}

void FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FVariableAreaDoorSheet::ParseShapeFromJson(InJsonObject);
	SetShapeType(ST_VariableAreaDoorSheet_ForDrawerDoor);
}

void FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FVariableAreaDoorSheet::ParseContentFromJson(InJsonObject);
}

void FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FVariableAreaDoorSheet::SaveToJson(JsonWriter);
}

void FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FVariableAreaDoorSheet::SaveAttriToJson(JsonWriter);
	auto SetDefaultAuxiliaryMetalInfo = [JsonWriter](const FAuxiliaryMetalInfo & InInfo, const FString & InJsonObjName, const int32 InType)
	{
		if (InInfo.ID != -1)
		{
			JsonWriter->WriteObjectStart(InJsonObjName);
			JsonWriter->WriteValue(TEXT("id"), InInfo.ID);
			JsonWriter->WriteValue(TEXT("name"), InInfo.Name);
			JsonWriter->WriteValue(TEXT("metalsType"), InType);
			if (InInfo.Location.IsValid())
			{
				JsonWriter->WriteValue(TEXT("hpos"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->Hpos);
				JsonWriter->WriteValue(TEXT("rotationalDirection"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->RotateDirection);
				JsonWriter->WriteValue(TEXT("hposVal"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->HposVal);
				JsonWriter->WriteValue(TEXT("vpos"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->Vpos);
				JsonWriter->WriteValue(TEXT("vposVal"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->VposVAl);
			}
			JsonWriter->WriteObjectEnd();
		}
	};
	FAuxiliaryMetalInfo HandleInfo = GetDefaultHandleInfo();
	FAuxiliaryMetalInfo LockInfo = GetDefaultLockInfo();
	SetDefaultAuxiliaryMetalInfo(HandleInfo, TEXT("handleInfo"),StaticCast<int32>(EMetalsType::MT_SHAKE_HANDS));
	SetDefaultAuxiliaryMetalInfo(LockInfo, TEXT("lockInfo"), StaticCast<int32>(EMetalsType::MT_LOCK));

	/*auto WriteHandleOrLockList = [&JsonWriter](const FString & InName, const TArray<FAuxiliaryMetalInfo> & InList)
	{
		JsonWriter->WriteArrayStart(InName);
		for (const auto & Ref : InList)
		{
			JsonWriter->WriteObjectStart();
			JsonWriter->WriteValue(TEXT("name"), Ref.Name);
			JsonWriter->WriteValue(TEXT("id"), Ref.ID);
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();
	};
	WriteHandleOrLockList(TEXT("hanleList"), HandleList);
	WriteHandleOrLockList(TEXT("lockList"), LockList);*/
}

void FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FVariableAreaDoorSheet::SaveShapeToJson(JsonWriter);
}

void FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FVariableAreaDoorSheet::SaveContentToJson(JsonWriter);
}

ASCTShapeActor * FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::SpawnShapeActor()
{
	ASCTShapeActor * DoorSheetActor = FVariableAreaDoorSheet::SpawnShapeActor();	
	return 	DoorSheetActor;
}

void FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::GetResourceUrls(TArray<FString>& OutResourceUrls)
{
	FVariableAreaDoorSheet::GetResourceUrls(OutResourceUrls);
}

void FDrawerDoorShape::FVariableAreaDoorSheetForDrawer::GetFileCachePaths(TArray<FString>& OutFileCachePaths)
{
	FVariableAreaDoorSheet::GetFileCachePaths(OutFileCachePaths);
}

FDrawerDoorShape::FDrawerDoorShape()
{
	SetShapeType(ST_DrawerDoor);
	SetShapeName(TEXT("抽面"));
	// 明装拉手
	FilterSpawnMetalType.Add(EMetalsType::MT_LATERAL_BOX_SHAKE_HAND);
	FilterParseMetalType.Add(EMetalsType::MT_LATERAL_BOX_SHAKE_HAND);
	
}

void FDrawerDoorShape::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//基类信息
	FDoorGroup::ParseFromJson(InJsonObject);

	//门缝值
	LeftRightGap = 2.0;
	UpDownGap = 2.0;
	BackGap = 1.0;

	//箱体拉手

	//抽面面板
	Drawface = MakeShareable(new FVariableAreaDoorSheetForDrawer);
	AddChildShape(Drawface);
	Drawface->SetParentShape(this);
	Drawface->ParseFromJson(InJsonObject);
	//
	UpdateDoorGroup();
}

void FDrawerDoorShape::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//型录类型和Id
	int32 CurrentType = InJsonObject->GetIntegerField(TEXT("type"));
	int32 CurrentId = InJsonObject->GetIntegerField(TEXT("id"));
	check(CurrentType == ShapeType && CurrentId == ShapeId);

	//更新型录基本信息
	FDoorGroup::ParseAttributesFromJson(InJsonObject);

	//门缝值
	LeftRightGap = InJsonObject->GetNumberField(TEXT("leftRightGap"));
	UpDownGap = InJsonObject->GetNumberField(TEXT("upDownGap"));

	//更新子级型录信息
	const TArray<TSharedPtr<FJsonValue>>* ChildList = nullptr;
	InJsonObject->TryGetArrayField(TEXT("children"), ChildList);
	if (ChildList != nullptr && (*ChildList).Num() > 0)
	{
		for (auto& It : *ChildList)
		{
			TSharedPtr<FJsonObject> ChildObject = It->AsObject();
			int32 ShapeType = ChildObject->GetNumberField(TEXT("type"));
			int64 ShapeId = ChildObject->GetNumberField(TEXT("id"));
			if (ShapeType != ST_VariableAreaDoorSheet_ForDrawerDoor)
			{
				continue;
			}
			TSharedPtr<FSCTShape> NewShape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ShapeType, ShapeId);
			TSharedPtr<FSCTShape> CopyShape = FSCTShapeManager::Get()->CopyShapeToNew(NewShape);
			switch (CopyShape->GetShapeType())
			{
				case ST_VariableAreaDoorSheet_ForDrawerDoor:
				{
					//添加子级对象
					TSharedPtr<FVariableAreaDoorSheetForDrawer> CurDoorSheet = StaticCastSharedPtr<FVariableAreaDoorSheetForDrawer>(CopyShape);
					SetDrawerDoorSheet(CurDoorSheet);
					//解析子级对象
					CopyShape->ParseAttributesFromJson(ChildObject);
					break;
				}			
				default:
					check(false);
					break;
			}
		}
	}


	// 拉手类型
	auto SetDefaultAuxiliaryMetalInfo = [InJsonObject](FDoorSheetShapeBase::FAuxiliaryMetalInfo & OutInfo, const FString & InJsonObjName)
	{
		if (!InJsonObject->HasField(InJsonObjName)) return;
		{
			const TSharedPtr<FJsonObject> & PosJsonObj = InJsonObject->GetObjectField(InJsonObjName);

			OutInfo.ID = PosJsonObj->GetIntegerField(TEXT("id"));
			OutInfo.Name = PosJsonObj->GetStringField(TEXT("name"));
			if (PosJsonObj->HasField(TEXT("hpos")))
			{
				OutInfo.Location = MakeShareable(new FDoorHanlerDefaultLocation);
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->MetalType = StaticCast<EMetalsType>(PosJsonObj->GetIntegerField(TEXT("metalsType")));
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->Hpos = PosJsonObj->GetIntegerField(TEXT("hpos"));
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->Vpos = PosJsonObj->GetIntegerField(TEXT("vpos"));
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->RotateDirection = PosJsonObj->GetIntegerField(TEXT("rotationalDirection"));
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->HposVal = PosJsonObj->GetNumberField(TEXT("hposVal"));
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->VposVAl = PosJsonObj->GetNumberField(TEXT("vposVal"));
			}
		}
	};
	SetDefaultAuxiliaryMetalInfo(CurrentHandleLocationInfo, TEXT("handleInfo"));
	if (InJsonObject->HasField(TEXT("hanleType")))
	{
		HanleType = StaticCast<EHandleMetalType>(InJsonObject->GetIntegerField(TEXT("hanleType")));
	}
}

void FDrawerDoorShape::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FDoorGroup::ParseShapeFromJson(InJsonObject);
}

void FDrawerDoorShape::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FDoorGroup::ParseContentFromJson(InJsonObject);
}

void FDrawerDoorShape::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//此接口暂时为了让BIM前段使用

	JsonWriter->WriteValue(TEXT("upExtension"), UpExtensionValue);
	JsonWriter->WriteValue(TEXT("downExtension"), DownExtensionValue);
	JsonWriter->WriteValue(TEXT("leftExtension"), LeftExtensionValue);
	JsonWriter->WriteValue(TEXT("rightExtension"), RightExtensionValue);

	// 获取第一个门板
	TSharedPtr<FDoorSheetShapeBase> DoorSheet = GetDrawDoorSheet();
	{
		//1、型录类型和ID
		JsonWriter->WriteValue(TEXT("type"), (int32)GetShapeType());
		JsonWriter->WriteValue(TEXT("id"), GetShapeId());
		JsonWriter->WriteValue(TEXT("category"), GetShapeCategory());

		//2、型录基本信息
		JsonWriter->WriteValue(TEXT("name"), GetShapeName());
		JsonWriter->WriteValue(TEXT("thumbnailUrl"), GetThumbnailUrl());
		JsonWriter->WriteValue(TEXT("code"), GetShapeCode());
		JsonWriter->WriteValue(TEXT("systemName"), GetSystemName());

		//3、产品状态
		JsonWriter->WriteValue(TEXT("status"), (int32)GetProductStatus());

		//4、型录显示条件
		// 	JsonWriter->WriteObjectStart(TEXT("showCondition"));
		// 	ShowCondition->SaveToJson(JsonWriter);
		// 	JsonWriter->WriteObjectEnd();

		//5、型录尺寸信息
		JsonWriter->WriteObjectStart(TEXT("width"));
		DoorSheet->GetShapeWidthAttri()->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteObjectStart(TEXT("depth"));
		DoorSheet->GetShapeDepthAttri()->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteObjectStart(TEXT("height"));
		DoorSheet->GetShapeHeightAttri()->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		//6、型录位置信息
		JsonWriter->WriteObjectStart(TEXT("posx"));
		DoorSheet->GetShapePosXAttri()->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteObjectStart(TEXT("posy"));
		DoorSheet->GetShapePosYAttri()->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteObjectStart(TEXT("posz"));
		DoorSheet->GetShapePosZAttri()->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		//7、型录位置信息
		JsonWriter->WriteObjectStart(TEXT("rotx"));
		DoorSheet->GetShapeRotXAttri()->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteObjectStart(TEXT("roty"));
		DoorSheet->GetShapeRotYAttri()->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteObjectStart(TEXT("rotz"));
		DoorSheet->GetShapeRotZAttri()->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}

	//门缝值
	JsonWriter->WriteArrayStart(TEXT("doorSlotValue"));
	JsonWriter->WriteValue(LeftRightGap);
	JsonWriter->WriteValue(UpDownGap);
	JsonWriter->WriteArrayEnd();

	// 封边
	{
		JsonWriter->WriteObjectStart(TEXT("edgeBanding"));
		const FDoorSheetShapeBase::FDoorPakMetaData & PakRef = DoorSheet->GetEdgeBanding();
		JsonWriter->WriteValue(TEXT("id"), PakRef.ID);
		JsonWriter->WriteValue(TEXT("name"), PakRef.Name);
		JsonWriter->WriteValue(TEXT("pakUrl"), PakRef.Url);
		JsonWriter->WriteValue(TEXT("pakMd5"), PakRef.MD5);
		JsonWriter->WriteValue(TEXT("thumbnailUrl"), PakRef.ThumbnailUrl);
		JsonWriter->WriteObjectEnd();
	}
	// 基材
	{
		JsonWriter->WriteObjectStart(TEXT("substrate"));
		JsonWriter->WriteValue(TEXT("id"), DoorSheet->GetSubstrateID());
		JsonWriter->WriteValue(TEXT("name"), DoorSheet->GetSubstrateName());
		JsonWriter->WriteObjectEnd();
	}
	// 材质
	{
		JsonWriter->WriteObjectStart(TEXT("material"));
		const FDoorSheetShapeBase::FDoorPakMetaData & PakRef = DoorSheet->GetMaterial();
		JsonWriter->WriteValue(TEXT("id"), PakRef.ID);
		JsonWriter->WriteValue(TEXT("name"), PakRef.Name);
		JsonWriter->WriteValue(TEXT("pakUrl"), PakRef.Url);
		JsonWriter->WriteValue(TEXT("pakMd5"), PakRef.MD5);
		JsonWriter->WriteValue(TEXT("thumbnailUrl"), PakRef.ThumbnailUrl);
		JsonWriter->WriteValue(TEXT("optimizeParam"), PakRef.OptimizeParam);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteValue(TEXT("hasMaterialReplace"), DoorSheet->HasReplaceMaterial());

	// 存储门所用到的五金
	{
		JsonWriter->WriteArrayStart(TEXT("children"));
		// 门组五金
		for (const auto & Ref : Metals)
		{
			JsonWriter->WriteObjectStart();
			Ref->SaveAttriToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
		// 门板五金 
		for (const auto & Ref : DoorSheet->GetMetals())
		{
			JsonWriter->WriteObjectStart();
			Ref->SaveAttriToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}

		JsonWriter->WriteArrayEnd();
	}

	// 存储Shapes
	{
		JsonWriter->WriteArrayStart(TEXT("shapes"));
		// 门组可替换五金
		for (const auto & Ref : OpetionsMetalsShapesList)
		{
			JsonWriter->WriteObjectStart();
			Ref.Value->SaveShapeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
		// 门板可替换五金 
		for (const auto & Ref : DoorSheet->GetOptionShapeMap())
		{
			JsonWriter->WriteObjectStart();
			Ref.Value->SaveShapeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}

		JsonWriter->WriteArrayEnd();
	}

	// 存储切块
	{

		// 存储门切块
		JsonWriter->WriteArrayStart(TEXT("divideBlocks"));

		auto WriteAttribute = [](const FString & InName, const FString & InRefName, const TSharedPtr<FShapeAttribute> & InAttri, TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
		{
			TSharedPtr<FNumberRangeAttri> RangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(InAttri);
			JsonWriter->WriteObjectStart(InName);
			JsonWriter->WriteValue(TEXT("id"), RangeAttri->GetAttributeId());
			JsonWriter->WriteValue(TEXT("type"), (int32)RangeAttri->GetAttributeType());
			JsonWriter->WriteValue(TEXT("refName"), InRefName);
			JsonWriter->WriteObjectStart(TEXT("value"));
			JsonWriter->WriteValue(TEXT("min"), RangeAttri->GetMinValue());
			JsonWriter->WriteValue(TEXT("max"), RangeAttri->GetMaxValue());
			JsonWriter->WriteObjectEnd();
			JsonWriter->WriteValue(TEXT("current"), RangeAttri->GetAttributeStr());
			JsonWriter->WriteObjectEnd();
		};

		TSharedPtr<FVariableAreaDoorSheet> VariableAreaDoorSheet = StaticCastSharedPtr<FVariableAreaDoorSheet>(DoorSheet);
		for (const auto & Ref : VariableAreaDoorSheet->GetDividBlocks())
		{
			JsonWriter->WriteObjectStart();
			WriteAttribute(TEXT("width"), TEXT("WIDTH"), Ref->GetShapeWidthAttri(), JsonWriter);
			WriteAttribute(TEXT("depth"), TEXT("DEPTH"), Ref->GetShapeDepthAttri(), JsonWriter);
			WriteAttribute(TEXT("height"), TEXT("HEIGHT"), Ref->GetShapeHeightAttri(), JsonWriter);
			WriteAttribute(TEXT("posx"), TEXT("POSX"), Ref->GetShapePosXAttri(), JsonWriter);
			WriteAttribute(TEXT("posy"), TEXT("POSY"), Ref->GetShapePosYAttri(), JsonWriter);
			WriteAttribute(TEXT("posz"), TEXT("POSZ"), Ref->GetShapePosZAttri(), JsonWriter);
			WriteAttribute(TEXT("rotx"), TEXT("ROTX"), Ref->GetShapeRotXAttri(), JsonWriter);
			WriteAttribute(TEXT("roty"), TEXT("ROTY"), Ref->GetShapeRotYAttri(), JsonWriter);
			WriteAttribute(TEXT("rotz"), TEXT("ROTZ"), Ref->GetShapeRotZAttri(), JsonWriter);
			JsonWriter->WriteValue(TEXT("pakUrl"), Ref->GetFileUrl());
			JsonWriter->WriteValue(TEXT("pakMd5"), Ref->GetFileMd5());
			JsonWriter->WriteValue(TEXT("fileName"), Ref->GetFileName());
			JsonWriter->WriteValue(TEXT("mwassetUrl"), TEXT(""));
			JsonWriter->WriteValue(TEXT("optimizeParam"), Ref->GetoptimizeParam());
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();
	}


	{
		TSharedPtr<FVariableAreaDoorSheet> VarSheet = StaticCastSharedPtr<FVariableAreaDoorSheet>(DoorSheet);

		JsonWriter->WriteArrayStart(TEXT("openDoorDirections"));
		for (const auto & Ref : VarSheet->GetAvailableOpenDirections())
		{
			JsonWriter->WriteValue(StaticCast<int32>(Ref));
		}
		JsonWriter->WriteArrayEnd();
		JsonWriter->WriteValue(TEXT("defaultOpenDoorIndex"), StaticCast<int32>(VarSheet->GetOpenDoorDirectionIndex()));
	}
}

void FDrawerDoorShape::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//保存型录基本信息
	FDoorGroup::SaveAttriToJson(JsonWriter);

	//门缝值
	JsonWriter->WriteValue(TEXT("leftRightGap"), LeftRightGap);
	JsonWriter->WriteValue(TEXT("upDownGap"), UpDownGap);

	//子型录信息
	JsonWriter->WriteArrayStart(TEXT("children"));
	//抽面门板
	if (Drawface.IsValid())
	{
		JsonWriter->WriteObjectStart();
		Drawface->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//箱体拉手五金
	for (auto & Ref : Metals)
	{
		JsonWriter->WriteObjectStart();
		Ref->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();


	const FDoorSheetShapeBase::FAuxiliaryMetalInfo & HandleInfo = GetDoorGroupCurrentHandleInfo();
	if (HandleInfo.ID != -1)
	{
		auto SetDefaultAuxiliaryMetalInfo = [JsonWriter](const FDoorSheetShapeBase::FAuxiliaryMetalInfo & InInfo, const FString & InJsonObjName, const int32 InType)
		{
			if (InInfo.ID != -1)
			{
				JsonWriter->WriteObjectStart(InJsonObjName);
				JsonWriter->WriteValue(TEXT("id"), InInfo.ID);
				JsonWriter->WriteValue(TEXT("name"), InInfo.Name);
				JsonWriter->WriteValue(TEXT("metalsType"), InType);
				if (InInfo.Location.IsValid())
				{
					JsonWriter->WriteValue(TEXT("hpos"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->Hpos);
					JsonWriter->WriteValue(TEXT("rotationalDirection"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->RotateDirection);
					JsonWriter->WriteValue(TEXT("hposVal"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->HposVal);
					JsonWriter->WriteValue(TEXT("vpos"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->Vpos);
					JsonWriter->WriteValue(TEXT("vposVal"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->VposVAl);
				}
				JsonWriter->WriteObjectEnd();
			}
		};

		SetDefaultAuxiliaryMetalInfo(HandleInfo, TEXT("handleInfo"), StaticCast<int32>(EMetalsType::MT_LATERAL_BOX_SHAKE_HAND));
	}
	JsonWriter->WriteValue(TEXT("hanleType"), StaticCast<int32>(GetDoorGroupHanleType()));

}

void FDrawerDoorShape::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FDoorGroup::SaveShapeToJson(JsonWriter);
	JsonWriter->WriteArrayStart(TEXT("shapes"));
	for (const auto & Ref : OpetionsMetalsShapesList)
	{
		JsonWriter->WriteObjectStart();
		Ref.Value->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
}

void FDrawerDoorShape::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FDoorGroup::SaveContentToJson(JsonWriter);
}

void FDrawerDoorShape::CopyTo(FSCTShape * OutShape)
{
	check(OutShape && OutShape->GetShapeType() == this->GetShapeType());
	//基类信息
	FDoorGroup::CopyTo(OutShape);
	//抽面信息
	FDrawerDoorShape* OutDrawerDoor = StaticCast<FDrawerDoorShape*>(OutShape);
	
	//门缝值
	OutDrawerDoor->LeftRightGap = LeftRightGap;
	OutDrawerDoor->UpDownGap = UpDownGap;
	//抽面
	if (Drawface.IsValid())
	{
		FVariableAreaDoorSheetForDrawer* NewDrawerFace = new FVariableAreaDoorSheetForDrawer;
		Drawface->CopyTo(NewDrawerFace);
		OutDrawerDoor->SetDrawerDoorSheet(MakeShareable(NewDrawerFace));
	}
	if (CurrentHandleLocationInfo.ID != -1)
	{
		OutDrawerDoor->CurrentHandleLocationInfo.ID = CurrentHandleLocationInfo.ID;
		OutDrawerDoor->CurrentHandleLocationInfo.Name = CurrentHandleLocationInfo.Name;
		OutDrawerDoor->CurrentHandleLocationInfo.Location = MakeShareable(new FDoorHanlerDefaultLocation);
		OutDrawerDoor->CurrentHandleLocationInfo.Location->MetalType = CurrentHandleLocationInfo.Location->MetalType;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutDrawerDoor->CurrentHandleLocationInfo.Location)->Hpos = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->Hpos;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutDrawerDoor->CurrentHandleLocationInfo.Location)->HposVal = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->HposVal;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutDrawerDoor->CurrentHandleLocationInfo.Location)->Vpos = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->Vpos;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutDrawerDoor->CurrentHandleLocationInfo.Location)->VposVAl = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->VposVAl;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutDrawerDoor->CurrentHandleLocationInfo.Location)->RotateDirection = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->RotateDirection;
	}
	OutDrawerDoor->HandleList = HandleList;
	OutDrawerDoor->BoxHanleSubType = BoxHanleSubType;
	OutDrawerDoor->HanleType = HanleType;
}

ASCTShapeActor * FDrawerDoorShape::SpawnShapeActor()
{
	//创建抽面型录
	ASCTShapeActor* NewDoorGroupActor = FDoorGroup::SpawnShapeActor();	
	//面板
	if (Drawface.IsValid())
	{
		Drawface->FilterSpawnMetalType.Empty();
		if (GetDoorGroupHanleType() == EHandleMetalType::E_MT_SHAKE_HANDS)
		{
			Drawface->FilterSpawnMetalType.Add(EMetalsType::MT_SHAKE_HANDS);
			Drawface->FilterSpawnMetalType.Add(EMetalsType::MT_LOCK);
		}
		else if (GetDoorGroupHanleType() == EHandleMetalType::E_MT_SEALING_SIDE_HANDLE)
		{
			Drawface->FilterSpawnMetalType.Add(EMetalsType::MT_SEALING_SIDE_HANDLE);
			Drawface->FilterSpawnMetalType.Add(EMetalsType::MT_LOCK);
		}
		ASCTShapeActor* NewDoorSheetActor = Drawface->SpawnShapeActor();
		NewDoorSheetActor->AttachToActorOverride(NewDoorGroupActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	if (GetDoorGroupHanleType() != EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND)
	{
		for (auto & Ref : Metals)
		{
			if (Ref->GetShapeActor())
			{
				Ref->DestroyShapeActor();
			}
		}
	}
	return NewDoorGroupActor;
}

void FDrawerDoorShape::SetCollisionProfileName(FName InProfileName)
{
	if (Drawface.IsValid())
	{
		Drawface->SetCollisionProfileName(InProfileName);
	}
}

void FDrawerDoorShape::GetResourceUrls(TArray<FString>& OutResourceUrls)
{
	FDoorGroup::GetResourceUrls(OutResourceUrls);
	if (Drawface.IsValid())
	{
		Drawface->GetResourceUrls(OutResourceUrls);
	}
}

void FDrawerDoorShape::GetFileCachePaths(TArray<FString>& OutFileCachePaths)
{
	FDoorGroup::GetFileCachePaths(OutFileCachePaths);
	if (Drawface.IsValid())
	{
		Drawface->GetFileCachePaths(OutFileCachePaths);
	}
}

void FDrawerDoorShape::HiddenDoorGroupActors(bool bHidden)
{
	if (Drawface.IsValid())
	{
		ASCTShapeActor* CurDrawerActor = Drawface->GetShapeActor();
		if (CurDrawerActor)
		{
			CurDrawerActor->SetShapeActorHiddenInGameRecursively(bHidden);
		}
	}
}

bool FDrawerDoorShape::GetDoorGoupIsHIdden() const {
	check(const_cast<FDrawerDoorShape*>(this)->GetDrawDoorSheet().IsValid());
	if (const_cast<FDrawerDoorShape*>(this)->GetDrawDoorSheet()->GetShapeActor())
	{
		return const_cast<FDrawerDoorShape*>(this)->GetDrawDoorSheet()->GetShapeActor()->bHidden;
	}
	else
	{
		return FDoorGroup::GetDoorGoupIsHIdden();
	}
}

float FDrawerDoorShape::GetDoorTotalDepth()
{
	float DoorDepth = 0.0;
	if (Drawface.IsValid())
	{
		DoorDepth = Drawface->GetShapeDepth();
	}
	return DoorDepth + BackGap;
}

bool FDrawerDoorShape::SetShapeWidth(float InIntValue)
{
	return SetShapeWidth(FString::Printf(TEXT("%f"), InIntValue));
}

bool FDrawerDoorShape::SetShapeWidth(const FString &InStrValue)
{
	bool bResult = true;
	if (Width->GetAttributeStr() != InStrValue)
	{
		float OldValue = GetShapeWidth();
		bResult = Width->SetAttributeValue(InStrValue);
		if (bResult)
		{
			bResult = UpdateDoorGroup();
			if (bResult)
			{
				if (BoundingBoxActor)
				{
					BoundingBoxActor->UpdateActorDimension();
				}
				if (WireFrameActor)
				{
					WireFrameActor->UpdateActorDimension();
				}
			}
			else
			{
				SetShapeWidth(OldValue);
			}
		}
	}
	return bResult;
}

bool FDrawerDoorShape::SetShapeDepth(float InIntValue)
{
	return SetShapeDepth(FString::Printf(TEXT("%f"), InIntValue));
}

bool FDrawerDoorShape::SetShapeDepth(const FString &InStrValue)
{
	bool bResult = true;
	if (Depth->GetAttributeStr() != InStrValue)
	{
		float OldValue = GetShapeDepth();
		bResult = Depth->SetAttributeValue(InStrValue);
		if (bResult)
		{
			bResult = UpdateDoorGroup();
			if (bResult)
			{
				if (BoundingBoxActor)
				{
					BoundingBoxActor->UpdateActorDimension();
				}
				if (WireFrameActor)
				{
					WireFrameActor->UpdateActorDimension();
				}
			} 
			else
			{
				SetShapeDepth(OldValue);
			}
		}
	}
	return bResult;
}

bool FDrawerDoorShape::SetShapeHeight(float InIntValue)
{
	return SetShapeHeight(FString::Printf(TEXT("%f"), InIntValue));
}

bool FDrawerDoorShape::SetShapeHeight(const FString &InStrValue)
{
	bool bResult = true;
	if (Height->GetAttributeStr() != InStrValue)
	{
		float OldValue = GetShapeHeight();
		bResult = Height->SetAttributeValue(InStrValue);
		if (bResult)
		{
			bResult = UpdateDoorGroup();
			if (bResult)
			{
				if (BoundingBoxActor)
				{
					BoundingBoxActor->UpdateActorDimension();
				}
				if (WireFrameActor)
				{
					WireFrameActor->UpdateActorDimension();
				}
			} 
			else
			{
				SetShapeHeight(OldValue);
			}
		}
	}
	return bResult;
}

void FDrawerDoorShape::SetUpExtensionValue(float InValue)
{
	UpExtensionValue = InValue;
	UpdateDoorGroup();
}

void FDrawerDoorShape::SetDownExtensionValue(float InValue)
{
	DownExtensionValue = InValue;
	UpdateDoorGroup();
}

void FDrawerDoorShape::SetLeftExtensionValue(float InValue)
{
	LeftExtensionValue = InValue;
	UpdateDoorGroup();
}

void FDrawerDoorShape::SetRightExtensionValue(float InValue)
{
	RightExtensionValue = InValue;
	UpdateDoorGroup();
}

void FDrawerDoorShape::SetDrawerDoorSheet(TSharedPtr<FVariableAreaDoorSheetForDrawer> InShape)
{
	Drawface = InShape; 
	//
	Drawface->SetParentShape(this);
	AddChildShape(Drawface);
}

bool FDrawerDoorShape::SetUpDownGapValue(float InValue)
{
	UpDownGap = InValue;
	UpdateDoorGroup();
	return true;
}

bool FDrawerDoorShape::SetLeftRightGapValue(float InValue)
{
	LeftRightGap = InValue;
	UpdateDoorGroup();
	return true;
}

bool FDrawerDoorShape::UpdateDoorGroup()
{
	if (!Drawface.IsValid())
	{
		return true;
	}

	//当前门组尺寸（考虑外延值）
	float DoorGroupWidth = GetShapeWidth() + LeftExtensionValue + RightExtensionValue;
	float DoorGroupDepth = GetShapeDepth();
	float DoorGroupHeight = GetShapeHeight() + UpExtensionValue + DownExtensionValue;
	//当前门组的位置（考虑外延值）
	float DoorGroupPosX = -LeftExtensionValue;
	float DoorGroupPosY = 0.0;
	float DoorGroupPosZ = -DownExtensionValue;

	auto GetCurHanleShape = [this]()->TSharedPtr<FAccessoryShape>
	{
		TSharedPtr<FAccessoryShape> RetShape = nullptr;
		const int32 Id = CurrentHandleLocationInfo.ID;
		for (auto & Ref : Metals)
		{
			if (Ref->GetShapeId() == Id)
			{
				RetShape = Ref;
				break;
			}
		}
		return 	RetShape;
	};

	// 箱体拉手
	if (GetDoorGroupHanleType() == EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND)
	{		
		// 设置箱体拉手的空间位置以及尺寸
		auto SetHanlePositionAndSize = [this](TSharedPtr<FAccessoryShape> & Shape, const float InPosX, const float InPosY, const float InPosZ, const float InW)
		{
			check(Shape.IsValid());
			Shape->SetShapePosX(InPosX);
			Shape->SetShapePosY(InPosY);
			Shape->SetShapePosZ(InPosZ);
			Shape->SetShapeWidth(InW);
		};

		float AccessPosX = 0.0f;
		float AccessPosY = 0.0f;
		float AccessPosZ = 0.0f;
		float AccessW = DoorGroupWidth;		
		// 吊柜下拉手
		if (BoxHanleSubType == ELateralBoxShakeHandSubType::E_LBSH_FLOOR_CUPBOARD_HANDLE)
		{
			TSharedPtr<FAccessoryShape> DesAccessShape = GetCurHanleShape();
			AccessPosX += DoorGroupPosX + AccessW / 2.0f;
			AccessPosZ = DoorGroupPosZ + DoorGroupHeight - DesAccessShape->GetShapeHeight();
			SetHanlePositionAndSize(DesAccessShape, AccessPosX, AccessPosY, AccessPosZ, AccessW);
			DoorGroupHeight = DoorGroupHeight - (DesAccessShape->GetShapeHeight() - StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->VposVAl) - DownExtensionValue ;
		}
		//吊柜上拉手
		else if (BoxHanleSubType == ELateralBoxShakeHandSubType::E_LBSH_WALL_CUPBOARD_HANDLE)
		{
			TSharedPtr<FAccessoryShape> DesAccessShape = GetCurHanleShape();
			AccessPosX += DoorGroupPosX + AccessW / 2.0f;
			AccessPosZ = DoorGroupPosZ + DesAccessShape->GetShapeHeight();
			SetHanlePositionAndSize(DesAccessShape, AccessPosX, AccessPosY, AccessPosZ, AccessW);
			DoorGroupHeight = DoorGroupHeight - (DesAccessShape->GetShapeHeight() - StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->VposVAl) - UpExtensionValue;
			DoorGroupPosZ = DesAccessShape->GetShapeHeight() - StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->VposVAl;
		}
		// 横装C拉手
		else if (BoxHanleSubType == ELateralBoxShakeHandSubType::LBSH_C_HANDL)
		{
			TSharedPtr<FAccessoryShape> DesAccessShape = GetCurHanleShape();
			AccessPosX += DoorGroupPosX + AccessW / 2.0f;
			DoorGroupHeight =  DoorGroupHeight - (DesAccessShape->GetShapeHeight() - 2.0 * StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->VposVAl) - UpExtensionValue;
			AccessPosZ = GetShapeHeight() - (DesAccessShape->GetShapeHeight() - StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->VposVAl);
			SetHanlePositionAndSize(DesAccessShape, AccessPosX, AccessPosY, AccessPosZ, AccessW);
		}
	}
	else if (GetDoorGroupHanleType() == EHandleMetalType::E_MT_SEALING_SIDE_HANDLE)
	{		
		auto GetDoorSheetEdgeHanleShape = [this]()->TSharedPtr<FAccessoryShape>
		{
			TSharedPtr<FAccessoryShape> RetShape = nullptr;
			const int32 Id = GetDrawDoorSheet()->GetDefaultHandleInfo().ID;
			for (auto & Ref : GetDrawDoorSheet()->GetMetals())
			{
				if (Ref->GetShapeId() == Id)
				{
					RetShape = Ref;
					break;
				}
			}
			return 	RetShape;
		};

		TSharedPtr<FAccessoryShape> AccessShape = GetDoorSheetEdgeHanleShape();
		const float HanleW = AccessShape->GetShapeWidth();
		const float HanleH = AccessShape->GetShapeHeight();
		float  InstallHeight = HanleH;
		for (const auto & Ref : AccessShape->GetMetalsPropertyList())
		{
			if (FMath::IsNearlyEqual(FCString::Atof(*Ref.propertyId), 12))
			{
				InstallHeight = FCString::Atof(*Ref.propertyValue);
				break;
			}
		}
		TSharedPtr<FDoorHanlerDefaultLocation> Loacation = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(GetDrawDoorSheet()->GetDefaultHandleInfo().Location);
		{
			AccessShape->SetShapeWidth(DoorGroupWidth - LeftRightGap * 2);
			AccessShape->SetShapePosX((DoorGroupWidth - LeftRightGap * 2) / 2.0f);
			AccessShape->SetShapePosY(AccessShape->GetShapeDepth() / 2.0f);
			// 封边拉手处于门板上面
			if (Loacation->Vpos == 1)
			{
				AccessShape->SetShapePosZ(DoorGroupHeight - InstallHeight - 2.0f * UpDownGap);
				AccessShape->SetShapeRotY(0.0f);
				Drawface->SetShapePosZ(DoorGroupPosZ + UpDownGap);
			}
			// 封边拉手处于门板下面
			else if (Loacation->Vpos == 2)
			{
				AccessShape->SetShapePosZ(UpDownGap);
				DoorGroupPosZ += InstallHeight;
				AccessShape->SetShapeRotY(180.0f);
				Drawface->SetShapePosZ(DoorGroupPosZ);
			}
		}
		DoorGroupHeight = DoorGroupHeight - InstallHeight;
	}

	bool bResult = false;
	//更新门板的位置和尺寸（考虑门缝值）
	bResult = Drawface->SetShapePosX(DoorGroupPosX + LeftRightGap);
	if (!bResult)
	{
		return false;
	}
	bResult = Drawface->SetShapePosY(DoorGroupPosY);
	if (!bResult)
	{
		return false;
	}
	if (GetDoorGroupHanleType() != EHandleMetalType::E_MT_SEALING_SIDE_HANDLE)
	{
		bResult = Drawface->SetShapePosZ(DoorGroupPosZ + UpDownGap);
		if (!bResult)
		{
			return false;
		}
	}		
	bResult = Drawface->SetShapeWidth(DoorGroupWidth - LeftRightGap * 2);
	//if (!bResult)
	//{
	//	return false;
	//}
	//bResult = Drawface->SetShapeDepth(DoorGroupDepth);
	bResult = Drawface->SetShapeHeight(DoorGroupHeight - UpDownGap * 2);
	//if (!bResult)
	//{
	//	return false;
	//}
	return true;
}

bool FDrawerDoorShape::SetDoorGroupHanleZValue(const float InValue)
{
	auto GetCurHanleShape = [this]()->TSharedPtr<FAccessoryShape>
	{
		TSharedPtr<FAccessoryShape> RetShape = nullptr;
		const int32 Id = CurrentHandleLocationInfo.ID;
		for (auto & Ref : Metals)
		{
			if (Ref->GetShapeId() == Id)
			{
				RetShape = Ref;
				break;
			}
		}
		return 	RetShape;
	};
	bool RetValue = false;
	do
	{
		if (GetCurHanleShape().IsValid() == false)
		{
			break;
		}		
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->VposVAl = InValue;
		RetValue = true;
	} while (false);
	return RetValue;
}

void FDrawerDoorShape::ReBuildDoorGroupHandle(const int32 InID)
{
	TSharedPtr<FAccessoryShape> CurMetal;
	const EMetalsType DesType = EMetalsType::MT_LATERAL_BOX_SHAKE_HAND;
	for (const auto & Ref : Metals)
	{
		if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == DesType)
		{
			CurMetal = Ref;
			break;
		}
	}

	check(CurMetal.IsValid());
	TSharedPtr<FAccessoryShape> AccessShape = StaticCastSharedPtr<FAccessoryShape>(*(GetOptionShapeMap().Find(InID)));
	TSharedPtr<FAccessoryShape> NewShape = StaticCastSharedPtr<FAccessoryShape>(FSCTShapeManager::Get()->CopyShapeToNew(AccessShape));
	NewShape->SetShapePosX(CurMetal->GetShapePosX());
	NewShape->SetShapePosY(CurMetal->GetShapePosY());
	NewShape->SetShapePosZ(CurMetal->GetShapePosZ());
	NewShape->LoadModelMesh(true);
	AddChildShape(NewShape);
	Metals.Add(NewShape);
	FDoorSheetShapeBase::FAuxiliaryMetalInfo & CurHandleInfo = GetDoorGroupCurrentHandleInfo();
	CurHandleInfo.ID = InID;
	CurHandleInfo.Name = AccessShape->GetShapeName();
	NewShape->SpawnShapeActor();
	NewShape->GetShapeActor()->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
	Metals.Remove(CurMetal);
	RemoveChildShape(CurMetal);
	CurMetal->DestroyShapeActor();
	if (NewShape->GetMetalsPropertyList().Num() > 0)
	{
		const FString & PropertyValueIdentity = NewShape->GetMetalsPropertyList()[0].PropertyValueIdentity;
		if (PropertyValueIdentity == TEXT("c5164a"))
		{
			BoxHanleSubType = ELateralBoxShakeHandSubType::E_LBSH_WALL_CUPBOARD_HANDLE;
		}
		else if (PropertyValueIdentity == TEXT("MIQHn1"))
		{
			BoxHanleSubType = ELateralBoxShakeHandSubType::E_LBSH_FLOOR_CUPBOARD_HANDLE;
		}
		else if (PropertyValueIdentity == TEXT("0Xkx6p"))
		{
			BoxHanleSubType = ELateralBoxShakeHandSubType::LBSH_C_HANDL;
		}
	}
}

bool FDrawerDoorShape::SetDoorGroupHanleType(const EHandleMetalType InHanleType)
{
	bool RetValue = false;
	do
	{
		if (HanleType == InHanleType) break;
		HanleType = InHanleType;

		for (auto & InnerRef : Drawface->GetMetals())
		{
			if (InnerRef->GetShapeActor())
			{
				InnerRef->DestroyShapeActor();
			}
		}

		for (auto & Ref : Metals)
		{
			if (Ref->GetShapeActor())
			{
				Ref->DestroyShapeActor();
			}
		}

		// 如果是箱体拉手
		if (HanleType == EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND)
		{									
			// 激活当前的箱体拉手
			for (auto & Ref : Metals)
			{
				check(Ref->GetShapeActor() == nullptr);
				Ref->SpawnShapeActor();
				Ref->GetShapeActor()->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
			}

			RetValue = true;
		}
		// 如果是明装拉手
		else if (HanleType == EHandleMetalType::E_MT_SHAKE_HANDS)
		{			
			RetValue = true;
		}
		// 如果是箱体拉手
		else if (HanleType == EHandleMetalType::E_MT_SEALING_SIDE_HANDLE)
		{
			RetValue = true;
		}
		else if (HanleType == EHandleMetalType::E_MT_None)
		{
			RetValue = true;
		}

	} while (false);

	return RetValue;
}

void FDrawerDoorShape::ProcessOptionsMetalType()
{
	for (const auto & Ref : OpetionsMetalsShapesList)
	{
		if (Ref.Value->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_LATERAL_BOX_SHAKE_HAND))
		{
			FDoorSheetShapeBase::FAuxiliaryMetalInfo TempMetalInfo;
			TempMetalInfo.ID = Ref.Key;
			TempMetalInfo.Name = Ref.Value->GetShapeName();
			HandleList.Add(TempMetalInfo);
		}
	}
	HanleType = EHandleMetalType::E_MT_SHAKE_HANDS;
	for (const auto & Ref : Metals)
	{
		if (Ref->GetShapeType() != ST_Accessory) continue;

		if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_LATERAL_BOX_SHAKE_HAND)
		{
			CurrentHandleLocationInfo.ID = Ref->GetShapeId();
			CurrentHandleLocationInfo.Name = Ref->GetShapeName();
			CurrentHandleLocationInfo.Location = MakeShareable(new FDoorHanlerDefaultLocation);
			StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->Vpos = 3;
			StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->VposVAl = 20.0f;
			StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->MetalType = StaticCast<EMetalsType>(Ref->GetShapeCategory());
			if (Ref->GetMetalsPropertyList().Num() > 0)
			{
				const FString & PropertyValueIdentity = Ref->GetMetalsPropertyList()[0].PropertyValueIdentity;
				if (PropertyValueIdentity == TEXT("c5164a"))
				{
					BoxHanleSubType = ELateralBoxShakeHandSubType::E_LBSH_WALL_CUPBOARD_HANDLE;
				}
				else if (PropertyValueIdentity == TEXT("MIQHn1"))
				{
					BoxHanleSubType = ELateralBoxShakeHandSubType::E_LBSH_FLOOR_CUPBOARD_HANDLE;
				}
				else if (PropertyValueIdentity == TEXT("0Xkx6p"))
				{
					BoxHanleSubType = ELateralBoxShakeHandSubType::LBSH_C_HANDL;
				}
			}
			HanleType = EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND;
		}
	}
}

FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor::FVariableAreaDoorSheetForSideHungDoor()
{
	// 铰链
	SetShapeType(ST_VariableAreaDoorSheet_ForSideHungDoor);
	// 明装拉手
	FilterParseMetalType.Add(EMetalsType::MT_SHAKE_HANDS);
	FilterParseMetalType.Add(EMetalsType::MT_HINGE);
	FilterParseMetalType.Add(EMetalsType::MT_SEALING_SIDE_HANDLE);
	FilterSpawnMetalType.Add(EMetalsType::MT_SHAKE_HANDS);
	FilterSpawnMetalType.Add(EMetalsType::MT_SEALING_SIDE_HANDLE);
}

void FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FVariableAreaDoorSheet::ParseAttributesFromJson(InJsonObject);	
	auto SetDefaultAuxiliaryMetalInfo = [InJsonObject](FAuxiliaryMetalInfo & OutInfo, const FString & InJsonObjName)
	{
		if (!InJsonObject->HasField(InJsonObjName)) return;
		{
			const TSharedPtr<FJsonObject> & PosJsonObj = InJsonObject->GetObjectField(InJsonObjName);			
			OutInfo.ID = PosJsonObj->GetIntegerField(TEXT("id"));
			OutInfo.Name = PosJsonObj->GetStringField(TEXT("name"));
			if (PosJsonObj->HasField(TEXT("hpos")))
			{
				OutInfo.Location = MakeShareable(new FDoorHanlerDefaultLocation);
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->Hpos = PosJsonObj->GetIntegerField(TEXT("hpos"));
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->Vpos = PosJsonObj->GetIntegerField(TEXT("vpos"));
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->RotateDirection = PosJsonObj->GetIntegerField(TEXT("rotationalDirection"));
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->HposVal = PosJsonObj->GetNumberField(TEXT("hposVal"));
				StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutInfo.Location)->VposVAl = PosJsonObj->GetNumberField(TEXT("vposVal"));
			}
			
		}
	};
	SetDefaultAuxiliaryMetalInfo(CurrentHandleLocationInfo, TEXT("handleInfo"));	
	SetDefaultAuxiliaryMetalInfo(CurrentHingeLocationInfo, TEXT("hingeInfo"));
}

void FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FVariableAreaDoorSheet::SaveAttriToJson(JsonWriter);
	auto SetDefaultAuxiliaryMetalInfo = [JsonWriter](const FAuxiliaryMetalInfo & InInfo, const FString & InJsonObjName, const int32 InType)
	{
		if (InInfo.ID != -1)
		{
			JsonWriter->WriteObjectStart(InJsonObjName);
			JsonWriter->WriteValue(TEXT("id"), InInfo.ID);
			JsonWriter->WriteValue(TEXT("name"), InInfo.Name);	
			JsonWriter->WriteValue(TEXT("metalsType"), InType);
			if (InInfo.Location.IsValid())
			{
				JsonWriter->WriteValue(TEXT("hpos"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->Hpos);
				JsonWriter->WriteValue(TEXT("rotationalDirection"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->RotateDirection);
				JsonWriter->WriteValue(TEXT("hposVal"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->HposVal);
				JsonWriter->WriteValue(TEXT("vpos"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->Vpos);
				JsonWriter->WriteValue(TEXT("vposVal"), StaticCastSharedPtr<FDoorHanlerDefaultLocation>(InInfo.Location)->VposVAl);
			}
			JsonWriter->WriteObjectEnd();
		}
	};
	FAuxiliaryMetalInfo HandleInfo = GetCurrentHandleInfo();
	SetDefaultAuxiliaryMetalInfo(HandleInfo, TEXT("handleInfo"), StaticCast<int32>(EMetalsType::MT_SHAKE_HANDS));
	HandleInfo = GetCurrentHingeleInfo();
	SetDefaultAuxiliaryMetalInfo(HandleInfo, TEXT("hingeInfo"), StaticCast<int32>(EMetalsType::MT_HINGE));
}

void FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor::CopyTo(FSCTShape * OutShape)
{
	check(OutShape);
	FVariableAreaDoorSheet::CopyTo(OutShape);
	FVariableAreaDoorSheetForSideHungDoor * OutBaseShape = StaticCast<FVariableAreaDoorSheetForSideHungDoor*>(OutShape);
	OutBaseShape->HandleList = HandleList;
	OutBaseShape->CurrentHandleLocationInfo.ID = CurrentHandleLocationInfo.ID;
	OutBaseShape->CurrentHandleLocationInfo.Name = CurrentHandleLocationInfo.Name;
	if (CurrentHandleLocationInfo.Location.IsValid())
	{
		OutBaseShape->CurrentHandleLocationInfo.Location = MakeShareable(new FDoorHanlerDefaultLocation);
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutBaseShape->CurrentHandleLocationInfo.Location)->Hpos = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->Hpos;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutBaseShape->CurrentHandleLocationInfo.Location)->HposVal = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->HposVal;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutBaseShape->CurrentHandleLocationInfo.Location)->Vpos = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->Vpos;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutBaseShape->CurrentHandleLocationInfo.Location)->VposVAl = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->VposVAl;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutBaseShape->CurrentHandleLocationInfo.Location)->RotateDirection = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location)->RotateDirection;
	}
	
	OutBaseShape->HingeList = HingeList;
	OutBaseShape->CurrentHingeLocationInfo.ID = CurrentHingeLocationInfo.ID;
	OutBaseShape->CurrentHingeLocationInfo.Name = CurrentHingeLocationInfo.Name;
	if (CurrentHingeLocationInfo.Location.IsValid())
	{
		OutBaseShape->CurrentHingeLocationInfo.Location = MakeShareable(new FDoorHanlerDefaultLocation);
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutBaseShape->CurrentHingeLocationInfo.Location)->Hpos = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHingeLocationInfo.Location)->Hpos;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutBaseShape->CurrentHingeLocationInfo.Location)->HposVal = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHingeLocationInfo.Location)->HposVal;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutBaseShape->CurrentHingeLocationInfo.Location)->Vpos = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHingeLocationInfo.Location)->Vpos;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutBaseShape->CurrentHingeLocationInfo.Location)->VposVAl = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHingeLocationInfo.Location)->VposVAl;
		StaticCastSharedPtr<FDoorHanlerDefaultLocation>(OutBaseShape->CurrentHingeLocationInfo.Location)->RotateDirection = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHingeLocationInfo.Location)->RotateDirection;
	}	
}

void FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor::ReCalDividBlocksPositionAndSize()
{
	FVariableAreaDoorSheet::ReCalDividBlocksPositionAndSize();	
	RecalHandlePosition();
}

ASCTShapeActor * FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor::SpawnShapeActor()
{	
	return FVariableAreaDoorSheet::SpawnShapeActor();
}

void FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor::ReBuildHandle()
{
	const int32 ShapeID = GetCurrentHandleInfo().ID;
	if (ShapeID == -1)
	{
		// 删除之前设置的明装拉手以及封边拉手
		for (int32 Index = Metals.Num() - 1; Index >= 0 ; --Index)
		{
			if (Metals[Index]->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SHAKE_HANDS) ||
				Metals[Index]->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SEALING_SIDE_HANDLE))
			{
				RemoveChildShape(Metals[Index]);
				Metals.RemoveAt(Index);				
			}
		}		
		return;
	}
	// 先确定该ID的五金是否已经创建 	
	EMetalsType InMetalType = EMetalsType::MT_None;
	for (const auto & Ref : GetOptionShapeMap())
	{
		if (Ref.Key == ShapeID)
		{
			InMetalType = StaticCast<EMetalsType>(Ref.Value->GetShapeCategory());
			break;
		}
	}
	// 删除互斥拉手		
	for (int32 Index = Metals.Num() - 1; Index >= 0; --Index)
	{
		if (InMetalType == EMetalsType::MT_SHAKE_HANDS && Metals[Index]->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SEALING_SIDE_HANDLE))
		{
			if (Metals[Index]->GetShapeActor())
			{
				Metals[Index]->DestroyShapeActor();				
			}
			Metals.RemoveAt(Index);
			
		}
		else if (InMetalType == EMetalsType::MT_SEALING_SIDE_HANDLE && Metals[Index]->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SHAKE_HANDS))
		{
			Metals.RemoveAt(Index);
		}
	}
	TSharedPtr<FAccessoryShape> CurMetal;
	for (const auto & Ref : Metals)
	{
		if (Ref->GetShapeCategory() == StaticCast<int32>(InMetalType))
		{
			CurMetal = Ref;
			break;
		}
	}

	if (CurMetal.IsValid())
	{
		if (CurMetal->GetShapeId() != ShapeID)
		{			
			TSharedPtr<FAccessoryShape> AccessShape = StaticCastSharedPtr<FAccessoryShape>(*(GetOptionShapeMap().Find(ShapeID)));
			TSharedPtr<FAccessoryShape> NewShape = StaticCastSharedPtr<FAccessoryShape>(FSCTShapeManager::Get()->CopyShapeToNew(AccessShape));			
			NewShape->SetShapePosX(CurMetal->GetShapePosX());
			NewShape->SetShapePosY(CurMetal->GetShapePosY());
			NewShape->SetShapePosZ(CurMetal->GetShapePosZ());
			NewShape->SetShapeRotX(CurMetal->GetShapeRotX());
			NewShape->SetShapeRotY(CurMetal->GetShapeRotY());
			NewShape->SetShapeRotZ(CurMetal->GetShapeRotZ());			
			NewShape->LoadModelMesh(true);
			AddChildShape(NewShape);
			Metals.Add(NewShape);			
			NewShape->SpawnShapeActor();
			if (NewShape->GetShapeActor())
			{
				NewShape->GetShapeActor()->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
			}
			Metals.Remove(CurMetal);
			RemoveChildShape(CurMetal);
			if (CurMetal->GetShapeActor())
			{
				CurMetal->GetShapeActor()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				CurMetal->DestroyShapeActor();
			}
		}
		else if(CurMetal->GetShapeActor() == nullptr)
		{
			CurMetal->SpawnShapeActor();
			CurMetal->GetShapeActor()->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
	else
	{
		TSharedPtr<FAccessoryShape> AccessShape = StaticCastSharedPtr<FAccessoryShape>(*(GetOptionShapeMap().Find(ShapeID)));
		TSharedPtr<FAccessoryShape> NewShape = StaticCastSharedPtr<FAccessoryShape>(FSCTShapeManager::Get()->CopyShapeToNew(AccessShape));	
		NewShape->LoadModelMesh(true);
		AddChildShape(NewShape);
		Metals.Add(NewShape);
		NewShape->SpawnShapeActor();
		NewShape->GetShapeActor()->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);		
	}

}

void FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor::RecalHandlePosition()
{
	if (CurrentHandleLocationInfo.ID == -1)	return;
	if (CurrentHandleLocationInfo.Location.IsValid() == false) return;
	TSharedPtr<FAccessoryShape> OperationShape;
	for (const auto Ref : Metals)
	{
		if (Ref->GetShapeId() == CurrentHandleLocationInfo.ID)
		{
			OperationShape = Ref;
			break;
		}
	}	
	check(OperationShape.IsValid());
	TSharedPtr<FDoorHanlerDefaultLocation> TempPtr = StaticCastSharedPtr<FDoorHanlerDefaultLocation>(CurrentHandleLocationInfo.Location);	
	if (OperationShape->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SHAKE_HANDS))
	{
		switch (TempPtr->Hpos)
		{
			case 1:
			{
				OperationShape->SetShapePosX(TempPtr->HposVal);
			}break;
			case 2:
			{
				OperationShape->SetShapePosX(GetShapeWidth() - TempPtr->HposVal);
			}break;
			case 3:
			{
				OperationShape->SetShapePosX(GetShapeWidth() / 2.0f);
			}break;
			default:
				check(false);
				break;
		}
		switch (TempPtr->Vpos)
		{
			case 1:
			{
				OperationShape->SetShapePosZ(GetShapeHeight() - TempPtr->VposVAl);
			}break;
			case 2:
			{
				OperationShape->SetShapePosZ(TempPtr->VposVAl);
			}break;
			case 3:
			{
				OperationShape->SetShapePosZ(GetShapeHeight() / 2.0f);
			}break;
			default:
				check(false);
				break;
		}
		switch (TempPtr->RotateDirection)
		{
		case 0:
			OperationShape->SetShapeRotY(0.0f);
			break;
		case 1:
			OperationShape->SetShapeRotY(90.0f);
			break;
		case 2:
			OperationShape->SetShapeRotY(-90.0f);
			break;
		default:
			check(false);
			break;
		}
		OperationShape->SetShapePosY(GetShapeDepth());
	}		
	if (OperationShape->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SHAKE_HANDS))
	{
		ShakeHanleTypePosition.HPositionType = TempPtr->Hpos;
		ShakeHanleTypePosition.HPositionValue = TempPtr->HposVal;
		ShakeHanleTypePosition.VPositionType = TempPtr->Vpos;
		ShakeHanleTypePosition.VPositionValue = TempPtr->VposVAl;
		ShakeHanleTypePosition.RotateDirection = TempPtr->RotateDirection;
	}
}

 void FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor::ReBuildHingele()
{
	 TSharedPtr<FAccessoryShape> CurMetal;
	 EMetalsType DesType = EMetalsType::MT_HINGE;
	 for (const auto & Ref : Metals)
	 {
		 if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == DesType)
		 {
			 CurMetal = Ref;
			 break;
		 }
	 }
	 if (CurMetal.IsValid() == false)
	 {
		 const int32 ShapeID = GetCurrentHandleInfo().ID;
		 TSharedPtr<FAccessoryShape> AccessShape = StaticCastSharedPtr<FAccessoryShape>(*(GetOptionShapeMap().Find(ShapeID)));		
		 AddChildShape(AccessShape);
		 Metals.Add(AccessShape);
		 Metals.Remove(CurMetal);
		 RemoveChildShape(CurMetal);		
	 }
}

 FVector FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor::GetDoorSheetVisualSize()
 {
	 FVector RetVaector = FVariableAreaDoorSheet::GetDoorSheetVisualSize();
	 do
	 {
		 if (CurrentHandleLocationInfo.ID == -1) break;
		 if (CurrentHandleLocationInfo.Location.IsValid() == false) break;
		 TSharedPtr<FAccessoryShape> OperationShape;
		 for (const auto Ref : Metals)
		 {
			 if (Ref->GetShapeId() == CurrentHandleLocationInfo.ID)
			 {
				 OperationShape = Ref;
				 break;
			 }
		 }
		 if (OperationShape.IsValid() == false) break;
		 if (OperationShape->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SEALING_SIDE_HANDLE))
		 {
			 RetVaector.Z += OperationShape->GetShapeHeight();
		 }

	 } while (false);
	
	 return RetVaector;
 }

void FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor::ProcessOptionsMetalType()
{
	for (const auto & Ref : OpetionsMetalsShapesList)
	{
		if (Ref.Value->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SHAKE_HANDS) ||
			Ref.Value->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SEALING_SIDE_HANDLE))
		{
			FAuxiliaryMetalInfo TempMetalInfo;
			TempMetalInfo.ID = Ref.Key;
			TempMetalInfo.Name = Ref.Value->GetShapeName();			
			HandleList.Add(TempMetalInfo);
		}		
		else if (Ref.Value->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_HINGE))
		{
			FAuxiliaryMetalInfo TempMetalInfo;
			TempMetalInfo.ID = Ref.Key;
			TempMetalInfo.Name = Ref.Value->GetShapeName();			
			HingeList.Add(TempMetalInfo);
		}
	}
	// 如果有明装拉手，则优先使用明装拉手
	bool bHasCommonHnadle = false;
	// 处理当前正常使用的五金
	for (const auto & Ref : GetMetals())
	{
		if (Ref->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SHAKE_HANDS))
		{
			FAuxiliaryMetalInfo & MetalInfoRef = GetCurrentHandleInfo();
			MetalInfoRef.ID = Ref->GetShapeId();			
			MetalInfoRef.Name = Ref->GetShapeName();
			bHasCommonHnadle = true;
		}
		else if (!bHasCommonHnadle && Ref->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SEALING_SIDE_HANDLE))
		{
			FAuxiliaryMetalInfo & MetalInfoRef = GetCurrentHandleInfo();
			MetalInfoRef.ID = Ref->GetShapeId();
			MetalInfoRef.Name = Ref->GetShapeName();			
		}
		if (Ref->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_HINGE))
		{
			FAuxiliaryMetalInfo & MetalInfoRef = GetCurrentHingeleInfo();
			MetalInfoRef.ID = Ref->GetShapeId();
			MetalInfoRef.Name = Ref->GetShapeName();			
		}
	}

}

