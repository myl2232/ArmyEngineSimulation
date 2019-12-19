#include "SCTBoardShape.h"
#include "SCTShape.h"
#include "JsonObject.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "SCTBoardActor.h"
#include "SCTAttribute.h"
#include "Components/MeshComponent.h"
#include "SCTBoardActor.h"
#include "TypedPrimitive.h"
#include "SCTGlobalDataForShape.h"
#include "SCTOutline.h"
#include "SCTShapeBoxActor.h"
#include "GTEngineLibrary.h"

namespace ShapWithHoleValueCheck
{
	// 检查外方内方挖洞板件参数是否有效
	static bool InnerRectHoleValidCheck(const float ShapW,const float ShapeD,
		const float SHX,const float SHY,const float SHW,const float SHD)
	{
		bool bResult = true;		

		do
		{
			// SHX SHY SHW CHD 单值有效性
			if (SHX < 0.0f  ||
				SHY < 0.0f  ||
				SHX > ShapW ||
				SHX > ShapW ||
				SHW < 0.0f  ||
				SHW > ShapW ||
				SHD < 0.0f  ||
				SHD > ShapeD
				)
			{
				bResult = false;
			}

			if (bResult == false) break;

			// SHW CHD 组合有效性
			if ((SHX + SHW) > ShapW ||
				(SHY + SHD) > ShapeD
				)
			{
				bResult = false;
			}

		} while (false);
		return bResult;
	}

	// 检查外方内圆挖洞板件参数是否有效
	static bool InnerCircleHoleValidCheck(const float ShapW,const float ShapeD,
		const float CHX,const float CHY,const float CHR)
	{
		bool bResult = true;		

		do
		{
			// CHX CHY CHR 单值有效性
			if (CHX < 0.0f ||
				CHX > ShapW ||
				CHY < 0.0f ||
				CHY > ShapeD ||
				CHR < 0.0f ||
				CHR > ShapW ||
				CHR > ShapeD
				)
			{
				bResult = false;
			}

			if (bResult == false) break;

			// CHX CHY CHR  组合有效性
			if ((CHX + CHR) > ShapW ||
				(CHY + CHR) > ShapeD ||
				(CHX < CHR)||
				(CHY) < CHR
				)
			{
				bResult = false;
			}

		} while (false);

		return bResult;
	}	
}


#define CURVESTEP 128

FBoardShape::FBoardShape()
	:FSCTShape()
{
	SetShapeType(ST_Board);
	SubstrateList = MakeShareable(new FTextSelectAttri(this));
	EdgeMaterialList = MakeShareable(new FTextSelectAttri(this));
	CoverdEdgeList = MakeShareable(new FTextSelectAttri(this));
	OutlineData = MakeShareable(new FSCTOutline());
}

FBoardShape::~FBoardShape()
{

}

void FBoardShape::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//1、基类
	FSCTShape::ParseShapeFromJson(InJsonObject);
	//修改尺寸最小值<=0的情况
	TSharedPtr<FNumberRangeAttri> BoardWidthAttri = StaticCastSharedPtr<FNumberRangeAttri>(Width);
	if (BoardWidthAttri->GetMinValue() <= 0.0)
	{
		BoardWidthAttri->SetMinValue(1.0);
	}
	TSharedPtr<FNumberRangeAttri> BoardDepthAttri = StaticCastSharedPtr<FNumberRangeAttri>(Depth);
	if (BoardDepthAttri->GetMinValue() <= 0.0)
	{
		BoardDepthAttri->SetMinValue(1.0);
	}

	//2、产品编码
	SetProduceCode(InJsonObject->GetStringField(TEXT("ProduceCode")));

	//3、板件类型
	int32 BoardUseType = GetShapeCategory();
	//根据板件类型设置板件角度
	switch (BoardUseType)
	{
		case BUT_VerticalBoard:   // 竖板
		case BUT_LeftBoard:
		case BUT_RightBoard:
		case BUT_RevealBoard:
		case BUT_ExposureBoard:
		case BUT_AdjustBoard:
		{
			SetShapeRotY(GetShapePosY() + 90.0);
			break;
		}
		case BUT_BackBoard:   // 背板
		case BUT_FrontBoard:
		case BUT_EnhanceBoard:
		case BUT_ConvergentBoard:
		case BUT_SkirtingBoard:
		{
			SetShapeRotY(GetShapeRotY() + 90.0);
			SetShapeRotZ(GetShapeRotZ() - 90.0);
			break;
		}
		case BUT_HorizontalBoard:   // 横板
		case BUT_TopBoard:
		case BUT_BottomBoard:
		case BUT_MobileBoard:
		default:
			break;
	}

	//3、板件基材
	SubstrateType = (ESubstrateType)InJsonObject->GetIntegerField(TEXT("substrateType"));
	TSharedPtr<FJsonObject> SubstrateObj = InJsonObject->GetObjectField(TEXT("substrate"));
	SubstrateId = SubstrateObj->GetIntegerField(TEXT("id"));
	SubstrateName = SubstrateObj->GetStringField(TEXT("name"));

	//4、板件材质信息
	TSharedPtr<FJsonObject> MaterialObj = InJsonObject->GetObjectField(TEXT("material"));
	MaterialData.ID = MaterialObj->GetIntegerField(TEXT("id"));
	MaterialData.Name = MaterialObj->GetStringField(TEXT("name"));
	MaterialData.Url = MaterialObj->GetStringField(TEXT("pakUrl"));
	MaterialData.MD5 = MaterialObj->GetStringField(TEXT("pakMd5"));
	MaterialData.ThumbnailUrl = MaterialObj->GetStringField(TEXT("thumbnailUrl"));
	MaterialData.OptimizeParam = MaterialObj->GetStringField(TEXT("optimizeParam"));

	//5、板件封边信息
	TSharedPtr<FJsonObject> edgeBandingObj = InJsonObject->GetObjectField(TEXT("edgeBanding"));
	EdgeBandingData.ID = edgeBandingObj->GetIntegerField(TEXT("id"));
	EdgeBandingData.Name = edgeBandingObj->GetStringField(TEXT("name"));
	EdgeBandingData.Url = edgeBandingObj->GetStringField(TEXT("pakUrl"));
	EdgeBandingData.MD5 = edgeBandingObj->GetStringField(TEXT("pakMd5"));
	EdgeBandingData.ThumbnailUrl = edgeBandingObj->GetStringField(TEXT("thumbnailUrl"));
	EdgeBandingData.OptimizeParam = edgeBandingObj->GetStringField(TEXT("optimizeParam"));

	//6、包边信息
	TSharedPtr<FJsonObject> coverBandinglObj = InJsonObject->GetObjectField(TEXT("coverEdgeBanding"));
	CoverBandingData.ID = coverBandinglObj->GetIntegerField(TEXT("id"));
	CoverBandingData.Name = coverBandinglObj->GetStringField(TEXT("name"));
	CoverBandingData.Url = coverBandinglObj->GetStringField(TEXT("pakUrl"));
	CoverBandingData.MD5 = coverBandinglObj->GetStringField(TEXT("pakMd5"));
	CoverBandingData.ThumbnailUrl = coverBandinglObj->GetStringField(TEXT("thumbnailUrl"));
	CoverBandingData.OptimizeParam = coverBandinglObj->GetStringField(TEXT("optimizeParam"));
	if (coverBandinglObj->HasField(TEXT("visible")))
	{
		const TArray<TSharedPtr<FJsonValue>>& AraryValue = coverBandinglObj->GetArrayField(TEXT("visible"));
		for (const auto Index : AraryValue)
		{
			LightEdgeIndexArray.Emplace(static_cast<int32>(Index->AsNumber()));
		}
	}

	//7、铝箔信息
	bUsingAluminumFoil = InJsonObject->GetBoolField(TEXT("veneer"));

	if (InJsonObject->HasField(TEXT("veneerInfo")))
	{
		const TSharedPtr<FJsonObject> & ObjRef = InJsonObject->GetObjectField(TEXT("veneerInfo"));
		VeneerInfoData.ID = ObjRef->GetIntegerField(TEXT("id"));
		VeneerInfoData.Url = ObjRef->GetStringField(TEXT("pakUrl"));
		VeneerInfoData.MD5 = ObjRef->GetStringField(TEXT("pakMd5"));
		VeneerInfoData.OptimizeParam = ObjRef->GetStringField(TEXT("optimizeParam"));
	}
	

	//8、板件形状类型
	SetBoardShapeType((EBoardShapeType)InJsonObject->GetIntegerField(TEXT("BoardShapeType")));

	//9、板件形状参数
	const TArray<TSharedPtr<FJsonValue>>* ShapeAttriArray = nullptr;
	InJsonObject->TryGetArrayField(TEXT("boardShapeProperty"), ShapeAttriArray);
	if (ShapeAttriArray != nullptr && (*ShapeAttriArray).Num() > 0)
	{
		for (int32 i = 0; i < (*ShapeAttriArray).Num(); ++i)
		{
			TSharedPtr<FJsonValue> AttriValue = ShapeAttriArray->operator[](i);
			TSharedPtr<FJsonObject> AttriObject = AttriValue->AsObject();
			TSharedPtr<FShapeAttribute> NewParam = ParseAttributeFromJson(AttriObject, this);
			ShapeParameters.Add(NewParam);
		}
	}
	// 解析形状属性的字符串值
	if (InJsonObject->HasField(TEXT("boardShapePropertyAtrriValue")))
	{
		const TArray<TSharedPtr<FJsonValue>> & ShapeAttriArrayValue = InJsonObject->GetArrayField(TEXT("boardShapePropertyAtrriValue"));
		check(ShapeAttriArrayValue.Num() == ShapeParameters.Num());
		for(int32 Index = 0; Index < ShapeAttriArrayValue.Num(); ++Index)
		{			
			ShapeParameters[Index]->SetAttributeValue(ShapeAttriArrayValue[Index]->AsObject()->GetStringField(TEXT("strValue")));
		}		
	}

	//10、板件延伸值
	TSharedPtr<FJsonObject> plankStretchObj = InJsonObject->GetObjectField(TEXT("plankStretch"));
	float UpStretch = plankStretchObj->GetIntegerField(TEXT("up"));
	SetShapeTopExpand(UpStretch);
	float DownStretch = plankStretchObj->GetIntegerField(TEXT("down"));
	SetShapeBottomExpand(DownStretch);
	float LeftStretch = plankStretchObj->GetIntegerField(TEXT("left"));
	SetShapeLeftExpand(LeftStretch);
	float RightStretch = plankStretchObj->GetIntegerField(TEXT("right"));
	SetShapeRightExpand(RightStretch);
	float FrontStretch = plankStretchObj->GetIntegerField(TEXT("front"));
	SetShapeFrontExpand(FrontStretch);
	float BackStretch = plankStretchObj->GetIntegerField(TEXT("back"));
	SetShapeBackExpand(BackStretch);

	//11、是否活动层板
	bActiveBoard = InJsonObject->GetBoolField(TEXT("movable"));
	ActiveRetract = InJsonObject->GetNumberField(TEXT("retractValue"));
	SetActiveRetractValue(ActiveRetract);

	//12、内缩值
	Contraction = InJsonObject->GetNumberField(TEXT("contraction"));
}

void FBoardShape::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//0、型录类型和ID
	int32 CurrentType = InJsonObject->GetNumberField(TEXT("type"));
	int32 CurrentId = InJsonObject->GetNumberField(TEXT("id"));
	check(CurrentType == ShapeType && CurrentId == ShapeId);

	//1、型录基本信息
	FSCTShape::ParseAttributesFromJson(InJsonObject);

	//2、板件基材
	SubstrateType = (ESubstrateType)InJsonObject->GetIntegerField(TEXT("substrateType"));
	TSharedPtr<FJsonObject> SubstrateObj = InJsonObject->GetObjectField(TEXT("substrate"));
	SubstrateId = SubstrateObj->GetIntegerField(TEXT("id"));
	SubstrateName = SubstrateObj->GetStringField(TEXT("name"));

	//3、板件材质信息
	TSharedPtr<FJsonObject> MaterialObj = InJsonObject->GetObjectField(TEXT("material"));
	MaterialData.ID = MaterialObj->GetIntegerField(TEXT("id"));
	MaterialData.Name = MaterialObj->GetStringField(TEXT("name"));
	MaterialData.Url = MaterialObj->GetStringField(TEXT("pakUrl"));
	MaterialData.MD5 = MaterialObj->GetStringField(TEXT("pakMd5"));
	MaterialData.ThumbnailUrl = MaterialObj->GetStringField(TEXT("thumbnailUrl"));
	MaterialData.OptimizeParam = MaterialObj->GetStringField(TEXT("optimizeParam"));

	//4、板件封边信息
	TSharedPtr<FJsonObject> edgeBandingObj = InJsonObject->GetObjectField(TEXT("edgeBanding"));
	EdgeBandingData.ID = edgeBandingObj->GetIntegerField(TEXT("id"));
	EdgeBandingData.Name = edgeBandingObj->GetStringField(TEXT("name"));
	EdgeBandingData.Url = edgeBandingObj->GetStringField(TEXT("pakUrl"));
	EdgeBandingData.MD5 = edgeBandingObj->GetStringField(TEXT("pakMd5"));
	EdgeBandingData.ThumbnailUrl = edgeBandingObj->GetStringField(TEXT("thumbnailUrl"));
	EdgeBandingData.OptimizeParam = edgeBandingObj->GetStringField(TEXT("optimizeParam"));

	//5、包边信息
	TSharedPtr<FJsonObject> coverBandinglObj = InJsonObject->GetObjectField(TEXT("coverEdgeBanding"));
	CoverBandingData.ID = coverBandinglObj->GetIntegerField(TEXT("id"));
	CoverBandingData.Name = coverBandinglObj->GetStringField(TEXT("name"));
	CoverBandingData.Url = coverBandinglObj->GetStringField(TEXT("pakUrl"));
	CoverBandingData.MD5 = coverBandinglObj->GetStringField(TEXT("pakMd5"));
	CoverBandingData.ThumbnailUrl = coverBandinglObj->GetStringField(TEXT("thumbnailUrl"));
	CoverBandingData.OptimizeParam = coverBandinglObj->GetStringField(TEXT("optimizeParam"));
	if (coverBandinglObj->HasField(TEXT("visibleEdgeList")))
	{
		const TArray<TSharedPtr<FJsonValue>>& AraryValue = coverBandinglObj->GetArrayField(TEXT("visibleEdgeList"));
		for (const auto Index : AraryValue)
		{
			LightEdgeIndexArray.Emplace(static_cast<int32>(Index->AsNumber()));
		}
	}

	//6、铝箔信息
	bUsingAluminumFoil = InJsonObject->GetBoolField(TEXT("veneer"));
	if (InJsonObject->HasField(TEXT("veneerInfo")))
	{
		const TSharedPtr<FJsonObject> & ObjRef = InJsonObject->GetObjectField(TEXT("veneerInfo"));
		VeneerInfoData.ID = ObjRef->GetIntegerField(TEXT("id"));
		VeneerInfoData.Url = ObjRef->GetStringField(TEXT("pakUrl"));
		VeneerInfoData.MD5 = ObjRef->GetStringField(TEXT("pakMd5"));
		VeneerInfoData.OptimizeParam = ObjRef->GetStringField(TEXT("optimizeParam"));
	}
	

	//7、板件形状类型
	int32 BoardShapeTypeInt = InJsonObject->GetIntegerField(TEXT("BoardShapeType"));
	SetBoardShapeType((EBoardShapeType)BoardShapeTypeInt);

	//8、板件形状参数
	const TArray<TSharedPtr<FJsonValue>>* ShapeAttriArray = nullptr;
	InJsonObject->TryGetArrayField(TEXT("boardShapeProperty"), ShapeAttriArray);
	if (ShapeAttriArray != nullptr && (*ShapeAttriArray).Num() > 0)
	{
		for (int32 i = 0; i<(*ShapeAttriArray).Num(); ++i)
		{
			TSharedPtr<FJsonValue> AttriValue = ShapeAttriArray->operator[](i);
			TSharedPtr<FJsonObject> AttriObject = AttriValue->AsObject();
			TSharedPtr<FShapeAttribute> NewParam = ParseAttributeFromJson(AttriObject, this);
			ShapeParameters.Add(NewParam);
		}
	}
	// 解析形状属性的字符串值
	if (InJsonObject->HasField(TEXT("boardShapePropertyAtrriValue")))
	{
		const TArray<TSharedPtr<FJsonValue>> & ShapeAttriArrayValue = InJsonObject->GetArrayField(TEXT("boardShapePropertyAtrriValue"));
		check(ShapeAttriArrayValue.Num() == ShapeParameters.Num());
		for (int32 Index = 0; Index < ShapeAttriArrayValue.Num(); ++Index)
		{
			ShapeParameters[Index]->SetAttributeValue(ShapeAttriArrayValue[Index]->AsObject()->GetStringField(TEXT("strValue")));
		}
	}


	//9、板件延伸值
	TSharedPtr<FJsonObject> plankStretchObj = InJsonObject->GetObjectField(TEXT("plankStretch"));
	float UpStretch = plankStretchObj->GetIntegerField(TEXT("up"));
	SetShapeTopExpand(UpStretch);
	float DownStretch = plankStretchObj->GetIntegerField(TEXT("down"));
	SetShapeBottomExpand(DownStretch);
	float LeftStretch = plankStretchObj->GetIntegerField(TEXT("left"));
	SetShapeLeftExpand(LeftStretch);
	float RightStretch = plankStretchObj->GetIntegerField(TEXT("right"));
	SetShapeRightExpand(RightStretch);
	float FrontStretch = plankStretchObj->GetIntegerField(TEXT("front"));
	SetShapeFrontExpand(FrontStretch);
	float BackStretch = plankStretchObj->GetIntegerField(TEXT("back"));
	SetShapeBackExpand(BackStretch);

	//10、是否活动层板
	bActiveBoard = InJsonObject->GetBoolField(TEXT("movable"));
	ActiveRetract = InJsonObject->GetNumberField(TEXT("retractValue"));
	SetActiveRetractValue(ActiveRetract);

	//11、内缩值
	Contraction = InJsonObject->GetNumberField(TEXT("contraction"));
	if (InJsonObject->HasTypedField<EJson::Number>(TEXT("confluenceContraction")))
	{
		ConfluenceContraction = InJsonObject->GetNumberField(TEXT("confluenceContraction"));
	}
}

void FBoardShape::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//1、解析基本信息
	FSCTShape::ParseShapeFromJson(InJsonObject);
	//2、产品编码
	SetProduceCode(InJsonObject->GetStringField(TEXT("ProduceCode")));
}

void FBoardShape::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//注意此次调用板件的ParseFromJson()
	FBoardShape::ParseFromJson(InJsonObject);
}

