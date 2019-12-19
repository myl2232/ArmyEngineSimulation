#pragma once
#include "ArmyObject.h"
#include "FArmyConstructionItemInterface.h"

class ARMYSCENEDATA_API FArmyAddWall :public FArmyObject
{
public:

	FArmyAddWall();
	~FArmyAddWall() {};
	FArmyAddWall(FArmyAddWall* Copy);

	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	virtual void SetState(EObjectState InState) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual bool Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual void GetVertexes(TArray<FVector>& OutVertexes) override;
	virtual void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false) override;
	virtual const FBox GetBounds();
	virtual TSharedPtr<FArmyLine> HoverLine(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	// FObject2D Interface End

	virtual void ApplyTransform(const FTransform& Trans) override;

	virtual void Delete() override;

	void  GetAttachHoleWallInfo(TArray<struct FBSPWallInfo>& OutAttachWallInfo);

	void GetRelatedHardwares(TArray<TWeakPtr<FArmyObject>>& relatedHardwares);

	/**
	* @ 判断点是否在此对象内
	* @param Pos - const FVector & - 输入要检测的点
	* @return bool - true 表示在面内，否则不在
	*/
	virtual bool IsPointInObj(const FVector & Pos);

	void SetVertexes(TArray<FVector>& Vertexes, bool bIsDeserialization = false);
	virtual void Generate(UWorld* InWorld) override;

	void SetMatType(FString InType);
	FString GetMatType() { return MatType; }

	TSharedPtr<FArmyLine> GetParallelLine() const;

    float GetWidth() const { return Width; }
    void SetWidth(float InWidth) { Width = InWidth; }

	void SetHeight(float InValue);
	float GetHeight() { return Height; }

	/**
	 * @ 预计算变换是否为越界，此操作不会修改新建墙数据
	 * @param InSelected - TSharedPtr<FArmyObject> - 新建墙对象
	 * @param trans - FTransform - 变换
	 * @param dir - int32 - 
	 * @return bool - true表示变换越界，false表示变换可进行
	 */
	void PreCalTransformIllegal(TSharedPtr<FArmyObject> SelectedObj, FTransform trans, int32 Flag);
	bool PreCalTransformIllegalCommon(TSharedPtr<FArmyObject> SelectedObj, FTransform trans, int32 Flag);

	/**@欧石楠 请求材质商品编号*/
	void ReqMaterialSaleID();
	void ResMaterialSaleID(struct FArmyHttpResponse Response);

	/**@欧石楠 计算周长和面积*/
	void GetAreaAndPerimeter(float& OutArea, float& OutPerimeter);

	/**@欧石楠 获取材质商品ID*/
	int GetMaterialSaleID() { return MaterialSaleID; }

	/**@欧石楠 获取选中的材质ID*/
	int GetSelectedMaterialID() { return SelectedMaterialID; }

	/** @欧石楠 获取整墙半墙标记*/
	bool GetIsHalfWall() { return bIsHalfWall; }

	void SetIsHalfWall(bool bValue);

	/** @欧石楠 设置绘制新建墙时的原始点*/
	void SetCachedPoints(TArray<FVector> &InCachedPoints);

	/** @欧石楠 获取顺逆时针*/
	bool GetInternalExtrusion() { return bInternalExtrusion; }

	/** @欧石楠 设置顺逆时针*/
	void SetInternalExtrusion(bool bValue) { bInternalExtrusion = bValue; }

	/** @欧石楠 根据属性面板设置的顺逆时针和宽度重新生成新建墙*/
	bool RegenerateWall();

	// 计算cad墙线
	bool CalCadWallsByHardwares(TArray<TSharedPtr<class FArmyPolygon>>&OutArray);
private:
	void UpdatePolyVertices(bool bIsDeserialization = false);

	void UpdateSeletedRulerLine();

	void DeleteRelevanceHardwares();
	/**
	 * @ 计算新建墙是否与内墙线、自身相交
	 * @param Points - TArray<FVector> - 新建墙点
	 * @return Bool - true 表示相交，否则不相交
	 */
	bool CalInternalWallIntersection(TArray<FVector> Points);

	void OnLineInputBoxCommitted(float InLength);

	void OnLineInputBoxCommittedCancel();

	void OnLineInputBoxCommittedCommon(const FText& InText, const ETextCommit::Type InTextAction);

public:
	/**@欧石楠 施工项相关数据*/
	TSharedPtr<FArmyConstructionItemInterface> ConstructionItemData;

	TSharedPtr<class FArmyPolygon> Polygon;

	bool bOffsetGenerate = false;

	bool bModified = false;

protected:
	UMaterialInstanceDynamic* MI_Line;
	FLinearColor Color;	

	FString MatType = TEXT("轻体砖");
	int MaterialSaleID;
	int SelectedMaterialID = 2;//轻体砖

	TArray< TSharedPtr<class FArmyLine> > PolyLines;

	TSharedPtr<class FArmyRulerLine> SelectedRulerLine;
	TSharedPtr<class FArmyLine> SelectedLine;
	FVector CenterPos;

	TSharedPtr<class FArmyAxisRuler> AxisRuler;

	/** @欧石楠 存储绘制新建墙时的原始数据*/
	TArray<FVector> CachedPoints;

	/** @欧石楠 新建墙的顺逆时针方向  默认顺时针*/
	bool bInternalExtrusion = true;	

    /** 厚度 */
    float Width;

    /** 高度 */
	float Height;

	/** @欧石楠 标记是否是半墙*/
	bool bIsHalfWall = false;

private:	

	//判断选择的线是否为新建墙端点线
	bool bIsStartOrEndLine = false;

	//@ 选择单边时，存储标尺线的方向
	FVector SelectedRulerDir;

	bool bDeserialization = false;
};

REGISTERCLASS(FArmyAddWall)