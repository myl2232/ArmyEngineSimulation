#pragma once
#include "Data/XRObject.h"
#include "Primitive2D/XRLine.h"

class ARMYSCENEDATA_API FArmyWHCTableObject final : public FArmyObject
{
public:
	struct FWHCTableBatchData
	{
		FWHCTableBatchData() {};

		TArray<FVector2D> VerFronts;
		TArray<FVector2D> VerBacks;
		float SplineWidth = 1;
		bool bClosed = false;
	};
	struct FLineTransformInfo
	{
		FLineTransformInfo() {};
		FLineTransformInfo(const TSharedPtr<FArmyLine> InBaseLine,const TSharedPtr<FArmyLine> InDrawLine):BaseLine(InBaseLine),DrawLine(InDrawLine)
		{};
		TSharedPtr<FArmyLine> BaseLine;
		TSharedPtr<FArmyLine> DrawLine;
	};
	struct FComponentInfo
	{
		FComponentInfo() {};
		FLineTransformInfo CenterLine;
		TArray<struct FSimpleElementVertex> BaseArray;
		TArray<struct FSimpleElementVertex> DrawArray;
	};
	struct FTableInfo
	{
		FString ComboStyle;//�ײͷ��
		FString BoardMaterial;//̨�����
		FString FrontEdgeName;//ǰ������
		FString BackEdgeName;//������

		TArray<FString> StoveTypeArray;//����ͺ�
		TArray<FString> SinkTypeArray;//ˮ���ͺ�

		float BoardHeight = 0;//̨����
		float FrontEdgeHeight = 0;//ǰ�ظ߶�
		float BackEdgeHeight = 0;//�󵵸߶�

		void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
		void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);
	};
    FArmyWHCTableObject();
    void SetTableOutline(const TArray<FArmyWHCTableObject::FWHCTableBatchData>& InBatchDataArray, float SplineWidth, const TArray<TWeakPtr<class FArmyFurniture>>& InComponentArray);

    virtual void Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View) override;

	/** �ƶ�λ�� */
	virtual void ApplyTransform(const FTransform& Trans)  override;

	virtual void GetTransForm(FRotator& InRot, FVector& InTran, FVector& InScale) const  override;

	/** ��ȡ��Χ��*/
	virtual const FBox GetBounds() override;

	/** ��ȡԭʼ��Χ�У�δ���б任�İ�Χ�У�*/
	virtual const FBox GetPreBounds()  override;

	/**	��ȡ����*/
	virtual const FVector GetBasePos()  override;

	/**	��ȡ������*/
	virtual void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false) override;

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;

	const TArray<FLineTransformInfo>& GetFrontOutLine() const { return TableFrontOutline; };
	const TArray<FLineTransformInfo>& GetBackOutLine() const { return TableBackOutline; };
	const TArray<FLineTransformInfo>& GetTableOutLine() const { return TableOutline; };

	void SetDrawComponent(bool bDraw = false) { DrawComponent = bDraw; };
	void SetDrawFrontOutLine(bool bDraw = false) { DrawFrontOutLine = bDraw; };
	void SetDrawBackOutLine(bool bDraw = false) { DrawBackOutLine = bDraw; };
	// ��û���CAD���߶� 0��Outline 1��FrontOutline 2��BackOutline
	void GetCadLines(TArray< TSharedPtr<FArmyLine> >& OutLines,int32 LineType=0);
	bool GetIsDrawFrontOutLine() { return DrawFrontOutLine; };
	bool GetIsDrawBackOutLine() { return DrawBackOutLine; };
	bool GetIsDrawComponent() { return DrawComponent; };
	TArray<FComponentInfo> GetDrawComponentArray() {
		return OnTabelComponentArray;
	};

private:
	void Update();
public:
	FTableInfo TableInfo;//̨�����������
private:
	bool DrawFrontOutLine;
	bool DrawBackOutLine;
	bool DrawComponent;
	FBox BoundingBox;//��Χ��
	FTransform LocalTransform; 

	/* ��ߡ�ˮ��*/
	TArray<FComponentInfo> OnTabelComponentArray;
    TArray<FLineTransformInfo> TableOutline;
    TArray<FLineTransformInfo> TableFrontOutline;
    TArray<FLineTransformInfo> TableBackOutline;
};
REGISTERCLASS(FArmyWHCTableObject)