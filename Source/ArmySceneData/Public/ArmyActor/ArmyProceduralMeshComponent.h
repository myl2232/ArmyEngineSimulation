// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Interfaces/Interface_CollisionDataProvider.h"
#include "Components/MeshComponent.h"
#include "PhysicsEngine/ConvexElem.h"
#include "ArmyProceduralMeshComponent.generated.h"

class FPrimitiveSceneProxy;

/**
*	Struct used to specify a tangent vector for a vertex
*	The Y tangent is computed from the cross product of the vertex normal (Tangent Z) and the TangentX member.
*/
USTRUCT(BlueprintType)
struct FArmyProcMeshTangent
{
	GENERATED_USTRUCT_BODY()

	/** Direction of X tangent for this vertex */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tangent)
	FVector TangentX;

	/** Bool that indicates whether we should flip the Y tangent when we compute it using cross product */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tangent)
	bool bFlipTangentY;

	FArmyProcMeshTangent()
		: TangentX(1.f, 0.f, 0.f)
		, bFlipTangentY(false)
	{}

	FArmyProcMeshTangent(float X, float Y, float Z)
		: TangentX(X, Y, Z)
		, bFlipTangentY(false)
	{}

	FArmyProcMeshTangent(FVector InTangentX, bool bInFlipTangentY)
		: TangentX(InTangentX)
		, bFlipTangentY(bInFlipTangentY)
	{}
};

/** One vertex for the procedural mesh, used for storing data internally */
struct FArmyProcMeshVertex
{
	/** Vertex position */
	FVector Position;

	/** Vertex normal */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vertex)
	//FVector Normal;

	/** Vertex tangent */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vertex)
	//FArmyProcMeshTangent Tangent;

	FVector2D LightMapUV;
	FVector TangentX;
	FVector TangentY;
	FVector TangentZ;

	/** Vertex color */
	FColor Color;

	/** Vertex texture co-ordinate */
	FVector2D UV;


	FArmyProcMeshVertex()
		: Position(0.f, 0.f, 0.f)
		, TangentX(0.f, 0.f, 0.f)
		, TangentY(0.f, 0.f, 0.f)
		, TangentZ(0.f, 0.f, 0.f)
		, Color(255, 255, 255)
		, UV(0.f, 0.f)
	{}
};

struct FArmyProcMeshTriangle
{
public:
	uint16 Vertex0;
	uint16 Vertex1;
	uint16 Vertex2;

	FArmyProcMeshTriangle()
		: Vertex0(0)
		, Vertex1(0)
		, Vertex2(0)
	{}

	FArmyProcMeshTriangle(uint16 InVertex0, uint16 InVertex1, uint16 InVertex2)
		: Vertex0(InVertex0)
		, Vertex1(InVertex1)
		, Vertex2(InVertex2)
	{}
};

/** One section of the procedural mesh. Each material has its own section. */
struct FArmyProcMeshSection
{
	/** Vertex buffer for this section */
	TArray<FArmyProcMeshVertex> ProcVertexBuffer;

	/** Index buffer for this section */
	TArray<int32> ProcIndexBuffer;
	/** Local bounding box of section */
	FBox SectionLocalBox;

	/** Should we build collision data for triangles in this section */
	bool bEnableCollision;

	/** Should we display this section */
	bool bSectionVisible;

	bool bSectionStaitcLighting;

	FArmyProcMeshSection()
		: SectionLocalBox(ForceInit)
		, bEnableCollision(false)
		, bSectionVisible(true)
		, bSectionStaitcLighting(true)
	{}

	/** Reset this section, clear all mesh info. */
	void Reset()
	{
		ProcVertexBuffer.Empty();
		ProcIndexBuffer.Empty();
		SectionLocalBox.Init();
		bEnableCollision = false;
		bSectionVisible = true;
		bSectionStaitcLighting = true;
	}
};

/**
*	Struct used to send update to mesh data
*	Arrays may be empty, in which case no update is performed.
*/
class FArmyProcMeshSectionUpdateData
{
public:
	/** Section to update */
	int32 TargetSection;
	/** New vertex information */
	TArray<FArmyProcMeshVertex> NewVertexBuffer;
};

