/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRActorVisitor.h
 * @Description 场景中各种Actor的访问管理
 *
 * @Author 欧石楠
 * @Date 2018年12月7日
 * @Version 1.0
 */

#pragma once

#include "ArmyActorConstant.h"
#include "SArmyModelContentBrowser.h"
#include "ArmyGameInstance.h"
#include "ArmyLightActor.h"
#include "ArmyReflectionCaptureActor.h"

/**
 * 原始点位
 */
class FArmyOriginalPointVisitor : public IArmyActorVisitor
{
public:
    virtual bool IsActorTagsMatched(AActor* Actor) override
    {
        return Actor->Tags.Contains(XRActorTag::OriginalPoint);
    }
};

/**
* 外墙
*/
class FArmyOuterWallVisitor : public IArmyActorVisitor
{
public:
	virtual bool IsActorTagsMatched(class AActor* Actor) override
	{
		return Actor->ActorHasTag(XRActorTag::OuterWall);
	}
};

/**
* 原始墙面
*/
class FArmyOriginalWallVisitor : public IArmyActorVisitor
{
public:
	virtual bool IsActorTagsMatched(class AActor* Actor) override
	{
		return Actor->ActorHasTag(XRActorTag::OriginalWall);
	}
};

/**
* 木作物体
*/
class FArmyWHCActorVisitor : public IArmyActorVisitor
{
public:
	virtual bool IsActorTagsMatched(class AActor* Actor) override
	{
		return Actor->ActorHasTag(XRActorTag::WHCActor) && (!Actor->ActorHasTag(XRActorTag::WHCAlwaysHidden));
	}
};

/**
* 半透明吊顶
*/
class FArmyTranslucentRoofActorVisitor : public IArmyActorVisitor
{
public:
	virtual bool IsActorTagsMatched(class AActor* Actor) override
	{
		return Actor->ActorHasTag(XRActorTag::TranslucentRoof);
	}
};

/** 
 * 门窗   
 */
class FArmyHardwareActorVisitor : public IArmyActorVisitor
{
public:
    virtual bool IsActorTagsMatched(class AActor* Actor) override
    {
        return Actor->ActorHasTag(XRActorTag::Door) ||
            Actor->ActorHasTag(XRActorTag::SecurityDoor) ||
            Actor->ActorHasTag(XRActorTag::Window);
    }
};

/**
 * 光源图标
 */
class FArmyLightIconVisitor : public IArmyActorVisitor
{
public:
    virtual bool IsActorTagsMatched(class AActor* Actor) override
    {
        return Actor->IsA(AXRLightActor::StaticClass()) ||
            Actor->IsA(AXRReflectionCaptureActor::StaticClass());
    }

    virtual void Show(bool bShow) override
    {
        for (auto It : Actors)
        {
            if (It->IsA(AXRLightActor::StaticClass()))
            {
                AXRLightActor* LightActor = Cast<AXRLightActor>(It);
                LightActor->ShowLightIcon(bShow);
            }
            else if (It->IsA(AXRReflectionCaptureActor::StaticClass()))
            {
                AXRReflectionCaptureActor* ReflectionCaptureActor = Cast<AXRReflectionCaptureActor>(It);
                ReflectionCaptureActor->ShowRCIcon(bShow);
            }
        }
    }
};

/**
 * 立面模式控制
 */
class FArmyHardModeActorVisitor : public IArmyActorVisitor
{
public:
    virtual bool IsActorTagsMatched(class AActor* Actor) override
    {
        return false;
    }

    virtual void Show(bool bShow) override
    {
        GGI->DesignEditor->WorldOutliner->Refresh(true);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetSoftPath(), false, bShow, bShow);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetHardPath(), false, bShow, bShow);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetHydropowerPath(), false, bShow, false);
       
		//@郭子阳 在立面模式和木作模式隐藏所有停用的原始点位，原始点位在E_LayoutModel
		if (bShow)
		{
			TArray<FObjectWeakPtr> ObjArray;
			FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_ComponentBase, ObjArray);
			for (auto ObjPtr : ObjArray)
			{
				FArmyFurniture* FurnitureObj = ObjPtr.Pin()->AsassignObj<FArmyFurniture>();
				if (FurnitureObj &&FurnitureObj->GetPropertyFlag(FArmyObject::PropertyType::FLAG_STOPPED))
				{
					if (FurnitureObj->GetRelevanceActor())
					{
					FurnitureObj->GetRelevanceActor()->SetActorHiddenInGame(true);
					}
				}
			}
		}
		
		GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetPipelinePath(), false, false, false);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetLightSourcePath(), false, bShow, true);

        TSharedPtr<IArmyActorVisitor> WHCVisitor = FArmyActorVisitorFactory::Get().CreateVisitor(AV_WHCActor);
        WHCVisitor->Show(bShow);
		WHCVisitor->Lock(true);
		TSharedPtr<IArmyActorVisitor> TranslucentRoofVisitor = FArmyActorVisitorFactory::Get().CreateVisitor(AV_TranslucentRoofActor);
		TranslucentRoofVisitor->Show(false);
		TranslucentRoofVisitor->Lock(true);
        TSharedPtr<IArmyActorVisitor> LightIconAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_LightIcon);
        LightIconAV->Show(false);

        TSharedPtr<IArmyActorVisitor> HardwareActorAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_HardwareActor);
        HardwareActorAV->Lock(!bShow);
    }
};

