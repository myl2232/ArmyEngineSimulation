
#include "SCTVeneeredSideBoardGroup.h"
#include "SCTModelShape.h"
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
#include "SCTGlobalDataForShape.h"

void FVeneeredSudeBoardBase::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseFromJson(InJsonObject);
	if (InJsonObject->HasTypedField<EJson::Number>(TEXT("modelShape")))
	{
		ModelShape = StaticCast<EVeneeredSudeBoardShape>(InJsonObject->GetIntegerField(TEXT("modelShape")));
	}
	// 材质
	if (InJsonObject->HasTypedField<EJson::Object>(TEXT("material")))
	{
		const TSharedPtr<FJsonObject> & MaterialObjRef = InJsonObject->GetObjectField(TEXT("material"));
		Material.ID = MaterialObjRef->GetIntegerField(TEXT("id"));
		Material.Name = MaterialObjRef->GetStringField(TEXT("name"));
		Material.Url = MaterialObjRef->GetStringField(TEXT("pakUrl"));
		Material.MD5 = MaterialObjRef->GetStringField(TEXT("pakMd5"));
		Material.ThumbnailUrl = MaterialObjRef->GetStringField(TEXT("thumbnailUrl"));
		Material.OptimizeParam = MaterialObjRef->GetStringField(TEXT("optimizeParam"));
	}
	if (InJsonObject->HasTypedField<EJson::Number>(TEXT("position")))
	{
		Position = StaticCast<EPosition>(InJsonObject->GetIntegerField(TEXT("position")));
	}
	// 封边
	if (InJsonObject->HasField(TEXT("edgeBanding")))
	{
		const TSharedPtr<FJsonObject> & TempEdgeBandingObj = InJsonObject->GetObjectField(TEXT("edgeBanding"));
		FVeneeredSudeBoardMaterial & PakRef = GetEdgeMaterial();
		PakRef.ID = TempEdgeBandingObj->GetIntegerField(TEXT("id"));
		PakRef.Name = TempEdgeBandingObj->GetStringField(TEXT("name"));
		PakRef.Url = TempEdgeBandingObj->GetStringField(TEXT("pakUrl"));
		PakRef.MD5 = TempEdgeBandingObj->GetStringField(TEXT("pakMd5"));
		PakRef.ThumbnailUrl = TempEdgeBandingObj->GetStringField(TEXT("thumbnailUrl"));
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

void FVeneeredSudeBoardBase::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseAttributesFromJson(InJsonObject);
	if (InJsonObject->HasTypedField<EJson::Number>(TEXT("modelShape")))
	{
		ModelShape = StaticCast<EVeneeredSudeBoardShape>(InJsonObject->GetIntegerField(TEXT("modelShape")));
	}
	// 材质
	if (InJsonObject->HasTypedField<EJson::Object>(TEXT("material")))
	{
		const TSharedPtr<FJsonObject> & MaterialObjRef = InJsonObject->GetObjectField(TEXT("material"));
		Material.ID = MaterialObjRef->GetIntegerField(TEXT("id"));
		Material.Name = MaterialObjRef->GetStringField(TEXT("name"));
		Material.Url = MaterialObjRef->GetStringField(TEXT("pakUrl"));
		Material.MD5 = MaterialObjRef->GetStringField(TEXT("pakMd5"));
		Material.ThumbnailUrl = MaterialObjRef->GetStringField(TEXT("thumbnailUrl"));
		Material.OptimizeParam = MaterialObjRef->GetStringField(TEXT("optimizeParam"));
	}
	if (InJsonObject->HasTypedField<EJson::Number>(TEXT("position")))
	{
		Position = StaticCast<EPosition>(InJsonObject->GetIntegerField(TEXT("position")));
	}
	// 封边
	if (InJsonObject->HasField(TEXT("edgeBanding")))
	{
		const TSharedPtr<FJsonObject> & TempEdgeBandingObj = InJsonObject->GetObjectField(TEXT("edgeBanding"));
		FVeneeredSudeBoardMaterial & PakRef = GetEdgeMaterial();
		PakRef.ID = TempEdgeBandingObj->GetIntegerField(TEXT("id"));
		PakRef.Name = TempEdgeBandingObj->GetStringField(TEXT("name"));
		PakRef.Url = TempEdgeBandingObj->GetStringField(TEXT("pakUrl"));
		PakRef.MD5 = TempEdgeBandingObj->GetStringField(TEXT("pakMd5"));
		PakRef.ThumbnailUrl = TempEdgeBandingObj->GetStringField(TEXT("thumbnailUrl"));
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

void FVeneeredSudeBoardBase::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseShapeFromJson(InJsonObject);
}

void FVeneeredSudeBoardBase::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseContentFromJson(InJsonObject);
}

