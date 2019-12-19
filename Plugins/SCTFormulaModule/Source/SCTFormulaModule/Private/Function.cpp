#include "Function.h"
#include "Formula.h"

IFunction::IFunction(FFormula *pFormula)
	: MainFormula(pFormula)
{
}

IFunction::~IFunction()
{
	for (int32 i = 0; i < NumParams; ++i)
		delete ppParams[i];
	delete ppParams;
}

bool IFunction::ParseFunctionParameters(const TCHAR* ParameterChar)
{
	//根据拆分参数
	bool bParamWrong = false;
	TArray<FString> ParamArray;
	FString ParameterEle; ParameterEle.Reserve(255);
	int32 Len = FCString::Strlen(ParameterChar);
	int32 Sentinel = 0;
	for (int32 i = 0; i < Len; ++i)
	{
		if (ParameterChar[i] == TEXT(','))
		{
			if (Sentinel == i)
			{ // 两个逗号连到了一起，或者是函数一开始就是一个逗号
				bParamWrong = true;
				break;
			}
			if (i - Sentinel <= 2)
			{
				return false; //不完整的参数
			}
			ParameterEle = FString(i - Sentinel, &ParameterChar[Sentinel]);
			ParameterEle.GetCharArray().Emplace(TEXT('\0'));
			ParamArray.Emplace(ParameterEle);
			Sentinel = i + 1;
		}
	}
	bParamWrong |= (Sentinel == Len); // 参数末尾是一个逗号
	if (bParamWrong)
	{
		return false;
	}
	if (Len - Sentinel <= 2)
	{
		return false; //不完整的参数
	}
	ParameterEle = FString(Len - Sentinel, &ParameterChar[Sentinel]);
	ParameterEle.GetCharArray().Emplace(TEXT('\0'));
	ParamArray.Emplace(ParameterEle);

	//根据参数创建子公式表达式
	NumParams = ParamArray.Num();
	ppParams = new FSubFormula*[NumParams];
	for (int32 i = 0; i < NumParams; ++i)
	{
		ppParams[i] = new FSubFormula(MainFormula);
		bool bRes = ppParams[i]->ParseFormula(ParamArray[i].GetCharArray().GetData(), ParamArray[i].Len());
		if (!bRes)
		{
			return false;
		}
	}

	return true;
}

bool FFunction_FindOwnVal::ParseFunctionParameters(const TCHAR *ParameterChar)
{
	//解析、创建参数表达式
	bool bRes = IFunction::ParseFunctionParameters(ParameterChar);
	if (!bRes)
	{
		return false;
	}

	//校验参数数量只能为一
	if (NumParams != 1)
	{
		return false;
	}

	//校验参数类型:参数返回类型必须为字符串
	if (ppParams[0]->GetFormulaReturnType() != IOperand::EOpType_String)
	{
		return false;
	}

	return true;
}

IOperand* FFunction_FindOwnVal::CalculateFunctionRetValue()
{
	//获取属性名
	FAny* AnyValue = ppParams[0]->CalculateFormula();
	check(AnyValue->IsSameType<FString>());
	FString PVal = *AnyCast<FString>(AnyValue);

	//计算属性值
	check(MainFormula->OnFindOwnValDelegate.IsBound());
	FAny* ResultAny = MainFormula->OnFindOwnValDelegate.Execute(PVal);

	//根据属性值类型赋值
	IOperand* ResultOperand = nullptr;
	if (ResultAny->IsSameType<bool>())
	{
		ResultOperand = new FBooleanOperand();
		ResultOperand->SetValue<bool>(*AnyCast<bool>(ResultAny));
	}
	else if (ResultAny->IsSameType<FString>())
	{
		ResultOperand = new FStringOperand();
		ResultOperand->SetValue<FString>(*AnyCast<FString>(ResultAny));
	}
	else if (ResultAny->IsSameType<float>())
	{
		ResultOperand = new FNumberOperand();
		ResultOperand->SetValue<float>(*AnyCast<float>(ResultAny));
	}

	return ResultOperand;
}

bool FFunction_FindEntValue::ParseFunctionParameters(const TCHAR *ParameterChar)
{
	bool bRes = IFunction::ParseFunctionParameters(ParameterChar);
	if (!bRes)
	{
		return false;
	}

	//校验参数数量只能为三
	if (NumParams != 3)
	{
		return false;
	}

	//校验参数类型:三个参数的返回类型必须为字符串
	if ((ppParams[0]->GetFormulaReturnType() != IOperand::EOpType_String) ||
		(ppParams[1]->GetFormulaReturnType() != IOperand::EOpType_String) ||
		(ppParams[2]->GetFormulaReturnType() != IOperand::EOpType_String))
	{
		return false;
	}

	return true;
}

