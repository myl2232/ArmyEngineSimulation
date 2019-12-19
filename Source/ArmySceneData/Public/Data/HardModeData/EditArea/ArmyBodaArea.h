#pragma once 
#include "Data/HardModeData/GridSystemCore/XRBrickUnit.h"
#include "ArmyBaseArea.h"
enum BodaCornerType
{
	CornerBrick,
	AntiCornerBrick
};
class ARMYSCENEDATA_API FArmyBodaArea :public FArmyBaseArea
{
public:
	FArmyBodaArea(const TArray<FVector>& InOutArea, const float InWidth, const float InLenght, const FVector& InXdir, const FVector& InYDir, const FVector& InCenter);

	void SetBodaBrickInfo(const float InWidth, const float InLength, float brickDist);

	void DrawConstructionMode(class FPrimitiveDrawInterface* PDI, const FSceneView* View);

	void UpdateBodaAreaBrickDist(const float InBrickDist);

	virtual void SetExtrusionHeight(float height)override;

	void SetBodaMaterial(UMaterialInstanceDynamic* InMaterial);

	void SetPlaneOrignPos(FVector TempPlaneOrignPos) { PlaneOrignPos = TempPlaneOrignPos; }

	void CaculateBodaArea(const TArray<FVector>& InVerts, const float InWidth, const float InLenght, const float InBrickDist);

	bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* ViwePortClient, float& dist)override;

	float GetBodaThick()const { return BrickDist; }

	const TSharedPtr<FArmyBaseEditStyle> GetBodaMatStyle() { return MatStyle; }

	TSharedPtr<FArmyBaseArea> AttachParentArea;

	void Destroy()override;

	void SetBodaContentItem(TSharedPtr<FContentItemSpace::FContentItem > InBodaBrickContentItem) { BodaBrickContentItem = InBodaBrickContentItem; }
	TSharedPtr<FContentItemSpace::FContentItem > GetBodaContentItem();

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;

	AXRWallActor* GetBodaWallActor()const { return WallActor; }

	int32 GetBrickNums() { return TotoalBricks.Num(); }
	TArray<TSharedPtr<FArmyBrickUnit>> GetTotoalBricks() { return TotoalBricks;  }

	virtual bool TestAreaCanAddOrNot(const TArray<FVector>& InTestArea, FGuid ObjId, TSharedPtr<FArmyBaseArea> OutParent = nullptr) override;

	virtual void CalculateOutAndInnerHoles(TArray<FVector>& InOutArea, TArray<TArray<FVector>>& InnearHoles);
private:
	void UpdateBodaActor();
	TArray<FVector> OutVerts;
	TArray<FVector> InneraVerts;
	float brickWidth;
	float brickLength;
	float BrickDist;
	TArray<TSharedPtr<FArmyBrickUnit>> TotoalBricks;
	TSharedPtr<FContentItemSpace::FContentItem > BodaBrickContentItem;

//****************施工项相关**********
public:
	//TSharedPtr<struct ConstructionPatameters> GetConstructionParameter();
};