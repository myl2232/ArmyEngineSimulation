#include "../../../Public/Operation/HardMode/ArmyMaterialBrushOperation.h"
#include "ArmyWallActor.h"
#include "ArmyBaseArea.h"
#include "ArmyBaseEditStyle.h"
#include "ArmyGameInstance.h"
#include "ArmyReplaceTextureOperation.h"
#include "ArmyFrameCommands.h"
#include "ArmyHardModeCommonData.h"
#include "ArmyHardModeController.h"
#include "ArmyHardModeWallController.h"
#include "ArmyHardModeFloorController.h"
#include "ArmyHardModeCeilingController.h"
#include "ArmyPlayerController.h"
#include "ArmyMouseCaptureManager.h"
TSharedPtr<SImage> FArmyMaterialBrushOperation::CursorWidget=nullptr;

bool FArmyMaterialBrushOperation::bRebeginAtStart = false;

//TSharedPtr<FContentItemSpace::FContentItem> FArmyMaterialBrushOperation::AbsorbededContentItem = nullptr;

FArmyMaterialBrushOperation::FArmyMaterialBrushOperation(EModelType InBelongModel) : FArmyOperation(InBelongModel)
{
}

void FArmyMaterialBrushOperation::BeginOperation(XRArgument InArg)
{
	if (IsActive)
	{
		return;
	}
	IsActive = true;

	//设置光标
	if (!CursorWidget.IsValid())
	{
		CursorWidget = SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.Straw"));
	}

	//禁用编辑按钮
	if (!IsFormerCanUseEditMenuSet)
	{
		FormerCanUseEditMenu = FArmyFrameCommands::GetCanUseEditMenu();
		IsFormerCanUseEditMenuSet = true;
	}
	FArmyFrameCommands::SetCanUseEditMenu(false);

 

	if (bRebeginAtStart && AbsorbededContentItem.IsValid())
	{
		SetBrushState(BrushState::Place);
	}
	else
	{
		SetBrushState(BrushState::Absorb);
	}
	bRebeginAtStart = false;
	
	
}

void FArmyMaterialBrushOperation::EndOperation()
{

	if (!IsActive)
	{
		return;
	}
	IsActive = false;

	//恢复编辑按钮的可用状态
	FArmyFrameCommands::SetCanUseEditMenu(FormerCanUseEditMenu);
	IsFormerCanUseEditMenuSet = false;


	SetBrushState(BrushState::Idle);

	

	FArmyOperation::EndOperation();


}

void FArmyMaterialBrushOperation::ProcessClick(FSceneView & View, HHitProxy * HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
	if (Event == IE_Released)
	{
		if (Key == EKeys::LeftMouseButton)
		{

			//获取选择的面
			if (GGI->DesignEditor->CurrentController->GetDesignModelType() == EModelType::E_HardModel)
			{
				TSharedPtr<FArmyHardModeController> HardModeController = StaticCastSharedPtr<FArmyHardModeController>(GGI->DesignEditor->CurrentController);
				switch (HardModeController->CurrentEditMode)
				{
				case HO_FreeEdit:
					SelectedArea=HardModeController->GetSelectedArea();
					break;
				case 	HO_FloorEdit:
					SelectedArea = HardModeController->HardModeFloorController->GetSelectedArea();// EndOperation();//  SetOperation((uint8)EHardModeOperation::HO_Replace);
					break;
				case		HO_WallEdit:
					SelectedArea = HardModeController->HardModeWallController->GetSelectedArea();//// SetOperation((uint8)EHardModeOperation::HO_Replace);
					break;
				case	HO_RoofEdit:
					SelectedArea = HardModeController->HardModeCeilingController->GetSelectedArea();// // SetOperation((uint8)EHardModeOperation::HO_Replace);
				}

			}

		}
		else if (Key == EKeys::RightMouseButton)
		{
			if (GGI->DesignEditor->CurrentController->GetDesignModelType() == EModelType::E_HardModel)
			{
				EndOperation();
				TSharedPtr<FArmyHardModeController> HardModeController = StaticCastSharedPtr<FArmyHardModeController>(GGI->DesignEditor->CurrentController);

				switch (HardModeController->CurrentEditMode)
				{
				case HO_FreeEdit:
					HardModeController->EndOperation();
					break;
				case 	HO_FloorEdit:
					HardModeController->HardModeFloorController->EndOperation();//  SetOperation((uint8)EHardModeOperation::HO_Replace);
					break;
				case		HO_WallEdit:
					HardModeController->HardModeWallController->EndOperation();// SetOperation((uint8)EHardModeOperation::HO_Replace);
					break;
				case	HO_RoofEdit:
					HardModeController->HardModeCeilingController->EndOperation();// SetOperation((uint8)EHardModeOperation::HO_Replace);
				}

			}
		}
	}

	if (!SelectedArea.IsValid())
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("无材质可复用"));
		return;
	}


	switch(CurrentBrushState)
	{
	case BrushState::Absorb:
		if (SelectedArea->GetMatStyle()->HasGoodID())
		{
			//吸取材质
			Source = SelectedArea;
			MatData = SelectedArea->GetMatStyle();
			AbsorbededContentItem = SelectedArea->GetMatStyle()->GetContentItem();
			SetBrushState(BrushState::Place);
		}
		else
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("无材质可复用"));
		}
		break;

	} 

}

