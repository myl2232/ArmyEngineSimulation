#include "SCTEmbededElectricalGroup.h"
#include "SCTModelShape.h"
#include "SCTBoardShape.h"
#include "SCTShapeManager.h"
#include "SCTAttribute.h"
#include "SCTShapeData.h"
#include "SCTShapeActor.h"
#include "SCTShapeBoxActor.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Json.h"
#include "Engine/EngineTypes.h"
#include "Engine/CollisionProfile.h"
#include "Actor/SCTModelActor.h"
#include "SCTShapeData.h"
#include "SCTModelShape.h"

void FInsertionBoard::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseFromJson(InJsonObject);	
	// 尺寸修正
	{
		Depth = Height;				
		Height = MakeShareable(new FNumberRangeAttri(this));
		Height->SetAttributeId(3);
		Height->SetRefName(TEXT("HEIGHT"));		
		StaticCastSharedPtr<FNumberRangeAttri>(Height)->SetMinValue(0.0f);
		StaticCastSharedPtr<FNumberRangeAttri>(Height)->SetMaxValue(10000.0f);
		StaticCastSharedPtr<FNumberRangeAttri>(Height)->SetAttributeValue(TEXT("100.0"));
		Width->SetAttributeValue(TEXT("100.0"));		
	}

	// 封边
	if (InJsonObject->HasField(TEXT("edgeBanding")))
	{
		const TSharedPtr<FJsonObject> & TempEdgeBandingObj = InJsonObject->GetObjectField(TEXT("edgeBanding"));
		FInsertBoardMaterial & PakRef = GetEdgeMaterial();
		PakRef.ID = TempEdgeBandingObj->GetIntegerField(TEXT("id"));
		PakRef.Name = TempEdgeBandingObj->GetStringField(TEXT("name"));
		PakRef.Url = TempEdgeBandingObj->GetStringField(TEXT("pakUrl"));
		PakRef.MD5 = TempEdgeBandingObj->GetStringField(TEXT("pakMd5"));
		PakRef.ThumbnailUrl = TempEdgeBandingObj->GetStringField(TEXT("thumbnailUrl"));
	}
	// 材质
	if (InJsonObject->HasField(TEXT("material")))
	{
		const TSharedPtr<FJsonObject> & TempMaterialObj = InJsonObject->GetObjectField(TEXT("material"));
		FInsertBoardMaterial & PakRef = GetMaterial();
		PakRef.ID = TempMaterialObj->GetIntegerField(TEXT("id"));
		PakRef.Name = TempMaterialObj->GetStringField(TEXT("name"));
		PakRef.Url = TempMaterialObj->GetStringField(TEXT("pakUrl"));
		PakRef.MD5 = TempMaterialObj->GetStringField(TEXT("pakMd5"));
		PakRef.ThumbnailUrl = TempMaterialObj->GetStringField(TEXT("thumbnailUrl"));
	}

	// 基材类型
	SubstrateType = InJsonObject->GetIntegerField(TEXT("substrateType"));

	// 基材
	if (InJsonObject->HasField(TEXT("substrate")))
	{
		const TSharedPtr<FJsonObject> & TempSubstrateObj = InJsonObject->GetObjectField(TEXT("substrate"));
		SubstrateID = TempSubstrateObj->GetIntegerField(TEXT("id"));
		SubstrateName = TempSubstrateObj->GetStringField(TEXT("name"));
	}
}

