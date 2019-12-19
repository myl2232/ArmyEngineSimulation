#include "SCTOutline.h"
#include "SCTAttribute.h"
#include "SCTShapeData.h"
#include "JsonObject.h"


FSCTOutline::FSCTOutline()
	:FSCTShape()
{

}
FSCTOutline::~FSCTOutline()
{

}

void FSCTOutline::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//注：暂时不需要调用基类接口

	//0、轮廓名称
	OutlineName = InJsonObject->GetStringField(TEXT("outlineName"));

	//1、轮廓类型
	OutlineType = (EOutlineType)InJsonObject->GetIntegerField(TEXT("outlineType"));

	//2、轮廓方向
	DirectType = InJsonObject->GetNumberField(TEXT("outlineDirect"));

	//3、轮廓参数
	const TArray<TSharedPtr<FJsonValue>>* OutlineAttriArray = nullptr;
	InJsonObject->TryGetArrayField(TEXT("OutlineParams"), OutlineAttriArray);
	if (OutlineAttriArray != nullptr && (*OutlineAttriArray).Num() > 0)
	{
		for (int32 i = 0; i < (*OutlineAttriArray).Num(); ++i)
		{
			TSharedPtr<FJsonValue> AttriValue = OutlineAttriArray->operator[](i);
			TSharedPtr<FJsonObject> AttriObject = AttriValue->AsObject();
			TSharedPtr<FShapeAttribute> NewParam = ParseAttributeFromJson(AttriObject, this);
			OutlineParameters.Add(NewParam);
		}
	}

	//4、轮廓尺寸
	SpaceWidth = InJsonObject->GetNumberField(TEXT("outlineWidth"));
	SpaceDepth = InJsonObject->GetNumberField(TEXT("outlineDepth"));
	SpaceHeight = InJsonObject->GetNumberField(TEXT("outlineHeight"));
}

void FSCTOutline::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//注：暂时不需要调用基类接口

	//0、轮廓名称
	OutlineName = InJsonObject->GetStringField(TEXT("outlineName"));

	//1、轮廓类型
	OutlineType = (EOutlineType)InJsonObject->GetIntegerField(TEXT("outlineType"));

	//2、轮廓方向
	DirectType = InJsonObject->GetNumberField(TEXT("outlineDirect"));

	//3、轮廓参数
	const TArray<TSharedPtr<FJsonValue>>* OutlineAttriArray = nullptr;
	InJsonObject->TryGetArrayField(TEXT("OutlineParams"), OutlineAttriArray);
	if (OutlineAttriArray != nullptr && (*OutlineAttriArray).Num() > 0)
	{
		for (int32 i = 0; i < (*OutlineAttriArray).Num(); ++i)
		{
			TSharedPtr<FJsonValue> AttriValue = OutlineAttriArray->operator[](i);
			TSharedPtr<FJsonObject> AttriObject = AttriValue->AsObject();
			TSharedPtr<FShapeAttribute> NewParam = ParseAttributeFromJson(AttriObject, this);
			OutlineParameters.Add(NewParam);
		}
	}

	//4、轮廓尺寸
	SpaceWidth = InJsonObject->GetNumberField(TEXT("outlineWidth"));
	SpaceDepth = InJsonObject->GetNumberField(TEXT("outlineDepth"));
	SpaceHeight = InJsonObject->GetNumberField(TEXT("outlineHeight"));
}

void FSCTOutline::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseShapeFromJson(InJsonObject);
}

void FSCTOutline::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseContentFromJson(InJsonObject);
}

void FSCTOutline::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//0、轮廓名称
	JsonWriter->WriteValue(TEXT("outlineName"), OutlineName);

	//1、轮廓类型
	JsonWriter->WriteValue(TEXT("outlineType"), (int32)GetOutlineType());

	//2、轮廓方向
	JsonWriter->WriteValue(TEXT("outlineDirect"), GetOutlineDirect());

	//3、轮廓参数
	JsonWriter->WriteArrayStart(TEXT("OutlineParams"));
	for (int32 i = 0; i < OutlineParameters.Num(); ++i)
	{
		JsonWriter->WriteObjectStart();
		OutlineParameters[i]->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//4、轮廓尺寸
	JsonWriter->WriteValue(TEXT("outlineWidth"), SpaceWidth);
	JsonWriter->WriteValue(TEXT("outlineDepth"), SpaceDepth);
	JsonWriter->WriteValue(TEXT("outlineHeight"), SpaceHeight);
}

