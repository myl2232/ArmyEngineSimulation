#pragma once

#include "Any.h"
#include "Elem.h"

//��ʽ�Ĳ�����Ԫ��
struct IOperand : public IFormulaElement 
{
	enum EOperandType
	{
		EOpType_Number,
		EOpType_String,
		EOpType_Boolean
	};

public:
	IOperand();
	virtual ~IOperand();

	/** ��ȡ��ʽԪ������ */
	virtual EFormulaElementType GetType() const override;

	/** ��ȡ/���ò�������ֵ */
	template <class ValueType>
	void SetValue(const ValueType &value) { Value = value; }
	FAny* GetValue() const;

	/** �ж������������������Ƿ���ͬ */
	bool IsSameType(IOperand *Other);

	/** ��ȡ���������� */
	virtual EOperandType GetOperandType() const = 0;

	/** ����������ز��� */
	virtual bool And(IOperand *Other) { return false; }
	virtual bool Or(IOperand *Other) { return false; }
	virtual bool Equal(IOperand *Other) { return false; }
	virtual bool NotEqual(IOperand *Other) { return false; }
	virtual bool Greater(IOperand *Other) { return false; }
	virtual bool Less(IOperand *Other) { return false; }
	virtual bool GEqual(IOperand *Other) { return false; }
	virtual bool LEqual(IOperand *Other) { return false; }
	virtual float Add(IOperand *Other) { return 0.0; }
	virtual float Sub(IOperand *Other) { return 0.0; }
	virtual float Mul(IOperand *Other) { return 0.0; }
	virtual float Div(IOperand *Other) { return 0.0; }

public:
	FAny Value;
};

//���ֲ�����
class FNumberOperand : public IOperand 
{
public:
	FNumberOperand();
	virtual EOperandType GetOperandType() const override;
	virtual bool Equal(IOperand *Other) override;
	virtual bool NotEqual(IOperand *Other) override;
	virtual bool Greater(IOperand *Other) override;
	virtual bool Less(IOperand *Other) override;
	virtual bool GEqual(IOperand *Other) override;
	virtual bool LEqual(IOperand *Other) override;
	virtual float Add(IOperand *Other) override;
	virtual float Sub(IOperand *Other) override;
	virtual float Mul(IOperand *Other) override;
	virtual float Div(IOperand *Other) override;
};

//�ַ���������
class FStringOperand : public IOperand 
{
public:
	FStringOperand();
	virtual EOperandType GetOperandType() const override;
	virtual bool Equal(IOperand *Other) override;
	virtual bool NotEqual(IOperand *Other) override;
};

//����������
class FBooleanOperand : public IOperand
{
public:
	FBooleanOperand();
	virtual EOperandType GetOperandType() const override;
	virtual bool And(IOperand *Other) override;
	virtual bool Or(IOperand *Other) override;
};

/** ���ݲ��������ʹ��������� */
IOperand* CreateOperand(IOperand::EOperandType nType);
/** ���ٲ����� */
void DestroyOperand(IOperand *pOperand);