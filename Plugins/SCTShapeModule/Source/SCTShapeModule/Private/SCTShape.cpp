#include "SCTShape.h"
#include "SCTAttribute.h"
#include "SCTModelShape.h"
#include "SCTBoardShape.h"
#include "SCTShapeActor.h"
#include "SCTShapeBoxActor.h"
#include "SCTCompActor.h"
#include "SCTShapeManager.h"
#include "JsonObject.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "SharedPointer.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/CollisionProfile.h"
#include "Any.h"
#include "SCTShapeData.h"
#include "MaterialManager.h"
#include "Materials/MaterialInstanceDynamic.h"

FSCTShape::FSCTShape()
: ShapeType(ST_None)
, ShapeId(0)
, ShapeCategory(0)
, ShapeActor(nullptr)
, WireFrameActor(nullptr)
, BoundingBoxActor(nullptr)
, ParentShape(nullptr)
{
	ProductStatus = PS_Draft;
	bJsonFileParsed = false;

	ShowCondition = MakeShareable(new FBoolAssignAttri(this));
	Width = MakeShareable(new FNumberRangeAttri(this));
	Depth = MakeShareable(new FNumberRangeAttri(this));
	Height = MakeShareable(new FNumberRangeAttri(this));
	PositionX = MakeShareable(new FNumberRangeAttri(this));
	PositionY = MakeShareable(new FNumberRangeAttri(this));
	PositionZ = MakeShareable(new FNumberRangeAttri(this));
	RotationX = MakeShareable(new FNumberRangeAttri(this));
	RotationY = MakeShareable(new FNumberRangeAttri(this));
	RotationZ = MakeShareable(new FNumberRangeAttri(this));

	SetShapeWidth(100.0);
	SetShapeDepth(100.0);
	SetShapeHeight(100.0);
	SetShapePosX(0.0);
	SetShapePosY(0.0);
	SetShapePosZ(0.0);
	SetShapeRotX(0.0);
	SetShapeRotY(0.0);
	SetShapeRotZ(0.0);
}

FSCTShape::~FSCTShape()
{
	if (ShapeActor)
	{
		ShapeActor->DetachFromActorOverride(FDetachmentTransformRules::KeepRelativeTransform);
		ShapeActor->Destroy();
		ShapeActor = nullptr;
	}
	if (WireFrameActor)
	{
		WireFrameActor->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		WireFrameActor->Destroy();
		WireFrameActor = nullptr;
	}
	if (BoundingBoxActor)
	{
		BoundingBoxActor->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		BoundingBoxActor->Destroy();
		BoundingBoxActor = nullptr;
	}
	ChildrenShapes.Reset();
}

void FSCTShape::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//1、型录类型
	int32 ShapeType = InJsonObject->GetIntegerField(TEXT("type"));
	SetShapeType((EShapeType)ShapeType);
	int32 ShapeCategory = InJsonObject->GetIntegerField(TEXT("category"));
	SetShapeCategory(ShapeCategory);

	//2、型录基本信息：名称、缩略图、code、数据库ID
	SetShapeId(InJsonObject->GetIntegerField(TEXT("id")));
	SetShapeName(InJsonObject->GetStringField(TEXT("name")));
	SetThumbnailUrl(InJsonObject->GetStringField(TEXT("thumbnailUrl")));
	SetShapeCode(InJsonObject->GetStringField(TEXT("code")));
	SetSystemName(InJsonObject->GetStringField(TEXT("systemName")));

	//3、产品状态
	SetProductStatus((EProductStatus)InJsonObject->GetIntegerField(TEXT("status")));
	SetProductAbnormal(InJsonObject->GetBoolField(TEXT("abnormal")));

	//4、型录尺寸变量信息
	if (InJsonObject->HasField(TEXT("width")))
	{
		TSharedPtr<FJsonObject> WidthObj = InJsonObject->GetObjectField(TEXT("width"));
		Width = ParseAttributeFromJson(WidthObj, this);
	}
	if (InJsonObject->HasField(TEXT("depth")))
	{
		TSharedPtr<FJsonObject> DepthObj = InJsonObject->GetObjectField(TEXT("depth"));
		Depth = ParseAttributeFromJson(DepthObj, this);
	}
	if (InJsonObject->HasField(TEXT("height")))
	{
		TSharedPtr<FJsonObject> HeightObj = InJsonObject->GetObjectField(TEXT("height"));
		Height = ParseAttributeFromJson(HeightObj, this);
	}

	//5、型录位置信息
	if (InJsonObject->HasField(TEXT("posx")))
	{
		TSharedPtr<FJsonObject> PosXObj = InJsonObject->GetObjectField(TEXT("posx"));
		PositionX = ParseAttributeFromJson(PosXObj, this);
	}
	if (InJsonObject->HasField(TEXT("posy")))
	{
		TSharedPtr<FJsonObject> PosYObj = InJsonObject->GetObjectField(TEXT("posy"));
		PositionY = ParseAttributeFromJson(PosYObj, this);
	}
	if (InJsonObject->HasField(TEXT("posz")))
	{
		TSharedPtr<FJsonObject> PosZObj = InJsonObject->GetObjectField(TEXT("posz"));
		PositionZ = ParseAttributeFromJson(PosZObj, this);
	}

	//6、型录旋转信息
	if (InJsonObject->HasField(TEXT("rotx")))
	{
		TSharedPtr<FJsonObject> RotXObj = InJsonObject->GetObjectField(TEXT("rotx"));
		RotationX = ParseAttributeFromJson(RotXObj, this);
	}
	if (InJsonObject->HasField(TEXT("roty")))
	{
		TSharedPtr<FJsonObject> RotYObj = InJsonObject->GetObjectField(TEXT("roty"));
		RotationY = ParseAttributeFromJson(RotYObj, this);
	}
	if (InJsonObject->HasField(TEXT("rotz")))
	{
		TSharedPtr<FJsonObject> RotZObj = InJsonObject->GetObjectField(TEXT("rotz"));
		RotationZ = ParseAttributeFromJson(RotZObj, this);
	}


	if (InJsonObject->HasTypedField<EJson::Array>(TEXT("tagsPool")))
	{
		const TArray<TSharedPtr<FJsonValue>> & TagsPoolJsonValuesRef = InJsonObject->GetArrayField(TEXT("tagsPool"));
		for (const auto & Ref : TagsPoolJsonValuesRef)
		{
			const TSharedPtr<FJsonObject> & JsonObj = Ref->AsObject();
			FGuid TempGuid;
			FString GuidStr = JsonObj->GetStringField(TEXT("guid"));
			check(FGuid::Parse(GuidStr, TempGuid));
			FTag & TagRef = TagPools.Add(TempGuid);
			TagRef.Guid = TempGuid;
			TagRef.Name = JsonObj->GetStringField(TEXT("name"));
			TagRef.Description = JsonObj->GetStringField(TEXT("description"));
			if (JsonObj->HasTypedField<EJson::String>("data"))
			{
				TagRef.Data = JsonObj->GetStringField(TEXT("data"));
			}
		}
	}

	//标识FileJson已经解析
	bJsonFileParsed = true;
}

