#include "SCTAttribute.h"
#include "SCTShape.h"
#include "Any.h"
#include "Formula.h"
#include "SCTStringTool.h"
#include "SharedPointer.h"
#include "JsonObject.h"
#include "JsonWriter.h"
#include "DelegateSignatureImpl.inl"
#include "CondensedJsonPrintPolicy.h"

const int32 DefaultRangeMin = -3000;
const int32 DefaultRangeMax = 3000;

FShapeAttribute::FShapeAttribute()
{
	AttributeId = 0;
	RefName = "";
	InStrValue = "";
	OutAnyValue = MakeShareable(new FAny());
}

FShapeAttribute::~FShapeAttribute()
{
	
}

void FShapeAttribute::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	AttributeId = InJsonObject->GetNumberField(TEXT("id"));

	RefName = InJsonObject->GetStringField(TEXT("refName"));
}

void FShapeAttribute::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//属性ID
	JsonWriter->WriteValue(TEXT("id"), GetAttributeId());

	//属性类型
	JsonWriter->WriteValue(TEXT("type"), (int32)GetAttributeType());
	
	//属性引用名
	JsonWriter->WriteValue(TEXT("refName"), GetRefName());
}

void FShapeAttribute::CopyTo(const TSharedPtr<FShapeAttribute> OutShapeAttri)
{
	OutShapeAttri->AttributeId = AttributeId;
	OutShapeAttri->RefName = RefName;
	OutShapeAttri->AttributeType = AttributeType;
	OutShapeAttri->InStrValue = InStrValue;
}

bool FShapeAttribute::SetAttributeValue(const FString& InValue)
{
	InStrValue = InValue;
	return true;
}

FString FShapeAttribute::GetAttributeStr() const
{
	return InStrValue;
}

bool FShapeAttribute::CheckIfAttributeIsFormula() const
{
	return !FSCTStringTool::VerifyStringContainNumberOnly(InStrValue);
}

TSharedPtr<FAny> FShapeAttribute::GetAttributeValue()
{
	return OutAnyValue;
}

void FShapeAttribute::SetDirtyFlag()
{

}

EShapeAttributeType FShapeAttribute::GetAttributeType() const
{
	return AttributeType;
}

void FShapeAttribute::SetAttributeId(int64 InID)
{
	AttributeId = InID;
}

int64 FShapeAttribute::GetAttributeId() const
{
	return AttributeId;
}

void FShapeAttribute::SetRefName(FString InName)
{
	RefName = InName;
}

FString FShapeAttribute::GetRefName() const
{
	return RefName;
}


FBoolAssignAttri::FBoolAssignAttri(FSCTShape* InShape)
{
	AttributeType = SAT_BoolAssign;
	FormulaValue = MakeShareable(new FFormula());
	FormulaValue->OnPValFuncDelegate.BindRaw(InShape, &FSCTShape::OnPValFunc);
	FormulaValue->OnFindEntFuncDelegate.BindRaw(InShape, &FSCTShape::OnFindEntValue);
	FormulaValue->OnFindOwnValDelegate.BindRaw(InShape, &FSCTShape::OnFindOwnValFunc);
}

FBoolAssignAttri::~FBoolAssignAttri()
{

}

void FBoolAssignAttri::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{

}

void FBoolAssignAttri::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{

}

void FBoolAssignAttri::CopyTo(const TSharedPtr<FShapeAttribute> OutShapeAttri)
{
	FShapeAttribute::CopyTo(OutShapeAttri);
}

bool FBoolAssignAttri::SetAttributeValue(const FString& InValue)
{
	FAny* ResultAny = FormulaValue->CalculateFormula(InValue);
	bool bResult = ResultAny && ResultAny->IsSameType<bool>();
	if (bResult)
	{
		InStrValue = InValue;
	}

	return bResult;
}
TSharedPtr<FAny> FBoolAssignAttri::GetAttributeValue()
{
	FAny* ResultAny = FormulaValue->CalculateFormula(InStrValue);
	bool ResultBool = *AnyCast<bool>(ResultAny);
	OutAnyValue->operator=<bool>(ResultBool);
	return OutAnyValue;
}