void FVeneeredSudeBoardBase::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveToJson(JsonWriter);
	
	JsonWriter->WriteValue(TEXT("modelShape"),StaticCast<int32>(ModelShape));
	
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
	JsonWriter->WriteValue(TEXT("position"),StaticCast<int>(Position));

	
}

void FVeneeredSudeBoardBase::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveAttriToJson(JsonWriter);
	JsonWriter->WriteValue(TEXT("modelShape"), StaticCast<int32>(ModelShape));

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
	JsonWriter->WriteValue(TEXT("position"), StaticCast<int>(Position));
}

void FVeneeredSudeBoardBase::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveShapeToJson(JsonWriter);
}

void FVeneeredSudeBoardBase::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveContentToJson(JsonWriter);
}

void FVeneeredSudeBoardBase::CopyTo(FSCTShape * OutShape)
{
	FSCTShape::CopyTo(OutShape);
	FVeneeredSudeBoardBase * Shape = StaticCast<FVeneeredSudeBoardBase*>(OutShape);
	Shape->ModelShape = ModelShape;
	Shape->Material = Material;
	Shape->EdgeBanding = EdgeBanding;
	Shape->SubstrateID = SubstrateID;
	Shape->SubstrateName = SubstrateName;
	Shape->SubstrateType = SubstrateType;
	Shape->Position = Position;
	Shape->bToSpawnActor = bToSpawnActor;
}

ASCTShapeActor * FVeneeredSudeBoardBase::SpawnShapeActor()
{
	return FSCTShape::SpawnShapeActor();
}

void FVeneeredSudeBoardBase::SpawnActorsForSelected(FName InSelectProfileName)
{
	ASCTShapeActor* Actor = GetShapeActor();
	if (!Actor)	return;

	//设置ProfileName
	SetCollisionProfileName(InSelectProfileName);
	//创建型录外包框
	ASCTWireframeActor* WireFrameActor = SpawnWireFrameActor();
	WireFrameActor->UpdateActorDimension();
	WireFrameActor->SetActorHiddenInGame(true);
	WireFrameActor->AttachToActor(Actor, FAttachmentTransformRules::KeepRelativeTransform);
}

void FVeneeredSudeBoardBase::SetCollisionProfileName(FName InProfileName)
{
	FSCTShape::SetCollisionProfileName(InProfileName);
}

void FVeneeredSudeBoardBase::GetResourceUrls(TArray<FString>& OutResourceUrls)
{
	if (Material.ID != -1 && Material.Url.IsEmpty() == false)
	{
		OutResourceUrls.Add(Material.Url);
	}
}

void FVeneeredSudeBoardBase::GetFileCachePaths(TArray<FString>& OutFileCachePaths)
{
	if (Material.Url.IsEmpty() == false)
	{
		const FString  RelativePath = FString::Printf(TEXT("Material/Board/BoardMaterial_%d.pak"), GetMaterial().ID);
		OutFileCachePaths.Emplace(
			FPaths::Combine(FGlobalDataForShape::GetInsPtr()->GetRootDir(), RelativePath)
		);
	}
}

void FVeneeredSudeBoardBase::HiddenVeneeredSudeBoard(bool bHidden)
{
	if (ShapeActor)
	{
		ShapeActor->SetShapeActorHiddenInGameRecursively(bHidden);
	}
}

void FNoneModelVeneeredSudeBoard::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FVeneeredSudeBoardBase::ParseFromJson(InJsonObject);
	if (InJsonObject->HasTypedField<EJson::Boolean>(TEXT("toUse")))
	{
		bIsToUse = InJsonObject->GetBoolField(TEXT("toUse"));
	}

}

void FNoneModelVeneeredSudeBoard::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FVeneeredSudeBoardBase::ParseAttributesFromJson(InJsonObject);
	if (InJsonObject->HasTypedField<EJson::Boolean>(TEXT("toUse")))
	{
		bIsToUse = InJsonObject->GetBoolField(TEXT("toUse"));
	}
}

void FNoneModelVeneeredSudeBoard::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FVeneeredSudeBoardBase::ParseShapeFromJson(InJsonObject);
}

void FNoneModelVeneeredSudeBoard::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FVeneeredSudeBoardBase::ParseContentFromJson(InJsonObject);
}

