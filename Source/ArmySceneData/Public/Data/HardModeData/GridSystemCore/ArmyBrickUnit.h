#pragma once
#include "SceneManagement.h"
#include "DynamicMeshBuilder.h"
#include "ArmyTextureAtlas.h"
#include "../DrawStyle/ArmyBaseEditStyle.h"
class FArmyBrickUnit
{
public:
	FArmyBrickUnit();
	FArmyBrickUnit(FVector InPos, float InWidth, float InHeight);
	~FArmyBrickUnit() {}

	FArmyBrickUnit(FArmyBrickUnit* Copy);

	void SetPoisition(FVector centerPosition);
	FVector GetPosition() { return BrickCenterPosition; }

	void SetBrickWidth(float InWidth);

	float GetBrickWidth()const { return BrickWidth; }

	void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	void SetBrickHeight(float InHeight);
	float GetBrickHeight()const { return BrickHeight; }

	void ApplyTransform(FTransform dist);
	// 逆时针旋转
	void RotateAroundCenterPos(float angle);

	void ApplyClipper(const TArray<FVector>& InClipperVerices, const TArray<TArray<FVector>>& Holes, const int32 useTextureIndex = 1, const int32 totalNum = 1, const float IntervalDistance=0);

	void SetTextureUrl(FString InTextureUrl) { MainTextureUrl = InTextureUrl; }

	void SetTextureID(uint32 Inid) { MainTextureId = Inid; }

	uint32 GetTextureID()const { return MainTextureId; }

	const TArray<FDynamicMeshVertex>& GetVertices()const { return  ClipperResults; }

	bool IsClipper()const { return ClipperResults.Num() > 0 ? true : false; }

	void SetRotateTextureAngle(float InAngle) { RoateTextureUV = InAngle; }

	void SetUVRects(const Rectf& InRectf);

	bool IsSelected(const FVector& Pos, class  UArmyEditorViewportClient* InViewportClient);

	void SetPlaneOffset(float Offset);

	void SetPlaneInfo(const FVector& InCenter, const FVector& InXDir, const FVector& InYdir);

	TArray<FVector> GetBrickVerts();
	FVector PlaneCenterPos = FVector(0, 0, 0);
	FVector PlaneXDir = FVector(1, 0, 0);
	FVector PlaneYDir = FVector(0, 1, 0);
	float polyOffset = 0.2f;

	// 常远 获得结果点
	TArray<TArray<FVector>> GetCutResults() { return CutResults; };
private:

	void TriangleBrickWithHole(const TArray<FVector>& BrickVertexs, const TArray<TArray<FVector>>& Holes, const int32 CurrentUseIndex = 1, const int32 TotalNum = 1);
	uint32 MainTextureId = 1;
	FString MainTextureUrl;
	FVector BrickWidthDirection;
	FVector BrickHeightDirection;
	FVector BrickCenterPosition;
	float BrickWidth;
	float BrickHeight;
	float RotateAngle;
	TArray< FDynamicMeshVertex> ClipperResults;
	TArray<TArray<FVector>> CutResults;
	float RoateTextureUV = 0;
	Rectf M_Rectf;

	TArray<FVector2D> points;
};