void FInsertionBoard::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseAttributesFromJson(InJsonObject);

	// 封边
	if (InJsonObject->HasField(TEXT("edgeBanding")))
	{
		const TSharedPtr<FJsonObject> & TempEdgeBandingObj = InJsonObject->GetObjectField(TEXT("edgeBanding"));
		FInsertBoardMaterial & PakRef = GetEdgeMaterial();
		PakRef.ID = TempEdgeBandingObj->GetIntegerField(TEXT("id"));
		PakRef.Name = TempEdgeBandingObj->GetStringField(TEXT("name"));
		PakRef.Url = TempEdgeBandingObj->GetStringField(TEXT("pakUrl"));
		PakRef.MD5 = TempEdgeBandingObj->GetStringField(TEXT("pakMd5"));
		PakRef.ThumbnailUrl = TempEdgeBandingObj->GetStringField(TEXT("thumbnailUrl"));
	}
	// 材质
	if (InJsonObject->HasField(TEXT("material")))
	{
		const TSharedPtr<FJsonObject> & TempMaterialObj = InJsonObject->GetObjectField(TEXT("material"));
		FInsertBoardMaterial & PakRef = GetMaterial();
		PakRef.ID = TempMaterialObj->GetIntegerField(TEXT("id"));
		PakRef.Name = TempMaterialObj->GetStringField(TEXT("name"));
		PakRef.Url = TempMaterialObj->GetStringField(TEXT("pakUrl"));
		PakRef.MD5 = TempMaterialObj->GetStringField(TEXT("pakMd5"));
		PakRef.ThumbnailUrl = TempMaterialObj->GetStringField(TEXT("thumbnailUrl"));
	}

	// 基材类型
	SubstrateType = InJsonObject->GetIntegerField(TEXT("substrateType"));

	// 基材
	if (InJsonObject->HasField(TEXT("substrate")))
	{
		const TSharedPtr<FJsonObject> & TempSubstrateObj = InJsonObject->GetObjectField(TEXT("substrate"));
		SubstrateID = TempSubstrateObj->GetIntegerField(TEXT("id"));
		SubstrateName = TempSubstrateObj->GetStringField(TEXT("name"));
	}

	//外延值
	{
		UpExtension = InJsonObject->GetNumberField(TEXT("upExtension"));
		DownExtension = InJsonObject->GetNumberField(TEXT("downExtension"));
		LeftExtension = InJsonObject->GetNumberField(TEXT("leftExtension"));
		RightExtension = InJsonObject->GetNumberField(TEXT("rightExtension"));
		InnerExtension = InJsonObject->GetNumberField(TEXT("innerExtension"));
	}

	// 修正外延值
	{
		UpFixedExtension = InJsonObject->GetNumberField(TEXT("upFixedExtension"));
		DownFixedExtension = InJsonObject->GetNumberField(TEXT("downFixedExtension"));
		LeftFixedExtension = InJsonObject->GetNumberField(TEXT("leftFixedExtension"));
		RightFixedExtension = InJsonObject->GetNumberField(TEXT("rightFixedExtension"));
		InnerFixedExtension = InJsonObject->GetNumberField(TEXT("innerFixedExtension"));
	}

	// 饰条位置	
	SetInserBoardPositionType(StaticCast<EInserBoardPositionType>(InJsonObject->GetIntegerField(TEXT("positionType"))));

	// 箱体拉手
	if (InJsonObject->HasField(TEXT("lateralBoxShape")))
	{
		const TSharedPtr<FJsonObject> & ObjRef = InJsonObject->GetObjectField(TEXT("lateralBoxShape"));
		SetLateralBoxHanleLenToInsertionBoard(ObjRef->GetNumberField(TEXT("lenToInsertalBoard")));
		TSharedPtr<FSCTShape> Shape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ST_Accessory, ObjRef->GetIntegerField(TEXT("shapeId")));
		TSharedPtr<FAccessoryShape> NewShape = StaticCastSharedPtr<FAccessoryShape>(FSCTShapeManager::Get()->CopyShapeToNew(Shape));
		SetLateralBoxHanleShape(NewShape);
	}
}

void FInsertionBoard::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseShapeFromJson(InJsonObject);	
}

void FInsertionBoard::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseContentFromJson(InJsonObject);
}

void FInsertionBoard::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveToJson(JsonWriter);
	// 封边
	{
		JsonWriter->WriteObjectStart(TEXT("edgeBanding"));
		JsonWriter->WriteValue(TEXT("id"),EdgeBanding.ID);
		JsonWriter->WriteValue(TEXT("name"),EdgeBanding.Name);
		JsonWriter->WriteValue(TEXT("pakUrl"), EdgeBanding.Url);
		JsonWriter->WriteValue(TEXT("pakMd5"), EdgeBanding.MD5);
		JsonWriter->WriteValue(TEXT("thumbnailUrl"), EdgeBanding.ThumbnailUrl);		
		JsonWriter->WriteObjectEnd();

	}
	// 材质
	{
		JsonWriter->WriteObjectStart(TEXT("material"));
		JsonWriter->WriteValue(TEXT("id"), Material.ID);
		JsonWriter->WriteValue(TEXT("name"), Material.Name);
		JsonWriter->WriteValue(TEXT("pakUrl"), Material.Url);
		JsonWriter->WriteValue(TEXT("pakMd5"), Material.MD5);
		JsonWriter->WriteValue(TEXT("thumbnailUrl"), Material.ThumbnailUrl);
		JsonWriter->WriteValue(TEXT("OptimizeParam"), Material.OptimizeParam);
		JsonWriter->WriteObjectEnd();
	}

	// 基材类型	
	JsonWriter->WriteValue(TEXT("substrateType"),SubstrateType);

	// 基材	
	{
		JsonWriter->WriteObjectStart(TEXT("substrate"));
		JsonWriter->WriteValue(TEXT("id"),SubstrateID);		
		JsonWriter->WriteValue(TEXT("name"), SubstrateName);
		JsonWriter->WriteObjectEnd();
	}
	//外延值
	{
		JsonWriter->WriteValue(TEXT("upExtension"), UpExtension);
		JsonWriter->WriteValue(TEXT("downExtension"), DownExtension);
		JsonWriter->WriteValue(TEXT("leftExtension"), LeftExtension);
		JsonWriter->WriteValue(TEXT("rightExtension"), RightExtension);
		JsonWriter->WriteValue(TEXT("innerExtension"), InnerExtension);
	}

	//修正外延值
	{
		JsonWriter->WriteValue(TEXT("upFixedExtension"), UpFixedExtension);
		JsonWriter->WriteValue(TEXT("downFixedExtension"), DownFixedExtension);
		JsonWriter->WriteValue(TEXT("leftFixedExtension"), LeftFixedExtension);
		JsonWriter->WriteValue(TEXT("rightFixedExtension"), RightFixedExtension);
		JsonWriter->WriteValue(TEXT("innerFixedExtension"), InnerFixedExtension);
	}

	// 箱体拉手存储
	if (LateralBoxShape.IsValid())
	{
		JsonWriter->WriteObjectStart(TEXT("lateralBoxShape"));
		JsonWriter->WriteValue(TEXT("shapeId"), LateralBoxShape->GetShapeId());
		JsonWriter->WriteValue(TEXT("lenToInsertalBoard"), GetLateralBoxLenToInsertionBoard());
		JsonWriter->WriteObjectEnd();
	}

}

