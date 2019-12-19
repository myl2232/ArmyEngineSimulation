#include "ArmyConstructionIndexSubModeController.h"
#include "ArmyGameInstance.h"
#include "ArmyViewportClient.h"
#include "ArmyToolsModule.h"
#include "ArmyToolBarBuilder.h"
#include "ArmyRoom.h"

#include "ArmyIndexSymbolOperation.h"
#include "ArmySymbolLeadLine.h"

FArmyConstructionIndexSubModeController::~FArmyConstructionIndexSubModeController()
{

}
void FArmyConstructionIndexSubModeController::Init()
{
	FloatWidget = MakeFloatWidget();

	FArmyDesignModeController::Init();
}
void FArmyConstructionIndexSubModeController::InitOperations()
{
	OperationMap.Add(HO_SYMBOLREGION, MakeShareable(new FArmyIndexSymbolOperation(E_ConstructionModel)));

	for (auto Operation : OperationMap)
	{
		Operation.Value->Init();
		Operation.Value->InitWidget(GVC->ViewportOverlayWidget);
		Operation.Value->EndOperationDelegate.BindRaw(this, &FArmyConstructionIndexSubModeController::EndOperation);
	}
}
void FArmyConstructionIndexSubModeController::BeginMode()
{
	FArmyDesignModeController::BeginMode();

	GVC->ViewportOverlayWidget->AddSlot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Top)
		.Padding(0, 4, 4, 0)
		[
			FloatWidget.ToSharedRef()
		];
	

	FArmyToolsModule::Get().GetMouseCaptureTool()->Reset();
	FArmyToolsModule::Get().GetMouseCaptureTool()->SetRefCoordinateSystem(FVector(ForceInitToZero), FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1));
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = true;

	//FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	//FArmyToolsModule::Get().GetRectSelectTool()->End();
	//FArmyToolsModule::Get().GetRectSelectTool()->EnableMultipleSelect(true);
	//FArmyToolsModule::Get().GetRectSelectTool()->Start();

	RefreshData();
}

