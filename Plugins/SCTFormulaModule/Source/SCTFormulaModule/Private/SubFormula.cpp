#include "SubFormula.h"
#include "Formula.h"
#include "Function.h"
#include "Containers/List.h"

#define VARIABLE_DELIM '@'

FSubFormula::FSubFormula(FFormula *Formula)
:MainFormula(Formula)
{
	bParsed = false;
	bUpdated = false;
	bVariDirty = false;
}

FSubFormula::~FSubFormula()
{
	ClearParsedElements();
	ClearCalculatedElements();
}

// 移除表达式两侧多余括号
void WipeoutBraces(const TCHAR* lpszFormula, int32 nLen, TCHAR*& lpszOutFormula, int32& OutLen)
{
	TDoubleLinkedList<TCHAR> BraceStack;

	int32 i = 0, nIgnoreCount = 0;
	bool bHadOther = false;
	while (i < nLen)
	{
		TCHAR c = lpszFormula[i++];
		if (c == TEXT('('))
		{
			int32 nCount = 0;
			// 当找到一个左括号时，将栈中所有的右括号出栈
			while (BraceStack.Num() > 0 && BraceStack.GetTail()->GetValue() == TEXT(')'))
			{
				BraceStack.RemoveNode(BraceStack.GetTail());
				++nCount;
			}

			// 将所有和右括号对应的左括号出栈
			while (BraceStack.Num() > 0 && nCount > 0)
			{
				BraceStack.RemoveNode(BraceStack.GetTail());
				--nCount;
			}

			if (bHadOther)
				++nIgnoreCount;
			else
				BraceStack.AddTail(c);
		}
		else if (c == TEXT(')'))
		{
			if (nIgnoreCount == 0)
				BraceStack.AddTail(c);
			else
				--nIgnoreCount;
		}
		else
		{
			int32 nCount = 0;
			while (BraceStack.Num() > 0 && BraceStack.GetTail()->GetValue() == TEXT(')'))
			{
				BraceStack.RemoveNode(BraceStack.GetTail());
				++nCount;
			}

			while (BraceStack.Num() > 0 && nCount > 0)
			{
				BraceStack.RemoveNode(BraceStack.GetTail());
				--nCount;
			}

			bHadOther = true;
		}
	}

	lpszOutFormula = const_cast<TCHAR*>(lpszFormula + BraceStack.Num() / 2);
	OutLen = nLen - BraceStack.Num();
}

//根据?:分割表达式
void SplitFormula(const TCHAR *lpszWhole, int32 nLen, TCHAR *&lpszCondition, int32 &nCondLen, TCHAR *&lpszLeft, int32 &nLeftLen, TCHAR *&lpszRight, int32 &nRightLen)
{
	lpszCondition = const_cast<TCHAR*>(lpszWhole);
	nCondLen = nLen;
	lpszLeft = nullptr;
	nLeftLen = 0;
	lpszRight = nullptr;
	nRightLen = 0;

	int32 nIndex = 0, nQuestionIndex = 0, nColonIndex = 0;
	int32 nConditionMet = 0;
	while (nIndex < nLen)
	{
		TCHAR c = lpszWhole[nIndex];
		if (c == TEXT('?'))
		{
			if (nConditionMet == 0) // 第一次在公式中遇到一个?，记录其字符索引位置，这个位置左面的所有字符就是Condition
				nQuestionIndex = nIndex;
			++nConditionMet; // 增加遇到?的次数
		}
		else if (c == TEXT(':'))
		{
			if (nConditionMet == 1) {
				// 遇到:时如果遇到?的次数为1，这个:就是和最外层?匹配的，记录其字符索引
				// 这个索引和之前记录的?索引位置之间的部分就是最外层的左子表达式
				// 右侧为最外层的右子表达式
				nColonIndex = nIndex;
				break;
			}
			else // 否则就减少一次计数，代表找到了一次?和:的匹配
				--nConditionMet;
		}
		++nIndex;
	}

	if (nQuestionIndex > 0)
	{
		nCondLen = nQuestionIndex;
		lpszLeft = const_cast<TCHAR*>(lpszWhole + nQuestionIndex + 1);
		nLeftLen = nColonIndex - nQuestionIndex - 1;
		lpszRight = const_cast<TCHAR*>(lpszWhole + nColonIndex + 1);
		nRightLen = nLen - nColonIndex - 1;
	}
}

