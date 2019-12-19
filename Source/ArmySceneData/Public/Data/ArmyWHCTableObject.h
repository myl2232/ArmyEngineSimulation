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
		FString ComboStyle;//套餐风格
		FString BoardMaterial;//台面材质
		FString FrontEdgeName;//前沿名称
		FString BackEdgeName;//后档名称

		TArray<FString> StoveTypeArray;//灶具型号
		TArray<FString> SinkTypeArray;//水槽型号

		float BoardHeight = 0;//台面厚度
		float FrontEdgeHeight = 0;//前沿高度
		float BackEdgeHeight = 0;//后档高度

		void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
		void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);
	};
    FArmyWHCTableObject();
    void SetTableOutline(const TArray<FArmyWHCTableObject::FWHCTableBatchData>& InBatchDataArray, float SplineWidth, const TArray<TWeakPtr<class FArmyFurniture>>& InComponentArray);

    virtual void Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View) override;

	/** 移动位置 */
	virtual void ApplyTransform(const FTransform& Trans)  override;

	virtual void GetTransForm(FRotator& InRot, FVector& InTran, FVector& InScale) const  override;

	/** 获取包围盒*/
	virtual const FBox GetBounds() override;

	/** 获取原始包围盒（未进行变换的包围盒）*/
	virtual const FBox GetPreBounds()  override;

	/**	获取基点*/
	virtual const FVector GetBasePos()  override;

	/**	获取所有线*/
	virtual void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false) override;

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;

	const TArray<FLineTransformInfo>& GetFrontOutLine() const { return TableFrontOutline; };
	const TArray<FLineTransformInfo>& GetBackOutLine() const { return TableBackOutline; };
	const TArray<FLineTransformInfo>& GetTableOutLine() const { return TableOutline; };

	void SetDrawComponent(bool bDraw = false) { DrawComponent = bDraw; };
	void SetDrawFrontOutLine(bool bDraw = false) { DrawFrontOutLine = bDraw; };
	void SetDrawBackOutLine(bool bDraw = false) { DrawBackOutLine = bDraw; };
	// 获得绘制CAD的线段 0：Outline 1：FrontOutline 2：BackOutline
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
	FTableInfo TableInfo;//台面的所有属性
private:
	bool DrawFrontOutLine;
	bool DrawBackOutLine;
	bool DrawComponent;
	FBox BoundingBox;//包围盒
	FTransform LocalTransform; 

	/* 灶具、水槽*/
	TArray<FComponentInfo> OnTabelComponentArray;
    TArray<FLineTransformInfo> TableOutline;
    TArray<FLineTransformInfo> TableFrontOutline;
    TArray<FLineTransformInfo> TableBackOutline;
};
REGISTERCLASS(FArmyWHCTableObject)