void FNoneModelVeneeredSudeBoard::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FVeneeredSudeBoardBase::SaveToJson(JsonWriter);	
	JsonWriter->WriteValue(TEXT("toUse"),bIsToUse);	
}

void FNoneModelVeneeredSudeBoard::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FVeneeredSudeBoardBase::SaveAttriToJson(JsonWriter);
	JsonWriter->WriteValue(TEXT("toUse"), bIsToUse);
}

void FNoneModelVeneeredSudeBoard::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FVeneeredSudeBoardBase::SaveShapeToJson(JsonWriter);
}

void FNoneModelVeneeredSudeBoard::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FVeneeredSudeBoardBase::SaveContentToJson(JsonWriter);
}

void FNoneModelVeneeredSudeBoard::CopyTo(FSCTShape * OutShape)
{
	FVeneeredSudeBoardBase::CopyTo(OutShape);
	FNoneModelVeneeredSudeBoard * Shape = StaticCast<FNoneModelVeneeredSudeBoard*>(OutShape);
	if (BoardShape.IsValid())
	{
		Shape->BoardShape = MakeShareable(new FBoardShape);
		BoardShape->CopyTo(Shape->BoardShape.Get());
		Shape->AddChildShape(Shape->BoardShape);
		Shape->BoardShape->SetParentShape(Shape);
	}
}

ASCTShapeActor * FNoneModelVeneeredSudeBoard::SpawnShapeActor()
{
	ASCTShapeActor * Actor = FVeneeredSudeBoardBase::SpawnShapeActor();
	if (Actor)
	{
		UpdateVeneeredSudeBoard();
	}
	return Actor;
}

void FNoneModelVeneeredSudeBoard::SpawnActorsForSelected(FName InSelectProfileName)
{
	if (BoardShape.IsValid())
	{
		BoardShape->SpawnActorsForSelected(InSelectProfileName);
	}
}

void FNoneModelVeneeredSudeBoard::SetCollisionProfileName(FName InProfileName)
{
	if (BoardShape.IsValid())
	{
		BoardShape->SetCollisionProfileName(InProfileName);
	}
}

void FNoneModelVeneeredSudeBoard::GetResourceUrls(TArray<FString>& OutResourceUrls)
{
	FVeneeredSudeBoardBase::GetResourceUrls(OutResourceUrls);
}

void FNoneModelVeneeredSudeBoard::GetFileCachePaths(TArray<FString>& OutFileCachePaths)
{
	FVeneeredSudeBoardBase::GetFileCachePaths(OutFileCachePaths);
}

void FNoneModelVeneeredSudeBoard::UpdateVeneeredSudeBoard()
{
	do
	{
		if (BoardShape.IsValid() == false)
		{
			CreateBoardShape();		
		}
		if (GetMaterial().Url.IsEmpty() || GetMaterial().ID == -1) break;
		FPakMetaData PakMetaData;
		PakMetaData.ID = Material.ID;
		PakMetaData.MD5 = Material.MD5;
		PakMetaData.Name = Material.Name;
		PakMetaData.OptimizeParam = Material.OptimizeParam;
		PakMetaData.ThumbnailUrl = Material.ThumbnailUrl;
		PakMetaData.Url = Material.Url;
		BoardShape->SetMaterialData(PakMetaData);
		BoardShape->SetShapeRotY(90.0f);
		BoardShape->SetShapePosX(Position == EPosition::E_Left ? 0.0f : GetShapeWidth());
		BoardShape->SetShapeHeight(GetShapeWidth());
		BoardShape->SetShapeDepth(GetShapeDepth());
		BoardShape->SetShapeWidth(GetShapeHeight());
		if (BoardShape->GetShapeActor() == nullptr && ShapeActor && bToSpawnActor)
		{
			AActor * BoardActor = BoardShape->SpawnShapeActor();
			BoardActor->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
		}
		if (!bToSpawnActor  && BoardShape->GetShapeActor())
		{
			BoardShape->DestroyShapeActor();
		}

	} while(false);	
}

void FNoneModelVeneeredSudeBoard::CreateBoardShape()
{	
	do
	{
		if (BoardShape.IsValid()) break ;
		BoardShape = MakeShareable(new FBoardShape);
		BoardShape->SetShapeName(TEXT("见光板"));
		BoardShape->SetParentShape(this);
		AddChildShape(BoardShape);
		BoardShape->SetBoardShapeType(BoST_Rectangle);		
		BoardShape->SetShapeWidth(100.0f);
		BoardShape->SetShapeHeight(100.0f);
		BoardShape->SetShapeDepth(100.0f);		
	} while (false);	
}