#define FSCTSHAPE_JSON_NUMBER_OR_STRING_PARSE(FileName,AttriToAssign)\
	if (InJsonObject->HasTypedField<EJson::Number>(FileName))\
	{\
		AttriToAssign(InJsonObject->GetNumberField(FileName));\
	}\
	else\
	{\
		FString WidthStr = InJsonObject->GetStringField(FileName);\
		AttriToAssign(WidthStr);\
	}
void FSCTShape::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//型录类型和id
	int32 ShapeType = InJsonObject->GetNumberField(TEXT("type"));
	int64 ShapeId = InJsonObject->GetNumberField(TEXT("id"));

	//异常状态
	SetProductAbnormal(InJsonObject->GetBoolField(TEXT("abnormal")));

	// 获取GUID
	if (InJsonObject->HasField(TEXT("shapeGuid")))
	{
		const FString TempShapeGuidStr = InJsonObject->GetStringField(TEXT("shapeGuid"));
		FGuid TempShapeGuid;
		const bool bParseResult = FGuid::Parse(TempShapeGuidStr, TempShapeGuid);
		check(bParseResult);
		ModifyShapeGuid(TempShapeGuid);
	}

	//型录尺寸
	FSCTSHAPE_JSON_NUMBER_OR_STRING_PARSE(TEXT("width"), SetShapeWidth);
	FSCTSHAPE_JSON_NUMBER_OR_STRING_PARSE(TEXT("depth"), SetShapeDepth);
	FSCTSHAPE_JSON_NUMBER_OR_STRING_PARSE(TEXT("height"), SetShapeHeight);

	//型录位置
	FSCTSHAPE_JSON_NUMBER_OR_STRING_PARSE(TEXT("posx"), SetShapePosX);
	FSCTSHAPE_JSON_NUMBER_OR_STRING_PARSE(TEXT("posy"), SetShapePosY);
	FSCTSHAPE_JSON_NUMBER_OR_STRING_PARSE(TEXT("posz"), SetShapePosZ);

	//型录旋转
	FSCTSHAPE_JSON_NUMBER_OR_STRING_PARSE(TEXT("rotx"), SetShapeRotX);
	FSCTSHAPE_JSON_NUMBER_OR_STRING_PARSE(TEXT("roty"), SetShapeRotY);
	FSCTSHAPE_JSON_NUMBER_OR_STRING_PARSE(TEXT("rotz"), SetShapeRotZ);

	if (InJsonObject->HasTypedField<EJson::Array>(TEXT("tagsPool")))
	{
		const TArray<TSharedPtr<FJsonValue>> & TagsPoolJsonValuesRef = InJsonObject->GetArrayField(TEXT("tagsPool"));
		for (const auto & Ref : TagsPoolJsonValuesRef)
		{
			const TSharedPtr<FJsonObject> & JsonObj = Ref->AsObject();
			FGuid TempGuid;
			FString GuidStr = JsonObj->GetStringField(TEXT("guid"));
			check(FGuid::Parse(GuidStr, TempGuid));
			FTag & TagRef = TagPools.Add(TempGuid);
			TagRef.Guid = TempGuid;
			TagRef.Name = JsonObj->GetStringField(TEXT("name"));
			TagRef.Description = JsonObj->GetStringField(TEXT("description"));
			if (JsonObj->HasTypedField<EJson::String>("data"))
			{
				TagRef.Data = JsonObj->GetStringField(TEXT("data"));
			}
		}
	}
}
#undef FSCTSHAPE_JSON_NUMBER_OR_STRING_PARSE

void FSCTShape::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//1、型录类型
	int32 ShapeType = InJsonObject->GetIntegerField(TEXT("type"));
	SetShapeType((EShapeType)ShapeType);
	int32 ShapeCategory = InJsonObject->GetIntegerField(TEXT("category"));
	SetShapeCategory(ShapeCategory);

	//2、型录基本信息：名称、缩略图、code、数据库ID
	SetShapeId(InJsonObject->GetIntegerField(TEXT("id")));
	SetShapeName(InJsonObject->GetStringField(TEXT("name")));
	SetThumbnailUrl(InJsonObject->GetStringField(TEXT("thumbnailUrl")));
	SetShapeCode(InJsonObject->GetStringField(TEXT("code")));
	SetSystemName(InJsonObject->GetStringField(TEXT("systemName")));

	//3、产品状态
	SetProductStatus((EProductStatus)InJsonObject->GetIntegerField(TEXT("status")));

	//4、型录尺寸变量信息
	TSharedPtr<FJsonObject> WidthObj = InJsonObject->GetObjectField(TEXT("width"));
	Width = ParseAttributeFromJson(WidthObj, this);
	TSharedPtr<FJsonObject> DepthObj = InJsonObject->GetObjectField(TEXT("depth"));
	Depth = ParseAttributeFromJson(DepthObj, this);
	TSharedPtr<FJsonObject> HeightObj = InJsonObject->GetObjectField(TEXT("height"));
	Height = ParseAttributeFromJson(HeightObj, this);

	//5、型录位置信息
	TSharedPtr<FJsonObject> PosXObj = InJsonObject->GetObjectField(TEXT("posx"));
	PositionX = ParseAttributeFromJson(PosXObj, this);
	TSharedPtr<FJsonObject> PosYObj = InJsonObject->GetObjectField(TEXT("posy"));
	PositionY = ParseAttributeFromJson(PosYObj, this);
	TSharedPtr<FJsonObject> PosZObj = InJsonObject->GetObjectField(TEXT("posz"));
	PositionZ = ParseAttributeFromJson(PosZObj, this);

	//6、型录旋转信息
	TSharedPtr<FJsonObject> RotXObj = InJsonObject->GetObjectField(TEXT("rotx"));
	RotationX = ParseAttributeFromJson(RotXObj, this);
	TSharedPtr<FJsonObject> RotYObj = InJsonObject->GetObjectField(TEXT("roty"));
	RotationY = ParseAttributeFromJson(RotYObj, this);
	TSharedPtr<FJsonObject> RotZObj = InJsonObject->GetObjectField(TEXT("rotz"));
	RotationZ = ParseAttributeFromJson(RotZObj, this);

	//标识FileJson已经解析
	bJsonFileParsed = true;
}

