#include "ArmyLayingPoint.h"
#include "SImage.h"
#include "ArmyStyle.h"

FArmyLayingPoint::FArmyLayingPoint()
{
    ObjectType = OT_LayingPoint;
    SetPropertyFlag(FLAG_POINTOBJ, true);

    GVC->ViewportOverlayWidget->AddSlot()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        [
            SAssignNew(Img_Point, SImage)
            .Image(FArmyStyle::Get().GetBrush("Icon.LayingPoint.Normal"))
            .Visibility(EVisibility::Collapsed)
        ];

    Area = nullptr;

    EditPoint = MakeShareable(new FArmyEditPoint(FVector::ZeroVector));
    EditPoint->Size = 8.f;
    EditPoint->DepthPriority = 2;
}

void FArmyLayingPoint::Destroy()
{
    if (GVC && GVC->ViewportOverlayWidget.IsValid())
    {
        GVC->ViewportOverlayWidget->RemoveSlot(Img_Point.ToSharedRef());
    }
}

void FArmyLayingPoint::Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
    if (Img_Point.IsValid())
    {
        FVector2D ImgPos = FVector2D::ZeroVector;
        GVC->WorldToPixel(FromBasePlane(GetPos()), ImgPos);
        FVector2D ViewportSize;
        GVC->GetViewportSize(ViewportSize);
        ImgPos -= ViewportSize / 2.f;
        Img_Point->SetRenderTransform(FSlateRenderTransform(ImgPos));
    }
}

const FBox FArmyLayingPoint::GetBounds()
{
    return EditPoint->GetBounds();
}

void FArmyLayingPoint::SetState(EObjectState InState)
{
    FArmyObject::SetState(InState);

    switch (InState)
    {
    case OS_Normal:
        Img_Point->SetImage(FArmyStyle::Get().GetBrush("Icon.LayingPoint.Normal"));
        break;

    case OS_Hovered:
        Img_Point->SetImage(FArmyStyle::Get().GetBrush("Icon.LayingPoint.Hovered"));
        break;

    case OS_Selected:
        Img_Point->SetImage(FArmyStyle::Get().GetBrush("Icon.LayingPoint.Selected"));
        break;

    default:
        break;
    }
}

bool FArmyLayingPoint::Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
    return Area.IsValid() ? EditPoint->Hover(ToBasePlane(Pos), InViewportClient) : false;
}

bool FArmyLayingPoint::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
    return Area.IsValid() ? EditPoint->IsSelected(ToBasePlane(Pos), InViewportClient) : false;
}

void FArmyLayingPoint::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    FArmyObject::SerializeToJson(JsonWriter);

    JsonWriter->WriteValue("pos", EditPoint->GetPos().ToString());

    SERIALIZEREGISTERCLASS(JsonWriter, FArmyLayingPoint)
}

void FArmyLayingPoint::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
    FArmyObject::Deserialization(InJsonData);

    FVector LayingPointPos = FVector::ZeroVector;
    FString PosStr;
    InJsonData->TryGetStringField("pos", PosStr);
    if (PosStr.Len() > 0)
    {
        LayingPointPos.InitFromString(PosStr);
    }
    SetPos(LayingPointPos);
}

void FArmyLayingPoint::Update()
{
    if (Area.IsValid() && Area->GetMatStyle()->HasGoodID() && !EditPoint->GetPos().Equals(Area->GetLayingPointPos()))
    {
        SetPos(Area->GetLayingPointPos());
    }
}

void FArmyLayingPoint::Show(bool bShow)
{
    EVisibility ImgVisibility = bShow ? EVisibility::HitTestInvisible : EVisibility::Collapsed;
    Img_Point->SetVisibility(ImgVisibility);
}

void FArmyLayingPoint::SetCaptured(bool bCaptured)
{
    if (bCaptured)
    {
        Img_Point->SetImage(FArmyStyle::Get().GetBrush("Icon.LayingPoint.Captured"));
    }
    else
    {
        SetState(GetState());
    }
}

void FArmyLayingPoint::SetArea(TSharedPtr<class FArmyBaseArea> InArea)
{
    if (InArea.IsValid() && InArea->GetMatStyle()->HasGoodID())
    {
        Area = InArea;
        SetPos(Area->GetLayingPointPos());
        Img_Point->SetVisibility(EVisibility::HitTestInvisible);
    }
    else
    {
        Img_Point->SetVisibility(EVisibility::Collapsed);
    }
}

void FArmyLayingPoint::SetPos(const FVector& InPos)
{
    EditPoint->SetPos(InPos);
    if (Area.IsValid())
    {
        Area->SetLayingPointPos(InPos);
    }
}

FVector FArmyLayingPoint::ToBasePlane(const FVector& InV)
{
    if (Area.IsValid())
    {
        float X = FVector::DotProduct((InV - Area->GetPlaneCenter()), Area->GetXDir().GetSafeNormal());
        float Y = FVector::DotProduct((InV - Area->GetPlaneCenter()), Area->GetYDir().GetSafeNormal());
        return FVector(X, Y, 0.f);
    }
    else
    {
        return FVector::ZeroVector;
    }
}

FVector FArmyLayingPoint::FromBasePlane(const FVector& InV)
{
    if (Area.IsValid())
    {
        return Area->GetPlaneCenter()
            + InV.X * Area->GetXDir().GetSafeNormal()
            + InV.Y * Area->GetYDir().GetSafeNormal()
            + InV.Z * Area->GetPlaneNormal().GetSafeNormal();
    }
    else
    {
        return FVector::ZeroVector;
    }
}