void FInsertionBoard::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveAttriToJson(JsonWriter);
	// 封边
	{
		JsonWriter->WriteObjectStart(TEXT("edgeBanding"));
		JsonWriter->WriteValue(TEXT("id"), EdgeBanding.ID);
		JsonWriter->WriteValue(TEXT("name"), EdgeBanding.Name);
		JsonWriter->WriteValue(TEXT("pakUrl"), EdgeBanding.Url);
		JsonWriter->WriteValue(TEXT("pakMd5"), EdgeBanding.MD5);
		JsonWriter->WriteValue(TEXT("thumbnailUrl"), EdgeBanding.ThumbnailUrl);
		JsonWriter->WriteObjectEnd();

	}
	// 材质
	{
		JsonWriter->WriteObjectStart(TEXT("material"));
		JsonWriter->WriteValue(TEXT("id"), Material.ID);
		JsonWriter->WriteValue(TEXT("name"), Material.Name);
		JsonWriter->WriteValue(TEXT("pakUrl"), Material.Url);
		JsonWriter->WriteValue(TEXT("pakMd5"), Material.MD5);
		JsonWriter->WriteValue(TEXT("thumbnailUrl"), Material.ThumbnailUrl);
		JsonWriter->WriteValue(TEXT("OptimizeParam"), Material.OptimizeParam);
		JsonWriter->WriteObjectEnd();
	}

	// 基材类型	
	JsonWriter->WriteValue(TEXT("substrateType"), SubstrateType);

	// 基材	
	{
		JsonWriter->WriteObjectStart(TEXT("substrate"));
		JsonWriter->WriteValue(TEXT("id"), SubstrateID);
		JsonWriter->WriteValue(TEXT("name"), SubstrateName);
		JsonWriter->WriteObjectEnd();
	}

	//外延值
	{
		JsonWriter->WriteValue(TEXT("upExtension"), UpExtension);
		JsonWriter->WriteValue(TEXT("downExtension"), DownExtension);
		JsonWriter->WriteValue(TEXT("leftExtension"), LeftExtension);
		JsonWriter->WriteValue(TEXT("rightExtension"), RightExtension);
		JsonWriter->WriteValue(TEXT("innerExtension"), InnerExtension);
	}

	//修正外延值
	{
		JsonWriter->WriteValue(TEXT("upFixedExtension"), UpFixedExtension);
		JsonWriter->WriteValue(TEXT("downFixedExtension"), DownFixedExtension);
		JsonWriter->WriteValue(TEXT("leftFixedExtension"), LeftFixedExtension);
		JsonWriter->WriteValue(TEXT("rightFixedExtension"), RightFixedExtension);
		JsonWriter->WriteValue(TEXT("innerFixedExtension"), InnerFixedExtension);
	}
	// 饰条位置
	JsonWriter->WriteValue(TEXT("positionType"),StaticCast<int32>(GetInserBoardPositionType()));

	// 箱体拉手存储
	if (LateralBoxShape.IsValid())
	{
		JsonWriter->WriteObjectStart(TEXT("lateralBoxShape"));		
		JsonWriter->WriteValue(TEXT("shapeId"), LateralBoxShape->GetShapeId());
		JsonWriter->WriteValue(TEXT("lenToInsertalBoard"), GetLateralBoxLenToInsertionBoard());
		JsonWriter->WriteObjectEnd();
	}
}

