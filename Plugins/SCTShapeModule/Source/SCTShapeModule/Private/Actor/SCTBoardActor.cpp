#include "SCTBoardActor.h"
#include "SCTShapeActor.h"
#include "SCTBoardShape.h"
#include "Triangulation.h"
#include "UObject/ConstructorHelpers.h"
#include "GTEngineLibrary.h"
#include "SCTGlobalDataForShape.h"
#include "MaterialManager.h"
#include "SCTShapeManager.h"
#include "Engine/CollisionProfile.h"
#include "Materials/MaterialInstanceDynamic.h"

#define TEXTURE_REPEAT_DISTANCE 512

ASCTBoardActor::ASCTBoardActor()
	: BoardThickness(18.0f)
	, bRotateTopBottomTexture(0)
	, bRotateSideTexture(0)
	, bDonotRepeatTexture(0)
    , bIsFirstCreateCompoent(true)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ASCTBoardActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASCTBoardActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FBox ASCTBoardActor::GetCollisionBox() const
{
	FBoardShape *BoardShape = (FBoardShape *)ShapeData;
	float Width = BoardShape->GetShapeWidth();
	float Depth = BoardShape->GetShapeDepth();
	float Height = BoardShape->GetShapeHeight();
	int32 UseType = BoardShape->GetShapeCategory();
	switch (UseType)
	{
	case BUT_HorizontalBoard:   // 横板
	case BUT_TopBoard:
	case BUT_BottomBoard:
	case BUT_MobileBoard:
		return FBox(FVector::ZeroVector, FVector(Width, Depth, Height));
	case BUT_VerticalBoard:   // 竖板
	case BUT_LeftBoard:
	case BUT_RightBoard:
	case BUT_RevealBoard:
	case BUT_ExposureBoard:
	case BUT_AdjustBoard:
		return FBox(FVector::ZeroVector, FVector(Height, Depth, Width));
	case BUT_BackBoard:   // 背板
	case BUT_FrontBoard:
	case BUT_EnhanceBoard:
	case BUT_ConvergentBoard:
	case BUT_SkirtingBoard:
			return FBox(FVector::ZeroVector, FVector(Depth, Height, Width));
	default:
		return ASCTShapeActor::GetCollisionBox();
	}
}

void ASCTBoardActor::UpdateActorDimension()
{
	UpdateBoardPrimitiveInfo();
}

void ASCTBoardActor::UpdateActorPosition()
{
	FVector ActorPos(ShapeData->GetShapePosX(), ShapeData->GetShapePosY(), ShapeData->GetShapePosZ());
	
	//板件
	FBoardShape *BoardShape = static_cast<FBoardShape*>(ShapeData);
	//修正板件外延值
	ActorPos += FVector(BoardShape->GetAxisXStretch(), BoardShape->GetAxisYStretch(), BoardShape->GetAxisZStretch());
	//修正板件内缩值（按板件方向调整内缩方向）
	ActorPos += BoardShape->GetShapeContractionDire();

	SetActorRelativeLocation(ActorPos / 10.f);
}