bool FSubFormula::ParseFormula(const TCHAR *lpszFormula, int32 nLen)
{
	//清除缓存结果
	ClearParsedElements();
	//bParsed = false;  //只需要在构造函数中置为false

	//移除两侧多余括号
	TCHAR *lpszFormulaWithoutBrace;
	int32 nLenWithoutBrace;
	WipeoutBraces(lpszFormula, nLen, lpszFormulaWithoutBrace, nLenWithoutBrace);

	//根据?:分割表达式
	TCHAR *lpszCondition, *lpszLeft, *lpszRight;
	int32 nCondLen, nLeftLen, nRightLen;
	SplitFormula(lpszFormulaWithoutBrace, nLenWithoutBrace, lpszCondition, nCondLen, lpszLeft, nLeftLen, lpszRight, nRightLen);
	// 左右子式只有一个存在，说明左右子式没有填全
	if ((nLeftLen > 0 && nRightLen == 0) || (nLeftLen == 0 && nRightLen > 0))
	{
		return false;
	}
	// 以'('或'@'结尾说明公式表达式不完整
	if (lpszCondition[nCondLen - 1] == TEXT('(') || lpszCondition[nCondLen - 1] == TEXT('@'))
	{
		return false;
	}

	// 解析表达式（或条件表达式）
	int32 i = 0;
	TDoubleLinkedList<TSharedPtr<IOperator>> TempOps;
	while (i < nCondLen)
	{
		TCHAR c = lpszCondition[i];
		if (c == TEXT('('))
		{
			TempOps.AddTail(*FFormula::s_Operators.Find(TEXT("(")));
		}
		else if (c == TEXT(')'))
		{
			// 将TempOps中的操作符移动到m_FinalElements中，直到TempOps栈顶的操作符是"("
			while (TempOps.Num() > 0 && FCString::Strcmp(TempOps.GetTail()->GetValue()->GetLiteralName(), TEXT("(")))
			{
				TDoubleLinkedList<TSharedPtr<IOperator>>::TDoubleLinkedListNode *Tail = TempOps.GetTail();
				ParsedElements.Push(Tail->GetValue());
				TempOps.RemoveNode(Tail);
			}
			if (TempOps.Num() > 0) // 如果m_TempOps不为空，弹出"("
			{
				TempOps.RemoveNode(TempOps.GetTail());
			}
			else // 如果临时栈空了，说明没有找到左括号，即有对应不上的右括号
			{
				return false;
			}
		}
		else if (c >= TEXT('0') && c <= TEXT('9'))
		{ // 检测到了一个数值类型操作数
			ParsedElements.Push(MakeShareable(GetNumberOperand(lpszCondition, i, true)));
			continue;
		}
		else if (FFormula::s_OperatorShortcuts.Find(c) != nullptr)
		{
			// 检测到负数，如果式子中间有负数，必须使用括号包起来
			if (c == TEXT('-') &&
				(lpszCondition[i + 1] >= TEXT('0') && lpszCondition[i + 1] <= TEXT('9')) &&
				(i == 0 || lpszCondition[i - 1] == TEXT('(')))
			{
				int32 MinusStart = i + 1;
				ParsedElements.Push(MakeShareable(GetNumberOperand(lpszCondition, MinusStart, false)));
				i = MinusStart;
				continue;
			}

			const TSharedPtr<IOperator>& Operator = GetOperator(lpszCondition, i, nCondLen);
			if (!Operator.IsValid()) // 操作符解析失败
			{
				return false;
			}
			// 如果m_TempOps不为空，或者m_TempOps的栈顶操作符优先级大于当前检测到的操作符优先级，
			// 就把m_TempOps的栈顶操作符移动到m_FinalElements中
			while (TempOps.Num() > 0 && TempOps.GetTail()->GetValue()->GetPriority() >= Operator->GetPriority())
			{
				TDoubleLinkedList<TSharedPtr<IOperator>>::TDoubleLinkedListNode *Tail = TempOps.GetTail();
				ParsedElements.Push(Tail->GetValue());
				TempOps.RemoveNode(Tail);
			}
			// 直到m_TempOps为空，或者其栈顶操作符的优先级小于等于当前操作符优先级
			TempOps.AddTail(Operator);
			continue;
		}
		else if (c == VARIABLE_DELIM)
		{ // //将@作为一个函数处理
			++i; // 跳过@
			IFunction* FunctionEle = ParseFunctionVar(lpszFormula, i, MainFormula);
			if (!FunctionEle)
			{
				return false;
			}
			ParsedElements.Push(MakeShareable(FunctionEle));
			continue;
		}
		else if (c <= TEXT('z') && c >= TEXT('a'))
		{ // 有可能是一个函数
			IFunction* FunctionEle = ParseFunction(lpszFormula, i, MainFormula);
			if (!FunctionEle)
			{
				return false;
			}
			ParsedElements.Push(MakeShareable(FunctionEle));
			continue;
		}
		else if (c == TEXT('"'))
		{
			IOperand* IOperandEle = GetStringOperand(lpszFormula, i);
			if (!IOperandEle)
			{
				return false;
			}
			ParsedElements.Push(MakeShareable(IOperandEle));
			continue;
		}
		++i;
	}

	// 整个表达式扫描完毕后，将m_TempOps中剩余的操作符全都添加到m_FinalElements中
	while (TempOps.Num() > 0)
	{
		TDoubleLinkedList<TSharedPtr<IOperator>>::TDoubleLinkedListNode *Tail = TempOps.GetTail();
		ParsedElements.Push(Tail->GetValue());
		TempOps.RemoveNode(Tail);
	}

	//校验操作符和操作数是否匹配
	TDoubleLinkedList<TSharedPtr<IFormulaElement>> Operandstk;
	for (int32 i = 0; i < ParsedElements.Num(); ++i)
	{
		TSharedPtr<IFormulaElement> &Elem = ParsedElements[i];
		if (Elem->GetType() == EFormulaElementType::EFEType_Operand)
		{ // 操作数推入到临时栈
			Operandstk.AddTail(Elem);
		}
		else if (Elem->GetType() == EFormulaElementType::EFEType_Function)
		{ // 函数推入到临时栈
			Operandstk.AddTail(Elem);
		}
		else
		{ // 遇到操作符，从操作数临时栈中弹出两个操作数
			if (Operandstk.Num() < 2)
			{
				return false;  //操作数不足
			}
			TSharedPtr<IFormulaElement> FormulaElem1 = Operandstk.GetTail()->GetValue();
			Operandstk.RemoveNode(Operandstk.GetTail());
			IOperand* Operand1 = nullptr;
			if (FormulaElem1->GetType() == EFEType_Function)
			{
				IFunction* FunElem = static_cast<IFunction*>(FormulaElem1.Get());
				Operand1 = FunElem->CalculateFunctionRetValue();
				if (!Operand1)
				{
					return false;
				}
			} 
			else
			{
				Operand1 = static_cast<IOperand*>(FormulaElem1.Get());
			}

			TSharedPtr<IFormulaElement> FormulaElem2 = Operandstk.GetTail()->GetValue();
			Operandstk.RemoveNode(Operandstk.GetTail());
			IOperand* Operand2 = nullptr;
			if (FormulaElem2->GetType() == EFEType_Function)
			{
				IFunction* FunElem = static_cast<IFunction*>(FormulaElem2.Get());
				Operand2 = FunElem->CalculateFunctionRetValue();
				if (!Operand2)
				{
					return false;
				}
			} 
			else
			{
				Operand2 = static_cast<IOperand*>(FormulaElem2.Get());
			}

			//校验操作数类型是否与对应操作符相匹配
			IOperand* ReturnOperand = ((IOperator*)Elem.Get())->ValidateOperandType(Operand2, Operand1);
			if (ReturnOperand != nullptr)
			{
				Operandstk.AddTail(MakeShareable(ReturnOperand));
			}
			else
			{
				return false;   //操作数类型不匹配
			}
		}
	}
	//保存最后返回的操作数的类型
	TSharedPtr<IFormulaElement> Cond = Operandstk.GetTail()->GetValue();
	Operandstk.RemoveNode(Operandstk.GetTail());

	EFormulaElementType ElementType = Cond->GetType();
	if (ElementType == EFEType_Function)
	{
		IFunction* ReturnFun = static_cast<IFunction*>(Cond.Get());
		IOperand* CalOperand = ReturnFun->CalculateFunctionRetValue();
		if (!CalOperand)
		{
			return false;
		}
		EReturnType = CalOperand->GetOperandType();
	}
	else if (ElementType == EFEType_Operand)
	{
		IOperand *ReturnOp = static_cast<IOperand*>(Cond.Get());
		EReturnType = ReturnOp->GetOperandType();
	}
		
	//计算最后Operandstk应该为空
	if (Operandstk.Num() != 0)
	{
		return false;
	}

	// 如果检测到了条件左右子式，递归进行解析
	if (nLeftLen > 0)
	{
		LeftFormula = MakeShareable(new FSubFormula(MainFormula));
		bool bLeft = LeftFormula->ParseFormula(lpszLeft, nLeftLen);
		if (!bLeft)
		{
			return false;
		}
	}

	if (nRightLen > 0)
	{
		RightFormula = MakeShareable(new FSubFormula(MainFormula));
		bool bRight = RightFormula->ParseFormula(lpszRight, nRightLen);
		if (!bRight)
		{
			return false;
		}
	}

	//解析校验成功后需要更新
	bParsed = true;
	bUpdated = false;
	return true;
}

