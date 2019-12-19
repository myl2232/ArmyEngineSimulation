/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRActorConstant.h
 * @Description Actor的标签和物体列表目录管理
 *
 * @Author 欧石楠
 * @Date 2018年12月7日
 * @Version 1.0
 */

#pragma once

namespace XRActorTag
{
    static const FName MoveableMeshActor = "MoveableMeshActor";
    static const FName BlurprintActor = "BlueprintVRSActor";
    static const FName HighlightActor = "AreaHighLight";
    static const FName IsSale = "IsSale";
    static const FName Immovable = "Immovable";
    static const FName CanNotDelete = "CanNotDelete";
    static const FName OriginalWall = "OriginalWall";
    static const FName LampSlot = "LampSlot";
    static const FName OriginalPoint = "OriginalPoint";
    static const FName HydropowerPipeActor = "HydropowerPipeActor";
    static const FName OuterWall = "OuterWall";
    static const FName EnvironmentAsset = "EnvironmentAsset";
    static const FName Door = "Door";
    static const FName SecurityDoor = "SecurityDoor";
    static const FName Window = "Window";
	static const FName Pass = "Pass";
    static const FName WHCActor = "WHCActor";
    static const FName WHCTable = "WHCTable";
    static const FName WHCFloor = "WHCFloor";
    static const FName WHCHang = "WHCHang";
	static const FName IgnoreCollision = "IGNORECOLLISION";
	static const FName TranslucentRoof = "TranslucentRoof";
	static const FName HelpArea = "HelpArea";
	static const FName AutoDesignPoint = "AutoDesignPoint";
	//@郭子阳  设置actor全局不可选择,因为是actor的tag,对原始户型和拆改模式无效
	static const FName UnSelectable = "Static_UnSelectable";
    //@zengy 木作模式始终不显示对象的标签
    static const FName WHCAlwaysHidden = "WHCAlwaysHidden";
}

class FArmyActorPath
{
public:
    static const FName GetSoftPath() { return FName(TEXT("软装设计")); }
    static const FName GetLightPath() { return FName(TEXT("软装设计/灯具/固定光源")); }
    static const FName GetHardPath() { return FName(TEXT("硬装设计")); }
    static const FName GetWallPath() { return FName(TEXT("硬装设计/墙面")); }
    static const FName GetRoofPath() { return FName(TEXT("硬装设计/顶面")); }
    static const FName GetFloorPath() { return FName(TEXT("硬装设计/地面")); }
    static const FName GetHydropowerPath() { return FName(TEXT("水电设计")); }
    static const FName GetOriginalPointPath() { return FName(TEXT("水电设计/原始点位")); }
    static const FName GetPipelinePath() { return FName(TEXT("水电设计/管线")); }
    static const FName GetSwitchPath() { return FName(TEXT("水电设计/开关")); }
    static const FName GetWHCPath() { return FName(TEXT("木作设计")); }
    static const FName GetWHCCabinetPath() { return FName(TEXT("木作设计/橱柜")); }
    static const FName GetWHCShapeFramePath() { return FName(TEXT("木作设计/橱柜/柜体")); }
    static const FName GetWHCTablePath() { return FName(TEXT("木作设计/橱柜/台面")); }
    static const FName GetWHCHangPath() { return FName(TEXT("木作设计/橱柜/上线条")); }
    static const FName GetWHCFloorPath() { return FName(TEXT("木作设计/橱柜/踢脚板")); }
    static const FName GetLightSourcePath() { return FName(TEXT("辅助光源")); }
};

struct FArmyLabel
{
    FArmyLabel(FString InName)
        : Name(InName)
        , Index(1)
    {}

    FString Name;
    int32 Index;

    const FString MakeLabel() { return FString::Printf(TEXT("%s%d"), *Name, Index++); }

    void ResetIndex() { Index = 1; }
};

static FArmyLabel WallLabel = FArmyLabel(TEXT("墙面"));
static FArmyLabel RoofLabel = FArmyLabel(TEXT("顶面"));
static FArmyLabel FloorLabel = FArmyLabel(TEXT("地面"));
static FArmyLabel PointlightLabel = FArmyLabel(TEXT("点光源"));
static FArmyLabel SpotlightLabel = FArmyLabel(TEXT("聚光灯"));
static FArmyLabel ReflectionCaptureLabel = FArmyLabel(TEXT("反射球"));
static FArmyLabel LampSlotLabel = FArmyLabel(TEXT("灯槽"));
static FArmyLabel LampLabel = FArmyLabel(TEXT("灯带"));

class FArmyActorLabel
{
public:
    static const FString MakeWallLabel() { return WallLabel.MakeLabel(); }
    static const FString MakeRoofLabel() { return RoofLabel.MakeLabel(); }
    static const FString MakeFloorLabel() { return FloorLabel.MakeLabel(); }
    static const FString MakePointlightLabel() { return PointlightLabel.MakeLabel(); }
    static const FString MakeSpotlightLabel() { return SpotlightLabel.MakeLabel(); }
    static const FString MakeReflectionCaptureLabel() { return ReflectionCaptureLabel.MakeLabel(); }
    static const FString MakeLampSlotLabel() { return LampSlotLabel.MakeLabel(); }
    static const FString MakeLampLabel() { return LampLabel.MakeLabel(); }

    static void ResetIndexes()
    {
        WallLabel.ResetIndex();
        RoofLabel.ResetIndex();
        FloorLabel.ResetIndex();
        PointlightLabel.ResetIndex();
        SpotlightLabel.ResetIndex();
        ReflectionCaptureLabel.ResetIndex();
        LampSlotLabel.ResetIndex();
        LampLabel.ResetIndex();
    }
};