#pragma once
#include "CoreMinimal.h"
#include "Operand.h"

class FFormula;
class FSubFormula;

//��ʽ�ĺ���Ԫ��
class IFunction : public IFormulaElement
{
public:
	IFunction(FFormula *pFormula);
	virtual ~IFunction();

	/** ���ع�ʽԪ������Ϊ�������� */
	virtual EFormulaElementType GetType() const override { return EFormulaElementType::EFEType_Function; }

	/** ������������ */
	virtual bool ParseFunctionParameters(const TCHAR *lpszContent);

	/** ���㺯�����ֵ */
	virtual IOperand* CalculateFunctionRetValue() = 0;

protected:
	FFormula* MainFormula;
	//��ʽ�Ĳ�����Ƕ�׹�ʽ��
	FSubFormula** ppParams;
	//��ʽ��������
	int32 NumParams;
};

// ��ȡ��������ֵ����
class FFunction_FindOwnVal : public IFunction
{
public:
	FFunction_FindOwnVal(FFormula *pFormula)
		: IFunction(pFormula) {}
	virtual bool ParseFunctionParameters(const TCHAR *lpszContent) override;
	virtual IOperand* CalculateFunctionRetValue() override;
};

// ��ȡ�ֵܶ�������ֵ����
class FFunction_FindEntValue : public IFunction
{
public:
	FFunction_FindEntValue(FFormula *pFormula)
		: IFunction(pFormula) {}
	virtual bool ParseFunctionParameters(const TCHAR *lpszContent) override;
	virtual IOperand* CalculateFunctionRetValue() override;
};

// ��ȡ������������ֵ����
class FFunction_Pval : public IFunction
{
public:
	FFunction_Pval(FFormula *pFormula)
		: IFunction(pFormula) {}
	virtual bool ParseFunctionParameters(const TCHAR *lpszContent) override;
	virtual IOperand* CalculateFunctionRetValue() override;
};

// �������жϺ���
class FFunction_Cond : public IFunction
{
public:
	FFunction_Cond(FFormula *pFormula)
		: IFunction(pFormula) {}
	virtual bool ParseFunctionParameters(const TCHAR *lpszContent) override;
	virtual IOperand* CalculateFunctionRetValue() override;
};

// ѡ����
class FFunction_If : public IFunction
{
public:
	FFunction_If(FFormula *pFormula)
		: IFunction(pFormula) {}

	virtual bool ParseFunctionParameters(const TCHAR *lpszContent) override;
	virtual IOperand* CalculateFunctionRetValue() override;
};

// �ı����Ӻ���
class FFunction_Contxt : public IFunction
{
public:
	FFunction_Contxt(FFormula *pFormula)
		: IFunction(pFormula) {}
	virtual bool ParseFunctionParameters(const TCHAR *lpszContent) override;
	virtual IOperand* CalculateFunctionRetValue() override;
};

// �����ѯ
class FFunction_Gllink : public IFunction
{
public:
	FFunction_Gllink(FFormula *pFormula)
		: IFunction(pFormula) {}
	virtual bool ParseFunctionParameters(const TCHAR *lpszContent) override;
	virtual IOperand* CalculateFunctionRetValue() override;
};

// �޶��ߴ纯��
class FFunction_Limitdim : public IFunction
{
public:
	FFunction_Limitdim(FFormula *pFormula)
		: IFunction(pFormula) {}
	virtual bool ParseFunctionParameters(const TCHAR *lpszContent) override;
	virtual IOperand* CalculateFunctionRetValue() override;
};

/** ���ݺ������ƴ������� */
IFunction* CreateFunction(const TCHAR *lpszSignature, FFormula *pFormula);
/** ���ٺ��� */
void DestroyFunction(IFunction *pFunction);

/** ͨ�������ַ������ʽ���غ��� */
IFunction* ParseFunction(const TCHAR *lpszFormula, int32 &nIndex, FFormula *MainFormula);
/** ����@���� */
IFunction* ParseFunctionVar(const TCHAR *lpszFormula, int32 &nIndex, FFormula *MainFormula);
