#include "ArmyDownLeadLabelOperation.h"
#include "ArmyDownLeadLabel.h"
#include "ArmyRectSelect.h"
#include "ArmyFurniture.h"
#include "HardModeData/EditArea/XRBaseArea.h"
#include "HardModeData/DrawStyle/XRBaseEditStyle.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyToolsModule.h"
#include "ArmySettings.h"

FArmyDownLeadLabelOperation::FArmyDownLeadLabelOperation(EModelType InBelongModel):FArmyOperation(InBelongModel)
{
	RectSelect = MakeShareable(new FArmyRectSelect);
	RectSelect->EnableMultipleSelect(false);
	RectSelect->AddFiltType(OT_RectArea);
	RectSelect->AddFiltType(OT_CircleArea);
	RectSelect->AddFiltType(OT_PolygonArea);
	RectSelect->AddFiltType(OT_WaterKnifeArea);
	RectSelect->AddFiltType(OT_RoomSpaceArea);
	RectSelect->AddFiltType(OT_ComponentBase);/** @ 梁晓菲 水电的物体类型都是ComponentBase */
	RectSelect->SetSelectUnit(FArmyRectSelect::Unit_Object);
}
void FArmyDownLeadLabelOperation::InitWidget(TSharedPtr<SOverlay> InParentWidget)
{
	ViewPortOverlay = InParentWidget;
}
void FArmyDownLeadLabelOperation::SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive)
{
	if (Object->GetType() == OT_DownLeadLabel)
	{
		CurrentDownLeadLabel = StaticCastSharedPtr<FArmyDownLeadLabel>(Object);
		OperationPoint = StaticCastSharedPtr<FArmyEditPoint>(InOperationPrimitive);
	}
}
void FArmyDownLeadLabelOperation::BeginOperation(XRArgument InArg)
{
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = true;
	FArmyToolsModule::Get().GetRectSelectTool()->End();

	CurrentArg = InArg;
	
	if (CurrentArg._ArgInt32 == 1)
	{
		FArmyToolsModule::Get().GetRectSelectTool()->End();

		CurrentOperationIndex = 1;
		RectSelect->Start();
		CurrentDownLeadLabel = MakeShareable(new FArmyDownLeadLabel);
		CurrentDownLeadLabel->Init(ViewPortOverlay);
		CurrentDownLeadLabel->SetTextSize(GXRSettings->GetAnnotationFontSize());
	}
}
void FArmyDownLeadLabelOperation::EndOperation()
{
	CurrentArg = XRArgument();
	CurrentDownLeadLabel = NULL;
	OperationPoint = NULL;
	CurrentOperationIndex = 0;
	RectSelect->Clear();
	RectSelect->End();
    FArmyOperation::EndOperation();
}
FObjectPtr FArmyDownLeadLabelOperation::GetOperationObject()
{ 
	return CurrentDownLeadLabel; 
}
TSharedPtr<FArmyEditPoint> FArmyDownLeadLabelOperation::GetOperationPoint()
{ 
	return OperationPoint; 
}
void FArmyDownLeadLabelOperation::Tick()
{

}
void FArmyDownLeadLabelOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (CurrentDownLeadLabel.IsValid())
	{
		CurrentDownLeadLabel->Draw(PDI, View);
	}
}
void FArmyDownLeadLabelOperation::DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, const FSceneView* View, FCanvas* Canvas)
{
	if (CurrentDownLeadLabel.IsValid())
	{
		CurrentDownLeadLabel->DrawHUD(InViewPortClient,ViewPort,View,Canvas);
	}
}
bool FArmyDownLeadLabelOperation::InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event)
{
	if (InViewPort->KeyState(EKeys::LeftMouseButton) && Event == IE_Pressed)
	{
		if (CurrentArg._ArgInt32 == 1)
		{
			if (CurrentOperationIndex == 1)
			{
				TArray<FObjectWeakPtr> TempObjArray;
				OnGetObjectArrayDelegate.ExecuteIfBound(TempObjArray);
				/* @梁晓菲 对物体排序*/
				TempObjArray.Sort([&](const FObjectWeakPtr& A, const FObjectWeakPtr& B)
				{
					return A.Pin()->GetType() < B.Pin()->GetType();
				});

				RectSelect->LeftMouseButtonPress(InViewPortClient, InViewPort->GetMouseX(), InViewPort->GetMouseY(), TempObjArray);

				for (auto ObjIt : RectSelect->GetSelectedObjects())
				{
					FText TextContent;
					switch (ObjIt.Pin()->GetType())
					{
					case OT_ComponentBase:/** @ 梁晓菲 水电的物体类型都是ComponentBase */
						//if (CurrentArg._ArgUint32 == 5)
						//{
						//	FArmyFurniture* Furniture = ObjIt.Pin()->AsassignObj<FArmyFurniture>();
						//	if (Furniture)
						//	{
						//		TextContent = FText::FromString(TEXT("高度：") + FString::FromInt(Furniture->Altitude * 10) + TEXT("mm"));
						//	}
						//}
						break;
					case OT_RectArea: //矩形绘制
					case OT_CircleArea: //矩形绘制
					case OT_PolygonArea: //矩形绘制
					case OT_WaterKnifeArea://水刀拼花
					case OT_RoomSpaceArea:
					{
						FArmyBaseArea* AreaObj = ObjIt.Pin()->AsassignObj<FArmyBaseArea>();
						if (AreaObj)
						{
							AreaType AT = AreaObj->SelectAreaType(FVector(InViewPort->GetMouseX(), InViewPort->GetMouseY(),0), InViewPortClient);
							if (AT == M_OutArea)
							{
								break;
							}

							const TSharedPtr<FArmyBaseEditStyle> AreaStyle = AreaObj->GetStyle(AT);
							if (!AreaStyle.IsValid())
							{
								break;
							}

							FString CraftStr;
							FString SizeStr = FString::FromInt(AreaStyle->GetMainTexWidth()) + TEXT("*") + FString::FromInt(AreaStyle->GetMainTexHeight());
							
							switch (int32(AreaStyle->GetEditType()))
							{
							case S_ContinueStyle:
							{
								CraftStr = TEXT("连续直铺");
								break;
							}
							case S_HerringBoneStyle:
							{
								CraftStr = TEXT("人字铺");
								break;
							}
							case S_TrapeZoidStyle:
							{
								CraftStr = TEXT("三六九铺");
								break;
							}
							case S_WhirlwindStyle:
							{
								CraftStr = TEXT("旋风铺");
								break;
							}
							case S_WorkerStyle:
							{
								CraftStr = TEXT("工字铺");
								break;
							}
							case S_CrossStyle:
							{
								CraftStr = TEXT("交错铺");
								break;
							}
							case S_IntervalStyle:
							{
								CraftStr = TEXT("间隔铺");
								break;
							}
							case S_SeamlessStyle:
							{
								CraftStr = TEXT("无缝铺");
								break;
							}
							case S_SlopeContinueStyle:
							{
								CraftStr = TEXT("斜铺");
								break;
							}
							default:
								break;
							}

							if (CurrentArg._ArgUint32 == 1)
							{
								TextContent = FText::FromString(TEXT("规格：") + SizeStr);
							}
							else if (CurrentArg._ArgUint32 == 2)
							{
								TextContent = FText::FromString(TEXT("材质："));
							}
							else if (CurrentArg._ArgUint32 == 3)
							{
								TextContent = FText::FromString(TEXT("注释：")); 
							}
							else if (CurrentArg._ArgUint32 == 4)
							{
								TextContent = FText::FromString(TEXT("工艺：") + CraftStr);
							}
							else if (CurrentArg._ArgUint32 == 5)
							{
								if (AreaObj->SurfaceType == 0)//地
								{
									TextContent = FText::FromString(TEXT("室内净高：") + FString::FromInt(AreaObj->GetExtrusionHeight() * 10) + TEXT("mm"));
								}
								else if (AreaObj->SurfaceType == 1)//墙
								{
									TextContent = FText::FromString(TEXT("离地高度：") + FString::FromInt(AreaObj->GetExtrusionHeight() * 10) + TEXT("mm"));
								}
								else if(AreaObj->SurfaceType == 2)//顶
								{
									TextContent = FText::FromString(TEXT("下返：") + FString::FromInt(AreaObj->GetExtrusionHeight() * 10) + TEXT("mm"));
								}
							}
						}
					}
						break;
					default:
						break;
					}

					CurrentDownLeadLabel->SetLabelContent(TextContent);

					break;
				}
				FVector2D CapturePoint = FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint();
				FArmyToolsModule::Get().GetMouseCaptureTool()->SetCurrentPoint(CapturePoint);
				CurrentDownLeadLabel->SetWorldPosition(FVector(CapturePoint, 0));
				CurrentOperationIndex = 2;
			}
			else if (CurrentOperationIndex == 2)
			{
				FVector2D CapturePoint = FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint();
				FArmyToolsModule::Get().GetMouseCaptureTool()->SetCurrentPoint(CapturePoint);
				CurrentDownLeadLabel->SetLeadPoint(FVector(CapturePoint, 0));
				CurrentOperationIndex = 3;
			}
			else if (CurrentOperationIndex == 3)
			{
				FVector2D CapturePoint = FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint();
				FArmyToolsModule::Get().GetMouseCaptureTool()->SetCurrentPoint(CapturePoint);
				CurrentDownLeadLabel->SetEndPoint(FVector(CapturePoint, 0));
				if (CurrentDownLeadLabel->GetLabelContent().IsEmpty())
				{
					CurrentDownLeadLabel->StartEditText();
				}
				FArmySceneData::Get()->Add(CurrentDownLeadLabel, XRArgument(1).ArgBoolean(true).ArgUint32(BelongModel));
				EndOperation();
			}
		}
		else if (CurrentArg._ArgInt32 == 2)
		{
			EndOperation();
		}
	}
    return false;
}
void FArmyDownLeadLabelOperation::MouseMove(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y)
{
	if (CurrentArg._ArgInt32 == 1)
	{
		if (CurrentOperationIndex == 2)
		{
			FVector2D CapturePoint = FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint();
			CurrentDownLeadLabel->SetLeadPoint(FVector(CapturePoint,0));
		}
		else if (CurrentOperationIndex == 3)
		{
			FVector2D CapturePoint = FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint();
			CurrentDownLeadLabel->SetEndPoint(FVector(CapturePoint,0));
		}
	}
	else if (CurrentArg._ArgInt32 == 2)
	{
		if (OperationPoint.IsValid())
		{
			FVector2D CapturePoint = FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint();
			OperationPoint->SetPos(FVector(CapturePoint,0));
			CurrentDownLeadLabel->Refresh();
		}
	}
}