bool FArmyConstructionIndexSubModeController::EndMode()
{
	GVC->ViewportOverlayWidget->RemoveSlot(FloatWidget.ToSharedRef());
	GGI->Window->DismissModalViewController();
	FArmyDesignModeController::EndMode();
	return true;
}
void FArmyConstructionIndexSubModeController::EndOperation()
{
	FArmyDesignModeController::EndOperation();
	FArmyToolsModule::Get().GetRectSelectTool()->Start();
}
void FArmyConstructionIndexSubModeController::SetDataContainner(const TSharedPtr<FArmyLayer> InLayer)
{
	LocalDataLayer = InLayer;
}
const TSharedPtr<FArmyLayer> FArmyConstructionIndexSubModeController::GetDataContainner() const
{
	return LocalDataLayer;
}
void FArmyConstructionIndexSubModeController::AddSymbolGroupData(const FGuid& InRoomId,const TSharedPtr<FIndexSymbolGroup> InSymbolGroup)
{
	SymbolGroupMap.Add(InRoomId, InSymbolGroup);
}
void FArmyConstructionIndexSubModeController::UpdateSymbol()
{
	TArray<FGuid> RoomIDArray;
	SymbolGroupMap.GetKeys(RoomIDArray);
	TArray<FObjectWeakPtr> RoomArray;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, RoomArray);
	for (auto RoomObj : RoomArray)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(RoomObj.Pin());
		if (RoomIDArray.Remove(Room->GetUniqueID()) == 0)
		{
			//new symbolgroup
			//获取房间的中心点
			FBox RoomBox = Room->GetBounds();
			FVector IndexPos = RoomBox.GetCenter();
			float MinDis = FLT_MAX;
			FVector FirstProjPos = FVector::ZeroVector;
			FVector SecondProjPos = FVector::ZeroVector;
			if (!Room->IsPointInRoom(IndexPos))
			{
				Room->CalPointToRoomLineMinDis(IndexPos, FirstProjPos, SecondProjPos);
				IndexPos = (FirstProjPos + SecondProjPos) / 2;
			}

			AddSymbolGroupData(Room->GetUniqueID(), MakeShareable(new FIndexSymbolGroup(IndexPos)));
		}
	}
	
	for (auto& RoomId : RoomIDArray)
	{
		SymbolGroupMap.Remove(RoomId);
	}
}
const TMap<FGuid, TSharedPtr<FIndexSymbolGroup>>& FArmyConstructionIndexSubModeController::GetSymbolGroupMap() const
{
	return SymbolGroupMap;
}
void FArmyConstructionIndexSubModeController::RefreshData()
{
	if (!LocalDataLayer.IsValid()) return;

	for (auto& ClassIt : LocalDataLayer->GetAllClass())
	{
		if (ClassIt.Value->IsVisible())
		{
			ClassIt.Value->ClearObjects();
		}
	}

	for (auto Obj : FArmySceneData::Get()->GetObjects(E_LayoutModel))
	{
		ObjectClassType ClassType = FArmyConstructionFunction::GetClassTypeByObject(Obj.Pin());
		TSharedPtr<FArmyClass> ObjClass = LocalDataLayer->GetOrCreateClass(ClassType, false);
		if (ObjClass.IsValid())
		{
			ObjClass->AddObject(Obj.Pin());
		}
	}
}
void FArmyConstructionIndexSubModeController::Clear()
{
}
void FArmyConstructionIndexSubModeController::Quit()
{
	EndMode();
}
void FArmyConstructionIndexSubModeController::CollectAllLinesAndPoints(TArray< TSharedPtr<FArmyLine> >& InOutLines, TArray< TSharedPtr<FArmyPoint> >& InOutPoints, TArray<TSharedPtr<FArmyCircle>>& InOutCircles)
{
	
}
TSharedPtr<SWidget> FArmyConstructionIndexSubModeController::MakeFloatWidget()
{
	return 
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Right)
		.VAlign(EVerticalAlignment::VAlign_Top)
		.Padding(0,40,20,0)
		[
			SNew(SBorder)
			.Padding(0)
			.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF191A1D"))
			.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
			[
				SNew(SBox)
				.WidthOverride(80)
				.HeightOverride(30)
				[
					SNew(SButton)
					.HAlign(EHorizontalAlignment::HAlign_Center)
					.VAlign(EVerticalAlignment::VAlign_Center)
					.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Orange"))
					.Text(FText::FromString(TEXT("生成立面图")))
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
					.OnClicked_Raw(this, &FArmyConstructionIndexSubModeController::FloatButtonClicked, 1)
				]
			]
		]
		+ SHorizontalBox::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Right)
		.VAlign(EVerticalAlignment::VAlign_Top)
		.Padding(0, 40, 40, 0)
		[
			SNew(SBox)
			.WidthOverride(80)
			.HeightOverride(30)
			[
				SNew(SButton)
				.HAlign(EHorizontalAlignment::HAlign_Center)
				.VAlign(EVerticalAlignment::VAlign_Center)
				.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("ComboBox.Button.Property"))
				.Text(FText::FromString(TEXT("取消")))
				.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
				.OnClicked_Raw(this, &FArmyConstructionIndexSubModeController::FloatButtonClicked, 0)
			]
		];
}
FReply FArmyConstructionIndexSubModeController::FloatButtonClicked(int32 InType)
{
	if (InType == 1)
	{
		SymbolGroupMap;
	}
	DelegateFloatButtonClicked.ExecuteIfBound(InType);
	return FReply::Handled();
}
TSharedPtr<SWidget> FArmyConstructionIndexSubModeController::MakeLeftPanelWidget()
{
	return nullptr;
}
TSharedPtr<SWidget> FArmyConstructionIndexSubModeController::MakeRightPanelWidget()
{
	return nullptr;
}

TSharedPtr<SWidget> FArmyConstructionIndexSubModeController::MakeToolBarWidget()
{
	return nullptr;
}