void FVeneerdSudeBoardGroup::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{

	FSCTShape::ParseFromJson(InJsonObject);
	if (InJsonObject->HasTypedField<EJson::Object>(TEXT("defaultVeneerdSudeBoardSetting")))
	{
		const TSharedPtr<FJsonObject> & DefaultVeneerdSudeBoardJsonObjRef = InJsonObject->GetObjectField(TEXT("defaultVeneerdSudeBoardSetting"));
		// 材质
		if (DefaultVeneerdSudeBoardJsonObjRef->HasTypedField<EJson::Object>(TEXT("material")))
		{
			const TSharedPtr<FJsonObject> & MaterialObjRef = DefaultVeneerdSudeBoardJsonObjRef->GetObjectField(TEXT("material"));
			VeneerdSudeBoardDefaultInfo.Material.ID = MaterialObjRef->GetIntegerField(TEXT("id"));
			VeneerdSudeBoardDefaultInfo.Material.Name = MaterialObjRef->GetStringField(TEXT("name"));
			VeneerdSudeBoardDefaultInfo.Material.Url = MaterialObjRef->GetStringField(TEXT("pakUrl"));
			VeneerdSudeBoardDefaultInfo.Material.MD5 = MaterialObjRef->GetStringField(TEXT("pakMd5"));
			VeneerdSudeBoardDefaultInfo.Material.ThumbnailUrl = MaterialObjRef->GetStringField(TEXT("thumbnailUrl"));
			VeneerdSudeBoardDefaultInfo.Material.OptimizeParam = MaterialObjRef->GetStringField(TEXT("optimizeParam"));
		}
		// 封边
		if (DefaultVeneerdSudeBoardJsonObjRef->HasField(TEXT("edgeBanding")))
		{
			const TSharedPtr<FJsonObject> & TempEdgeBandingObj = DefaultVeneerdSudeBoardJsonObjRef->GetObjectField(TEXT("edgeBanding"));
			VeneerdSudeBoardDefaultInfo.EdgeBanding.ID = TempEdgeBandingObj->GetIntegerField(TEXT("id"));
			VeneerdSudeBoardDefaultInfo.EdgeBanding.Name = TempEdgeBandingObj->GetStringField(TEXT("name"));
			VeneerdSudeBoardDefaultInfo.EdgeBanding.Url = TempEdgeBandingObj->GetStringField(TEXT("pakUrl"));
			VeneerdSudeBoardDefaultInfo.EdgeBanding.MD5 = TempEdgeBandingObj->GetStringField(TEXT("pakMd5"));
			VeneerdSudeBoardDefaultInfo.EdgeBanding.ThumbnailUrl = TempEdgeBandingObj->GetStringField(TEXT("thumbnailUrl"));
		}

		// 基材类型
		VeneerdSudeBoardDefaultInfo.SubstrateType = DefaultVeneerdSudeBoardJsonObjRef->GetIntegerField(TEXT("substrateType"));
		VeneerdSudeBoardDefaultInfo.substrateHeight = DefaultVeneerdSudeBoardJsonObjRef->GetNumberField(TEXT("substrateHeight"));
		// 基材
		if (DefaultVeneerdSudeBoardJsonObjRef->HasField(TEXT("substrate")))
		{
			const TSharedPtr<FJsonObject> & TempSubstrateObj = DefaultVeneerdSudeBoardJsonObjRef->GetObjectField(TEXT("substrate"));
			VeneerdSudeBoardDefaultInfo.SubstrateID = TempSubstrateObj->GetIntegerField(TEXT("id"));
			VeneerdSudeBoardDefaultInfo.SubstrateName = TempSubstrateObj->GetStringField(TEXT("name"));
		}

	}
	check(InJsonObject->HasTypedField<EJson::Array>(TEXT("children")));
	const TArray<TSharedPtr<FJsonValue>> & veneerdSudeBoardGroup = InJsonObject->GetArrayField(TEXT("children"));
	for (const auto & Ref : veneerdSudeBoardGroup)
	{
		const TSharedPtr<FJsonObject> & ObjRef = Ref->AsObject();
		EShapeType Type = StaticCast<EShapeType>(ObjRef->GetIntegerField(TEXT("type")));
		int32 ShapeId = ObjRef->GetIntegerField(TEXT("id"));
		switch (Type)
		{
			case ST_NoneModelVeneeredBoard:
			{
				TSharedPtr<FNoneModelVeneeredSudeBoard> Shape = MakeShareable(new FNoneModelVeneeredSudeBoard);
				Shape->ParseAttributesFromJson(ObjRef);
				AddChildShape(Shape);
				Shape->SetParentShape(this);
				VeneeredSudeBoards.Emplace(Shape);
			}break;
			default:
				check(false)
					break;
		}

	}

}