void ASCTBoardActor::UpdateActorBoardMaterial()
{
	// 优先使用外部设置的材质实例
	if(TemporaryMaterial)
	{
		SetBoardMaterial(TemporaryMaterial);
		// 更新板件材质顶部底部材质的时候，顺便更新封边材质，因为封边材质受板件上下面材质影响
		UpdateEdgeMaterial();
		return;
	}

	FBoardShape* BoardShape = StaticCast<FBoardShape*>(ShapeData);
	int32 MaterialID = BoardShape->GetMaterialData().ID;
	const FString PakUrl = BoardShape->GetMaterialData().Url;	
	const FString PakMd5 = BoardShape->GetMaterialData().MD5;
	const FString PakOptimizeParam = BoardShape->GetMaterialData().OptimizeParam;
	if (PakUrl.IsEmpty()) return;
	auto ApplayMaterialWithActor = [this, BoardShape](UMaterialInterface * InMaterialInstance, const FString & InErrorMsg)
	{
		if (InMaterialInstance)
		{
			if (FMath::IsNearlyZero(BoardShape->GetMaterialRotateValue()) == false)
			{
				UMaterialInstanceDynamic * DynamicMaterial = Cast<UMaterialInstanceDynamic>(InMaterialInstance);				
				if (DynamicMaterial)
				{
					UMaterialInstanceDynamic* NewMID = UMaterialInstanceDynamic::Create(DynamicMaterial->Parent, nullptr);
					NewMID->CopyParameterOverrides(DynamicMaterial);					
					auto SetScalarParameterValue = [NewMID](const FName & InName, const float InValue)
					{
						float ParaVaule = 0.0f;
						if (NewMID->GetScalarParameterValue(InName, ParaVaule))
						{
							NewMID->SetScalarParameterValue(InName, InValue);
						}
					};
					// 此名称一定要与DLC common包中的材质参数保持一致
					SetScalarParameterValue(FName(TEXT("NormalMap_UV_Ratote")), BoardShape->GetMaterialRotateValue());
					SetScalarParameterValue(FName(TEXT("Diffuse_UV_Rotate")), BoardShape->GetMaterialRotateValue());
					SetBoardMaterial(NewMID);
				}
			}
			else
			{
				SetBoardMaterial(InMaterialInstance);
			}
			// 更新板件材质顶部底部材质的时候，顺便更新封边材质，因为封边材质受板件上下面材质影响
			UpdateEdgeMaterial();
		}	
		else
		{
			UMaterialInterface * DefaultMaterial = FMaterialManagerInstatnce::GetIns().GetMaterialManagerPtr()->GetLitLitMaterial();
			UMaterialInstanceDynamic * DefaultMaterialInstance = UMaterialInstanceDynamic::Create(DefaultMaterial,nullptr);
			SetBoardMaterial(DefaultMaterialInstance);
		}
	};
	FMaterialLoadCallback MCallback;
	MCallback.BindLambda(ApplayMaterialWithActor);
	FMaterialManagerInstatnce::GetIns().GetMaterialManagerPtr()->GetOrCreateMaterialInterfaceWithParameter(
		EMaterialType::Board_Material, PakUrl, FString::FromInt(MaterialID), PakMd5, PakOptimizeParam, MCallback);
}

void ASCTBoardActor::UpdateActorCoveredEdgeMaterial()
{
	FBoardShape* BoardShape = StaticCast<FBoardShape*>(ShapeData);
	int64 MaterialID = BoardShape->GetCoveredEdgeMaterialValue().ID;
	const FString MaterialUrl = BoardShape->GetCoveredEdgeMaterialValue().Url;
	const FString MaterialMd5 = BoardShape->GetCoveredEdgeMaterialValue().MD5;
	const FString PakOptimizeParam = BoardShape->GetCoveredEdgeMaterialValue().OptimizeParam;
	const TArray<int32> & EdgesIndex = BoardShape->GetLightCoveredEdges();
	auto ApplayMaterialWithActor = [this,&EdgesIndex](UMaterialInterface * InMaterialInstance, const FString & InErrorMsg)
	{
		if (InMaterialInstance)
		{
			SetBoardEdgeMaterial(InMaterialInstance, EdgesIndex);
		}
		else
		{
			UpdateEdgeMaterial();
		}
	};
	FMaterialLoadCallback MCallback;
	MCallback.BindLambda(ApplayMaterialWithActor);		
	FMaterialManagerInstatnce::GetIns().GetMaterialManagerRef().GetOrCreateMaterialInterfaceWithParameter(EMaterialType::Covered_Material,
		MaterialUrl, FString::FromInt(MaterialID), MaterialMd5, PakOptimizeParam, MCallback);
}

