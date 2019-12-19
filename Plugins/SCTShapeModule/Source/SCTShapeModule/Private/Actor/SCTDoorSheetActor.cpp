
#include "Actor/SCTDoorSheetActor.h"
#include "SCTDoorSheet.h"

void ASCTDoorSheetActor::UpdateActorDimension()
{
	StaticCast<FDoorSheetShapeBase*>(ShapeData)->ReCalDividBlocksPositionAndSize();
	for (const auto & Ref : ShapeData->GetChildrenShapes())
	{
		ASCTShapeActor* CurChildActor = Ref->GetShapeActor();
		if (CurChildActor)
		{
			CurChildActor->UpdateActorDimension();
			CurChildActor->UpdateActorPosition();
		}
	}
}