void FSCTShape::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//0、类型
	int32 ShapeType = InJsonObject->GetIntegerField(TEXT("type"));
	SetShapeType((EShapeType)ShapeType);
	int32 ShapeCategory = InJsonObject->GetIntegerField(TEXT("category"));
	SetShapeCategory(ShapeCategory);
	//1、名称
	FString ShapeName = InJsonObject->GetStringField(TEXT("name"));
	SetShapeName(ShapeName);
	//3、Id
	int32 ShapeId = InJsonObject->GetIntegerField(TEXT("id"));
	SetShapeId(ShapeId);
	//4、编码
	FString ShapeCode = InJsonObject->GetStringField(TEXT("code"));
	SetShapeCode(ShapeCode);
	//5、缩略图URL
	FString ThumbnailUrl = InJsonObject->GetStringField(TEXT("thumbnailUrl"));
	SetThumbnailUrl(ThumbnailUrl);
	//6、Json文件URL
	FString JsonFileUrl = InJsonObject->GetStringField(TEXT("jsonUrl"));
	SetJsonFileUrl(JsonFileUrl);
	//7、修改时间
	FString UpdateTime = InJsonObject->GetStringField(TEXT("updateTimeStr"));
	SetUpdataTime(UpdateTime);
	//8、产品状态
	int32 ProductStutas = InJsonObject->GetIntegerField(TEXT("status"));
	SetProductStatus((EProductStatus)ProductStutas);
	//9、异常标识
	bool bAbnormal = InJsonObject->GetBoolField(TEXT("abnormal"));
	SetProductAbnormal(bAbnormal);
}

void FSCTShape::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
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
	Width->SaveToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteObjectStart(TEXT("depth"));
	Depth->SaveToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteObjectStart(TEXT("height"));
	Height->SaveToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();

	//6、型录位置信息
	JsonWriter->WriteObjectStart(TEXT("posx"));
	PositionX->SaveToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteObjectStart(TEXT("posy"));
	PositionY->SaveToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteObjectStart(TEXT("posz"));
	PositionZ->SaveToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();

	//7、型录位置信息
	JsonWriter->WriteObjectStart(TEXT("rotx"));
	RotationX->SaveToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteObjectStart(TEXT("roty"));
	RotationY->SaveToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteObjectStart(TEXT("rotz"));
	RotationZ->SaveToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();


	if (TagPools.Num() > 0)
	{
		JsonWriter->WriteArrayStart(TEXT("tagsPool"));
		{
			for (const auto & Ref : TagPools)
			{
				JsonWriter->WriteObjectStart();
				{
					JsonWriter->WriteValue(TEXT("guid"), Ref.Value.Guid.ToString());
					JsonWriter->WriteValue(TEXT("name"), Ref.Value.Name);
					JsonWriter->WriteValue(TEXT("description"), Ref.Value.Description);
					JsonWriter->WriteValue(TEXT("data"), Ref.Value.Data);
				}
				JsonWriter->WriteObjectEnd();
			}

		}
		JsonWriter->WriteArrayEnd();
	}

}

void FSCTShape::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//1、型录类型
	JsonWriter->WriteValue(TEXT("type"), (int32)GetShapeType());
	//2、型录id
	JsonWriter->WriteValue(TEXT("id"), GetShapeId());
	//3、型录显示条件
	//JsonWriter->WriteValue(TEXT("showCondition"), ShowCondition->GetAttributeStr());

	//3、异常标识
	JsonWriter->WriteValue(TEXT("abnormal"), GetProductAbnormal());

	//4、型录尺寸
	JsonWriter->WriteValue(TEXT("width"), Width->GetAttributeStr());
	JsonWriter->WriteValue(TEXT("depth"), Depth->GetAttributeStr());
	JsonWriter->WriteValue(TEXT("height"), Height->GetAttributeStr());

	//5、型录位置
	JsonWriter->WriteValue(TEXT("posx"), PositionX->GetAttributeStr());
	JsonWriter->WriteValue(TEXT("posy"), PositionY->GetAttributeStr());
	JsonWriter->WriteValue(TEXT("posz"), PositionZ->GetAttributeStr());

	//6、型录旋转
	JsonWriter->WriteValue(TEXT("rotx"), RotationX->GetAttributeStr());
	JsonWriter->WriteValue(TEXT("roty"), RotationY->GetAttributeStr());
	JsonWriter->WriteValue(TEXT("rotz"), RotationZ->GetAttributeStr());

	// 存储GUID
	JsonWriter->WriteValue(TEXT("shapeGuid"),GetShapeGuid().ToString());	

	if (TagPools.Num() > 0)
	{
		JsonWriter->WriteArrayStart(TEXT("tagsPool"));
		{
			for (const auto & Ref : TagPools)
			{
				JsonWriter->WriteObjectStart();
				{
					JsonWriter->WriteValue(TEXT("guid"), Ref.Value.Guid.ToString());
					JsonWriter->WriteValue(TEXT("name"), Ref.Value.Name);
					JsonWriter->WriteValue(TEXT("description"), Ref.Value.Description);
					JsonWriter->WriteValue(TEXT("data"), Ref.Value.Data);
				}
				JsonWriter->WriteObjectEnd();
			}

		}
		JsonWriter->WriteArrayEnd();
	}

}

void FSCTShape::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
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
	JsonWriter->WriteValue(TEXT("abnormal"), GetProductAbnormal());

	//4、型录显示条件
	// 	JsonWriter->WriteObjectStart(TEXT("showCondition"));
	// 	ShowCondition->SaveToJson(JsonWriter);
	// 	JsonWriter->WriteObjectEnd();

	//5、型录尺寸信息
	JsonWriter->WriteObjectStart(TEXT("width"));
	Width->SaveToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteObjectStart(TEXT("depth"));
	Depth->SaveToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteObjectStart(TEXT("height"));
	Height->SaveToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();

	//6、型录位置信息
	JsonWriter->WriteObjectStart(TEXT("posx"));
	PositionX->SaveToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteObjectStart(TEXT("posy"));
	PositionY->SaveToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteObjectStart(TEXT("posz"));
	PositionZ->SaveToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();

	//7、型录位置信息
	JsonWriter->WriteObjectStart(TEXT("rotx"));
	RotationX->SaveToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteObjectStart(TEXT("roty"));
	RotationY->SaveToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteObjectStart(TEXT("rotz"));
	RotationZ->SaveToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();
}

