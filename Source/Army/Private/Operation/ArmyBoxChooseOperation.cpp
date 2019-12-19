#include "ArmyBoxChooseOperation.h"
#include "ArmyToolsModule.h"

void FArmyBoxChooseOperation::BeginOperation(XRArgument InArg /* = XRArgument() */)
{
	FArmyOperation::BeginOperation(InArg);
	FArmyToolsModule::Get().GetRectSelectTool()->End();

	//初始化变量
	FontFramePos = CurrentFramePos = FVector::ZeroVector;
	FlagStartDrag = true;
}

void FArmyBoxChooseOperation::EndOperation()
{
	SelectedObjList.Empty();
	FArmyOperation::EndOperation();
}

bool FArmyBoxChooseOperation::InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event)
{
	if (Key == EKeys::LeftMouseButton && Event == IE_Released)
	{
		//SelectTextLabel->SetState(OS_Normal);
		EndOperation();
	}
	return false;
}

void FArmyBoxChooseOperation::MouseDrag(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, FKey Key)
{
	FVector2D MousePos;
	GVC->GetMousePosition(MousePos);
	FVector WorldPos = GVC->PixelToWorld(MousePos.X, MousePos.Y, 0);
	WorldPos.Z = 0;

	if (FlagStartDrag)
	{
		FontFramePos = CurrentFramePos = WorldPos;
		FlagStartDrag = false;
	}
	else
	{
		CurrentFramePos = WorldPos;
	}

	//if (FVector::Distance(FontFramePos, CurrentFramePos) > 0.01)
	{
		FVector OffSet = CurrentFramePos - FontFramePos;
		FontFramePos = CurrentFramePos;
		
		for (FObjectPtr Object : SelectedObjList)
		{
			//计算鼠标拖拽规则
			FTransform Trans;
			FVector TargetOffset = CalSelectOperation(Object, OffSet);
			if (TargetOffset != FVector::ZeroVector)
			{
				Trans.SetLocation(TargetOffset);
				Object->ApplyTransform(Trans);
			}
		}
	}
}

void FArmyBoxChooseOperation::SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPoint)
{
	if (Object.IsValid())
	{
		SelectedObjList.Emplace(Object);
	}
}

FVector FArmyBoxChooseOperation::CalSelectOperation(const FObjectPtr& ObjectIt,const FVector InOffset) const
{
	FVector TargetOffset = FVector::ZeroVector;
	if (ObjectIt.IsValid())
	{
		if (ObjectIt->GetType() == EObjectType::OT_Dimensions ||
			ObjectIt->GetType() == EObjectType::OT_InSideWallDimensions ||
			ObjectIt->GetType() == EObjectType::OT_OutSideWallDimensions ||
			ObjectIt->GetType() == EObjectType::OT_AddWallDimensions ||
			ObjectIt->GetType() == EObjectType::OT_DeleteWallDimensions)
		{
			FArmyDimensions* Dimension = ObjectIt->AsassignObj<FArmyDimensions>();
			float Dot = FVector::DotProduct(InOffset.GetSafeNormal(), (Dimension->MainLine->GetStart() - Dimension->MainLine->GetEnd()).GetSafeNormal());
			if (Dot<0.26f && Dot>-0.26f) // COS 60 = 0.5f
			{
				//矫正移动方向，使之与标出方向垂直
				FVector CrossVector = FVector::CrossProduct(Dimension->MainLine->GetStart() - Dimension->MainLine->GetEnd(), FVector::UpVector);
				float Length = InOffset.Size();
				if (FVector::DotProduct(InOffset.GetSafeNormal(), CrossVector.GetSafeNormal()) > 0)
				{
					TargetOffset = CrossVector.GetSafeNormal();
				}
				else
				{
					TargetOffset = -CrossVector.GetSafeNormal();
				}
				TargetOffset *= Length;
			}
			else
			{
				//方向与标尺方向偏移太大的不要移动
				TargetOffset = FVector::ZeroVector;
			}
		}
		else if (ObjectIt->GetType() == EObjectType::OT_ComponentBase)
		{
			TargetOffset = FVector::ZeroVector;
		}
		else //非标尺
		{
			TargetOffset = InOffset;
		}
	}

	return TargetOffset;
}

