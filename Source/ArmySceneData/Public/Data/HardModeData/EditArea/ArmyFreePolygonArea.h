#pragma once
#include "ArmyBaseArea.h"
#include "ArmyRegularPolygonArea.h"
class ARMYSCENEDATA_API FArmyFreePolygonArea :public FArmyRegularPolygonArea
{
public:
	FArmyFreePolygonArea();
	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View)override;
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	void SetVertices(const TArray<FVector> InVertices);
	virtual EDrawAreaType GetDrawAreaType() override { return EDrawAreaType::CuastomPolygon; };
	/* @梁晓菲 通过四轴坐标改变距墙尺寸的时候调用*/
	void ApplyTransform(const FTransform& Trans) override;

	virtual void RefreshPolyVertices() override;
    
	void Move(const FVector& Offset) override;

};

REGISTERCLASS(FArmyFreePolygonArea)