void FBoardShape::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//1、解析基本信息
	FSCTShape::SaveToJson(JsonWriter);

	//2、产品编码
	JsonWriter->WriteValue(TEXT("ProduceCode"), GetProduceCode());

	//3、板件基材
	JsonWriter->WriteValue(TEXT("substrateType"), (int32)GetSubstrateType());
	JsonWriter->WriteObjectStart(TEXT("substrate"));
	JsonWriter->WriteValue(TEXT("id"), SubstrateId);
	JsonWriter->WriteValue(TEXT("name"), SubstrateName);
	JsonWriter->WriteObjectEnd();

	//4、板件材质ID
	JsonWriter->WriteObjectStart(TEXT("material"));
	JsonWriter->WriteValue(TEXT("id"), MaterialData.ID);
	JsonWriter->WriteValue(TEXT("name"), MaterialData.Name);
	JsonWriter->WriteValue(TEXT("pakUrl"), MaterialData.Url);
	JsonWriter->WriteValue(TEXT("pakMd5"), MaterialData.MD5);
	JsonWriter->WriteValue(TEXT("thumbnailUrl"), MaterialData.ThumbnailUrl);
	JsonWriter->WriteValue(TEXT("optimizeParam"), MaterialData.OptimizeParam);	
	JsonWriter->WriteObjectEnd();
	
	//5、封边信息
	JsonWriter->WriteObjectStart(TEXT("edgeBanding"));
	JsonWriter->WriteValue(TEXT("id"), EdgeBandingData.ID);
	JsonWriter->WriteValue(TEXT("name"), EdgeBandingData.Name);
	JsonWriter->WriteValue(TEXT("pakUrl"), EdgeBandingData.Url);
	JsonWriter->WriteValue(TEXT("pakMd5"), EdgeBandingData.MD5);
	JsonWriter->WriteValue(TEXT("ThumbnailUrl"), EdgeBandingData.ThumbnailUrl);
	JsonWriter->WriteValue(TEXT("optimizeParam"), EdgeBandingData.OptimizeParam);
	JsonWriter->WriteObjectEnd();

	//6、铝箔信息
	JsonWriter->WriteValue(TEXT("veneer"), bUsingAluminumFoil);

	// 存储铝箔信息
	if (bUsingAluminumFoil)
	{
		
		int32 MaterialID = 0;
		FString MaterialUrl, MaterialMd5, Paramter;
		bool Result = FGlobalDataForShape::GetInsPtr()->GetSingleVennerMat(MaterialID, MaterialUrl, MaterialMd5, Paramter);
		check(Result);
		JsonWriter->WriteObjectStart(TEXT("veneerInfo"));
		JsonWriter->WriteValue(TEXT("id"), MaterialID);		
		JsonWriter->WriteValue(TEXT("pakUrl"), MaterialUrl);
		JsonWriter->WriteValue(TEXT("pakMd5"), MaterialMd5);		
		JsonWriter->WriteValue(TEXT("optimizeParam"), Paramter);		
		JsonWriter->WriteObjectEnd();
	}

	//7、板件形状
	JsonWriter->WriteValue(TEXT("BoardShapeType"), (int32)GetBoardShapeType());

	//8、板件形状属性
	JsonWriter->WriteArrayStart(TEXT("boardShapeProperty"));
	for (int32 i = 0; i < ShapeParameters.Num(); ++i)
	{
		JsonWriter->WriteObjectStart();
		ShapeParameters[i]->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	// 保存形状属性的字符串值
	JsonWriter->WriteArrayStart(TEXT("boardShapePropertyAtrriValue"));
	for (int32 i = 0; i < ShapeParameters.Num(); ++i)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue(TEXT("refName"), ShapeParameters[i]->GetRefName());
		JsonWriter->WriteValue(TEXT("strValue"), ShapeParameters[i]->GetAttributeStr());
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//板件延伸值
	JsonWriter->WriteObjectStart(TEXT("plankStretch"));
	JsonWriter->WriteValue(TEXT("up"), GetShapeTopExpand());
	JsonWriter->WriteValue(TEXT("down"), GetShapeBottomExpand());
	JsonWriter->WriteValue(TEXT("left"), GetShapeLeftExpand());
	JsonWriter->WriteValue(TEXT("right"), GetShapeRightExpand());
	JsonWriter->WriteValue(TEXT("front"), GetShapeFrontExpand());
	JsonWriter->WriteValue(TEXT("back"), GetShapeBackExpand());
	JsonWriter->WriteObjectEnd();

	//活动层板
	JsonWriter->WriteValue(TEXT("movable"), bActiveBoard);
	JsonWriter->WriteValue(TEXT("retractValue"), ActiveRetract);

	//内缩值
	JsonWriter->WriteValue(TEXT("contraction"), Contraction);
}

void FBoardShape::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//1、型录基本信息
	FSCTShape::SaveAttriToJson(JsonWriter);

	//2、板件基材信息
	JsonWriter->WriteValue(TEXT("substrateType"), (int32)GetSubstrateType());
	JsonWriter->WriteObjectStart(TEXT("substrate"));
	JsonWriter->WriteValue(TEXT("id"), SubstrateId);
	JsonWriter->WriteValue(TEXT("name"), SubstrateName);
	JsonWriter->WriteObjectEnd();

	//3、板件材质信息
	JsonWriter->WriteObjectStart(TEXT("material"));
	JsonWriter->WriteValue(TEXT("id"), MaterialData.ID);
	JsonWriter->WriteValue(TEXT("name"), MaterialData.Name);
	JsonWriter->WriteValue(TEXT("pakMd5"), MaterialData.MD5);
	JsonWriter->WriteValue(TEXT("pakUrl"), MaterialData.Url);
	JsonWriter->WriteValue(TEXT("thumbnailUrl"), MaterialData.ThumbnailUrl);
	JsonWriter->WriteValue(TEXT("optimizeParam"), MaterialData.OptimizeParam);
	JsonWriter->WriteObjectEnd();

	//4、板件封边材质ID
	JsonWriter->WriteObjectStart(TEXT("edgeBanding"));
	JsonWriter->WriteValue(TEXT("id"), EdgeBandingData.ID);
	JsonWriter->WriteValue(TEXT("name"), EdgeBandingData.Name);
	JsonWriter->WriteValue(TEXT("pakMd5"), EdgeBandingData.MD5);
	JsonWriter->WriteValue(TEXT("pakUrl"), EdgeBandingData.Url);
	JsonWriter->WriteValue(TEXT("thumbnailUrl"), EdgeBandingData.ThumbnailUrl);
	JsonWriter->WriteValue(TEXT("optimizeParam"), EdgeBandingData.OptimizeParam);
	JsonWriter->WriteObjectEnd();

	//7、包边信息	
	JsonWriter->WriteObjectStart(TEXT("coverEdgeBanding"));
	JsonWriter->WriteValue(TEXT("id"), CoverBandingData.ID);
	JsonWriter->WriteValue(TEXT("name"), CoverBandingData.Name);
	JsonWriter->WriteValue(TEXT("pakUrl"), CoverBandingData.Url);
	JsonWriter->WriteValue(TEXT("pakMd5"), CoverBandingData.MD5);
	JsonWriter->WriteValue(TEXT("thumbnailUrl"), CoverBandingData.ThumbnailUrl);
	JsonWriter->WriteValue(TEXT("optimizeParam"), CoverBandingData.OptimizeParam);
	JsonWriter->WriteArrayStart(TEXT("visibleEdgeList"));
	for (const auto Index : LightEdgeIndexArray)
	{
		JsonWriter->WriteValue(Index);
	}
	JsonWriter->WriteArrayEnd();
	JsonWriter->WriteObjectEnd();

	//铝箔 
	JsonWriter->WriteValue(TEXT("veneer"), bUsingAluminumFoil);
	if (bUsingAluminumFoil)
	{
#if 0
		int32 MaterialID = 0;
		FString MaterialUrl, MaterialMd5, Paramter;
		bool Result = FGlobalDataForShape::GetInsPtr()->GetSingleVennerMat(MaterialID, MaterialUrl, MaterialMd5, Paramter);
		check(Result);
		JsonWriter->WriteObjectStart(TEXT("veneerInfo"));
		JsonWriter->WriteValue(TEXT("id"), MaterialID);
		JsonWriter->WriteValue(TEXT("pakUrl"), MaterialUrl);
		JsonWriter->WriteValue(TEXT("pakMd5"), MaterialMd5);
		JsonWriter->WriteValue(TEXT("optimizeParam"), Paramter);		
		JsonWriter->WriteObjectEnd();
#else
		JsonWriter->WriteObjectStart(TEXT("veneerInfo"));
		JsonWriter->WriteValue(TEXT("id"), GetVeneerInfoData().ID);
		JsonWriter->WriteValue(TEXT("pakUrl"), GetVeneerInfoData().Url);
		JsonWriter->WriteValue(TEXT("pakMd5"), GetVeneerInfoData().MD5);
		JsonWriter->WriteValue(TEXT("optimizeParam"), GetVeneerInfoData().OptimizeParam);
		JsonWriter->WriteObjectEnd();
#endif
	}

	//5、板件形状
	JsonWriter->WriteValue(TEXT("BoardShapeType"), (int32)GetBoardShapeType());

	//6、板件形状参数
	JsonWriter->WriteArrayStart(TEXT("boardShapeProperty"));
	for (int32 i = 0; i < ShapeParameters.Num(); ++i)
	{
		JsonWriter->WriteObjectStart();
		ShapeParameters[i]->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	// 保存形状属性的字符串值
	JsonWriter->WriteArrayStart(TEXT("boardShapePropertyAtrriValue"));
	for (int32 i = 0; i < ShapeParameters.Num(); ++i)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue(TEXT("refName"), ShapeParameters[i]->GetRefName());
		JsonWriter->WriteValue(TEXT("strValue"), ShapeParameters[i]->GetAttributeStr());
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//板件延伸值
	JsonWriter->WriteObjectStart(TEXT("plankStretch"));
	JsonWriter->WriteValue(TEXT("up"), GetShapeTopExpand());
	JsonWriter->WriteValue(TEXT("down"), GetShapeBottomExpand());
	JsonWriter->WriteValue(TEXT("left"), GetShapeLeftExpand());
	JsonWriter->WriteValue(TEXT("right"), GetShapeRightExpand());
	JsonWriter->WriteValue(TEXT("front"), GetShapeFrontExpand());
	JsonWriter->WriteValue(TEXT("back"), GetShapeBackExpand());
	JsonWriter->WriteObjectEnd();

	//活动层板
	JsonWriter->WriteValue(TEXT("movable"), bActiveBoard);
	JsonWriter->WriteValue(TEXT("retractValue"), ActiveRetract);

	//内缩值
	JsonWriter->WriteValue(TEXT("contraction"), Contraction);
	JsonWriter->WriteValue(TEXT("confluenceContraction"), ConfluenceContraction);
}

void FBoardShape::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//1、解析基本信息
	FSCTShape::SaveShapeToJson(JsonWriter);
	//2、产品编码
	JsonWriter->WriteValue(TEXT("ProduceCode"), GetProduceCode());
}

void FBoardShape::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveContentToJson(JsonWriter);
}

void FBoardShape::GetResourceUrls(TArray<FString> &OutResourceUrls)
{
	if (!MaterialData.Url.IsEmpty())
	{
		OutResourceUrls.Push(MaterialData.Url);
	}
	if (!EdgeBandingData.Url.IsEmpty())
	{
		OutResourceUrls.Push(EdgeBandingData.Url);
	}
	if (!CoverBandingData.Url.IsEmpty())
	{
		OutResourceUrls.Push(CoverBandingData.Url);
	}
	if (bUsingAluminumFoil && VeneerInfoData.Url.IsEmpty() == false)
	{
		OutResourceUrls.Push(VeneerInfoData.Url);
	}
}

void FBoardShape::GetFileCachePaths(TArray<FString> &OutFileCachePaths)
{
	if (!MaterialData.Url.IsEmpty())
	{
		const FString RelativePath = TEXT("Material/Board/BoardMaterial_") + FString::Printf(TEXT("%d.pak"), MaterialData.ID);
		OutFileCachePaths.Emplace(
			FPaths::Combine(FGlobalDataForShape::GetInsPtr()->GetRootDir(), RelativePath)
		);
	}
	if (!EdgeBandingData.Url.IsEmpty())
	{
		const FString RelativePath = TEXT("Material/Edge/EdgeMaterial_") + FString::Printf(TEXT("%d.pak"), EdgeBandingData.ID);
		OutFileCachePaths.Emplace(
			FPaths::Combine(FGlobalDataForShape::GetInsPtr()->GetRootDir(), RelativePath)
		);
	}
	if (!CoverBandingData.Url.IsEmpty())
	{
		const FString RelativePath = TEXT("Material/CoveredEdge/CoveredMaterial_") + FString::Printf(TEXT("%d.pak"), CoverBandingData.ID);
		OutFileCachePaths.Emplace(
			FPaths::Combine(FGlobalDataForShape::GetInsPtr()->GetRootDir(), RelativePath)
			);
	}
	if (bUsingAluminumFoil && VeneerInfoData.Url.IsEmpty() == false)
	{
		const FString RelativePath = TEXT("Material/AluminumFoil/AluminumFoilMaterial") + FString::Printf(TEXT("%d.pak"), VeneerInfoData.ID);
		OutFileCachePaths.Emplace(
			FPaths::Combine(FGlobalDataForShape::GetInsPtr()->GetRootDir(), RelativePath)
		);
	}
}

void FBoardShape::CopyTo(FSCTShape* OutShape)
{
	FSCTShape::CopyTo(OutShape);

	FBoardShape* BoardShape = StaticCast<FBoardShape*>(OutShape);
	//产品编码
	BoardShape->ProduceCode = ProduceCode;

	//基材信息
	BoardShape->SubstrateType = SubstrateType;
	BoardShape->SubstrateId = SubstrateId;
	BoardShape->SubstrateName = SubstrateName;

	//材质信息
	BoardShape->MaterialData = MaterialData;

	//封边信息
	BoardShape->EdgeBandingData = EdgeBandingData;
	EdgeMaterialList->CopyTo(BoardShape->EdgeMaterialList);

	//包边信息
	BoardShape->CoverBandingData = CoverBandingData;
	BoardShape->LightEdgeIndexArray = LightEdgeIndexArray;
	CoverdEdgeList->CopyTo(BoardShape->CoverdEdgeList);

	//铝箔信息
	BoardShape->bUsingAluminumFoil = bUsingAluminumFoil;
	BoardShape->VeneerInfoData = VeneerInfoData;

	//形状类型
	BoardShape->BoardShapeType = BoardShapeType;

	//延伸值
	BoardShape->LeftExtension = LeftExtension;
	BoardShape->RightExtension = RightExtension;
	BoardShape->BackExtension = BackExtension;
	BoardShape->FrontExtension = FrontExtension;
	//内缩值
	BoardShape->Contraction = Contraction;
	BoardShape->ConfluenceContraction = ConfluenceContraction;

	//是否为活动层板
	BoardShape->SetBoardActive(bActiveBoard, ActiveRetract);
	
	// 材质旋转值
	BoardShape->MaterialRotateValue = MaterialRotateValue;

	//板件形状参数
	for (int32 i = 0; i < ShapeParameters.Num(); ++i)
	{
		BoardShape->ShapeParameters.Add(CopyAttributeToNew(ShapeParameters[i], BoardShape));
	}
}

ASCTShapeActor* FBoardShape::SpawnShapeActor()
{
	//创建生成型录Actor
	UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();
	ASCTBoardActor* NewShapeActor = World->SpawnActor<ASCTBoardActor>(
		ASCTBoardActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);

	//设置关联的Shape
	ShapeActor = NewShapeActor;
	NewShapeActor->SetShape(this);

	//设置板件点位信息
	float BoardThickness = GetShapeHeight();
	NewShapeActor->SetBoardPointsAndThickness(BoardThickness);

	//设置型录的位置的旋转
	NewShapeActor->UpdateActorPosition();
	NewShapeActor->UpdateActorRotation();

	// 更新板件材质 
	NewShapeActor->UpdateActorBoardMaterial();

	// 更新封边材质
	NewShapeActor->UpdateEdgeMaterial();

	// 更新包边材质
	NewShapeActor->UpdateActorCoveredEdgeMaterial();

	// 更新铝箔
	NewShapeActor->UpdateAluminumFildMaterial();

	//设置型录可见性
	bool bParentVisible = NewShapeActor->GetVisibleStateFromParent();
	NewShapeActor->SetActorHiddenInGame(!GetShowCondition() || !bParentVisible);

	return NewShapeActor;
}

// void FBoardShape::DestroyShapeActor()
// {
// 	if (ShapeActor)
// 	{
// 		ShapeActor->DetachFromActorOverride(FDetachmentTransformRules::KeepRelativeTransform);
// 		ShapeActor->Destroy();
// 		ShapeActor = nullptr;
// 	}
// }

void FBoardShape::SpawnActorsForSelected(FName InProfileName)
{
	ASCTShapeActor* CurBoardActor = GetShapeActor();
	if (!CurBoardActor)
	{
		return;
	}

	//设置ProfileName
	ASCTBoardActor* CurBoardShapeActor = StaticCast<ASCTBoardActor*>(CurBoardActor);
	CurBoardShapeActor->SetCollisionProfileName(InProfileName);
	//创建型录外包框
	ASCTWireframeActor* WireFrameActor = SpawnWireFrameActor();
	WireFrameActor->UpdateActorDimension();
	WireFrameActor->SetActorHiddenInGame(true);
	WireFrameActor->AttachToActor(CurBoardActor, FAttachmentTransformRules::KeepRelativeTransform);
}

void FBoardShape::SetCollisionProfileName(FName InProfileName)
{
	if (ShapeActor == nullptr)
	{
		return;
	}
	ASCTBoardActor * BoardActor = Cast<ASCTBoardActor>(ShapeActor);
	check(BoardActor != nullptr)	
	BoardActor->SetCollisionProfileName(InProfileName);		
}

bool FBoardShape::SetShapeWidth(float InIntValue)
{
	return FBoardShape::SetShapeWidth(FString::Printf(TEXT("%f"), InIntValue));
}

bool FBoardShape::SetShapeWidth(const FString &InStrValue) 
{
	FString OldWidthStr = Width->GetAttributeStr();
	bool bResult = true;
	if (OldWidthStr != InStrValue)
	{
		bResult = Width->SetAttributeValue(InStrValue);
		//检验板件宽度是否与异形板参数冲突
		if (bResult)
		{
			float ShapeWidth = GetShapeWidth();
			switch (GetBoardShapeType())
			{
			case BoST_LeftCut:
			case BoST_RightCut:
			{
				float ShapeParam = GetShapeParamValueByRefName(RefName_TW);
				if (ShapeWidth <= ShapeParam)
				{
					bResult = false;
				}
				break;
			}
			case BoST_LeftCorner:
			case BoST_RightCorner:
			{
				float ShapeParam = GetShapeParamValueByRefName(RefName_CW);
				if (ShapeWidth <= ShapeParam)
				{
					bResult = false;
				}
				break;
			}
			case BoST_LeftOblique:
			case BoST_RightOblique:
			{
				float ShapeParam = GetShapeParamValueByRefName(RefName_SW);
				if (ShapeWidth <= ShapeParam)
				{
					bResult = false;
				}
				break;
			}
			case BoST_LeftOutArc:
			case BoST_RightOutArc:
			case BoST_LeftInArc:
			case BoST_RightInArc:
			{
				float ShapeParam = GetShapeParamValueByRefName(RefName_TLEN);
				if (ShapeWidth <= ShapeParam)
				{
					bResult = false;
				}
				break;
			}
			case BoST_LeftCornerCut:
			case BoST_RightCornerCut:
			{
				float ShapeParam0 = GetShapeParamValueByRefName(RefName_TW);
				float ShapeParam1 = GetShapeParamValueByRefName(RefName_CW);
				if (ShapeWidth <= ShapeParam0 + ShapeParam1)
				{
					bResult = false;
				}
				break;
			}
			case BoST_LeftInArcCut:
			case BoST_RightInArcCut:
			{
				float ShapeParam0 = GetShapeParamValueByRefName(RefName_TLEN);
				float ShapeParam1 = GetShapeParamValueByRefName(RefName_TW);
				if (ShapeWidth <= ShapeParam0 + ShapeParam1)
				{
					bResult = false;
				}
				break;
			}
			case BoST_InnerCircleHole:
			{
				bResult = CheckInnerCircleHoleValied();
				break;
			}
			case BoST_InnerRectangleHole:
			{
				bResult = CheckInnerRectHoleValied();
				break;
			}
			case BoST_LeftRightOutArc:
			{
				float ShapeParam0 = GetShapeParamValueByRefName(RefName_LTLEN);
				float ShapeParam1 = GetShapeParamValueByRefName(RefName_RTLEN);
				if (ShapeWidth < ShapeParam0 + ShapeParam1)
				{
					bResult = false;
				}
				break;
			}
			default:
				break;
			}

			//若出现冲突，恢复形状参数的原来属性值
			if (!bResult)
			{
				Width->SetAttributeValue(OldWidthStr);
			}
		}

		//若Actor已生成，同时更新Actor
		if (bResult)
		{
			UpdateAssociatedActors(1);
		}
	}

	return bResult;
}

bool FBoardShape::SetShapeDepth(float InIntValue)
{
	return FBoardShape::SetShapeDepth(FString::Printf(TEXT("%f"), InIntValue));
}