/**
*	Component that allows you to specify custom triangle mesh geometry
*	Beware! This feature is experimental and may be substantially changed in future releases.
*/
UCLASS(hidecategories = (Object, LOD), meta = (BlueprintSpawnableComponent), ClassGroup = Rendering)
class ARMYSCENEDATA_API UXRProceduralMeshComponent : public UMeshComponent, public IInterface_CollisionDataProvider
{
	GENERATED_UCLASS_BODY()

	/** ʵ���ռ�������Ϣ�ӿ� */
	virtual void GetStaticLightingInfo(FStaticLightingPrimitiveInfo& OutPrimitiveInfo, const TArray<ULightComponent*>& InRelevantLights, const FLightingBuildOptions& Options) override;

	/** ֧�־�̬�� */
	virtual bool SupportsStaticLighting() const override { return true; }
	
	/** �決����������Ч */
	virtual bool IsPrecomputedLightingValid() const override { return true; }
	
	/** ��ȡ����ͼ�ֱ��� */
	virtual bool GetLightMapResolution(int32& Width, int32& Height) const override;
	
	/** ��ȡ����ͼ�ֱ��� */
	virtual int32 GetStaticLightMapResolution() const override;

	virtual ELightMapInteractionType GetStaticLightingType() const override;

	virtual bool ShouldRenderSelected() const override;

	virtual void InvalidateLightingCacheDetailed(bool bInvalidateBuildEnqueuedLighting, bool bTranslationOnly) override;

	/** �������ù���ͼ�ֱ��ʣ����ڽ������������Զ����� */
	void SetLightmapResolution(int32 InResolution);

	struct FLightmassPrimitiveSettings LightmassSettings;

	/** ��ǰ����Ĺ���ͼID�����뻧�͵�ʱ����Json�з����л�����ֵ */
	//FGuid LightMapID;

private:
	/** ProceduralMeshתΪStaticMesh */
	UStaticMesh* ConvertToStaticMesh();

	/** �Զ��������ͼ�ֱ��� */
	void AutoSetLightMapResolution(float InScale = 1.f);

public:
	/** ���ݴ���������㣨˳ʱ��/��ʱ��Կɣ��ͺ�ȣ��������¼������������ϼ��������������ǻ��ʷ� */
	static void Triangulate_Extrusion(const TArray<FVector>& InOutlineVertices, const float Thickness, TArray<FArmyProcMeshVertex>& OutVertices, TArray<FArmyProcMeshTriangle>& OutTriangles, float InRepeatDistance = 50.f);

	/** ���¼��㴫��Ķ����Tangent */
	static void RecomputeTangents(TArray<FArmyProcMeshVertex>& InVertices, TArray<FArmyProcMeshTriangle>& InTriangles);

	/** ���ݴ���������㣨˳ʱ��/��ʱ��Կɣ��ͺ�ȣ��������¼������������ϼ����������ɼ����壬�÷������Զ�����֪ͨGPU���»��� */
	void CreateMeshSection_Extrude(int32 SectionIndex, const TArray<FVector>& InOutlineVertices, float InTickness);

	/**
	 * �����Ѽ���õ�Vertices��Triangles������/����ָ����Section
	 * @param SectionIndex - int32 - Ҫ��������µ���������
	 * @param InVertices - TArray<FArmyProcMeshVertex> & - �Ѽ���õĶ��㻺��
	 * @param InTriangles - TArray<FArmyProcMeshTriangle> & - �Ѽ���õ���������
	 * @param bRecomputeTangets - bool - �Ƿ���Ҫ����Tangents��Ĭ�ϻ��Զ����㣬����������������������ˣ��˴����Դ�false
	 * @param bRemainLastMaterial - bool - �Ƿ����ʹ��֮ǰ��Section�ϵĲ���
	 * @return void - 
	 */
	void CreateMeshSection(int32 SectionIndex, TArray<FArmyProcMeshVertex>& InVertices, TArray<FArmyProcMeshTriangle>& InTriangles, bool bRecomputeTangets = true, bool bRemainLastMaterial = true);