void FBoolAssignAttri::SetDirtyFlag()
{
	FormulaValue->SetVariDirty();
}

bool FBoolAssignAttri::GetBoolValue()
{
	if (InStrValue.IsEmpty())
	{
		return true;
	}
	FAny* ResultAny = FormulaValue->CalculateFormula(InStrValue);
	check(ResultAny->IsSameType<bool>());
	return *AnyCast<bool>(ResultAny);
}

TSharedPtr<FFormula> FBoolAssignAttri::GetFormulaObj() const
{
	return FormulaValue;
}

FNumberAssignAttri::FNumberAssignAttri(FSCTShape* InShape)
{
	AttributeType = SAT_NumberAssign;
	FormulaValue = MakeShareable(new FFormula());
	FormulaValue->OnPValFuncDelegate.BindRaw(InShape, &FSCTShape::OnPValFunc);
	FormulaValue->OnFindEntFuncDelegate.BindRaw(InShape, &FSCTShape::OnFindEntValue);
	FormulaValue->OnFindOwnValDelegate.BindRaw(InShape, &FSCTShape::OnFindOwnValFunc);
}

FNumberAssignAttri::~FNumberAssignAttri()
{

}

void FNumberAssignAttri::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FShapeAttribute::ParseFromJson(InJsonObject);
	float CurrentValue = InJsonObject->GetNumberField(TEXT("current"));
	InStrValue = FString::Printf(TEXT("%f"), CurrentValue);
}

void FNumberAssignAttri::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//属性基本信息
	FShapeAttribute::SaveToJson(JsonWriter);
	//当前值
	JsonWriter->WriteValue(TEXT("current"), GetNumberValue());
}

void FNumberAssignAttri::CopyTo(const TSharedPtr<FShapeAttribute> OutShapeAttri)
{
	FShapeAttribute::CopyTo(OutShapeAttri);
}

bool FNumberAssignAttri::SetAttributeValue(const FString& InValue)
{
	FAny* ResultAny = FormulaValue->CalculateFormula(InValue);
	bool bResult = ResultAny && ResultAny->IsSameType<float>();
	if (bResult)
	{
		InStrValue = InValue;
		OnAttrValueChanged.ExecuteIfBound(FString::Printf(TEXT("%f"), *AnyCast<float>(ResultAny)));
	}

	return bResult;
}

TSharedPtr<FAny> FNumberAssignAttri::GetAttributeValue()
{
	FAny* ResultAny = FormulaValue->CalculateFormula(InStrValue);
	float ResultInt = *AnyCast<float>(ResultAny);
	OutAnyValue->operator=<float>(ResultInt);
	return OutAnyValue;
}

void FNumberAssignAttri::SetDirtyFlag()
{
	FormulaValue->SetVariDirty();
}

float FNumberAssignAttri::GetNumberValue()
{
	FAny* FormulaResult = FormulaValue->CalculateFormula(InStrValue);
	check(FormulaResult->IsSameType<float>());
	return *AnyCast<float>(FormulaResult);
}

TSharedPtr<FFormula> FNumberAssignAttri::GetFormulaObj() const
{
	return FormulaValue;
}

FTextAssignAttri::FTextAssignAttri(FSCTShape* InShape)
{
	AttributeType = SAT_TextAssign;
	FormulaValue = MakeShareable(new FFormula());
	FormulaValue->OnPValFuncDelegate.BindRaw(InShape, &FSCTShape::OnPValFunc);
	FormulaValue->OnFindEntFuncDelegate.BindRaw(InShape, &FSCTShape::OnFindEntValue);
	FormulaValue->OnFindOwnValDelegate.BindRaw(InShape, &FSCTShape::OnFindOwnValFunc);
}

FTextAssignAttri::~FTextAssignAttri()
{

}

void FTextAssignAttri::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{

}

void FTextAssignAttri::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{

}