bool FBoardShape::SetShapeDepth(const FString &InStrValue)
{
	FString OldDepthStr = Depth->GetAttributeStr();
	bool bResult = true;
	if (OldDepthStr != InStrValue)
	{
		bResult = Depth->SetAttributeValue(InStrValue);
		//检验板件宽度是否与异形板参数冲突
		if (bResult)
		{
			float ShapeDepth = GetShapeDepth();
			switch (GetBoardShapeType())
			{
			case BoST_LeftCut:
			case BoST_RightCut:
			{
				float ShapeParam = GetShapeParamValueByRefName(RefName_TD);
				if (ShapeDepth <= ShapeParam)
				{
					bResult = false;
				}
				break;
			}
			case BoST_LeftCorner:
			case BoST_RightCorner:
			{
				float ShapeParam = GetShapeParamValueByRefName(RefName_CD);
				if (ShapeDepth <= ShapeParam)
				{
					bResult = false;
				}
				break;
			}
			case BoST_LeftOblique:
			case BoST_RightOblique:
			{
				float ShapeParam = GetShapeParamValueByRefName(RefName_SD);
				if (ShapeDepth <= ShapeParam)
				{
					bResult = false;
				}
				break;
			}
			case BoST_LeftOutArc:
			case BoST_RightOutArc:
			case BoST_LeftInArc:
			case BoST_RightInArc:
			{
				float ShapeParam = GetShapeParamValueByRefName(RefName_TLEN);
				if (ShapeDepth <= ShapeParam)
				{
					bResult = false;
				}
				break;
			}
			case BoST_LeftCornerCut:
			case BoST_RightCornerCut:
			{
				float ShapeParam0 = GetShapeParamValueByRefName(RefName_TD);
				float ShapeParam1 = GetShapeParamValueByRefName(RefName_CD);
				if (ShapeDepth <= ShapeParam0 + ShapeParam1)
				{
					bResult = false;
				}
				break;
			}
			case BoST_LeftInArcCut:
			case BoST_RightInArcCut:
			{
				float ShapeParam0 = GetShapeParamValueByRefName(RefName_TD);
				float ShapeParam1 = GetShapeParamValueByRefName(RefName_TLEN);
				if (ShapeDepth <= ShapeParam0 || ShapeDepth <= ShapeParam1)
				{
					bResult = false;
				}
				break;
			}
			case BoST_InnerCircleHole:
			{
				bResult = CheckInnerCircleHoleValied();
				break;
			}
			case BoST_InnerRectangleHole:
			{
				bResult = CheckInnerRectHoleValied();
				break;
			}
			case BoST_LeftRightOutArc:
			{
				float ShapeParam0 = GetShapeParamValueByRefName(RefName_LTLEN);
				float ShapeParam1 = GetShapeParamValueByRefName(RefName_RTLEN);
				if (ShapeDepth < ShapeParam0 || ShapeDepth < ShapeParam1)
				{
					bResult = false;
				}
				break;
			}
			default:
				break;
			}

			//若出现冲突，恢复形状参数的原来属性值
			if (!bResult)
			{
				Depth->SetAttributeValue(OldDepthStr);
			}
		}
		//若Actor已生成，同时更新Actor
		if (bResult)
		{
			UpdateAssociatedActors(1);
		}
	}

	return bResult;
}

bool FBoardShape::ModifyShapeDimentionDelta(const FVector &InDelta)
{
	bool bResult = false;
	switch (GetShapeCategory())
	{
	case BUT_HorizontalBoard:   //横版
	case BUT_TopBoard:
	case BUT_BottomBoard:
	case BUT_MobileBoard:
	{
		bResult = SetShapeWidth(GetShapeWidth() + InDelta.X * 10.0f);
		bResult = SetShapeDepth(GetShapeDepth() + InDelta.Y * 10.0f);
		break;
	}
	case BUT_VerticalBoard:     //竖板
	case BUT_LeftBoard:
	case BUT_RightBoard:
	case BUT_RevealBoard:
	case BUT_ExposureBoard:
	case BUT_AdjustBoard:
	{
		bResult = SetShapeWidth(GetShapeWidth() + InDelta.Z * 10.0f);
		bResult = SetShapeDepth(GetShapeDepth() + InDelta.Y * 10.0f);
		break;
	}
	case BUT_BackBoard:      //背板:
	case BUT_FrontBoard:
	case BUT_EnhanceBoard:
	case BUT_ConvergentBoard:
	case BUT_SkirtingBoard:
	{
		bResult = SetShapeWidth(GetShapeWidth() + InDelta.Z * 10.0f);
		bResult = SetShapeDepth(GetShapeDepth() + InDelta.X * 10.0f);
		break;
	}
	default:
		check(false);
		break;
	}

	return bResult;
}

float FBoardShape::GetShapePosLeftSide()
{
	switch (ShapeCategory)
	{
	case BUT_HorizontalBoard:
	case BUT_TopBoard:
	case BUT_BottomBoard:
	case BUT_MobileBoard:
	case BUT_BackBoard:
	case BUT_FrontBoard:
	case BUT_EnhanceBoard:
	case BUT_ConvergentBoard:
	case BUT_SkirtingBoard:
		return GetShapePosX();
	case BUT_VerticalBoard:
	case BUT_LeftBoard:
	case BUT_RightBoard:
	case BUT_RevealBoard:
	case BUT_ExposureBoard:
	case BUT_AdjustBoard:
		return GetShapePosX() - GetShapeHeight();
	default:
		return 0.0f;
	}
}

float FBoardShape::GetShapePosRightSide()
{
	switch (ShapeCategory)
	{
	case BUT_HorizontalBoard:   //横板
	case BUT_TopBoard:
	case BUT_BottomBoard:
	case BUT_MobileBoard:
		return GetShapePosX() + GetShapeWidth();
	case BUT_VerticalBoard:  //竖板
	case BUT_LeftBoard:
	case BUT_RightBoard:
	case BUT_RevealBoard:
	case BUT_ExposureBoard:
	case BUT_AdjustBoard:
		return GetShapePosX();
	case BUT_BackBoard:   //背板
	case BUT_FrontBoard:
	case BUT_EnhanceBoard:
	case BUT_ConvergentBoard:
	case BUT_SkirtingBoard:
		return GetShapePosX() + GetShapeDepth();
	default:
		return 0.0f;
	}
}

float FBoardShape::GetShapePosFrontSide()
{
	switch (ShapeCategory)
	{
	case BUT_HorizontalBoard:  //横板
	case BUT_TopBoard:
	case BUT_BottomBoard:
	case BUT_MobileBoard:
	case BUT_VerticalBoard:   //竖板
	case BUT_LeftBoard:
	case BUT_RightBoard:
	case BUT_RevealBoard:
	case BUT_ExposureBoard:
	case BUT_AdjustBoard:
		return GetShapePosY() + GetShapeDepth();
	case BUT_BackBoard:   //背板
	case BUT_FrontBoard:
	case BUT_EnhanceBoard:
	case BUT_ConvergentBoard:
	case BUT_SkirtingBoard:
		return GetShapePosY() + GetShapeHeight();
	default:
		return 0.0f;
	}
}

float FBoardShape::GetShapePosTopSide()
{
	switch (ShapeCategory)
	{
	case BUT_HorizontalBoard:  //横板
	case BUT_TopBoard:
	case BUT_BottomBoard:
	case BUT_MobileBoard:
		return GetShapePosZ() + GetShapeHeight();
	case BUT_VerticalBoard:   //竖板
	case BUT_LeftBoard:
	case BUT_RightBoard:
	case BUT_RevealBoard:
	case BUT_ExposureBoard:
	case BUT_AdjustBoard:
	case BUT_BackBoard:   //背板
	case BUT_FrontBoard:
	case BUT_EnhanceBoard:
	case BUT_ConvergentBoard:
	case BUT_SkirtingBoard:
		return GetShapePosZ() + GetShapeWidth();
	default:
		return 0.0f;
	}
}

bool FBoardShape::SetShapePosLeftSide(float InValue)
{
	bool bResult = false;
	switch (ShapeCategory)
	{
	case BUT_HorizontalBoard:   // 横板
	case BUT_TopBoard:
	case BUT_BottomBoard:
	case BUT_MobileBoard:
	case BUT_BackBoard:   // 背板
	case BUT_FrontBoard:
	case BUT_EnhanceBoard:
	case BUT_ConvergentBoard:
	case BUT_SkirtingBoard:
	{
		bResult = SetShapePosX(InValue);
		break;
	}
	case BUT_VerticalBoard:   // 竖板
	case BUT_LeftBoard:
	case BUT_RightBoard:
	case BUT_RevealBoard:
	case BUT_ExposureBoard:
	case BUT_AdjustBoard:
	{
		float BoardHeight = GetShapeHeight();
		bResult = SetShapePosX(InValue + BoardHeight);
	}
	default:
		break;
	}
	return bResult;
}
bool FBoardShape::SetShapePosRightSide(float InValue)
{
	bool bResult = false;
	switch (ShapeCategory)
	{
	case BUT_HorizontalBoard:   // 横板
	case BUT_TopBoard:
	case BUT_BottomBoard:
	case BUT_MobileBoard:
	{
		float PosX = GetShapePosX();
		if (InValue > PosX)
		{
			bResult = SetShapeWidth(InValue - PosX);
		}
		break;
	}
	case BUT_BackBoard:   // 背板
	case BUT_FrontBoard:
	case BUT_EnhanceBoard:
	case BUT_ConvergentBoard:
	case BUT_SkirtingBoard:
	{
		float PosX = GetShapePosX();
		if (InValue > PosX)
		{
			bResult = SetShapeDepth(InValue - PosX);
		}
		break;
	}
	case BUT_VerticalBoard:   // 竖板
	case BUT_LeftBoard:
	case BUT_RightBoard:
	case BUT_RevealBoard:
	case BUT_ExposureBoard:
	case BUT_AdjustBoard:
	default:
		check(false);
		break;
	}
	return bResult;
}
bool FBoardShape::SetShapePosFrontSide(float InValue)
{
	bool bResult = false;
	switch (ShapeCategory)
	{
	case BUT_HorizontalBoard:   // 横板
	case BUT_TopBoard:
	case BUT_BottomBoard:
	case BUT_MobileBoard:
	case BUT_VerticalBoard:   // 竖板
	case BUT_LeftBoard:
	case BUT_RightBoard:
	case BUT_RevealBoard:
	case BUT_ExposureBoard:
	case BUT_AdjustBoard:
	{
		float PosY = GetShapePosY();
		if (InValue > PosY)
		{
			bResult = SetShapeDepth(InValue - PosY);
		}
		break;
	}
	case BUT_BackBoard:   // 背板
	case BUT_FrontBoard:
	case BUT_EnhanceBoard:
	case BUT_ConvergentBoard:
	case BUT_SkirtingBoard:
	default:
		check(false);
		break;
	}
	return bResult;
}
bool FBoardShape::SetShapePosTopSide(float InValue)
{
	bool bResult = false;
	switch (ShapeCategory)
	{
	case BUT_VerticalBoard:   // 竖板
	case BUT_LeftBoard:
	case BUT_RightBoard:
	case BUT_RevealBoard:
	case BUT_ExposureBoard:
	case BUT_AdjustBoard:
	case BUT_BackBoard:   // 背板
	case BUT_FrontBoard:
	case BUT_EnhanceBoard:
	case BUT_ConvergentBoard:
	case BUT_SkirtingBoard:
	{
		float PosZ = GetShapePosZ();
		if (InValue > PosZ)
		{
			bResult = SetShapeWidth(InValue - PosZ);
		}
		break;
	}
	case BUT_HorizontalBoard:   // 横板
	case BUT_TopBoard:
	case BUT_BottomBoard:
	case BUT_MobileBoard:
	default:
		check(false);
		break;
	}
	return bResult;
}

const TSharedPtr<FShapeAttribute>& FBoardShape::GetSubstrateListAttri() const
{
	return SubstrateList;
}

bool FBoardShape::SetEdgeBandingData(const FPakMetaData& InData)
{
	EdgeBandingData = InData;
	FString StrValue = FString::FromInt(EdgeBandingData.ID);
	bool bResult = EdgeMaterialList->SetAttributeValue(StrValue);
	if (bResult)
	{
		ASCTBoardActor * Actor = Cast<ASCTBoardActor>(ShapeActor);
		if (Actor)
		{
			Actor->UpdateEdgeMaterial();			
		}
	}
	return bResult;
}

bool FBoardShape::SetMaterialData(const FPakMetaData & InData)
{
	MaterialData = InData;
	ASCTBoardActor * Actor = Cast<ASCTBoardActor>(ShapeActor);	
	if (Actor)
	{
		ShapeActor->UpdateActorBoardMaterial();		
		if (bUsingAluminumFoil)
		{
			Actor->UpdateAluminumFildMaterial();
		}
	}
	return true;
}

const FPakMetaData & FBoardShape::GetEdgeBandingData() const
{
	return EdgeBandingData;
}

bool FBoardShape::SetCoveredEdgeMaterial(const FPakMetaData & InData)
{
	CoverBandingData = InData;
	LightEdgeIndexArray = GetShapeDimensionMatchingTranslucent();

	FString StrValue = FString::FromInt(CoverBandingData.ID);
	bool bResult = CoverdEdgeList->SetAttributeValue(StrValue);
	if (bResult)
	{
		ASCTBoardActor * Actor = Cast<ASCTBoardActor>(ShapeActor);
		if (!Actor)
		{
			return false;
		}
		Actor->UpdateActorCoveredEdgeMaterial();
	}
	return bResult;
}

const FPakMetaData & FBoardShape::GetCoveredEdgeMaterialValue() const
{	
	return CoverBandingData;
}

const TArray<int32> & FBoardShape::GetLightCoveredEdges() const
{
	return LightEdgeIndexArray;
}

const TSharedPtr<FShapeAttribute>& FBoardShape::GetCoveredEdgeMaterialTypeAttri() const
{
	return CoverdEdgeList;
}

void FBoardShape::SetUsingAluminumFild(bool InUsing)
{
	bUsingAluminumFoil = InUsing;	
	ASCTBoardActor * Actor = Cast<ASCTBoardActor>(ShapeActor);
	if (Actor)
	{
		Actor->UpdateAluminumFildMaterial();
	}
}

const TSharedPtr<FShapeAttribute>& FBoardShape::GetEdgeMaterialTypeAttri() const
{
	return EdgeMaterialList;
}

float FBoardShape::GetShapeParamValue(int32 Index)
{
	check(Index < ShapeParameters.Num());
	check(ShapeParameters[Index]->GetAttributeType() == SAT_NumberRange);
	TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(ShapeParameters[Index]);
	return NumberRangeAttri->GetNumberValue();
}

float FBoardShape::GetShapeParamValueByRefName(const FString &InRefName)
{
	for (int32 i = 0; i < ShapeParameters.Num(); ++i)
	{
		if (ShapeParameters[i]->GetRefName() == InRefName)
			return GetShapeParamValue(i);
	}
	return 0.0f;
}

bool FBoardShape::AddShapeParamAttri(TArray<TSharedPtr<FShapeAttribute>> InAttriArray)
{
	for (const auto & Ref : InAttriArray)
	{
		if (GetShapeParamAttriByRefName(Ref->GetRefName()).IsValid())
		{
			continue;
		}
		ShapeParameters.Add(Ref);
	}		
	return true;
}

const TSharedPtr<FShapeAttribute>& FBoardShape::GetShapeParamAttri(int32 Index) const
{
	check(Index < ShapeParameters.Num());
	return ShapeParameters[Index];
}

TSharedPtr<FShapeAttribute> FBoardShape::GetShapeParamAttriByRefName(const FString &InRefName) const
{
	for (int32 i = 0; i < ShapeParameters.Num(); ++i)
	{
		if (ShapeParameters[i]->GetRefName() == InRefName)
			return GetShapeParamAttri(i);
	}
	return TSharedPtr<FShapeAttribute>();
}

bool FBoardShape::SetShapeParamValue(int32 Index, float InValue)
{
	return SetShapeParamValue(Index, FString::Printf(TEXT("%f"), InValue));
}

bool FBoardShape::SetShapeParamValueByRefName(const FString &InRefName, float InValue)
{
	for (int32 i = 0; i < ShapeParameters.Num(); ++i)
	{
		if (ShapeParameters[i]->GetRefName() == InRefName)
			return SetShapeParamValue(i, InValue);
	}
	return false;
}
bool FBoardShape::SetShapeParamValue(int32 Index, const FString &InValue)
{
	check(Index < ShapeParameters.Num());

	FString OldParamStr = ShapeParameters[Index]->GetAttributeStr();
	bool bResult = ShapeParameters[Index]->SetAttributeValue(InValue);
	//验证是否与板件尺寸冲突
	if (bResult)
	{
		float ParamValue = GetShapeParamValue(Index);
		switch (GetBoardShapeType())
		{
		case BoST_LeftCut:
		case BoST_RightCut:
		case BoST_LeftCorner:
		case BoST_RightCorner:
		{
			if (Index == 0)
			{
				float BoardWidth = GetShapeWidth();
				if (ParamValue >= BoardWidth)
				{
					bResult = false;
				}
			} 
			else
			{
				float BoardDepth = GetShapeDepth();
				if (ParamValue >= BoardDepth)
				{
					bResult = false;
				}
			}
			break;
		}
		case BoST_LeftOblique:
		case BoST_RightOblique:
		{
			if (Index == 0)
			{
				float BoardWidth = GetShapeWidth();
				if (ParamValue > BoardWidth)
				{
					bResult = false;
				}
			}
			else
			{
				float BoardDepth = GetShapeDepth();
				if (ParamValue > BoardDepth)
				{
					bResult = false;
				}
			}
			break;
		}
		case BoST_LeftOutArc:
		case BoST_RightOutArc:
		case BoST_LeftInArc:
		case BoST_RightInArc:
		{
			if (Index == 0)
			{
				float BoardWidth = GetShapeWidth();
				float BoardDepth = GetShapeDepth();
				if (ParamValue >= BoardDepth || ParamValue >= BoardWidth)
				{
					bResult = false;
				}
			}
			else if (Index == 1)
			{
				if (ParamValue <= 0.0)
				{
					bResult = false;
				}
			}
			break;
		}
		case BoST_LeftCornerCut:
		case BoST_RightCornerCut:
		{
			if (Index == 0 || Index == 2)
			{
				float ParamValue2 = 0.0;
				if (Index == 0)
				{
					ParamValue2 = GetShapeParamValue(2);
				} 
				else if (Index == 2)
				{
					ParamValue2 = GetShapeParamValue(0);
				}
				float BoardWidth = GetShapeWidth();
				if (ParamValue + ParamValue2 >= BoardWidth)
				{
					bResult = false;
				}
			} 
			else
			{
				float ParamValue2 = 0.0;
				if (Index == 1)
				{
					ParamValue2 = GetShapeParamValue(3);
				}
				else if (Index == 3)
				{
					ParamValue2 = GetShapeParamValue(1);
				}
				float BoardDepth = GetShapeDepth();
				if (ParamValue + ParamValue2 >= BoardDepth)
				{
					bResult = false;
				}
			}
			break;
		}
		case BoST_LeftInArcCut:
		case BoST_RightInArcCut:
		{
			if (Index == 0)
			{
				float BoardWidth = GetShapeWidth();
				float BoardDepth = GetShapeDepth();
				if (ParamValue >= BoardDepth || ParamValue >= BoardWidth)
				{
					bResult = false;
				}
			}
			else if (Index == 2)
			{
				float BoardWidth = GetShapeWidth();
				float ParamValue2 = GetShapeParamValue(0);
				if (ParamValue + ParamValue2 >= BoardWidth)
				{
					bResult = false;
				}
			} 
			else if (Index == 3)
			{
				float BoardDepth = GetShapeDepth();
				//float ParamValue2 = GetShapeParamValue(0);
				if (ParamValue/* + ParamValue2*/ >= BoardDepth)
				{
					bResult = false;
				}
			}			
			break;
		}
		case BoST_InnerRectangleHole:
		{
			bResult = CheckInnerRectHoleValied();
			break;
		}
		case BoST_InnerCircleHole:
		{
			bResult = CheckInnerCircleHoleValied();
			break;
		}
		case BoST_LeftRightOutArc:
		{
			if (Index == 0 || Index == 2)
			{
				float BoardWidth = GetShapeWidth();
				float BoardDepth = GetShapeDepth();
				float ParamValue0 = GetShapeParamValue(0);
				float ParamValue2 = GetShapeParamValue(2);
				if (ParamValue0 > BoardDepth || ParamValue2 > BoardDepth || ParamValue0 + ParamValue2 > BoardWidth)
				{
					bResult = false;
				}
			}
			else if (Index == 1 || Index == 3)
			{
				if (ParamValue <= 0.0)
				{
					bResult = false;
				}
			}
			break;
		}
		default:
			break;
		}

		//若出现冲突，恢复形状参数的原来属性值
		if (!bResult)
		{
			ShapeParameters[Index]->SetAttributeValue(OldParamStr);
		}
	}

	//若Actor已生成，同时更新Actor
	if (bResult && ShapeActor)
	{
		ShapeActor->UpdateActorDimension();
	}
	return bResult;
}