void FSCTShape::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//1、类型
	JsonWriter->WriteValue(TEXT("type"), (int32)GetShapeType());
	JsonWriter->WriteValue(TEXT("category"), GetShapeCategory());
	//2、ID
	JsonWriter->WriteValue(TEXT("id"), GetShapeId());
	//3、名称
	JsonWriter->WriteValue(TEXT("name"), GetShapeName());
	//4、编码
	JsonWriter->WriteValue(TEXT("code"), GetShapeCode());
	//6、产品状态
	JsonWriter->WriteValue(TEXT("status"), (int32)GetProductStatus());
	//7、缩略图Url
	JsonWriter->WriteValue(TEXT("thumbnailUrl"), GetThumbnailUrl());
	//8、Json文件Url
	JsonWriter->WriteValue(TEXT("jsonUrl"), GetJsonFileUrl());

	//9、型录尺寸变化范围
	//Width
	float MinWidth = GetShapeWidth();
	float MaxWidth = MinWidth;
	if (Width->GetAttributeType() == SAT_NumberRange)
	{
		TSharedPtr<FNumberRangeAttri> RangeWidth = StaticCastSharedPtr<FNumberRangeAttri>(Width);
		MinWidth = RangeWidth->GetMinValue();
		MaxWidth = RangeWidth->GetMaxValue();
	}
	JsonWriter->WriteValue(TEXT("minWidth"), MinWidth);
	JsonWriter->WriteValue(TEXT("maxWidth"), MaxWidth);
	JsonWriter->WriteValue(TEXT("currentWidth"), GetShapeWidth());
	//Depth
	float MinDepth = GetShapeDepth();
	float MaxDepth = MinDepth;
	if (Depth->GetAttributeType() == SAT_NumberRange)
	{
		TSharedPtr<FNumberRangeAttri> RangeDepth = StaticCastSharedPtr<FNumberRangeAttri>(Depth);
		MinDepth = RangeDepth->GetMinValue();
		MaxDepth = RangeDepth->GetMaxValue();
	}
	JsonWriter->WriteValue(TEXT("minDepth"), MinDepth);
	JsonWriter->WriteValue(TEXT("maxDepth"), MaxDepth);
	JsonWriter->WriteValue(TEXT("currentDepth"), GetShapeDepth());
	//Height
	float MinHeight = GetShapeHeight();
	float MaxHeight = MinHeight;
	if (Height->GetAttributeType() == SAT_NumberRange)
	{
		TSharedPtr<FNumberRangeAttri> RangeHeight = StaticCastSharedPtr<FNumberRangeAttri>(Height);
		MinHeight = RangeHeight->GetMinValue();
		MaxHeight = RangeHeight->GetMaxValue();
	}
	JsonWriter->WriteValue(TEXT("minHeight"), MinHeight);
	JsonWriter->WriteValue(TEXT("maxHeight"), MaxHeight);
	JsonWriter->WriteValue(TEXT("currentHeight"), GetShapeHeight());
}

void FSCTShape::GetResourceUrls(TArray<FString> &OutResourceUrls)
{
	for (const auto& Child : ChildrenShapes)
		Child->GetResourceUrls(OutResourceUrls);
}

void FSCTShape::GetFileCachePaths(TArray<FString> &OutFileCachePaths)
{
	for (const auto& Child : ChildrenShapes)
		Child->GetFileCachePaths(OutFileCachePaths);
}

void FSCTShape::CheckResourceUrlsAndCachePaths(TArray<FString>& OutResourceUrls, TArray<FString>& OutFileCachePaths)
{
	check(OutResourceUrls.Num() == OutFileCachePaths.Num());
	// 查空 - 剔除空url
	TArray<FString> EmptyCheckCachePaths;
	TArray<FString> EmptyCheckUrls;
	for (int32 Index = 0; Index < OutResourceUrls.Num(); ++Index)
	{	
		if (OutResourceUrls[Index].IsEmpty() == false)
		{
			EmptyCheckUrls.Emplace(OutResourceUrls[Index]);
			EmptyCheckCachePaths.Emplace(OutFileCachePaths[Index]);
		}
	}

	class FindRepeatResourceIndex
	{
	public:
		FindRepeatResourceIndex(const TArray<FString>& InFileCachePaths) :CachePaths(InFileCachePaths) {}
	public:
		void operator()(const FString & InStr)
		{
			++Index;
			bool IsAlreadyExist = false;
			UniqueStrings.Add(InStr, &IsAlreadyExist);
			if (IsAlreadyExist == false)
			{
				int32 FindIndex = CachePaths.Contains(InStr);
				check(FindIndex != -1);
				SaveIndices.Add(Index);
			}
		}
	public:
		TArray<int32> SaveIndices;
		TSet<FString> UniqueStrings;
		int32 Index = -1;
		const TArray<FString> & CachePaths;
	};
	// 查重复
	FindRepeatResourceIndex FindRepeatResourceIndex(EmptyCheckCachePaths);
	for (const auto & Ref : EmptyCheckCachePaths)
	{
		FindRepeatResourceIndex(Ref);
	}
	TArray<FString> RetCheckCachePaths;
	TArray<FString> RetCheckUrls;
	for (const auto & Ref : FindRepeatResourceIndex.UniqueStrings)
	{
		RetCheckCachePaths.Add(Ref);
	}
	for (const auto & Ref : FindRepeatResourceIndex.SaveIndices)
	{
		RetCheckUrls.Add(EmptyCheckUrls[Ref]);
	}
	OutResourceUrls = RetCheckUrls;
	OutFileCachePaths = RetCheckCachePaths;
}

void FSCTShape::CopyTo(FSCTShape* OutShape)
{
	//型录类型
	OutShape->ShapeType = ShapeType;
	OutShape->ShapeCategory = ShapeCategory;
	//OutShape->ShapeCategory = ShapeCategory;
	//型录标识信息
	OutShape->ShapeId = ShapeId;
	OutShape->ShapeName = ShapeName;
	OutShape->ShapeCode = ShapeCode;
	OutShape->ThumbnailUrl = ThumbnailUrl;
	OutShape->SystemName = SystemName;
	//系统属性
	OutShape->ShowCondition = CopyAttributeToNew(ShowCondition, OutShape);
	OutShape->Width = CopyAttributeToNew(Width, OutShape);
	OutShape->Depth = CopyAttributeToNew(Depth, OutShape);
	OutShape->Height = CopyAttributeToNew(Height, OutShape);
	OutShape->PositionX = CopyAttributeToNew(PositionX, OutShape);
	OutShape->PositionY = CopyAttributeToNew(PositionY, OutShape);
	OutShape->PositionZ = CopyAttributeToNew(PositionZ, OutShape);
	OutShape->RotationX = CopyAttributeToNew(RotationX, OutShape);
	OutShape->RotationY = CopyAttributeToNew(RotationY, OutShape);
	OutShape->RotationZ = CopyAttributeToNew(RotationZ, OutShape);
	OutShape->ShapeCopyFromGuid = ShapeGuid;
	OutShape->TagPools = TagPools;
}

ASCTShapeActor* FSCTShape::SpawnShapeActor()
{
	//创建生成型录Actor
	UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();
	ASCTCompActor* NewShapeActor = World->SpawnActor<ASCTCompActor>(
		ASCTCompActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);

	//设置型录位置和旋转
	FVector ActorPos(GetShapePosX(), GetShapePosY(), GetShapePosZ());
	NewShapeActor->SetActorPosition(ActorPos);
	FRotator ActorRot(GetShapeRotY(), GetShapeRotZ(), GetShapeRotX());
	NewShapeActor->SetActorRotator(ActorRot);

	//设置型录可见性
	bool bParentVisible = NewShapeActor->GetVisibleStateFromParent();
	NewShapeActor->SetActorHiddenInGame(!GetShowCondition() || !bParentVisible);

	//设置关联的Shape
	NewShapeActor->SetShape(this);

	ShapeActor = NewShapeActor;
	return NewShapeActor;
}

