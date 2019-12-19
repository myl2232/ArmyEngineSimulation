#pragma  once

#include "ArmyBaseEditStyle.h"
#include "SceneManagement.h"
#include "DynamicMeshBuilder.h"

class ARMYSCENEDATA_API FArmySeamlessStyle :public FArmyBaseEditStyle
{
public: 
	FArmySeamlessStyle();

	//@郭子阳
	//复制style的参数
	virtual void CopyFrom(TSharedPtr <FArmyBaseEditStyle >  OldStyle);

	virtual void Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View)override;

	virtual void CalculateClipperBricks(const TArray<FVector>& OutAreaVertices, const TArray<TArray<FVector>>& InneraHoles) override;

	virtual void SetDrawArea(const TArray<FVector>& outAreas, const TArray<TArray<FVector>>& Holes)override;

	virtual void DrawWireFrame(class FPrimitiveDrawInterface* PDI, const FSceneView* View);

	/* @梁晓菲 无缝铺给边画线*/
	virtual	void DrawHoles(class FPrimitiveDrawInterface* PDI, const FSceneView* View)override;

	virtual void SetStylePlaneOffset(float InOffset)override;

	virtual void GetVertexInfo(TArray<FDynamicMeshVertex>& AllVertexs)override;


	virtual	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);

	virtual	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);

	int32 SeamlesStyleType = 0;// 0 表示壁纸，1表示墙漆
	// 墙漆施工项目
	FString ColorPaintValueKey;
private:
	void TriangleArea( const TArray<FVector>& OutAreas, const TArray<TArray<FVector>>& Holes);

	TArray<FDynamicMeshVertex> TotalVertexs;
	TArray<FVector2D> points;
	// zhx add for reset offset 
	TArray<FVector> SpaceOutAreas;
	TArray<TArray<FVector>> SpaceHoles;

};