void FInsertionBoard::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveShapeToJson(JsonWriter);
}

void FInsertionBoard::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveContentToJson(JsonWriter);
}

void FInsertionBoard::CopyTo(FSCTShape* OutShape)
{
	check(OutShape->GetShapeType() == ST_InsertionBoard);
	FSCTShape::CopyTo(OutShape);
	FInsertionBoard * TrueOutShape = StaticCast<FInsertionBoard*>(OutShape);
	TrueOutShape->UpExtension = UpExtension;
	TrueOutShape->DownExtension = DownExtension;
	TrueOutShape->LeftExtension = LeftExtension;
	TrueOutShape->RightExtension = RightExtension;
	TrueOutShape->InnerExtension = InnerExtension;
	TrueOutShape->UpFixedExtension = UpFixedExtension;
	TrueOutShape->DownFixedExtension = DownFixedExtension;
	TrueOutShape->LeftFixedExtension = LeftFixedExtension;
	TrueOutShape->RightFixedExtension = RightFixedExtension;
	TrueOutShape->InnerFixedExtension = InnerFixedExtension;
	TrueOutShape->EdgeBanding = EdgeBanding;
	TrueOutShape->Material = Material;
	TrueOutShape->SubstrateType = SubstrateType;
	TrueOutShape->SubstrateID = SubstrateID;
	TrueOutShape->SubstrateName = SubstrateName;
	TrueOutShape->InserBoardPositionType = InserBoardPositionType;
	if (BoardShape.IsValid())
	{
		TrueOutShape->BoardShape = MakeShareable(new FBoardShape);
		BoardShape->CopyTo(TrueOutShape->BoardShape.Get());
		if (LateralBoxShape.IsValid())
		{
			TSharedPtr<FAccessoryShape> NewLateralBoxShape = MakeShareable(new FAccessoryShape);
			LateralBoxShape->CopyTo(NewLateralBoxShape.Get());
			TrueOutShape->SetLateralBoxHanleShape(NewLateralBoxShape);
			TrueOutShape->SetLateralBoxHanleLenToInsertionBoard(GetLateralBoxLenToInsertionBoard());
		}
	}	
}

ASCTShapeActor* FInsertionBoard::SpawnShapeActor()
{
    ShapeActor = FSCTShape::SpawnShapeActor();	
	if (BoardShape.IsValid())
	{
		UpdateBoardShape();
	}
	return 	ShapeActor;
}

void FInsertionBoard::SpawnActorsForSelected(FName InProfileName)
{	
}

void FInsertionBoard::SetCollisionProfileName(FName InProfileName)
{
	FSCTShape::SetCollisionProfileName(InProfileName);	
}

void FInsertionBoard::GetResourceUrls(TArray<FString>& OutResourceUrls)
{
	FSCTShape::GetResourceUrls(OutResourceUrls);
	if (Material.Url.IsEmpty() == false)
	{
		OutResourceUrls.Add(Material.Url);
	}
	if (LateralBoxShape.IsValid() && (LateralBoxShape->GetFileUrl().IsEmpty() == false))
	{
		LateralBoxShape->GetResourceUrls(OutResourceUrls);
	}
}

void FInsertionBoard::GetFileCachePaths(TArray<FString>& OutFileCachePaths)
{
	FSCTShape::GetFileCachePaths(OutFileCachePaths);
	if (Material.Url.IsEmpty() == false)
	{
		const FString  RelativePath = FString::Printf(TEXT("Material/Board/BoardMaterial_%d.pak"), GetMaterial().ID);
		OutFileCachePaths.Emplace(
			FPaths::Combine(FGlobalDataForShape::GetInsPtr()->GetRootDir(), RelativePath)
		);
	}
	if (LateralBoxShape.IsValid() && (LateralBoxShape->GetFileUrl().IsEmpty() == false))
	{
		LateralBoxShape->GetFileCachePaths(OutFileCachePaths);
	}
}

void FInsertionBoard::SetLateralBoxHanleShape(const TSharedPtr<FAccessoryShape> InShape)
{
	if (LateralBoxShape.IsValid() == false)
	{
		LateralBoxShape = InShape;
		AddChildShape(LateralBoxShape);
		LateralBoxShape->SetParentShape(this);
	}
}

void FInsertionBoard::RemoveLateralBoxHandleShape()
{
	if (LateralBoxShape.IsValid())
	{
		RemoveChildShape(LateralBoxShape);
		LateralBoxShape = nullptr;
	}
}