bool FBoardShape::SetShapeParamValueByRefName(const FString &InRefName, const FString &InValue)
{
	for (int32 i = 0; i < ShapeParameters.Num(); ++i)
	{
		if (ShapeParameters[i]->GetRefName() == InRefName)
			return SetShapeParamValue(i, InValue);
	}
	return false;
}

void FBoardShape::CalcBoardPoint(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints)
{
	OutPoints.Empty();
	OutEdgePoints.Empty();
	//TODO:根据板件形状类型计算板件点位
	switch (BoardShapeType)
	{
	case BoST_Rectangle://矩形板
	case BoST_InnerCircleHole:	
	case BoST_InnerRectangleHole:
	{
		CalcRectanglePoints(OutEdgePoints,OutPoints);
		break;
	}	
	case BoST_LeftCut://左切角板
	{
		CalcLeftCutPoints(OutEdgePoints, OutPoints);
		break;
	}
	case BoST_RightCut://右切角板
	{
		CalcRightCutPoints(OutEdgePoints, OutPoints);
		break;
	}
	case BoST_LeftOblique://左五角板
	{
		CalcLeftObliquePoints(OutEdgePoints, OutPoints);
		break;
	}
	case BoST_RightOblique://右五角板
	{
		CalcRightObliquePoints(OutEdgePoints, OutPoints);
		break;
	}
	case BoST_LeftOutArc://左圆弧板
	{
		CalcLeftOutArcPoints(OutEdgePoints, OutPoints);
		break;
	}
	case BoST_RightOutArc://右圆弧板
	{
		CalcRightOutArcPoints(OutEdgePoints, OutPoints);
		break;
	}
	case BoST_LeftInArc://左圆弧转角板
	{
		CalcLeftInArcPoints(OutEdgePoints, OutPoints);
		break;
	}
	case BoST_RightInArc://右圆弧转角板
	{
		CalcRightInArcPoints(OutEdgePoints, OutPoints);
		break;
	}
	case BoST_LeftCorner://左转角板
	{
		CalcLeftCornerPoints(OutEdgePoints, OutPoints);
		break;
	}
	case BoST_RightCorner://右转角板
	{
		CalcRightCornerPoints(OutEdgePoints, OutPoints);
		break;
	}
	case BoST_LeftCornerCut://左转角切角板
	{
		CalcLeftCornerCutPoints(OutEdgePoints, OutPoints);
		break;
	}
	case BoST_RightCornerCut://右转角切角板
	{
		CalcRightCornerCutPoints(OutEdgePoints, OutPoints);
		break;
	}
	case BoST_LeftInArcCut://左转角切角板
	{
		CalcLeftInArcCutPoints(OutEdgePoints, OutPoints);
		break;
	}
	case BoST_RightInArcCut://右转角切角板
	{
		CalcRightInArcCutPoints(OutEdgePoints, OutPoints);
		break;
	}
	case BoST_LeftRightOutArc: //双圆弧板
	{
		CalcLeftRightOutArcPoints(OutEdgePoints, OutPoints);
		break;
	}
	default:
		break;
	}
}
void FBoardShape::CalcBoardHolePoints(TArray<TArray<FVector>>& OutPoints)
{
	OutPoints.Empty();
	if (BoardShapeType != BoST_InnerCircleHole &&
		BoardShapeType != BoST_InnerRectangleHole)
	{
		return;
	}

	switch (BoardShapeType)
	{
		case BoST_InnerRectangleHole:
		{
			// 绘制矩形--依据左里角点以及宽度与深度
			check(ShapeParameters.Num() >= 4 && ShapeParameters.Num() % 4 == 0);
			const INT32 HoleCount = ShapeParameters.Num() / 4;
			for (INT32 Index = 0; Index < HoleCount; ++Index)
			{
				float XPos = GetShapeParamValue(Index * 4 );
				float YPos = GetShapeParamValue(Index * 4 + 1);
				float Width = GetShapeParamValue(Index * 4 + 2);
				float Depth = GetShapeParamValue(Index * 4 + 3);

				TArray<FVector> TempArray;

				FVector NewPoint0;
				NewPoint0.X = XPos;
				NewPoint0.Y = YPos;
				NewPoint0.Z = 0.0;
				TempArray.Emplace(NewPoint0);
				FVector NewPoint1;
				NewPoint1.X = XPos + Width;
				NewPoint1.Y = YPos;
				NewPoint1.Z = 0.0;
				TempArray.Emplace(NewPoint1);
				FVector NewPoint2;
				NewPoint2.X = XPos + Width;
				NewPoint2.Y = YPos + Depth;
				NewPoint2.Z = 0.0;
				TempArray.Emplace(NewPoint2);
				FVector NewPoint3;
				NewPoint3.X = XPos;
				NewPoint3.Y = YPos + Depth;
				NewPoint3.Z = 0.0;
				TempArray.Emplace(NewPoint3);
				OutPoints.Emplace(TempArray);
			}
			
		}
		break;
		case BoST_InnerCircleHole:
		{
			// 绘制圆 -- 依据圆心与半径
			float XPos = GetShapeParamValueByRefName(TEXT("CHX"));
			float YPos = GetShapeParamValueByRefName(TEXT("CHY"));
			float R = GetShapeParamValueByRefName(TEXT("CHR"));	
			FVector Center = FVector(XPos,YPos,0.0f);						
			FVector RotVec = FVector(0.0f, 1.0f, 0.0f).GetSafeNormal();
			const uint16 SampleNum = 36;
			float Delta = (HALF_PI * 4.0f) / SampleNum;
			FVector AxisVec = FVector::UpVector;
			TArray<FVector> TempArray;
			for (int32 i = 0; i < SampleNum; ++i)
			{
				FQuat qRot(AxisVec, Delta * i);
				FVector tempVec = qRot.RotateVector(RotVec);
				TempArray.Emplace(Center + tempVec * R);
			}
			OutPoints.Emplace(TempArray);
		}
		break;
		default:
			break;
	}
}

void FBoardShape::ReBuildEdge(TArray<TArray<FVector>>& OutOuterEdgePoints, TArray<TArray<FVector>>& OutInnerHolesPoints)
{
	if (BoardShapeType != BoST_InnerRectangleHole)	return;
	// 暂时只对两个洞的做处理 
	if (ShapeParameters.Num() != 8 && ShapeParameters.Num() != 4) return;
	OutOuterEdgePoints.Empty();
	OutInnerHolesPoints.Empty();

	TArray<TArray<FVector>> OuterEdgePoints;
	TArray<FVector>  OuterPoints;
	CalcRectanglePoints(OuterEdgePoints, OuterPoints);
	
	TArray<TArray<FVector>> InnerHolesPoints;	
	CalcBoardHolePoints(InnerHolesPoints);

	bool bFirstHoleOk = false;
	bool bSecondHoleOk = false;
	// 进行第一个方洞与外轮廓线的检查
	TArray<FVector>	TestVertexArray;
	UGTEngineLibrary::CalPloygonDiffenceSet(OuterPoints, InnerHolesPoints[0], TestVertexArray);
	// 证明第一个洞完全处于外轮廓的内部，且与外轮廓没有边的重叠
	if (TestVertexArray.Num() == 4)
	{
		bFirstHoleOk = true;
	}
	if (ShapeParameters.Num() == 8)
	{
		const int32 CurrentVertexNum = TestVertexArray.Num();
		UGTEngineLibrary::CalPloygonDiffenceSet(TestVertexArray, InnerHolesPoints[1], TestVertexArray);
		if (CurrentVertexNum == TestVertexArray.Num())
		{
			bSecondHoleOk = true;
		}
	}
	// 构建边
	for (int32 i = 0; i < TestVertexArray.Num() - 1; ++i)
	{
		TArray<FVector> NewLine = { TestVertexArray[i],TestVertexArray[i + 1] };
		OutOuterEdgePoints.Emplace(NewLine);
	}
	TArray<FVector> NewLine = { TestVertexArray[TestVertexArray.Num() - 1],TestVertexArray[0] };
	OutOuterEdgePoints.Emplace(NewLine);
	if (bFirstHoleOk)
	{
		OutInnerHolesPoints.Emplace(InnerHolesPoints[0]);
    }
	if (bSecondHoleOk)
	{
		OutInnerHolesPoints.Emplace(InnerHolesPoints[1]);
	}

}

void FBoardShape::SetBoardOutline(TSharedPtr<FSCTOutline> InOutline)
{
	OutlineData = InOutline;
	if (ShapeActor)
	{
		ShapeActor->UpdateActorDimension();
	}
}
TSharedPtr<FSCTOutline> FBoardShape::GetBoardOutline()
{
	return OutlineData;
}