void FSCTShape::DestroyShapeActor()
{
	if (WireFrameActor)
	{
		WireFrameActor->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		WireFrameActor->Destroy();
		WireFrameActor = nullptr;
	}
	if (BoundingBoxActor)
	{
		BoundingBoxActor->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		BoundingBoxActor->Destroy();
		BoundingBoxActor = nullptr;
	}
	if (ShapeActor)
	{
		ShapeActor->DetachFromActorOverride(FDetachmentTransformRules::KeepRelativeTransform);
		ShapeActor->Destroy();
		ShapeActor = nullptr;
	}
}

void FSCTShape::SetCollisionProfileName(FName InProfileName)
{

}

//空间属性信息操作
bool FSCTShape::SetShowCondition(const FString &InStrValue)
{
	return ShowCondition->SetAttributeValue(InStrValue);
}

bool FSCTShape::SetShowCondition(bool InbValue)
{
	FString StrValue = InbValue ? TEXT("1>0") : TEXT("1<0");
	return SetShowCondition(StrValue);
}

bool FSCTShape::GetShowCondition()
{
	if (!ShowCondition.IsValid())
	{
		return true;
	}
	TSharedPtr<FBoolAssignAttri> ShowCondAttri = StaticCastSharedPtr<FBoolAssignAttri>(ShowCondition);
	return ShowCondAttri->GetBoolValue();
}

const TSharedPtr<FShapeAttribute>& FSCTShape::GetShowCondAttri() const
{
	return ShowCondition;
}

void FSCTShape::ChangeShapeWidthAttriType(EShapeAttributeType InType)
{
	Width = MakeAttributeByType(InType, this);
}

bool FSCTShape::SetShapeWidth(const FString &InStrValue)
{
	bool bResult = true;
	if (Width->GetAttributeStr() != InStrValue)
	{
		bResult = Width->SetAttributeValue(InStrValue);
		//若Actor已生成，同时更新Actor
		if (bResult)
		{
			UpdateAssociatedActors(1);
		}
	}
	return bResult;
}

bool FSCTShape::SetShapeWidth(float InIntValue)
{
	return SetShapeWidth(FString::Printf(TEXT("%f"), InIntValue));
}

float FSCTShape::GetShapeWidth()
{
	check(Width.IsValid());
	if (Width->GetAttributeType() == SAT_NumberRange)
	{
		TSharedPtr<FNumberRangeAttri> WidthAttri = StaticCastSharedPtr<FNumberRangeAttri>(Width);
		return WidthAttri->GetNumberValue();
	}
	else if (Width->GetAttributeType() == SAT_NumberSelect)
	{
		TSharedPtr<FNumberSelectAttri> WidthAttri = StaticCastSharedPtr<FNumberSelectAttri>(Width);
		return WidthAttri->GetNumberValue();
	}
	else
	{
		TSharedPtr<FNumberAssignAttri> WidthAttri = StaticCastSharedPtr<FNumberAssignAttri>(Width);
		return WidthAttri->GetNumberValue();
	}
}

const TSharedPtr<FShapeAttribute>& FSCTShape::GetShapeWidthAttri() const
{
	return Width;
}

void FSCTShape::ChangeShapeDepthAttriType(EShapeAttributeType InType)
{
	Depth = MakeAttributeByType(InType, this);
}

bool FSCTShape::SetShapeDepth(const FString &InStrValue)
{
	bool bResult = true;
	if (Depth->GetAttributeStr() != InStrValue)
	{
		bResult = Depth->SetAttributeValue(InStrValue);
		//若Actor已生成，同时更新Actor
		if (bResult)
		{
			UpdateAssociatedActors(1);
		}
	}
	return bResult;
}

bool FSCTShape::SetShapeDepth(float InIntValue)
{
	return SetShapeDepth(FString::Printf(TEXT("%f"), InIntValue));
}

float FSCTShape::GetShapeDepth()
{
	check(Depth.IsValid());
	if (Depth->GetAttributeType() == SAT_NumberRange)
	{
		TSharedPtr<FNumberRangeAttri> DepthAttri = StaticCastSharedPtr<FNumberRangeAttri>(Depth);
		return DepthAttri->GetNumberValue();
	}
	else if (Depth->GetAttributeType() == SAT_NumberSelect)
	{
		TSharedPtr<FNumberSelectAttri> DepthAttri = StaticCastSharedPtr<FNumberSelectAttri>(Depth);
		return DepthAttri->GetNumberValue();
	}
	else
	{
		TSharedPtr<FNumberAssignAttri> DepthAttri = StaticCastSharedPtr<FNumberAssignAttri>(Depth);
		return DepthAttri->GetNumberValue();
	}
}

const TSharedPtr<FShapeAttribute>& FSCTShape::GetShapeDepthAttri() const
{
	return Depth;
}

void FSCTShape::ChangeShapeHeightAttriType(EShapeAttributeType InType)
{
	Height = MakeAttributeByType(InType, this);
}

bool FSCTShape::SetShapeHeight(const FString &InStrValue)
{
	bool bResult = true;
	if (Height->GetAttributeStr() != InStrValue)
	{
		bResult = Height->SetAttributeValue(InStrValue);
		//若Actor已生成，同时更新Actor
		if (bResult)
		{
			UpdateAssociatedActors(1);
		}
	}
	return bResult;
}

bool FSCTShape::SetShapeHeight(float InIntValue)
{
	return SetShapeHeight(FString::Printf(TEXT("%f"), InIntValue));
}

float FSCTShape::GetShapeHeight()
{
	check(Height.IsValid());
	if (Height->GetAttributeType() == SAT_NumberRange)
	{
		TSharedPtr<FNumberRangeAttri> HeightAttri = StaticCastSharedPtr<FNumberRangeAttri>(Height);
		return HeightAttri->GetNumberValue();
	}
	else if (Height->GetAttributeType() == SAT_NumberSelect)
	{
		TSharedPtr<FNumberSelectAttri> HeightAttri = StaticCastSharedPtr<FNumberSelectAttri>(Height);
		return HeightAttri->GetNumberValue();
	}
	else
	{
		TSharedPtr<FNumberAssignAttri> HeightAttri = StaticCastSharedPtr<FNumberAssignAttri>(Height);
		return HeightAttri->GetNumberValue();
	}
}

const TSharedPtr<FShapeAttribute>& FSCTShape::GetShapeHeightAttri() const
{
	return Height;
}

void FSCTShape::ChangeShapeposXAttriType(EShapeAttributeType InType)
{
	PositionX = MakeAttributeByType(InType, this);
}

bool FSCTShape::SetShapePosX(const FString &InStrValue)
{
	bool bResult = true;
	if (PositionX->GetAttributeStr() != InStrValue)
	{
		bResult = PositionX->SetAttributeValue(InStrValue);
		//若Actor已生成，同时更新Actor
		if (bResult)
		{
			UpdateAssociatedActors(2);
			UpdateChileShapes(2);
		}
	}
	return bResult;
}