void ASCTBoardActor::UpdateAluminumFildMaterial()
{
	FBoardShape* BoardShape = StaticCast<FBoardShape*>(ShapeData);
	auto ApplayMaterialWithActor = [this](UMaterialInterface * InMaterialInstance, const FString & InErrorMsg)
	{
		if (InMaterialInstance)
		{
			SetBoardTopEdgeMaterial(InMaterialInstance);
		}
		else
		{
			UpdateActorBoardMaterial();
		}
	};
	if (BoardShape->IsUsingAluminumFild())
	{		
		int32 MaterialID = 0;
		FString MaterialUrl, MaterialMd5, Paramter;
		if (BoardShape->GetVeneerInfoData().ID != 0)
		{
			MaterialID = BoardShape->GetVeneerInfoData().ID;
			MaterialUrl = BoardShape->GetVeneerInfoData().Url;
			MaterialMd5 = BoardShape->GetVeneerInfoData().MD5;
			Paramter = BoardShape->GetVeneerInfoData().OptimizeParam;
		}		
		FMaterialLoadCallback MCallback;
		MCallback.BindLambda(ApplayMaterialWithActor);
		FMaterialManagerInstatnce::GetIns().GetMaterialManagerRef().GetOrCreateMaterialInterfaceWithParameter(EMaterialType::AluminumFoil_Material,
			MaterialUrl, FString::FromInt(MaterialID), MaterialMd5, Paramter, MCallback);
	}
	else
	{
		UpdateActorBoardMaterial();
	}
}


void ASCTBoardActor::UpdateEdgeMaterial(const bool IsBoardMaterial/* = true*/)
{	
	// 优先使用外部设置的材质实例
	if (TemporaryMaterial)
	{
		SetBoardEdgeMaterial(TemporaryMaterial);
		return;
	}

	FBoardShape* BoardShape = StaticCast<FBoardShape*>(ShapeData);		
	UMaterialInterface * MaterialInterface = nullptr;
	// 如果是同色的，则使用板件的材质
	if (IsBoardMaterial)
	{		
		const int64 MaterialID = BoardShape->GetMaterialData().ID;
		if (MaterialID != 0)
		{
			MaterialInterface =
				FMaterialManagerInstatnce::GetIns().GetMaterialManagerPtr()->GetMaterialInterface(EMaterialType::Board_Material, FString::FromInt(MaterialID));
		}
	}
	else
	{
		const int32 edgeMaterialId = BoardShape->GetEdgeBandingData().ID;
		if (edgeMaterialId != 0)
		{
			MaterialInterface =
				FMaterialManagerInstatnce::GetIns().GetMaterialManagerPtr()->GetMaterialInterface(EMaterialType::Edge_Material, FString::FromInt(edgeMaterialId));
		}
	}	
	if (!MaterialInterface)
	{
		UMaterialInterface * DefaultMaterials = FMaterialManagerInstatnce::GetIns().GetMaterialManagerPtr()->GetLitLitMaterial();
		MaterialInterface = Cast<UMaterialInterface>(UMaterialInstanceDynamic::Create(DefaultMaterials, nullptr));
		SetBoardMaterial(MaterialInterface);
	}
	SetBoardEdgeMaterial(MaterialInterface);
}

void ASCTBoardActor::SetBoardPointsAndThickness(float InThickness)
{
	BoardThickness = InThickness;
	UpdateBoardPrimitiveInfo();
}

void ASCTBoardActor::SetBoardThickness(float InThickness)
{
	BoardThickness = InThickness;
	UpdateBoardPrimitiveInfo();
}

void ASCTBoardActor::SetBoardMaterial(UMaterialInterface *InOverrideMaterial)
{
	for (int32 Index = 0; Index < BoardPrimitiveArray.Num(); ++Index)
	{
		BoardPrimitiveArray[Index]->SetMaterial(0, InOverrideMaterial);
	}
}

void ASCTBoardActor::SetBoardEdgeMaterial(class UMaterialInterface *InOverrideMaterial)
{
	for (int32 Index = 0; Index < BoardEdgePrimitiveArray.Num(); ++Index)
	{
		BoardEdgePrimitiveArray[Index]->SetMaterial(0, InOverrideMaterial);
	}	
}

