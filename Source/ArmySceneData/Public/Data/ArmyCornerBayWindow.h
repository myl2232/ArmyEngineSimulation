#pragma once

#include "ArmyObject.h"
#include "ArmyHardware.h"

class ARMYSCENEDATA_API FArmyCornerBayWindow : public FArmyHardware
{
public:
    /** 
     * 杞椋樼獥绫诲瀷
     */
	enum ECornerBayWindowType
	{
        /** 鏃犳尅鏉?*/
		CornerAntiBoard,

        /** 宸︽尅鏉?*/
		CornerLeftBoard,

        /** 鍙虫尅鏉?*/
		CornerRightBoard,

        /** 鍙屼晶鎸℃澘 */
		CornerDoubleSideBoard
	};

	FArmyCornerBayWindow();
	~FArmyCornerBayWindow();
	FArmyCornerBayWindow(FArmyCornerBayWindow* Copy);

    //~ Begin FArmyHardware Interface
	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	virtual void SetState(EObjectState InState) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual bool Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual void Generate(UWorld* InWorld) override;
    virtual void Update() override;
    virtual void UpdateStartPoint() override;
    virtual void UpdateEndPoint() override;
	//~ End FArmyHardware Interface

    void SetProperty(const FVector& InPos, const FVector& leftWallDirection, const FVector& rightWallDirection, const float leftWallWidth, const float rightWallWidth);

	/** 鑾峰彇闂ㄧ殑鍖呭洿鐩?*/
	TArray<FVector> GetBoundingBox();

	/** @纪仁泽 获得立面投影 */
	TArray<struct FLinesInfo> GetFacadeBox();

	/** bsp鐢熸垚瑁佸壀 */
	TArray<FVector> GetClipingBox();

	void SetLeftWindowLength(float InLength);
	float GetLeftWindowLength()const { return LeftWindowLength; }

	void SetRightWindowLength(float InLength);
	float GetRightWindowLength()const { return RightWindowLength; }

	void SetLeftWindowOffset(float InOffset);
	float GetLeftWindowOffset()const { return LeftWalllOffset; }

	void SetRightWindowOffset(float InOffset);
	float GetRightWindowOffset()const { return RightWallOffset; }

	void SetWindowHoleDepth(float InDepth);
	float GetWindowHoleDepth()const { return BayWindowHoleDepth; }

	//void SetHeight(float InHeight);
	//float GetHeight()const { return Height; }

	virtual void SetHeightToFloor(float InHeight) override;
	//float GetHeightToFloor()const { return HeightToFloor; }

	void SetWindowBottomBoardHeight(float InBottomHeight) { WindowBottomHeight = InBottomHeight; }
	float GetWindowBottomBoardHeight()const { return WindowBottomHeight; }

	void SetWindowBottomBoardExtrudeLength(float InExtrude) { WindowBottomExtrudeLength = InExtrude; }
	float GetWindowBottomBoardExtrudeLength() { return WindowBottomExtrudeLength; }

	void GenerateWindowModel(UWorld* world);
	void DestroyWindowModel();

	void SetWindowCornerType(int i);

	int32 GetWindowCornerType()const { return (int32)CornerWindowType; }

	virtual const void GetAlonePoints(TArray< TSharedPtr<FArmyPoint> >& OutPoints) override;

public:
	FVector LeftDirection;
	FVector RightDirection;
	float RightWallWidth;
    float LeftWallWidth;
    FVector InnearCenterPos;
	FVector LeftUpPointPos;
	FVector RightUpPointPos;
	
	UArmyEditorViewportClient* LocalViewPortClient;

	FVector LeftWallDimPos;
	FVector RightWallDimPos;

protected:
	void UpateAntiBoardCornerBayWindow();
	void UpdateLeftBoardCornerWindow();
	void UpdateRightBoardCornerWindow();
	void UpdateDoubleBoardCornerWindow();
	ECornerBayWindowType CornerWindowType;
	void GenerateWindowGlassAndPillars();
	void CombineWindowWall();
	void UpdateOperationPointPos();
	void DeselectPoints();
	void DrawAntiBayWindow(FPrimitiveDrawInterface* PDI, const FSceneView* View);
	void DrawWithVertices(TArray<FVector> points, FPrimitiveDrawInterface* PDI, const FSceneView* View, FLinearColor color);
	void UpateInnearVertice(TArray<FVector>& Points);
	void UpdateMiddleOrOutVertice(TArray<FVector>& Points, float depth, float InLeftOffset, float InRightOffset);
	void UpdateBottomVertice(TArray<FVector>& Points);
	float LeftWalllOffset;
	float RightWallOffset;
	float LeftWindowLength;
	float RightWindowLength;
	float BayWindowHoleDepth;
	//float WindowHeight;
	float WindowBottomHeight;
	float WindowBottomExtrudeLength;
	//float HeightToFloor;

	class AXRShapeActor* WindowBoard;
	class AXRShapeActor* WindowGlass;
	class AXRShapeActor* WindowPillar;

	class AXRShapeActor* WindowWall;

	// 鏃犳澘瀛愰绐?
	TArray<FVector> OutWindowVerticeList;
	TArray<FVector> MiddleWindowVerticeList;
	TArray<FVector> InnearWindowVerticeList;
	TArray<FVector> BottomWindowVerticeList;

	// 鍙充晶鎸℃澘椋樼獥
	TArray<FVector> RightBoardMiddleList;
	TArray<FVector> RightBoardInnearList;
	TArray<FVector> RightBoardRightList;

	// 宸︿晶鎸℃澘椋樼獥
	TArray<FVector> LeftBoardLeftList;
	TArray<FVector> LeftBoardMiddleList;
	TArray<FVector> LeftBoardInnearList;

	// 鍙屼晶鏃犳尅鏉块绐?
	TArray<FVector> DoubleBoardLeftList;
	TArray<FVector> DoubleBoardMiddleList;
	TArray<FVector> DoubleBoardInnearList;
	TArray<FVector> DoubleBoardRightList;
private:
	//鏄惁浣跨敤榛樿鏉愯川
	bool bDefaultMaterial = true;
	//绐楀彴鐭虫潗璐?
	UMaterialInterface * WindowStoneMat;
};

REGISTERCLASS(FArmyCornerBayWindow)