void FTextAssignAttri::CopyTo(const TSharedPtr<FShapeAttribute> OutShapeAttri)
{
	FShapeAttribute::CopyTo(OutShapeAttri);
}

bool FTextAssignAttri::SetAttributeValue(const FString& InValue)
{
	FAny* ResultAny = FormulaValue->CalculateFormula(InValue);
	bool bResult = ResultAny && ResultAny->IsSameType<FString>();
	if (bResult)
	{
		InStrValue = InValue;
		OnAttrValueChanged.ExecuteIfBound(*AnyCast<FString>(ResultAny));
	}

	return bResult;
}

TSharedPtr<FAny> FTextAssignAttri::GetAttributeValue()
{
	FAny* ResultAny = FormulaValue->CalculateFormula(InStrValue);
	FString ResultStr = *AnyCast<FString>(ResultAny);
	OutAnyValue->operator=<FString>(ResultStr);
	return OutAnyValue;
}

void FTextAssignAttri::SetDirtyFlag()
{
	FormulaValue->SetVariDirty();
}

FString FTextAssignAttri::GetStringValue()
{
	FAny* ResultAny = FormulaValue->CalculateFormula(InStrValue);
	check(ResultAny->IsSameType<FString>());
	return *AnyCast<FString>(ResultAny);
}

TSharedPtr<FFormula> FTextAssignAttri::GetFormulaObj() const
{
	return FormulaValue;
}

FNumberRangeAttri::FNumberRangeAttri(FSCTShape* InShape)
{
	AttributeType = SAT_NumberRange;

	MinValue = DefaultRangeMin;
	MaxValue = DefaultRangeMax;

	FormulaValue = MakeShareable(new FFormula());
	FormulaValue->OnPValFuncDelegate.BindRaw(InShape, &FSCTShape::OnPValFunc);
	FormulaValue->OnFindEntFuncDelegate.BindRaw(InShape, &FSCTShape::OnFindEntValue);
	FormulaValue->OnFindOwnValDelegate.BindRaw(InShape, &FSCTShape::OnFindOwnValFunc);
}

FNumberRangeAttri::~FNumberRangeAttri()
{

}

void FNumberRangeAttri::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//解析基本信息
	FShapeAttribute::ParseFromJson(InJsonObject);

	//解析特征信息
	const TSharedPtr<FJsonObject>& ValueObj = InJsonObject->GetObjectField(TEXT("value"));
	MinValue = ValueObj->GetNumberField(TEXT("min"));
	MaxValue = ValueObj->GetNumberField(TEXT("max"));

	float CurrentValue = InJsonObject->GetNumberField(TEXT("current"));
	InStrValue = FString::Printf(TEXT("%3f"), CurrentValue);
}

void FNumberRangeAttri::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//属性基本信息
	FShapeAttribute::SaveToJson(JsonWriter);

	//属性值
	JsonWriter->WriteObjectStart(TEXT("value"));
	JsonWriter->WriteValue(TEXT("min"), GetMinValue());//最小限制
	JsonWriter->WriteValue(TEXT("max"), GetMaxValue());//最大限制
	JsonWriter->WriteObjectEnd();

	//当前值
	JsonWriter->WriteValue(TEXT("current"), GetNumberValue());
}

void FNumberRangeAttri::CopyTo(const TSharedPtr<FShapeAttribute> OutShapeAttri)
{
	FShapeAttribute::CopyTo(OutShapeAttri);
	TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(OutShapeAttri);
	NumberRangeAttri->MinValue = MinValue;
	NumberRangeAttri->MaxValue = MaxValue;
	NumberRangeAttri->InStrValue = GetAttributeStr();
}

bool FNumberRangeAttri::SetAttributeValue(const FString& InValue)
{
	FAny* ResultAny = FormulaValue->CalculateFormula(InValue);
	bool bResult = ResultAny && ResultAny->IsSameType<float>();
	if (bResult)
	{
		float ResultFloat = *AnyCast<float>(ResultAny);
		if ((MinValue - ResultFloat) > 0.01f || (ResultFloat - MaxValue) > 0.01f)
		{
			bResult = false;
		}
		else
		{
			InStrValue = InValue;
			OnAttrValueChanged.ExecuteIfBound(FString::Printf(TEXT("%f"), ResultFloat));
		}
	}
	return bResult;
}

