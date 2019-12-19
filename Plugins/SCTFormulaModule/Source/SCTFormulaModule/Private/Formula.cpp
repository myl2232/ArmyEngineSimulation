#include "Formula.h"

FString WipeoutSpace(const FString &FormulaStr)
{
	FString Result = FormulaStr;
	return Result.Replace(TEXT(" "), TEXT(""));
}

FFormula::OperatorMap FFormula::s_Operators;
FFormula::OperatorShortcuts FFormula::s_OperatorShortcuts;
FFormula::FunctionSignatures FFormula::s_FunctionSignatures;

void FFormula::InitGlobalResource()
{
	FBraceOperator *pBraceOperator = new FBraceOperator;
	s_Operators.Add(pBraceOperator->GetLiteralName(), MakeShareable(pBraceOperator));
	s_OperatorShortcuts.Add(pBraceOperator->GetShortcut());

	FConditionOperator *pConditionOperator = new FConditionOperator;
	s_Operators.Add(pConditionOperator->GetLiteralName(), MakeShareable(pConditionOperator));
	s_OperatorShortcuts.Add(pConditionOperator->GetShortcut());

	FAndOperator *pAndOperator = new FAndOperator;
	s_Operators.Add(pAndOperator->GetLiteralName(), MakeShareable(pAndOperator));
	s_OperatorShortcuts.Add(pAndOperator->GetShortcut());

	FOrOperator *pOrOperator = new FOrOperator;
	s_Operators.Add(pOrOperator->GetLiteralName(), MakeShareable(pOrOperator));
	s_OperatorShortcuts.Add(pOrOperator->GetShortcut());

	FEqualOperator *pEqOperator = new FEqualOperator;
	s_Operators.Add(pEqOperator->GetLiteralName(), MakeShareable(pEqOperator));
	s_OperatorShortcuts.Add(pEqOperator->GetShortcut());

	FNotEqualOperator *pNEqOperator = new FNotEqualOperator;
	s_Operators.Add(pNEqOperator->GetLiteralName(), MakeShareable(pNEqOperator));
	s_OperatorShortcuts.Add(pNEqOperator->GetShortcut());

	FGreaterOperator *pGreaterOperator = new FGreaterOperator;
	s_Operators.Add(pGreaterOperator->GetLiteralName(), MakeShareable(pGreaterOperator));
	s_OperatorShortcuts.Add(pGreaterOperator->GetShortcut());

	FLessOperator *pLessOperator = new FLessOperator;
	s_Operators.Add(pLessOperator->GetLiteralName(), MakeShareable(pLessOperator));
	s_OperatorShortcuts.Add(pLessOperator->GetShortcut());

	FGreaterEqualOperator *pGreatorEqOperator = new FGreaterEqualOperator;
	s_Operators.Add(pGreatorEqOperator->GetLiteralName(), MakeShareable(pGreatorEqOperator));
	s_OperatorShortcuts.Add(pGreatorEqOperator->GetShortcut());

	FLessEqualOperator *pLessEqOperator = new FLessEqualOperator;
	s_Operators.Add(pLessEqOperator->GetLiteralName(), MakeShareable(pLessEqOperator));
	s_OperatorShortcuts.Add(pLessEqOperator->GetShortcut());

	FAddOperator *pAddOperator = new FAddOperator;
	s_Operators.Add(pAddOperator->GetLiteralName(), MakeShareable(pAddOperator));
	s_OperatorShortcuts.Add(pAddOperator->GetShortcut());

	FSubOperator *pSubOperator = new FSubOperator;
	s_Operators.Add(pSubOperator->GetLiteralName(), MakeShareable(pSubOperator));
	s_OperatorShortcuts.Add(pSubOperator->GetShortcut());

	FMulOperator *pMulOperator = new FMulOperator;
	s_Operators.Add(pMulOperator->GetLiteralName(), MakeShareable(pMulOperator));
	s_OperatorShortcuts.Add(pMulOperator->GetShortcut());

	FDivOperator *pDivOperator = new FDivOperator;
	s_Operators.Add(pDivOperator->GetLiteralName(), MakeShareable(pDivOperator));
	s_OperatorShortcuts.Add(pDivOperator->GetShortcut());

	s_FunctionSignatures.Emplace(TEXT("findentvalue"));
	s_FunctionSignatures.Emplace(TEXT("pval"));
	s_FunctionSignatures.Emplace(TEXT("@"));
	s_FunctionSignatures.Emplace(TEXT("cond"));
	s_FunctionSignatures.Emplace(TEXT("if"));
	s_FunctionSignatures.Emplace(TEXT("contxt"));
	s_FunctionSignatures.Emplace(TEXT("gllink"));
	s_FunctionSignatures.Emplace(TEXT("limitdim"));
}

FFormula::FFormula()
{
	RootSubFormula = MakeShareable(new FSubFormula(this));
}

FFormula::~FFormula()
{
}

bool FFormula::ParseFormula(const FString &FormulaStr)
{
	// 解析字符串
	bool bParseRes = false;
	if (FormulaStr.Contains(TEXT(" ")))
	{
		FString FormulaWithoutSpace = WipeoutSpace(FormulaStr);
		if (!FormulaWithoutSpace.IsEmpty())
		{
			bParseRes = RootSubFormula->ParseFormula(FormulaWithoutSpace.GetCharArray().GetData(), FormulaWithoutSpace.Len());
		}
	}
	else
	{
		bParseRes = RootSubFormula->ParseFormula(FormulaStr.GetCharArray().GetData(), FormulaStr.Len());
	}

	// 字符串解析成功后保存该字符串
	if (bParseRes)
	{
		ParsedFormulaStr = FormulaStr;
	}
	return bParseRes;
}

FAny* FFormula::CalculateFormula(const FString &FormulaStr, bool bForceRecalculate)
{
	//1、强制重新解析
	if (bForceRecalculate)
	{
		 bool bParseRes = ParseFormula(FormulaStr);
		 if (!bParseRes)
		 {
			 return nullptr;
		 }
	}

	//2、输入字符串没有经过解析校验，需要重新解析
	if (FormulaStr != ParsedFormulaStr && FormulaStr != CachedFormulaStr)
	{
		bool bParseRes = ParseFormula(FormulaStr);
		if (!bParseRes)
		{
			return nullptr;
		}
	}

	//3、输入字符串仍然是原来的公式表达式
	if (FormulaStr == CachedFormulaStr)
	{
	   //新验证未提交，无需更新解析结果
	}
	//4、输入字符串是新解析的公式表达式
	else //FormulaStr == ParsedFormulaStr
	{
		RootSubFormula->SetUpdatedFlag(false);
		CachedFormulaStr = ParsedFormulaStr;
	}

	//5、利用公式的解析后元素计算结果
	return RootSubFormula->CalculateFormula();
}

void FFormula::SetVariDirty()
{
	RootSubFormula->SetVariDirtyFlag(true);
}