void FInsertionBoard::UpdateBoardShape()
{
	if (InserBoardPositionType == EInserBoardPositionType::E_None) return;
		 

	const float InsertaionBoardShapeWidth = GetShapeWidth() + GetLeftExtension() + GetLeftFixedExtension() + GetRightExtension() + GetRightFixedExtension();
	float InsertaionBoardShapeHeight = GetShapeHeight() + GetUpExtension() + GetUpFixedExtension() + GetDownExtension() + GetDownFixedExtension();
	const float InsertaionBoardShapeDepth = GetShapeDepth();
	float InsertionBoardPosZ = -(GetDownExtension() + GetDownFixedExtension());
	// 更新箱体拉手信息
	if(IsLaterBoxHanleValid())
	{

		float AccessPosX = 0.0f;
		float AccessPosY = 0.0f;
		float AccessPosZ = 0.0f;
		float AccessW = InsertaionBoardShapeWidth;

		// 设置箱体拉手的空间位置以及尺寸
		auto SetHanlePositionAndSize = [this](TSharedPtr<FAccessoryShape> & Shape, const float InPosX, const float InPosY, const float InPosZ, const float InW)
		{
			check(Shape.IsValid());
			Shape->SetShapePosX(InPosX);
			Shape->SetShapePosY(InPosY);
			Shape->SetShapePosZ(InPosZ);
			Shape->SetShapeWidth(InW);
		};
		check(LateralBoxShape->GetMetalsPropertyList().Num() > 0);
		const FString & PropertyValueIdentity = LateralBoxShape->GetMetalsPropertyList()[0].PropertyValueIdentity;	
		//地柜上拉手
		if (PropertyValueIdentity == TEXT("MIQHn1"))
		{		
			AccessPosX += AccessW / 2.0f - GetLeftExtension() - GetLeftFixedExtension();
			AccessPosZ += InsertaionBoardShapeHeight - GetDownExtension() - GetDownFixedExtension() - LateralBoxShape->GetShapeHeight();
			SetHanlePositionAndSize(LateralBoxShape, AccessPosX, AccessPosY, AccessPosZ, AccessW);
			InsertaionBoardShapeHeight = InsertaionBoardShapeHeight - (LateralBoxShape->GetShapeHeight() - GetLateralBoxLenToInsertionBoard());			
		}
		if (LateralBoxShape->GetShapeActor() == nullptr && ShapeActor)
		{
			AActor * Actor = LateralBoxShape->SpawnShapeActor();
			Actor->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}

	 // 处理饰条板件
	{
		if (BoardShape.IsValid() == false)
		{
			CreateBoardShape();
		}
		BoardShape->SetShapeWidth(InsertaionBoardShapeWidth);
		BoardShape->SetShapeHeight(InsertaionBoardShapeHeight);
		BoardShape->SetShapeDepth(InsertaionBoardShapeDepth);

		BoardShape->SetShapePosX(-(GetLeftExtension() + GetLeftFixedExtension()));
		BoardShape->SetShapePosZ(InsertionBoardPosZ);

		FPakMetaData PakMetaData;
		PakMetaData.ID = Material.ID;
		PakMetaData.MD5 = Material.MD5;
		PakMetaData.Name = Material.Name;
		PakMetaData.OptimizeParam = Material.OptimizeParam;
		PakMetaData.ThumbnailUrl = Material.ThumbnailUrl;
		PakMetaData.Url = Material.Url;
		BoardShape->SetMaterialData(PakMetaData);
		if (BoardShape->GetShapeActor() == nullptr && ShapeActor)
		{
			AActor * BoardActor = BoardShape->SpawnShapeActor();
			BoardActor->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
}

bool FInsertionBoard::CreateBoardShape()
{
	bool RetResult = false;
	do
	{
		if (BoardShape.IsValid()) return false;
		BoardShape = MakeShareable(new FBoardShape);
		BoardShape->SetParentShape(this);
		AddChildShape(BoardShape);
		BoardShape->SetBoardShapeType(BoST_Rectangle);
		BoardShape->SetShapeCategory(BUT_BackBoard);
			
		BoardShape->SetShapeWidth(GetShapeWidth());
		BoardShape->SetShapeHeight(GetShapeDepth());
		BoardShape->SetShapeDepth(GetShapeHeight());
		RetResult = true;
	} while (false);
	return RetResult;
}


/**------------------------ 电器饰条组 --------------------*/

void FEmbededElectricalGroup::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseFromJson(InJsonObject);	
	SetRightRelativeBoardWidth(InJsonObject->GetNumberField(TEXT("rightRelativeBoardWidth")));
	SetLeftRelativeBoardWidth(InJsonObject->GetNumberField(TEXT("leftRelativeBoardWidth")));
	if (InJsonObject->HasField(TEXT("children")))
	{
		const TArray<TSharedPtr<FJsonValue>> & ChildrenRef = InJsonObject->GetArrayField(TEXT("children"));
		for (const auto & Ref : ChildrenRef)
		{	
			const TSharedPtr<FJsonObject> & ObjRef = Ref->AsObject();
			const EShapeType ShapeType = StaticCast<EShapeType>(ObjRef->GetIntegerField(TEXT("type")));
			const int32 ShapeId = ObjRef->GetIntegerField(TEXT("id"));
			if (ShapeType == ST_Accessory)
			{
				TSharedPtr<FAccessoryShape> Shape = StaticCastSharedPtr<FAccessoryShape>(FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ST_Accessory, ShapeId));
				check(Shape.IsValid());
				Shape->ParseAttributesFromJson(ObjRef);
				SetEmbededEletricalShape(Shape);
				AddChildShape(Shape);
			}
			else if (ShapeType == ST_InsertionBoard)
			{
				TSharedPtr<FInsertionBoard> Shape = StaticCastSharedPtr<FInsertionBoard>(FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ST_InsertionBoard, ShapeId));
				check(Shape.IsValid());
				Shape->ParseAttributesFromJson(ObjRef);
				SetInsertionBoardShape(Shape);
				AddChildShape(Shape);
			}
			else
			{
				check(false);
			}			 
		}
	}
}