TSharedPtr<FAny> FNumberRangeAttri::GetAttributeValue()
{
	FAny* ResultAny = FormulaValue->CalculateFormula(InStrValue);
	check(ResultAny->IsSameType<float>());
	float IntClamp = FMath::Clamp(*AnyCast<float>(ResultAny), MinValue, MaxValue);
	OutAnyValue->operator=<float>(IntClamp);
	return OutAnyValue;
}

void FNumberRangeAttri::SetDirtyFlag()
{
	FormulaValue->SetVariDirty();
}

void FNumberRangeAttri::SetMinValue(float InValue)
{
	MinValue = InValue;
}
float FNumberRangeAttri::GetMinValue() const
{
	return MinValue;
}

void FNumberRangeAttri::SetMaxValue(float InValue)
{
	MaxValue = InValue;
}
float FNumberRangeAttri::GetMaxValue() const
{
	return MaxValue;
}

float FNumberRangeAttri::GetNumberValue()
{
	FAny* ResultAny = FormulaValue->CalculateFormula(InStrValue);
	check(ResultAny->IsSameType<float>());
	return FMath::Clamp(*AnyCast<float>(ResultAny), MinValue, MaxValue);
}

TSharedPtr<FFormula> FNumberRangeAttri::GetFormulaObj() const
{
	return FormulaValue;
}

FTextSelectAttri::FTextSelectAttri(FSCTShape* InShape)
{
	AttributeType = SAT_TextSelect;
}

FTextSelectAttri::~FTextSelectAttri()
{

}

void FTextSelectAttri::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//解析基本信息
	FShapeAttribute::ParseFromJson(InJsonObject);

	//解析特征信息
	const TArray<TSharedPtr<FJsonValue>>* SelectArray = nullptr;
	InJsonObject->TryGetArrayField(TEXT("value"), SelectArray);
	if (SelectArray != nullptr && (*SelectArray).Num() > 0)
	{
		for (auto& It : *SelectArray)
		{
			TSharedPtr<FJsonObject> ValueObject = It->AsObject();
			int32 id = ValueObject->GetNumberField(TEXT("id"));
			FString value = ValueObject->GetStringField(TEXT("value"));
			AddSelectValue(FString::FromInt(id), value);
		}
	}

	int32 CurrentValue = InJsonObject->GetNumberField(TEXT("current"));
	InStrValue = FString::FromInt(CurrentValue);
}

void FTextSelectAttri::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//属性基本信息
	FShapeAttribute::SaveToJson(JsonWriter);

	JsonWriter->WriteArrayStart(TEXT("valueList"));

	const TMap<FString, FString>& SelectedValues = GetSelectedValues();
	for (auto it = SelectedValues.CreateConstIterator(); it; ++it)
	{
		JsonWriter->WriteObjectStart();
		FString KetStr = it.Key();
		FString ValueStr = it.Value();
		if (!KetStr.IsEmpty())
		{
			JsonWriter->WriteValue(TEXT("id"), FCString::Atoi64(KetStr.GetCharArray().GetData()));
		}
		JsonWriter->WriteValue(TEXT("value"), ValueStr);
		JsonWriter->WriteObjectEnd();
	}

	JsonWriter->WriteArrayEnd();
}

void FTextSelectAttri::CopyTo(const TSharedPtr<FShapeAttribute> OutShapeAttri)
{
	FShapeAttribute::CopyTo(OutShapeAttri);
	
	TSharedPtr<FTextSelectAttri> TextSelectAttri = StaticCastSharedPtr<FTextSelectAttri>(OutShapeAttri);
	for (auto it = SelectedValues.CreateIterator(); it; ++it)
	{
		TextSelectAttri->AddSelectValue(it.Key(), it.Value());
	}
}