	/**
	 *	Create/replace a section for this procedural mesh component.
	 *	This function is deprecated for Blueprints because it uses the unsupported 'Color' type. Use new 'Create Mesh Section' function which uses LinearColor instead.
	 *	@param	SectionIndex		Index of the section to create or replace.
	 *	@param	Vertices			Vertex buffer of all vertex positions to use for this mesh section.
	 *	@param	Triangles			Index buffer indicating which vertices make up each triangle. Length must be a multiple of 3.
	 *	@param	Normals				Optional array of normal vectors for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	UV0					Optional array of texture co-ordinates for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	VertexColors		Optional array of colors for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	Tangents			Optional array of tangent vector for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	bCreateCollision	Indicates whether collision should be created for this section. This adds significant cost.
	 */
	//UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh", meta = (DeprecatedFunction, DeprecationMessage = "This function is deprecated for Blueprints because it uses the unsupported 'Color' type. Use new 'Create Mesh Section' function which uses LinearColor instead.", DisplayName = "Create Mesh Section FColor", AutoCreateRefTerm = "Normals,UV0,VertexColors,Tangents"))
	//void CreateMeshSection(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FVector2D>& UV0, const TArray<FColor>& VertexColors, const TArray<FArmyProcMeshTangent>& Tangents, bool bCreateCollision);

	/**
	 *	Create/replace a section for this procedural mesh component.
	 *	@param	SectionIndex		Index of the section to create or replace.
	 *	@param	Vertices			Vertex buffer of all vertex positions to use for this mesh section.
	 *	@param	Triangles			Index buffer indicating which vertices make up each triangle. Length must be a multiple of 3.
	 *	@param	Normals				Optional array of normal vectors for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	UV0					Optional array of texture co-ordinates for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	VertexColors		Optional array of colors for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	Tangents			Optional array of tangent vector for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	bCreateCollision	Indicates whether collision should be created for this section. This adds significant cost.
	 */
	//UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh", meta = (DisplayName = "Create Mesh Section", AutoCreateRefTerm = "Normals,UV0,VertexColors,Tangents"))
	//void CreateMeshSection_LinearColor(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<int32>& Triangles, const TArray<FVector>& Normals, const TArray<FVector2D>& UV0, const TArray<FLinearColor>& VertexColors, const TArray<FArmyProcMeshTangent>& Tangents, bool bCreateCollision);


	/**
	 *	Updates a section of this procedural mesh component. This is faster than CreateMeshSection, but does not let you change topology. Collision info is also updated.
	 *	This function is deprecated for Blueprints because it uses the unsupported 'Color' type. Use new 'Create Mesh Section' function which uses LinearColor instead.
	 *	@param	Vertices			Vertex buffer of all vertex positions to use for this mesh section.
	 *	@param	Normals				Optional array of normal vectors for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	UV0					Optional array of texture co-ordinates for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	VertexColors		Optional array of colors for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	Tangents			Optional array of tangent vector for each vertex. If supplied, must be same length as Vertices array.
	 */
	//UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh", meta = (DeprecatedFunction, DeprecationMessage = "This function is deprecated for Blueprints because it uses the unsupported 'Color' type. Use new 'Update Mesh Section' function which uses LinearColor instead.", DisplayName = "Update Mesh Section FColor", AutoCreateRefTerm = "Normals,UV0,VertexColors,Tangents"))
	//void UpdateMeshSection(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<FVector>& Normals, const TArray<FVector2D>& UV0, const TArray<FColor>& VertexColors, const TArray<FArmyProcMeshTangent>& Tangents);

	/**
	 *	Updates a section of this procedural mesh component. This is faster than CreateMeshSection, but does not let you change topology. Collision info is also updated.
	 *	@param	Vertices			Vertex buffer of all vertex positions to use for this mesh section.
	 *	@param	Normals				Optional array of normal vectors for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	UV0					Optional array of texture co-ordinates for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	VertexColors		Optional array of colors for each vertex. If supplied, must be same length as Vertices array.
	 *	@param	Tangents			Optional array of tangent vector for each vertex. If supplied, must be same length as Vertices array.
	 */
	//UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh", meta = (DisplayName = "Update Mesh Section", AutoCreateRefTerm = "Normals,UV0,VertexColors,Tangents"))
	//void UpdateMeshSection_LinearColor(int32 SectionIndex, const TArray<FVector>& Vertices, const TArray<FVector>& Normals, const TArray<FVector2D>& UV0, const TArray<FLinearColor>& VertexColors, const TArray<FArmyProcMeshTangent>& Tangents);


	/** Clear a section of the procedural mesh. Other sections do not change index. */
	UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh")
	void ClearMeshSection(int32 SectionIndex);

	/** Clear all mesh sections and reset to empty state */
	UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh")
	void ClearAllMeshSections();

	/** Control visibility of a particular section */
	UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh")
	void SetMeshSectionVisible(int32 SectionIndex, bool bNewVisibility);

