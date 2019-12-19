#include "ArmyWHCModeFrontBoardCabModifier.h"
#include "ArmyWHCabinet.h"
#include "ArmyShapeFrameActor.h"
#include "SCTShape.h"
#include "Actor/XRActorConstant.h"
#include "Actor/SCTShapeActor.h"
#include "UtilityTools/SCTShapeUtilityTools.h"
#include "SCTAnimation.h"
#include "ArmyViewportClient.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"

constexpr int32 ModifyThreshold = 150;
constexpr float ModifyAdding = 50.0f;

void FArmyWHCFrontBoardCabModifier::AddFrontBoardCab(FShapeInRoom *InCabinet)
{
    check(IsFrontBoardCab(InCabinet));
    FrontBoardCabs.Emplace(InCabinet);
}

TArray<FFrontBoardCabInfo> FArmyWHCFrontBoardCabModifier::AddCabinet(FShapeInRoom *InCabinet)
{
	if (IsFrontBoardCab(InCabinet))
	{
		// 放置的新柜子是一个插脚柜
        FrontBoardCabs.Emplace(InCabinet);
	}
    // 依次处理每一个插脚柜
    // 即使放置的是一个插脚柜，但此插脚柜可能会阻挡其它的插脚柜，所以遍历所有的插脚柜重新计算
    int32 NumDoors;
    float StartPoint, EndPoint;
    TArray<FFrontBoardCabInfo> ResultArray;
    for (auto &FrontBoardCab : FrontBoardCabs)
    {
        if (ProcessFrontBoardCab(FrontBoardCab, NumDoors, StartPoint, EndPoint))
            ResultArray.Emplace(FrontBoardCab, NumDoors, StartPoint, EndPoint);
    }
    return MoveTemp(ResultArray);
}

TArray<FFrontBoardCabInfo> FArmyWHCFrontBoardCabModifier::MoveCabinet()
{
    // 即使放置的是一个插脚柜，但此插脚柜可能会阻挡其它的插脚柜，所以遍历所有的插脚柜重新计算
    int32 NumDoors;
    float StartPoint, EndPoint;
    TArray<FFrontBoardCabInfo> ResultArray;
    for (auto &FrontBoardCab : FrontBoardCabs)
    {
        if (ProcessFrontBoardCab(FrontBoardCab, NumDoors, StartPoint, EndPoint))
            ResultArray.Emplace(FrontBoardCab, NumDoors, StartPoint, EndPoint);
    }
    return MoveTemp(ResultArray);
}

void FArmyWHCFrontBoardCabModifier::DeleteCabinet(FShapeInRoom *InCabinet)
{
    int32 Index = FrontBoardCabs.IndexOfByPredicate([InCabinet](FShapeInRoom *Data) -> bool { return Data == InCabinet; });
    if (Index != INDEX_NONE)
        FrontBoardCabs.RemoveAt(Index);
}

void FArmyWHCFrontBoardCabModifier::ClearFrontBoardCabs()
{
    FrontBoardCabs.Empty();
}

bool FArmyWHCFrontBoardCabModifier::IsFrontBoardCab(FShapeInRoom *InCabinet) const
{
    bool bOutSingleDoor = false, bOutDoubleDoor = false, bOutCurIsSingleDoor = false, bOutCurIsLeft = false;
	float OutDoorSheetWidth = 0.0f;
    return FSCTShapeUtilityTool::GetFrontBoardCabinetDoorModifyInfo(InCabinet->Shape.Get(), bOutSingleDoor, bOutDoubleDoor, OutDoorSheetWidth, bOutCurIsSingleDoor, bOutCurIsLeft);
}