void FVeneerdSudeBoardGroup::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseAttributesFromJson(InJsonObject);
	if (InJsonObject->HasTypedField<EJson::Object>(TEXT("defaultVeneerdSudeBoardSetting")))
	{
		const TSharedPtr<FJsonObject> & DefaultVeneerdSudeBoardJsonObjRef = InJsonObject->GetObjectField(TEXT("defaultVeneerdSudeBoardSetting"));
		// 材质
		if (DefaultVeneerdSudeBoardJsonObjRef->HasTypedField<EJson::Object>(TEXT("material")))
		{
			const TSharedPtr<FJsonObject> & MaterialObjRef = DefaultVeneerdSudeBoardJsonObjRef->GetObjectField(TEXT("material"));
			VeneerdSudeBoardDefaultInfo.Material.ID = MaterialObjRef->GetIntegerField(TEXT("id"));
			VeneerdSudeBoardDefaultInfo.Material.Name = MaterialObjRef->GetStringField(TEXT("name"));
			VeneerdSudeBoardDefaultInfo.Material.Url = MaterialObjRef->GetStringField(TEXT("pakUrl"));
			VeneerdSudeBoardDefaultInfo.Material.MD5 = MaterialObjRef->GetStringField(TEXT("pakMd5"));
			VeneerdSudeBoardDefaultInfo.Material.ThumbnailUrl = MaterialObjRef->GetStringField(TEXT("thumbnailUrl"));
			VeneerdSudeBoardDefaultInfo.Material.OptimizeParam = MaterialObjRef->GetStringField(TEXT("optimizeParam"));
		}		
		// 封边
		if (DefaultVeneerdSudeBoardJsonObjRef->HasField(TEXT("edgeBanding")))
		{
			const TSharedPtr<FJsonObject> & TempEdgeBandingObj = DefaultVeneerdSudeBoardJsonObjRef->GetObjectField(TEXT("edgeBanding"));			
			VeneerdSudeBoardDefaultInfo.EdgeBanding.ID = TempEdgeBandingObj->GetIntegerField(TEXT("id"));
			VeneerdSudeBoardDefaultInfo.EdgeBanding.Name = TempEdgeBandingObj->GetStringField(TEXT("name"));
			VeneerdSudeBoardDefaultInfo.EdgeBanding.Url = TempEdgeBandingObj->GetStringField(TEXT("pakUrl"));
			VeneerdSudeBoardDefaultInfo.EdgeBanding.MD5 = TempEdgeBandingObj->GetStringField(TEXT("pakMd5"));
			VeneerdSudeBoardDefaultInfo.EdgeBanding.ThumbnailUrl = TempEdgeBandingObj->GetStringField(TEXT("thumbnailUrl"));
		}

		// 基材类型
		VeneerdSudeBoardDefaultInfo.SubstrateType = DefaultVeneerdSudeBoardJsonObjRef->GetIntegerField(TEXT("substrateType"));
		VeneerdSudeBoardDefaultInfo.substrateHeight = DefaultVeneerdSudeBoardJsonObjRef->GetNumberField(TEXT("substrateHeight"));
		// 基材
		if (DefaultVeneerdSudeBoardJsonObjRef->HasField(TEXT("substrate")))
		{
			const TSharedPtr<FJsonObject> & TempSubstrateObj = DefaultVeneerdSudeBoardJsonObjRef->GetObjectField(TEXT("substrate"));
			VeneerdSudeBoardDefaultInfo.SubstrateID = TempSubstrateObj->GetIntegerField(TEXT("id"));
			VeneerdSudeBoardDefaultInfo.SubstrateName = TempSubstrateObj->GetStringField(TEXT("name"));
		}

	}

	check(InJsonObject->HasTypedField<EJson::Array>(TEXT("children")));
	const TArray<TSharedPtr<FJsonValue>> & veneerdSudeBoardGroup = InJsonObject->GetArrayField(TEXT("children"));
	for (const auto & Ref : veneerdSudeBoardGroup)
	{
		const TSharedPtr<FJsonObject> & ObjRef = Ref->AsObject();
		EShapeType Type = StaticCast<EShapeType>(ObjRef->GetIntegerField(TEXT("type")));
		int32 ShapeId = ObjRef->GetIntegerField(TEXT("id"));
		switch (Type)
		{
			case ST_NoneModelVeneeredBoard:
			{
				TSharedPtr<FNoneModelVeneeredSudeBoard> Shape = MakeShareable(new FNoneModelVeneeredSudeBoard);
				Shape->ParseAttributesFromJson(ObjRef);
				AddChildShape(Shape);
				Shape->SetParentShape(this);
				VeneeredSudeBoards.Emplace(Shape);
			}break;
			default:
				check(false)
					break;
		}

	}

}

