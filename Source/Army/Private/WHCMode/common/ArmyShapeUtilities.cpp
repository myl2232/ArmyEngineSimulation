#include "ArmyShapeUtilities.h"
#include "SCTCabinetShape.h"
#include "SCTSpaceShape.h"
#include "SCTBoardShape.h"
#include "SCTDoorGroup.h"
#include "SCTSlidingDrawerShape.h"
#include "Data/WHCModeData/XRWHCModeData.h"
#include "SCTResManager.h"
#include "MaterialManager.h"
#include "Materials/MaterialInterface.h"
#include "UtilityTools/SCTShapeUtilityTools.h"

namespace XRShapeUtilities
{
    void IterateCabinetRecursively(FSpaceShape *InSpaceShape, const FOnIterateCabinet & OnIterateCabinet)
    {
        if (OnIterateCabinet.IsBound())
        {
            if (OnIterateCabinet.Execute(InSpaceShape))
            {
                const TArray<TSharedPtr<FSpaceShape>> & ChildSpaces = InSpaceShape->GetChildSpaceShapes();
                if (ChildSpaces.Num() > 0)
                {
                    for (const auto & ChildSpace : ChildSpaces)
                    {
                        IterateCabinetRecursively(ChildSpace.Get(), OnIterateCabinet);
                    }
                }
                else if (InSpaceShape->GetInsideSpace().IsValid())
                {
                    const TArray<TSharedPtr<FSpaceShape>> & ChildInsideSpaces = InSpaceShape->GetInsideSpace()->GetChildSpaceShapes();
                    for (const auto & ChildInsideSpace : ChildInsideSpaces)
                    {
                        IterateCabinetRecursively(ChildInsideSpace.Get(), OnIterateCabinet);
                    }
                }
            }
        }
    }

    void IterateCabinet(FCabinetShape *InCabShape, FOnIterateCabinet OnIterateCabinet)
    {
        const TSharedPtr<FSpaceShape> &TopSpaceShape = InCabShape->GetTopSpaceShape();
        if (TopSpaceShape.IsValid())
            IterateCabinetRecursively(TopSpaceShape.Get(), OnIterateCabinet);
    }

    TSharedPtr<FBoardShape> GetCabinetFrameBoard(FCabinetShape *InCabShape, int32 Type)
    {
        TSharedPtr<FBoardShape> RetBoard;
        IterateCabinet(InCabShape, 
            FOnIterateCabinet::CreateLambda(
                [&RetBoard, Type](FSpaceShape * InSpaceShape) -> bool {
                    RetBoard = InSpaceShape->GetFrameBoard(Type);
                    return RetBoard.IsValid() ? false : true;
                }
            )
        );
        return RetBoard;
    }

    void GetCabinetDoors(FCabinetShape *InCabShape, TArray<FDoorGroup*> &OutDoorGroups, int32 Flag)
    {
        OutDoorGroups.Empty();
        IterateCabinet(InCabShape, 
            FOnIterateCabinet::CreateLambda(
                [&OutDoorGroups, &Flag](FSpaceShape * InSpaceShape) -> bool {
                    TSharedPtr<FDoorGroup> pDoorGroup = InSpaceShape->GetDoorShape();
                    if (!pDoorGroup.IsValid())
                    {
                        if (InSpaceShape->GetInsideSpace().IsValid())
                            pDoorGroup = InSpaceShape->GetInsideSpace()->GetDoorShape();
                    }
                    if (pDoorGroup.IsValid())
                    {
                        if ((Flag & FLAG_SIDEHUNGDOORS) != 0 && pDoorGroup->GetShapeType() == ST_SideHungDoor)
                            OutDoorGroups.Emplace(pDoorGroup.Get());
                        else if ((Flag & FLAG_SLIDINGDOORS) != 0 && pDoorGroup->GetShapeType() == ST_SlidingDoor)
                            OutDoorGroups.Emplace(pDoorGroup.Get());
                    }

                    // 继续向下递归
                    return true;
                }
            )
        );
    }

    void GetCabinetDrawerDoors(FCabinetShape *InCabShape, TArray<FDoorGroup*> &OutDrawerDoors)
    {
        OutDrawerDoors.Empty();
        IterateCabinet(InCabShape,
            FOnIterateCabinet::CreateLambda(
                [&OutDrawerDoors](FSpaceShape * InSpaceShape) -> bool {
                    TSharedPtr<FDrawerGroupShape> pDrawerGroup = InSpaceShape->GetCoveredDrawerGroup();
                    if (!pDrawerGroup.IsValid())
                    {
                        if (InSpaceShape->GetInsideSpace().IsValid())
                            pDrawerGroup = InSpaceShape->GetInsideSpace()->GetCoveredDrawerGroup();
                    }
                    if (pDrawerGroup.IsValid())
                    {
                        TSharedPtr<FSlidingDrawerShape> PrimitiveDrawer = pDrawerGroup->GetPrimitiveDrawer();
                        const TArray<TSharedPtr<FDrawerDoorShape>> & PrimitiveDrawerDoors = PrimitiveDrawer->GetDrawerDoorShapes();
                        if (PrimitiveDrawerDoors.Num() > 0)
                            OutDrawerDoors.Emplace(PrimitiveDrawerDoors[0].Get());

                        TArray<TSharedPtr<FSlidingDrawerShape>> CopiedDrawers = pDrawerGroup->GetCopyDrawerShapes();
                        for (const auto & CopiedDrawer : CopiedDrawers)
                        {
                            const TArray<TSharedPtr<FDrawerDoorShape>> & DrawerDoors = CopiedDrawer->GetDrawerDoorShapes();
                            if (DrawerDoors.Num() > 0)
                                OutDrawerDoors.Emplace(DrawerDoors[0].Get());
                        }
                    }

                    // 继续向下递归
                    return true;
                }
            )
        );
    }

    TSharedPtr<FCommonPakData> CreateMtlPakData(const FMtlInfo * InMtlInfo)
    {
        TSharedPtr<FCommonPakData> PakData = MakeShareable(new FCommonPakData);
        PakData->ID = InMtlInfo->Id;
        PakData->Name = InMtlInfo->MtlName;
        PakData->Url = InMtlInfo->MtlUrl;
        PakData->MD5 = InMtlInfo->MtlMd5;
        PakData->ThumbnailUrl = InMtlInfo->MtlThumbnailUrl;
        PakData->OptimizeParam = InMtlInfo->MtlParam;
        return MoveTemp(PakData);
    }

    UMaterialInterface* LoadMtl(FMtlInfo * InMtlInfo)
    {
        InMtlInfo->Mtl = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(
			FSCTShapeUtilityTool::GetFullCachePathByMaterialTypeAndID(EMaterialType::Board_Material, InMtlInfo->Id), 
			InMtlInfo->MtlParam);
		if (InMtlInfo->Mtl != nullptr)
        {
			InMtlInfo->Mtl->AddToRoot();
            return InMtlInfo->Mtl;
        }
        else
        {
            return nullptr;
        }
    }
}