IOperand* FFunction_FindEntValue::CalculateFunctionRetValue()
{
	//获取对象Typ
	FAny* AnyValue0 = ppParams[0]->CalculateFormula();
	check(AnyValue0->IsSameType<FString>());
	FString TypeStr = *AnyCast<FString>(AnyValue0);
	int32 ShapeType = FCString::Atoi(TypeStr.GetCharArray().GetData());

	//获取对象ID
	FAny* AnyValue1 = ppParams[1]->CalculateFormula();
	check(AnyValue1->IsSameType<FString>());
	FString IdStr = *AnyCast<FString>(AnyValue1);
	int64 ShapeId = FCString::Atoi64(IdStr.GetCharArray().GetData());

	//获取属性名
	FAny* AnyValue2 = ppParams[2]->CalculateFormula();
	check(AnyValue2->IsSameType<FString>());
	FString ValStr = *AnyCast<FString>(AnyValue2);

	//计算属性值
	FAny* ResultAny = nullptr;
	check(MainFormula->OnFindEntFuncDelegate.IsBound());
	ResultAny = MainFormula->OnFindEntFuncDelegate.Execute(ShapeType, ShapeId, ValStr);
	if (!ResultAny)
	{
		return nullptr;
	}

	//根据属性值类型赋值
	IOperand* ResultOperand = nullptr;
	if (ResultAny->IsSameType<bool>())
	{
		ResultOperand = new FBooleanOperand();
		ResultOperand->SetValue<bool>(*AnyCast<bool>(ResultAny));
	}
	else if (ResultAny->IsSameType<FString>())
	{
		ResultOperand = new FStringOperand();
		ResultOperand->SetValue<FString>(*AnyCast<FString>(ResultAny));
	}
	else if (ResultAny->IsSameType<float>())
	{
		ResultOperand = new FNumberOperand();
		ResultOperand->SetValue<float>(*AnyCast<float>(ResultAny));
	}

	return ResultOperand;
}

bool FFunction_Pval::ParseFunctionParameters(const TCHAR *ParameterChar)
{
	bool bRes = IFunction::ParseFunctionParameters(ParameterChar);
	if (!bRes)
	{
		return false;
	}

	//校验参数数量只能为一
	if (NumParams != 1)
	{
		return false;
	}

	//校验参数类型:参数返回类型必须为字符串
	if (ppParams[0]->GetFormulaReturnType() != IOperand::EOpType_String)
	{
		return false;
	}

	return true;
}

IOperand* FFunction_Pval::CalculateFunctionRetValue()
{
	//获取属性名
	FAny* AnyValue = ppParams[0]->CalculateFormula();
	check(AnyValue->IsSameType<FString>());
	FString PVal = *AnyCast<FString>(AnyValue);

	//计算属性值
	check(MainFormula->OnPValFuncDelegate.IsBound());
	FAny* ResultAny = MainFormula->OnPValFuncDelegate.Execute(PVal);

	//根据属性值类型赋值
	IOperand* ResultOperand = nullptr;
	if (ResultAny->IsSameType<bool>())
	{
		ResultOperand = new FBooleanOperand();
		ResultOperand->SetValue<bool>(*AnyCast<bool>(ResultAny));
	}
	else if (ResultAny->IsSameType<FString>())
	{
		ResultOperand = new FStringOperand();
		ResultOperand->SetValue<FString>(*AnyCast<FString>(ResultAny));
	}
	else if (ResultAny->IsSameType<float>())
	{
		ResultOperand = new FNumberOperand();
		ResultOperand->SetValue<float>(*AnyCast<float>(ResultAny));
	}

	return ResultOperand;
}

bool FFunction_Cond::ParseFunctionParameters(const TCHAR *ParameterChar)
{
	bool bRes = IFunction::ParseFunctionParameters(ParameterChar);
	if (!bRes)
	{
		return false;
	}

	//校验参数数量:参数数量至少为3，且参数数量为基数
	if (NumParams < 3 || NumParams % 2 == 0)
	{
		return false;
	}

	//校验参数类型:
	IOperand::EOperandType FirstType = IOperand::EOpType_Number;
	for (int32 i = 0; i < NumParams - 1; ++i)
	{
		if (i % 2 == 0)
		{ // cond条件的第偶数个参数为bool类型
			if (ppParams[i]->GetFormulaReturnType() != IOperand::EOpType_Boolean)
			{
				return false;
			}
		}
		else
		{
			if (i == 1) // 获取索引值为1的参数类型，所有后面的第奇数个参数类型都必须和这个类型相同
			{
				FirstType = ppParams[i]->GetFormulaReturnType();
			}
			else if (ppParams[i]->GetFormulaReturnType() != FirstType)
			{
				return false;
			}
		}
	}

	return true;
}