bool FSCTShape::SetShapePosX(float InIntValue)
{
	return SetShapePosX(FString::Printf(TEXT("%f"), InIntValue));
}

float FSCTShape::GetShapePosX()
{
	check(PositionX.IsValid());
	TSharedPtr<FNumberRangeAttri> PosXAttri = StaticCastSharedPtr<FNumberRangeAttri>(PositionX);
	return PosXAttri->GetNumberValue();
}

const TSharedPtr<FShapeAttribute>& FSCTShape::GetShapePosXAttri() const
{
	return PositionX;
}

float FSCTShape::GetShapePosLeftSide()
{
	return GetShapePosX();
}

float FSCTShape::GetShapePosRightSide()
{
	return GetShapePosX() + GetShapeWidth();
}

bool FSCTShape::SetShapePosLeftSide(float InValue)
{
	return SetShapePosX(InValue);
}

bool FSCTShape::SetShapePosRightSide(float InValue)
{
	float ShapeLeft = GetShapePosX();
	if (InValue <= ShapeLeft)
	{
		return false;
	}
	return SetShapeWidth(InValue - ShapeLeft);
}

void FSCTShape::ChangeShapePosYAttriType(EShapeAttributeType InType)
{
	PositionY = MakeAttributeByType(InType, this);
}

bool FSCTShape::SetShapePosY(const FString &InStrValue)
{
	bool bResult = true;
	if (PositionY->GetAttributeStr() != InStrValue)
	{
		bResult = PositionY->SetAttributeValue(InStrValue);
		//若Actor已生成，同时更新Actor
		if (bResult)
		{
			UpdateAssociatedActors(2);
			UpdateChileShapes(2);
		}
	}
	return bResult;
}

bool FSCTShape::SetShapePosY(float InIntValue)
{
	return SetShapePosY(FString::Printf(TEXT("%f"), InIntValue));
}

float FSCTShape::GetShapePosY()
{
	check(PositionY.IsValid());
	TSharedPtr<FNumberRangeAttri> PosYAttri = StaticCastSharedPtr<FNumberRangeAttri>(PositionY);
	return PosYAttri->GetNumberValue();
}

const TSharedPtr<FShapeAttribute>& FSCTShape::GetShapePosYAttri() const
{
	return PositionY;
}

float FSCTShape::GetShapePosBackSide()
{
	return GetShapePosY();
}

float FSCTShape::GetShapePosFrontSide()
{
	return GetShapePosY() + GetShapeDepth();
}

bool FSCTShape::SetShapePosBackSide(float InValue)
{
	return SetShapePosY(InValue);
}

bool FSCTShape::SetShapePosFrontSide(float InValue)
{
	float ShapeBack = GetShapePosY();
	if (InValue <= ShapeBack)
	{
		return false;
	}
	return SetShapeDepth(InValue - ShapeBack);
}

void FSCTShape::ChangeShapePosZAttriType(EShapeAttributeType InType)
{
	PositionZ = MakeAttributeByType(InType, this);
}

bool FSCTShape::SetShapePosZ(const FString &InStrValue)
{
	bool bResult = true;
	if (PositionZ->GetAttributeStr() != InStrValue)
	{
		bResult = PositionZ->SetAttributeValue(InStrValue);
		//若Actor已生成，同时更新Actor
		if (bResult)
		{
			UpdateAssociatedActors(2);
			UpdateChileShapes(2);
		}
	}
	return bResult;
}

bool FSCTShape::SetShapePosZ(float InIntValue)
{
	return SetShapePosZ(FString::Printf(TEXT("%f"), InIntValue));
}

float FSCTShape::GetShapePosZ()
{
	check(PositionZ.IsValid());
	TSharedPtr<FNumberRangeAttri> PosZAttri = StaticCastSharedPtr<FNumberRangeAttri>(PositionZ);
	return PosZAttri->GetNumberValue();
}

const TSharedPtr<FShapeAttribute>& FSCTShape::GetShapePosZAttri() const
{
	return PositionZ;
}

float FSCTShape::GetShapePosBottomSide()
{
	return GetShapePosZ();
}

float FSCTShape::GetShapePosTopSide()
{
	return GetShapePosZ() + GetShapeHeight();
}

bool FSCTShape::SetShapePosBottomSide(float InValue)
{
	return SetShapePosZ(InValue);
}

bool FSCTShape::SetShapePosTopSide(float InValue)
{
	float ShapeBottom = GetShapePosZ();
	if (InValue <= ShapeBottom)
	{
		return false;
	}
	return SetShapeHeight(InValue - ShapeBottom);
}

void FSCTShape::ChangeShapeRotXAttriType(EShapeAttributeType InType)
{
	RotationX = MakeAttributeByType(InType, this);
}

bool FSCTShape::SetShapeRotX(const FString &InStrValue)
{
	bool bResult = RotationX->SetAttributeValue(InStrValue);
	//若Actor已生成，同时更新Actor
	if (bResult)
	{
		UpdateAssociatedActors(3);
		UpdateChileShapes(0);
	}
	return bResult;
}

bool FSCTShape::SetShapeRotX(float InIntValue)
{
	return SetShapeRotX(FString::Printf(TEXT("%f"), InIntValue));
}

float FSCTShape::GetShapeRotX()
{
	check(RotationX.IsValid());
	TSharedPtr<FNumberRangeAttri> RotZAttri = StaticCastSharedPtr<FNumberRangeAttri>(RotationX);
	return RotZAttri->GetNumberValue();
}

const TSharedPtr<FShapeAttribute>& FSCTShape::GetShapeRotXAttri() const
{
	return RotationX;
}

void FSCTShape::ChangeShapeRotYAttriType(EShapeAttributeType InType)
{
	RotationY = MakeAttributeByType(InType, this);
}

bool FSCTShape::SetShapeRotY(const FString &InStrValue)
{
	bool bResult = RotationY->SetAttributeValue(InStrValue);
	//若Actor已生成，同时更新Actor
	if (bResult)
	{
		UpdateAssociatedActors(3);
		UpdateChileShapes(0);
	}
	return bResult;
}

bool FSCTShape::SetShapeRotY(float InIntValue)
{
	return SetShapeRotY(FString::Printf(TEXT("%f"), InIntValue));
}

float FSCTShape::GetShapeRotY()
{
	check(RotationY.IsValid());
	TSharedPtr<FNumberRangeAttri> RotZAttri = StaticCastSharedPtr<FNumberRangeAttri>(RotationY);
	return RotZAttri->GetNumberValue();
}

const TSharedPtr<FShapeAttribute>& FSCTShape::GetShapeRotYAttri() const
{
	return RotationY;
}

void FSCTShape::ChangeShapeRotZAttriType(EShapeAttributeType InType)
{
	RotationZ = MakeAttributeByType(InType, this);
}

