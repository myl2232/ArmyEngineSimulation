#pragma once
#include "CoreMinimal.h"
#include "Operand.h"

class FFormula;
class FSubFormula;

//公式的函数元素
class IFunction : public IFormulaElement
{
public:
	IFunction(FFormula *pFormula);
	virtual ~IFunction();

	/** 返回公式元素类型为函数类型 */
	virtual EFormulaElementType GetType() const override { return EFormulaElementType::EFEType_Function; }

	/** 解析函数参数 */
	virtual bool ParseFunctionParameters(const TCHAR *lpszContent);

	/** 计算函数结果值 */
	virtual IOperand* CalculateFunctionRetValue() = 0;

protected:
	FFormula* MainFormula;
	//公式的参数（嵌套公式）
	FSubFormula** ppParams;
	//公式参数数量
	int32 NumParams;
};

// 获取对象属性值函数
class FFunction_FindOwnVal : public IFunction
{
public:
	FFunction_FindOwnVal(FFormula *pFormula)
		: IFunction(pFormula) {}
	virtual bool ParseFunctionParameters(const TCHAR *lpszContent) override;
	virtual IOperand* CalculateFunctionRetValue() override;
};

// 获取兄弟对象属性值函数
class FFunction_FindEntValue : public IFunction
{
public:
	FFunction_FindEntValue(FFormula *pFormula)
		: IFunction(pFormula) {}
	virtual bool ParseFunctionParameters(const TCHAR *lpszContent) override;
	virtual IOperand* CalculateFunctionRetValue() override;
};

// 获取父级对象属性值函数
class FFunction_Pval : public IFunction
{
public:
	FFunction_Pval(FFormula *pFormula)
		: IFunction(pFormula) {}
	virtual bool ParseFunctionParameters(const TCHAR *lpszContent) override;
	virtual IOperand* CalculateFunctionRetValue() override;
};

// 多条件判断函数
class FFunction_Cond : public IFunction
{
public:
	FFunction_Cond(FFormula *pFormula)
		: IFunction(pFormula) {}
	virtual bool ParseFunctionParameters(const TCHAR *lpszContent) override;
	virtual IOperand* CalculateFunctionRetValue() override;
};

// 选择函数
class FFunction_If : public IFunction
{
public:
	FFunction_If(FFormula *pFormula)
		: IFunction(pFormula) {}

	virtual bool ParseFunctionParameters(const TCHAR *lpszContent) override;
	virtual IOperand* CalculateFunctionRetValue() override;
};

// 文本连接函数
class FFunction_Contxt : public IFunction
{
public:
	FFunction_Contxt(FFormula *pFormula)
		: IFunction(pFormula) {}
	virtual bool ParseFunctionParameters(const TCHAR *lpszContent) override;
	virtual IOperand* CalculateFunctionRetValue() override;
};

// 分组查询
class FFunction_Gllink : public IFunction
{
public:
	FFunction_Gllink(FFormula *pFormula)
		: IFunction(pFormula) {}
	virtual bool ParseFunctionParameters(const TCHAR *lpszContent) override;
	virtual IOperand* CalculateFunctionRetValue() override;
};

// 限定尺寸函数
class FFunction_Limitdim : public IFunction
{
public:
	FFunction_Limitdim(FFormula *pFormula)
		: IFunction(pFormula) {}
	virtual bool ParseFunctionParameters(const TCHAR *lpszContent) override;
	virtual IOperand* CalculateFunctionRetValue() override;
};

/** 根据函数名称创建函数 */
IFunction* CreateFunction(const TCHAR *lpszSignature, FFormula *pFormula);
/** 销毁函数 */
void DestroyFunction(IFunction *pFunction);

/** 通过解析字符串表达式返回函数 */
IFunction* ParseFunction(const TCHAR *lpszFormula, int32 &nIndex, FFormula *MainFormula);
/** 解析@函数 */
IFunction* ParseFunctionVar(const TCHAR *lpszFormula, int32 &nIndex, FFormula *MainFormula);