//设置左延伸
bool FBoardShape::SetShapeLeftExpand(const float InIntValue)
{
	//设置外延值
	switch (GetShapeCategory())
	{
	case BUT_TopBoard://顶板和底板
	case BUT_BottomBoard:
		LeftExtension = InIntValue;
		break;
	case BUT_LeftBoard://左板和右板
	case BUT_RightBoard:
		break;
	case BUT_FrontBoard://前板和背板
	case BUT_BackBoard:
		BackExtension = InIntValue;
		break;
	default:
		//其他板件不存在外延值
		break;
	}

	//更新板件Actor
	if (ShapeActor)
	{
		ShapeActor->UpdateActorDimension();//根据板件维度与延伸值更新板件位置
		ShapeActor->UpdateActorPosition();//根据板件位置值与延伸值更新板件位置
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

	return true;
}
float FBoardShape::GetShapeLeftExpand()
{
	float OutResult = 0.0;
	switch (GetShapeCategory())
	{
	case BUT_TopBoard://顶板和底板
	case BUT_BottomBoard:
		OutResult = LeftExtension;
		break;
	case BUT_LeftBoard://左板和右板
	case BUT_RightBoard:
		break;
	case BUT_FrontBoard://前板和背板
	case BUT_BackBoard:
		OutResult = BackExtension;
		break;
	default:
		//其他板件不存在外延值
		break;
	}
	return OutResult;
}

bool FBoardShape::SetShapeRightExpand(const float InIntValue)
{
	//设置外延值
	switch (GetShapeCategory())
	{
	case BUT_TopBoard://顶板和底板
	case BUT_BottomBoard:
		RightExtension = InIntValue;
		break;
	case BUT_LeftBoard://左板和右板
	case BUT_RightBoard:
		break;
	case BUT_FrontBoard://前板和背板
	case BUT_BackBoard:
		FrontExtension = InIntValue;
		break;
	default:
		//其他板件不存在外延值
		break;
	}

	//更新板件Actor
	if (ShapeActor)
	{
		ShapeActor->UpdateActorDimension();//根据板件维度与延伸值更新板件位置
		ShapeActor->UpdateActorPosition();//根据板件位置值与延伸值更新板件位置
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

	return true;
}
float FBoardShape::GetShapeRightExpand()
{
	float OutResult = 0.0;
	switch (GetShapeCategory())
	{
	case BUT_TopBoard://顶板和底板
	case BUT_BottomBoard:
		OutResult = RightExtension;
		break;
	case BUT_LeftBoard://左板和右板
	case BUT_RightBoard:
		break;
	case BUT_FrontBoard://前板和背板
	case BUT_BackBoard:
		OutResult = FrontExtension;
		break;
	default:
		//其他板件不存在外延值
		break;
	}
	return OutResult;
}
bool FBoardShape::SetShapeTopExpand(const float InIntValue)
{
	//设置外延值
	switch (GetShapeCategory())
	{
	case BUT_TopBoard://顶板和底板
	case BUT_BottomBoard:
		break;
	case BUT_LeftBoard://左板和右板
	case BUT_RightBoard:
		RightExtension = InIntValue;
		break;
	case BUT_FrontBoard://前板和背板
	case BUT_BackBoard:
		RightExtension = InIntValue;
		break;
	default:
		//其他板件不存在外延值
		break;
	}

	//更新板件Actor
	if (ShapeActor)
	{
		ShapeActor->UpdateActorDimension();//根据板件维度与延伸值更新板件位置
		ShapeActor->UpdateActorPosition();//根据板件位置值与延伸值更新板件位置
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

	return true;
}
float FBoardShape::GetShapeTopExpand()
{
	float OutResult = 0.0;
	switch (GetShapeCategory())
	{
	case BUT_TopBoard://顶板和底板
	case BUT_BottomBoard:
		break;
	case BUT_LeftBoard://左板和右板
	case BUT_RightBoard:
		OutResult = RightExtension;
		break;
	case BUT_FrontBoard://前板和背板
	case BUT_BackBoard:
		OutResult = RightExtension;
		break;
	default:
		//其他板件不存在外延值
		break;
	}
	return OutResult;
}
bool FBoardShape::SetShapeBottomExpand(const float InIntValue)
{
	//设置外延值
	switch (GetShapeCategory())
	{
	case BUT_TopBoard://顶板和底板
	case BUT_BottomBoard:
		break;
	case BUT_LeftBoard://左板和右板
	case BUT_RightBoard:
		LeftExtension = InIntValue;
		break;
	case BUT_FrontBoard://前板和背板
	case BUT_BackBoard:
		LeftExtension = InIntValue;
		break;
	default:
		//其他板件不存在外延值
		break;
	}

	//更新板件Actor
	if (ShapeActor)
	{
		ShapeActor->UpdateActorDimension();//根据板件维度与延伸值更新板件位置
		ShapeActor->UpdateActorPosition();//根据板件位置值与延伸值更新板件位置
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

	return true;
}
float FBoardShape::GetShapeBottomExpand()
{
	float OutResult = 0.0;
	switch (GetShapeCategory())
	{
	case BUT_TopBoard://顶板和底板
	case BUT_BottomBoard:
		break;
	case BUT_LeftBoard://左板和右板
	case BUT_RightBoard:
		OutResult = LeftExtension;
		break;
	case BUT_FrontBoard://前板和背板
	case BUT_BackBoard:
		OutResult = LeftExtension;
		break;
	default:
		//其他板件不存在外延值
		break;
	}
	return OutResult;
}
bool FBoardShape::SetShapeFrontExpand(const float InIntValue)
{
	//设置外延值
	switch (GetShapeCategory())
	{
		case BUT_TopBoard://顶板和底板
		case BUT_BottomBoard:
			FrontExtension = InIntValue;
			break;
		case BUT_LeftBoard://左板和右板
		case BUT_RightBoard:
			FrontExtension = InIntValue;
			break;
		case BUT_FrontBoard://前板和背板
		case BUT_BackBoard:
			break;
		default:
			//其他板件不存在外延值
			break;
	}

	//更新板件Actor
	if (ShapeActor)
	{
		ShapeActor->UpdateActorDimension();//根据板件维度与延伸值更新板件位置
		ShapeActor->UpdateActorPosition();//根据板件位置值与延伸值更新板件位置
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

	return true;
}
float FBoardShape::GetShapeFrontExpand()
{
	float OutResult = 0.0;
	switch (GetShapeCategory())
	{
	case BUT_TopBoard://顶板和底板
	case BUT_BottomBoard:
		OutResult = FrontExtension;
		break;
	case BUT_LeftBoard://左板和右板
	case BUT_RightBoard:
		OutResult = FrontExtension;
		break;
	case BUT_FrontBoard://前板和背板
	case BUT_BackBoard:
		break;
	default:
		//其他板件不存在外延值
		break;
	}
	return OutResult;
}
bool FBoardShape::SetShapeBackExpand(const float InIntValue)
{
	//设置外延值
	switch (GetShapeCategory())
	{
	case BUT_TopBoard://顶板和底板
	case BUT_BottomBoard:
		BackExtension = InIntValue;
		break;
	case BUT_LeftBoard://左板和右板
	case BUT_RightBoard:
		BackExtension = InIntValue;
		break;
	case BUT_FrontBoard://前板和背板
	case BUT_BackBoard:
		break;
	default:
		//其他板件不存在外延值
		break;
	}

	//更新板件Actor
	if (ShapeActor)
	{
		ShapeActor->UpdateActorDimension();//根据板件维度与延伸值更新板件位置
		ShapeActor->UpdateActorPosition();//根据板件位置值与延伸值更新板件位置
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

	return true;
}
float FBoardShape::GetShapeBackExpand()
{
	float OutResult = 0.0;
	switch (GetShapeCategory())
	{
	case BUT_TopBoard://顶板和底板
	case BUT_BottomBoard:
		OutResult = BackExtension;
		break;
	case BUT_LeftBoard://左板和右板
	case BUT_RightBoard:
		OutResult = BackExtension;
		break;
	case BUT_FrontBoard://前板和背板
	case BUT_BackBoard:
		break;
	default:
		//其他板件不存在外延值
		break;
	}
	return OutResult;
}
//获取板件宽度方向外延值修正量
float FBoardShape::GetWidthStretch()
{
	return LeftExtension + RightExtension;
}
//获取板件深度方向外延值修正量
float FBoardShape::GetDepthStretch()
{
	return BackExtension + FrontExtension;
}

float FBoardShape::GetAxisXStretch()
{
	float Expand = 0;
	switch (GetShapeCategory())
	{
	case BUT_TopBoard://顶板和底板
	case BUT_BottomBoard:
		Expand = GetLeftExtension();
		break;
	case BUT_LeftBoard://左板和右板
	case BUT_RightBoard:
		break;
	case BUT_FrontBoard://前板和背板
	case BUT_BackBoard:
		Expand = GetBackExtension();
		break;
	default:
		//其他板件不存在内缩值
		break;
	}
	return -Expand;
}
float FBoardShape::GetAxisYStretch()
{
	float Expand = 0;
	switch (GetShapeCategory())
	{
	case BUT_TopBoard://顶板和底板
	case BUT_BottomBoard:
		Expand = GetBackExtension();
		break;
	case BUT_LeftBoard://左板和右板
	case BUT_RightBoard:
		Expand = GetBackExtension();
		break;
	case BUT_FrontBoard://前板和背板
	case BUT_BackBoard:
		break;
	default:
		//其他板件不存在内缩值
		break;
	}
	return -Expand;
}
float FBoardShape::GetAxisZStretch()
{
	float Expand = 0;
	switch (GetShapeCategory())
	{
	case BUT_TopBoard://顶板和底板
	case BUT_BottomBoard:
		break;
	case BUT_LeftBoard://左板和右板
	case BUT_RightBoard:
		Expand = GetLeftExtension();
		break;
	case BUT_FrontBoard://前板和背板
	case BUT_BackBoard:
		Expand = GetLeftExtension();
		break;
	default:
		//其他板件不存在内缩值
		break;
	}
	return -Expand;
}

//设置内缩值
bool FBoardShape::SetShapeContraction(float InValue)
{
	Contraction = InValue;
	UpdateAssociatedActors(2);
	UpdateChileShapes(0);
	return true;
}


bool FBoardShape::SetShapeConfluenceContraction(const float InValue)
{
	ConfluenceContraction = InValue;
	UpdateAssociatedActors(2);
	UpdateChileShapes(0);
	return true;
}

//获取板件内缩值（判定板件方向）
FVector FBoardShape::GetShapeContractionDire()
{
	FVector Contraction = FVector::ZeroVector;
	switch (GetShapeCategory())
	{
		case BUT_TopBoard://顶板和底板
			Contraction -= FVector::UpVector * (GetShapeContraction() + GetShapeConfluenceContraction());
			break;
		case BUT_BottomBoard:
			Contraction += FVector::UpVector * (GetShapeContraction() + GetShapeConfluenceContraction());
			break;
		case BUT_LeftBoard://左板和右板
			Contraction += FVector::ForwardVector * (GetShapeContraction() + GetShapeConfluenceContraction());
			break;
		case BUT_RightBoard:
			Contraction -= FVector::ForwardVector * (GetShapeContraction() + GetShapeConfluenceContraction());
			break;
		case BUT_FrontBoard://前板和背板
			Contraction -= FVector::RightVector * (GetShapeContraction() + GetShapeConfluenceContraction());
			break;
		case BUT_BackBoard:
			Contraction += FVector::RightVector * (GetShapeContraction() + GetShapeConfluenceContraction());
			break;
		default:
			//其他板件不存在内缩值
			break;
	}

	return Contraction;
}

//设置板件是否为活动板
void FBoardShape::SetBoardActive(bool bActive, float InValue/* = 18.0*/)
{
	bActiveBoard = bActive;	
	switch (GetShapeCategory())
	{
		case BUT_TopBoard:
		case BUT_BottomBoard:
		case BUT_MobileBoard:
		case BUT_HorizontalBoard:
		{
			bActiveBoard ? FrontExtension += -InValue : FrontExtension += ActiveRetract;
			//更新板件Actor
			if (ShapeActor)
			{
				ShapeActor->UpdateActorDimension();
				ShapeActor->UpdateActorPosition();
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
			break;
		}
		default:
			break;
	}
	ActiveRetract = InValue;
}
bool FBoardShape::IsBoardActive()
{
	return bActiveBoard;
}
void FBoardShape::SetActiveRetractValue(float InValue)
{
	ActiveRetract = InValue;
	if (!bActiveBoard)
	{
		return;
	}

	switch (GetShapeCategory())
	{
		case BUT_MobileBoard:
		case BUT_HorizontalBoard:
		{
			FrontExtension = -ActiveRetract;
			//更新板件Actor
			if (ShapeActor)
			{
				ShapeActor->UpdateActorDimension();
				ShapeActor->UpdateActorPosition();
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
			break;
	}
	default:
		break;
	}
}
float FBoardShape::GetActiveRetractValue()
{
	return ActiveRetract;
}

void FBoardShape::CalcRectanglePoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints)
{
	//板件尺寸
	float BWidth = GetShapeWidth() + GetWidthStretch();
	float BDepth = GetShapeDepth() + GetDepthStretch();

	//板件方向
	int32 BoardDirect = GetShapeCategory();

	//轮廓类型
	EOutlineType OutlineType = OutlineData->GetOutlineType();
	switch (OutlineType)
	{
		case OLT_None:
		{
			// 构建点
			OutPoints = {
				FVector(0.0, 0.0, 0.0),
				FVector(BWidth, 0.0, 0.0),
				FVector(BWidth, BDepth, 0.0),
				FVector(0.0, BDepth, 0.0)
			};
			break;
		}
		case OLT_LeftGirder:
		{
			float GWidth = OutlineData->GetOutlineParamValueByRefName(TEXT("GW"));
			float GHeight = OutlineData->GetOutlineParamValueByRefName(TEXT("GH"));

			switch (BoardDirect)
			{
				case 1:
				case 4:
				case 5:
				{
					float BPosZ = GetShapePosZ();
					float SHeight = OutlineData->GetSpaceHeight();
					if (BPosZ < SHeight - GHeight)
					{
						OutPoints = {
							FVector(0.0, 0.0, 0.0),
							FVector(BWidth, 0.0, 0.0),
							FVector(BWidth, BDepth, 0.0),
							FVector(0.0, BDepth, 0.0)
						};
					}
					else
					{
						OutPoints = {
							FVector(GWidth, 0.0, 0.0),
							FVector(BWidth, 0.0, 0.0),
							FVector(BWidth, BDepth, 0.0),
							FVector(GWidth, BDepth, 0.0)
						};
					}
					break;
			}
			case 3:
			case 8:
			{
				OutPoints = {
					FVector(0.0f, 0.0f, 0.0f),
					FVector(BWidth - GHeight, 0.0, 0.0f),
					FVector(BWidth - GHeight, GWidth, 0.0),
					FVector(BWidth, GWidth, 0.0),
					FVector(BWidth, BDepth, 0.0),
					FVector(0.0f, BDepth, 0.0)
				};
				break;
			}
			case 2:
			case 6:
			case 7:
			{
				float BPosX = GetShapePosX();
				if (BPosX > GWidth)
				{
					OutPoints = {
						FVector(0.0, 0.0, 0.0),
						FVector(BWidth, 0.0, 0.0),
						FVector(BWidth, BDepth, 0.0),
						FVector(0.0, BDepth, 0.0)
					};
				}
				else
				{
					OutPoints = {
						FVector(0.0, 0.0, 0.0),
						FVector(BWidth - GHeight, 0.0, 0.0),
						FVector(BWidth - GHeight, BDepth, 0.0),
						FVector(0.0, BDepth, 0.0)
					};
				}
				break;
			}
			default:
				check(false);
				break;
			}
			break;
		}
		case OLT_RightGirder:
		{
			float GWidth = OutlineData->GetOutlineParamValueByRefName(TEXT("GW"));
			float GHeight = OutlineData->GetOutlineParamValueByRefName(TEXT("GH"));

			switch (BoardDirect)
			{
				case 1:
				case 4:
				case 5:
				{
					float BPosZ = GetShapePosZ();
					float SHeight = OutlineData->GetSpaceHeight();
					if (BPosZ < SHeight - GHeight)
					{
						OutPoints = {
							FVector(0.0, 0.0, 0.0),
							FVector(BWidth, 0.0, 0.0),
							FVector(BWidth, BDepth, 0.0),
							FVector(0.0, BDepth, 0.0)
						};
					}
					else
					{
						OutPoints = {
							FVector(0.0, 0.0, 0.0),
							FVector(BWidth - GWidth, 0.0, 0.0),
							FVector(BWidth - GWidth, BDepth, 0.0),
							FVector(0.0, BDepth, 0.0)
						};
					}
					break;
				}
				case 3:
				case 8:
				{
					OutPoints = {
						FVector(0.0f, 0.0f, 0.0f),
						FVector(BWidth, 0.0, 0.0f),
						FVector(BWidth, BDepth - GWidth, 0.0),
						FVector(BWidth - GHeight, BDepth - GWidth, 0.0),
						FVector(BWidth - GHeight, BDepth, 0.0),
						FVector(0.0f, BDepth, 0.0)
					};
					break;
				}
				case 2:
				case 6:
				case 7:
				{
					float BPosX = GetShapePosX();
					float SWidth = OutlineData->GetSpaceWidth();
					if (BPosX > SWidth - GWidth)
					{
						OutPoints = {
							FVector(0.0, 0.0, 0.0),
							FVector(BWidth, 0.0, 0.0),
							FVector(BWidth, BDepth, 0.0),
							FVector(0.0, BDepth, 0.0)
						};
					}
					else
					{
						OutPoints = {
							FVector(0.0, 0.0, 0.0),
							FVector(BWidth - GHeight, 0.0, 0.0),
							FVector(BWidth - GHeight, BDepth, 0.0),
							FVector(0.0, BDepth, 0.0)
						};
					}
					break;
			}
			default:
				check(false);
				break;
			}
			break;
		}
		case OLT_BackGirder:
		{
			float GDepth = OutlineData->GetOutlineParamValueByRefName(TEXT("GD"));
			float GHeight = OutlineData->GetOutlineParamValueByRefName(TEXT("GH"));

			switch (BoardDirect)
			{
			case 1:
			case 4:
			case 5:
			{
				float BPosZ = GetShapePosZ();
				float SHeight = OutlineData->GetSpaceHeight();
				if (BPosZ < SHeight - GHeight)
				{
					OutPoints = {
						FVector(0.0, 0.0, 0.0),
						FVector(BWidth, 0.0, 0.0),
						FVector(BWidth, BDepth, 0.0),
						FVector(0.0, BDepth, 0.0)
					};
				}
				else
				{
					OutPoints = {
						FVector(0.0, GDepth, 0.0),
						FVector(BWidth, GDepth, 0.0),
						FVector(BWidth, BDepth, 0.0),
						FVector(0.0, BDepth, 0.0)
					};
				}
				break;
			}
			case 3:
			case 8:
			{
				float PosY = GetShapePosY();
				if (PosY > GDepth)
				{
					OutPoints = {
						FVector(0.0, 0.0, 0.0),
						FVector(BWidth, 0.0, 0.0),
						FVector(BWidth, BDepth, 0.0),
						FVector(0.0, BDepth, 0.0)
					};
				}
				else
				{
					OutPoints = {
						FVector(0.0, 0.0, 0.0),
						FVector(BWidth-GHeight, 0.0, 0.0),
						FVector(BWidth-GHeight, BDepth, 0.0),
						FVector(0.0, BDepth, 0.0)
					};
				}
				break;
			}
			case 2:
			case 6:
			case 7:
			{
				OutPoints = {
					FVector(0.0f, 0.0f, 0.0f),
					FVector(BWidth - GHeight, 0.0, 0.0f),
					FVector(BWidth - GHeight, GDepth, 0.0),
					FVector(BWidth, GDepth, 0.0),
					FVector(BWidth, BDepth, 0.0),
					FVector(0.0f, BDepth, 0.0)
				};
				break;
			}
			default:
				check(false);
				break;
			}
			break;
		}
		case OLT_LeftPiller:
		{
			float PWidth = OutlineData->GetOutlineParamValueByRefName(TEXT("PW"));
			float PDepth = OutlineData->GetOutlineParamValueByRefName(TEXT("PD"));

			switch (BoardDirect)
			{
			case 1:
			case 4:
			case 5:
			{
				OutPoints = {
					FVector(PWidth, 0.0f, 0.0f),
					FVector(BWidth, 0.0f, 0.0f),
					FVector(BWidth, BDepth, 0.0f),
					FVector(0.0f, BDepth, 0.0f),
					FVector(0.0f, PDepth, 0.0f),
					FVector(PWidth, PDepth, 0.0f)
				};
				break;
			}
			case 3:
			case 8:
			{
				float PosY = GetShapePosY();
				if (PosY > PDepth)
				{
					OutPoints = {
					FVector(0.0, 0.0, 0.0),
					FVector(BWidth, 0.0, 0.0),
					FVector(BWidth, BDepth, 0.0),
					FVector(0.0, BDepth, 0.0)
					};
				}
				else
				{
					OutPoints = {
						FVector(0.0, PDepth, 0.0),
						FVector(BWidth, PDepth, 0.0),
						FVector(BWidth, BDepth, 0.0),
						FVector(0.0, BDepth, 0.0)
					};
				}
				break;
			}
			case 2:
			case 6:
			case 7:
			{
				float PosX = GetShapePosX();
				if (PosX > PWidth)
				{
					OutPoints = {
						FVector(0.0, 0.0, 0.0),
						FVector(BWidth, 0.0, 0.0),
						FVector(BWidth, BDepth, 0.0),
						FVector(0.0, BDepth, 0.0)
					};
				}
				else
				{
					OutPoints = {
						FVector(0.0, PDepth, 0.0),
						FVector(BWidth, PDepth, 0.0),
						FVector(BWidth, BDepth, 0.0),
						FVector(0.0, BDepth, 0.0)
					};
				}
				break;
			}
			default:
				check(false);
				break;
			}
			break;
		}
		case OLT_RightPiller:
		{
			float PWidth = OutlineData->GetOutlineParamValueByRefName(TEXT("PW"));
			float PDepth = OutlineData->GetOutlineParamValueByRefName(TEXT("PD"));

			switch (BoardDirect)
			{
			case 1:
			case 4:
			case 5:
			{
				OutPoints = {
					FVector(0.0f, 0.0f, 0.0f),
					FVector(BWidth - PWidth, 0.0f, 0.0f),
					FVector(BWidth - PWidth, PDepth, 0.0f),
					FVector(BWidth, PDepth, 0.0f),
					FVector(BWidth, BDepth, 0.0f),
					FVector(0.0f, BDepth, 0.0f)
				};
				break;
			}
			case 3:
			case 8:
			{
				float PosY = GetShapePosY();
				if (PosY > PDepth)
				{
					OutPoints = {
						FVector(0.0, 0.0, 0.0),
						FVector(BWidth, 0.0, 0.0),
						FVector(BWidth, BDepth, 0.0),
						FVector(0.0, BDepth, 0.0)
					};
				}
				else
				{
					OutPoints = {
						FVector(0.0, 0.0, 0.0),
						FVector(BWidth, 0.0, 0.0),
						FVector(BWidth, BDepth-PDepth, 0.0),
						FVector(0.0, BDepth-PDepth, 0.0)
					};
				}
				break;
			}
			case 2:
			case 6:
			case 7:
			{
				float PosX = GetShapePosX();
				if (PosX < BWidth - PWidth)
				{
					OutPoints = {
						FVector(0.0, 0.0, 0.0),
						FVector(BWidth, 0.0, 0.0),
						FVector(BWidth, BDepth, 0.0),
						FVector(0.0, BDepth, 0.0)
					};
				}
				else
				{
					OutPoints = {
						FVector(0.0, PDepth, 0.0),
						FVector(BWidth, PDepth, 0.0),
						FVector(BWidth, BDepth, 0.0),
						FVector(0.0, BDepth, 0.0)
					};
				}
				break;
			}
			default:
				check(false);
				break;
			}
			break;
		}
		case OLT_MiddlePiller:
		{
			float PPosX = OutlineData->GetOutlineParamValueByRefName(TEXT("PX"));
			float PWidth = OutlineData->GetOutlineParamValueByRefName(TEXT("PW"));
			float PDepth = OutlineData->GetOutlineParamValueByRefName(TEXT("PD"));

			switch (BoardDirect)
			{
			case 1:
			case 4:
			case 5:
			{
				OutPoints = {
					FVector(0.0f, 0.0f, 0.0f),
					FVector(PPosX, 0.0f, 0.0f),
					FVector(PPosX, PDepth, 0.0f),
					FVector(PPosX + PWidth, PDepth, 0.0f),
					FVector(PPosX + PWidth, 0.0f, 0.0f),
					FVector(BWidth, 0.0f, 0.0f),
					FVector(BWidth, BDepth, 0.0f),
					FVector(0.0f, BDepth, 0.0f)
				};
				break;
			}
			case 3:
			case 8:
			{
				float BPosY = GetShapePosY();
				if (BPosY > PDepth)
				{
					OutPoints = {
						FVector(0.0, 0.0, 0.0),
						FVector(BWidth, 0.0, 0.0),
						FVector(BWidth, BDepth, 0.0),
						FVector(0.0, BDepth, 0.0)
					};
				}
				else
				{
					OutPoints = {
						FVector(0.0, 0.0, 0.0),
						FVector(BWidth, 0.0, 0.0),
						FVector(BWidth, PPosX, 0.0),
						FVector(0.0, PPosX, 0.0)
					};
				}
				break;
			}
			case 2:
			case 6:
			case 7:
			{
				float BPosX = GetShapePosX();
				float SWidth = OutlineData->GetSpaceWidth();
				if ((BPosX < PPosX) || (BPosX > PPosX + PWidth))
				{
					OutPoints = {
						FVector(0.0, 0.0, 0.0),
						FVector(BWidth, 0.0, 0.0),
						FVector(BWidth, BDepth, 0.0),
						FVector(0.0, BDepth, 0.0)
					};
				}
				else
				{
					OutPoints = {
						FVector(0.0, PDepth, 0.0),
						FVector(BWidth, PDepth, 0.0),
						FVector(BWidth, BDepth, 0.0),
						FVector(0.0, BDepth, 0.0)
					};
				}
				break;
			}
			default:
				check(false);
				break;
			}
			break;
		}
		default:
			break;
	}

	// 构建边
	for (int32 i=0; i<OutPoints.Num()-1; ++i)
	{
		TArray<FVector> NewLine = {
			OutPoints[i],
			OutPoints[i+1]
		};
		OutEdgePoints.Emplace(NewLine);
	}
	TArray<FVector> NewLine = {
		OutPoints[OutPoints.Num() - 1],
		OutPoints[0]
	};
	OutEdgePoints.Emplace(NewLine);
}


void FBoardShape::CalcLeftCutPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints)
{
	check(ShapeParameters.Num() == 2);
	float CutWidth = GetShapeParamValueByRefName(TEXT("TW"));
	float CutDepth = GetShapeParamValueByRefName(TEXT("TD"));

	float Width = GetShapeWidth() + GetWidthStretch();
	float Depth = GetShapeDepth() + GetDepthStretch();

	// 构建点
	FVector NewPoint0;
	NewPoint0.X = CutWidth;
	NewPoint0.Y = CutDepth;
	NewPoint0.Z = 0.0;
	OutPoints.Emplace(NewPoint0);
	
	FVector NewPoint1;
	NewPoint1.X = CutWidth;
	NewPoint1.Y = 0.0;
	NewPoint1.Z = 0.0;
	OutPoints.Emplace(NewPoint1);
	
	FVector NewPoint2;
	NewPoint2.X = Width;
	NewPoint2.Y = 0.0;
	NewPoint2.Z = 0.0;
	OutPoints.Emplace(NewPoint2);
	
	FVector NewPoint3;
	NewPoint3.X = Width;
	NewPoint3.Y = Depth;
	NewPoint3.Z = 0.0;
	OutPoints.Emplace(NewPoint3);
	
	FVector NewPoint4;
	NewPoint4.X = 0.0;
	NewPoint4.Y = Depth;
	NewPoint4.Z = 0.0;
	OutPoints.Emplace(NewPoint4);
	
	FVector NewPoint5;
	NewPoint5.X = 0.0;
	NewPoint5.Y = CutDepth;
	NewPoint5.Z = 0.0;
	OutPoints.Emplace(NewPoint5);

	// 构建边
	TArray<FVector> Line0;
	Line0.Emplace(NewPoint0);
	Line0.Emplace(NewPoint1);

	TArray<FVector> Line1;
	Line1.Emplace(NewPoint1);
	Line1.Emplace(NewPoint2);

	TArray<FVector> Line2;
	Line2.Emplace(NewPoint2);
	Line2.Emplace(NewPoint3);


	TArray<FVector> Line3;
	Line3.Emplace(NewPoint3);
	Line3.Emplace(NewPoint4);

	TArray<FVector> Line4;
	Line4.Emplace(NewPoint4);
	Line4.Emplace(NewPoint5);

	TArray<FVector> Line5;
	Line5.Emplace(NewPoint5);
	Line5.Emplace(NewPoint0);

	// 存储边
	OutEdgePoints.Emplace(Line0);
	OutEdgePoints.Emplace(Line1);
	OutEdgePoints.Emplace(Line2);
	OutEdgePoints.Emplace(Line3);
	OutEdgePoints.Emplace(Line4);
	OutEdgePoints.Emplace(Line5);
	
}

void FBoardShape::CalcRightCutPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints)
{
	check(ShapeParameters.Num() == 2);
	float CutWidth = GetShapeParamValueByRefName(TEXT("TW"));
	float CutDepth = GetShapeParamValueByRefName(TEXT("TD"));

	float Width = GetShapeWidth() + GetWidthStretch();
	float Depth = GetShapeDepth() + GetDepthStretch();
	// 构建点
	FVector NewPoint0;
	NewPoint0.X = 0.0;
	NewPoint0.Y = 0.0;
	NewPoint0.Z = 0.0;
	OutPoints.Emplace(NewPoint0);
	
	FVector NewPoint1;
	NewPoint1.X = Width - CutWidth;
	NewPoint1.Y = 0.0;
	NewPoint1.Z = 0.0;
	OutPoints.Emplace(NewPoint1);
	
	FVector NewPoint2;
	NewPoint2.X = Width - CutWidth;
	NewPoint2.Y = CutDepth;
	NewPoint2.Z = 0.0;
	OutPoints.Emplace(NewPoint2);
	
	FVector NewPoint3;
	NewPoint3.X = Width;
	NewPoint3.Y = CutDepth;
	NewPoint3.Z = 0.0;
	OutPoints.Emplace(NewPoint3);
	
	FVector NewPoint4;
	NewPoint4.X = Width;
	NewPoint4.Y = Depth;
	NewPoint4.Z = 0.0;
	OutPoints.Emplace(NewPoint4);
	
	FVector NewPoint5;
	NewPoint5.X = 0.0;
	NewPoint5.Y = Depth;
	NewPoint5.Z = 0.0;
	OutPoints.Emplace(NewPoint5);

	// 构建边
	TArray<FVector> Line0;
	Line0.Emplace(NewPoint0);
	Line0.Emplace(NewPoint1);

	TArray<FVector> Line1;
	Line1.Emplace(NewPoint1);
	Line1.Emplace(NewPoint2);

	TArray<FVector> Line2;
	Line2.Emplace(NewPoint2);
	Line2.Emplace(NewPoint3);

	TArray<FVector> Line3;
	Line3.Emplace(NewPoint3);
	Line3.Emplace(NewPoint4);

	TArray<FVector> Line4;
	Line4.Emplace(NewPoint4);
	Line4.Emplace(NewPoint5);

	TArray<FVector> Line5;	
	Line5.Emplace(NewPoint5);
	Line5.Emplace(NewPoint0);
	
	// 存储边
	OutEdgePoints.Emplace(Line0);
	OutEdgePoints.Emplace(Line1);
	OutEdgePoints.Emplace(Line2);
	OutEdgePoints.Emplace(Line3);
	OutEdgePoints.Emplace(Line4);
	OutEdgePoints.Emplace(Line5);
}

void FBoardShape::CalcLeftObliquePoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints)
{

	check(ShapeParameters.Num() == 2);
	float ObliqueWidth = GetShapeParamValueByRefName(TEXT("SW"));
	float ObliqueDepth = GetShapeParamValueByRefName(TEXT("SD"));

	float Width = GetShapeWidth() + GetWidthStretch();
	float Depth = GetShapeDepth() + GetDepthStretch();

	// 构建点
	FVector NewPoint0;
	NewPoint0.X = 0.0;
	NewPoint0.Y = 0.0;
	NewPoint0.Z = 0.0;
	OutPoints.Emplace(NewPoint0);
	
	FVector NewPoint1;
	NewPoint1.X = Width;
	NewPoint1.Y = 0.0;
	NewPoint1.Z = 0.0;
	OutPoints.Emplace(NewPoint1);
	
	FVector NewPoint2;
	NewPoint2.X = Width;
	NewPoint2.Y = Depth - ObliqueDepth;
	NewPoint2.Z = 0.0;
	OutPoints.Emplace(NewPoint2);
	
	FVector NewPoint3;
	NewPoint3.X = Width - ObliqueWidth;
	NewPoint3.Y = Depth;
	NewPoint3.Z = 0.0;
	OutPoints.Emplace(NewPoint3);
	
	FVector NewPoint4;
	NewPoint4.X = 0.0;
	NewPoint4.Y = Depth;
	NewPoint4.Z = 0.0;
	OutPoints.Emplace(NewPoint4);
	
	// 构建边
	TArray<FVector> Line0;
	Line0.Emplace(NewPoint0);
	Line0.Emplace(NewPoint1);

	TArray<FVector> Line1;
	Line1.Emplace(NewPoint1);
	Line1.Emplace(NewPoint2);

	TArray<FVector> Line2;
	Line2.Emplace(NewPoint2);
	Line2.Emplace(NewPoint3);

	TArray<FVector> Line3;
	Line3.Emplace(NewPoint3);
	Line3.Emplace(NewPoint4);

	TArray<FVector> Line4;
	Line4.Emplace(NewPoint4);
	Line4.Emplace(NewPoint0);

	// 存储
	OutEdgePoints.Emplace(Line0);
	OutEdgePoints.Emplace(Line1);
	OutEdgePoints.Emplace(Line2);
	OutEdgePoints.Emplace(Line3);
	OutEdgePoints.Emplace(Line4);

}

void FBoardShape::CalcRightObliquePoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints)
{
	check(ShapeParameters.Num() == 2);
	float ObliqueWidth = GetShapeParamValueByRefName(TEXT("SW"));
	float ObliqueDepth = GetShapeParamValueByRefName(TEXT("SD"));

	float Width = GetShapeWidth() + GetWidthStretch();
	float Depth = GetShapeDepth() + GetDepthStretch();

	FVector NewPoint0;
	NewPoint0.X = 0.0;
	NewPoint0.Y = 0.0;
	NewPoint0.Z = 0.0;
	OutPoints.Emplace(NewPoint0);

	FVector NewPoint1;
	NewPoint1.X = Width;
	NewPoint1.Y = 0.0;
	NewPoint1.Z = 0.0;
	OutPoints.Emplace(NewPoint1);

	FVector NewPoint2;
	NewPoint2.X = Width;
	NewPoint2.Y = Depth;
	NewPoint2.Z = 0.0;
	OutPoints.Emplace(NewPoint2);

	FVector NewPoint3;
	NewPoint3.X = ObliqueWidth;
	NewPoint3.Y = Depth;
	NewPoint3.Z = 0.0;
	OutPoints.Emplace(NewPoint3);

	FVector NewPoint4;
	NewPoint4.X = 0.0;
	NewPoint4.Y = Depth - ObliqueDepth;
	NewPoint4.Z = 0.0;
	OutPoints.Emplace(NewPoint4);

	// 构建存储边
	TArray<FVector> Line0;
	Line0.Emplace(NewPoint0);
	Line0.Emplace(NewPoint1);
	OutEdgePoints.Emplace(Line0);

	TArray<FVector> Line1;
	Line1.Emplace(NewPoint1);
	Line1.Emplace(NewPoint2);
	OutEdgePoints.Emplace(Line1);

	TArray<FVector> Line2;
	Line2.Emplace(NewPoint2);
	Line2.Emplace(NewPoint3);
	OutEdgePoints.Emplace(Line2);

	TArray<FVector> Line3;
	Line3.Emplace(NewPoint3);
	Line3.Emplace(NewPoint4);
	OutEdgePoints.Emplace(Line3);

	TArray<FVector> Line4;
	Line4.Emplace(NewPoint4);
	Line4.Emplace(NewPoint0);
	OutEdgePoints.Emplace(Line4);
}

void FBoardShape::CalcLeftOutArcPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints)
{
	check(ShapeParameters.Num() == 2);
	float Length = GetShapeParamValueByRefName(TEXT("TLEN"));
	float ArcAlph = GetShapeParamValueByRefName(TEXT("ANGLE"));
	ArcAlph = FMath::Clamp<float>(ArcAlph, 0, 90);
	ArcAlph = PI * ArcAlph / 180.0;

	float Width = GetShapeWidth() + GetWidthStretch();
	float Depth = GetShapeDepth() + GetDepthStretch();

	FVector ArcPnt0;
	ArcPnt0.X = Width;
	ArcPnt0.Y = Depth - Length;
	ArcPnt0.Z = 0.0;
	FVector ArcPnt1;
	ArcPnt1.X = Width - Length;
	ArcPnt1.Y = Depth;
	ArcPnt1.Z = 0.0;

	//计算圆弧圆心
	FVector CenterPnt;
	if (FMath::Abs(HALF_PI - ArcAlph) < DELTA)
	{
		CenterPnt.X = Width - Length;
		CenterPnt.Y = Depth - Length;
		CenterPnt.Z = 0.0;
	}
	else
	{
		float Alph0 = (HALF_PI - ArcAlph) / 2.0;
		float Alph1 = Alph0 + ArcAlph;
		float TanA0 = FMath::Tan(Alph0);
		float TanA1 = FMath::Tan(Alph1);

		CenterPnt.X = (ArcPnt1.Y - ArcPnt0.Y + TanA0 * ArcPnt0.X - TanA1 * ArcPnt1.X) / (TanA0 - TanA1);
		CenterPnt.Y = (TanA1 * ArcPnt0.Y - TanA0 * ArcPnt1.Y + TanA0 * TanA1 * (ArcPnt1.X - ArcPnt0.X)) / (TanA1 - TanA0);
		CenterPnt.Z = 0.0;
	}

	//计算圆弧板件点位
	FVector NewPoint0;
	NewPoint0.X = 0.0;
	NewPoint0.Y = 0.0;
	NewPoint0.Z = 0.0;
	OutPoints.Emplace(NewPoint0);
	
	FVector NewPoint1;
	NewPoint1.X = Width;
	NewPoint1.Y = 0.0;
	NewPoint1.Z = 0.0;
	OutPoints.Emplace(NewPoint1);
	
	TArray<FVector> Line0;
	Line0.Emplace(NewPoint0);
	Line0.Emplace(NewPoint1);
	OutEdgePoints.Emplace(Line0);

	TArray<FVector> Line1;
	Line1.Emplace(NewPoint1);
	Line1.Emplace(ArcPnt0);
	OutEdgePoints.Emplace(Line1);

	TArray<FVector> ArcPoints;
	ArcPoints.Add(ArcPnt0);
	CalcArcPoints(ArcPoints, ArcPnt0, ArcPnt1, CenterPnt, ArcAlph);
	ArcPoints.Add(ArcPnt1);
	OutPoints.Append(ArcPoints);
	OutEdgePoints.Emplace(ArcPoints);


	FVector NewPoint4;
	NewPoint4.X = 0.0;
	NewPoint4.Y = Depth;
	NewPoint4.Z = 0.0;
	OutPoints.Emplace(NewPoint4);

	TArray<FVector> Line2;
	Line2.Emplace(ArcPnt1);
	Line2.Emplace(NewPoint4);
	OutEdgePoints.Emplace(Line2);

	TArray<FVector> Line3;
	Line3.Emplace(NewPoint4);
	Line3.Emplace(NewPoint0);
	OutEdgePoints.Emplace(Line3);
}

void FBoardShape::CalcRightOutArcPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints)
{
	check(ShapeParameters.Num() == 2);
	float Length = GetShapeParamValueByRefName(TEXT("TLEN"));
	float ArcAlph = GetShapeParamValueByRefName(TEXT("ANGLE"));
	ArcAlph = FMath::Clamp<float>(ArcAlph, 0, 90);
	ArcAlph = PI * ArcAlph / 180.0;

	float Width = GetShapeWidth() + GetWidthStretch();
	float Depth = GetShapeDepth() + GetDepthStretch();

	FVector ArcPnt0;
	ArcPnt0.X = Length;
	ArcPnt0.Y = Depth;
	ArcPnt0.Z = 0.0;
	FVector ArcPnt1;
	ArcPnt1.X = 0.0;
	ArcPnt1.Y = Depth - Length;
	ArcPnt1.Z = 0.0;

	//计算圆弧圆心
	FVector CenterPnt;
	if (FMath::Abs(HALF_PI - ArcAlph) < DELTA)
	{
		CenterPnt.X = Length;
		CenterPnt.Y = Depth - Length;
		CenterPnt.Z = 0.0;
	}
	else
	{
		float Alph0 = (HALF_PI - ArcAlph) / 2.0;
		float Alph1 = Alph0 + ArcAlph;
		float TanA0 = FMath::Tan(Alph0);
		float TanA1 = FMath::Tan(Alph1);

		CenterPnt.X = (TanA1 * ArcPnt0.X - TanA0 * ArcPnt1.X + TanA0 * TanA1 * (ArcPnt0.Y - ArcPnt1.Y)) / (TanA1 - TanA0);
		CenterPnt.Y = (ArcPnt1.X - ArcPnt0.X + TanA1 * ArcPnt1.Y - TanA0 * ArcPnt0.Y) / (TanA1 - TanA0);
		CenterPnt.Z = 0.0;
	}

	//计算圆弧板件点位
	FVector NewPoint0;
	NewPoint0.X = 0.0;
	NewPoint0.Y = 0.0;
	NewPoint0.Z = 0.0;
	OutPoints.Emplace(NewPoint0);

	FVector NewPoint1;
	NewPoint1.X = Width;
	NewPoint1.Y = 0.0;
	NewPoint1.Z = 0.0;
	OutPoints.Emplace(NewPoint1);
	
	TArray<FVector> Line0;
	Line0.Emplace(NewPoint0);
	Line0.Emplace(NewPoint1);
	OutEdgePoints.Emplace(Line0);

	FVector NewPoint2;
	NewPoint2.X = Width;
	NewPoint2.Y = Depth;
	NewPoint2.Z = 0.0;
	OutPoints.Emplace(NewPoint2);

	TArray<FVector> Line1;
	Line1.Emplace(NewPoint1);
	Line1.Emplace(NewPoint2);
	OutEdgePoints.Emplace(Line1);


	TArray<FVector> Line2;
	Line2.Emplace(NewPoint2);
	Line2.Emplace(ArcPnt0);
	OutEdgePoints.Emplace(Line2);


	TArray<FVector> Arc0;
	Arc0.Add(ArcPnt0);
	CalcArcPoints(Arc0, ArcPnt0, ArcPnt1, CenterPnt, ArcAlph);
	Arc0.Add(ArcPnt1);
	OutPoints.Append(Arc0);
	OutEdgePoints.Emplace(Arc0);

	TArray<FVector> Line3;
	Line3.Emplace(ArcPnt1);
	Line3.Emplace(NewPoint0);	
	OutEdgePoints.Emplace(Line3);
}

void FBoardShape::CalcLeftInArcPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints)
{
	check(ShapeParameters.Num() == 2);
	float Length = GetShapeParamValueByRefName(TEXT("TLEN"));
	float ArcAlph = GetShapeParamValueByRefName(TEXT("ANGLE"));
	ArcAlph = FMath::Clamp<float>(ArcAlph, 0, 90);
	ArcAlph = PI * ArcAlph / 180.0;

	float Width = GetShapeWidth() + GetWidthStretch();
	float Depth = GetShapeDepth() + GetDepthStretch();

	FVector ArcPnt0;
	ArcPnt0.X = Width;
	ArcPnt0.Y = Depth - Length;
	ArcPnt0.Z = 0.0;
	FVector ArcPnt1;
	ArcPnt1.X = Width - Length;
	ArcPnt1.Y = Depth;
	ArcPnt1.Z = 0.0;

	//计算圆弧圆心
	FVector CenterPnt;
	if (FMath::Abs(HALF_PI - ArcAlph) < DELTA)
	{
		CenterPnt.X = Width;
		CenterPnt.Y = Depth;
		CenterPnt.Z = 0.0;
	}
	else
	{
		float Alph0 = (HALF_PI - ArcAlph) / 2.0;
		float Alph1 = Alph0 + ArcAlph;
		float TanA0 = FMath::Tan(Alph0);
		float TanA1 = FMath::Tan(Alph1);

		CenterPnt.X = (ArcPnt0.Y - ArcPnt1.Y + TanA0 * ArcPnt1.X - TanA1 * ArcPnt0.X) / (TanA0 - TanA1);
		CenterPnt.Y = (TanA1 * ArcPnt1.Y - TanA0 * ArcPnt0.Y + TanA0 * TanA1 * (ArcPnt0.X - ArcPnt1.X)) / (TanA1 - TanA0);
		CenterPnt.Z = 0.0;
	}

	//计算圆弧板件点位
	FVector NewPoint0;
	NewPoint0.X = 0.0;
	NewPoint0.Y = 0.0;
	NewPoint0.Z = 0.0;
	OutPoints.Emplace(NewPoint0);
	
	FVector NewPoint1;
	NewPoint1.X = Width;
	NewPoint1.Y = 0.0;
	NewPoint1.Z = 0.0;
	OutPoints.Emplace(NewPoint1);
	
	TArray<FVector> Line0;
	Line0.Emplace(NewPoint0);
	Line0.Emplace(NewPoint1);
	OutEdgePoints.Emplace(Line0);


	TArray<FVector> Line1;
	Line1.Emplace(NewPoint1);
	Line1.Emplace(ArcPnt0);
	OutEdgePoints.Emplace(Line1);

	TArray<FVector> ArcArray;
	ArcArray.Add(ArcPnt0);
	CalcArcPoints(ArcArray, ArcPnt0, ArcPnt1, CenterPnt, ArcAlph, false);
	ArcArray.Add(ArcPnt1);
	OutPoints.Append(ArcArray);
	OutEdgePoints.Emplace(ArcArray);

	FVector NewPoint2;
	NewPoint2.X = 0.0;
	NewPoint2.Y = Depth;
	NewPoint2.Z = 0.0;
	OutPoints.Emplace(NewPoint2);

	TArray<FVector> Line2;
	Line2.Emplace(ArcPnt1);
	Line2.Emplace(NewPoint2);
	OutEdgePoints.Emplace(Line2);

	TArray<FVector> Line3;
	Line3.Emplace(NewPoint2);
	Line3.Emplace(NewPoint0);
	OutEdgePoints.Emplace(Line3);

}