IOperand* FFunction_Cond::CalculateFunctionRetValue()
{
	IOperand* ResultOperand = nullptr;
	for (int i = 0; i < NumParams; i++)
	{
		bool bFinish = false;
		bool Condition = false;
		if (i == NumParams - 1)  //最后一个元素，则该元素即为函数结果
		{
			--i;  //此处减一为后面算法统一
			bFinish = true;
		}
		else  //非最后一个元素，则该元素为bool型条件值，计算表达式结果
		{
			FAny* CurrentAny = ppParams[i]->CalculateFormula();
			check(CurrentAny->IsSameType<bool>());
			Condition = *AnyCast<bool>(CurrentAny);
		}

		//最后一个元素或条件为真，则后面一个元素即为函数结果
		if (bFinish || Condition)
		{
			FAny* ResultAny = ppParams[i + 1]->CalculateFormula();
			if (ResultAny->IsSameType<float>())
			{
				float ResultInt = *AnyCast<float>(ResultAny);
				ResultOperand = new FNumberOperand();
				ResultOperand->SetValue<float>(ResultInt);
			}
			else if (ResultAny->IsSameType<FString>())
			{
				FString Str = *AnyCast<FString>(ResultAny);
				ResultOperand = new FStringOperand();
				ResultOperand->SetValue<FString>(Str);
			}
			else if (ResultAny->IsSameType<bool>())
			{
				bool ResultB = *AnyCast<bool>(ResultAny);
				ResultOperand = new FBooleanOperand();
				ResultOperand->SetValue<bool>(ResultB);
			}

			break;  //计算结束，退出for循环
		}
		else
		{
			i++;
		}
	}

	return ResultOperand;
}

bool FFunction_If::ParseFunctionParameters(const TCHAR *ParameterChar)
{
	bool bRes = IFunction::ParseFunctionParameters(ParameterChar);
	if (!bRes)
	{
		return false;
	}

	//校验参数数量只能为三
	if (NumParams != 3)
	{
		return false;
	}

	//校验参数类型:第一个参数的返回类型必须为Bool，且第二个参数和第三个参数的返回类型必须相同
	if ((ppParams[0]->GetFormulaReturnType() != IOperand::EOpType_Boolean) ||
		((ppParams[1]->GetFormulaReturnType() != (ppParams[2]->GetFormulaReturnType()))))
	{
		return false;
	}

	return true;
}

IOperand* FFunction_If::CalculateFunctionRetValue()
{
	IOperand* ResultOperand = nullptr;

	FAny* ConAny = ppParams[0]->CalculateFormula();
	check(ConAny->IsSameType<bool>());
	bool Condition = *AnyCast<bool>(ConAny);

	FAny* ResultAny = nullptr;
	if (Condition)
	{
		ResultAny = ppParams[1]->CalculateFormula();
	}
	else
	{
		ResultAny = ppParams[2]->CalculateFormula();
	}

	//计算函数结果
	if (ResultAny->IsSameType<float>())
	{
		float ResultInt = *AnyCast<float>(ResultAny);
		ResultOperand = new FNumberOperand();
		ResultOperand->SetValue<float>(ResultInt);
	}
	else if (ResultAny->IsSameType<FString>())
	{
		FString ResultStr = *AnyCast<FString>(ResultAny);
		ResultOperand = new FStringOperand();
		ResultOperand->SetValue<FString>(ResultStr);
	}
	else if (ResultAny->IsSameType<bool>())
	{
		bool ResultB = *AnyCast<bool>(ResultAny);
		ResultOperand = new FBooleanOperand();
		ResultOperand->SetValue<bool>(ResultB);
	}

	return ResultOperand;
}