void FArmyMaterialBrushOperation::ProcessClickInViewPortClient(FViewport * InViewPort, FKey Key, EInputEvent Event)
{
	//把ProcessClickInViewPortClient函数转换为ProcessClick

	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		InViewPort,
		GVC->GetScene(),
		GVC->EngineShowFlags)
		.SetRealtimeUpdate(true));
	FSceneView *View = GVC->CalcSceneView(&ViewFamily);

	FArmyInputEventState InputState(InViewPort, Key, Event);
	
	FViewport* InputStateViewport = InViewPort;


		int32	HitX = InputStateViewport->GetMouseX();
		int32	HitY = InputStateViewport->GetMouseY();

		const float XScale = (float)View->ViewRect.Max.X / (float)View->UnscaledViewRect.Max.X;
		const float YScale = (float)View->ViewRect.Max.Y / (float)View->UnscaledViewRect.Max.Y;
		HitX = HitX * XScale;
		HitY = HitY * YScale;

			InViewPort->InvalidateHitProxy();
			HHitProxy* HitProxy = InputStateViewport->GetHitProxy(HitX, HitY);
			
			InputStateViewport->SetPreCaptureMousePosFromSlateCursor();
			ProcessClick(*View, HitProxy, Key, Event, HitX, HitY);
	
}

void FArmyMaterialBrushOperation::SetBrushState(BrushState NewState)
{

	auto StartReplaceTexture = [&]() {
		FArmyReplaceTextureOperation::SetCurrentItem(AbsorbededContentItem);
		FArmyReplaceTextureOperation::SetCurrentMatDataSource(MatData);
		if (GGI->DesignEditor->CurrentController->GetDesignModelType() == EModelType::E_HardModel)
		{
			TSharedPtr<FArmyHardModeController> HardModeController = StaticCastSharedPtr<FArmyHardModeController>(GGI->DesignEditor->CurrentController);

			switch (HardModeController->CurrentEditMode)
			{
			case HO_FreeEdit:
				HardModeController->SetOperation((uint8)EHardModeOperation::HO_Replace);
				break;
			case 	HO_FloorEdit:
				HardModeController->HardModeFloorController->ReplaceTextureOperation(AbsorbededContentItem);//  SetOperation((uint8)EHardModeOperation::HO_Replace);
				break;
			case		HO_WallEdit:
				HardModeController->HardModeWallController->ReplaceTextureOperation(AbsorbededContentItem);// SetOperation((uint8)EHardModeOperation::HO_Replace);
				break;
			case	HO_RoofEdit:
				HardModeController->HardModeCeilingController->ReplaceTextureOperation(AbsorbededContentItem);// SetOperation((uint8)EHardModeOperation::HO_Replace);
				break;
			}

		}
		
	};


	switch (CurrentBrushState)
	{
	case BrushState::Idle:
		if (NewState == BrushState::Absorb)
		{
			//从闲置状态转换到吸管状态
			CurrentBrushState = NewState;
			AbsorbededContentItem=nullptr;
			//切换光标
			//GXRPC->EXE_SetCustomCursor();
			CursorWidget->SetVisibility(EVisibility::Visible);
			GVC->SetCustomCursorWidget(CursorWidget.ToSharedRef());
			GVC->CurCursor = EMouseCursor::Type::Custom;
	//	SetMouseCursorWidget(EMouseCursor::Type::Custom, )

		}
		else if (NewState == BrushState::Place)
		{
			StartReplaceTexture();
		}
		break;
	case BrushState::Absorb:

		if (NewState == BrushState::Place)
		{
			//从吸管状态转换到放置状态
			CurrentBrushState = NewState;

			GVC->CurCursor = EMouseCursor::Type::Default;
			CursorWidget->SetVisibility(EVisibility::Hidden);
			StartReplaceTexture();
			
			
		}
		if (NewState == BrushState::Idle)
		{
			//从吸管状态转换到闲置状态
			CurrentBrushState = NewState;

			GVC->CurCursor = EMouseCursor::Type::Default;
			CursorWidget->SetVisibility(EVisibility::Hidden);
		}
		break;
	case BrushState::Place:
		if (NewState == BrushState::Idle)
		{
			//从放置状态转换到闲置状态
			CurrentBrushState = NewState;

		}
		break;
	}

}
