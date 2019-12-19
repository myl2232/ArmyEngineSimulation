#pragma once

enum EFormulaElementType
{
	EFEType_Operand,
	EFEType_Operator,
	EFEType_Function
};

//��ʽ����Ԫ��
struct IFormulaElement 
{
	virtual ~IFormulaElement() {}

	virtual EFormulaElementType GetType() const = 0;
};