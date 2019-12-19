#pragma once

#include "Operand.h"

// ��ʽ�Ĳ�����Ԫ��
class IOperator : public IFormulaElement
{
public:
	virtual ~IOperator() {}
	virtual EFormulaElementType GetType() const override
	{
		return EFormulaElementType::EFEType_Operator;
	}

	/** ��ȡ������������ȼ� */
	virtual int GetPriority() const = 0;
	/** ��ȡ���������������� */
	virtual const TCHAR* GetLiteralName() const = 0;
	/** ��ȡ������������ĸ��ʶ */
	virtual TCHAR GetShortcut() const = 0;
	/** У��������Ƿ����Ӧ�Ĳ�������ƥ�� */
	virtual IOperand* ValidateOperandType(IOperand* Operand1, IOperand* Operand2) = 0;
	/** ����������������� */
	virtual IOperand* Calculate(IOperand *op1, IOperand *op2) = 0;
};

//����
class FBraceOperator : public IOperator
{
public:
	virtual IOperand* ValidateOperandType(IOperand* Operand1, IOperand* Operand2) override;
	virtual IOperand* Calculate(IOperand *op1, IOperand *op2) override;
	virtual int GetPriority() const override { return -1; }
	virtual const TCHAR* GetLiteralName() const override { return TEXT("("); }
	virtual TCHAR GetShortcut() const override { return TEXT('('); }
};

// cond
class FConditionOperator : public IOperator
{
public:
	virtual IOperand* ValidateOperandType(IOperand* Operand1, IOperand* Operand2) override;
	virtual IOperand* Calculate(IOperand *op1, IOperand *op2) override;
	virtual int GetPriority() const override { return 0; }
	virtual const TCHAR* GetLiteralName() const override { return TEXT("?"); }
	virtual TCHAR GetShortcut() const override { return TEXT('?'); }
};

// and
class FAndOperator : public IOperator
{
public:
	virtual IOperand* ValidateOperandType(IOperand* Operand1, IOperand* Operand2) override;
	virtual IOperand* Calculate(IOperand *op1, IOperand *op2) override;
	virtual int GetPriority() const override { return 1; }
	virtual const TCHAR* GetLiteralName() const override { return TEXT("AND"); }
	virtual TCHAR GetShortcut() const override { return TEXT('A'); }
};

// or
class FOrOperator : public IOperator
{
public:
	virtual IOperand* ValidateOperandType(IOperand* Operand1, IOperand* Operand2) override;
	virtual IOperand* Calculate(IOperand *op1, IOperand *op2) override;
	virtual int GetPriority() const override { return 1; }
	virtual const TCHAR* GetLiteralName() const override { return TEXT("OR"); }
	virtual TCHAR GetShortcut() const override { return TEXT('O'); }
};

// ==
class FEqualOperator : public IOperator
{
public:
	virtual IOperand* ValidateOperandType(IOperand* Operand1, IOperand* Operand2) override;
	virtual IOperand* Calculate(IOperand *op1, IOperand *op2) override;
	virtual int GetPriority() const override { return 2; }
	virtual const TCHAR* GetLiteralName() const override { return TEXT("="); }
	virtual TCHAR GetShortcut() const override { return TEXT('='); }
};

// !=
class FNotEqualOperator : public IOperator
{
public:
	virtual IOperand* ValidateOperandType(IOperand* Operand1, IOperand* Operand2) override;
	virtual IOperand* Calculate(IOperand *op1, IOperand *op2) override;
	virtual int GetPriority() const override { return 2; }
	virtual const TCHAR* GetLiteralName() const override { return TEXT("!="); }
	virtual TCHAR GetShortcut() const override { return TEXT('!'); }
};

// >
class FGreaterOperator : public IOperator
{
public:
	virtual IOperand* ValidateOperandType(IOperand* Operand1, IOperand* Operand2) override;
	virtual IOperand* Calculate(IOperand *op1, IOperand *op2) override;
	virtual int GetPriority() const override { return 2; }
	virtual const TCHAR* GetLiteralName() const override { return TEXT(">"); }
	virtual TCHAR GetShortcut() const override { return TEXT('>'); }
};

// <
class FLessOperator : public IOperator
{
public:
	virtual IOperand* ValidateOperandType(IOperand* Operand1, IOperand* Operand2) override;
	virtual IOperand* Calculate(IOperand *op1, IOperand *op2) override;
	virtual int GetPriority() const override { return 2; }
	virtual const TCHAR* GetLiteralName() const override { return TEXT("<"); }
	virtual TCHAR GetShortcut() const override { return TEXT('<'); }
};

// >=
class FGreaterEqualOperator : public IOperator
{
public:
	virtual IOperand* ValidateOperandType(IOperand* Operand1, IOperand* Operand2) override;
	virtual IOperand* Calculate(IOperand *op1, IOperand *op2) override;
	virtual int GetPriority() const override { return 2; }
	virtual const TCHAR* GetLiteralName() const override { return TEXT(">="); }
	virtual TCHAR GetShortcut() const override { return TEXT('>'); }
};

// <=
class FLessEqualOperator : public IOperator
{
public:
	virtual IOperand* ValidateOperandType(IOperand* Operand1, IOperand* Operand2) override;
	virtual IOperand* Calculate(IOperand *op1, IOperand *op2) override;
	virtual int GetPriority() const override { return 2; }
	virtual const TCHAR* GetLiteralName() const override { return TEXT("<="); }
	virtual TCHAR GetShortcut() const override { return TEXT('<'); }
};

// +
class FAddOperator : public IOperator
{
public:
	virtual IOperand* ValidateOperandType(IOperand* Operand1, IOperand* Operand2) override;
	virtual IOperand* Calculate(IOperand *op1, IOperand *op2) override;
	virtual int GetPriority() const override { return 3; }
	virtual const TCHAR* GetLiteralName() const override { return TEXT("+"); }
	virtual TCHAR GetShortcut() const override { return TEXT('+'); }
};

// -
class FSubOperator : public IOperator
{
public:
	virtual IOperand* ValidateOperandType(IOperand* Operand1, IOperand* Operand2) override;
	virtual IOperand* Calculate(IOperand *op1, IOperand *op2) override;
	virtual int GetPriority() const override { return 3; }
	virtual const TCHAR* GetLiteralName() const override { return TEXT("-"); }
	virtual TCHAR GetShortcut() const override { return TEXT('-'); }
};

// *
class FMulOperator : public IOperator
{
public:
	virtual IOperand* ValidateOperandType(IOperand* Operand1, IOperand* Operand2) override;
	virtual IOperand* Calculate(IOperand *op1, IOperand *op2) override;
	virtual int GetPriority() const override { return 4; }
	virtual const TCHAR* GetLiteralName() const override { return TEXT("*"); }
	virtual TCHAR GetShortcut() const override { return TEXT('*'); }
};

// /
class FDivOperator : public IOperator
{
public:
	virtual IOperand* ValidateOperandType(IOperand* Operand1, IOperand* Operand2) override;
	virtual IOperand* Calculate(IOperand *op1, IOperand *op2) override;
	virtual int GetPriority() const override { return 4; }
	virtual const TCHAR* GetLiteralName() const override { return TEXT("/"); }
	virtual TCHAR GetShortcut() const override { return TEXT('/'); }
};