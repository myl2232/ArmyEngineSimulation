#pragma once
#include "CoreMinimal.h"
#include "Operator.h"

class FFormula;
class IFunction;
// 公式
class FSubFormula
{
public:
	FSubFormula(FFormula *Formula);
	~FSubFormula();

	/** 解析公式 */
	bool ParseFormula(const TCHAR *lpszFormula, int32 nLen);

	/** 更新解析元素结果 */
	void UpdateFormulaElements();

	/** 计算公式值 */
	FAny* CalculateFormula();

	/** 获取公式返回操作数的类型 */
	IOperand::EOperandType GetFormulaReturnType();

	/** 设置是否完成解析结果更新状态 */
	void SetUpdatedFlag(bool bFlag);
	/** 设置外部变量是否改变状态 */
	void SetVariDirtyFlag(bool bFlag);

private:
	/** 解析数字操作数 */
	IOperand* GetNumberOperand(const TCHAR *lpszFormula, int32 &nIndex, bool bPositive);

	/** 解析字符操作数 */
	IOperand* GetStringOperand(const TCHAR *lpszFormula, int32 &nIndex);
	
	/** 解析操作符 */
	TSharedPtr<IOperator> GetOperator(const TCHAR *lpszFormula, int32 &nIndex, int32 nCount);

	/** 清除解析后的元素结果 */
	void ClearParsedElements();
	/** 清除计算用的元素结果 */
	void ClearCalculatedElements();

private:
	// 标识是否已经成功解析校验
	bool bParsed;
	// 标识是否已经更新了解析结果
	bool bUpdated;
	// 标识是否有外部变量改变
	bool bVariDirty;

	// 用于公式计算的公式元素结果
	TArray<TSharedPtr<IFormulaElement>> CalculatedElements;
	// 公式计算结果
	FAny ResultAny;

	// 公式校验后缓存的公式元素结果
	TArray<TSharedPtr<IFormulaElement>> ParsedElements;
	// 公式校验后确认的结果操作数类型
	IOperand::EOperandType EReturnType;

	FFormula *MainFormula;
	TSharedPtr<FSubFormula> LeftFormula;
	TSharedPtr<FSubFormula> RightFormula;
};
