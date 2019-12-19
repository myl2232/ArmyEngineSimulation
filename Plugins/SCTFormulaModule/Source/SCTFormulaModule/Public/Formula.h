#pragma once
#include "SubFormula.h"

//��ʽ����ӿ���
class SCTFORMULAMODULE_API FFormula
{
public:
	/** ȫ�ֳ�ʼ����ʽ�����Դ */
	static void InitGlobalResource();
	typedef TMap<FString, TSharedPtr<IOperator>, FDefaultSetAllocator, TDefaultMapKeyFuncs<FString, TSharedPtr<IOperator>, false>> OperatorMap;
	static OperatorMap s_Operators;
	typedef TSet<TCHAR, DefaultKeyFuncs<TCHAR, false>, FDefaultSetAllocator> OperatorShortcuts;
	static OperatorShortcuts s_OperatorShortcuts;
	typedef TSet<FString, DefaultKeyFuncs<FString, false>, FDefaultSetAllocator> FunctionSignatures;
	static FunctionSignatures s_FunctionSignatures;

	FFormula();
	~FFormula();

	/** ������ʽ�ַ��� */
	bool ParseFormula(const FString &FormulaStr);

	/** ���㹫ʽֵ */
	FAny* CalculateFormula(const FString &FormulaStr, bool bForceRecalculate = false);

	/** ����/��ȡ��ʽ�ַ��� */
	void SetFormulaStr(const FString &FormulaStr) { CachedFormulaStr = FormulaStr; }
	const FString& GetFormulaStr() const { return CachedFormulaStr; }

	/** �����ⲿ�����ı�״̬ */
	void SetVariDirty();

	/** ��ȡ�Լ���������ֵ */
	DECLARE_DELEGATE_RetVal_OneParam(FAny*, FOnFunc_OwnVal, const FString&);
	FOnFunc_OwnVal OnFindOwnValDelegate;

	/** ��ȡ������������ֵ���� */
	DECLARE_DELEGATE_RetVal_OneParam(FAny*, FOnFunc_PVal, const FString&);
	FOnFunc_PVal OnPValFuncDelegate;

	/** ��ȡ�ֵܶ�������ֵ */
	DECLARE_DELEGATE_RetVal_ThreeParams(FAny*, FOnFunc_FindEnt, int32, int64, const FString&);
	FOnFunc_FindEnt OnFindEntFuncDelegate;

private:
	// ��ǰ��ʽ�ַ�����Ϣ
	FString CachedFormulaStr;
	// У��ɹ��Ĺ�ʽ�ַ�����Ϣ
	FString ParsedFormulaStr;

	//��ʽ���ʽ
	TSharedPtr<FSubFormula> RootSubFormula;
};