/**
 * 立面子模式控制
 */
class FArmyHardSubModeActorVisitor : public IArmyActorVisitor
{
public:
    virtual bool IsActorTagsMatched(class AActor* Actor) override
    {
        return Actor->ActorHasTag(XRActorTag::Door) ||
            Actor->ActorHasTag(XRActorTag::Pass) ||
            Actor->ActorHasTag(XRActorTag::Window) ||
            Actor->ActorHasTag(XRActorTag::OriginalWall) ||
            Actor->ActorHasTag(XRActorTag::OuterWall) ||
            Actor->ActorHasTag(XRActorTag::SecurityDoor) ||
            Actor->ActorHasTag(XRActorTag::WHCActor) ||
            Actor->ActorHasTag(XRActorTag::HelpArea);
    }

    virtual void Show(bool bShow) override
    {
        GGI->DesignEditor->WorldOutliner->Refresh(true);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetHardPath(), false, !bShow, true);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetSoftPath(), false, !bShow, true);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetHydropowerPath(), false, false);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetLightSourcePath(), false, !bShow, false);

        IArmyActorVisitor::Show(!bShow);

        TSharedPtr<IArmyActorVisitor> OuterRoomVisitor = FArmyActorVisitorFactory::Get().CreateVisitor(AV_OuterWall);
        OuterRoomVisitor->Lock(true);
		TSharedPtr<IArmyActorVisitor> TranslucentRoofVisitor = FArmyActorVisitorFactory::Get().CreateVisitor(AV_TranslucentRoofActor);
		TranslucentRoofVisitor->Show(false);
		TranslucentRoofVisitor->Lock(true);
        TSharedPtr<IArmyActorVisitor> LightIconAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_LightIcon);
        LightIconAV->Show(false);
    }
};

/**
 * 水电模式控制
 */
class FArmyHydropowerModeActorVisitor : public IArmyActorVisitor
{
public:
    virtual bool IsActorTagsMatched(class AActor* Actor) override
    {
        return Actor->ActorHasTag(XRActorTag::HelpArea);
    }


    virtual void Show(bool bShow) override
    {
        GGI->DesignEditor->WorldOutliner->Refresh(true);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetSoftPath(), false, false, false);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetLightPath(), false, bShow, bShow);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetHardPath(), false, false, false);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetHydropowerPath(), false, bShow, bShow);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetOriginalPointPath(), false, bShow, bShow);
		GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetLightSourcePath(), false, bShow, true);

        IArmyActorVisitor::Show(!bShow);

        TSharedPtr<IArmyActorVisitor> WHCVisitor = FArmyActorVisitorFactory::Get().CreateVisitor(AV_WHCActor);
        WHCVisitor->Show(false);
		WHCVisitor->Lock(true);
		TSharedPtr<IArmyActorVisitor> TranslucentRoofVisitor = FArmyActorVisitorFactory::Get().CreateVisitor(AV_TranslucentRoofActor);
		TranslucentRoofVisitor->Show(false);
		TranslucentRoofVisitor->Lock(true);
        TSharedPtr<IArmyActorVisitor> LightIconAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_LightIcon);
        LightIconAV->Show(false);

        TSharedPtr<IArmyActorVisitor> HardwareActorAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_HardwareActor);
        HardwareActorAV->Lock(bShow);

    }
};

/**
 * 木作模式控制
 */
class FArmyWHCModeActorVisitor : public IArmyActorVisitor
{
public:
    virtual bool IsActorTagsMatched(class AActor* Actor) override
    {
        return Actor->ActorHasTag(XRActorTag::WHCActor) && (!Actor->ActorHasTag(XRActorTag::WHCAlwaysHidden));
    }

    virtual void Show(bool bShow) override
    {
        GGI->DesignEditor->WorldOutliner->Refresh(true);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetHardPath(), false, bShow, false);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetSoftPath(), false, bShow, false);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetHydropowerPath(), false, bShow, false);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetLightSourcePath(), false, bShow, false);

        IArmyActorVisitor::Show(bShow);
        IArmyActorVisitor::Lock(true);

		TSharedPtr<IArmyActorVisitor> TranslucentRoofVisitor = FArmyActorVisitorFactory::Get().CreateVisitor(AV_TranslucentRoofActor);
		TranslucentRoofVisitor->Show(false);
		TranslucentRoofVisitor->Lock(true);
        TSharedPtr<IArmyActorVisitor> LightIconAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_LightIcon);
        LightIconAV->Show(false);

        // 木作模式不让选中门窗
        TSharedPtr<IArmyActorVisitor> HardwareActorAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_HardwareActor);
        HardwareActorAV->Lock(bShow);


		//@郭子阳 在立面模式和木作模式隐藏所有停用的原始点位，原始点位在E_LayoutModel
		if (bShow)
		{
			TArray<FObjectWeakPtr> ObjArray;
			FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_ComponentBase, ObjArray);
			for (auto ObjPtr : ObjArray)
			{
				FArmyFurniture* FurnitureObj = ObjPtr.Pin()->AsassignObj<FArmyFurniture>();
				if (FurnitureObj &&FurnitureObj->GetPropertyFlag(FArmyObject::PropertyType::FLAG_STOPPED))
				{
					if (FurnitureObj->GetRelevanceActor())
					{
						FurnitureObj->GetRelevanceActor()->SetActorHiddenInGame(true);
					}
				}
			}
		}
    }
};