bool FTextSelectAttri::SetAttributeValue(const FString& InValue)
{
	auto it = SelectedValues.Find(InValue);
	bool bResult = it != nullptr;
	if (bResult)
	{
		InStrValue = InValue;
		OnAttrValueChanged.ExecuteIfBound(*it);
	}

	return bResult;
}

TSharedPtr<FAny> FTextSelectAttri::GetAttributeValue()
{
	auto it = SelectedValues.Find(InStrValue);
	if (it == nullptr)
	{
		return nullptr;
	}
	OutAnyValue->operator=<FString>(*it);
	return OutAnyValue;
}

void FTextSelectAttri::SetDirtyFlag()
{

}

void FTextSelectAttri::AddSelectValue(const FString& Key, const FString& Value)
{
	SelectedValues.Add(Key, Value);
}

const TMap<FString, FString>& FTextSelectAttri::GetSelectedValues() const
{
	return SelectedValues;
}

void FTextSelectAttri::RemoveSelectedValue(const FString& KeyValue)
{
	SelectedValues.Remove(KeyValue);
}

void FTextSelectAttri::RemoveAllValues()
{
	SelectedValues.Reset();
}

FString FTextSelectAttri::GetStringValue()
{
	auto it = SelectedValues.Find(InStrValue);
	if (it == nullptr)
	{
		return "";
	}
	return *it;
}

FNumberSelectAttri::FNumberSelectAttri(FSCTShape* InShape)
{
	AttributeType = SAT_NumberSelect;
}

FNumberSelectAttri::~FNumberSelectAttri()
{

}

void FNumberSelectAttri::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//解析基本信息
	FShapeAttribute::ParseFromJson(InJsonObject);

	//解析特征信息
	const TArray<TSharedPtr<FJsonValue>>* SelectArray = nullptr;
	InJsonObject->TryGetArrayField(TEXT("value"), SelectArray);
	if (SelectArray != nullptr && (*SelectArray).Num() > 0)
	{
		for (auto& It : *SelectArray)
		{
			TSharedPtr<FJsonObject> ValueObject = It->AsObject();
			int32 id = ValueObject->GetNumberField(TEXT("id"));
			float value = ValueObject->GetIntegerField(TEXT("value"));
			AddSelectValue(FString::FromInt(id), value);
		}
	}

	//当前索引值
	int32 CurrentValue = InJsonObject->GetNumberField(TEXT("current"));
	InStrValue = FString::FromInt(CurrentValue); //将整型索引转换为字符串
}

void FNumberSelectAttri::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//属性基本信息
	FShapeAttribute::SaveToJson(JsonWriter);

	//可选值集合
	JsonWriter->WriteArrayStart(TEXT("value"));
	const TMap<FString, float>& SelectedValues = GetSelectedValues();
	for (auto it = SelectedValues.CreateConstIterator(); it; ++it)
	{
		JsonWriter->WriteObjectStart();
		FString KetStr = it.Key();
		float Valueint = it.Value();
		if (!KetStr.IsEmpty())
		{
			JsonWriter->WriteValue(TEXT("id"), FCString::Atoi(KetStr.GetCharArray().GetData()));
		}
		JsonWriter->WriteValue(TEXT("value"), FString::FromInt(Valueint));
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//当前索引值
	FString CurrentIndex = GetAttributeStr();//将索引值转换为整型
	JsonWriter->WriteValue(TEXT("current"), FCString::Atoi(CurrentIndex.GetCharArray().GetData()));
}

void FNumberSelectAttri::CopyTo(const TSharedPtr<FShapeAttribute> OutShapeAttri)
{
	FShapeAttribute::CopyTo(OutShapeAttri);
	TSharedPtr<FNumberSelectAttri> NumberSelectAttri = StaticCastSharedPtr<FNumberSelectAttri>(OutShapeAttri);
	for (auto it = SelectedValues.CreateIterator(); it; ++it)
	{
		NumberSelectAttri->AddSelectValue(it.Key(), it.Value());
	}
	NumberSelectAttri->InStrValue = GetAttributeStr();
}

