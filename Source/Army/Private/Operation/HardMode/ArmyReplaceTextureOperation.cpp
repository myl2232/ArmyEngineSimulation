#include "ArmyReplaceTextureOperation.h"
#include "ArmyCircleArea.h"
#include "ArmyEditorEngine.h"
#include "ArmyWhirlwindStyle.h"
#include "ArmyIntervalStyle.h"
#include "ArmySeamlessStyle.h"
#include "SArmyInputBox.h"
#include "ArmyMouseCapture.h"
#include "ArmyRectArea.h"
#include "ArmyCircleArea.h"
#include "ArmyWallActor.h"
#include "ArmyRect.h"
#include "ArmyRegularPolygonArea.h"
#include "ArmyCrossStyle.h"
#include "ArmyWorkerStyle.h"
#include "ArmyContinueStyle.h"
#include "ArmyCommonTypes.h"
#include "ArmyHerringBoneStyle.h"
#include "ArmyTrapezoidStyle.h"
#include "ArmyRoomSpaceArea.h"
#include "ArmyModalManager.h"
#include "SContentItem.h"
#include "ArmyHardModeController.h"
#include "ArmyCore/Public/Math/XRMath.h"
#include "ArmyExtrusionActor.h"
#include "ArmyResourceModule.h"
#include "EngineUtils.h"
#include "ArmyGameInstance.h"
#include "ArmyConstructionManager.h"
#include "Runtime/Online/ImageDownload/Public/WebImageCache.h"
#include "ArmySlateModule.h"
#include "ArmyBaseEditStyle.h"

 TSharedPtr<FArmyBaseEditStyle>  FArmyReplaceTextureOperation::MatDataSource=nullptr;
TSharedPtr<FContentItemSpace::FContentItem> FArmyReplaceTextureOperation::ContentItem=nullptr;

FArmyReplaceTextureOperation::FArmyReplaceTextureOperation(EModelType InBelongModel) :
	FArmyOperation(InBelongModel)
{
	UMaterial* LineMaterial = FArmyEngineModule::Get().GetEngineResource()->GetDefaultFloorTextureMat();
	MI_WaterKnifeTextureMat = UMaterialInstanceDynamic::Create(LineMaterial, NULL);
	MI_WaterKnifeTextureMat->AddToRoot();

	if (!ThumbnailImage.IsValid())
	{
		ThumbnailImage = SNew(SImage);
		ThumbnailImage->SetVisibility(EVisibility::Collapsed);
		GVC->ViewportOverlayWidget->AddSlot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.WidthOverride(30.f)
			.HeightOverride(30.f)
			[
				ThumbnailImage.ToSharedRef()
			]
			];
	}
}

void FArmyReplaceTextureOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	/* @ 梁晓菲 放置扣条Hover时如果Hover了单边，绘制单边*/
	if (bHoverBuckleEdge)
	{
		PDI->DrawLine(HoverEdgeStart, HoverEdgeEnd, FLinearColor(FColor(0XFFFF9800)), 1, 5.0f, 0.0, true);
	}
}

bool FArmyReplaceTextureOperation::InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event)
{
	return false;
}

void FArmyReplaceTextureOperation::ProcessClickInViewPortClient(FViewport* InViewPort, FKey Key, EInputEvent Event)
{
	if (CurrentEditState == 1)
	{
		if (Key == EKeys::LeftMouseButton)
		{
			TSharedPtr<FArmyBaseArea> selectedArea = PickOperation(FVector(GVC->GetCurrentMousePos(), 0.0), GVC);
			if (selectedArea.IsValid())
			{
				UpdateEditAreaSytle(selectedArea);
			}
		}
		/** @梁晓菲 右键取消操作*/
		if (Key == EKeys::RightMouseButton || Key == EKeys::Escape)
		{
			EndOperation();
		}
	}
}

void FArmyReplaceTextureOperation::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
	if (Event == IE_Released)
	{
		if (Key == EKeys::LeftMouseButton)
		{
			if (HitProxy)
			{
				if (HitProxy->IsA(HActor::StaticGetType()))
				{
					HActor* actorProxy = (HActor*)(HitProxy);
					AXRWallActor* wallActor = Cast<AXRWallActor>(actorProxy->Actor);
					AArmyExtrusionActor* ExtrusionActor = Cast<AArmyExtrusionActor>(actorProxy->Actor);
					if (wallActor)
					{
						UpdateEditAreaSytle(wallActor->AttachSurface);
					}
					else if (ExtrusionActor)
					{
						UpdateEditAreaSytle(ExtrusionActor->AttachSurfaceArea.Pin());
					}
				}
			}
		}
	}
}

void FArmyReplaceTextureOperation::BeginOperation(XRArgument InArg /*= XRArgument()*/)
{
	CurrentState = 1;
}

void FArmyReplaceTextureOperation::EndOperation()
{
	if (CurrentState == 1)
	{
		CurrentState = 0;

		MatDataSource = nullptr;
		//if (SContemtItemWidget.IsValid())
		//{
		//	TSharedPtr<SImage> ThumbnailImage1 = SContemtItemWidget->GetThumbnailImage();
		//	if (ThumbnailImage1.IsValid())
		//	{
		//		ThumbnailImage1->SetVisibility(EVisibility::Collapsed);
		//		GVC->ViewportOverlayWidget->RemoveSlot(ThumbnailImage1.ToSharedRef());
		//	}
		//	SContemtItemWidget = nullptr;
		//}


		if (ThumbnailImage.IsValid())
		{
			ThumbnailImage->SetVisibility(EVisibility::Collapsed);
			//	GVC->ViewportOverlayWidget->RemoveSlot(ThumbnailImage.ToSharedRef());
		}


		FArmyOperation::EndOperation();
	}

}

