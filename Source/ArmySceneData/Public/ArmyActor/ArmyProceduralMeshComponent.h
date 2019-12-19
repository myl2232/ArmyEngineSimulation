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

	/** 实现收集光照信息接口 */
	virtual void GetStaticLightingInfo(FStaticLightingPrimitiveInfo& OutPrimitiveInfo, const TArray<ULightComponent*>& InRelevantLights, const FLightingBuildOptions& Options) override;

	/** 支持静态光 */
	virtual bool SupportsStaticLighting() const override { return true; }
	
	/** 烘焙光照数据有效 */
	virtual bool IsPrecomputedLightingValid() const override { return true; }
	
	/** 获取光照图分辨率 */
	virtual bool GetLightMapResolution(int32& Width, int32& Height) const override;
	
	/** 获取光照图分辨率 */
	virtual int32 GetStaticLightMapResolution() const override;

	virtual ELightMapInteractionType GetStaticLightingType() const override;

	virtual bool ShouldRenderSelected() const override;

	virtual void InvalidateLightingCacheDetailed(bool bInvalidateBuildEnqueuedLighting, bool bTranslationOnly) override;

	/** 主动设置光照图分辨率，后期将不会根据面积自动计算 */
	void SetLightmapResolution(int32 InResolution);

	struct FLightmassPrimitiveSettings LightmassSettings;

	/** 当前组件的光照图ID，载入户型的时候会从Json中反序列化并赋值 */
	//FGuid LightMapID;

private:
	/** ProceduralMesh转为StaticMesh */
	UStaticMesh* ConvertToStaticMesh();

	/** 自动计算光照图分辨率 */
	void AutoSetLightMapResolution(float InScale = 1.f);

public:
	/** 根据传入的轮廓点（顺时针/逆时针皆可）和厚度（正数向下挤出，负数向上挤出），进行三角化剖分 */
	static void Triangulate_Extrusion(const TArray<FVector>& InOutlineVertices, const float Thickness, TArray<FArmyProcMeshVertex>& OutVertices, TArray<FArmyProcMeshTriangle>& OutTriangles, float InRepeatDistance = 50.f);

	/** 重新计算传入的顶点的Tangent */
	static void RecomputeTangents(TArray<FArmyProcMeshVertex>& InVertices, TArray<FArmyProcMeshTriangle>& InTriangles);

	/** 根据传入的轮廓点（顺时针/逆时针皆可）和厚度（正数向下挤出，负数向上挤出），生成几何体，该方法会自动调用通知GPU更新缓存 */
	void CreateMeshSection_Extrude(int32 SectionIndex, const TArray<FVector>& InOutlineVertices, float InTickness);

	/**
	 * 根据已计算好的Vertices和Triangles，创建/更新指定的Section
	 * @param SectionIndex - int32 - 要创建或更新的区域索引
	 * @param InVertices - TArray<FArmyProcMeshVertex> & - 已计算好的顶点缓存
	 * @param InTriangles - TArray<FArmyProcMeshTriangle> & - 已计算好的索引缓存
	 * @param bRecomputeTangets - bool - 是否需要计算Tangents，默认会自动计算，如果你有自信在外面计算好了，此处可以传false
	 * @param bRemainLastMaterial - bool - 是否继续使用之前此Section上的材质
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

	/** 直接平铺光照UV，而不是拆散最优平铺 */
	bool bAutoWrapFlatLightUV = false;

	bool BSkitLine = false;

	/** 光照图分辨率 */
	int32 LightMapResolution;

	/** 将GenMesh转为StaticMesh，临时保存好，Lightmass烘焙返回光照数据后，还会把数据填入次SMC中，拷贝到Proxy中，也会用到 */
	UPROPERTY()
		UStaticMeshComponent* TempConvertedMeshComponentForBuildingLight;

	FGuid GetLightMapIDFromOwner() const;

	//根据此变量决定组件是否需要烘焙，默认false
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