void FBoardShape::CalcRightInArcPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints)
{
	check(ShapeParameters.Num() == 2);
	float Length = GetShapeParamValueByRefName(TEXT("TLEN"));
	float ArcAlph = GetShapeParamValueByRefName(TEXT("ANGLE"));
	ArcAlph = FMath::Clamp<float>(ArcAlph, 0, 90);
	ArcAlph = PI * ArcAlph / 180.0;

	float Width = GetShapeWidth() + GetWidthStretch();
	float Depth = GetShapeDepth() + GetDepthStretch();

	FVector ArcPnt0;
	ArcPnt0.X = Length;
	ArcPnt0.Y = Depth;
	ArcPnt0.Z = 0.0;
	FVector ArcPnt1;
	ArcPnt1.X = 0.0;
	ArcPnt1.Y = Depth - Length;
	ArcPnt1.Z = 0.0;

	//计算圆弧圆心
	FVector CenterPnt;
	if (FMath::Abs(HALF_PI - ArcAlph) < DELTA)
	{
		CenterPnt.X = 0.0;
		CenterPnt.Y = Depth;
		CenterPnt.Z = 0.0;
	}
	else
	{
		float Alph0 = (HALF_PI - ArcAlph) / 2.0;
		float Alph1 = Alph0 + ArcAlph;
		float TanA0 = FMath::Tan(Alph0);
		float TanA1 = FMath::Tan(Alph1);

		CenterPnt.X = (TanA1 * ArcPnt1.X - TanA0 * ArcPnt0.X + TanA0 * TanA1 * (ArcPnt1.Y - ArcPnt0.Y)) / (TanA1 - TanA0);
		CenterPnt.Y = (ArcPnt0.X - ArcPnt1.X + TanA1 * ArcPnt0.Y - TanA0 * ArcPnt1.Y) / (TanA1 - TanA0);
		CenterPnt.Z = 0.0;
	}

	//计算圆弧板件点位
	FVector NewPoint0;
	NewPoint0.X = 0.0;
	NewPoint0.Y = 0.0;
	NewPoint0.Z = 0.0;
	OutPoints.Emplace(NewPoint0);
	
	FVector NewPoint1;
	NewPoint1.X = Width;
	NewPoint1.Y = 0.0;
	NewPoint1.Z = 0.0;
	OutPoints.Emplace(NewPoint1);

	TArray<FVector> Line0;
	Line0.Emplace(NewPoint0);
	Line0.Emplace(NewPoint1);
	OutEdgePoints.Emplace(Line0);
	
	FVector NewPoint2;
	NewPoint2.X = Width;
	NewPoint2.Y = Depth;
	NewPoint2.Z = 0.0;
	OutPoints.Emplace(NewPoint2);

	TArray<FVector> Line1;
	Line1.Emplace(NewPoint1);
	Line1.Emplace(NewPoint2);
	OutEdgePoints.Emplace(Line1);
	
	TArray<FVector> Line2;
	Line2.Emplace(NewPoint2);
	Line2.Emplace(ArcPnt0);
	OutEdgePoints.Emplace(Line2);

	TArray<FVector> Arc0;
	Arc0.Add(ArcPnt0);
	CalcArcPoints(Arc0, ArcPnt0, ArcPnt1, CenterPnt, ArcAlph, false);
	Arc0.Add(ArcPnt1);
	OutPoints.Append(Arc0);
	OutEdgePoints.Emplace(Arc0);

	TArray<FVector> Line3;
	Line3.Emplace(ArcPnt1);
	Line3.Emplace(NewPoint0);
	OutEdgePoints.Emplace(Line3);
}

void FBoardShape::CalcLeftCornerPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints)
{
	check(ShapeParameters.Num() == 2);
	float CornerWidth = GetShapeParamValueByRefName(TEXT("CW"));
	float CornerDepth = GetShapeParamValueByRefName(TEXT("CD"));

	float Width = GetShapeWidth() + GetWidthStretch();
	float Depth = GetShapeDepth() + GetDepthStretch();

	// 构建点 
	FVector NewPoint0;
	NewPoint0.X = 0.0;
	NewPoint0.Y = 0.0;
	NewPoint0.Z = 0.0;
	OutPoints.Emplace(NewPoint0);
	
	FVector NewPoint1;
	NewPoint1.X = Width;
	NewPoint1.Y = 0.0;
	NewPoint1.Z = 0.0;
	OutPoints.Emplace(NewPoint1);
	
	FVector NewPoint2;
	NewPoint2.X = Width;
	NewPoint2.Y = Depth - CornerDepth;
	NewPoint2.Z = 0.0;
	OutPoints.Emplace(NewPoint2);
	
	FVector NewPoint3;
	NewPoint3.X = Width - CornerWidth;
	NewPoint3.Y = Depth - CornerDepth;
	NewPoint3.Z = 0.0;
	OutPoints.Emplace(NewPoint3);
	
	FVector NewPoint4;
	NewPoint4.X = Width - CornerWidth;
	NewPoint4.Y = Depth;
	NewPoint4.Z = 0.0;
	OutPoints.Emplace(NewPoint4);
	
	FVector NewPoint5;
	NewPoint5.X = 0.0;
	NewPoint5.Y = Depth;
	NewPoint5.Z = 0.0;
	OutPoints.Emplace(NewPoint5);

	// 构建并存储边
	TArray<FVector> Line0;
	Line0.Emplace(NewPoint0);
	Line0.Emplace(NewPoint1);
	OutEdgePoints.Emplace(Line0);

	TArray<FVector> Line1;
	Line1.Emplace(NewPoint1);
	Line1.Emplace(NewPoint2);
	OutEdgePoints.Emplace(Line1);

	TArray<FVector> Line2;
	Line2.Emplace(NewPoint2);
	Line2.Emplace(NewPoint3);
	OutEdgePoints.Emplace(Line2);

	TArray<FVector> Line3;
	Line3.Emplace(NewPoint3);
	Line3.Emplace(NewPoint4);
	OutEdgePoints.Emplace(Line3);

	TArray<FVector> Line4;
	Line4.Emplace(NewPoint4);
	Line4.Emplace(NewPoint5);
	OutEdgePoints.Emplace(Line4);

	TArray<FVector> Line5;
	Line5.Emplace(NewPoint5);
	Line5.Emplace(NewPoint0);
	OutEdgePoints.Emplace(Line5);

}

