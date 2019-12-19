#pragma once
#include "SubFormula.h"

//公式对外接口类
class SCTFORMULAMODULE_API FFormula
{
public:
	/** 全局初始化公式相关资源 */
	static void InitGlobalResource();
	typedef TMap<FString, TSharedPtr<IOperator>, FDefaultSetAllocator, TDefaultMapKeyFuncs<FString, TSharedPtr<IOperator>, false>> OperatorMap;
	static OperatorMap s_Operators;
	typedef TSet<TCHAR, DefaultKeyFuncs<TCHAR, false>, FDefaultSetAllocator> OperatorShortcuts;
	static OperatorShortcuts s_OperatorShortcuts;
	typedef TSet<FString, DefaultKeyFuncs<FString, false>, FDefaultSetAllocator> FunctionSignatures;
	static FunctionSignatures s_FunctionSignatures;

	FFormula();
	~FFormula();

	/** 解析公式字符串 */
	bool ParseFormula(const FString &FormulaStr);

	/** 计算公式值 */
	FAny* CalculateFormula(const FString &FormulaStr, bool bForceRecalculate = false);

	/** 设置/获取公式字符串 */
	void SetFormulaStr(const FString &FormulaStr) { CachedFormulaStr = FormulaStr; }
	const FString& GetFormulaStr() const { return CachedFormulaStr; }

	/** 设置外部变量改变状态 */
	void SetVariDirty();

	/** 获取自己变量属性值 */
	DECLARE_DELEGATE_RetVal_OneParam(FAny*, FOnFunc_OwnVal, const FString&);
	FOnFunc_OwnVal OnFindOwnValDelegate;

	/** 获取父级对象属性值代理 */
	DECLARE_DELEGATE_RetVal_OneParam(FAny*, FOnFunc_PVal, const FString&);
	FOnFunc_PVal OnPValFuncDelegate;

	/** 获取兄弟对象属性值 */
	DECLARE_DELEGATE_RetVal_ThreeParams(FAny*, FOnFunc_FindEnt, int32, int64, const FString&);
	FOnFunc_FindEnt OnFindEntFuncDelegate;

private:
	// 当前公式字符串信息
	FString CachedFormulaStr;
	// 校验成功的公式字符串信息
	FString ParsedFormulaStr;

	//公式表达式
	TSharedPtr<FSubFormula> RootSubFormula;
};