void FArmyReplaceTextureOperation::Tick()
{
	if (CurrentState == 1)
	{
	/*	if (SContemtItemWidget.IsValid())
		{
			FVector CurrentPoint = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0.0f);
			TSharedPtr<SImage> ThumbnailImage = SContemtItemWidget->GetThumbnailImage();*/
			if (ThumbnailImage.IsValid())
			{
				//设置缩略图图标随鼠标移动
				FVector2D ViewportSize;
				GVC->GetViewportSize(ViewportSize);
				FVector2D PixPosTemp;
				GVC->GetMousePosition(PixPosTemp);
				PixPosTemp -= ViewportSize / 2;
				ThumbnailImage->SetRenderTransform(FSlateRenderTransform(PixPosTemp));
			}
		//}
	}
}

void FArmyReplaceTextureOperation::MouseMove(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y)
{
	TSharedPtr<FArmyBaseArea> HoveredArea;
	if (CurrentEditState == 1)
	{
		HoveredArea = PickOperation(FVector(GVC->GetCurrentMousePos(), 0.0), GVC);
	}
	else
	{
		TArray<TSharedPtr< FArmyBaseArea>> surfaces = FArmySceneData::Get()->PickOperation(FVector(GVC->GetCurrentMousePos(), 0.0), GVC);
		if (surfaces.Num() > 0)
		{
			HoveredArea = surfaces[0];
		}
	}
	if (HoveredArea.IsValid())
	{
		AXRWallActor* wall = HoveredArea->GetWallActor();
		if (wall)
		{
			GXREditor->SelectNone(true, true);
			GXREditor->SelectActor(wall, true, false);
		}
	}
	if (HoveredArea.IsValid() && ContentItem->codeStrId == "GC0124" && HoveredArea->SurfaceType == 0 && HoveredArea->GetType() == OT_RoomSpaceArea)
	{
		TSharedPtr<FArmyRoomSpaceArea> roomArea = StaticCastSharedPtr<FArmyRoomSpaceArea>(HoveredArea);
		if (roomArea->GenerateFromObjectType == OT_Pass || roomArea->GenerateFromObjectType == OT_NewPass ||
			roomArea->GenerateFromObjectType == OT_Door || roomArea->GenerateFromObjectType == OT_SecurityDoor ||
			roomArea->GenerateFromObjectType == OT_SlidingDoor)
		{
			if (HoveredArea->BHoverSingleEdge(FVector2D(X, Y), GVC, HoverEdgeStart, HoverEdgeEnd))
			{
				bHoverBuckleEdge = true;
				return;
			}
		}
	}
	bHoverBuckleEdge = false;
}

void FArmyReplaceTextureOperation::SetCurrentItem(TSharedPtr<FContentItemSpace::FContentItem> item)
{
	if (!item.IsValid())
	{
		return;
	}
	ContentItem = item;

	SetThumbnail(item->ThumbnailURL);
}

TSharedPtr<SImage>  FArmyReplaceTextureOperation::ThumbnailImage =nullptr;
//@郭子阳 设置Thumbnail
void FArmyReplaceTextureOperation::SetThumbnail(FString ThumbnailUrl)
{
	TAttribute<const FSlateBrush*> ThumbnailImageBrush = FArmySlateModule::Get().WebImageCache->Download(ThumbnailUrl).Get().Attr();
	//TSharedPtr<SImage>  
	ThumbnailImage->SetImage(ThumbnailImageBrush);

	//if (ThumbnailImage.IsValid())
	//{
		ThumbnailImage->SetVisibility(EVisibility::HitTestInvisible);
	/*	GVC->ViewportOverlayWidget->AddSlot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.WidthOverride(30.f)
			.HeightOverride(30.f)
			[
				ThumbnailImage.ToSharedRef()
			]
			];*/
	//}
}

//void FArmyReplaceTextureOperation::SetCurrentSelectItem(TSharedPtr<class SContentItem> item)
//{
//	SContemtItemWidget = item;
//	if (item.IsValid())
//	{
//		TSharedPtr<SImage> ThumbnailImage = SContemtItemWidget->GetThumbnailImage();
//		if (ThumbnailImage.IsValid())
//		{
//			ThumbnailImage->SetVisibility(EVisibility::HitTestInvisible);
//			GVC->ViewportOverlayWidget->AddSlot()
//				.HAlign(HAlign_Center)
//				.VAlign(VAlign_Center)
//				[
//					SNew(SBox)
//					.WidthOverride(30.f)
//					.HeightOverride(30.f)
//					[
//						ThumbnailImage.ToSharedRef()
//					]
//				];
//		}
//	}
//}

void FArmyReplaceTextureOperation::SetCurrentEditSurface(TSharedPtr<class FArmyRoomSpaceArea> RoomSpaceArea)
{
	CurrentEditSurface = RoomSpaceArea;
}

void FArmyReplaceTextureOperation::SetEditMode(uint32 mode)
{
	CurrentEditState = mode;
}

bool FArmyReplaceTextureOperation::ReplaceTextureOperation()
{

	TArray<TSharedPtr< FArmyBaseArea>> surfaces = FArmySceneData::Get()->PickOperation(FVector(GVC->GetCurrentMousePos(), 0.0), GVC);
	if (surfaces.Num() > 0)
	{
		UpdateEditAreaSytle(surfaces[0]);
		return true;
	}
	return false;

}