void FBoardShape::CalcRightCornerPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints)
{
	check(ShapeParameters.Num() == 2);
	float CornerWidth = GetShapeParamValueByRefName(TEXT("CW"));
	float CornerDepth = GetShapeParamValueByRefName(TEXT("CD"));

	float Width = GetShapeWidth() + GetWidthStretch();
	float Depth = GetShapeDepth() + GetDepthStretch();

	// 构建点
	FVector NewPoint0;
	NewPoint0.X = 0.0;
	NewPoint0.Y = 0.0;
	NewPoint0.Z = 0.0;
	OutPoints.Emplace(NewPoint0);
	
	FVector NewPoint1;
	NewPoint1.X = Width;
	NewPoint1.Y = 0.0;
	NewPoint1.Z = 0.0;
	OutPoints.Emplace(NewPoint1);
	
	FVector NewPoint2;
	NewPoint2.X = Width;
	NewPoint2.Y = Depth;
	NewPoint2.Z = 0.0;
	OutPoints.Emplace(NewPoint2);
	
	FVector NewPoint3;
	NewPoint3.X = CornerWidth;
	NewPoint3.Y = Depth;
	NewPoint3.Z = 0.0;
	OutPoints.Emplace(NewPoint3);
	
	FVector NewPoint4;
	NewPoint4.X = CornerWidth;
	NewPoint4.Y = Depth - CornerDepth;
	NewPoint4.Z = 0.0;
	OutPoints.Emplace(NewPoint4);
	
	FVector NewPoint5;
	NewPoint5.X = 0.0;
	NewPoint5.Y = Depth - CornerDepth;
	NewPoint5.Z = 0.0;
	OutPoints.Emplace(NewPoint5);

	// 构建边
	TArray<FVector> Line0;
	Line0.Emplace(NewPoint0);
	Line0.Emplace(NewPoint1);

	TArray<FVector> Line1;
	Line1.Emplace(NewPoint1);
	Line1.Emplace(NewPoint2);

	TArray<FVector> Line2;
	Line2.Emplace(NewPoint2);
	Line2.Emplace(NewPoint3);

	TArray<FVector> Line3;
	Line3.Emplace(NewPoint3);
	Line3.Emplace(NewPoint4);

	TArray<FVector> Line4;
	Line4.Emplace(NewPoint4);
	Line4.Emplace(NewPoint5);

	TArray<FVector> Line5;
	Line5.Emplace(NewPoint5);
	Line5.Emplace(NewPoint0);

	// 存储边	
	OutEdgePoints.Emplace(Line0);
	OutEdgePoints.Emplace(Line1);
	OutEdgePoints.Emplace(Line2);
	OutEdgePoints.Emplace(Line3);
	OutEdgePoints.Emplace(Line4);
	OutEdgePoints.Emplace(Line5);
}

void FBoardShape::CalcLeftCornerCutPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints)
{
	check(ShapeParameters.Num() == 4);
	float CornerWidth = GetShapeParamValueByRefName(TEXT("CW"));
	float CornerDepth = GetShapeParamValueByRefName(TEXT("CD"));
	float CutWidth = GetShapeParamValueByRefName(TEXT("TW"));
	float CutDepth = GetShapeParamValueByRefName(TEXT("TD"));

	float Width = GetShapeWidth() + GetWidthStretch();
	float Depth = GetShapeDepth() + GetDepthStretch();

	// 构建点
	FVector NewPoint0;
	NewPoint0.X = CutWidth;
	NewPoint0.Y = CutDepth;
	NewPoint0.Z = 0.0;
	OutPoints.Emplace(NewPoint0);
	
	FVector NewPoint1;
	NewPoint1.X = CutWidth;
	NewPoint1.Y = 0.0;
	NewPoint1.Z = 0.0;
	OutPoints.Emplace(NewPoint1);
	
	FVector NewPoint2;
	NewPoint2.X = Width;
	NewPoint2.Y = 0.0;
	NewPoint2.Z = 0.0;
	OutPoints.Emplace(NewPoint2);
	
	FVector NewPoint3;
	NewPoint3.X = Width;
	NewPoint3.Y = Depth - CornerDepth;
	NewPoint3.Z = 0.0;
	OutPoints.Emplace(NewPoint3);
	
	FVector NewPoint4;
	NewPoint4.X = Width - CornerWidth;
	NewPoint4.Y = Depth - CornerDepth;
	NewPoint4.Z = 0.0;
	OutPoints.Emplace(NewPoint4);
	
	FVector NewPoint5;
	NewPoint5.X = Width - CornerWidth;
	NewPoint5.Y = Depth;
	NewPoint5.Z = 0.0;
	OutPoints.Emplace(NewPoint5);
	
	FVector NewPoint6;
	NewPoint6.X = 0.0;
	NewPoint6.Y = Depth;
	NewPoint6.Z = 0.0;
	OutPoints.Emplace(NewPoint6);
	
	FVector NewPoint7;
	NewPoint7.X = 0.0;
	NewPoint7.Y = CutDepth;
	NewPoint7.Z = 0.0;
	OutPoints.Emplace(NewPoint7);

	// 构建边
	TArray<FVector> Line0;
	Line0.Emplace(NewPoint0);
	Line0.Emplace(NewPoint1);

	TArray<FVector> Line1;
	Line1.Emplace(NewPoint1);
	Line1.Emplace(NewPoint2);
	
	TArray<FVector> Line2;
	Line2.Emplace(NewPoint2);
	Line2.Emplace(NewPoint3);	

	TArray<FVector> Line3;
	Line3.Emplace(NewPoint3);
	Line3.Emplace(NewPoint4);
	
	TArray<FVector> Line4;
	Line4.Emplace(NewPoint4);
	Line4.Emplace(NewPoint5);
	
	TArray<FVector> Line5;
	Line5.Emplace(NewPoint5);
	Line5.Emplace(NewPoint6);
	
	TArray<FVector> Line6;
	Line6.Emplace(NewPoint6);
	Line6.Emplace(NewPoint7);
	
	TArray<FVector> Line7;
	Line7.Emplace(NewPoint7);
	Line7.Emplace(NewPoint0);
	
	// 存储边
	OutEdgePoints.Emplace(Line0);
	OutEdgePoints.Emplace(Line1);
	OutEdgePoints.Emplace(Line2);
	OutEdgePoints.Emplace(Line3);
	OutEdgePoints.Emplace(Line4);
	OutEdgePoints.Emplace(Line5);
	OutEdgePoints.Emplace(Line6);
	OutEdgePoints.Emplace(Line7);
	
}

void FBoardShape::CalcRightCornerCutPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints)
{
	check(ShapeParameters.Num() == 4);
	float CornerWidth = GetShapeParamValueByRefName(TEXT("CW"));
	float CornerDepth = GetShapeParamValueByRefName(TEXT("CD"));
	float CutWidth = GetShapeParamValueByRefName(TEXT("TW"));
	float CutDepth = GetShapeParamValueByRefName(TEXT("TD"));

	float Width = GetShapeWidth() + GetWidthStretch();
	float Depth = GetShapeDepth() + GetDepthStretch();

	// 构建点
	FVector NewPoint0;
	NewPoint0.X = 0.0;
	NewPoint0.Y = 0.0;
	NewPoint0.Z = 0.0;
	OutPoints.Emplace(NewPoint0);
	
	FVector NewPoint1;
	NewPoint1.X = Width - CutWidth;
	NewPoint1.Y = 0.0;
	NewPoint1.Z = 0.0;
	OutPoints.Emplace(NewPoint1);
	
	FVector NewPoint2;
	NewPoint2.X = Width - CutWidth;
	NewPoint2.Y = CutDepth;
	NewPoint2.Z = 0.0;
	OutPoints.Emplace(NewPoint2);
	
	FVector NewPoint3;
	NewPoint3.X = Width;
	NewPoint3.Y = CutDepth;
	NewPoint3.Z = 0.0;
	OutPoints.Emplace(NewPoint3);
	
	FVector NewPoint4;
	NewPoint4.X = Width;
	NewPoint4.Y = Depth;
	NewPoint4.Z = 0.0;
	OutPoints.Emplace(NewPoint4);
	
	FVector NewPoint5;
	NewPoint5.X = CornerWidth;
	NewPoint5.Y = Depth;
	NewPoint5.Z = 0.0;
	OutPoints.Emplace(NewPoint5);
	
	FVector NewPoint6;
	NewPoint6.X = CornerWidth;
	NewPoint6.Y = Depth - CornerDepth;
	NewPoint6.Z = 0.0;
	OutPoints.Emplace(NewPoint6);
	
	FVector NewPoint7;
	NewPoint7.X = 0.0;
	NewPoint7.Y = Depth - CornerDepth;
	NewPoint7.Z = 0.0;
	OutPoints.Emplace(NewPoint7);
	
	// 构建边
	TArray<FVector> Line0;
	Line0.Emplace(NewPoint0);
	Line0.Emplace(NewPoint1);

	TArray<FVector> Line1;
	Line1.Emplace(NewPoint1);
	Line1.Emplace(NewPoint2);

	TArray<FVector> Line2;
	Line2.Emplace(NewPoint2);
	Line2.Emplace(NewPoint3);

	TArray<FVector> Line3;
	Line3.Emplace(NewPoint3);
	Line3.Emplace(NewPoint4);

	TArray<FVector> Line4;
	Line4.Emplace(NewPoint4);
	Line4.Emplace(NewPoint5);

	TArray<FVector> Line5;
	Line5.Emplace(NewPoint5);
	Line5.Emplace(NewPoint6);


	TArray<FVector> Line6;
	Line6.Emplace(NewPoint6);
	Line6.Emplace(NewPoint7);

	TArray<FVector> Line7;
	Line7.Emplace(NewPoint7);
	Line7.Emplace(NewPoint0);

	// 存储边
	OutEdgePoints.Emplace(Line0);
	OutEdgePoints.Emplace(Line1);
	OutEdgePoints.Emplace(Line2);
	OutEdgePoints.Emplace(Line3);
	OutEdgePoints.Emplace(Line4);
	OutEdgePoints.Emplace(Line5);
	OutEdgePoints.Emplace(Line6);
	OutEdgePoints.Emplace(Line7);
}

void FBoardShape::CalcLeftInArcCutPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints)
{
	check(ShapeParameters.Num() == 4);
	float Length = GetShapeParamValueByRefName(TEXT("TLEN"));
	float ArcAlph = GetShapeParamValueByRefName(TEXT("ANGLE"));
	float CutWidth = GetShapeParamValueByRefName(TEXT("TW"));
	float CutDepth = GetShapeParamValueByRefName(TEXT("TD"));
	ArcAlph = FMath::Clamp<float>(ArcAlph, 0, 90);
	ArcAlph = PI * ArcAlph / 180.0;

	float Width = GetShapeWidth() + GetWidthStretch();
	float Depth = GetShapeDepth() + GetDepthStretch();

	FVector ArcPnt0;
	ArcPnt0.X = Width;
	ArcPnt0.Y = Depth - Length;
	ArcPnt0.Z = 0.0;
	FVector ArcPnt1;
	ArcPnt1.X = Width - Length;
	ArcPnt1.Y = Depth;
	ArcPnt1.Z = 0.0;

	//计算圆弧圆心
	FVector CenterPnt;
	if (FMath::Abs(HALF_PI - ArcAlph) < DELTA)
	{
		CenterPnt.X = Width;
		CenterPnt.Y = Depth;
		CenterPnt.Z = 0.0;
	}
	else
	{
		float Alph0 = (HALF_PI - ArcAlph) / 2.0;
		float Alph1 = Alph0 + ArcAlph;
		float TanA0 = FMath::Tan(Alph0);
		float TanA1 = FMath::Tan(Alph1);

		CenterPnt.X = (ArcPnt0.Y - ArcPnt1.Y + TanA0 * ArcPnt1.X - TanA1 * ArcPnt0.X) / (TanA0 - TanA1);
		CenterPnt.Y = (TanA1 * ArcPnt1.Y - TanA0 * ArcPnt0.Y + TanA0 * TanA1 * (ArcPnt0.X - ArcPnt1.X)) / (TanA1 - TanA0);
		CenterPnt.Z = 0.0;
	}

	//计算圆弧板件点位
	FVector NewPoint0;
	NewPoint0.X = CutWidth;
	NewPoint0.Y = CutDepth;
	NewPoint0.Z = 0.0;
	OutPoints.Emplace(NewPoint0);
	
	FVector NewPoint1;
	NewPoint1.X = CutWidth;
	NewPoint1.Y = 0.0;
	NewPoint1.Z = 0.0;
	OutPoints.Emplace(NewPoint1);

	TArray<FVector> Line0;
	Line0.Emplace(NewPoint0);
	Line0.Emplace(NewPoint1);
	OutEdgePoints.Emplace(Line0);

	
	FVector NewPoint2;
	NewPoint2.X = Width;
	NewPoint2.Y = 0.0;
	NewPoint2.Z = 0.0;
	OutPoints.Emplace(NewPoint2);

	TArray<FVector> Line1;
	Line1.Emplace(NewPoint1);
	Line1.Emplace(NewPoint2);
	OutEdgePoints.Emplace(Line1);
	
	TArray<FVector> Line2;
	Line2.Emplace(NewPoint2);
	Line2.Emplace(ArcPnt0);
	OutEdgePoints.Emplace(Line2);
	
	TArray<FVector> Arc0;
	Arc0.Add(ArcPnt0);
	CalcArcPoints(Arc0, ArcPnt0, ArcPnt1, CenterPnt, ArcAlph, false);
	Arc0.Add(ArcPnt1);
	OutPoints.Append(Arc0);
	OutEdgePoints.Emplace(Arc0);

	FVector NewPoint5;
	NewPoint5.X = 0.0;
	NewPoint5.Y = Depth;
	NewPoint5.Z = 0.0;
	OutPoints.Emplace(NewPoint5);
	
	TArray<FVector> Line3;
	Line3.Emplace(ArcPnt1);
	Line3.Emplace(NewPoint5);
	OutEdgePoints.Emplace(Line3);

	FVector NewPoint6;
	NewPoint6.X = 0.0;
	NewPoint6.Y = CutDepth;
	NewPoint6.Z = 0.0;
	OutPoints.Emplace(NewPoint6);
	
	TArray<FVector> Line4;
	Line4.Emplace(NewPoint5);
	Line4.Emplace(NewPoint6);
	OutEdgePoints.Emplace(Line4);

	TArray<FVector> Line5;
	Line5.Emplace(NewPoint6);
	Line5.Emplace(NewPoint0);
	OutEdgePoints.Emplace(Line5);

}

void FBoardShape::CalcRightInArcCutPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints)
{
	check(ShapeParameters.Num() == 4);
	float Length = GetShapeParamValueByRefName(TEXT("TLEN"));
	float ArcAlph = GetShapeParamValueByRefName(TEXT("ANGLE"));
	float CutWidth = GetShapeParamValueByRefName(TEXT("TW"));
	float CutDepth = GetShapeParamValueByRefName(TEXT("TD"));
	ArcAlph = FMath::Clamp<float>(ArcAlph, 0, 90);
	ArcAlph = PI * ArcAlph / 180.0;

	float Width = GetShapeWidth() + GetWidthStretch();
	float Depth = GetShapeDepth() + GetDepthStretch();

	FVector ArcPnt0;
	ArcPnt0.X = Length;
	ArcPnt0.Y = Depth;
	ArcPnt0.Z = 0.0;
	FVector ArcPnt1;
	ArcPnt1.X = 0.0;
	ArcPnt1.Y = Depth - Length;
	ArcPnt1.Z = 0.0;

	//计算圆弧圆心
	FVector CenterPnt;
	if (FMath::Abs(HALF_PI - ArcAlph) < DELTA)
	{
		CenterPnt.X = 0.0;
		CenterPnt.Y = Depth;
		CenterPnt.Z = 0.0;
	}
	else
	{
		float Alph0 = (HALF_PI - ArcAlph) / 2.0;
		float Alph1 = Alph0 + ArcAlph;
		float TanA0 = FMath::Tan(Alph0);
		float TanA1 = FMath::Tan(Alph1);

		CenterPnt.X = (TanA1 * ArcPnt1.X - TanA0 * ArcPnt0.X + TanA0 * TanA1 * (ArcPnt1.Y - ArcPnt0.Y)) / (TanA1 - TanA0);
		CenterPnt.Y = (ArcPnt0.X - ArcPnt1.X + TanA1 * ArcPnt0.Y - TanA0 * ArcPnt1.Y) / (TanA1 - TanA0);
		CenterPnt.Z = 0.0;
	}

	//计算圆弧板件点位
	FVector NewPoint0;
	NewPoint0.X = 0.0;
	NewPoint0.Y = 0.0;
	NewPoint0.Z = 0.0;
	OutPoints.Emplace(NewPoint0);
	
	FVector NewPoint1;
	NewPoint1.X = Width - CutWidth;
	NewPoint1.Y = 0.0;
	NewPoint1.Z = 0.0;
	OutPoints.Emplace(NewPoint1);

	TArray<FVector> Line0;
	Line0.Emplace(NewPoint0);
	Line0.Emplace(NewPoint1);
	OutEdgePoints.Emplace(Line0);
	
	FVector NewPoint2;
	NewPoint2.X = Width - CutWidth;
	NewPoint2.Y = CutDepth;
	NewPoint2.Z = 0.0;
	OutPoints.Emplace(NewPoint2);

	TArray<FVector> Line1;
	Line1.Emplace(NewPoint1);
	Line1.Emplace(NewPoint2);	
	OutEdgePoints.Emplace(Line1);

	
	FVector NewPoint3;
	NewPoint3.X = Width;
	NewPoint3.Y = CutDepth;
	NewPoint3.Z = 0.0;
	OutPoints.Emplace(NewPoint3);

	TArray<FVector> Line2;
	Line2.Emplace(NewPoint2);
	Line2.Emplace(NewPoint3);
	OutEdgePoints.Emplace(Line2);
	
	FVector NewPoint4;
	NewPoint4.X = Width;
	NewPoint4.Y = Depth;
	NewPoint4.Z = 0.0;
	OutPoints.Emplace(NewPoint4);

	TArray<FVector> Line3;
	Line3.Emplace(NewPoint3);
	Line3.Emplace(NewPoint4);
	OutEdgePoints.Emplace(Line3);

	TArray<FVector> Line4;
	Line4.Emplace(NewPoint4);
	Line4.Emplace(ArcPnt0);
	OutEdgePoints.Emplace(Line4);

	TArray<FVector> Arc0;
	Arc0.Add(ArcPnt0);
	CalcArcPoints(Arc0, ArcPnt0, ArcPnt1, CenterPnt, ArcAlph, false);
	Arc0.Add(ArcPnt1);
	OutPoints.Append(Arc0);
	OutEdgePoints.Emplace(Arc0);


	TArray<FVector> Line5;
	Line5.Emplace(ArcPnt1);
	Line5.Emplace(NewPoint0);
	OutEdgePoints.Emplace(Line5);

}