bool FArmyWHCFrontBoardCabModifier::ProcessFrontBoardCab(FShapeInRoom *InFrontBoardCab, int32 &OutNumDoors, float &OutStartPoint, float &OutEndPoint)
{
    FRotator CabRotation = InFrontBoardCab->ShapeFrame->GetActorRotation();
    FVector CabForward = CabRotation.RotateVector(FVector::ForwardVector);
    FVector CabRight = CabRotation.RotateVector(FVector::RightVector);
    int8 LeftOrRight = 0;
    AXRShapeFrame *HitOtherCab = nullptr;
    FVector HitPoint(EForceInit::ForceInitToZero);
    // 检测插脚柜是否和其它某个柜子相交
    if (CheckFrontBoardCabinetHitOnlyOneSide(InFrontBoardCab, CabForward, CabRight, LeftOrRight, HitOtherCab, HitPoint))
    {
        // 相交后检测插脚柜和相交到柜子的相对夹角
        FVector OtherCabRight = HitOtherCab->GetActorRotation().RotateVector(FVector::RightVector);
        float Angle = FMath::Acos(CabRight.DotProduct(CabRight, OtherCabRight));
        int32 iAngle = FMath::RoundToInt(Angle * 180.0f / PI);
        if (iAngle == 0 || iAngle == 180)
        {
            // 两个柜子是平行的
            return ModifyFrontBoardCab(InFrontBoardCab, HitOtherCab->ShapeInRoomRef, LeftOrRight, 0, HitPoint, -CabRight, OutNumDoors, OutStartPoint, OutEndPoint);
        }
        else if (iAngle == 90 || iAngle == -90)
        {
            // 两个柜子是垂直的
            return ModifyFrontBoardCab(InFrontBoardCab, HitOtherCab->ShapeInRoomRef, LeftOrRight, 1, HitPoint, -CabRight, OutNumDoors, OutStartPoint, OutEndPoint);
        }
    }
    return false;
}

bool FArmyWHCFrontBoardCabModifier::CheckFrontBoardCabinetHitOnlyOneSide(FShapeInRoom *InFrontBoardCab, const FVector &CabForward, const FVector &CabRight, int8 &OutLeftOrRight, AXRShapeFrame *&OutHitOtherCab, FVector &HitPoint) const
{
	FVector CabLocation = InFrontBoardCab->ShapeFrame->GetActorLocation();
    float CabHalfWidthInCentimeter = InFrontBoardCab->Shape->GetShapeWidth() * 0.05f;
	float CabDepthInCentimeter = InFrontBoardCab->Shape->GetShapeDepth() * 0.1f;
	float CabHalfHeightInCentimeter = InFrontBoardCab->Shape->GetShapeHeight() * 0.05f;
    FVector CabLeftTestLocation = CabLocation + CabRight * CabDepthInCentimeter - CabForward * (CabHalfWidthInCentimeter - 0.05f) + FVector::UpVector * CabHalfHeightInCentimeter;
    FVector CabRightTestLocation = CabLocation + CabRight * CabDepthInCentimeter + CabForward * (CabHalfWidthInCentimeter - 0.05f) + FVector::UpVector * CabHalfHeightInCentimeter;

    UWorld * RESTRICT World = GVC->GetWorld();

    FHitResult HitResultLeft, HitResultRight;

    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = false;
    QueryParams.AddIgnoredActor(InFrontBoardCab->ShapeFrame);
    QueryParams.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(World, 0));
    bool bLeftHitResult = World->SweepSingleByChannel(HitResultLeft, 
        CabLeftTestLocation, CabLeftTestLocation + CabRight * 10000.0f, InFrontBoardCab->ShapeFrame->GetActorRotation().Quaternion(),
        ECC_GameTraceChannel1, 
        FCollisionShape::MakeBox(FVector(0.04f, 0.04f, CabHalfHeightInCentimeter)),
        QueryParams);
    bool bRightHitResult = World->SweepSingleByChannel(HitResultRight, 
        CabRightTestLocation, CabRightTestLocation + CabRight * 10000.0f, InFrontBoardCab->ShapeFrame->GetActorRotation().Quaternion(),
        ECC_GameTraceChannel1, 
        FCollisionShape::MakeBox(FVector(0.04f, 0.04f, CabHalfHeightInCentimeter)),
        QueryParams);
    // 自动变化的前提是两个柜子的距离能够生成调整板
    bLeftHitResult = bLeftHitResult ? (FMath::RoundToInt(HitResultLeft.Distance * 10.0f) <= ModifyThreshold) : false;
    bRightHitResult = bRightHitResult ? (FMath::RoundToInt(HitResultRight.Distance * 10.0f) <= ModifyThreshold) : false;
    if (bLeftHitResult && bRightHitResult) // 左右都有相交不能自动修改插脚封板
        return false;
    else if (bLeftHitResult)
    {
        OutLeftOrRight = 1;
        AActor *HitActor = HitResultLeft.GetActor();
        check(HitActor && HitActor->IsA<AXRShapeFrame>());
        OutHitOtherCab = Cast<AXRShapeFrame>(HitActor);
        HitPoint = HitResultLeft.Location;
        return true;
    }
    else if (bRightHitResult)
    {
        OutLeftOrRight = 2;
        AActor *HitActor = HitResultRight.GetActor();
        check(HitActor && HitActor->IsA<AXRShapeFrame>());
        OutHitOtherCab = Cast<AXRShapeFrame>(HitActor);
        HitPoint = HitResultRight.Location;
        return true;
    }
    else
        return false;
}