void FEmbededElectricalGroup::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseAttributesFromJson(InJsonObject);		
	SetRightRelativeBoardWidth(InJsonObject->GetNumberField(TEXT("rightRelativeBoardWidth")));
	SetLeftRelativeBoardWidth(InJsonObject->GetNumberField(TEXT("leftRelativeBoardWidth")));
	if (InJsonObject->HasField(TEXT("children")))
	{
		const TArray<TSharedPtr<FJsonValue>> & ChildrenRef = InJsonObject->GetArrayField(TEXT("children"));
		for (const auto & Ref : ChildrenRef)
		{
			const TSharedPtr<FJsonObject> & ObjRef = Ref->AsObject();
			const EShapeType ShapeType = StaticCast<EShapeType>(ObjRef->GetIntegerField(TEXT("type")));
			const int32 ShapeId = ObjRef->GetIntegerField(TEXT("id"));
			if (ShapeType == ST_Accessory)
			{
				TSharedPtr<FAccessoryShape> Shape = StaticCastSharedPtr<FAccessoryShape>(FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ST_Accessory, ShapeId));
				check(Shape.IsValid());
				TSharedPtr<FAccessoryShape> NewShape = StaticCastSharedPtr<FAccessoryShape>(FSCTShapeManager::Get()->CopyShapeToNew(Shape));				
				NewShape->ParseAttributesFromJson(ObjRef);
				SetEmbededEletricalShape(NewShape);				
			}
			else if (ShapeType == ST_InsertionBoard)
			{
				TSharedPtr<FInsertionBoard> Shape = StaticCastSharedPtr<FInsertionBoard>(FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ST_InsertionBoard, ShapeId));
				check(Shape.IsValid());
				TSharedPtr<FInsertionBoard> NewShape = StaticCastSharedPtr<FInsertionBoard>(FSCTShapeManager::Get()->CopyShapeToNew(Shape));
				NewShape->ParseAttributesFromJson(ObjRef);
				SetInsertionBoardShape(NewShape);
			}
			else
			{
				check(false);
			}

		}
	}
}

void FEmbededElectricalGroup::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseShapeFromJson(InJsonObject);
}

void FEmbededElectricalGroup::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseContentFromJson(InJsonObject);
}

void FEmbededElectricalGroup::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveToJson(JsonWriter);	
	JsonWriter->WriteValue(TEXT("rightRelativeBoardWidth"), GetRightRelativeBoardWidth());
	JsonWriter->WriteValue(TEXT("leftRelativeBoardWidth"), GetLeftRelativeBoardWidth());

	JsonWriter->WriteArrayStart(TEXT("children"));	
	// 饰条
	if (InsertionBoardShape.IsValid())
	{		
		JsonWriter->WriteObjectStart();
		InsertionBoardShape->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	// 电器
	if (EmbededEletricalShape.IsValid())
	{
		JsonWriter->WriteObjectStart();
		EmbededEletricalShape->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}

	JsonWriter->WriteArrayEnd();
}