void FSCTOutline::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//0、轮廓名称
	JsonWriter->WriteValue(TEXT("outlineName"), OutlineName);

	//1、轮廓类型
	JsonWriter->WriteValue(TEXT("outlineType"), (int32)GetOutlineType());

	//2、轮廓方向
	JsonWriter->WriteValue(TEXT("outlineDirect"), GetOutlineDirect());

	//3、轮廓参数
	JsonWriter->WriteArrayStart(TEXT("OutlineParams"));
	for (int32 i = 0; i < OutlineParameters.Num(); ++i)
	{
		JsonWriter->WriteObjectStart();
		OutlineParameters[i]->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//4、轮廓尺寸
	JsonWriter->WriteValue(TEXT("outlineWidth"), SpaceWidth);
	JsonWriter->WriteValue(TEXT("outlineDepth"), SpaceDepth);
	JsonWriter->WriteValue(TEXT("outlineHeight"), SpaceHeight);
}

void FSCTOutline::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveShapeToJson(JsonWriter);
}

void FSCTOutline::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveContentToJson(JsonWriter);
}

void FSCTOutline::CopyTo(FSCTShape* OutShape)
{
	FSCTOutline* OutlineSpace = StaticCast<FSCTOutline*>(OutShape);

	//轮廓类型
	OutlineSpace->OutlineType = OutlineType;

	//轮廓方向: 1-X轴，2-Y轴，3-Z轴
	OutlineSpace->DirectType = DirectType;
	
	//轮廓参数
	for (int32 i=0; i<OutlineParameters.Num(); ++i)
	{
		TSharedPtr<FNumberRangeAttri> NRAttri = StaticCastSharedPtr<FNumberRangeAttri>(OutlineParameters[i]);
		TSharedPtr<FNumberRangeAttri> NewAttri = MakeShareable(new FNumberRangeAttri(OutShape));
		NewAttri->SetRefName(NRAttri->GetRefName());
		NewAttri->SetAttributeValue(NRAttri->GetAttributeStr());
		NewAttri->SetMinValue(NRAttri->GetMinValue());
		NewAttri->SetMaxValue(NRAttri->GetMaxValue());
		OutlineSpace->AddOutlineParamAttri(NewAttri);
	}

	//轮廓尺寸
	OutlineSpace->SetSpaceWidth(SpaceWidth);
	OutlineSpace->SetSpaceDepth(SpaceDepth);
	OutlineSpace->SetSpaceHeight(SpaceHeight);
}

