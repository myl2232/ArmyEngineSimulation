#pragma once

#include "Any.h"
#include "Elem.h"

//公式的操作数元素
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

	/** 获取公式元素类型 */
	virtual EFormulaElementType GetType() const override;

	/** 获取/设置操作数的值 */
	template <class ValueType>
	void SetValue(const ValueType &value) { Value = value; }
	FAny* GetValue() const;

	/** 判断两个操作数的类型是否相同 */
	bool IsSameType(IOperand *Other);

	/** 获取操作数类型 */
	virtual EOperandType GetOperandType() const = 0;

	/** 操作数的相关操作 */
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

//数字操作数
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

//字符串操作数
class FStringOperand : public IOperand 
{
public:
	FStringOperand();
	virtual EOperandType GetOperandType() const override;
	virtual bool Equal(IOperand *Other) override;
	virtual bool NotEqual(IOperand *Other) override;
};

//布尔操作数
class FBooleanOperand : public IOperand
{
public:
	FBooleanOperand();
	virtual EOperandType GetOperandType() const override;
	virtual bool And(IOperand *Other) override;
	virtual bool Or(IOperand *Other) override;
};

/** 根据操作数类型创建操作数 */
IOperand* CreateOperand(IOperand::EOperandType nType);
/** 销毁操作数 */
void DestroyOperand(IOperand *pOperand);