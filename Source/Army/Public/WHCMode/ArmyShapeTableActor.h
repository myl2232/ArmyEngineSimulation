#pragma once
#include "VertexType.h"
#include "SCTShapeData.h"
#include "Engine/StaticMeshActor.h"
#include "ArmyShapeTableActor.generated.h"

UCLASS()

class AXRShapeTableActor : public AActor
{
    GENERATED_BODY()
public:
	AXRShapeTableActor();
    void RefreshMesh(const TArray<FPUVVertex>& Vertices, const TArray<uint16>& Indices);
	void SetMaterial(UMaterialInterface* pMaterial);

	virtual bool IsSelectedInEditor() const override { return bOverrideSelectionFlag; }
	void SetIsSelected(bool bSelected);

	void SetIsSingleTable(bool bInSingleTable) { bSingleTable = bInSingleTable; }
	bool IsSingleTable() const { return bSingleTable; }

private:
    class UPNTUVPrimitive* mPrimitive;
	bool bOverrideSelectionFlag = false;
	bool bSingleTable = false;
};

UCLASS()
class AXRWhcGapActor : public AStaticMeshActor
{
	GENERATED_BODY()
public:
	AXRWhcGapActor();
	virtual bool IsSelectedInEditor() const override { return bOverrideSelectionFlag; }
	void SetIsSelected(bool bSelected);
	struct FShapeInRoom* mCabinet;
	int32 mGapIdx;
	bool bOverrideSelectionFlag = false;
};

UCLASS()
class AXRElecDeviceActor : public AStaticMeshActor
{
	GENERATED_BODY()
public:
	AXRElecDeviceActor();

	virtual bool IsSelectedInEditor() const override { return bOverrideSelectionFlag; }
	void SetIsSelected(bool bSelected);

	void SetType(EMetalsType InMetalType) { ElecDevType = InMetalType; }
	EMetalsType GetType() const { return ElecDevType; }

	struct FShapeAccessory *ShapeAccRef = nullptr;

private:
	bool bOverrideSelectionFlag = false;
	EMetalsType ElecDevType = EMetalsType::MT_None;
};