void FBoardShape::CalcLeftRightOutArcPoints(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutPoints)
{
	check(ShapeParameters.Num() == 4);
	float RightLength = GetShapeParamValueByRefName(TEXT("RTLEN"));
	float RightArcAlph = GetShapeParamValueByRefName(TEXT("RANGLE"));
	RightArcAlph = FMath::Clamp<float>(RightArcAlph, 0, 90);
	RightArcAlph = PI * RightArcAlph / 180.0;
	float LeftLength = GetShapeParamValueByRefName(TEXT("LTLEN"));
	float LeftArcAlph = GetShapeParamValueByRefName(TEXT("LANGLE"));
	LeftArcAlph = FMath::Clamp<float>(LeftArcAlph, 0, 90);
	LeftArcAlph = PI * LeftArcAlph / 180.0;

	float Width = GetShapeWidth() + GetWidthStretch();
	float Depth = GetShapeDepth() + GetDepthStretch();

	//右圆弧起止点
	FVector RightArcPnt0;
	RightArcPnt0.X = Width;
	RightArcPnt0.Y = Depth - RightLength;
	RightArcPnt0.Z = 0.0;
	FVector RightArcPnt1;
	RightArcPnt1.X = Width - RightLength;
	RightArcPnt1.Y = Depth;
	RightArcPnt1.Z = 0.0;
	//右圆弧圆心
	FVector RightCenterPnt;
	if (FMath::Abs(HALF_PI - RightArcAlph) < DELTA)
	{
		RightCenterPnt.X = Width - RightLength;
		RightCenterPnt.Y = Depth - RightLength;
		RightCenterPnt.Z = 0.0;
	}
	else
	{
		float Alph0 = (HALF_PI - RightArcAlph) / 2.0;
		float Alph1 = Alph0 + RightArcAlph;
		float TanA0 = FMath::Tan(Alph0);
		float TanA1 = FMath::Tan(Alph1);

		RightCenterPnt.X = (RightArcPnt1.Y - RightArcPnt0.Y + TanA0 * RightArcPnt0.X - TanA1 * RightArcPnt1.X) / (TanA0 - TanA1);
		RightCenterPnt.Y = (TanA1 * RightArcPnt0.Y - TanA0 * RightArcPnt1.Y + TanA0 * TanA1 * (RightArcPnt1.X - RightArcPnt0.X)) / (TanA1 - TanA0);
		RightCenterPnt.Z = 0.0;
	}

	//左圆弧起止点
	FVector LeftArcPnt0;
	LeftArcPnt0.X = LeftLength;
	LeftArcPnt0.Y = Depth;
	LeftArcPnt0.Z = 0.0;
	FVector LeftArcPnt1;
	LeftArcPnt1.X = 0.0;
	LeftArcPnt1.Y = Depth - LeftLength;
	LeftArcPnt1.Z = 0.0;
	//左圆弧圆心
	FVector LeftCenterPnt;
	if (FMath::Abs(HALF_PI - LeftArcAlph) < DELTA)
	{
		LeftCenterPnt.X = LeftLength;
		LeftCenterPnt.Y = Depth - LeftLength;
		LeftCenterPnt.Z = 0.0;
	}
	else
	{
		float Alph0 = (HALF_PI - LeftArcAlph) / 2.0;
		float Alph1 = Alph0 + LeftArcAlph;
		float TanA0 = FMath::Tan(Alph0);
		float TanA1 = FMath::Tan(Alph1);

		LeftCenterPnt.X = (TanA1 * LeftArcPnt0.X - TanA0 * LeftArcPnt1.X + TanA0 * TanA1 * (LeftArcPnt0.Y - LeftArcPnt1.Y)) / (TanA1 - TanA0);
		LeftCenterPnt.Y = (LeftArcPnt1.X - LeftArcPnt0.X + TanA1 * LeftArcPnt1.Y - TanA0 * LeftArcPnt0.Y) / (TanA1 - TanA0);
		LeftCenterPnt.Z = 0.0;
	}

	//计算圆弧板件点位
	FVector NewPoint0;
	NewPoint0.X = 0.0;
	NewPoint0.Y = 0.0;
	NewPoint0.Z = 0.0;
	OutPoints.Emplace(NewPoint0);

	FVector NewPoint1;
	NewPoint1.X = Width;
	NewPoint1.Y = 0.0;
	NewPoint1.Z = 0.0;
	OutPoints.Emplace(NewPoint1);

	TArray<FVector> Line0;
	Line0.Emplace(NewPoint0);
	Line0.Emplace(NewPoint1);
	OutEdgePoints.Emplace(Line0);

	TArray<FVector> Line1;
	Line1.Emplace(NewPoint1);
	Line1.Emplace(RightArcPnt0);
	OutEdgePoints.Emplace(Line1);

	TArray<FVector> RightArcPoints;
	RightArcPoints.Add(RightArcPnt0);
	CalcArcPoints(RightArcPoints, RightArcPnt0, RightArcPnt1, RightCenterPnt, RightArcAlph);
	RightArcPoints.Add(RightArcPnt1);
	OutPoints.Append(RightArcPoints);
	OutEdgePoints.Emplace(RightArcPoints);

	TArray<FVector> Line2;
	Line2.Emplace(RightArcPnt1);
	Line2.Emplace(LeftArcPnt0);
	OutEdgePoints.Emplace(Line2);

	TArray<FVector> LeftArcPoints;
	LeftArcPoints.Add(LeftArcPnt0);
	CalcArcPoints(LeftArcPoints, LeftArcPnt0, LeftArcPnt1, LeftCenterPnt, LeftArcAlph);
	LeftArcPoints.Add(LeftArcPnt1);
	OutPoints.Append(LeftArcPoints);
	OutEdgePoints.Emplace(LeftArcPoints);

	TArray<FVector> Line3;
	Line3.Emplace(LeftArcPnt1);
	Line3.Emplace(NewPoint0);
	OutEdgePoints.Emplace(Line3);
}

void FBoardShape::CalcArcPoints(TArray<FVector>& OutPoints, 
	FVector ArcPnt0, FVector ArcPnt1, FVector CenterPnt, 
	float ArcAlph, bool bClockWise/* = true*/)
{
	float Radius = FMath::Sqrt((ArcPnt0.Y - CenterPnt.Y) * (ArcPnt0.Y - CenterPnt.Y) + ((ArcPnt0.X - CenterPnt.X) * (ArcPnt0.X - CenterPnt.X)));
	FVector RotVec = (ArcPnt0 - CenterPnt).GetSafeNormal();
	float Delta = ArcAlph / CURVESTEP;

	FVector AxisVec = FVector::UpVector;
	if (!bClockWise)
	{
		AxisVec = -1.0 * FVector::UpVector;
	}

	for (int32 i = 0; i < CURVESTEP; ++i)
	{
		FQuat qRot(AxisVec, Delta);
		RotVec = qRot.RotateVector(RotVec);
		OutPoints.Emplace(CenterPnt + RotVec * Radius);
	}
}

// void FBoardShape::ProcessOutline(TArray<TArray<FVector>> & OutEdgePoints, TArray<FVector> & OutBoardPoints)
// {
// 	//1、计算轮廓顶点序列
// 	EOutlineType OutlineType = OutlineShape->GetOutlineType();
// 	if (OutlineType == OLT_None)
// 	{
// 		return;
// 	}
// 
// 	//2、构建轮廓CSGNode
// 	//2.0、计算轮廓网格点
// 	TArray<FVector> OutlinePoints;
// 	OutlineShape->CalOutlineVertexForCSGNode(OutlinePoints);
// 	//2.1、构建轮廓点CSGVertex
// 	TArray<CSG::FCSGVertex> OutlineVertices;
// 	for (int32 i = 0; i < OutlinePoints.Num(); ++i)
// 	{
// 		CSG::FCSGVertex NewVertex(OutlinePoints[i]);
// 		OutlineVertices.Add(NewVertex);
// 	}
// 	//2.2、构建轮廓CSGPolygen
// 	TArray<CSG::FCSGPolygon> OutlinePolygons;
// 	for (int32 i = 0; i < OutlineVertices.Num(); i = i + 3)
// 	{
// 		TArray<CSG::FCSGVertex> NewVertices;
// 		NewVertices.Add(OutlineVertices[i]);
// 		NewVertices.Add(OutlineVertices[i + 1]);
// 		NewVertices.Add(OutlineVertices[i + 2]);
// 		CSG::FCSGPolygon NewPolygon(NewVertices);
// 		OutlinePolygons.Add(NewPolygon);
// 	}
// 	//2.3、构建轮廓CSGNode
// 	CSG::FCSGNode* OutlineCSGNode = CSG::MakeCSGNode(OutlinePolygons);
// 
// 	//test
// 	TArray<CSG::FCSGVertex> OutlineVertices11;
// 	TArray<uint16> OutlineIndices11;
// 	CSG::ToCSGVertices(OutlineCSGNode, OutlineVertices11, OutlineIndices11);
// 
// 	//3、构建板件CSGNode
// 	//3.0、计算板件网格点
// 	TArray<FVector> BoardPoints;
// 	CalBoardPointsForCSGNode(BoardPoints);
// 	//3.1、构建板件点CSGVertex
// 	TArray<CSG::FCSGVertex> BoardVertices;
// 	for (int32 i=0; i<BoardPoints.Num(); ++i)
// 	{
// 		CSG::FCSGVertex NewVertex(BoardPoints[i]);
// 		BoardVertices.Add(NewVertex);
// 	}
// 	//3.2、构建板件CSGPolygen
// 	TArray<CSG::FCSGPolygon> BoardPolygons;
// 	for (int32 i=0; i<BoardVertices.Num(); i=i+3)
// 	{
// 		TArray<CSG::FCSGVertex> NewVertices;
// 		NewVertices.Add(BoardVertices[i]);
// 		NewVertices.Add(BoardVertices[i+1]);
// 		NewVertices.Add(BoardVertices[i+2]);
// 		CSG::FCSGPolygon NewPolygon(NewVertices);
// 		BoardPolygons.Add(NewPolygon);
// 	}
// 	//3.3、构建板件CSGNode
// 	CSG::FCSGNode* BoardCSGNode = CSG::MakeCSGNode(BoardPolygons);
// 
// 	//test
// 	TArray<CSG::FCSGVertex> BoardVertices11;
// 	TArray<uint16> BoardIndices11;
// 	CSG::ToCSGVertices(BoardCSGNode, BoardVertices11, BoardIndices11);
// 
// 	//4、板件布尔减轮廓
// 	CSG::Subtract(BoardCSGNode, OutlineCSGNode);
// 
// 	//5、计算板件点位索引
// 	TArray<CSG::FCSGVertex> CSGVertices;
// 	TArray<uint16> CSGIndices;
// 	CSG::ToCSGVertices(BoardCSGNode, CSGVertices, CSGIndices);
// 
// 	//6、转换板件顶点序列
// 
// 	//7、计算板件封边序列
// }

bool FBoardShape::CheckInnerRectHoleValied() // const
{
	check(GetShapeParamCount() >= 4 && GetShapeParamCount() % 4 == 0);
	const INT32 RectHoleCount = GetShapeParamCount() / 4;
	bool RetValue = false;
	const float BoardShapWidth = GetShapeWidth();
	const float BoardShapeDepth = GetShapeDepth();
	for (INT32 Index = 0; Index < RectHoleCount; ++Index)
	{
		const float InnerSHX = GetShapeParamValue(Index * 4 + 0);
		const float InnerSHY = GetShapeParamValue(Index * 4 + 1);
		const float InnerSHW = GetShapeParamValue(Index * 4 + 2);
		const float InnerSHD = GetShapeParamValue(Index * 4 + 3);

		RetValue = ShapWithHoleValueCheck::InnerRectHoleValidCheck(BoardShapWidth, BoardShapeDepth, InnerSHX, InnerSHY, InnerSHW, InnerSHD);
		if (!RetValue) break;
	}
	return 	RetValue;
		
}

bool FBoardShape::CheckInnerCircleHoleValied() // const
{
	const float InnerCHX = GetShapeParamValue(0);
	const float InnerCHY = GetShapeParamValue(1);
	const float InnerCHR = GetShapeParamValue(2);
	const float BoardShapWidth = GetShapeWidth();
	const float BoardShapeDepth = GetShapeDepth();
	return ShapWithHoleValueCheck::InnerCircleHoleValidCheck(BoardShapWidth, BoardShapeDepth, InnerCHX, InnerCHY, InnerCHR);
}


//板件见光的判定
TArray<int32> FBoardShape::GetShapeDimensionMatchingTranslucent()
{
	//保存不见光的面
	TArray<int32> TranslucentPlaneBuffer;
	//保存见光的板面
	TArray<int32> CullFacePlaneBuffer;

	FVector ShapeDesiredLocation(GetShapePosX(), GetShapePosY(), GetShapePosZ());
	switch (GetShapeCategory())
	{
	case BUT_HorizontalBoard:   // 横板 // 水平板件，在X方向进行查询时只检查侧板，在Y方向进行查询时只检查背板和面板
	case BUT_TopBoard:
	case BUT_BottomBoard:
	case BUT_MobileBoard:
	{
		//计算周边面的点
		FVector LeftPos = ShapeDesiredLocation + FVector::RightVector*(GetShapeDepth()) + FVector::UpVector*(GetShapeHeight());
		FVector RightPos = LeftPos + FVector::ForwardVector * GetShapeWidth();
		FVector BackPos = ShapeDesiredLocation + FVector::ForwardVector*(GetShapeWidth()) + FVector::UpVector*(GetShapeHeight());
		FVector FrontPos = BackPos + FVector::RightVector * GetShapeDepth();

		//判定周边面是否为见光
		//背面
		!IsPlaneCullFace(BackPos / 10.0f, -FVector::RightVector) ? TranslucentPlaneBuffer.Emplace(1) : CullFacePlaneBuffer.Emplace(1);
		//右面
		!IsPlaneCullFace(RightPos / 10.0f, FVector::ForwardVector) ? TranslucentPlaneBuffer.Emplace(2) : CullFacePlaneBuffer.Emplace(2);
		//前面
		!IsPlaneCullFace(FrontPos / 10.0f, FVector::RightVector) ? TranslucentPlaneBuffer.Emplace(3) : CullFacePlaneBuffer.Emplace(3);
		//左面
		!IsPlaneCullFace(LeftPos / 10.0f, -FVector::ForwardVector) ? TranslucentPlaneBuffer.Emplace(4) : CullFacePlaneBuffer.Emplace(4);

		break;
	}
	case BUT_VerticalBoard:   // 竖板 // 竖直侧板件，在Z方向进行查询时只检查顶底板和层板，在Y方向进行查询时只检查背板和面板
	case BUT_LeftBoard:
	case BUT_RightBoard:
	case BUT_RevealBoard:
	case BUT_ExposureBoard:
	case BUT_AdjustBoard:
	{
		//计算周边面的点

		FVector BackPos = ShapeDesiredLocation + FVector::ForwardVector*(GetShapeHeight() / 2.0) + FVector::UpVector*(GetShapeWidth() / 2.0f);
		FVector FrontPos = BackPos + FVector::RightVector * GetShapeDepth();
		FVector BottomPos = ShapeDesiredLocation + FVector::RightVector*(GetShapeDepth() / 2.0) + FVector::ForwardVector*(GetShapeHeight() / 2.0f);
		FVector TopPos = BottomPos + FVector::UpVector * GetShapeWidth();


		//判定周边面是否为见光
		//背面
		!IsPlaneCullFace(BackPos / 10.0f, -FVector::RightVector) ? TranslucentPlaneBuffer.Emplace(1) : CullFacePlaneBuffer.Emplace(1);
		//上面
		!IsPlaneCullFace(TopPos / 10.0f, FVector::UpVector) ? TranslucentPlaneBuffer.Emplace(2) : CullFacePlaneBuffer.Emplace(2);
		//前面
		!IsPlaneCullFace(FrontPos / 10.0f, FVector::RightVector) ? TranslucentPlaneBuffer.Emplace(3) : CullFacePlaneBuffer.Emplace(3);
		//下面
		!IsPlaneCullFace(BottomPos / 10.0f, -FVector::UpVector) ? TranslucentPlaneBuffer.Emplace(4) : CullFacePlaneBuffer.Emplace(4);

		break;
	}
	case BUT_BackBoard:   // 背板，在Z方向进行查询时只检查顶底板和层板，在X方向进行查询时只检查侧板
	case BUT_FrontBoard:
	case BUT_EnhanceBoard:
	case BUT_ConvergentBoard:
	case BUT_SkirtingBoard:
	{
		//计算周边面的点
		FVector LeftPos = ShapeDesiredLocation + FVector::RightVector*(GetShapeHeight() / 2.0f) + FVector::UpVector*(GetShapeWidth() / 2.0f);
		FVector RightPos = LeftPos + FVector::ForwardVector * GetShapeDepth();
		FVector BottomPos = ShapeDesiredLocation + FVector::RightVector*(GetShapeHeight() / 2.0) + FVector::ForwardVector*(GetShapeDepth() / 2.0f);
		FVector TopPos = BottomPos + FVector::UpVector * GetShapeWidth();

		//判定周边面是否为见光
		//左面
		!IsPlaneCullFace(LeftPos / 10.0f, -FVector::ForwardVector) ? TranslucentPlaneBuffer.Emplace(1) : CullFacePlaneBuffer.Emplace(1);
		//上面
		!IsPlaneCullFace(TopPos / 10.0f, FVector::UpVector) ? TranslucentPlaneBuffer.Emplace(2) : CullFacePlaneBuffer.Emplace(2);
		//右面
		!IsPlaneCullFace(RightPos / 10.0f, FVector::ForwardVector) ? TranslucentPlaneBuffer.Emplace(3) : CullFacePlaneBuffer.Emplace(3);
		//下面
		!IsPlaneCullFace(BottomPos / 10.0f, -FVector::UpVector) ? TranslucentPlaneBuffer.Emplace(4) : CullFacePlaneBuffer.Emplace(4);

		break;
	}
	default:
		break;
	}

	return MoveTemp(TranslucentPlaneBuffer);
}


//判断板面的是否和其他Shape相交
bool FBoardShape::IsPlaneCullFace(const FVector& PlaneCorePos, const FVector& Direction)
{
	FCollisionQueryParams TraceParams(FName(TEXT("RayCast")), false);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.AddIgnoredActor(GetShapeActor());

	FHitResult HitResult;
	FVector EndPos = PlaneCorePos + Direction / 10.0f;

	//射线检测
	GWorld->LineTraceSingleByChannel(HitResult,
		PlaneCorePos,
		EndPos,
		ECC_GameTraceChannel3,
		TraceParams);

	if (HitResult.GetActor() != nullptr)
	{
		ASCTShapeActor *ShapeActor = Cast<ASCTShapeActor>(HitResult.GetActor());
		if (ShapeActor)
		{
			//判定两边时候相交吻合
			if (Direction == FVector(-1, 0, 0))
			{
				return FMath::Abs(PlaneCorePos.X - ShapeActor->GetShape()->GetShapePosRightSide() / 10.0f) < KINDA_SMALL_NUMBER;
			}
			else if (Direction == FVector(1, 0, 0))
			{
				return FMath::Abs(PlaneCorePos.X - ShapeActor->GetShape()->GetShapePosLeftSide() / 10.0f) < KINDA_SMALL_NUMBER;
			}
			else if (Direction == FVector(0, -1, 0))
			{
				return FMath::Abs(PlaneCorePos.Y - ShapeActor->GetShape()->GetShapePosFrontSide() / 10.0f) < KINDA_SMALL_NUMBER;
			}
			else if (Direction == FVector(0, 1, 0))
			{
				return FMath::Abs(PlaneCorePos.Y - ShapeActor->GetShape()->GetShapePosBackSide() / 10.0f) < KINDA_SMALL_NUMBER;
			}
			else if (Direction == FVector(0, 0, -1))
			{
				return FMath::Abs(PlaneCorePos.Z - ShapeActor->GetShape()->GetShapePosTopSide() / 10.0f) < KINDA_SMALL_NUMBER;
			}
			else if (Direction == FVector(0, 0, 1))
			{
				return FMath::Abs(PlaneCorePos.Z - ShapeActor->GetShape()->GetShapePosBottomSide() / 10.0f) < KINDA_SMALL_NUMBER;
			}
			else
			{
				return false;
			}
		}
	}

	return false;
}

bool FBoardShape::IsNeedRebuilEdgeAndHolePointSet() const
{	
	return (ShapeParameters.Num() == 8 || ShapeParameters.Num() == 4) && BoardShapeType == BoST_InnerRectangleHole;
}