void ASCTBoardActor::SetBoardEdgeMaterial(UMaterialInterface * InOverrideMaterial, const TArray<int32>& InSpecialEdgeIndex)
{
	for (int32 Index = 0; Index < InSpecialEdgeIndex.Num(); ++Index)
	{
		const int32 TrueIndex = InSpecialEdgeIndex[Index] - 1;
		check(BoardEdgePrimitiveArray.IsValidIndex(TrueIndex))
		BoardEdgePrimitiveArray[ TrueIndex ]->SetMaterial(0, InOverrideMaterial);
	}
}

void ASCTBoardActor::SetBoardTopEdgeMaterial(UMaterialInterface * InOverrideMaterial)
{
	check(BoardPrimitiveArray.IsValidIndex(1));
	BoardPrimitiveArray[1]->SetMaterial(0, InOverrideMaterial);
}

void ASCTBoardActor::SetCollisionProfileName(FName InCollisionName)
{
	for (int32 Index = 0; Index < BoardEdgePrimitiveArray.Num(); Index++)
	{
		BoardEdgePrimitiveArray[Index]->SetCollisionProfileName(InCollisionName);
	}
	for (int32 Index = 0; Index < BoardPrimitiveArray.Num(); Index++)
	{
		BoardPrimitiveArray[Index]->SetCollisionProfileName(InCollisionName);
	}
}

void ASCTBoardActor::SetRotateTopBottomTexture(bool bInRotate)
{
	uint8 DesireValue = bInRotate ? 1 : 0;
	if (bRotateTopBottomTexture != DesireValue)
	{
		bRotateTopBottomTexture = DesireValue;
		//FixMes: isOk?
		UPNTUVPrimitive *Primitive = BoardPrimitiveArray[0];
		TArray<FPUVVertex> &Vertices = Primitive->GetVertices();
		if (Vertices.Num() > 0)
		{
			int32 NumOutline = BoardCWPoints.Num();
			for (int32 i = 0; i < NumOutline; ++i)
			{
				FVector2D &TexcoordTop = Vertices[i].Texcoord0;
				Vertices[i].Texcoord0.Set(TexcoordTop.Y, TexcoordTop.X);
				FVector2D &TexcoordBottom = Vertices[i + NumOutline].Texcoord0;
				Vertices[i + NumOutline].Texcoord0.Set(TexcoordBottom.Y, TexcoordBottom.X);
			}
			Primitive->MarkRenderStateDirty();
		}
	}
}

void ASCTBoardActor::SetRotateSideTexture(bool bInRotate)
{
	uint8 DesireValue = bInRotate ? 1 : 0;
	if (bRotateSideTexture != DesireValue)
	{
		bRotateSideTexture = DesireValue;
		//FixMes: isOk?
		UPNTUVPrimitive *Primitive = BoardPrimitiveArray[0];
		TArray<FPUVVertex> &Vertices = Primitive->GetVertices();
		if (Vertices.Num() > 0)
		{
			int32 NumOutline = BoardCWPoints.Num();
			int32 NumOffset = NumOutline * 2;
			int32 NumIter = NumOutline * 4;
			for (int32 i = 0; i < NumIter; ++i)
			{
				FVector2D &Texcoord = Vertices[i + NumOffset].Texcoord0;
				Vertices[i + NumOffset].Texcoord0.Set(Texcoord.Y, Texcoord.X);
			}
			Primitive->MarkRenderStateDirty();
		}
	}
}