bool FSCTShape::SetShapeRotZ(const FString &InStrValue)
{
	bool bResult = RotationZ->SetAttributeValue(InStrValue);
	//若Actor已生成，同时更新Actor
	if (bResult)
	{
		UpdateAssociatedActors(3);
		UpdateChileShapes(0);
	}
	return bResult;
}

bool FSCTShape::SetShapeRotZ(float InIntValue)
{
	return SetShapeRotZ(FString::Printf(TEXT("%f"), InIntValue));
}

float FSCTShape::GetShapeRotZ()
{
	check(RotationZ.IsValid());
	TSharedPtr<FNumberRangeAttri> RotZAttri = StaticCastSharedPtr<FNumberRangeAttri>(RotationZ);
	return RotZAttri->GetNumberValue();
}

const TSharedPtr<FShapeAttribute>& FSCTShape::GetShapeRotZAttri() const
{
	return RotationZ;
}



bool FSCTShape::ModifyShapeDimentionDelta(const FVector &InDelta)
{
	bool bResult = true;
	if (FMath::Abs(InDelta.X) > KINDA_SMALL_NUMBER)
	{
		bResult = SetShapeWidth(GetShapeWidth() + InDelta.X * 10.0f);
		if (!bResult)
		{
			return false;
		}
	}
	if (FMath::Abs(InDelta.Y) > KINDA_SMALL_NUMBER)
	{
		bResult = SetShapeDepth(GetShapeDepth() + InDelta.Y * 10.0f);
		if (!bResult)
		{
			return false;
		}
	}
	if (FMath::Abs(InDelta.Z) > KINDA_SMALL_NUMBER)
	{
		bResult = SetShapeHeight(GetShapeHeight() + InDelta.Z * 10.0f);
		if (!bResult)
		{
			return false;
		}
	}
	return bResult;
}

bool FSCTShape::ModifyShapePositionDelta(const FVector &InDelta)
{
	bool bResult = true;
	if (FMath::Abs(InDelta.X) > KINDA_SMALL_NUMBER)
	{
		bResult = SetShapePosX(GetShapePosX() + InDelta.X * 10.0f);
		if (!bResult)
		{
			return false;
		}
	}
	if (FMath::Abs(InDelta.Y) > KINDA_SMALL_NUMBER)
	{
		bResult = SetShapePosY(GetShapePosY() + InDelta.Y * 10.0f);
		if (!bResult)
		{
			return false;
		}
	}
	if (FMath::Abs(InDelta.Z) > KINDA_SMALL_NUMBER)
	{
		bResult = SetShapePosZ(GetShapePosZ() + InDelta.Z * 10.0f);
		if (!bResult)
		{
			return false;
		}
	}
	return bResult;
}

//子级型录
int32 FSCTShape::GetChildShapeIndex(FSCTShape* CurShape)
{
	for (int32 i = 0; i < ChildrenShapes.Num(); ++i)
	{
		if (ChildrenShapes[i].Get() == CurShape)
		{
			return i;
		}
	}
	return -1;
}

TSharedPtr<FSCTShape> FSCTShape::GetChildShape(FSCTShape* CurShape)
{
	for (int32 i = 0; i < ChildrenShapes.Num(); ++i)
	{
		if (ChildrenShapes[i].Get() == CurShape)
		{
			return ChildrenShapes[i];
		}
	}
	return nullptr;
}

void FSCTShape::AddChildShape(const TSharedPtr<FSCTShape> &ChildShape)
{
	ChildrenShapes.Add(ChildShape);
}

void FSCTShape::InsertChildShape(FSCTShape* InCurShape, const TSharedPtr<FSCTShape> &ChildShape)
{
	if (InCurShape)
	{
		int32 IndexFind = GetChildShapeIndex(InCurShape);
		ChildrenShapes.Insert(ChildShape, IndexFind);
	}
	else
	{
		ChildrenShapes.Add(ChildShape);
	}
}

void FSCTShape::RemoveChildShape(const TSharedPtr<FSCTShape> &ChildShape)
{
	ChildrenShapes.Remove(ChildShape);
}

const TArray<TSharedPtr<FSCTShape>>& FSCTShape::GetChildrenShapes() const
{
	return ChildrenShapes;
}

//型录Actor
ASCTShapeActor* FSCTShape::GetShapeActor() const
{
	return ShapeActor;
}

ASCTBoundingBoxActor* FSCTShape::GetBoundingBoxActor() const
{
	return BoundingBoxActor;
}

ASCTWireframeActor* FSCTShape::GetWireframeActor() const
{
	return WireFrameActor;
}