void FEmbededElectricalGroup::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveAttriToJson(JsonWriter);
	JsonWriter->WriteValue(TEXT("rightRelativeBoardWidth"), GetRightRelativeBoardWidth());
	JsonWriter->WriteValue(TEXT("leftRelativeBoardWidth"), GetLeftRelativeBoardWidth());	
	JsonWriter->WriteArrayStart(TEXT("children"));
	// 饰条
	if (InsertionBoardShape.IsValid())
	{
		JsonWriter->WriteObjectStart();
		InsertionBoardShape->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	// 电器
	if (EmbededEletricalShape.IsValid())
	{
		JsonWriter->WriteObjectStart();
		EmbededEletricalShape->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
}

void FEmbededElectricalGroup::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveShapeToJson(JsonWriter);	
}

void FEmbededElectricalGroup::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveContentToJson(JsonWriter);
}

void FEmbededElectricalGroup::CopyTo(FSCTShape* OutShape)
{
	check(OutShape->GetShapeType() == ST_EmbeddedElectricalGroup);
	FSCTShape::CopyTo(OutShape);
	FEmbededElectricalGroup * TrueShape = StaticCast<FEmbededElectricalGroup*>(OutShape);
	TrueShape->LeftRelativeBoardWidth = LeftRelativeBoardWidth;
	TrueShape->RightRelativeBoardWidth = RightRelativeBoardWidth;	
	if (GetInsertionBoardShape().IsValid())
	{
		TSharedPtr<FInsertionBoard> InsertionBoardShape = MakeShareable(new FInsertionBoard);
		GetInsertionBoardShape()->CopyTo(InsertionBoardShape.Get());
		TrueShape->SetInsertionBoardShape(InsertionBoardShape);			
	}
	if (GetEmbededEletricalShape().IsValid())
	{
		TSharedPtr<FAccessoryShape> EmbededEletricalShape = MakeShareable(new FAccessoryShape);
		GetEmbededEletricalShape()->CopyTo(EmbededEletricalShape.Get());
		TrueShape->SetEmbededEletricalShape(EmbededEletricalShape);		
	}
}