bool FArmyWHCFrontBoardCabModifier::ReverseHit(FShapeInRoom *InFrontBoardCab, FShapeInRoom *InOtherCab, int8 nFlag, const FVector &InHitPoint, const FVector &InReverseDir, FVector &OutReverseHitPoint) const
{
    FVector OtherCabLocation = InOtherCab->ShapeFrame->GetActorLocation();
    OtherCabLocation.Z = InHitPoint.Z;
    FVector OtherCabForward = InOtherCab->ShapeFrame->GetActorRotation().RotateVector(FVector::ForwardVector);
    FVector OtherCabRight = InOtherCab->ShapeFrame->GetActorRotation().RotateVector(FVector::RightVector);
    float CabHalfWidthInCentimeter = InOtherCab->Shape->GetShapeWidth() * 0.05f;
    float CabDepthInCentimeter = InOtherCab->Shape->GetShapeDepth() * 0.1f;
	float CabHalfHeightInCentimeter = InOtherCab->Shape->GetShapeHeight() * 0.05f;

    UWorld * RESTRICT World = GVC->GetWorld();

    FHitResult HitResult1, HitResult2;

    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = false;
    QueryParams.AddIgnoredActor(InOtherCab->ShapeFrame);
    QueryParams.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(World, 0));


    FVector TestLocation1(EForceInit::ForceInitToZero), TestLocation2(EForceInit::ForceInitToZero);
    float Shrink = 0.0f;
    if (nFlag == 0)
    {
        TestLocation1 = OtherCabLocation - OtherCabForward * CabHalfWidthInCentimeter;
        TestLocation2 = OtherCabLocation + OtherCabForward * CabHalfWidthInCentimeter;
        Shrink = CabDepthInCentimeter * 0.5f;
    }
    else if (nFlag == 1)
    {
        TestLocation1 = OtherCabLocation;
        TestLocation2 = OtherCabLocation + OtherCabRight * CabDepthInCentimeter;
        Shrink = CabHalfWidthInCentimeter;
    }
    bool bHitResult1 = World->LineTraceSingleByChannel(HitResult1, TestLocation1, TestLocation1 + InReverseDir * 10000.0f, ECC_GameTraceChannel1, QueryParams);
    bool bHitResult2 = World->LineTraceSingleByChannel(HitResult2, TestLocation2, TestLocation2 + InReverseDir * 10000.0f, ECC_GameTraceChannel1, QueryParams);
    auto CheckHitResult = [this](bool bHit, FShapeInRoom *FrontBoardCab, const FHitResult &HitResult, float InModifyThreshold, float Shrink) -> bool {
        bool bRet = bHit ? (HitResult.GetActor() == FrontBoardCab->ShapeFrame && FMath::RoundToInt((HitResult.Distance - Shrink) * 10.0f) <= InModifyThreshold) : false;
        if (bRet)
        {
            FVector FrontLowerLeft = GetCabFrontLowerLeft(FrontBoardCab) * 10.0f;
            FVector FrontLowerRight = GetCabFrontLowerRight(FrontBoardCab) * 10.0f;
            FVector HitLocation = HitResult.Location * 10.0f;
            bRet = (FMath::RoundToInt(FrontLowerLeft.X) == FMath::RoundToInt(HitLocation.X) && FMath::RoundToInt(FrontLowerLeft.Y) == FMath::RoundToInt(HitLocation.Y)) ||
                    (FMath::RoundToInt(FrontLowerRight.X) == FMath::RoundToInt(HitLocation.X) && FMath::RoundToInt(FrontLowerRight.Y) == FMath::RoundToInt(HitLocation.Y));
            return !bRet;
        }
        else
            return false;
    };
    bHitResult1 = CheckHitResult(bHitResult1, InFrontBoardCab, HitResult1, ModifyThreshold, Shrink);
    bHitResult2 = CheckHitResult(bHitResult2, InFrontBoardCab, HitResult2, ModifyThreshold, Shrink);
    if (bHitResult1 && bHitResult2)
        return false;
    else if (bHitResult1)
    {
        OutReverseHitPoint = HitResult1.Location;
        return true;
    }
    else if (bHitResult2)
    {
        OutReverseHitPoint = HitResult2.Location;
        return true;
    }
    else
        return false;
}