TSharedPtr<SWidget> FArmyConstructionIndexSubModeController::MakeSettingBarWidget()
{
	return nullptr;
}
void FArmyConstructionIndexSubModeController::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	if (CurrentSymbolRegion.IsValid())
	{
		CurrentSymbolRegion->Draw(PDI, View);
	}
	for (auto SymbolGroupIt : SymbolGroupMap)
	{
		SymbolGroupIt.Value->Draw(PDI, View);
	}
	if (LocalDataLayer.IsValid())
	{
		LocalDataLayer->Draw(PDI, View);
	}
	FArmyDesignModeController::Draw(View, PDI);
}
void FArmyConstructionIndexSubModeController::Draw(FViewport* InViewport, const FSceneView* View, FCanvas* SceneCanvas)
{
	FArmyDesignModeController::Draw(InViewport, View, SceneCanvas);
}
bool FArmyConstructionIndexSubModeController::InputKey(FViewport* Viewport, FKey Key, EInputEvent Event)
{
	if (!CurrentOperation.IsValid())
	{
		if (Event == IE_Pressed && (Key == EKeys::LeftMouseButton || Key == EKeys::RightMouseButton))
		{
			if (Key == EKeys::LeftMouseButton)
			{	
				FVector Pos(GVC->PixelToWorld(Viewport->GetMouseX(), Viewport->GetMouseY(), 0));
				Pos.Z = 0;
				if (CurrentSymbolRegion.IsValid())
				{
					TSharedPtr<FArmyEditPoint> SelectPoint = CurrentSymbolRegion->SelectPoint(Pos, GVC);
					if (SelectPoint.IsValid())
					{
						SetOperation(HO_SYMBOLREGION, XRArgument().ArgBoolean(true));
						CurrentOperation->SetSelected(CurrentSymbolRegion, SelectPoint);
						return true;
					}
				}
				

				{
					float newLength = 0.f;
					float newWidth = 0.f;
					float offset = 10.f; //选择框的偏移
					TSharedPtr<FArmyRoom> CurrentRoom; // 当前房间
					FVector NewCenterPos = FVector(ForceInitToZero);
					FVector RoomCenterPos = FVector(ForceInitToZero);
					for (auto SymbolGroupIt : SymbolGroupMap)
					{
						CurrentRoom = StaticCastSharedPtr<FArmyRoom>(FArmySceneData::Get()->GetObjectByGuid(E_HomeModel, SymbolGroupIt.Key).Pin());
						FBox roomBounds = CurrentRoom->GetBounds();
						RoomCenterPos = roomBounds.GetCenter();
						//为指引线设置房间信息
						SymbolGroupIt.Value->SymbolGroupLeadLine->SetCurrentRoomBoundBox(CurrentRoom->GetBounds());

						FArmySymbolLeadLine::ESymbolType CurrentSymbolType = FArmySymbolLeadLine::EST_NONE;
						if (SymbolGroupIt.Value->SymbolGroupLeadLine->Pick(Pos, CurrentSymbolType))
						{
							//@ 杨云鹤 获得当前屋子包围盒的 最大最小点
							
							auto m_VRoomSpaceMinPos = CurrentRoom->GetBounds().Min;
							auto m_VRoomSpaceMaxPos = CurrentRoom->GetBounds().Max;

							switch (CurrentSymbolType)
							{
							case FArmySymbolLeadLine::EST_LEFT:
								CurrentSymbolRegion = SymbolGroupIt.Value->SymbolLeftRectRegion;

								/**@ 杨云鹤 
								*	根据选取的方位，设置选择框数据，使其贴合墙壁
								*/////////////////////////////////////////////////////////////////////////
								NewCenterPos.X = SymbolGroupIt.Value->SymbolGroupLeadLine->GetBasePos().X;
								NewCenterPos.Y = RoomCenterPos.Y;
								CurrentSymbolRegion->SetBasePos(NewCenterPos);
								newLength = abs(m_VRoomSpaceMaxPos.Y - m_VRoomSpaceMinPos.Y) + offset;
								newWidth = abs(m_VRoomSpaceMinPos.X - NewCenterPos.X) + offset / 2;
								CurrentSymbolRegion->SetSelectRectData(newLength, newWidth);
			
								//////////////////////////////////////////////////////////////////////////

								if (CurrentSymbol.IsValid())
								{
									CurrentSymbol->SetState(OS_Normal);
								}
								CurrentSymbol = SymbolGroupIt.Value->SymbolLeft;
								CurrentSymbol->SetState(OS_Selected);
								break;
							case FArmySymbolLeadLine::EST_RIGHT:
								CurrentSymbolRegion = SymbolGroupIt.Value->SymbolRightRectRegion;

								NewCenterPos.X = SymbolGroupIt.Value->SymbolGroupLeadLine->GetBasePos().X;
								NewCenterPos.Y = RoomCenterPos.Y;
								CurrentSymbolRegion->SetBasePos(NewCenterPos);
								newLength = abs(m_VRoomSpaceMaxPos.Y - m_VRoomSpaceMinPos.Y) + offset;
								newWidth = abs(m_VRoomSpaceMaxPos.X - NewCenterPos.X) + offset / 2;
								CurrentSymbolRegion->SetSelectRectData(newLength, newWidth);
								if (CurrentSymbol.IsValid())
								{
									CurrentSymbol->SetState(OS_Normal);
								}
								CurrentSymbol = SymbolGroupIt.Value->SymbolRight;
								CurrentSymbol->SetState(OS_Selected);
								break;
							case FArmySymbolLeadLine::EST_TOP:
								CurrentSymbolRegion = SymbolGroupIt.Value->SymbolTopRectRegion;

								NewCenterPos.X = RoomCenterPos.X;
								NewCenterPos.Y = SymbolGroupIt.Value->SymbolGroupLeadLine->GetBasePos().Y;
								CurrentSymbolRegion->SetBasePos(NewCenterPos);
								newLength = abs(m_VRoomSpaceMaxPos.X - m_VRoomSpaceMinPos.X) + offset;
								newWidth = abs(m_VRoomSpaceMinPos.Y - NewCenterPos.Y) + offset / 2;
								CurrentSymbolRegion->SetSelectRectData(newLength, newWidth);
								if (CurrentSymbol.IsValid())
								{
									CurrentSymbol->SetState(OS_Normal);
								}
								CurrentSymbol = SymbolGroupIt.Value->SymbolTop;
								CurrentSymbol->SetState(OS_Selected);
								break;
							case FArmySymbolLeadLine::EST_BOTTOM:
								CurrentSymbolRegion = SymbolGroupIt.Value->SymbolBottomRectRegion;

								NewCenterPos.X = RoomCenterPos.X;
								NewCenterPos.Y = SymbolGroupIt.Value->SymbolGroupLeadLine->GetBasePos().Y;
								CurrentSymbolRegion->SetBasePos(NewCenterPos);
								newLength = abs(m_VRoomSpaceMaxPos.X - m_VRoomSpaceMinPos.X) + offset;
								newWidth = abs(m_VRoomSpaceMaxPos.Y - NewCenterPos.Y) + offset / 2;
								CurrentSymbolRegion->SetSelectRectData(newLength, newWidth);

								if (CurrentSymbol.IsValid())
								{
									CurrentSymbol->SetState(OS_Normal);
								}
								CurrentSymbol = SymbolGroupIt.Value->SymbolBottom;
								CurrentSymbol->SetState(OS_Selected);
								break;
							default:
								CurrentSymbolRegion = nullptr;
								if (CurrentSymbol.IsValid())
								{
									CurrentSymbol->SetState(OS_Normal);
									CurrentSymbol = nullptr;
								}
								break;
							}

							return true;
						}
						else if (SymbolGroupIt.Value->SymbolGroupLeadLine->GetTargetType(FArmySymbolLeadLine::EST_LEFT) && SymbolGroupIt.Value->SymbolLeft->IsSelected(Pos, GVC))
						{
							CurrentSymbolRegion = SymbolGroupIt.Value->SymbolLeftRectRegion;
							if (CurrentSymbol.IsValid())
							{
								CurrentSymbol->SetState(OS_Normal);
							}
							CurrentSymbol = SymbolGroupIt.Value->SymbolLeft;
							CurrentSymbol->SetState(OS_Selected);
							return true;
						}
						else if (SymbolGroupIt.Value->SymbolGroupLeadLine->GetTargetType(FArmySymbolLeadLine::EST_RIGHT) && SymbolGroupIt.Value->SymbolRight->IsSelected(Pos, GVC))
						{
							CurrentSymbolRegion = SymbolGroupIt.Value->SymbolRightRectRegion;
							if (CurrentSymbol.IsValid())
							{
								CurrentSymbol->SetState(OS_Normal);
							}
							CurrentSymbol = SymbolGroupIt.Value->SymbolRight;
							CurrentSymbol->SetState(OS_Selected);
							return true;
						}
						else if (SymbolGroupIt.Value->SymbolGroupLeadLine->GetTargetType(FArmySymbolLeadLine::EST_TOP) && SymbolGroupIt.Value->SymbolTop->IsSelected(Pos, GVC))
						{
							CurrentSymbolRegion = SymbolGroupIt.Value->SymbolTopRectRegion;
							if (CurrentSymbol.IsValid())
							{
								CurrentSymbol->SetState(OS_Normal);
							}
							CurrentSymbol = SymbolGroupIt.Value->SymbolTop;
							CurrentSymbol->SetState(OS_Selected);
							return true;
						}
						else if (SymbolGroupIt.Value->SymbolGroupLeadLine->GetTargetType(FArmySymbolLeadLine::EST_BOTTOM) && SymbolGroupIt.Value->SymbolBottom->IsSelected(Pos, GVC))
						{
							CurrentSymbolRegion = SymbolGroupIt.Value->SymbolBottomRectRegion;
							if (CurrentSymbol.IsValid())
							{
								CurrentSymbol->SetState(OS_Normal);
							}
							CurrentSymbol = SymbolGroupIt.Value->SymbolBottom;
							CurrentSymbol->SetState(OS_Selected);
							return true;
						}
						else
						{
							if (SymbolGroupIt.Value->EditPoint->Hover(Pos, GVC))
							{
								CurrentSymbolGroup = SymbolGroupIt.Value;
								return true;
							}
						}
					}
				}
			}

			CurrentSymbolRegion = nullptr;
			if (CurrentSymbol.IsValid())
			{
				CurrentSymbol->SetState(OS_Normal);
				CurrentSymbol = nullptr;
			}
		}
		else if (Event == IE_Released && Key == EKeys::LeftMouseButton)
		{
			if (CurrentSymbolGroup.IsValid())
			{
				CurrentSymbolGroup->EditPoint->SetState(FArmyPrimitive::OPS_Normal);

				TSharedPtr<FArmyRoom> CurrentRoom;
				for (auto symbolGroup : SymbolGroupMap)
				{
					//@ 杨云鹤 获得当前操作的房间
					if (symbolGroup.Value == CurrentSymbolGroup)
					{
						CurrentRoom = StaticCastSharedPtr<FArmyRoom>(FArmySceneData::Get()->GetObjectByGuid(E_HomeModel, symbolGroup.Key).Pin());
					}
					else
					{
						continue;
					}
					//@ 杨云鹤 符号组只有在在房间内的时候更新上次位置信息数据信息
					if (CurrentRoom->IsPointInRoom(symbolGroup.Value->EditPoint->GetPos()))
					{
						symbolGroup.Value->SymbolGroupLeadLine->SetBasePos(symbolGroup.Value->EditPoint->GetPos());
						CurrentSymbolGroup->SymbolGroupLeadLine->SetTargetPos(symbolGroup.Value->SymbolGroupLeadLine->GetBasePos());
					}
					else
					{
						CurrentSymbolGroup->SymbolGroupLeadLine->SetTargetPos(symbolGroup.Value->EditPoint->GetPos());
					}

				}
			}

			CurrentSymbolGroup = nullptr;
		}
	}
	
	return FArmyDesignModeController::InputKey(Viewport, Key, Event);
}

void FArmyConstructionIndexSubModeController::MouseMove(FViewport* Viewport, int32 X, int32 Y)
{
	FArmyDesignModeController::MouseMove(Viewport, X, Y);
}

bool FArmyConstructionIndexSubModeController::MouseDrag(FViewport* Viewport, FKey Key)
{
	if (CurrentSymbolGroup.IsValid())
	{
		FVector Pos(GVC->PixelToWorld(Viewport->GetMouseX(), Viewport->GetMouseY(), 0));
		Pos.Z = 0;
		CurrentSymbolGroup->Update(Pos);
	}
	return FArmyDesignModeController::MouseDrag(Viewport, Key);
}

void FArmyConstructionIndexSubModeController::Tick(float DeltaSeconds)
{
	FArmyDesignModeController::Tick(DeltaSeconds);
}
void FArmyConstructionIndexSubModeController::Load(TSharedPtr<FJsonObject> Data)
{
}

bool FArmyConstructionIndexSubModeController::Save(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter)
{
	return true;
}
void FArmyConstructionIndexSubModeController::Delete()
{

}