bool FFunction_Contxt::ParseFunctionParameters(const TCHAR *ParameterChar)
{
	bool bRes = IFunction::ParseFunctionParameters(ParameterChar);
	if (!bRes)
	{
		return false;
	}

	//校验参数数量：必须大于1
	if (NumParams <= 1)
	{
		return false;
	}

	//校验参数类型：所有参数的返回类型都是字符串
	for (int32 i = 0; i < NumParams; ++i)
	{
		if (ppParams[i]->GetFormulaReturnType() != IOperand::EOpType_String)
		{
			return false;
		}
	}

	return true;
}

IOperand* FFunction_Contxt::CalculateFunctionRetValue()
{
	FString ResultStr;
	for (int i = 0; i < NumParams; i++)
	{
		FAny* AnyValue = ppParams[i]->CalculateFormula();
		ResultStr += *AnyCast<FString>(AnyValue);

		if (i != NumParams - 1)
		{
			ResultStr += TEXT(",");
		}
	}

	IOperand* ResultOperand = new FNumberOperand();
	ResultOperand->SetValue<FString>(ResultStr);

	return ResultOperand;
}

bool FFunction_Gllink::ParseFunctionParameters(const TCHAR *ParameterChar)
{
	bool bRes = IFunction::ParseFunctionParameters(ParameterChar);
	if (!bRes)
	{
		return false;
	}

	//校验参数数量：只能为4
	if (NumParams != 4)
	{
		return false;
	}

	//校验参数类型：
	if (ppParams[0]->GetFormulaReturnType() != IOperand::EOpType_String)
	{
		return false;
	}
	if (ppParams[1]->GetFormulaReturnType() != IOperand::EOpType_Number)
	{
		return false;
	}
	if (ppParams[2]->GetFormulaReturnType() != IOperand::EOpType_Number)
	{
		return false;
	}
	if (ppParams[3]->GetFormulaReturnType() != IOperand::EOpType_Number)
	{
		return false;
	}

	return true;
}

IOperand* FFunction_Gllink::CalculateFunctionRetValue()
{
	//字符串
	FAny* StringAny = ppParams[0]->CalculateFormula();
	FString InputStr = *AnyCast<FString>(StringAny);

	//分组参数
	TArray<int32> Parameters;
	for (int i = 1; i < NumParams; i++)
	{
		FAny* NumberAny = ppParams[i]->CalculateFormula();
		Parameters.Push((int32)*AnyCast<float>(NumberAny));
	}

	//将字符串根据‘:’拆分
	TArray<FString> SplitedStrArray;
	while (1)
	{
		FString Left, Right;
		InputStr.Split(TEXT(":"), &Left, &Right);
		SplitedStrArray.Push(Left);

		if (Right.Find(TEXT(":")) == -1)
		{
			SplitedStrArray.Push(Right);
			break;
		}
		InputStr = Right;
	}

	//计算查找分组子字符串
	IOperand* ResultOperand = nullptr;
	FString ResultStr = SplitedStrArray[Parameters[0] * (Parameters[1] - 1) + (Parameters[2] - 1)];
	if (FCString::Atoi(ResultStr.GetCharArray().GetData()) != 0)
	{
		ResultOperand = new FNumberOperand();
		ResultOperand->SetValue<float>(FCString::Atoi(ResultStr.GetCharArray().GetData()));
	}
	else
	{
		ResultOperand = new FStringOperand();
		ResultOperand->SetValue<FString>(ResultStr);
	}
	
	return ResultOperand;
}

bool FFunction_Limitdim::ParseFunctionParameters(const TCHAR *ParameterChar)
{
	bool bRes = IFunction::ParseFunctionParameters(ParameterChar);
	if (!bRes)
	{
		return false;
	}

	//校验参数数量：只能为3
	if (NumParams != 3)
	{
		return false;
	}

	//校验参数类型：所有参数的返回类型都是数字
	for (int32 i = 0; i < NumParams; ++i)
	{
		if (ppParams[i]->GetFormulaReturnType() != IOperand::EOpType_Number)
		{
			return false;
		}
	}

	return true;
}

IOperand* FFunction_Limitdim::CalculateFunctionRetValue()
{
	TArray<int32> Parameters;
	for (int i = 0; i < NumParams; i++)
	{
		FAny* AnyValue = ppParams[i]->CalculateFormula();
		Parameters.Push((int32)*AnyCast<float>(AnyValue));
	}

	int32 ResultInt;
	if (Parameters[0] < Parameters[1])
		ResultInt = Parameters[1];
	else if (Parameters[0] > Parameters[2])
		ResultInt = Parameters[2];
	else
		ResultInt = Parameters[0];

	IOperand* ResultOperand = new FNumberOperand();
	ResultOperand->SetValue<float>(ResultInt);

	return ResultOperand;
}