/**
 * 立面模式平面视图控制
 */
class FArmyTopHardModeActorVisitor : public IArmyActorVisitor
{
public:
    virtual bool IsActorTagsMatched(class AActor* Actor) override
    {
        return Actor->ActorHasTag(XRActorTag::OuterWall) ||
            Actor->ActorHasTag(XRActorTag::OriginalWall) ||
            Actor->ActorHasTag(XRActorTag::HelpArea);
    }

    virtual void Show(bool bShow) override
    {
        GGI->DesignEditor->WorldOutliner->Refresh(true);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetWallPath(), false, !bShow, true);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetRoofPath(), false, !bShow, true);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetLightPath(), false, !bShow, false);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetLightSourcePath(), false, !bShow, !bShow);
        IArmyActorVisitor::Show(!bShow);

		TSharedPtr<IArmyActorVisitor> TranslucentRoofVisitor = FArmyActorVisitorFactory::Get().CreateVisitor(AV_TranslucentRoofActor);
		TranslucentRoofVisitor->Show(false);
		TranslucentRoofVisitor->Lock(true);
        TSharedPtr<IArmyActorVisitor> LightIconAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_LightIcon);
        LightIconAV->Show(false);
    }
};

/**
 * 水电模式平面视图控制
 */
class FArmyTopHydropowerModeActorVisitor : public IArmyActorVisitor
{
public:
    virtual bool IsActorTagsMatched(class AActor* Actor) override
    {
        return Actor->ActorHasTag(XRActorTag::OuterWall);
    }

    virtual void Show(bool bShow) override
    {
        GGI->DesignEditor->WorldOutliner->Refresh(true);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetRoofPath(), false, !bShow, false);

        IArmyActorVisitor::Show(!bShow);

        // 显隐原始顶面
        TArray< TWeakPtr<FArmyObject> > OriginalRoofs;
        FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, OriginalRoofs);

        for (auto It : OriginalRoofs)
        {
            TSharedPtr<FArmyRoomSpaceArea> RoomSpaceArea = StaticCastSharedPtr<FArmyRoomSpaceArea>(It.Pin());
            if (RoomSpaceArea.IsValid() && RoomSpaceArea->SurfaceType == 2)
            {
                RoomSpaceArea->SetOriginalActorVisible(!bShow);
            }
        }
    }
};

/**
* 立面模式顶面视图控制
*/
class FArmyHardModeCeilingModeActorVisitor : public IArmyActorVisitor
{
public:
	virtual bool IsActorTagsMatched(class AActor* Actor) override
	{
		return Actor->ActorHasTag(XRActorTag::OuterWall) ||
            Actor->ActorHasTag(XRActorTag::OriginalWall);
	}

	virtual void Show(bool bShow) override
	{
        GGI->DesignEditor->WorldOutliner->Refresh(true);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetWallPath(), false, !bShow, true);
		GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetRoofPath(), false, !bShow, true);
		GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetLightPath(), false, bShow, true);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetLightSourcePath(), false, bShow, bShow);
		IArmyActorVisitor::Show(!bShow);

		TSharedPtr<IArmyActorVisitor> TranslucentRoofVisitor = FArmyActorVisitorFactory::Get().CreateVisitor(AV_TranslucentRoofActor);
		TranslucentRoofVisitor->Show(true);
		TranslucentRoofVisitor->Lock(true);
        TSharedPtr<IArmyActorVisitor> LightIconAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_LightIcon);
        LightIconAV->Show(true);
	}
};

/**
 * 模型
 */
class FArmyModelActorVisitor : public IArmyActorVisitor
{
public:
    virtual bool IsActorTagsMatched(class AActor* Actor) override
    {
        return Actor->ActorHasTag(XRActorTag::BlurprintActor) ||
            Actor->ActorHasTag(XRActorTag::MoveableMeshActor);
    }

    virtual void Show(bool bShow) override
    {
        GGI->DesignEditor->WorldOutliner->Refresh(true);

        for (auto ObjGroup : SArmyModelContentBrowser::ObjectGroupCategoriesArray)
        {
            GGI->DesignEditor->WorldOutliner->CloseExpander(FName(*(ObjGroup->ModelName)), false, bShow, bShow);
        }

        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetSoftPath(), false, !bShow, !bShow);
        GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetHydropowerPath(), false, bShow, bShow);

        IArmyActorVisitor::Show(bShow);
    }
};