bool FArmyWHCFrontBoardCabModifier::ModifyFrontBoardCab(FShapeInRoom *InFrontBoardCab, FShapeInRoom *InOtherCab, int8 InLeftOrRight, int8 nFlag, const FVector &InHitPoint, const FVector &InReverseHitDir, int32 &OutNumDoors, float &OutStartPoint, float &OutEndPoint)
{
    OutNumDoors = 0;
    OutStartPoint = OutEndPoint = 0.0f;

    bool bRet = false;
    FVector ReverseHitPoint;
    if (ReverseHit(InFrontBoardCab, InOtherCab, nFlag, InHitPoint, InReverseHitDir, ReverseHitPoint))
    {
        FVector FrontLowerLeft = GetCabFrontLowerLeft(InFrontBoardCab);
        ReverseHitPoint.Z = FrontLowerLeft.Z;
        if (InLeftOrRight == 1)
        {
            OutEndPoint = (ReverseHitPoint - FrontLowerLeft).Size() * 10.0f + ModifyAdding;
        }
        else if (InLeftOrRight == 2)
        {
            OutStartPoint = (ReverseHitPoint - FrontLowerLeft).Size() * 10.0f - ModifyAdding;
            OutEndPoint = InFrontBoardCab->Shape->GetShapeWidth();
        }

        for (auto &Animation : InFrontBoardCab->Animations)
            Animation->Stop();
        bRet = FSCTShapeUtilityTool::ModifyCabinetFrontBoardSize(InFrontBoardCab->Shape.Get(), OutStartPoint, OutEndPoint);
        if (bRet)
        {
            InFrontBoardCab->PrepareShapeAnimations();
            InFrontBoardCab->MakeImmovable();
            FSCTShapeUtilityTool::SetActorTag(InFrontBoardCab->Shape->GetShapeActor(), XRActorTag::WHCActor);

            bool bOutSingleDoor = false, bOutDoubleDoor = false, OutCurIsSingleDoor = false, bOutCurIsLeft = false;
            float OutDoorSheetsWidth = 0.0f;
            FSCTShapeUtilityTool::GetFrontBoardCabinetDoorModifyInfo(InFrontBoardCab->Shape.Get(), bOutSingleDoor, bOutDoubleDoor, OutDoorSheetsWidth, OutCurIsSingleDoor, bOutCurIsLeft);
            OutNumDoors = OutCurIsSingleDoor ? 1 : 2;
        }
    }
    return bRet;
}

FVector FArmyWHCFrontBoardCabModifier::GetCabFrontLowerLeft(FShapeInRoom *InCab) const
{
    FVector Location = InCab->ShapeFrame->GetActorLocation();
    float CabHalfWidthInCentimeter = InCab->Shape->GetShapeWidth() * 0.05f;
    float CabDepthInCentimeter = InCab->Shape->GetShapeDepth() * 0.1f;
    FVector CabForward = InCab->ShapeFrame->GetActorRotation().RotateVector(FVector::ForwardVector);
    FVector CabRight = InCab->ShapeFrame->GetActorRotation().RotateVector(FVector::RightVector);
    return Location + CabRight * CabDepthInCentimeter - CabForward * CabHalfWidthInCentimeter;
}

FVector FArmyWHCFrontBoardCabModifier::GetCabFrontLowerRight(FShapeInRoom *InCab) const
{
    FVector Location = InCab->ShapeFrame->GetActorLocation();
    float CabHalfWidthInCentimeter = InCab->Shape->GetShapeWidth() * 0.05f;
    float CabDepthInCentimeter = InCab->Shape->GetShapeDepth() * 0.1f;
    FVector CabForward = InCab->ShapeFrame->GetActorRotation().RotateVector(FVector::ForwardVector);
    FVector CabRight = InCab->ShapeFrame->GetActorRotation().RotateVector(FVector::RightVector);
    return Location + CabRight * CabDepthInCentimeter + CabForward * CabHalfWidthInCentimeter;
}