void ASCTBoardActor::SetDonotRepeatTexture(bool bInDonotRepeat)
{
	uint8 DesireValue = bInDonotRepeat ? 1 : 0;
	if (bDonotRepeatTexture != DesireValue)
	{
		bDonotRepeatTexture = DesireValue;
		FBox Box(BoardCWPoints);
		Box.Min /= 10.0f;
		Box.Max /= 10.0f;		
		float TexcoordXDenominator = bDonotRepeatTexture ? Box.GetSize().X : TEXTURE_REPEAT_DISTANCE;
		float TexcoordYDenominator = bDonotRepeatTexture ? Box.GetSize().Y : TEXTURE_REPEAT_DISTANCE;
		//FixMes: isOk?
		UPNTUVPrimitive *Primitive = BoardPrimitiveArray[0];
		TArray<FPUVVertex> &Vertices = Primitive->GetVertices();
		if (Vertices.Num() > 0)
		{
			int32 NumOutline = BoardCWPoints.Num();
			for (int32 i = 0; i < NumOutline; ++i)
			{
				if (bRotateTopBottomTexture == 0)
				{
					Vertices[i].Texcoord0.Set((Vertices[i].Position.X - Box.Min.X) / TexcoordXDenominator, (Vertices[i].Position.Y - Box.Min.Y) / TexcoordYDenominator);
					Vertices[i + NumOutline].Texcoord0.Set((Vertices[i + NumOutline].Position.X - Box.Min.X) / TexcoordXDenominator, (Vertices[i + NumOutline].Position.Y - Box.Min.Y) / TexcoordYDenominator);
				}
				else
				{
					Vertices[i].Texcoord0.Set((Vertices[i].Position.Y - Box.Min.Y) / TexcoordXDenominator, (Vertices[i].Position.X - Box.Min.X) / TexcoordYDenominator);
					Vertices[i + NumOutline].Texcoord0.Set((Vertices[i + NumOutline].Position.Y - Box.Min.Y) / TexcoordXDenominator, (Vertices[i + NumOutline].Position.X - Box.Min.X) / TexcoordYDenominator);
				}
			}
			Primitive->MarkRenderStateDirty();
		}
	}
}