bool FNumberSelectAttri::SetAttributeValue(const FString& InValue)
{
	FString ToSerarchValue = InValue;
	if(InValue.IsNumeric())
	{
		ToSerarchValue = FString::FromInt(StaticCast<int32>(FCString::Atof(*InValue)));
	}
	auto it = SelectedValues.Find(ToSerarchValue);
	bool bResult = it != nullptr;
	if (bResult)
	{
		InStrValue = InValue;
		OnAttrValueChanged.ExecuteIfBound(FString::Printf(TEXT("%f"), *it));
	}

	return bResult;
}

TSharedPtr<FAny> FNumberSelectAttri::GetAttributeValue()
{
	FString ToSerarchValue = InStrValue;
	if (InStrValue.IsNumeric())
	{
		ToSerarchValue = FString::FromInt(StaticCast<int32>(FCString::Atof(*InStrValue)));
	}
	auto it = SelectedValues.Find(ToSerarchValue);
	if (it == nullptr)
	{
		return nullptr;
	}
	OutAnyValue->operator=<float>(*it);
	return OutAnyValue;
}

void FNumberSelectAttri::SetDirtyFlag()
{

}

void FNumberSelectAttri::AddSelectValue(const FString& Key, float Value)
{
	SelectedValues.Add(Key, Value);
}

const TMap<FString, float>& FNumberSelectAttri::GetSelectedValues() const
{
	return SelectedValues;
}

void FNumberSelectAttri::RemoveSelectedValue(const FString& KeyValue)
{
	SelectedValues.Remove(KeyValue);
}

void FNumberSelectAttri::RemoveAllValues()
{
	SelectedValues.Reset();
}

float FNumberSelectAttri::GetNumberValue()
{
	FString ToSerarchValue = InStrValue;
	if (InStrValue.IsNumeric())
	{
		ToSerarchValue = FString::FromInt(StaticCast<int32>(FCString::Atof(*InStrValue)));
	}
	auto it = SelectedValues.Find(ToSerarchValue);
	if (it == nullptr)
	{
		return 0.0;
	}
	return *it;
}

FBoardPointAttr::FBoardPointAttr(FSCTShape* InShape)
{
	PosXAttri = MakeShareable(new FNumberAssignAttri(InShape));
	PosYAttri = MakeShareable(new FNumberAssignAttri(InShape));
}

FBoardPointAttr::~FBoardPointAttr()
{

}

void FBoardPointAttr::SetPositionX(const FString& InPosX)
{
	PosXAttri->SetAttributeValue(InPosX);
}
void FBoardPointAttr::SetPositionY(const FString& InPosY)
{
	PosYAttri->SetAttributeValue(InPosY);
}

int32 FBoardPointAttr::GetPositionX()
{
	return PosXAttri->GetNumberValue();
}
int32 FBoardPointAttr::GetPositionY()
{
	return PosYAttri->GetNumberValue();
}

void FBoardPointAttr::ParsePointAttri(const TSharedPtr<FJsonObject>& InJsonObject)
{
	SetPositionX(InJsonObject->GetStringField(TEXT("X")));
	SetPositionY(InJsonObject->GetStringField(TEXT("Y")));
}

void FBoardPointAttr::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//Start-----BoardPoint
	JsonWriter->WriteObjectStart();
	//X坐标信息
	TSharedPtr<FFormula> PosXFormula = PosXAttri->GetFormulaObj();
	JsonWriter->WriteValue(TEXT("X"), PosXFormula->GetFormulaStr());
	//Y坐标信息
	TSharedPtr<FFormula> PosYFormula = PosYAttri->GetFormulaObj();
	JsonWriter->WriteValue(TEXT("Y"), PosYFormula->GetFormulaStr());
	//End-----BoardPoint
	JsonWriter->WriteObjectEnd();
}

void FBoardPointAttr::CopyTo(const TSharedPtr<FBoardPointAttr> OutPointAttri)
{
	OutPointAttri->SetPositionX(PosXAttri->InStrValue);
	OutPointAttri->SetPositionY(PosYAttri->InStrValue);
}

