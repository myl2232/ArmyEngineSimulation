#pragma once

enum EFormulaElementType
{
	EFEType_Operand,
	EFEType_Operator,
	EFEType_Function
};

//公式基本元素
struct IFormulaElement 
{
	virtual ~IFormulaElement() {}

	virtual EFormulaElementType GetType() const = 0;
};