void FVeneerdSudeBoardGroup::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseShapeFromJson(InJsonObject);
}

void FVeneerdSudeBoardGroup::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseContentFromJson(InJsonObject);
}

void FVeneerdSudeBoardGroup::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveToJson(JsonWriter);
	JsonWriter->WriteObjectStart(TEXT("defaultVeneerdSudeBoardSetting"));
	{
		{
			JsonWriter->WriteObjectStart(TEXT("edgeBanding"));
			JsonWriter->WriteValue(TEXT("id"), VeneerdSudeBoardDefaultInfo.EdgeBanding.ID);
			JsonWriter->WriteValue(TEXT("name"), VeneerdSudeBoardDefaultInfo.EdgeBanding.Name);
			JsonWriter->WriteValue(TEXT("pakUrl"), VeneerdSudeBoardDefaultInfo.EdgeBanding.Url);
			JsonWriter->WriteValue(TEXT("pakMd5"), VeneerdSudeBoardDefaultInfo.EdgeBanding.MD5);
			JsonWriter->WriteValue(TEXT("thumbnailUrl"), VeneerdSudeBoardDefaultInfo.EdgeBanding.ThumbnailUrl);
			JsonWriter->WriteObjectEnd();

		}
		// 材质
		{
			JsonWriter->WriteObjectStart(TEXT("material"));
			JsonWriter->WriteValue(TEXT("id"), VeneerdSudeBoardDefaultInfo.Material.ID);
			JsonWriter->WriteValue(TEXT("name"), VeneerdSudeBoardDefaultInfo.Material.Name);
			JsonWriter->WriteValue(TEXT("pakUrl"), VeneerdSudeBoardDefaultInfo.Material.Url);
			JsonWriter->WriteValue(TEXT("pakMd5"), VeneerdSudeBoardDefaultInfo.Material.MD5);
			JsonWriter->WriteValue(TEXT("thumbnailUrl"), VeneerdSudeBoardDefaultInfo.Material.ThumbnailUrl);
			JsonWriter->WriteValue(TEXT("OptimizeParam"), VeneerdSudeBoardDefaultInfo.Material.OptimizeParam);
			JsonWriter->WriteObjectEnd();
		}

		// 基材类型	
		JsonWriter->WriteValue(TEXT("substrateType"), VeneerdSudeBoardDefaultInfo.SubstrateType);
		JsonWriter->WriteValue(TEXT("substrateHeight"), VeneerdSudeBoardDefaultInfo.substrateHeight);
		// 基材	
		{
			JsonWriter->WriteObjectStart(TEXT("substrate"));
			JsonWriter->WriteValue(TEXT("id"), VeneerdSudeBoardDefaultInfo.SubstrateID);
			JsonWriter->WriteValue(TEXT("name"), VeneerdSudeBoardDefaultInfo.SubstrateName);
			JsonWriter->WriteObjectEnd();
		}
	}
	JsonWriter->WriteObjectEnd();
	JsonWriter->WriteArrayStart(TEXT("children"));
	for (const auto & Ref : VeneeredSudeBoards)
	{
		JsonWriter->WriteObjectStart();
		Ref->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteArrayEnd();
	}
	JsonWriter->WriteArrayEnd();
}

