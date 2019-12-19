#pragma once

#include "CoreMinimal.h"
#include "JsonWriter.h"
#include "CondensedJsonPrintPolicy.h"
#include "SCTShapeData.h"

class FAny;
class FFormula;
class FSCTShape;
class FJsonObject;



//��¼���Ի���
class SCTSHAPEMODULE_API FShapeAttribute
{
	friend class FBoardPointAttr;
public:
	FShapeAttribute();
	virtual ~FShapeAttribute();

public:
	/** ����Json���� */
	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject);
	/** ����ΪJson���� */
	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);

	/** ������������ */
	virtual void CopyTo(const TSharedPtr<FShapeAttribute> OutShapeAttri);

	/** ������������ֵ */
	virtual bool SetAttributeValue(const FString& InValue);
	FString GetAttributeStr() const;
	bool CheckIfAttributeIsFormula() const;
	/** ��ȡ�������ֵ */
	virtual TSharedPtr<FAny> GetAttributeValue();

	/** ��������ֵ��Ҫ���¼��� */
	virtual void SetDirtyFlag();

public:
	/** ������������ */
	EShapeAttributeType GetAttributeType() const;

	void SetAttributeId(int64 InID);
	int64 GetAttributeId() const;

	void SetRefName(FString InName);
	FString GetRefName() const;

public:
	DECLARE_DELEGATE_OneParam(FOnAttrValueChanged, const FString&)
	FOnAttrValueChanged OnAttrValueChanged;

protected:
	// ID
	int64 AttributeId;
	// ��������
	FString RefName;

	//��������
	EShapeAttributeType AttributeType;

	// �������
	FString InStrValue;
	// ������
	TSharedPtr<FAny> OutAnyValue;
};

//����������
class SCTSHAPEMODULE_API FBoolAssignAttri : public FShapeAttribute
{
public:
	FBoolAssignAttri(FSCTShape* InShape);
	~FBoolAssignAttri();

public:
	/** ����Json���� */
	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	/** ����ΪJson���� */
	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	/** ������������ */
	virtual void CopyTo(const TSharedPtr<FShapeAttribute> OutShapeAttri) override;

	/** ������������ֵ */
	virtual bool SetAttributeValue(const FString& InValue) override;
	/** ��ȡ�������ֵ */
	virtual TSharedPtr<FAny> GetAttributeValue() override;

	/** ��������ֵ��Ҫ���¼��� */
	virtual void SetDirtyFlag() override;

public:
	/** ��ȡ����������ֵ */
	bool GetBoolValue();

	/** ��ȡ��ʽ���� */
	TSharedPtr<FFormula> GetFormulaObj() const;

private:
	//��ʽ���ʽ
	TSharedPtr<FFormula> FormulaValue;
};

//�ɸ�ֵ����ֵ
class SCTSHAPEMODULE_API FNumberAssignAttri : public FShapeAttribute
{
public:
	FNumberAssignAttri(FSCTShape* InShape);
	~FNumberAssignAttri();

public:
	/** ����Json���� */
	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	/** ����ΪJson���� */
	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	/** ������������ */
	virtual void CopyTo(const TSharedPtr<FShapeAttribute> OutShapeAttri) override;

	/** ������������ֵ */
	virtual bool SetAttributeValue(const FString& InValue) override;
	/** ��ȡ�������ֵ */
	virtual TSharedPtr<FAny> GetAttributeValue() override;

	/** ��������ֵ��Ҫ���¼��� */
	virtual void SetDirtyFlag() override;

public:
	/** ��ȡ��ֵ������ֵ */
	float GetNumberValue();

	/** ��ȡ��ʽ���� */
	TSharedPtr<FFormula> GetFormulaObj() const;

private:
	//��ʽ���ʽ
	TSharedPtr<FFormula> FormulaValue;
};

//��ΧԼ����ֵ
class SCTSHAPEMODULE_API FNumberRangeAttri : public FShapeAttribute
{
public:
	FNumberRangeAttri(FSCTShape* InShape);
	~FNumberRangeAttri();

public:
	/** ����Json���� */
	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	/** ����ΪJson���� */
	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	/** ������������ */
	virtual void CopyTo(const TSharedPtr<FShapeAttribute> OutShapeAttri) override;

	/** ������������ֵ */
	virtual bool SetAttributeValue(const FString& InValue) override;
	/** ��ȡ�������ֵ */
	virtual TSharedPtr<FAny> GetAttributeValue() override;

	/** ��������ֵ��Ҫ���¼��� */
	virtual void SetDirtyFlag() override;

public:
	/** ����/��ȡ������Сֵ */
	void SetMinValue(float InValue);
	float GetMinValue() const;

	/** ����/��ȡ�������ֵ */
	void SetMaxValue(float InValue);
	float GetMaxValue() const;

	/** ��ȡ��ֵ������ֵ */
	float GetNumberValue();

	/** ��ȡ��ʽ���� */
	TSharedPtr<FFormula> GetFormulaObj() const;

private:
	/** ������Сֵ */
	float MinValue;
	/** �������ֵ */
	float MaxValue;
	/** ��ʽ���ʽ */
	TSharedPtr<FFormula> FormulaValue;
};

