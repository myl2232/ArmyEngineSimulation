#pragma once
#include "CoreMinimal.h"
#include "Operator.h"

class FFormula;
class IFunction;
// ��ʽ
class FSubFormula
{
public:
	FSubFormula(FFormula *Formula);
	~FSubFormula();

	/** ������ʽ */
	bool ParseFormula(const TCHAR *lpszFormula, int32 nLen);

	/** ���½���Ԫ�ؽ�� */
	void UpdateFormulaElements();

	/** ���㹫ʽֵ */
	FAny* CalculateFormula();

	/** ��ȡ��ʽ���ز����������� */
	IOperand::EOperandType GetFormulaReturnType();

	/** �����Ƿ���ɽ����������״̬ */
	void SetUpdatedFlag(bool bFlag);
	/** �����ⲿ�����Ƿ�ı�״̬ */
	void SetVariDirtyFlag(bool bFlag);

private:
	/** �������ֲ����� */
	IOperand* GetNumberOperand(const TCHAR *lpszFormula, int32 &nIndex, bool bPositive);

	/** �����ַ������� */
	IOperand* GetStringOperand(const TCHAR *lpszFormula, int32 &nIndex);
	
	/** ���������� */
	TSharedPtr<IOperator> GetOperator(const TCHAR *lpszFormula, int32 &nIndex, int32 nCount);

	/** ����������Ԫ�ؽ�� */
	void ClearParsedElements();
	/** ��������õ�Ԫ�ؽ�� */
	void ClearCalculatedElements();

private:
	// ��ʶ�Ƿ��Ѿ��ɹ�����У��
	bool bParsed;
	// ��ʶ�Ƿ��Ѿ������˽������
	bool bUpdated;
	// ��ʶ�Ƿ����ⲿ�����ı�
	bool bVariDirty;

	// ���ڹ�ʽ����Ĺ�ʽԪ�ؽ��
	TArray<TSharedPtr<IFormulaElement>> CalculatedElements;
	// ��ʽ������
	FAny ResultAny;

	// ��ʽУ��󻺴�Ĺ�ʽԪ�ؽ��
	TArray<TSharedPtr<IFormulaElement>> ParsedElements;
	// ��ʽУ���ȷ�ϵĽ������������
	IOperand::EOperandType EReturnType;

	FFormula *MainFormula;
	TSharedPtr<FSubFormula> LeftFormula;
	TSharedPtr<FSubFormula> RightFormula;
};