void FVeneerdSudeBoardGroup::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveAttriToJson(JsonWriter);

	JsonWriter->WriteObjectStart(TEXT("defaultVeneerdSudeBoardSetting"));
	{
		{
			JsonWriter->WriteObjectStart(TEXT("edgeBanding"));
			JsonWriter->WriteValue(TEXT("id"), VeneerdSudeBoardDefaultInfo.EdgeBanding.ID);
			JsonWriter->WriteValue(TEXT("name"), VeneerdSudeBoardDefaultInfo.EdgeBanding.Name);
			JsonWriter->WriteValue(TEXT("pakUrl"), VeneerdSudeBoardDefaultInfo.EdgeBanding.Url);
			JsonWriter->WriteValue(TEXT("pakMd5"), VeneerdSudeBoardDefaultInfo.EdgeBanding.MD5);
			JsonWriter->WriteValue(TEXT("thumbnailUrl"), VeneerdSudeBoardDefaultInfo.EdgeBanding.ThumbnailUrl);
			JsonWriter->WriteObjectEnd();

		}
		// 材质
		{
			JsonWriter->WriteObjectStart(TEXT("material"));
			JsonWriter->WriteValue(TEXT("id"), VeneerdSudeBoardDefaultInfo.Material.ID);
			JsonWriter->WriteValue(TEXT("name"), VeneerdSudeBoardDefaultInfo.Material.Name);
			JsonWriter->WriteValue(TEXT("pakUrl"), VeneerdSudeBoardDefaultInfo.Material.Url);
			JsonWriter->WriteValue(TEXT("pakMd5"), VeneerdSudeBoardDefaultInfo.Material.MD5);
			JsonWriter->WriteValue(TEXT("thumbnailUrl"), VeneerdSudeBoardDefaultInfo.Material.ThumbnailUrl);
			JsonWriter->WriteValue(TEXT("OptimizeParam"), VeneerdSudeBoardDefaultInfo.Material.OptimizeParam);
			JsonWriter->WriteObjectEnd();
		}

		// 基材类型	
		JsonWriter->WriteValue(TEXT("substrateType"), VeneerdSudeBoardDefaultInfo.SubstrateType);		
		JsonWriter->WriteValue(TEXT("substrateHeight"), VeneerdSudeBoardDefaultInfo.substrateHeight);
		// 基材	
		{
			JsonWriter->WriteObjectStart(TEXT("substrate"));
			JsonWriter->WriteValue(TEXT("id"), VeneerdSudeBoardDefaultInfo.SubstrateID);
			JsonWriter->WriteValue(TEXT("name"), VeneerdSudeBoardDefaultInfo.SubstrateName);
			JsonWriter->WriteObjectEnd();
		}
	}
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteArrayStart(TEXT("children"));
	for (const auto & Ref : VeneeredSudeBoards)
	{
		JsonWriter->WriteObjectStart();
		Ref->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
}

void FVeneerdSudeBoardGroup::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveShapeToJson(JsonWriter);
}

void FVeneerdSudeBoardGroup::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveContentToJson(JsonWriter);
}

void FVeneerdSudeBoardGroup::CopyTo(FSCTShape * OutShape)
{
	FSCTShape::CopyTo(OutShape);
	FVeneerdSudeBoardGroup * Shape = StaticCast<FVeneerdSudeBoardGroup*>(OutShape);
	Shape->VeneerdSudeBoardDefaultInfo = VeneerdSudeBoardDefaultInfo;
	Shape->VeneeredSudeBoards.Empty();
	for (const auto & Ref : VeneeredSudeBoards)
	{
	  TSharedPtr<FVeneeredSudeBoardBase> NewShape = StaticCastSharedPtr<FVeneeredSudeBoardBase>(FSCTShapeManager::Get()->CopyShapeToNew(Ref));
	  Shape->AddChildShape(NewShape);
	  NewShape->SetParentShape(Shape);
	  Shape->VeneeredSudeBoards.Emplace(NewShape);
	}

}