	/** Returns whether a particular section is currently visible */
	UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh")
	bool IsMeshSectionVisible(int32 SectionIndex) const;

	/** Returns number of sections currently created for this component */
	UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh")
	int32 GetNumSections() const;

	/** Add simple collision convex to this component */
	UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh")
	void AddCollisionConvexMesh(TArray<FVector> ConvexVerts);

	/** Add simple collision convex to this component */
	UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh")
	void ClearCollisionConvexMeshes();

	/** Function to replace _all_ simple collision in one go */
	void SetCollisionConvexMeshes(const TArray< TArray<FVector> >& ConvexMeshes);

	//~ Begin Interface_CollisionDataProvider Interface
	virtual bool GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData) override;
	virtual bool ContainsPhysicsTriMeshData(bool InUseAllTriData) const override;
	virtual bool WantsNegXTriMesh() override{ return false; }
	//~ End Interface_CollisionDataProvider Interface

	/** 
	 *	Controls whether the complex (Per poly) geometry should be treated as 'simple' collision. 
	 *	Should be set to false if this component is going to be given simple collision and simulated.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Procedural Mesh")
	bool bUseComplexAsSimpleCollision;

	/**
	*	Controls whether the physics cooking should be done off the game thread. This should be used when collision geometry doesn't have to be immediately up to date (For example streaming in far away objects)
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Procedural Mesh")
	bool bUseAsyncCooking;

	/** Collision data */
	UPROPERTY(Instanced)
	class UBodySetup* ProcMeshBodySetup;

	/** 
	 *	Get pointer to internal data for one section of this procedural mesh component. 
	 *	Note that pointer will becomes invalid if sections are added or removed.
	 */
	FArmyProcMeshSection* GetProcMeshSection(int32 SectionIndex);

	/** Replace a section with new section geometry */
	void SetProcMeshSection(int32 SectionIndex, const FArmyProcMeshSection& Section);

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual class UBodySetup* GetBodySetup() override;
	virtual UMaterialInterface* GetMaterialFromCollisionFaceIndex(int32 FaceIndex, int32& SectionIndex) const override;
	//~ End UPrimitiveComponent Interface.

	//~ Begin UMeshComponent Interface.
	virtual int32 GetNumMaterials() const override;
	//~ End UMeshComponent Interface.

	//~ Begin UObject Interface
	virtual void PostLoad() override;
	//~ End UObject Interface.

	/** ֱ��ƽ�̹���UV�������ǲ�ɢ����ƽ�� */
	bool bAutoWrapFlatLightUV = false;

	bool BSkitLine = false;

	/** ����ͼ�ֱ��� */
	int32 LightMapResolution;

	/** ��GenMeshתΪStaticMesh����ʱ����ã�Lightmass�決���ع������ݺ󣬻�������������SMC�У�������Proxy�У�Ҳ���õ� */
	UPROPERTY()
		UStaticMeshComponent* TempConvertedMeshComponentForBuildingLight;

	FGuid GetLightMapIDFromOwner() const;

	//���ݴ˱�����������Ƿ���Ҫ�決��Ĭ��false
	bool bBuildStaticLighting;

private:
	//~ Begin USceneComponent Interface.
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ Begin USceneComponent Interface.


	/** Update LocalBounds member from the local box of each section */
	void UpdateLocalBounds();
	/** Ensure ProcMeshBodySetup is allocated and configured */
	void CreateProcMeshBodySetup();
	/** Mark collision data as dirty, and re-create on instance if necessary */
	void UpdateCollision();
	/** Once async physics cook is done, create needed state */
	void FinishPhysicsAsyncCook(UBodySetup* FinishedBodySetup);

	/** Helper to create new body setup objects */
	UBodySetup* CreateBodySetupHelper();

	/** Array of sections of mesh */
	TArray<FArmyProcMeshSection> ProcMeshSections;

	/** Convex shapes used for simple collision */
	UPROPERTY()
	TArray<FKConvexElem> CollisionConvexElems;

	/** Local space bounds of mesh */
	UPROPERTY()
	FBoxSphereBounds LocalBounds;
	
	/** Queue for async body setups that are being cooked */
	UPROPERTY(transient)
	TArray<UBodySetup*> AsyncBodySetupQueue;

	friend class FArmyProceduralMeshSceneProxy;
};