//型录外包框
ASCTBoundingBoxActor* FSCTShape::SpawnBoundingBoxActor()
{
	BoundingBoxActor = GEngine->GetWorldContextFromGameViewport(
		GEngine->GameViewport)->World()->SpawnActor<ASCTBoundingBoxActor>(
			ASCTBoundingBoxActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	BoundingBoxActor->SetShape(this);
	UMaterialInterface* DefaultMaterial = FMaterialManagerInstatnce::GetIns().GetMaterialManagerPtr()->GetColorUnlitTMaterial();
	UMaterialInstanceDynamic* LineDynMaterial = UMaterialInstanceDynamic::Create(DefaultMaterial, nullptr);
	LineDynMaterial->SetVectorParameterValue(TEXT("Emissive"), FLinearColor::Black);
	LineDynMaterial->SetScalarParameterValue(TEXT("Alpha"), 0.8f);
	BoundingBoxActor->SetLineMaterial(LineDynMaterial);
	UMaterialInstanceDynamic* RegionDynMaterial = UMaterialInstanceDynamic::Create(DefaultMaterial, nullptr);
	RegionDynMaterial->SetVectorParameterValue(TEXT("Emissive"), FLinearColor::White);
	RegionDynMaterial->SetScalarParameterValue(TEXT("Alpha"), 0.0f);
	BoundingBoxActor->SetRegionMaterial(RegionDynMaterial);
	//BoundingBoxActor->SetIsEnableCollision(false);
	BoundingBoxActor->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	return BoundingBoxActor;
}

ASCTWireframeActor* FSCTShape::SpawnWireFrameActor()
{
	WireFrameActor = GEngine->GetWorldContextFromGameViewport(
		GEngine->GameViewport)->World()->SpawnActor<ASCTWireframeActor>(
			ASCTWireframeActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	WireFrameActor->SetShape(this);
	UMaterialInterface* DefaultMaterial = FMaterialManagerInstatnce::GetIns().GetMaterialManagerPtr()->GetForegroundColorMaterial();
	UMaterialInstanceDynamic* LineDynMaterial = UMaterialInstanceDynamic::Create(DefaultMaterial, nullptr);
	LineDynMaterial->SetVectorParameterValue(TEXT("Emissive"), FLinearColor::Blue);
	LineDynMaterial->SetScalarParameterValue(TEXT("Alpha"), 1.0f);
	WireFrameActor->SetLineMaterial(LineDynMaterial);
	WireFrameActor->SetIsEnableCollision(false);
	WireFrameActor->SetActorHiddenInGame(true);
	WireFrameActor->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	return WireFrameActor;
}

void FSCTShape::SetBoundingBoxActorHidden(bool bHidden)
{
	if (BoundingBoxActor)
	{
		BoundingBoxActor->SetActorHiddenInGame(bHidden);
	}
}

void FSCTShape::SetWireFrameActorHidden(bool bHidden)
{
	if (WireFrameActor)
	{
		WireFrameActor->SetActorHiddenInGame(bHidden);
	}
}

bool FSCTShape::SetShapeHoverFlag(bool bFlag)
{
	if (CurrentShowStatus > 2)
	{
		return false;
	}
	if (WireFrameActor)
	{
		WireFrameActor->SetActorHiddenInGame(!bFlag);
 		if (bFlag)
 		{
 			WireFrameActor->SetLineColor(FLinearColor(102/255.0, 125/255.0, 255/255.0, 1.0f));
 		}
		CurrentShowStatus = bFlag ? 2 : 0;
	}
	return true;
}

bool FSCTShape::SetShapeSelectFlag(bool bFlag)
{	
	if (CurrentShowStatus > 3)
	{
		return false;
	}
	if (WireFrameActor)
	{
 		if (bFlag)
 		{
 			WireFrameActor->SetLineColor(FLinearColor(0/255.0, 38/255.0, 255/255.0, 1.0f));
 		}		
		WireFrameActor->SetActorHiddenInGame(!bFlag);
		CurrentShowStatus = bFlag ? 3 : 0;
	}
	return true;
}

bool FSCTShape::SetShapeAbsorbFlag(bool bFlag)
{
	if (CurrentShowStatus > 1)
	{
		return false;
	}
	if (WireFrameActor)
	{
		if (bFlag)
		{
			WireFrameActor->SetLineColor(FLinearColor(11/255.0, 254/255.0, 253/255.0, 1.0f));
		}
		WireFrameActor->SetActorHiddenInGame(!bFlag);
		CurrentShowStatus = bFlag ? 1 : 0;
	}
	return true;
}

bool FSCTShape::SetShapeConflictFlag(bool bFlag)
{
	if (CurrentShowStatus > 4)
	{
		return false;
	}
	if (WireFrameActor)
	{
		if (bFlag)
		{
			WireFrameActor->SetLineColor(FLinearColor(144/255.0, 68/255.0, 54/255.0, 1.0f));
		}
		WireFrameActor->SetActorHiddenInGame(!bFlag);
		CurrentShowStatus = bFlag ? 4 : 0;
	}
	return true;
}

void FSCTShape::SetShapeShowStatus(int32 InStatus)
{
	CurrentShowStatus = InStatus;
	if (WireFrameActor && 0 == CurrentShowStatus)
	{
		WireFrameActor->SetActorHiddenInGame(true);
	}
}

void FSCTShape::SetWireFrameActorColor(const FLinearColor &InColor)
{
	if (WireFrameActor)
	{
		WireFrameActor->SetLineColor(InColor);
		WireFrameActor->SetActorHiddenInGame(false);
	}
	CurrentShowStatus = 0;
}
int32 FSCTShape::GetShapeShowStatus()
{
	return CurrentShowStatus;
}

void FSCTShape::UpdateShapeDimentionPosition()
{
	UpdateAssociatedActors(0);
	UpdateChileShapes(0);
}

void FSCTShape::UpdateAssociatedActors(int32 InType)
{
	//更新自身Actor
	if (ShapeActor)
	{
		switch (InType)
		{
		case 1:
		{
			ShapeActor->UpdateActorDimension();
			break;
		}
		case 2:
		{
			ShapeActor->UpdateActorPosition();
			break;
		}
		case 3:
		{
			ShapeActor->UpdateActorRotation();
			break;
		}
		default:
			ShapeActor->UpdateActorDimension();
			ShapeActor->UpdateActorPosition();
			ShapeActor->UpdateActorRotation();
			break;
		}
	}

	//更新外包框和包围盒
	if (WireFrameActor)
	{
		WireFrameActor->UpdateActorDimension();
	}
	if (BoundingBoxActor)
	{
		BoundingBoxActor->UpdateActorDimension();
	}
}

void FSCTShape::UpdateChileShapes(int32 InType)
{
	for (int32 i = 0; i < ChildrenShapes.Num(); ++i)
	{
		ChildrenShapes[i]->UpdateAssociatedActors(InType);
		ChildrenShapes[i]->UpdateChileShapes(InType);
	}
}

bool FSCTShape::IsFileJsonDownloadParsed() const
{
	return bJsonFileParsed;
}

//公式相关
FAny* FSCTShape::OnPValFunc(const FString& ValName)
{
	if (!ParentShape)
	{
		return nullptr;
	}
	return ParentShape->OnFindOwnValFunc(ValName);
}

FAny* FSCTShape::OnFindEntValue(int32 ShapeType, int64 ShapeId, const FString& ValName)
{
	if (!ParentShape)
	{
		return nullptr;
	}

	//获取兄弟型录
	const TArray<TSharedPtr<FSCTShape>>& ChildrenShapes = ParentShape->GetChildrenShapes();
	for (int32 i = 0; i < ChildrenShapes.Num(); i++)
	{
		if (ShapeType == ChildrenShapes[i]->ShapeType && ShapeId == ChildrenShapes[i]->ShapeId)
		{
			FAny* CurrentValue = ChildrenShapes[i]->OnFindOwnValFunc(ValName);
			if (CurrentValue)
			{
				return CurrentValue;
			}
		}
	}

	return nullptr;
}

FAny* FSCTShape::OnFindOwnValFunc(const FString& ValName)
{
	if (ValName == TEXT("W"))
	{
		return Width->GetAttributeValue().Get();
	}
	else if (ValName == TEXT("D"))
	{
		return Depth->GetAttributeValue().Get();
	}
	else if (ValName == TEXT("H"))
	{
		return Height->GetAttributeValue().Get();
	}
	else if (ValName == TEXT("POSX"))
	{
		return PositionX->GetAttributeValue().Get();
	}
	else if (ValName == TEXT("POSY"))
	{
		return PositionY->GetAttributeValue().Get();
	}
	else if (ValName == TEXT("POSZ"))
	{
		return PositionZ->GetAttributeValue().Get();
	}
	else if (ValName == TEXT("ROTX"))
	{
		return RotationX->GetAttributeValue().Get();
	}
	else if (ValName == TEXT("ROTY"))
	{
		return RotationY->GetAttributeValue().Get();
	}
	else if (ValName == TEXT("ROTZ"))
	{
		return RotationZ->GetAttributeValue().Get();
	}
	else if (ValName == TEXT("ShowCond"))
	{
		return ShowCondition->GetAttributeValue().Get();
	}
	else
	{
		return nullptr;
	}
}