ASCTShapeActor * FVeneerdSudeBoardGroup::SpawnShapeActor()
{
	ASCTShapeActor * GroupActor = FSCTShape::SpawnShapeActor();
	for (const auto & Ref : VeneeredSudeBoards)
	{
		ASCTShapeActor * Actor = Ref->SpawnShapeActor();
		Actor->AttachToActor(GroupActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	return GroupActor;
}

void FVeneerdSudeBoardGroup::SpawnActorsForSelected(FName InSelectProfileName)
{
	for (const auto & it : VeneeredSudeBoards)
	{		
		it->SpawnActorsForSelected(InSelectProfileName);
	}
}

void FVeneerdSudeBoardGroup::SetCollisionProfileName(FName InProfileName)
{
	for (const auto & Ref : VeneeredSudeBoards)
	{
		Ref->SetCollisionProfileName(InProfileName);
	}
}

void FVeneerdSudeBoardGroup::GetResourceUrls(TArray<FString>& OutResourceUrls)
{
	for (const auto & Ref : VeneeredSudeBoards)
	{
		Ref->GetResourceUrls(OutResourceUrls);
	}
}

void FVeneerdSudeBoardGroup::GetFileCachePaths(TArray<FString>& OutFileCachePaths)
{
	for (const auto & Ref : VeneeredSudeBoards)
	{
		Ref->GetFileCachePaths(OutFileCachePaths);
	}
}

void FVeneerdSudeBoardGroup::HiddenVeneerdGroup(bool bHidden)
{
	for (auto & Ref : VeneeredSudeBoards)
	{
		Ref->HiddenVeneeredSudeBoard(bHidden);
	}

}

TSharedPtr<FVeneeredSudeBoardBase> FVeneerdSudeBoardGroup::GetSpecialPositionBoard(const FVeneeredSudeBoardBase::EPosition InPosition)
{
	TSharedPtr<FVeneeredSudeBoardBase> RetResult = nullptr;
	for (const auto & Ref : VeneeredSudeBoards)
	{
		if (Ref->GetPosition() == InPosition)
		{
			RetResult = Ref;
			break;
		}
	}
	return 	 RetResult;
}

void FVeneerdSudeBoardGroup::AddVeneeredSudeBoard(TSharedPtr<FVeneeredSudeBoardBase> InBoard)
{
	for (const auto & Ref : VeneeredSudeBoards)
	{
		check(Ref->GetPosition() != InBoard->GetPosition());
	}
	VeneeredSudeBoards.Emplace(InBoard);
	InBoard->SetParentShape(this);
	AddChildShape(InBoard);
}

void FVeneerdSudeBoardGroup::DeleteVeneeredSudeBoard(FVeneeredSudeBoardBase * InBoard)
{
	for (const auto Ref : VeneeredSudeBoards)
	{
		if (Ref.Get() == InBoard)
		{
			VeneeredSudeBoards.Remove(Ref);
			RemoveChildShape(Ref);
			break;
		}
	}
	
}

bool FVeneerdSudeBoardGroup::SetSpecialBoardActorValid(const FVeneeredSudeBoardBase::EPosition InPosition, const bool bInValid)
{
	bool bRetResult = false;
	do
	{
		TSharedPtr<FVeneeredSudeBoardBase> DesBoard;
		for (const auto & Ref : VeneeredSudeBoards)
		{
			if (Ref->GetPosition() == InPosition)
			{
				DesBoard = Ref;
				break;
			}
		}
		bRetResult = DesBoard.IsValid();
		if(!bRetResult) break;
		DesBoard->SetSpawnActorValid(bInValid);
		DesBoard->UpdateVeneeredSudeBoard();
		bRetResult = true;
	} while (false);
	return bRetResult;
}

bool FVeneerdSudeBoardGroup::GetSpecialBoardActorValid(const FVeneeredSudeBoardBase::EPosition InPosition)
{
	bool bRetResult = false;
	do
	{
		TSharedPtr<FVeneeredSudeBoardBase> DesBoard;
		for (const auto & Ref : VeneeredSudeBoards)
		{
			if (Ref->GetPosition() == InPosition)
			{
				DesBoard = Ref;
				break;
			}
		}
		bRetResult = DesBoard.IsValid();
		if (!bRetResult) break;		
		bRetResult = DesBoard->GetSpawnActorVallid();
	} while (false);
	return bRetResult;
}

void FVeneerdSudeBoardGroup::UpdateGroup()
{
	if (GetShapeActor() == nullptr) return;
	const float Px = GetShapePosX();
	const float Py = GetShapePosY();
	const float Pz = GetShapePosZ();
	const float W = GetShapeWidth();
	const float H = GetShapeHeight();
	const float D = GetShapeDepth();
	for (const auto & Ref : VeneeredSudeBoards)
	{		
		switch (Ref->GetPosition())
		{
			case FVeneeredSudeBoardBase::EPosition::E_Left:
			{
				Ref->SetShapePosX(0.0f);
				Ref->SetShapePosY(0.0f);
				Ref->SetShapePosZ(0.0f);
				Ref->SetShapeHeight(H);	
				Ref->SetShapeDepth(D);
			}break;
			case FVeneeredSudeBoardBase::EPosition::E_Right:
			{
				Ref->SetShapePosX(GetShapeWidth());
				Ref->SetShapePosY(0.0f);
				Ref->SetShapePosZ(0.0f);
				Ref->SetShapeHeight(H);
				Ref->SetShapeDepth(D);
			}break;
			default:
			{
				check(false);
			}break;
		}
		if (Ref->GetShapeActor() == nullptr)
		{
			ASCTShapeActor * Actor = Ref->SpawnShapeActor();
			Actor->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
		}
		Ref->UpdateVeneeredSudeBoard();
	}

}