void FSubFormula::UpdateFormulaElements()
{
	ClearCalculatedElements();
	for (int32 i = 0; i < ParsedElements.Num(); ++i)
	{
		CalculatedElements.Push(ParsedElements[i]);
	}
	//更新完成后需要重新计算公式结果
	bUpdated = true;
	bVariDirty = true; 
}

FAny* FSubFormula::CalculateFormula()
{
	//公式解析校验失败或没有初始化
	if (!bParsed)
	{
		return nullptr;
	}
	//有新的解析结果需要更新
	if (!bUpdated)
	{
		UpdateFormulaElements();
	}
	//外部变量没有变化且已缓存了计算结果
// 	if (!bVariDirty)
// 	{
// 		return &ResultAny;
// 	}

	//重新计算公式结果
	TDoubleLinkedList<TSharedPtr<IFormulaElement>> Operandstk;
	for (int32 i = 0; i < CalculatedElements.Num(); ++i)
	{
		TSharedPtr<IFormulaElement> &Elem = CalculatedElements[i];
		if (Elem->GetType() == EFormulaElementType::EFEType_Operand)
		{ // 操作数推入到临时栈
			Operandstk.AddTail(Elem);
		}
		else if (Elem->GetType() == EFormulaElementType::EFEType_Function)
		{ // 函数，计算返回值
			IFunction* FunElem = static_cast<IFunction*>(Elem.Get());
			IOperand* OperandElem = FunElem->CalculateFunctionRetValue();
			Operandstk.AddTail(MakeShareable(OperandElem));
		}
		else
		{ // 遇到操作符，从操作数临时栈中弹出两个操作数
			TSharedPtr<IFormulaElement> Op1 = Operandstk.GetTail()->GetValue();
			IOperand *pOp1 = static_cast<IOperand*>(Op1.Get());
			Operandstk.RemoveNode(Operandstk.GetTail());
			TSharedPtr<IFormulaElement> Op2 = Operandstk.GetTail()->GetValue();
			IOperand *pOp2 = static_cast<IOperand*>(Op2.Get());
			Operandstk.RemoveNode(Operandstk.GetTail());

			//注意操作数的顺序（pOp2是第一个, pOp1是第二个）
			Operandstk.AddTail(MakeShareable(((IOperator *)Elem.Get())->Calculate(pOp2, pOp1)));
		}
	}
	//获取计算结果Element
	TSharedPtr<IFormulaElement> ResultElement = Operandstk.GetTail()->GetValue();
	Operandstk.RemoveNode(Operandstk.GetTail());

	// 如果存在左右子式，则根据Condition决定应该计算哪个子式
	// 如果不存在左右子式，说明表达式不包含分支，直接输出pCond
	if (LeftFormula.IsValid() && RightFormula.IsValid())
	{
		bool bCond = *(AnyCast<bool>((static_cast<IOperand*>(ResultElement.Get()))->GetValue()));
		ResultAny = *(bCond ? LeftFormula->CalculateFormula() : RightFormula->CalculateFormula());
	}
	else
	{
		// 缓存计算结果
		IOperand *ResultOperand = static_cast<IOperand*>(ResultElement.Get());
		ResultAny = *(ResultOperand->GetValue());
	}

	bVariDirty = false;
	return &ResultAny;
}

