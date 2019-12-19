/**
 * Copyright 2019 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRLayingPoint.h
 * @Description 起铺点
 *
 * @Author 欧石楠
 * @Date 2019年4月23日
 * @Version 1.0
 */

#pragma once

#include "ArmyObject.h"

class ARMYSCENEDATA_API FArmyLayingPoint : public FArmyObject
{
public:
    FArmyLayingPoint();

    virtual void Destroy() override;
    virtual void Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
    virtual const FBox GetBounds() override;
    virtual void SetState(EObjectState InState) override;
    virtual bool Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
    virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
    virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
    virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;

    /** @欧石楠 更新起铺点位置 */
    void Update();

    void Show(bool bShow);

    void SetCaptured(bool bCaptured);

    void SetArea(TSharedPtr<class FArmyBaseArea> InArea);

    void SetPos(const FVector& InPos);

    FVector GetPos() const { return EditPoint->GetPos(); }
    bool IsShown() const { return Img_Point.IsValid() && Img_Point->GetVisibility() == EVisibility::HitTestInvisible; }

private:
    FVector ToBasePlane(const FVector& InV);
    FVector FromBasePlane(const FVector& InV);

private:
    TSharedPtr<class SImage> Img_Point;

    TSharedPtr<FArmyBaseArea> Area;

    TSharedPtr<FArmyEditPoint> EditPoint;
};

REGISTERCLASS(FArmyLayingPoint)