void FBoardPointAttr::SetAttributesDirty()
{
	PosXAttri->SetDirtyFlag();
	PosYAttri->SetDirtyFlag();
}

TSharedPtr<FShapeAttribute> MakeAttributeByType(EShapeAttributeType InAttriType, FSCTShape* InShape)
{
	TSharedPtr<FShapeAttribute> NewAttribute = nullptr;
	switch (InAttriType)
	{
	case SAT_BoolAssign://可赋值型布尔
	{
		NewAttribute = MakeShareable(new FBoolAssignAttri(InShape));
		break;
	}
	case SAT_TextAssign://可赋值型文本
	{
		NewAttribute = MakeShareable(new FTextAssignAttri(InShape));
		break;
	}
	case SAT_NumberAssign://可赋值型数值
	{
		NewAttribute = MakeShareable(new FNumberAssignAttri(InShape));
		break;
	}
	case SAT_NumberRange://范围约束数值
	{
		NewAttribute = MakeShareable(new FNumberRangeAttri(InShape));
		break;
	}
	case SAT_TextSelect://可选值型文本
	{
		NewAttribute = MakeShareable(new FTextSelectAttri(InShape));
		break;
	}
	case SAT_NumberSelect://可选值性数值
	{
		NewAttribute = MakeShareable(new FNumberSelectAttri(InShape));
		break;
	}
	default:
		break;
	}

	return NewAttribute;
}

TSharedPtr<FShapeAttribute> ParseAttributeFromJson(const TSharedPtr<FJsonObject>& InJsonObject, FSCTShape* InShape)
{
	//1、属性值类型
	int32 AttriType = InJsonObject->GetIntegerField(TEXT("type"));
	
	//2、确定属性类型
	TSharedPtr<FShapeAttribute> ShapeAttribute = nullptr;
	switch (AttriType)
	{
	case 1:
		ShapeAttribute = MakeShareable(new FBoolAssignAttri(InShape));
		break;
	case 2:
		ShapeAttribute = MakeShareable(new FNumberAssignAttri(InShape));
		break;
	case 3:
		ShapeAttribute = MakeShareable(new FNumberRangeAttri(InShape));
		break;
	case 4:
		ShapeAttribute = MakeShareable(new FNumberSelectAttri(InShape));
		break;
	case 5:
		ShapeAttribute = MakeShareable(new FTextAssignAttri(InShape));
		break;
	case 6:
		ShapeAttribute = MakeShareable(new FTextSelectAttri(InShape));
		break;
	default:
		check(false);
		break;
	}

	//4、解析具体类型的属性信息
	ShapeAttribute->ParseFromJson(InJsonObject);

	//5、完成
	return ShapeAttribute;
}

TSharedPtr<FShapeAttribute> CopyAttributeToNew(const TSharedPtr<FShapeAttribute>& InShapeAttri, FSCTShape* InShape)
{
	if (!InShapeAttri.IsValid() || !InShape)
	{
		return nullptr;
	}

	TSharedPtr<FShapeAttribute> NewShapeAttri = nullptr;
	EShapeAttributeType AttriType = InShapeAttri->GetAttributeType();
	switch (AttriType)
	{
	case SAT_NumberRange://范围约束数值
	{
		NewShapeAttri = MakeShareable(new FNumberRangeAttri(InShape));
		break;
	}
	case SAT_TextSelect://可选值型文本
	{
		NewShapeAttri = MakeShareable(new FTextSelectAttri(InShape));
		break;
	}
	case SAT_NumberSelect://可选值性数值
	{
		NewShapeAttri = MakeShareable(new FNumberSelectAttri(InShape));
		break;
	}
	case SAT_NumberAssign://可赋值性数值
	{
		NewShapeAttri = MakeShareable(new FNumberAssignAttri(InShape));
		break;
	}
	case SAT_BoolAssign://可赋值性数值
	{
		NewShapeAttri = MakeShareable(new FBoolAssignAttri(InShape));
		break;
	}
	default:
		check(false);
		break;
	}

	InShapeAttri->CopyTo(NewShapeAttri);

	return NewShapeAttri;
}