IOperand::EOperandType FSubFormula::GetFormulaReturnType()
{
	return EReturnType;
}

void FSubFormula::SetUpdatedFlag(bool bFlag)
{
	bUpdated = bFlag;
}

void FSubFormula::SetVariDirtyFlag(bool bFlag)
{
	bVariDirty = bFlag;
}

IOperand* FSubFormula::GetNumberOperand(const TCHAR *lpszFormula, int32 &nIndex, bool bPositive)
{
	FString str; str.Reserve(31);
	while ((lpszFormula[nIndex] >= TEXT('0') && lpszFormula[nIndex] <= TEXT('9')) || lpszFormula[nIndex] == TEXT('.'))
	{
		str.GetCharArray().Emplace(lpszFormula[nIndex++]);
	}
	str.GetCharArray().Emplace(TEXT('\0'));

	IOperand *pOp = CreateOperand(IOperand::EOpType_Number);
	float Value = FCString::Atof(str.GetCharArray().GetData());
	pOp->SetValue<float>(bPositive ? Value : -Value);
	return pOp;
}

IOperand* FSubFormula::GetStringOperand(const TCHAR *lpszFormula, int32 &nIndex)
{
	int32 Len = FCString::Strlen(lpszFormula);

	bool bFoundMatchedQuote = false;
	FString str; str.Reserve(255);
	++nIndex;
	while (nIndex < Len)
	{
		if (lpszFormula[nIndex] == TEXT('"'))
		{
			++nIndex;
			bFoundMatchedQuote = true;
			break;
		}
		str.GetCharArray().Emplace(lpszFormula[nIndex++]);
	}
	str.GetCharArray().Emplace(TEXT('\0'));

	if (!bFoundMatchedQuote)
	{
		return nullptr;
	}

	IOperand *pOp = CreateOperand(IOperand::EOpType_String);
	pOp->SetValue<FString>(str);
	return pOp;
}

TSharedPtr<IOperator> FSubFormula::GetOperator(const TCHAR *lpszFormula, int32 &nIndex, int32 nCount)
{
	FString str; str.Reserve(7);
	while (nIndex < nCount)
	{
		if ((lpszFormula[nIndex] >= TEXT('0') && lpszFormula[nIndex] <= TEXT('9')) ||
			lpszFormula[nIndex] == TEXT('(') || lpszFormula[nIndex] == TEXT(')') ||
			lpszFormula[nIndex] == VARIABLE_DELIM)
			break;
		str.GetCharArray().Emplace(lpszFormula[nIndex++]);
	}
	str.GetCharArray().Emplace(TEXT('\0'));

	bool bContain = FFormula::s_Operators.Contains(str);
	if (!bContain)
	{
		return nullptr;
	}
	return *FFormula::s_Operators.Find(str);
}

void FSubFormula::ClearParsedElements()
{
	ParsedElements.Empty();
}

void FSubFormula::ClearCalculatedElements()
{
	CalculatedElements.Empty();
}