ASCTShapeActor* FEmbededElectricalGroup::SpawnShapeActor()
{
	ASCTShapeActor* GroupActor = FSCTShape::SpawnShapeActor();
	if (GetInsertionBoardShape().IsValid())
	{
		ASCTShapeActor * InsertionBoardActor = InsertionBoardShape->SpawnShapeActor();
		InsertionBoardActor->AttachToActor(GroupActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	if (GetEmbededEletricalShape().IsValid())
	{
		ASCTShapeActor * EmbedEletricalActor = EmbededEletricalShape->SpawnShapeActor();
		EmbedEletricalActor->AttachToActor(GroupActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	return 	GroupActor;
}

void FEmbededElectricalGroup::SpawnActorsForSelected(FName InProfileName)
{		
	ASCTShapeActor* CurActor = GetShapeActor();
	if (!CurActor)
	{
		return;
	}
	//创建型录外包框
	ASCTWireframeActor* WireFrameActor = SpawnWireFrameActor();
	WireFrameActor->UpdateActorDimension();
	WireFrameActor->SetActorHiddenInGame(true);
	WireFrameActor->AttachToActor(CurActor, FAttachmentTransformRules::KeepRelativeTransform);
	//生成模型的包围盒，用于鼠标点选
	ASCTBoundingBoxActor* BoundingBoxActor = SpawnBoundingBoxActor();
	BoundingBoxActor->SetCollisionProfileName(InProfileName);
	BoundingBoxActor->SetLineAlpha(0.0f);
	BoundingBoxActor->SetRegionAlpha(0.0f);
	BoundingBoxActor->UpdateActorDimension();
	BoundingBoxActor->AttachToActor(CurActor, FAttachmentTransformRules::KeepRelativeTransform);
}

void FEmbededElectricalGroup::SetCollisionProfileName(FName InProfileName)
{
	FSCTShape::SetCollisionProfileName(InProfileName);	
	EmbededEletricalShape->SetCollisionProfileName(InProfileName);
	ProfileName = InProfileName;
}

void FEmbededElectricalGroup::GetResourceUrls(TArray<FString> &OutResourceUrls)
{
	FSCTShape::GetResourceUrls(OutResourceUrls);
	if (InsertionBoardShape.IsValid())
	{
		InsertionBoardShape->GetResourceUrls(OutResourceUrls);
	}
	if (EmbededEletricalShape.IsValid())
	{
		EmbededEletricalShape->GetResourceUrls(OutResourceUrls);
	}
}

void FEmbededElectricalGroup::GetFileCachePaths(TArray<FString> &OutFileCachePaths)
{
	FSCTShape::GetFileCachePaths(OutFileCachePaths);	
	if (InsertionBoardShape.IsValid())
	{
		InsertionBoardShape->GetFileCachePaths(OutFileCachePaths);
	}
	if (EmbededEletricalShape.IsValid())
	{
		EmbededEletricalShape->GetFileCachePaths(OutFileCachePaths);
	}
}

void FEmbededElectricalGroup::UpdateEmbededEletricalShape()
{
	if (EmbededEletricalShape.IsValid() == false) return;
	const FVector BoxCenter = EmbededEletricalShape->GetBoxSphereBounds(true).GetBox().GetCenter() * 10.0f;
	const FVector BoxExtern = EmbededEletricalShape->GetBoxSphereBounds(true).GetBox().GetExtent() * 10.0f;
	//  设置五金电器的位置
	{	
		if (EmbededEletricalShape->IsMeshValid())
		{
			//EmbededEletricalShape->SetShapePosY(-BoxCenter.Y + BoxExtern.Y + GetShapeDepth() - BoxExtern.Y * 2.0f + (EmbededEletricalShape->GetShapeDepth() - BoxExtern.Y * 2.0f));
			EmbededEletricalShape->SetShapePosY(-BoxCenter.Y + BoxExtern.Y + GetShapeDepth() - BoxExtern.Y * 2.0f);
			EmbededEletricalShape->SetShapePosX(-BoxCenter.X + BoxExtern.X +(GetShapeWidth()) / 2.0f - BoxExtern.X );			
		}
	}
	if (InsertionBoardShape.IsValid())
	{	
		/*if (EmbededEletricalShape->IsMeshValid())
		{
			EmbededEletricalShape->SetShapePosY(BoxCenter.Y + BoxExtern.Y + GetShapeDepth() - BoxExtern.Y * 2.0f - InsertionBoardShape->GetShapeDepth());
		}*/
		InsertionBoardShape->SetShapeWidth(GetShapeWidth());		
		InsertionBoardShape->SetShapePosX(0.0f);
		InsertionBoardShape->SetShapePosY(GetShapeDepth() - InsertionBoardShape->GetShapeDepth());
		float InsertionBoardZ = 0.0f;
		float InsertionBoardHeight = 20.0f;
		check(InsertionBoardShape->GetInserBoardPositionType() != FInsertionBoard::EInserBoardPositionType::E_None);
		if (InsertionBoardShape->GetInserBoardPositionType() == FInsertionBoard::EInserBoardPositionType::E_Up)
		{
			float TempEmbedElectricalBottomPosition = EmbededEletricalShape->GetShapePosZ();
			if (EmbededEletricalShape->IsMeshValid())
			{
				TempEmbedElectricalBottomPosition = TempEmbedElectricalBottomPosition - (BoxCenter.Z - BoxExtern.Z);
			}
			InsertionBoardHeight = GetShapeHeight() - (TempEmbedElectricalBottomPosition + EmbededEletricalShape->GetShapeHeight());
			InsertionBoardZ = GetShapeHeight() - InsertionBoardHeight;
		}
		InsertionBoardShape->SetShapePosZ(InsertionBoardZ);
		InsertionBoardShape->SetShapeHeight(InsertionBoardHeight);
		if (InsertionBoardShape->GetShapeActor() == nullptr && ShapeActor)
		{
			AActor * InsertionActor = InsertionBoardShape->SpawnShapeActor();
			InsertionActor->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
		}
		InsertionBoardShape->SetCollisionProfileName(ProfileName);
		InsertionBoardShape->UpdateBoardShape();
	}
	
}

void FEmbededElectricalGroup::SetInsertionBoardShape(TSharedPtr<FInsertionBoard> InInsertionBoardShape)
{	
	if (InsertionBoardShape.IsValid())
	{
		RemoveChildShape(InsertionBoardShape);
	}
	InsertionBoardShape = InInsertionBoardShape;
	InsertionBoardShape->SetParentShape(this);
	AddChildShape(InInsertionBoardShape);
}

void FEmbededElectricalGroup::RemoveInserionBoardShpae()
{
	if (InsertionBoardShape.IsValid() == false) return;
	RemoveChildShape(InsertionBoardShape);
	InsertionBoardShape = nullptr;
}

void FEmbededElectricalGroup::SetEmbededEletricalShape(TSharedPtr<FAccessoryShape> InShape)
{
	if (EmbededEletricalShape.IsValid())
	{
		RemoveChildShape(EmbededEletricalShape);
	}
	EmbededEletricalShape = InShape;
	EmbededEletricalShape->SetParentShape(this);
	AddChildShape(EmbededEletricalShape);
}