IFunction* CreateFunction(const TCHAR *lpszSignature, FFormula *pFormula)
{
	if (FCString::Strcmp(lpszSignature, TEXT("findentvalue")) == 0)
		return new FFunction_FindEntValue(pFormula);
	else if (FCString::Strcmp(lpszSignature, TEXT("pval")) == 0)
		return new FFunction_Pval(pFormula);
	else if (FCString::Strcmp(lpszSignature, TEXT("@")) == 0)
		return new FFunction_FindOwnVal(pFormula);
	else if (FCString::Strcmp(lpszSignature, TEXT("cond")) == 0)
		return new FFunction_Cond(pFormula);
	else if (FCString::Strcmp(lpszSignature, TEXT("if")) == 0)
		return new FFunction_If(pFormula);
	else if (FCString::Strcmp(lpszSignature, TEXT("contxt")) == 0)
		return new FFunction_Contxt(pFormula);
	else if (FCString::Strcmp(lpszSignature, TEXT("gllink")) == 0)
		return new FFunction_Gllink(pFormula);
	else if (FCString::Strcmp(lpszSignature, TEXT("limitdim")) == 0)
		return new FFunction_Limitdim(pFormula);
	else
		return nullptr;
}

void DestroyFunction(IFunction *pFunction)
{
	delete pFunction;
}

IFunction* ParseFunction(const TCHAR *lpszFormula, int32 &nIndex, FFormula *MainFormula)
{
	int32 Len = FCString::Strlen(lpszFormula);

	//解析函数签名
	FString signature; signature.Reserve(31);
	while (lpszFormula[nIndex] >= TEXT('a') && lpszFormula[nIndex] <= TEXT('z') && nIndex < Len)
	{
		signature.GetCharArray().Emplace(lpszFormula[nIndex++]);
	}
	signature.GetCharArray().Emplace(TEXT('\0'));
	if (FFormula::s_FunctionSignatures.Find(signature) == nullptr)
	{
		return nullptr;
	}

	//获取参数字符串
	FString parameterStr; parameterStr.Reserve(255);
	++nIndex;  // 跳过函数的左括号
	size_t nMatchedBrace = 1;
	while (nIndex < Len)
	{
		char c = lpszFormula[nIndex++];
		if (c == '(')
			++nMatchedBrace;
		else if (c == ')')
		{
			--nMatchedBrace;
			if (nMatchedBrace == 0)	break;
		}
		parameterStr.GetCharArray().Emplace(c);
	}
	parameterStr.GetCharArray().Emplace(TEXT('\0'));
	if (nMatchedBrace > 0 || parameterStr.IsEmpty())
	{
		return nullptr;  // 括号不匹配或未传参数
	}

	IFunction* NewFunction = CreateFunction(signature.GetCharArray().GetData(), MainFormula);

	//解析函数参数
	bool bRes = NewFunction->ParseFunctionParameters(parameterStr.GetCharArray().GetData());
	if (!bRes)
	{
		//delete NewFunction;
		//有时候delete会崩溃zzq
		NewFunction = nullptr;
	}
	return NewFunction;
}

IFunction* ParseFunctionVar(const TCHAR* lpszFormula, int32 &nIndex, FFormula *MainFormula)
{
	//创建@函数
	FString signature; signature.Reserve(25);
	signature.GetCharArray().Emplace(TEXT('@'));
	signature.GetCharArray().Emplace(TEXT('\0'));
	IFunction *pFunction = CreateFunction(signature.GetCharArray().GetData(), MainFormula);

	//获取参数字符串
	FString parameterStr; parameterStr.Reserve(31);
	parameterStr.GetCharArray().Emplace(TEXT('"'));
	while ((lpszFormula[nIndex] >= TEXT('0') && lpszFormula[nIndex] <= TEXT('9')) ||
		(lpszFormula[nIndex] >= TEXT('A') && lpszFormula[nIndex] <= TEXT('Z')))
	{
		parameterStr.GetCharArray().Emplace(lpszFormula[nIndex++]);
	}
	parameterStr.GetCharArray().Emplace(TEXT('"'));
	parameterStr.GetCharArray().Emplace(TEXT('\0'));

	//解析函数参数
	bool bRes = pFunction->ParseFunctionParameters(parameterStr.GetCharArray().GetData());
	if (!bRes)
	{
		//delete pFunction;
		//有时候delete会崩溃zzq
		pFunction = nullptr;
	}
	return pFunction;
}

