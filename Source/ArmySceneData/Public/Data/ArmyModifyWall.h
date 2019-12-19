#pragma once

#include "ArmyObject.h"
#include "FArmyConstructionItemInterface.h"

class ARMYSCENEDATA_API FArmyModifyWall :public FArmyObject, public FArmyConstructionItemInterface
{
public:
	/**
	* 双墙墙体捕捉信息
	*/
	struct FDoubleWallCaptureInfo
	{
		FDoubleWallCaptureInfo()
			: FirstLine(NULL)
			, SecondLine(NULL)
			, MinDistanceWall(NULL)
			, Pos(ForceInitToZero)
			, Direction(ForceInitToZero)
			, Thickness(24)
		{}

		TSharedPtr<FArmyLine> FirstLine;
		TSharedPtr<FArmyLine> SecondLine;
		TSharedPtr<FArmyLine> MinDistanceWall;
		FVector Pos;
		FVector Direction;
		float Thickness;
	};

	FArmyModifyWall();
	~FArmyModifyWall() {}

	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	// FObject2D Interface Begin
	virtual void SetState(EObjectState InState) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual bool Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual void GetVertexes(TArray<FVector>& OutVertexes) override;
	virtual void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false) override;
	virtual const FBox GetBounds();
	virtual void Delete() override;
	virtual void Generate(UWorld* InWorld) override;

	/**
	* @ 判断点是否在此对象内
	* @param Pos - const FVector & - 输入要检测的点
	* @return bool - true 表示在面内，否则不在
	*/
	virtual bool IsPointInObj(const FVector & Pos);

	// FObject2D Interface End
	void SetMaterial(UMaterial* InMaterial);
	void SetVertexes(TArray<FVector>& Vertexes);
	void AddBorderLine(TSharedPtr<FArmyLine> InLine);//为实现施工图拆改后墙体效果，而加的断墙线,在SetVertexes之后调用
	int32 GetWallType()const { return CurrentWallType; }
	void SetWallType(int32 InType) { CurrentWallType = InType; }

	void SetOperationLinePos(const FVector& InStartPos, const FVector& InEndPos);
	void UpdateDashLine();

	void SetMatType(FText InStr);
	FText GetMatType() { return MatType; }

    float GetWidth() const { return Width; }
    void SetWidth(float InWidth) { Width = InWidth; }

	void SetLength(float InValue);
	float GetLength() { return FVector::Distance(OperationLineStartPos, OperationLineEndPos); }

	void SetHeight(float InValue);
	float GetHeight() { return Height; }

	void SetFloorHeight(float InValue);
	float GetFloorHeight() { return FloorHeight; }

	FVector GetDir() const { return (OperationLineEndPos - OperationLineStartPos).GetSafeNormal(); };

	const TArray<TSharedPtr<FArmyLine>>& GetBorderLines() const { return BorderLines; };

	const FDoubleWallCaptureInfo& GetWallCaptureInfo() const { return CaptureWallInfo; };

	/**@欧石楠 请求材质商品编号*/
	void ReqMaterialSaleID();
	void ResMaterialSaleID(struct FArmyHttpResponse Response);

	/**@欧石楠 计算周长和面积*/
	void GetAreaAndPerimeter(float& OutArea, float& OutPerimeter);

	/**@欧石楠 获取材质商品ID*/
	int GetMaterialSaleID() { return MaterialSaleID; }

	//@ 获取操作线的起止点在墙上的中线点位置
	FVector & GetStartPos() { return OperationLineStartPosCenter; }
	FVector & GetEndPos() { return OperationLineEndPosCenter; };

	/**@欧石楠 获取选中的材质ID*/
	int GetSelectedMaterialID() { return SelectedMaterialID; }

	void ResetCalPolygonVertices();
    
public:
	/**@欧石楠 施工项相关数据*/
	TSharedPtr<FArmyConstructionItemInterface> ConstructionItemData;

	TSharedPtr<class FArmyPolygon> Polygon;

private:
	TArray<FVector> GetClipingBox();

	bool CaptureDoubleLine(const FVector& Pos, FDoubleWallCaptureInfo& OutInfo);

	float RulerOffset = 30.f;
	TSharedPtr<class FArmyRoom> FirstRelatedRoom;
	TSharedPtr<class FArmyRoom> SecondRelatedRoom;
	TSharedPtr<class FArmyRulerLine> FirstLineLeftRulerLine;
	void OnFirstLineLInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	TSharedPtr<class FArmyRulerLine> FirstLineRightRulerLine;
	void OnFirstLineRInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	TSharedPtr<class FArmyRulerLine> SecondLineLeftRulerLine;
	void OnSecondLineLInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	TSharedPtr<class FArmyRulerLine> SecondLineRightRulerLine;
	void OnSecondLineRInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	void UpdateBorderLine();

	void UpdateCombineHardwares(bool InbGenerate = false);

	bool IsSuccessful();

	struct FDoubleWallCaptureInfo CaptureWallInfo;

	/**@欧石楠 拆改墙材质*/
	FText MatType = FText::FromString(TEXT("轻体砖"));
	int MaterialSaleID;
	int SelectedMaterialID = 2;//轻体砖
protected:
	TArray<TSharedPtr<FArmyLine>> BorderLines;
	UMaterialInstanceDynamic* MI_Line;
	FLinearColor Color;
	int32 CurrentWallType = 0; // 0  拆改墙，1 承重墙
	TArray<TWeakPtr<FArmyLine>> AllLines;
	bool bUpdateDashLine = false;

	FVector OperationLineStartPos;
	FVector OperationLineEndPos;

    /** 宽度 */
    float Width;

    /** 高度 */
	float Height;

	float FloorHeight = 0;

	//@ 自身标尺线
	TSharedPtr<class FArmyRulerLine> HardWareRulerLine;
	//标尺
	float RulerOffsetDis;

	//此处两个点在拆除墙中线点方向的端点,不在关联的边线上
	FVector OperationLineStartPosCenter;
	FVector OperationLineEndPosCenter;
};
REGISTERCLASS(FArmyModifyWall)