//��ѡֵ����ֵ
class SCTSHAPEMODULE_API FNumberSelectAttri : public FShapeAttribute
{
public:
	FNumberSelectAttri(FSCTShape* InShape);
	~FNumberSelectAttri();

public:
	/** ����Json���� */
	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	/** ����ΪJson���� */
	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	/** ������������ */
	virtual void CopyTo(const TSharedPtr<FShapeAttribute> OutShapeAttri) override;

	/** ������������ֵ */
	virtual bool SetAttributeValue(const FString& InValue) override;
	/** ��ȡ�������ֵ */
	virtual TSharedPtr<FAny> GetAttributeValue() override;

	/** ��������ֵ��Ҫ���¼��� */
	virtual void SetDirtyFlag() override;

public:
	/** ��ӿ�ѡ�ı�ֵ */
	void AddSelectValue(const FString& Key, float Value);

	/** ��ȡ��ѡ�ı�ֵ���� */
	const TMap<FString, float>& GetSelectedValues() const;

	/** �Ƴ���ѡ�ı�ֵ */
	void RemoveSelectedValue(const FString& KeyValue);
	void RemoveAllValues();

	/** ��ȡ��ֵ������ֵ */
	float GetNumberValue();

private:
	/** ��ѡ��ֵ���� */
	TMap<FString, float> SelectedValues;
};

//�ɸ�ֵ���ı�
class SCTSHAPEMODULE_API FTextAssignAttri : public FShapeAttribute
{
public:
	FTextAssignAttri(FSCTShape* InShape);
	~FTextAssignAttri();

public:
	/** ����Json���� */
	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	/** ����ΪJson���� */
	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	/** ������������ */
	virtual void CopyTo(const TSharedPtr<FShapeAttribute> OutShapeAttri) override;

	/** ������������ֵ */
	virtual bool SetAttributeValue(const FString& InValue) override;
	/** ��ȡ�������ֵ */
	virtual TSharedPtr<FAny> GetAttributeValue() override;

	/** ��������ֵ��Ҫ���¼��� */
	virtual void SetDirtyFlag() override;

public:
	/** ��ȡ�ı�������ֵ */
	FString GetStringValue();

	/** ��ȡ��ʽ���� */
	TSharedPtr<FFormula> GetFormulaObj() const;

private:
	/** ��ʽ���ʽ */
	TSharedPtr<FFormula> FormulaValue;
};

//��ѡֵ���ı�
class SCTSHAPEMODULE_API FTextSelectAttri : public FShapeAttribute
{
public:
	FTextSelectAttri(FSCTShape* InShape);
	~FTextSelectAttri();

public:
	/** ����Json���� */
	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	/** ����ΪJson���� */
	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	/** ������������ */
	virtual void CopyTo(const TSharedPtr<FShapeAttribute> OutShapeAttri) override;

	/** ������������ֵ */
	virtual bool SetAttributeValue(const FString& InValue) override;
	/** ��ȡ�������ֵ */
	virtual TSharedPtr<FAny> GetAttributeValue() override;

	/** ��������ֵ��Ҫ���¼��� */
	virtual void SetDirtyFlag() override;

	bool SetAttributeKey(const FString & InKey);

public:
	/** ��ӿ�ѡ�ı�ֵ */
	void AddSelectValue(const FString& Key, const FString& Value);

	/** ��ȡ��ѡ�ı�ֵ���� */
	const TMap<FString, FString>& GetSelectedValues() const;

	/** �Ƴ���ѡ�ı�ֵ */
	void RemoveSelectedValue(const FString& KeyValue);
	void RemoveAllValues();

	/** ��ȡ�ı�������ֵ */
	FString GetStringValue();

private:
	/** ��ѡ�ı�ֵ���� */
	TMap<FString, FString> SelectedValues;
};

//�����λ����
class SCTSHAPEMODULE_API FBoardPointAttr
{
public:
	FBoardPointAttr(FSCTShape* InShape);
	~FBoardPointAttr();

public:
	/** ���õ�λ���� */
	void SetPositionX(const FString& InPosX);
	void SetPositionY(const FString& InPosY);
	/** ��ȡ��λ���� */
	int32 GetPositionX();
	int32 GetPositionY();

	/** ����Json���� */
	void ParsePointAttri(const TSharedPtr<FJsonObject>& InJsonObject);
	/** ����ΪJson���� */
	void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
	/** ���������µĶ��� */
	void CopyTo(const TSharedPtr<FBoardPointAttr> OutPointAttri);

	/** �������Ըı��ʶ */
	void SetAttributesDirty();

private:
	TSharedPtr<FNumberAssignAttri> PosXAttri;
	TSharedPtr<FNumberAssignAttri> PosYAttri;
};

/** �������ʹ���һ���µ����� */
TSharedPtr<FShapeAttribute> SCTSHAPEMODULE_API MakeAttributeByType(EShapeAttributeType InAttriType, FSCTShape* InShape);

/** ����JSon�ļ��������� */
TSharedPtr<FShapeAttribute> SCTSHAPEMODULE_API ParseAttributeFromJson(const TSharedPtr<FJsonObject>& InJsonObject, FSCTShape* InShape);

/** �������ƴ����µ����� */
TSharedPtr<FShapeAttribute> SCTSHAPEMODULE_API CopyAttributeToNew(const TSharedPtr<FShapeAttribute>& InShapeAttri, FSCTShape* InShape);
