#include "ArmyDetailbaseSlate.h"
#include "Engine/StaticMeshActor.h"
#include "PhysicsEngine/AggregateGeom.h"
#include "PhysicsEngine/BodySetup.h"
#include "Animation/SkeletalMeshActor.h"


FArmyDetailBase::FArmyDetailBase()
	:SelectedUObject(nullptr)
{

}

FArmyDetailBase::~FArmyDetailBase()
{
}

TSharedRef<SWidget> FArmyDetailBase::CreateDetailWidget()
{
	DetailBuilder = MakeShareable(new FArmyDetailBuilder);
	DetailWidget = DetailBuilder->BuildDetail();

	return DetailWidget->AsShared();
}

void FArmyDetailBase::ShowSelectedDetial(TSharedPtr<FArmyObject> InSelectedObject)
{
}

void FArmyDetailBase::ShowSelectedDetial(TSharedPtr<FArmyObject> InSelectedObject, bool bHiddenSelected)
{
	ShowSelectedDetial(InSelectedObject);
}

void FArmyDetailBase::ShowSelectedDetial(UObject * InSelectedObject)
{
}

void FArmyDetailBase::RefreshSelectedDetial()
{
	ShowSelectedDetial(SelectedArmyObject);
	ShowSelectedDetial(SelectedUObject);
}

TSharedRef<SWidget> FArmyDetailBase::GetDetailWidget()
{
	return DetailWidget->AsShared();
}

void FArmyDetailBase::HiddenDetailWidget(EVisibility State)
{
	DetailWidget->SetVisibility(State);
}

FVector FArmyDetailBase::CalcActorSize(AActor * InActor)
{
	if (InActor->ActorHasTag(FName("MoveableMeshActor")) || InActor->ActorHasTag(FName("BlueprintVRSActor"))) {
		AStaticMeshActor* SMA = Cast<AStaticMeshActor>(InActor);
		UStaticMeshComponent* SKeletalMA = Cast<UStaticMeshComponent>(InActor->GetRootComponent());
		
		if (SMA && SMA->GetStaticMeshComponent()->GetBodySetup())
		{
			FKAggregateGeom& agg = SMA->GetStaticMeshComponent()->GetBodySetup()->AggGeom;
			FBox AggGeomBox = agg.CalcAABB(FTransform(FVector(0, 0, 0)));
			return AggGeomBox.GetSize()*InActor->GetActorScale3D();
		}

		if (SKeletalMA && SKeletalMA->GetStaticMesh()->BodySetup)
		{
			FBox AggGeomBox = SKeletalMA->GetStaticMesh()->BodySetup->AggGeom.CalcAABB(FTransform(FVector(0, 0, 0)));
			return AggGeomBox.GetSize()*InActor->GetActorScale3D();
		}
	}
	return FVector(0, 0, 0);
}

FVector FArmyDetailBase::CalcActorDefaultSize(AActor * InActor)
{
	if (InActor->ActorHasTag(FName("MoveableMeshActor"))) {
		AStaticMeshActor* SMA = Cast<AStaticMeshActor>(InActor);
		if (SMA)
		{
			FKAggregateGeom& agg = SMA->GetStaticMeshComponent()->GetBodySetup()->AggGeom;
			FBox AggGeomBox = agg.CalcAABB(FTransform(FVector(0, 0, 0)));
			return AggGeomBox.GetSize();
		}
	}
	return FVector(0, 0, 0);
}


void FArmyDetailBase::AddToolTipForWidget(TSharedRef<SWidget> InWidget, const TAttribute<FText> &InToolTipText)
{
	InWidget->SetToolTip(
		SNew(SToolTip)
			.Text(InToolTipText)
			.Font(FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12").Font)
			.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
			.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
	);
}