void FSCTOutline::SetOutlineType(EOutlineType InType)
{
	OutlineParameters.Empty();
	OutlineType = InType;
	switch (OutlineType)
	{
	case OLT_LeftGirder:
	{
		DirectType = 2;

		TSharedPtr<FNumberRangeAttri> NewAttri0 = MakeShareable(new FNumberRangeAttri(this));
		NewAttri0->SetRefName(TEXT("GW"));
		NewAttri0->SetAttributeValue(TEXT("100.0"));
		NewAttri0->SetMinValue(0.0);
		NewAttri0->SetMaxValue(1000.0);
		OutlineParameters.Add(NewAttri0);
		TSharedPtr<FNumberRangeAttri> NewAttri1 = MakeShareable(new FNumberRangeAttri(this));
		NewAttri1->SetRefName(TEXT("GH"));
		NewAttri1->SetAttributeValue(TEXT("100.0"));
		NewAttri1->SetMinValue(0.0);
		NewAttri1->SetMaxValue(1000.0);
		OutlineParameters.Add(NewAttri1);
		break;
	}
	case OLT_RightGirder:
	{
		DirectType = 2;

		TSharedPtr<FNumberRangeAttri> NewAttri0 = MakeShareable(new FNumberRangeAttri(this));
		NewAttri0->SetRefName(TEXT("GW"));
		NewAttri0->SetAttributeValue(TEXT("100.0"));
		NewAttri0->SetMinValue(0.0);
		NewAttri0->SetMaxValue(1000.0);
		OutlineParameters.Add(NewAttri0);
		TSharedPtr<FNumberRangeAttri> NewAttri1 = MakeShareable(new FNumberRangeAttri(this));
		NewAttri1->SetRefName(TEXT("GH"));
		NewAttri1->SetAttributeValue(TEXT("100.0"));
		NewAttri1->SetMinValue(0.0);
		NewAttri1->SetMaxValue(1000.0);
		OutlineParameters.Add(NewAttri1);
		break;
	}
	case OLT_BackGirder:
	{
		DirectType = 1;

		TSharedPtr<FNumberRangeAttri> NewAttri0 = MakeShareable(new FNumberRangeAttri(this));
		NewAttri0->SetRefName(TEXT("GD"));
		NewAttri0->SetAttributeValue(TEXT("100.0"));
		NewAttri0->SetMinValue(0.0);
		NewAttri0->SetMaxValue(1000.0);
		OutlineParameters.Add(NewAttri0);
		TSharedPtr<FNumberRangeAttri> NewAttri1 = MakeShareable(new FNumberRangeAttri(this));
		NewAttri1->SetRefName(TEXT("GH"));
		NewAttri1->SetAttributeValue(TEXT("100.0"));
		NewAttri1->SetMinValue(0.0);
		NewAttri1->SetMaxValue(1000.0);
		OutlineParameters.Add(NewAttri1);
		break;
	}
	case OLT_LeftPiller:
	{
		DirectType = 3;

		TSharedPtr<FNumberRangeAttri> NewAttri0 = MakeShareable(new FNumberRangeAttri(this));
		NewAttri0->SetRefName(TEXT("PW"));
		NewAttri0->SetAttributeValue(TEXT("100.0"));
		NewAttri0->SetMinValue(0.0);
		NewAttri0->SetMaxValue(1000.0);
		OutlineParameters.Add(NewAttri0);
		TSharedPtr<FNumberRangeAttri> NewAttri1 = MakeShareable(new FNumberRangeAttri(this));
		NewAttri1->SetRefName(TEXT("PD"));
		NewAttri1->SetAttributeValue(TEXT("100.0"));
		NewAttri1->SetMinValue(0.0);
		NewAttri1->SetMaxValue(1000.0);
		OutlineParameters.Add(NewAttri1);
		break;
	}
	case OLT_RightPiller:
	{
		DirectType = 3;

		TSharedPtr<FNumberRangeAttri> NewAttri0 = MakeShareable(new FNumberRangeAttri(this));
		NewAttri0->SetRefName(TEXT("PW"));
		NewAttri0->SetAttributeValue(TEXT("100.0"));
		NewAttri0->SetMinValue(0.0);
		NewAttri0->SetMaxValue(1000.0);
		OutlineParameters.Add(NewAttri0);
		TSharedPtr<FNumberRangeAttri> NewAttri1 = MakeShareable(new FNumberRangeAttri(this));
		NewAttri1->SetRefName(TEXT("PD"));
		NewAttri1->SetAttributeValue(TEXT("100.0"));
		NewAttri1->SetMinValue(0.0);
		NewAttri1->SetMaxValue(1000.0);
		OutlineParameters.Add(NewAttri1);
		break;
	}
	case OLT_MiddlePiller:
	{
		DirectType = 3;

		TSharedPtr<FNumberRangeAttri> NewAttri0 = MakeShareable(new FNumberRangeAttri(this));
		NewAttri0->SetRefName(TEXT("PX"));
		NewAttri0->SetAttributeValue(TEXT("500.0"));
		NewAttri0->SetMinValue(0.0);
		NewAttri0->SetMaxValue(1000.0);
		OutlineParameters.Add(NewAttri0);
		TSharedPtr<FNumberRangeAttri> NewAttri1 = MakeShareable(new FNumberRangeAttri(this));
		NewAttri1->SetRefName(TEXT("PW"));
		NewAttri1->SetAttributeValue(TEXT("100.0"));
		NewAttri1->SetMinValue(0.0);
		NewAttri1->SetMaxValue(1000.0);
		OutlineParameters.Add(NewAttri1);
		TSharedPtr<FNumberRangeAttri> NewAttri2 = MakeShareable(new FNumberRangeAttri(this));
		NewAttri2->SetRefName(TEXT("PD"));
		NewAttri2->SetAttributeValue(TEXT("100.0"));
		NewAttri2->SetMinValue(0.0);
		NewAttri2->SetMaxValue(1000.0);
		OutlineParameters.Add(NewAttri2);
		break;
	}
	case OLT_CornerPiller:
	{
		DirectType = 3;

		TSharedPtr<FNumberRangeAttri> NewAttri0 = MakeShareable(new FNumberRangeAttri(this));
		NewAttri0->SetRefName(TEXT("CW"));
		NewAttri0->SetAttributeValue(TEXT("100.0"));
		NewAttri0->SetMinValue(0.0);
		NewAttri0->SetMaxValue(1000.0);
		OutlineParameters.Add(NewAttri0);
		TSharedPtr<FNumberRangeAttri> NewAttri1 = MakeShareable(new FNumberRangeAttri(this));
		NewAttri1->SetRefName(TEXT("CD"));
		NewAttri1->SetAttributeValue(TEXT("100.0"));
		NewAttri1->SetMinValue(0.0);
		NewAttri1->SetMaxValue(1000.0);
		OutlineParameters.Add(NewAttri1);

		TSharedPtr<FNumberRangeAttri> NewAttri2 = MakeShareable(new FNumberRangeAttri(this));
		NewAttri2->SetRefName(TEXT("PW"));
		NewAttri2->SetAttributeValue(TEXT("100.0"));
		NewAttri2->SetMinValue(0.0);
		NewAttri2->SetMaxValue(1000.0);
		OutlineParameters.Add(NewAttri2);
		TSharedPtr<FNumberRangeAttri> NewAttri3 = MakeShareable(new FNumberRangeAttri(this));
		NewAttri3->SetRefName(TEXT("PD"));
		NewAttri3->SetAttributeValue(TEXT("100.0"));
		NewAttri3->SetMinValue(0.0);
		NewAttri3->SetMaxValue(1000.0);
		OutlineParameters.Add(NewAttri3);
		break;
	}
	default:
		DirectType = 0;
		break;
	}
}