TSharedPtr<FArmyBaseArea> FArmyReplaceTextureOperation::PickOperation(const FVector& pos, class UArmyEditorViewportClient* InViewportClient)
{
	if (CurrentEditSurface.IsValid())
	{
		//float dist;
		//if (CurrentEditSurface->IsSelected(pos, InViewportClient, dist))
		//	return CurrentEditSurface;
		//TArray<TSharedPtr<FArmyBaseArea>> EditAreas = CurrentEditSurface->GetEditAreas();
		//int number = EditAreas.Num();
		//for (int i = 0; i < number; i++)
		//{
		//	if (EditAreas[i]->IsSelected(pos, InViewportClient, dist))
		//	{
		//		return EditAreas[i];
		//	}
		//}
		FVector LocalPositon = FArmyOperation::GetIntersectPointWithPlane(FVector2D(pos), CurrentEditSurface);
		auto selectedArea = CurrentEditSurface->FindArea(LocalPositon);
		if (selectedArea.IsValid())
		{
			return selectedArea;
		}
	}
	return NULL;
}

void FArmyReplaceTextureOperation::UpdateEditAreaSytle(TSharedPtr<FArmyBaseArea> editArea)
{
	if (!editArea.IsValid())
		return;
	if (!ContentItem.IsValid())
		return;
	TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ContentItem->GetResObjNoComponent();
	if (resArr.Num() < 1)
		return;
	TSharedPtr<FContentItemSpace::FResObj> ResObj = resArr[0];
	TSharedPtr<FContentItemSpace::FProductObj> PdtObj = ContentItem->ProObj;

	TSharedPtr<FContentItemSpace::FArmyHardModeRes> Res = StaticCastSharedPtr<FContentItemSpace::FArmyHardModeRes>(ResObj);
	TSharedPtr<FContentItemSpace::FArmyHardModeProduct> Pro = StaticCastSharedPtr<FContentItemSpace::FArmyHardModeProduct>(PdtObj);
	checkSlow(Pro->Width != 0.0f);
	checkSlow(Pro->Length != 0.0f);

	FString _FilePath = ResObj->FilePath;
	if (!FPaths::FileExists(*_FilePath))//文件不存在直接返回
		return;
	UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();

	if (ResObj.IsValid())
	{
		if (editArea->SurportPlaceArea == 0 && Res->placePosition.bFloor == false)
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("该商品不支持放置在地面"));
			return;
		}

		if (editArea->SurportPlaceArea == 1 && Res->placePosition.bWall == false)
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("该商品不支持放置在墙面"));
			return;
		}
		if (editArea->SurportPlaceArea == 2 && Res->placePosition.bCeiling == false)
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("该商品不支持放置在顶面"));
			return;
		}
	}



	CategryApplicationType PavingMethod = Res->ResourceCategryType;

	FGuid BodaAreaID;

	TSharedPtr<FArmyBaseEditStyle> MatStyle;// = nullptr;
	TSharedPtr<FArmyBodaArea> BodaArea;// = nullptr;
	switch (PavingMethod)
	{
	case CAT_None:
		break;
	case CAT_WallPaper:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
		if (MID)
		{

			MatStyle = MakeShareable(new FArmySeamlessStyle());
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
			if (ContentItem->codeStrId.Equals(TEXT("GC0294")))
			{
				FLinearColor color;
				MID->GetVectorParameterValue("Diffuse_Color", color);
				MatStyle->SetWallPaintColor(color);
				TSharedPtr<FArmySeamlessStyle> tempStyle = StaticCastSharedPtr<FArmySeamlessStyle>(MatStyle);
				tempStyle->SeamlesStyleType = 1;
			}
		}
		else
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		}
		break;
	}
	case CAT_Continue:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
		if (MID)
		{
			MatStyle = MakeShareable(new FArmyContinueStyle());
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
		}
		else
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		break;
	}
	case CAT_SlopeContinue:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
		if (MID)
		{
			MatStyle = MakeShareable(new FArmySlopeContinueStyle());
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
		}
		else
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		break;
	}
	case CAT_People:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		if (mat)
		{
			MatStyle = MakeShareable(new FArmyHerringBoneStyle());
			UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
		}
		else
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		break;
	}
	case CAT_Worker:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		if (mat)
		{
			MatStyle = MakeShareable(new FArmyWorkerStyle());
			UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
		}
		else
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		break;
	}
	case CAT_Wind:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		if (mat)
		{
			MatStyle = MakeShareable(new FArmyWhirlwindStyle());
			UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
		}
		else
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		break;
	}
	case CAT_Space:
	{
		break;
		MatStyle = MakeShareable(new FArmyIntervalStyle());
		FString _FilePath1 = Res->FilePath;
		FString _FilePath2 = Res->FilePath;
		UTexture2D * FloorTexture1 = FArmyEngineModule::Get().GetEngineResource()->ImportTexture(_FilePath1);
		UTexture2D * FloorTexture2 = FArmyEngineModule::Get().GetEngineResource()->ImportTexture(_FilePath2);
		if (FloorTexture1 != NULL && FloorTexture2 != NULL)
		{
			MatStyle->SetFirstAndSecondImage(FloorTexture1, FloorTexture2);
			MatStyle->SetBrickWidthAndHeight(Pro->Width, Pro->Length);

			MatStyle->SetMainTextureLocalPath(_FilePath1);
			MatStyle->SetSecondTexturePath(_FilePath2);
		}

		break;
	}
	case CAT_Interlace:
	{
		break;
		MatStyle = MakeShareable(new FArmyCrossStyle());
		FString _FilePath1 = Res->FilePath;
		FString _FilePath2 = Res->FilePath;
		UTexture2D * FloorTexture1 = FArmyEngineModule::Get().GetEngineResource()->ImportTexture(_FilePath1);
		UTexture2D * FloorTexture2 = FArmyEngineModule::Get().GetEngineResource()->ImportTexture(_FilePath2);
		if (FloorTexture1 != NULL && FloorTexture2 != NULL)
		{
			MatStyle->SetFirstAndSecondImage(FloorTexture1, FloorTexture2);
			MatStyle->SetBrickWidthAndHeight(Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath1);
			MatStyle->SetSecondTexturePath(_FilePath2);
		}

		break;
	}
	case CAT_TrapeZoid:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		if (mat)
		{
			MatStyle = MakeShareable(new FArmyTrapezoidStyle());
			UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
		}
		else
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		break;
	}
	case CAT_AntiCornerBrickMethod:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		if (!mat)
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
			break;
		}
		if (editArea->GetType() == OT_RectArea)
		{
			TSharedPtr<FArmyRectArea> ResultArea = StaticCastSharedPtr<FArmyRectArea>(editArea);
			SCOPE_TRANSACTION(TEXT("修改矩形区域无角砖波打线"));

			UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
			ResultArea->SetBodaBrickAndThickness(Pro->Length / 10.f, Pro->Width / 10.f, ContentItem);
			ResultArea->SetBodaMaterial(MID, ContentItem->ID);
			ResultArea->SetBodaTextureURL(Res->FilePath);

			ResultArea->SetCornerType(AntiCornerBrick);
			ResultArea->SetState(OS_Normal);
			editArea->Modify();
			BodaArea = ResultArea->GetBodaSurfaceArea();
				MatStyle = ResultArea->GetBodaSurfaceArea()->GetBodaMatStyle();
		}
		//else if (editArea->GetType() == OT_PolygonArea)
		//{
		//	TSharedPtr<FArmyRegularPolygonArea> ResultRegularPolygon = StaticCastSharedPtr<FArmyRegularPolygonArea>(editArea);
		//	SCOPE_TRANSACTION(TEXT("修改多边形区域无角砖波打线"));
		//	FString _FilePath = Res->FilePath;
		//	ResultRegularPolygon->SetBodaThickness(10);
		//	UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
		//	ResultRegularPolygon->SetBodaMaterial(MID, ContentItem->ID);

		//	ResultRegularPolygon->SetState(OS_Normal);
		//	ResultRegularPolygon->SetBodaTextureURL(_FilePath);
		//	editArea->Modify();
		//	BodaAreaID = ResultRegularPolygon->GetUniqueID();
		//	//MatStyle = ResultRegularPolygon->GetBodaSurfaceArea()->GetBodaMatStyle();
		//}
		else
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("该区域不支持角砖铺贴"));
		}

		break;
	}
	case CAT_CornerBrickMethod:
	{
		break;
		if (editArea->GetType() == OT_RectArea)
		{
			TSharedPtr<FArmyRectArea> ResultArea = StaticCastSharedPtr<FArmyRectArea>(editArea);
			SCOPE_TRANSACTION(TEXT("矩形区域有角砖添加"));
			ResultArea->SetCornerType(CornerBrick);
			FString _FilePath1 = Res->FilePath;
			FString _FilePath2 = Res->FilePath;
			ResultArea->SetBodaTextureURL(_FilePath1);
			ResultArea->SetCornerTextureURL(_FilePath2);
			UTexture2D * FloorTexture1 = FArmyEngineModule::Get().GetEngineResource()->ImportTexture(_FilePath1);
			UTexture2D * FloorTexture2 = FArmyEngineModule::Get().GetEngineResource()->ImportTexture(_FilePath2);
			if (FloorTexture2 != NULL && FloorTexture1 != NULL)
			{
				ResultArea->SetState(OS_Normal);
			}
			editArea->Modify();
			BodaArea = ResultArea->GetBodaSurfaceArea();
			MatStyle = ResultArea->GetBodaSurfaceArea()->GetBodaMatStyle();
		}

		else
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("该区域不支持角砖铺贴"));
		}

		break;
	}
	case CAT_FloorKickingLine:
    {
        FString _FilePath = Res->FilePath;
		if (editArea->SurfaceType == 0)
		{
			if (editArea->GetType() == OT_RoomSpaceArea)
			{
				//SCOPE_TRANSACTION(TEXT("房间区域添加踢脚线"));
				TSharedPtr<FArmyRoomSpaceArea> roomArea = StaticCastSharedPtr<FArmyRoomSpaceArea>(editArea);
				FString FileMaterialPath = TEXT("");
				if (resArr.Num() > 1)
				{
					FileMaterialPath = resArr[1]->FilePath;
				}
				roomArea->GenerateExtrudeActor(GVC->GetWorld(), ContentItem);
				editArea->Modify();
			}
			editArea->SkitLineGoodsID(ContentItem->ID);
		}
		else
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("该区域不支持此放样操作"));
		}
		break;
	}
	case CAT_CrownMoulding:
	{
		if (editArea->SurfaceType == 2)
		{
			editArea->SkitLineGoodsID(ContentItem->ID);

			TSharedPtr<FArmyRoomSpaceArea> tempRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(editArea);
			if (tempRoom->GenerateFromObjectType == OT_Beam
				|| editArea->GetType() == OT_HelpRectArea
				|| editArea->GetType() == OT_ClinderArea)
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("该区域不支持此放样操作"));
				break;
			}

			//SCOPE_TRANSACTION(TEXT("房间区域添加顶角线"));
			FString FileMaterialPath = TEXT("");
			if (resArr.Num() > 1)
			{
				FileMaterialPath = resArr[1]->FilePath;
			}
			editArea->GenerateExtrudeActor(GVC->GetWorld(), ContentItem);
			editArea->Modify();
		}
		else
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("该区域不支持此放样操作"));
		}
		break;
	}
	case CAT_MoldingLine:
	{
		if (editArea->SurfaceType == 1 && editArea->GetType() != OT_RoomSpaceArea)
		{
			if (editArea->GetType() == OT_RectArea ||
				editArea->GetType() == OT_CircleArea ||
				editArea->GetType() == OT_FreePolygonArea ||
				editArea->GetType() == OT_PolygonArea)
			{
				SCOPE_TRANSACTION(TEXT("墙面区域添加造型线"));
				FString FileMaterialPath = TEXT("");
				if (resArr.Num() > 1)
				{
					FileMaterialPath = resArr[1]->FilePath;
				}
				editArea->GenerateWallMoldingLine(GVC->GetWorld(), ContentItem);
				editArea->SkitLineGoodsID(ContentItem->ID);
				editArea->Modify();
			}
		}
	}
	break;
	case CAT_LampSlot:
	{
		if (editArea->SurfaceType == 2)
		{
			if (editArea->GetDeltaHeight() != 0.0f)
			{
				SCOPE_TRANSACTION(TEXT("顶面添加灯槽"));
				editArea->GenerateLampSlot(GVC->GetWorld(), ContentItem, 13.f);
				editArea->SkitLineGoodsID(ContentItem->ID);
				editArea->Modify();
			}
			else
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("请首先对吊顶下吊"));
			}
		}
	}
	break;
	case CAT_Buckle:
	{
		/* @梁晓菲 V1.2版本只支持在过门石区域单边添加扣条*/
		if (editArea->SurfaceType == 0 && editArea->GetType() == OT_RoomSpaceArea)
		{
			TSharedPtr<FArmyRoomSpaceArea> roomArea = StaticCastSharedPtr<FArmyRoomSpaceArea>(editArea);
			if (roomArea->GenerateFromObjectType == OT_Pass || roomArea->GenerateFromObjectType == OT_NewPass ||
				roomArea->GenerateFromObjectType == OT_Door || roomArea->GenerateFromObjectType == OT_SecurityDoor ||
				roomArea->GenerateFromObjectType == OT_SlidingDoor)
			{
				editArea->SetbIsSingleBuckle(bHoverBuckleEdge);
				editArea->GenerateBuckleActorLine(GVC->GetWorld(), ContentItem);
			}
			else
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("请在过门石区域添加扣条"));
			}
		}
		else
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("请在过门石区域添加扣条"));
		}
		break;
	}
	default:
		break;
	}

	auto IsBodaArea = [&PavingMethod]()->bool
	{
		return 	PavingMethod == CAT_AntiCornerBrickMethod ||
			PavingMethod == CAT_CornerBrickMethod ;
	};


	if (MatStyle.IsValid() && !IsBodaArea())
	{
		TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ContentItem->GetResObjNoComponent();
		if (resArr.Num() < 1)
			return;
		//TSharedPtr<FArmyHardModeRes> tempRes = StaticCastSharedPtr<FArmyHardModeRes>(resArr[0]);

		SCOPE_TRANSACTION(TEXT("修改铺贴样式"));
		
		MatStyle->SetPlaneInfo(editArea->GetPlaneCenter(), editArea->GetXDir(), editArea->GetYDir());
		if (MatDataSource.IsValid())
		{
			MatStyle->CopyFrom(MatDataSource);
		}
		else
		{
			MatStyle->SetGoodsID(ContentItem->ID);
			MatStyle->SetCurrentItem(ContentItem);
//			MatStyle->SetIsDefaultStyle(true);
		}
		editArea->SetStyle(MatStyle);
		if (!MatStyle->DefaultGapColor)
		{
			editArea->SetGapColor(MatStyle->GapColor);
		}
		editArea->Modify();


	}


	//@郭子阳 V1.5
	//铺贴类获取施工项
	if (PavingMethod != CAT_Continue&&PavingMethod != CAT_Wind&&PavingMethod != CAT_Space
		&& PavingMethod != CAT_Interlace && PavingMethod != CAT_People&&PavingMethod != CAT_TrapeZoid
		&& PavingMethod != CAT_AntiCornerBrickMethod
		&& PavingMethod != CAT_CornerBrickMethod
		&& PavingMethod != CAT_WallPaper && PavingMethod != CAT_Worker)
	{
		//@郭子阳 施工项会在生成方样线条时请求，在此处不再请求

		///** @梁晓菲 放样的时候存储ItemID以及相应的施工项数据*/
		//TSharedPtr<FJsonObject> TempJsonObj = FArmySceneData::Get()->GetJsonDataByItemID(ContentItem->ID);
		//if (!TempJsonObj.IsValid())
		//{
		//	ReqConstructionData(ContentItem->ID);
		//}
	}
	else {
		if (PavingMethod != CAT_None &&MatStyle.IsValid())
		{
			//ConstructionPatameters Parameter;

			////这个面有没有下吊
			//bool HasHung = false;
			//if (editArea->SurportPlaceArea == 2 && editArea->GetExtrusionHeight() != 0)
			//{
			//	HasHung = true;
			//}

			//EStyleType PaveMethod = EStyleType::OT_StyleTypeNone;
			//PaveMethod = MatStyle->GetEditType();
		
			if (IsBodaArea())
			{
			//	Parameter.SetPaveInfo(MatStyle, EWallType::All, HasHung);
				XRConstructionManager::Get()->TryToFindConstructionData(BodaArea->GetUniqueID(), editArea->GetMatConstructionParameter(), nullptr);
				
			}
			else
			{ 
			//	Parameter.SetPaveInfo(MatStyle, (EWallType)editArea->SurportPlaceArea, HasHung);
				XRConstructionManager::Get()->TryToFindConstructionData(editArea->GetUniqueID(), editArea->GetMatConstructionParameter(), nullptr);
			}
		}
	}


	if (CurrentEditState == 1)
	{
		//EndOperation();
	}
	else if (CurrentEditState == 0)
	{
		if (PavingMethod != CAT_Continue && PavingMethod != CAT_Wind && PavingMethod != CAT_Space
			&& PavingMethod != CAT_Interlace && PavingMethod != CAT_People && PavingMethod != CAT_TrapeZoid
			&& PavingMethod != CAT_WallPaper && PavingMethod != CAT_Worker && PavingMethod != CAT_SlopeContinue)
		{
			EndOperation();
		}
	}
}