void ASCTBoardActor::UpdateBoardPrimitiveInfo()
{
	//重新计算板件点位
	FBoardShape* BoardShape = StaticCast<FBoardShape*>(ShapeData);

	BoardThickness = BoardShape->GetShapeHeight();
	BoardShape->CalcBoardHolePoints(BoardHoleCWPoint);
	BoardShape->CalcBoardPoint(BoardCWPointsArray, BoardCWPoints);

	TArray<uint16> IndexList;
	TArray<FVector> VertexArray;
	
	bool Result = UGTEngineLibrary::TriangulatePolyWithFastEarcut(BoardCWPoints, BoardHoleCWPoint, VertexArray, IndexList);
	if (Result == false) return;

	TArray<TArray<FPUVVertex>> TopAndBottomPUVArray;
	TArray<TArray<uint16>>     TopAndBottomIndexArray;
	TArray<TArray<FPUVVertex>> EdgesPUVArray;
	TArray<TArray<uint16>>     EdgesIndexArray;

	FBox Box(BoardCWPoints);
	int32 NumOutlinePoint = VertexArray.Num();
	float TexcoordXDenominator = bDonotRepeatTexture ? Box.GetSize().X : TEXTURE_REPEAT_DISTANCE;
	float TexcoordYDenominator = bDonotRepeatTexture ? Box.GetSize().Y : TEXTURE_REPEAT_DISTANCE;
	int32 NumTopTriangle = IndexList.Num() / 3;	

	// 顶面与底面
	TArray<FPUVVertex> TempBottomVertices;
	TArray<FPUVVertex> TempTopVertices;
	TArray<uint16>     TempBottomIndex;
	TArray<uint16>     TempTopIndex;
	{
		TempBottomIndex.SetNumUninitialized(IndexList.Num());
		TempTopIndex.SetNumUninitialized(IndexList.Num());
		TempBottomVertices.SetNumUninitialized(NumOutlinePoint);
		TempTopVertices.SetNumUninitialized(NumOutlinePoint);
		for (int32 i = 0; i < NumOutlinePoint; ++i)
		{
			FPUVVertex Vertex;
			Vertex.Position = VertexArray[i];
			if (bRotateTopBottomTexture == 0)
				Vertex.Texcoord0.Set((Vertex.Position.X - Box.Min.X) / TexcoordXDenominator, (Vertex.Position.Y - Box.Min.Y) / TexcoordYDenominator);
			else
				Vertex.Texcoord0.Set((Vertex.Position.Y - Box.Min.Y) / TexcoordYDenominator, (Vertex.Position.X - Box.Min.X) / TexcoordXDenominator);
			Vertex.Position /= 10.0f;
			TempBottomVertices[i] = Vertex; // 底面点
			Vertex.Position.Z = BoardThickness / 10.0f;
			TempTopVertices[i] = Vertex; // 顶面点
		}
		for (int32 i = 0; i < NumTopTriangle; ++i)
		{
			TempTopIndex[i * 3] = IndexList[i * 3];
			TempTopIndex[i * 3 + 1] = IndexList[i * 3 + 2];
			TempTopIndex[i * 3 + 2] = IndexList[i * 3 + 1];
			TempBottomIndex[i * 3] = IndexList[i * 3];
			TempBottomIndex[i * 3 + 1] = IndexList[i * 3 + 1];
			TempBottomIndex[i * 3 + 2] = IndexList[i * 3 + 2];
		}
	}	

	// 填充顶面底面索引	
	TopAndBottomPUVArray.Emplace(TempBottomVertices);
	TopAndBottomPUVArray.Emplace(TempTopVertices);
	TopAndBottomIndexArray.Emplace(TempBottomIndex);
	TopAndBottomIndexArray.Emplace(TempTopIndex);

	if (BoardShape->IsNeedRebuilEdgeAndHolePointSet())
	{
		BoardShape->ReBuildEdge(BoardCWPointsArray, BoardHoleCWPoint);
	}
	 //填充侧面点
	{
		auto EdgeTriangulatePoly = [=](const TArray<FVector> & InVertexArray,
			TArray<FPUVVertex> & OutVertexArray, TArray<uint16> & OutIndexArray)
		{
			float LengthHorizontal = 0.0f;
			for (int32 Index = 0; Index < InVertexArray.Num(); ++Index)
			{
				FPUVVertex Vertex0;
				Vertex0.Position = InVertexArray[Index] / 10.0f;
				FPUVVertex Vertex1;
				Vertex1.Position = FVector(Vertex0.Position.X, Vertex0.Position.Y, BoardThickness / 10.0f);
				FPUVVertex Vertex2;
				Vertex2.Position = InVertexArray[++Index] / 10.0f;
				float Length = (Vertex2.Position - Vertex0.Position).Size() * 10.0f; // 长度转换回毫米
				FPUVVertex Vertex3;
				Vertex3.Position = FVector(Vertex2.Position.X, Vertex2.Position.Y, BoardThickness / 10.0f);
				if (bRotateSideTexture == 0)
				{
					Vertex0.Texcoord0.Set(LengthHorizontal / TEXTURE_REPEAT_DISTANCE, 0.0f);
					Vertex1.Texcoord0.Set(LengthHorizontal / TEXTURE_REPEAT_DISTANCE, BoardThickness / TEXTURE_REPEAT_DISTANCE);
					LengthHorizontal += Length;
					Vertex2.Texcoord0.Set(LengthHorizontal / TEXTURE_REPEAT_DISTANCE, 0.0f);
					Vertex3.Texcoord0.Set(LengthHorizontal / TEXTURE_REPEAT_DISTANCE, BoardThickness / TEXTURE_REPEAT_DISTANCE);
				}
				else
				{
					Vertex0.Texcoord0.Set(0.0f, LengthHorizontal / TEXTURE_REPEAT_DISTANCE);
					Vertex1.Texcoord0.Set(BoardThickness / TEXTURE_REPEAT_DISTANCE, LengthHorizontal / TEXTURE_REPEAT_DISTANCE);
					LengthHorizontal += Length;
					Vertex2.Texcoord0.Set(0.0f, LengthHorizontal / TEXTURE_REPEAT_DISTANCE);
					Vertex3.Texcoord0.Set(BoardThickness / TEXTURE_REPEAT_DISTANCE, LengthHorizontal / TEXTURE_REPEAT_DISTANCE);
				}
				OutVertexArray.Emplace(Vertex0);
				OutVertexArray.Emplace(Vertex1);
				OutVertexArray.Emplace(Vertex2);
				OutVertexArray.Emplace(Vertex3);			
			}					
			uint32 TriangleCount = 2 * InVertexArray.Num() - 2;
			for(uint32 Index = 0; Index < TriangleCount; Index = Index + 2)
			{
				const uint16 VertexIndex = Index ;
				OutIndexArray.Emplace(VertexIndex);
				OutIndexArray.Emplace(VertexIndex + 1);
				OutIndexArray.Emplace(VertexIndex + 2);
				OutIndexArray.Emplace(VertexIndex + 2);
				OutIndexArray.Emplace(VertexIndex + 1);
				OutIndexArray.Emplace(VertexIndex + 3);
			}
		};
		TArray<FPUVVertex>  TempEdgeVertexArray;
		TArray<uint16> TempEdgeIndexArray;
		for (int32 Index = 0; Index < BoardCWPointsArray.Num(); ++Index)
		{
			TempEdgeVertexArray.Empty();
			TempEdgeIndexArray.Empty();
			EdgeTriangulatePoly(BoardCWPointsArray[Index], TempEdgeVertexArray, TempEdgeIndexArray);
			EdgesPUVArray.Emplace(TempEdgeVertexArray);
			EdgesIndexArray.Emplace(TempEdgeIndexArray);			
		}		
	}		

	// 如果有洞，则把洞的封边作为一个整体作为最后一个边，填充到边数组里面	
	
	{
				
		for (const auto & Ref : BoardHoleCWPoint)
		{
			TArray<FPUVVertex> InnerHoleVertices;
			TArray<uint16>     InnerHoleIndex;
			check(Ref.Num() >= 3);							
			for (int32 Index = 0; Index < Ref.Num(); ++Index)
			{
				FPUVVertex TempBottomPoint, TempTopPoint, Vertex;
				Vertex.Position = Ref[Index];
				TempBottomPoint.Position = Ref[Index];
				if (bRotateTopBottomTexture == 0)
					TempBottomPoint.Texcoord0.Set((Vertex.Position.X - Box.Min.X) / TexcoordXDenominator, (Vertex.Position.Y - Box.Min.Y) / TexcoordYDenominator);
				else
					TempBottomPoint.Texcoord0.Set((Vertex.Position.Y - Box.Min.Y) / TexcoordYDenominator, (Vertex.Position.X - Box.Min.X) / TexcoordXDenominator);
				TempBottomPoint.Position /= 10.0f;
				TempTopPoint = TempBottomPoint;
				TempTopPoint.Position.Z = BoardThickness / 10.0f;
				InnerHoleVertices.Emplace(TempBottomPoint);
				InnerHoleVertices.Emplace(TempTopPoint);
			}
			// 计算洞的索引
			{
				uint32 TriangleCount = 2 * Ref.Num() - 2;
				for (uint32 Index = 0; Index < TriangleCount; Index = Index + 2)
				{
					const uint16 VertexIndex = Index;
					InnerHoleIndex.Emplace(VertexIndex + 1);
					InnerHoleIndex.Emplace(VertexIndex);
					InnerHoleIndex.Emplace(VertexIndex + 2);
					InnerHoleIndex.Emplace(VertexIndex + 1);
					InnerHoleIndex.Emplace(VertexIndex + 2);
					InnerHoleIndex.Emplace(VertexIndex + 3);
				}
				// 添加起始点与终止点拟合三角形
				uint16 EndPosIndex = TriangleCount;
				InnerHoleIndex.Emplace(EndPosIndex + 1);
				InnerHoleIndex.Emplace(EndPosIndex);
				InnerHoleIndex.Emplace(1);
				InnerHoleIndex.Emplace(1);
				InnerHoleIndex.Emplace(EndPosIndex);
				InnerHoleIndex.Emplace(0);
			}
			EdgesPUVArray.Emplace(InnerHoleVertices);
			EdgesIndexArray.Emplace(InnerHoleIndex);					
		}
	}
	CreateOrUpdatePrimitiveCompoent(TopAndBottomPUVArray, TopAndBottomIndexArray, EdgesPUVArray, EdgesIndexArray);
}