bool FSCTOutline::SetSpaceWidth(float InValue)
{
	//验证输入值是否有效，若有效同时修改轮廓参数变化范围
	switch (OutlineType)
	{
	case OLT_LeftGirder:
	{
		TSharedPtr<FShapeAttribute> GWAttri = GetOutlineParamAttriByRefName(TEXT("GW"));
		check(GWAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> GWNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(GWAttri);
		float GWidth = GWNRAttri->GetNumberValue();
		if (GWidth >= InValue)
		{
			return false;
		}
		//修改参数的变化范围
		GWNRAttri->SetMaxValue(InValue);
		break;
	}
	case OLT_RightGirder:
	{
		TSharedPtr<FShapeAttribute> GWAttri = GetOutlineParamAttriByRefName(TEXT("GW"));
		check(GWAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> GWNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(GWAttri);
		float GWidth = GWNRAttri->GetNumberValue();
		if (GWidth >= InValue)
		{
			return false;
		}
		//修改参数的变化范围
		GWNRAttri->SetMaxValue(InValue);
		break;
	}
	case OLT_BackGirder:
	{
		break;
	}
	case OLT_LeftPiller:
	{
		TSharedPtr<FShapeAttribute> PWAttri = GetOutlineParamAttriByRefName(TEXT("PW"));
		check(PWAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> PWNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(PWAttri);
		float PWidth = PWNRAttri->GetNumberValue();
		if (PWidth >= InValue)
		{
			return false;
		}
		//修改参数的变化范围
		PWNRAttri->SetMaxValue(InValue);
		break;
	}
	case OLT_RightPiller:
	{
		TSharedPtr<FShapeAttribute> PWAttri = GetOutlineParamAttriByRefName(TEXT("PW"));
		check(PWAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> PWNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(PWAttri);
		float PWidth = PWNRAttri->GetNumberValue();
		if (PWidth >= InValue)
		{
			return false;
		}
		//修改参数的变化范围
		PWNRAttri->SetMaxValue(InValue);
		break;
	}
	case OLT_MiddlePiller:
	{
		TSharedPtr<FShapeAttribute> PWAttri = GetOutlineParamAttriByRefName(TEXT("PW"));
		check(PWAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> PWNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(PWAttri);
		float PWidth = PWNRAttri->GetNumberValue();
		if (PWidth >= InValue)
		{
			return false;
		}
		//修改参数的变化范围
		PWNRAttri->SetMaxValue(InValue);
		break;
	}
	case OLT_CornerPiller:
	{
		TSharedPtr<FShapeAttribute> CWAttri = GetOutlineParamAttriByRefName(TEXT("CW"));
		check(CWAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> CWNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(CWAttri);
		float CWidth = CWNRAttri->GetNumberValue();
		TSharedPtr<FShapeAttribute> PWAttri = GetOutlineParamAttriByRefName(TEXT("PW"));
		check(PWAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> PWNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(PWAttri);
		float PWidth = PWNRAttri->GetNumberValue();
		if (CWidth + PWidth >= InValue)
		{
			return false;
		}
		//修改参数的变化范围
		CWNRAttri->SetMaxValue(InValue - PWidth);
		PWNRAttri->SetMaxValue(InValue - CWidth);
		break;
	}
	default:
		break;
	}

	SpaceWidth = InValue;
	return true;
}

bool FSCTOutline::SetSpaceDepth(float InValue)
{
	//验证输入值是否有效，若有效同时修改轮廓参数变化范围
	switch (OutlineType)
	{
	case OLT_LeftGirder:
	{
		break;
	}
	case OLT_RightGirder:
	{
		break;
	}
	case OLT_BackGirder:
	{
		TSharedPtr<FShapeAttribute> GDAttri = GetOutlineParamAttriByRefName(TEXT("GD"));
		check(GDAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> GDNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(GDAttri);
		float GDepth = GDNRAttri->GetNumberValue();
		if (GDepth >= InValue)
		{
			return false;
		}
		//修改参数的变化范围
		GDNRAttri->SetMaxValue(InValue);
		break;
	}
	case OLT_LeftPiller:
	{
		TSharedPtr<FShapeAttribute> PDAttri = GetOutlineParamAttriByRefName(TEXT("PD"));
		check(PDAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> PDNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(PDAttri);
		float PDepth = PDNRAttri->GetNumberValue();
		if (PDepth >= InValue)
		{
			return false;
		}
		//修改参数的变化范围
		PDNRAttri->SetMaxValue(InValue);
		break;
	}
	case OLT_RightPiller:
	{
		TSharedPtr<FShapeAttribute> PDAttri = GetOutlineParamAttriByRefName(TEXT("PD"));
		check(PDAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> PDNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(PDAttri);
		float PDepth = PDNRAttri->GetNumberValue();
		if (PDepth >= InValue)
		{
			return false;
		}
		//修改参数的变化范围
		PDNRAttri->SetMaxValue(InValue);
		break;
	}
	case OLT_MiddlePiller:
	{
		TSharedPtr<FShapeAttribute> PDAttri = GetOutlineParamAttriByRefName(TEXT("PD"));
		check(PDAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> PDNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(PDAttri);
		float PDepth = PDNRAttri->GetNumberValue();
		if (PDepth >= InValue)
		{
			return false;
		}
		//修改参数的变化范围
		PDNRAttri->SetMaxValue(InValue);
		break;
	}
	case OLT_CornerPiller:
	{
		TSharedPtr<FShapeAttribute> CDAttri = GetOutlineParamAttriByRefName(TEXT("CD"));
		check(CDAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> CDNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(CDAttri);
		float CDepth = CDNRAttri->GetNumberValue();
		TSharedPtr<FShapeAttribute> PDAttri = GetOutlineParamAttriByRefName(TEXT("PD"));
		check(PDAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> PDNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(PDAttri);
		float PDepth = PDNRAttri->GetNumberValue();
		if (CDepth + PDepth >= InValue)
		{
			return false;
		}
		//修改参数的变化范围
		CDNRAttri->SetMaxValue(InValue - PDepth);
		PDNRAttri->SetMaxValue(InValue - CDepth);
		break;
	}
	default:
		break;
	}

	SpaceDepth = InValue;
	return true;
}

bool FSCTOutline::SetSpaceHeight(float InValue)
{
	//验证输入值是否有效，若有效同时修改轮廓参数变化范围
	switch (OutlineType)
	{
	case OLT_LeftGirder:
	{
		TSharedPtr<FShapeAttribute> GHAttri = GetOutlineParamAttriByRefName(TEXT("GH"));
		check(GHAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> GHNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(GHAttri);
		float GHeight = GHNRAttri->GetNumberValue();
		if (GHeight >= InValue)
		{
			return false;
		}
		//修改参数的变化范围
		GHNRAttri->SetMaxValue(InValue);
		break;
	}
	case OLT_RightGirder:
	{
		TSharedPtr<FShapeAttribute> GHAttri = GetOutlineParamAttriByRefName(TEXT("GH"));
		check(GHAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> GHNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(GHAttri);
		float GHeight = GHNRAttri->GetNumberValue();
		if (GHeight >= InValue)
		{
			return false;
		}
		//修改参数的变化范围
		GHNRAttri->SetMaxValue(InValue);
		break;
	}
	case OLT_BackGirder:
	{
		TSharedPtr<FShapeAttribute> GHAttri = GetOutlineParamAttriByRefName(TEXT("GH"));
		check(GHAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> GHNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(GHAttri);
		float GHeight = GHNRAttri->GetNumberValue();
		if (GHeight >= InValue)
		{
			return false;
		}
		//修改参数的变化范围
		GHNRAttri->SetMaxValue(InValue);
		break;
	}
	case OLT_LeftPiller:
	{
		break;
	}
	case OLT_RightPiller:
	{
		break;
	}
	case OLT_MiddlePiller:
	{
		break;
	}
	case OLT_CornerPiller:
	{
		break;
	}
	default:
		break;
	}

	SpaceHeight = InValue;
	return true;
}

bool FSCTOutline::IsValidForWidth(float InValue)
{
	switch (OutlineType)
	{
	case OLT_LeftGirder:
	{
		TSharedPtr<FShapeAttribute> GWAttri = GetOutlineParamAttriByRefName(TEXT("GW"));
		check(GWAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> GWNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(GWAttri);
		float GWidth = GWNRAttri->GetNumberValue();
		if (GWidth >= InValue)
		{
			return false;
		}
		break;
	}
	case OLT_RightGirder:
	{
		TSharedPtr<FShapeAttribute> GWAttri = GetOutlineParamAttriByRefName(TEXT("GW"));
		check(GWAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> GWNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(GWAttri);
		float GWidth = GWNRAttri->GetNumberValue();
		if (GWidth >= InValue)
		{
			return false;
		}
		break;
	}
	case OLT_BackGirder:
	{
		break;
	}
	case OLT_LeftPiller:
	{
		TSharedPtr<FShapeAttribute> PWAttri = GetOutlineParamAttriByRefName(TEXT("PW"));
		check(PWAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> PWNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(PWAttri);
		float PWidth = PWNRAttri->GetNumberValue();
		if (PWidth >= InValue)
		{
			return false;
		}
		break;
	}
	case OLT_RightPiller:
	{
		TSharedPtr<FShapeAttribute> PWAttri = GetOutlineParamAttriByRefName(TEXT("PW"));
		check(PWAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> PWNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(PWAttri);
		float PWidth = PWNRAttri->GetNumberValue();
		if (PWidth >= InValue)
		{
			return false;
		}
		break;
	}
	case OLT_MiddlePiller:
	{
		TSharedPtr<FShapeAttribute> PWAttri = GetOutlineParamAttriByRefName(TEXT("PW"));
		check(PWAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> PWNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(PWAttri);
		float PWidth = PWNRAttri->GetNumberValue();
		if (PWidth >= InValue)
		{
			return false;
		}
		break;
	}
	case OLT_CornerPiller:
	{
		TSharedPtr<FShapeAttribute> CWAttri = GetOutlineParamAttriByRefName(TEXT("CW"));
		check(CWAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> CWNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(CWAttri);
		float CWidth = CWNRAttri->GetNumberValue();
		TSharedPtr<FShapeAttribute> PWAttri = GetOutlineParamAttriByRefName(TEXT("PW"));
		check(PWAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> PWNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(PWAttri);
		float PWidth = PWNRAttri->GetNumberValue();
		if (CWidth + PWidth >= InValue)
		{
			return false;
		}
		break;
	}
	default:
		break;
	}

	return true;
}

bool FSCTOutline::IsValidForDepth(float InValue)
{
	switch (OutlineType)
	{
	case OLT_LeftGirder:
	{
		break;
	}
	case OLT_RightGirder:
	{
		break;
	}
	case OLT_BackGirder:
	{
		TSharedPtr<FShapeAttribute> GDAttri = GetOutlineParamAttriByRefName(TEXT("GD"));
		check(GDAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> GDNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(GDAttri);
		float GDepth = GDNRAttri->GetNumberValue();
		if (GDepth >= InValue)
		{
			return false;
		}
		break;
	}
	case OLT_LeftPiller:
	{
		TSharedPtr<FShapeAttribute> PDAttri = GetOutlineParamAttriByRefName(TEXT("PD"));
		check(PDAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> PDNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(PDAttri);
		float PDepth = PDNRAttri->GetNumberValue();
		if (PDepth >= InValue)
		{
			return false;
		}
		break;
	}
	case OLT_RightPiller:
	{
		TSharedPtr<FShapeAttribute> PDAttri = GetOutlineParamAttriByRefName(TEXT("PD"));
		check(PDAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> PDNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(PDAttri);
		float PDepth = PDNRAttri->GetNumberValue();
		if (PDepth >= InValue)
		{
			return false;
		}
		break;
	}
	case OLT_MiddlePiller:
	{
		TSharedPtr<FShapeAttribute> PDAttri = GetOutlineParamAttriByRefName(TEXT("PD"));
		check(PDAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> PDNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(PDAttri);
		float PDepth = PDNRAttri->GetNumberValue();
		if (PDepth >= InValue)
		{
			return false;
		}
		break;
	}
	case OLT_CornerPiller:
	{
		TSharedPtr<FShapeAttribute> CDAttri = GetOutlineParamAttriByRefName(TEXT("CD"));
		check(CDAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> CDNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(CDAttri);
		float CDepth = CDNRAttri->GetNumberValue();
		TSharedPtr<FShapeAttribute> PDAttri = GetOutlineParamAttriByRefName(TEXT("PD"));
		check(PDAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> PDNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(PDAttri);
		float PDepth = PDNRAttri->GetNumberValue();
		if (CDepth + PDepth >= InValue)
		{
			return false;
		}
		break;
	}
	default:
		break;
	}

	return true;
}

bool FSCTOutline::IsValidForHeight(float InValue)
{
	switch (OutlineType)
	{
	case OLT_LeftGirder:
	{
		TSharedPtr<FShapeAttribute> GHAttri = GetOutlineParamAttriByRefName(TEXT("GH"));
		check(GHAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> GHNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(GHAttri);
		float GHeight = GHNRAttri->GetNumberValue();
		if (GHeight >= InValue)
		{
			return false;
		}
		break;
	}
	case OLT_RightGirder:
	{
		TSharedPtr<FShapeAttribute> GHAttri = GetOutlineParamAttriByRefName(TEXT("GH"));
		check(GHAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> GHNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(GHAttri);
		float GHeight = GHNRAttri->GetNumberValue();
		if (GHeight >= InValue)
		{
			return false;
		}
		break;
	}
	case OLT_BackGirder:
	{
		TSharedPtr<FShapeAttribute> GHAttri = GetOutlineParamAttriByRefName(TEXT("GH"));
		check(GHAttri->GetAttributeType() == SAT_NumberRange);
		TSharedPtr<FNumberRangeAttri> GHNRAttri = StaticCastSharedPtr<FNumberRangeAttri>(GHAttri);
		float GHeight = GHNRAttri->GetNumberValue();
		if (GHeight >= InValue)
		{
			return false;
		}
		break;
	}
	case OLT_LeftPiller:
	{
		break;
	}
	case OLT_RightPiller:
	{
		break;
	}
	case OLT_MiddlePiller:
	{
		break;
	}
	case OLT_CornerPiller:
	{
		break;
	}
	default:
		break;
	}

	return true;
}

float FSCTOutline::GetOutlineWidthRange(float& OutMinValue, float& OutMaxValue)
{
	return 0.0;
}

float FSCTOutline::GetOutlineDepthRange(float& OutMinValue, float& OutMaxValue)
{
	return 0.0;
}

float FSCTOutline::GetOutlineHeightRange(float& OutMinValue, float& OutMaxValue)
{
	return 0.0;
}

bool FSCTOutline::SplitSpaceByVirtual(TArray<TSharedPtr<FSCTOutline>> ResultOutlines, 
	ESpaceDirectType InDirect, TArray<float> InPartValues)
{
	return false;
}

bool FSCTOutline::SplitSpaceByBoard(TArray<TSharedPtr<FSCTOutline>> ResultOutlines, 
	FBoardShape* InBoard, ESpaceDirectType InDirect, TArray<float> InPartValues)
{
	return false;
}

float FSCTOutline::GetOutlineParamValueByRefName(const FString &InRefName)
{
	for (int32 i = 0; i < OutlineParameters.Num(); ++i)
	{
		if (OutlineParameters[i]->GetRefName() == InRefName)
		{
			check(OutlineParameters[i]->GetAttributeType() == SAT_NumberRange);
			TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(OutlineParameters[i]);
			return NumberRangeAttri->GetNumberValue();
		}
	}
	return 0.0f;
}

float FSCTOutline::GetOutlineParamValueByIndex(int32 InIndex)
{
	if (InIndex < 0 || InIndex >= OutlineParameters.Num())
	{
		return 0.0;
	}
	check(OutlineParameters[InIndex]->GetAttributeType() == SAT_NumberRange);
	TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(OutlineParameters[InIndex]);
	return NumberRangeAttri->GetNumberValue();
}

bool FSCTOutline::SetOutlineParamValueByRefName(const FString &InRefName, float InValue)
{
	for (int32 i = 0; i < OutlineParameters.Num(); ++i)
	{
		if (OutlineParameters[i]->GetRefName() == InRefName)
		{
			FString StrValue = FString::Printf(TEXT("%f"), InValue);
			FString OldParamStr = OutlineParameters[i]->GetAttributeStr();
			bool bResult = OutlineParameters[i]->SetAttributeValue(StrValue);
			if (!bResult)
			{
				OutlineParameters[i]->SetAttributeValue(OldParamStr);
			}
			return bResult;
		}
	}
	return true;
}

TSharedPtr<FShapeAttribute> FSCTOutline::GetOutlineParamAttriByRefName(const FString &InRefName)
{
	for (int32 i = 0; i < OutlineParameters.Num(); ++i)
	{
		if (OutlineParameters[i]->GetRefName() == InRefName)
		{
			return OutlineParameters[i];
		}
	}
	return nullptr;
}

TSharedPtr<FShapeAttribute> FSCTOutline::GetOutlineParamAttriByIndex(int32 InIndex)
{
	if (InIndex < 0 || InIndex >= OutlineParameters.Num())
	{
		return nullptr;
	}
	return OutlineParameters[InIndex];
}

void FSCTOutline::AddOutlineParamAttri(TSharedPtr<FShapeAttribute> InAttri)
{
	OutlineParameters.Add(InAttri);
}

void FSCTOutline::ClearOutlineParamAttris()
{
	OutlineParameters.Empty();
}