void FArmyReplaceTextureOperation::CopyEditAreaSytle(TSharedPtr<FArmyBaseArea> SourceEditArea, TSharedPtr<FArmyBaseArea> DesitinationEditArea)
{
	if (!( SourceEditArea.IsValid()
		&& DesitinationEditArea.IsValid()))
	{
		return;
	}
	FContentItemPtr ContentItem = SourceEditArea->GetStyle()->GetContentItem();
	if (!ContentItem.IsValid())
	{
		return;
	}

	TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ContentItem->GetResObjNoComponent();
	if (resArr.Num() < 1)
		return;
	TSharedPtr<FContentItemSpace::FResObj> ResObj = resArr[0];
	TSharedPtr<FContentItemSpace::FProductObj> PdtObj = ContentItem->ProObj;

	TSharedPtr<FContentItemSpace::FArmyHardModeRes> Res = StaticCastSharedPtr<FContentItemSpace::FArmyHardModeRes>(ResObj);
	TSharedPtr<FContentItemSpace::FArmyHardModeProduct> Pro = StaticCastSharedPtr<FContentItemSpace::FArmyHardModeProduct>(PdtObj);
	checkSlow(Pro->Width != 0.0f);
	checkSlow(Pro->Length != 0.0f);

	FString _FilePath = ResObj->FilePath;
	if (!FPaths::FileExists(*_FilePath))//文件不存在直接返回
		return;
	UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();

	if (ResObj.IsValid())
	{
		if (DesitinationEditArea->SurportPlaceArea == 0 && Res->placePosition.bFloor == false)
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("该商品不支持放置在地面"));
			return;
		}

		if (DesitinationEditArea->SurportPlaceArea == 1 && Res->placePosition.bWall == false)
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("该商品不支持放置在墙面"));
			return;
		}
		if (DesitinationEditArea->SurportPlaceArea == 2 && Res->placePosition.bCeiling == false)
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("该商品不支持放置在顶面"));
			return;
		}
	}

	auto MakeMatStyle = [](EStyleType type)->TSharedPtr<FArmyBaseEditStyle>
	{
		switch (type)
		{
		case 1:
			return  MakeShareable(new FArmyContinueStyle());
			break;
		case 2:
			return  MakeShareable(new FArmyHerringBoneStyle());
			break;
		case 3:
			return  MakeShareable(new FArmyTrapezoidStyle());
			break;
		case 4:
			return  MakeShareable(new FArmyWhirlwindStyle());
			break;
		case 5:
			return MakeShareable(new FArmyWorkerStyle());
			break;
		case 6:
			return MakeShareable(new FArmyCrossStyle());
			break;
		case 7:
			return MakeShareable(new FArmyIntervalStyle());
			break;
		case 8:
			return MakeShareable(new FArmySeamlessStyle());
			break;
		case 10:
			return MakeShareable(new FArmySlopeContinueStyle());
			break;
		default:
			return MakeShareable(new FArmyContinueStyle());
			
			break;
		}
	};

	CategryApplicationType PavingMethod = Res->ResourceCategryType;

	TSharedPtr<FArmyBaseEditStyle> MatStyle = nullptr;
	switch (PavingMethod)
	{
	case CAT_None:
		break;
	case CAT_WallPaper:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
		if (MID)
		{

			MatStyle = MakeMatStyle(SourceEditArea->GetStyle()->GetEditType());;
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
			if (ContentItem->codeStrId.Equals(TEXT("GC0294")))
			{
				FLinearColor color;
				MID->GetVectorParameterValue("Diffuse_Color", color);
				MatStyle->SetWallPaintColor(color);
				TSharedPtr<FArmySeamlessStyle> tempStyle = StaticCastSharedPtr<FArmySeamlessStyle>(MatStyle);
				tempStyle->SeamlesStyleType = 1;
			}
		}
		else
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		}
		break;
	}
	case CAT_Continue:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
		if (MID)
		{
			MatStyle = MakeMatStyle(SourceEditArea->GetStyle()->GetEditType());;
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
		}
		else
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		break;
	}
	case CAT_SlopeContinue:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
		if (MID)
		{
			MatStyle = MakeMatStyle(SourceEditArea->GetStyle()->GetEditType());
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
		}
		else
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		break;
	}
	case CAT_People:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		if (mat)
		{
			MatStyle = MakeMatStyle(SourceEditArea->GetStyle()->GetEditType());
			UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
		}
		else
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		break;
	}
	case CAT_Worker:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		if (mat)
		{
			MatStyle = MakeMatStyle(SourceEditArea->GetStyle()->GetEditType());
			UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
		}
		else
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		break;
	}
	case CAT_Wind:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		if (mat)
		{
			MatStyle = MakeMatStyle(SourceEditArea->GetStyle()->GetEditType());
			UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
		}
		else
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		break;
	}
	case CAT_Space:
	{
		break;
		MatStyle = MakeMatStyle(SourceEditArea->GetStyle()->GetEditType());
		FString _FilePath1 = Res->FilePath;
		FString _FilePath2 = Res->FilePath;
		UTexture2D * FloorTexture1 = FArmyEngineModule::Get().GetEngineResource()->ImportTexture(_FilePath1);
		UTexture2D * FloorTexture2 = FArmyEngineModule::Get().GetEngineResource()->ImportTexture(_FilePath2);
		if (FloorTexture1 != NULL && FloorTexture2 != NULL)
		{
			MatStyle->SetFirstAndSecondImage(FloorTexture1, FloorTexture2);
			MatStyle->SetBrickWidthAndHeight(Pro->Width, Pro->Length);

			MatStyle->SetMainTextureLocalPath(_FilePath1);
			MatStyle->SetSecondTexturePath(_FilePath2);
		}

		break;
	}
	case CAT_Interlace:
	{
		break;
		MatStyle = MakeMatStyle(SourceEditArea->GetStyle()->GetEditType());
		FString _FilePath1 = Res->FilePath;
		FString _FilePath2 = Res->FilePath;
		UTexture2D * FloorTexture1 = FArmyEngineModule::Get().GetEngineResource()->ImportTexture(_FilePath1);
		UTexture2D * FloorTexture2 = FArmyEngineModule::Get().GetEngineResource()->ImportTexture(_FilePath2);
		if (FloorTexture1 != NULL && FloorTexture2 != NULL)
		{
			MatStyle->SetFirstAndSecondImage(FloorTexture1, FloorTexture2);
			MatStyle->SetBrickWidthAndHeight(Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath1);
			MatStyle->SetSecondTexturePath(_FilePath2);
		}

		break;
	}
	case CAT_TrapeZoid:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		if (mat)
		{
			MatStyle = MakeMatStyle(SourceEditArea->GetStyle()->GetEditType());
			UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
			MatStyle->SetMainMaterial(MID, Pro->Width, Pro->Length);
			MatStyle->SetMainTextureLocalPath(_FilePath);
		}
		else
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
		break;
	}
	case CAT_AntiCornerBrickMethod:
	{
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
		if (!mat)
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效,请检查文件"));
			break;
		}
		if (DesitinationEditArea->GetType() == OT_RectArea)
		{
			TSharedPtr<FArmyRectArea> ResultArea = StaticCastSharedPtr<FArmyRectArea>(DesitinationEditArea);
			SCOPE_TRANSACTION(TEXT("修改矩形区域无角砖波打线"));

			UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
			ResultArea->SetBodaBrickAndThickness(Pro->Length / 10.f, Pro->Width / 10.f, ContentItem);
			ResultArea->SetBodaMaterial(MID, ContentItem->ID);
			ResultArea->SetBodaTextureURL(Res->FilePath);

			ResultArea->SetCornerType(AntiCornerBrick);
			ResultArea->SetState(OS_Normal);
			DesitinationEditArea->Modify();
		}
		else if (DesitinationEditArea->GetType() == OT_PolygonArea)
		{
			TSharedPtr<FArmyRegularPolygonArea> ResultRegularPolygon = StaticCastSharedPtr<FArmyRegularPolygonArea>(DesitinationEditArea);
			SCOPE_TRANSACTION(TEXT("修改多边形区域无角砖波打线"));
			FString _FilePath = Res->FilePath;
			ResultRegularPolygon->SetBodaThickness(10);
			UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
			ResultRegularPolygon->SetBodaMaterial(MID, ContentItem->ID);

			ResultRegularPolygon->SetState(OS_Normal);
			ResultRegularPolygon->SetBodaTextureURL(_FilePath);
			DesitinationEditArea->Modify();
		}
		else
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("该区域不支持角砖铺贴"));
		}

		break;
	}
	case CAT_CornerBrickMethod:
	{
		break;
		if (DesitinationEditArea->GetType() == OT_RectArea)
		{
			TSharedPtr<FArmyRectArea> ResultArea = StaticCastSharedPtr<FArmyRectArea>(DesitinationEditArea);
			SCOPE_TRANSACTION(TEXT("矩形区域有角砖添加"));
			ResultArea->SetCornerType(CornerBrick);
			FString _FilePath1 = Res->FilePath;
			FString _FilePath2 = Res->FilePath;
			ResultArea->SetBodaTextureURL(_FilePath1);
			ResultArea->SetCornerTextureURL(_FilePath2);
			UTexture2D * FloorTexture1 = FArmyEngineModule::Get().GetEngineResource()->ImportTexture(_FilePath1);
			UTexture2D * FloorTexture2 = FArmyEngineModule::Get().GetEngineResource()->ImportTexture(_FilePath2);
			if (FloorTexture2 != NULL && FloorTexture1 != NULL)
			{
				ResultArea->SetState(OS_Normal);
			}
			DesitinationEditArea->Modify();
		}

		else
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("该区域不支持角砖铺贴"));
		}

		break;
	}
	case CAT_FloorKickingLine:
	{
		FString _FilePath = Res->FilePath;
		if (DesitinationEditArea->SurfaceType == 0)
		{
			if (DesitinationEditArea->GetType() == OT_RoomSpaceArea)
			{
				//SCOPE_TRANSACTION(TEXT("房间区域添加踢脚线"));
				TSharedPtr<FArmyRoomSpaceArea> roomArea = StaticCastSharedPtr<FArmyRoomSpaceArea>(DesitinationEditArea);
				FString FileMaterialPath = TEXT("");
				if (resArr.Num() > 1)
				{
					FileMaterialPath = resArr[1]->FilePath;
				}
				roomArea->GenerateExtrudeActor(GVC->GetWorld(), ContentItem);
				DesitinationEditArea->Modify();
			}
			DesitinationEditArea->SkitLineGoodsID(ContentItem->ID);
		}
		else
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("该区域不支持此放样操作"));
		}
		break;
	}
	case CAT_CrownMoulding:
	{
		if (DesitinationEditArea->SurfaceType == 2)
		{
			DesitinationEditArea->SkitLineGoodsID(ContentItem->ID);

			TSharedPtr<FArmyRoomSpaceArea> tempRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(DesitinationEditArea);
			if (tempRoom->GenerateFromObjectType == OT_Beam
				|| DesitinationEditArea->GetType() == OT_HelpRectArea
				|| DesitinationEditArea->GetType() == OT_ClinderArea)
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("该区域不支持此放样操作"));
				break;
			}

			//SCOPE_TRANSACTION(TEXT("房间区域添加顶角线"));
			FString FileMaterialPath = TEXT("");
			if (resArr.Num() > 1)
			{
				FileMaterialPath = resArr[1]->FilePath;
			}
			DesitinationEditArea->GenerateExtrudeActor(GVC->GetWorld(), ContentItem);
			DesitinationEditArea->Modify();
		}
		else
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("该区域不支持此放样操作"));
		}
		break;
	}
	case CAT_MoldingLine:
	{
		if (DesitinationEditArea->SurfaceType == 1 && DesitinationEditArea->GetType() != OT_RoomSpaceArea)
		{
			if (DesitinationEditArea->GetType() == OT_RectArea ||
				DesitinationEditArea->GetType() == OT_CircleArea ||
				DesitinationEditArea->GetType() == OT_FreePolygonArea ||
				DesitinationEditArea->GetType() == OT_PolygonArea)
			{
				SCOPE_TRANSACTION(TEXT("墙面区域添加造型线"));
				FString FileMaterialPath = TEXT("");
				if (resArr.Num() > 1)
				{
					FileMaterialPath = resArr[1]->FilePath;
				}
				DesitinationEditArea->GenerateWallMoldingLine(GVC->GetWorld(), ContentItem);
				DesitinationEditArea->SkitLineGoodsID(ContentItem->ID);
				DesitinationEditArea->Modify();
			}
		}
	}
	break;
	case CAT_LampSlot:
	{
		if (DesitinationEditArea->SurfaceType == 2)
		{
			if (DesitinationEditArea->GetDeltaHeight() != 0.0f)
			{
				SCOPE_TRANSACTION(TEXT("顶面添加灯槽"));
				DesitinationEditArea->GenerateLampSlot(GVC->GetWorld(), ContentItem, 13.f);
				DesitinationEditArea->SkitLineGoodsID(ContentItem->ID);
				DesitinationEditArea->Modify();
			}
			else
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("请首先对吊顶下吊"));
			}
		}
	}
	break;
	
	case CAT_Buckle:
	{
		//不考虑扣条
		break;
	}
	default:
		break;
	}
	if (MatStyle.IsValid())
	{
		TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ContentItem->GetResObjNoComponent();
		if (resArr.Num() < 1)
			return;
		TSharedPtr<FArmyHardModeRes> tempRes = StaticCastSharedPtr<FArmyHardModeRes>(resArr[0]);

		SCOPE_TRANSACTION(TEXT("修改铺贴样式"));
		MatStyle->SetGoodsID(ContentItem->ID);
		MatStyle->SetPlaneInfo(DesitinationEditArea->GetPlaneCenter(), DesitinationEditArea->GetXDir(), DesitinationEditArea->GetYDir());
		MatStyle->SetCurrentItem(ContentItem);
		DesitinationEditArea->SetStyle(MatStyle);
		DesitinationEditArea->Modify();

		//复制基本数据
		MatStyle->CopyFrom(SourceEditArea->GetStyle());
	}

	//复制砖缝颜色
	DesitinationEditArea->SetGapColor(SourceEditArea->GetGapColor());
}

void FArmyReplaceTextureOperation::ReqConstructionData(int32 GoodsId)
{
	FString url = FString::Printf(TEXT("/api/quota/goods/%d"), GoodsId);
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(url, FArmyHttpRequestCompleteDelegate::CreateRaw(this, &FArmyReplaceTextureOperation::ResConstructionData, GoodsId));
	Request->ProcessRequest();
}

void FArmyReplaceTextureOperation::ResConstructionData(FArmyHttpResponse Response, int32 GoodsId)
{
	if (Response.bWasSuccessful && Response.Data.IsValid())
	{
		FArmySceneData::Get()->AddItemIDConstructionData(GoodsId, Response.Data);
	}
}