void ASCTBoardActor::CreateOrUpdatePrimitiveCompoent(const TArray<TArray<FPUVVertex>> & InTopAndBottomPUVArray, const TArray< TArray<uint16> > & InTopAndBottomIndexArray,
	const TArray<TArray<FPUVVertex>> & InEdgesPUVArray, const TArray< TArray<uint16> > & InEdgesIndexArray )
{		
	check(InTopAndBottomPUVArray.Num() == 2);
	check(InTopAndBottomPUVArray.Num() == InTopAndBottomIndexArray.Num());
	check(InEdgesPUVArray.Num() == InEdgesIndexArray.Num());

	auto CreateCompoent = [this](UPNTUVPrimitive * BoardPrimitive ,const TArray<FPUVVertex> & InVertexArray,
		const TArray<uint16> & InIndexArray )
	{		
		BoardPrimitive->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
		this->AddOwnedComponent(BoardPrimitive);
		BoardPrimitive->RegisterComponent();
		BoardPrimitive->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		BoardPrimitive->SetPrimitiveInfo(InVertexArray, InIndexArray);			
	};
	if (bIsFirstCreateCompoent)
	{
		bIsFirstCreateCompoent = false;
								
		// 创建底面组件
		UPNTUVPrimitive * BottomPrimitive = NewObject<UPNTUVPrimitive>(this);
		CreateCompoent(BottomPrimitive, InTopAndBottomPUVArray[0], InTopAndBottomIndexArray[0]);		
		BoardPrimitiveArray.Emplace(BottomPrimitive);

		// 创建顶面组件
		UPNTUVPrimitive * TopPrimitive = NewObject<UPNTUVPrimitive>(this);
		CreateCompoent(TopPrimitive, InTopAndBottomPUVArray[1], InTopAndBottomIndexArray[1]);		
		BoardPrimitiveArray.Emplace(TopPrimitive);		
		
		// 创建各个边组件
		for (int32 Index = 0; Index < InEdgesPUVArray.Num(); Index++)
		{
			FString ComName = TEXT("EdgePrimitive_") + FString::FromInt(Index);
			UPNTUVPrimitive * EdgePrimitive = NewObject<UPNTUVPrimitive>(this);
			EdgePrimitive->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			CreateCompoent(EdgePrimitive, InEdgesPUVArray[Index], InEdgesIndexArray[Index]);
			BoardEdgePrimitiveArray.Emplace(EdgePrimitive);
		}

		UpdateActorBoardMaterial();
		UpdateEdgeMaterial();		
	}
	else
	{
		FBoardShape* BoardShape = StaticCast<FBoardShape*>(ShapeData);
		if (BoardShape->IsNeedRebuilEdgeAndHolePointSet())
		{
			// 清空所有边组件
			for (auto & Ref : BoardEdgePrimitiveArray)
			{
				Ref->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
				Ref->DestroyComponent();
			}
			BoardEdgePrimitiveArray.Empty();
			// 创建各个边组件
			for (int32 Index = 0; Index < InEdgesPUVArray.Num(); Index++)
			{
				FString ComName = TEXT("EdgePrimitive_") + FString::FromInt(Index);
				UPNTUVPrimitive * EdgePrimitive = NewObject<UPNTUVPrimitive>(this);
				EdgePrimitive->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
				CreateCompoent(EdgePrimitive, InEdgesPUVArray[Index], InEdgesIndexArray[Index]);
				BoardEdgePrimitiveArray.Emplace(EdgePrimitive);
			}
			UpdateEdgeMaterial();
		}

		check(InTopAndBottomPUVArray.Num() == BoardPrimitiveArray.Num());
		check(InEdgesPUVArray.Num() == BoardEdgePrimitiveArray.Num());
		// 更新顶面组件
		BoardPrimitiveArray[0]->SetPrimitiveInfo(InTopAndBottomPUVArray[0], InTopAndBottomIndexArray[0]);
 		BoardPrimitiveArray[1]->SetPrimitiveInfo(InTopAndBottomPUVArray[1], InTopAndBottomIndexArray[1]);
 		// 更新边组件
 		for (int32 Index = 0; Index < InEdgesPUVArray.Num(); Index++)
 		{						
 			BoardEdgePrimitiveArray[Index]->SetPrimitiveInfo(InEdgesPUVArray[Index], InEdgesIndexArray[Index]);
 		}
	}
}
