#include "UtilityTools/SCTShapeUtilityTools.h"
#include "SCTShapeData.h"
#include "SCTShapeManager.h"
#include "SCTShape.h"
#include "SCTBoardShape.h"
#include "SCTModelShape.h"
#include "SCTBaseShape.h"
#include "SCTInsideShape.h"
#include "SCTUnitShape.h"
#include "SCTDoorGroup.h"
#include "SCTCabinetShape.h"
#include "SCTSpaceShape.h"
#include "SCTFrameShape.h"
#include "SCTDoorSheet.h"
#include "Actor/SCTBoardActor.h"
#include "SCTSlidingDrawerShape.h"
#include "GameFramework/Actor.h"
#include "MaterialManager.h"
#include "SCTEmbededElectricalGroup.h"
#include "SCTAttribute.h"
#include "SCTEmbededElectricalGroup.h"
#include "SCTVeneeredSideBoardGroup.h"


TSharedPtr<FSCTShape> FSCTShapeUtilityTool::ReplaceDoorGroupAndKeepMetalTransform(FSCTShape* InSrcDoorSheetShape, FSCTShape* InDesDoorGroup)
{		
	bool bIsSideHungDoor = InSrcDoorSheetShape && InSrcDoorSheetShape->GetParentShape() && InSrcDoorSheetShape->GetParentShape()->GetShapeType() == ST_SideHungDoor;
	bIsSideHungDoor = bIsSideHungDoor && InDesDoorGroup && InDesDoorGroup->GetShapeType() == ST_SideHungDoor;
	bool bIsDrawer = InSrcDoorSheetShape && InSrcDoorSheetShape->GetParentShape() && InSrcDoorSheetShape->GetParentShape()->GetShapeType() == ST_DrawerDoor;
	bIsDrawer = bIsDrawer && InDesDoorGroup && InDesDoorGroup->GetShapeType() == ST_DrawerDoor;
	bool bIsSlidingDoor = InSrcDoorSheetShape && InSrcDoorSheetShape->GetParentShape() && InSrcDoorSheetShape->GetParentShape()->GetShapeType() == ST_SlidingDoor;
	bIsSlidingDoor = bIsSlidingDoor && InDesDoorGroup && InDesDoorGroup->GetShapeType() == ST_SlidingDoor;
	check(bIsSideHungDoor || bIsDrawer || bIsSlidingDoor);
	// 判断尺寸是否合适
	auto GetDoorSheetRange = [](const TSharedPtr<FShapeAttribute> & InAttri)->TPair<float, float>
	{
		float MinValue = FCString::Atof(*InAttri->GetAttributeStr());
		float MaxValue = MinValue;
		if (SAT_NumberRange == InAttri->GetAttributeType())
		{
			TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(InAttri);
			MinValue = NumberRangeAttri->GetMinValue();
			MaxValue = NumberRangeAttri->GetMaxValue();
		}
		return TPair<float, float>(MinValue, MaxValue);
	};

	if (bIsSideHungDoor)
	{	
		// 上翻门不允许与平开们进行替换		
		{			
			FSideHungDoor * DesSideHungGroup = StaticCast<FSideHungDoor*>(InDesDoorGroup);
			FVariableAreaDoorSheet * VariableAreaDoorSheet = StaticCast<FVariableAreaDoorSheet *>(InSrcDoorSheetShape);
			if (VariableAreaDoorSheet->GetOpenDoorDirection() == FVariableAreaDoorSheet::EDoorOpenDirection::E_Top)
			{				
				if (DesSideHungGroup->GetDoorSheets()[0]->GetOpenDoorDirection() != FVariableAreaDoorSheet::EDoorOpenDirection::E_Top)
				{
					return nullptr;
				}
			}
			if(DesSideHungGroup->GetDoorSheets()[0]->GetOpenDoorDirection() == FVariableAreaDoorSheet::EDoorOpenDirection::E_Top)
			{
				if (VariableAreaDoorSheet->GetOpenDoorDirection() != FVariableAreaDoorSheet::EDoorOpenDirection::E_Top)
				{
					return nullptr;
				}
			}
		}
		
		{
			const float CurDoorSeetWidth = InSrcDoorSheetShape->GetShapeWidth();
			TPair<float, float> Range = GetDoorSheetRange(StaticCast<FSideHungDoor*>(InDesDoorGroup)->GetDoorSheets()[0]->GetShapeWidthAttri());
			if (CurDoorSeetWidth > Range.Value || CurDoorSeetWidth < Range.Key)
			{
				return nullptr;
			}
		}
		{
			const float CurDoorSeetHeight = InSrcDoorSheetShape->GetShapeHeight();
			TPair<float, float> Range = GetDoorSheetRange(StaticCast<FSideHungDoor*>(InDesDoorGroup)->GetDoorSheets()[0]->GetShapeHeightAttri());
			if (CurDoorSeetHeight > Range.Value || CurDoorSeetHeight < Range.Key)
			{
				return nullptr;
			}
		}

		// 从给定的门板，找到对应的顶层空间
		auto GetSpecialTopSpaceShape = [](FSCTShape * InShape)->FSCTShape *
		{
			FSCTShape * RetShape = nullptr;
			do
			{
				if (!InShape) break;
				if (InShape->GetShapeType() != ST_Space) break;
				FSpaceShape * SpaceShape = StaticCast<FSpaceShape*>(InShape);
				if (SpaceShape->IsContainDoorGroup() == false) break;
				RetShape = SpaceShape;

			} while (false);
			return 	RetShape;
		};
		FSCTShape * ToFindShape = InSrcDoorSheetShape;
		do
		{
			if (!ToFindShape) break;
			if (GetSpecialTopSpaceShape(ToFindShape))
			{
				break;
			}
			ToFindShape = ToFindShape->GetParentShape();

		} while (true);
		// 查找选中门板在门组中的索引
		FSideHungDoor * SideHungDoor = StaticCast<FSideHungDoor *>(InSrcDoorSheetShape->GetParentShape());
		// 上翻门与平开们不允许进行替换		

		int32 SrcDoorSheetIndexInDoorGroup = -1;
		for (int32 Index = 0; Index < SideHungDoor->GetDoorSheetNum(); ++Index)
		{
			if (SideHungDoor->GetDoorSheets()[Index].Get() == InSrcDoorSheetShape)
			{
				SrcDoorSheetIndexInDoorGroup = Index;
				break;
			}
		}
		check(SrcDoorSheetIndexInDoorGroup != -1);

		// 确保根据InSrcDoorSheetShape一定可以找到对应顶层空间
		check(ToFindShape && ToFindShape->GetShapeType() == ST_Space);
		FSpaceShape * SpaceShape = StaticCast<FSpaceShape *>(ToFindShape);
		TSharedPtr<FSideHungDoor> SrcSideHungDoor = StaticCastSharedPtr<FSideHungDoor>(SpaceShape->GetDoorShape());

		// 把手位置替换 
		TSharedPtr<FSideHungDoor> DesSideHungDoorGroup = MakeShareable(new FSideHungDoor);
		InDesDoorGroup->CopyTo(DesSideHungDoorGroup.Get());
		DesSideHungDoorGroup->GetCurrentHandleInfo().Location = SrcSideHungDoor->GetCurrentHandleInfo().Location;
		if (SrcSideHungDoor->GetHanleType() == FSideHungDoor::EHandleMetalType::E_MT_None)
		{
			DesSideHungDoorGroup->SetHanleType(FSideHungDoor::EHandleMetalType::E_MT_None);
		}
		// 原始门板是明装拉手或者封边拉手，则需要先确认目标门板是否也有该类型的拉手
		else if (SrcSideHungDoor->GetHanleType() != FSideHungDoor::EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND)
		{
			const EMetalsType ToMatchType = SrcSideHungDoor->GetHanleType() == FSideHungDoor::EHandleMetalType::E_MT_SHAKE_HANDS ?
				EMetalsType::MT_SHAKE_HANDS : EMetalsType::MT_SEALING_SIDE_HANDLE;
			const TMap<int32, TSharedPtr<FSCTShape>> & MapRef = DesSideHungDoorGroup->GetDoorSheets()[0]->GetOptionShapeMap();
			bool bIsMatch = false;
			bool bHasHanle = false;
			for (const auto & Ref : MapRef)
			{
				if (Ref.Value->GetShapeType() != ST_Accessory) continue;
				if (Ref.Value->GetShapeCategory() == StaticCast<int32>(ToMatchType))
				{
					bIsMatch = true;
					bHasHanle = true;
					break;
				}
				if (Ref.Value->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SHAKE_HANDS) ||
					Ref.Value->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SEALING_SIDE_HANDLE))
				{
					bHasHanle = true;
				}
			}
			if (bIsMatch)
			{
				DesSideHungDoorGroup->SetHanleType(SrcSideHungDoor->GetHanleType());
			}
			else
			{
				if (bHasHanle)
				{
					const FSideHungDoor::EHandleMetalType DesType = SrcSideHungDoor->GetHanleType() == FSideHungDoor::EHandleMetalType::E_MT_SEALING_SIDE_HANDLE ?
						FSideHungDoor::EHandleMetalType::E_MT_SHAKE_HANDS : FSideHungDoor::EHandleMetalType::E_MT_SEALING_SIDE_HANDLE;

					DesSideHungDoorGroup->SetHanleType(DesType);
				}
				else
				{
					DesSideHungDoorGroup->SetHanleType(FSideHungDoor::EHandleMetalType::E_MT_None);
				}
			}
		}
		else
		{

			// 判断是否有目标门组是否具有与当前门组相同的箱体拉手
			const int32 ShapeId = SrcSideHungDoor->GetCurrentHandleInfo().ID;
			bool bFindLateralBox = false;
			for (auto & Ref : DesSideHungDoorGroup->GetOptionShapeMap())
			{
				if (Ref.Key == ShapeId &&  Ref.Value->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_LATERAL_BOX_SHAKE_HAND))
				{
					bFindLateralBox = true;
					break;
				}
			}
			if (!bFindLateralBox)
			{
				return nullptr;
			}

			DesSideHungDoorGroup->SetCurrentHandleInfo(SrcSideHungDoor->GetCurrentHandleInfo());
			DesSideHungDoorGroup->SetHanleType(SrcSideHungDoor->GetHanleType());
		}
		
		bool bResult = false;
		// 处理插角封板
		{
			const FSideHungDoor::EFrontBoardPositionType FrontBoardPositionType = SideHungDoor->GetFrontBoardPositionType();
			const FSideHungDoor::EFrontBoardLinkageRule FrontBoardLinkageRule = SideHungDoor->GetFrontBoardLinkageRule();
			const int32 FrontBoardSubstarteId = SideHungDoor->GetFrontBoardSubstrate();
			const int32 FrontBoardEdgeBading = SideHungDoor->GetFrontBoardEdgeBanding();
			const float FrontBoardWidth = SideHungDoor->GetFrontBoardWidth();
			const float FrontBoardDepth = SideHungDoor->GetFrontBoardDepth();
			const float FrontBoardMinWidth = SideHungDoor->GetFrontBoardMinWidth();
			const float FrontBoardTopExtern = SideHungDoor->GetFrontBoardTopExtern();
			const float FrontBoardBottomExtern = SideHungDoor->GetFrontBoardBottomExtern();
			const float FrontBoardCornerCutWidth = SideHungDoor->GetFrontBoardCornerCutWidth();
			const float FrontBoardCornerCutHeight = SideHungDoor->GetFrontBoardCornerCutHeight();
			const FDoorSheetShapeBase::FDoorPakMetaData FrontBoardMaterial = DesSideHungDoorGroup->GetDoorSheets()[0]->GetMaterial();

			// 处理帮板
			const int32 AssistBoardSubstrateId = SideHungDoor->GetAssistBoardSubstrate();
			const int32 AssistBoardEdgeBaningId = SideHungDoor->GetAssistBoardEdgeBanding();
			const float AssistBoardWidth = SideHungDoor->GetAssistBoardWidth();
			const float AssistBoardHegith = SideHungDoor->GetAssistBoardHeight();
			const float AssistBoardDepth = SideHungDoor->GetAssistBoardDepth();
			const float AssistBoardTopExtern = SideHungDoor->GetAssistBoardTopExtern();
			const float AssistBoardBottomExtern = SideHungDoor->GetAssistBoardBottomExtern();
			const float AssistBoardLengthToFrontBoard = SideHungDoor->GetAssistBoardLengthToFrontBoard();
			const FDoorSheetShapeBase::FDoorPakMetaData AssistBoardMaterial = DesSideHungDoorGroup->GetDoorSheets()[0]->GetMaterial();


			const FSideHungDoor::EFrontBoardType FrontBoardType = SideHungDoor->GetFrontBoardType();
			const FSideHungDoor::EFrontBoardCornerType FrontBoardCornerType = SideHungDoor->GetFrontBoardCornerType();

			const float SrcDoorWidth = SideHungDoor->GetShapeWidth();
			const float SrcDoorHeight = SideHungDoor->GetShapeDepth();
			//置换门板
			SpaceShape->RemoveDoorGroup();

			if (DesSideHungDoorGroup->GetHanleType() == FSideHungDoor::EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND)
			{
				DesSideHungDoorGroup->ReBuildHandle(DesSideHungDoorGroup->GetCurrentHandleInfo().ID);
			}
			DesSideHungDoorGroup->SetShapeWidth(SrcDoorWidth);
			DesSideHungDoorGroup->SetShapeHeight(SrcDoorHeight);
			DesSideHungDoorGroup->SetFrontBoardPositionType(FrontBoardPositionType);
			DesSideHungDoorGroup->SetFrontBoardLinkageRule(FrontBoardLinkageRule);
			DesSideHungDoorGroup->SetFrontBoardSubstrate(FrontBoardSubstarteId);
			DesSideHungDoorGroup->SetFrontBoardEdgeBanding(FrontBoardEdgeBading);
			DesSideHungDoorGroup->SetFrontBoardWidth(FrontBoardWidth);
			DesSideHungDoorGroup->SetFrontBoardDepth(FrontBoardDepth);
			DesSideHungDoorGroup->SetFrontBoardMinWidth(FrontBoardMinWidth);
			DesSideHungDoorGroup->SetFrontBoardTopExtern(FrontBoardTopExtern);
			DesSideHungDoorGroup->SetFrontBoardBottomExtern(FrontBoardBottomExtern);
			DesSideHungDoorGroup->SetFrontBoardCornerCutWidth(FrontBoardCornerCutWidth);
			DesSideHungDoorGroup->SetFrontBoardCornerCutHeight(FrontBoardCornerCutHeight);


			// 处理帮板
			{
				DesSideHungDoorGroup->SetAssistBoardSubstrate(AssistBoardSubstrateId);
				DesSideHungDoorGroup->SetAssistBoardEdgeBanding(AssistBoardEdgeBaningId);
				DesSideHungDoorGroup->SetAssistBoardWidth(AssistBoardWidth);
				DesSideHungDoorGroup->SetAssistBoardHeight(AssistBoardHegith);
				DesSideHungDoorGroup->SetAssistBoardDepth(AssistBoardDepth);
				DesSideHungDoorGroup->SetAssistBoardTopExtern(AssistBoardTopExtern);
				DesSideHungDoorGroup->SetAsssistBoardBottomExtern(AssistBoardBottomExtern);
				DesSideHungDoorGroup->SetAssistBoardLengthToFrontBoard(AssistBoardLengthToFrontBoard);

			}

			DesSideHungDoorGroup->SetFrontBoardMaterial(FrontBoardMaterial);
			DesSideHungDoorGroup->SetAssistBoardMaterial(AssistBoardMaterial);
			DesSideHungDoorGroup->SetFrontBoardType(FrontBoardType);
			DesSideHungDoorGroup->SetFrontBoardCornerType(FrontBoardCornerType);
			bResult = SpaceShape->AddSideHungDoor(DesSideHungDoorGroup, false);
			DesSideHungDoorGroup->GetShapeCopyFromGuid() = SideHungDoor->GetShapeCopyFromGuid();			

		}
		if (bResult)
		{
			// 保证替换前后，门组内部门板数量一致
			DesSideHungDoorGroup->SetDoorSheetNum(SrcSideHungDoor->GetDoorSheetNum());
			check(DesSideHungDoorGroup->GetDoorSheetNum() == SrcSideHungDoor->GetDoorSheetNum());
			// 门组替换的时候,保留之前门组的外盖信息，位置信息	
			DesSideHungDoorGroup->SetUpCorverType(SrcSideHungDoor->GetUpCorverType());
			DesSideHungDoorGroup->SetDownCorverType(SrcSideHungDoor->GetDownCorverType());
			DesSideHungDoorGroup->SetLeftCorverType(SrcSideHungDoor->GetLeftCorverType());
			DesSideHungDoorGroup->SetRightCorverType(SrcSideHungDoor->GetRightCorverType());
			DesSideHungDoorGroup->SetBackGapValue(SrcSideHungDoor->GetBackGapValue());
			DesSideHungDoorGroup->SetUpDownGapValue(SrcSideHungDoor->GetUpDownGapValue());
			DesSideHungDoorGroup->SetLeftRightGapValue(SrcSideHungDoor->GetLeftRightGapValue());
			DesSideHungDoorGroup->SetUpExtensionValue(SrcSideHungDoor->GetUpExtensionValue());
			DesSideHungDoorGroup->SetDownExtensionValue(SrcSideHungDoor->GetDownExtensionValue());
			DesSideHungDoorGroup->SetLeftExtensionValue(SrcSideHungDoor->GetLeftExtensionValue());
			DesSideHungDoorGroup->SetRightExtensionValue(SrcSideHungDoor->GetRightExtensionValue());

			FCabinetShape* CurCabinetShape = SpaceShape->GetCabinetShapeData();
			CurCabinetShape->UpdateCabinet();
		}
		if (DesSideHungDoorGroup->GetHanleType() == FSideHungDoor::EHandleMetalType::E_MT_SEALING_SIDE_HANDLE ||
			DesSideHungDoorGroup->GetHanleType() == FSideHungDoor::EHandleMetalType::E_MT_SHAKE_HANDS)
		{
			for (int32 Index = 0; Index < DesSideHungDoorGroup->GetDoorSheetNum(); ++Index)
			{
				if (SrcSideHungDoor->GetDoorSheets()[Index]->IsShakeHanlePositionValid())
				{
					const FVariableAreaDoorSheet::FSShakeHanle ShakeHandlePositon = SrcSideHungDoor->GetDoorSheets()[Index]->GetShakeHanleTypePosition();
					DesSideHungDoorGroup->GetDoorSheets()[Index]->GetShakeHanleTypePosition() = ShakeHandlePositon;
				}
				if (SrcSideHungDoor->GetDoorSheets()[Index]->IsSealingSideHandleValid())
				{
					const FVariableAreaDoorSheet::FSSealingSideHandle SealingSideHanlePosition = SrcSideHungDoor->GetDoorSheets()[Index]->GetSealingSideHandlePosition();
					DesSideHungDoorGroup->GetDoorSheets()[Index]->GetSealingSideHandlePosition() = SealingSideHanlePosition;
				}
				DesSideHungDoorGroup->GetDoorSheets()[Index]->GetCurrentHandleInfo().Location = MakeShareable(new FDoorHanlerDefaultLocation);
				bool bIsShankeHanle = DesSideHungDoorGroup->GetHanleType() == FSideHungDoor::EHandleMetalType::E_MT_SHAKE_HANDS;
				if (bIsShankeHanle)
				{
					if (DesSideHungDoorGroup->GetDoorSheets()[Index]->IsShakeHanlePositionValid())
					{
						const FVariableAreaDoorSheet::FSShakeHanle & ShakeHandlePositon = DesSideHungDoorGroup->GetDoorSheets()[Index]->GetShakeHanleTypePosition();
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DesSideHungDoorGroup->GetDoorSheets()[Index]->GetCurrentHandleInfo().Location)->Hpos = ShakeHandlePositon.HPositionType;
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DesSideHungDoorGroup->GetDoorSheets()[Index]->GetCurrentHandleInfo().Location)->HposVal = ShakeHandlePositon.HPositionValue;
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DesSideHungDoorGroup->GetDoorSheets()[Index]->GetCurrentHandleInfo().Location)->Vpos = ShakeHandlePositon.VPositionType;
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DesSideHungDoorGroup->GetDoorSheets()[Index]->GetCurrentHandleInfo().Location)->VposVAl = ShakeHandlePositon.VPositionValue;
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DesSideHungDoorGroup->GetDoorSheets()[Index]->GetCurrentHandleInfo().Location)->RotateDirection = ShakeHandlePositon.RotateDirection;
					}
					else
					{
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DesSideHungDoorGroup->GetDoorSheets()[Index]->GetCurrentHandleInfo().Location)->Hpos = 3;
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DesSideHungDoorGroup->GetDoorSheets()[Index]->GetCurrentHandleInfo().Location)->HposVal = 0.0f;
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DesSideHungDoorGroup->GetDoorSheets()[Index]->GetCurrentHandleInfo().Location)->Vpos = 3;
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DesSideHungDoorGroup->GetDoorSheets()[Index]->GetCurrentHandleInfo().Location)->VposVAl = 0.0f;
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DesSideHungDoorGroup->GetDoorSheets()[Index]->GetCurrentHandleInfo().Location)->RotateDirection = 0;
					}
				}
				else
				{
					if (DesSideHungDoorGroup->GetDoorSheets()[Index]->IsSealingSideHandleValid())
					{
						const FVariableAreaDoorSheet::FSSealingSideHandle & SealingHandlePositon = DesSideHungDoorGroup->GetDoorSheets()[Index]->GetSealingSideHandlePosition();
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DesSideHungDoorGroup->GetDoorSheets()[Index]->GetCurrentHandleInfo().Location)->Vpos = SealingHandlePositon.VPostionType;
					}
					else
					{
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(DesSideHungDoorGroup->GetDoorSheets()[Index]->GetCurrentHandleInfo().Location)->Vpos = 1;
					}
				}
				DesSideHungDoorGroup->GetDoorSheets()[Index]->ReBuildHandle();
				if (bIsShankeHanle)
				{
					DesSideHungDoorGroup->GetDoorSheets()[Index]->RecalHandlePosition();
				}
			}
		}		
		DesSideHungDoorGroup->UpdateDoorGroup();
		return  DesSideHungDoorGroup->GetDoorSheets()[SrcDoorSheetIndexInDoorGroup];
	}
	else if(bIsDrawer)
	{
		{
			const float CurDoorSeetWidth = InSrcDoorSheetShape->GetShapeWidth();
			TPair<float, float> Range = GetDoorSheetRange(StaticCast<FDrawerDoorShape*>(InDesDoorGroup)->GetDrawDoorSheet()->GetShapeWidthAttri());
			if (CurDoorSeetWidth > Range.Value || CurDoorSeetWidth < Range.Key)
			{
				return nullptr;
			}
		}
		{
			const float CurDoorSeetHeight = InSrcDoorSheetShape->GetShapeHeight();
			TPair<float, float> Range = GetDoorSheetRange(StaticCast<FDrawerDoorShape*>(InDesDoorGroup)->GetDrawDoorSheet()->GetShapeHeightAttri());
			if (CurDoorSeetHeight > Range.Value || CurDoorSeetHeight < Range.Key)
			{
				return nullptr;
			}
		}
		// 删除已有的抽面，加入新的抽面
		FSlidingDrawerShape* SlidingDrawerShape =  StaticCast<FSlidingDrawerShape*>(InSrcDoorSheetShape->GetParentShape()->GetParentShape());
		// 把手位置替换 
		TSharedPtr<FDrawerDoorShape> DesDrawerDoorShape = MakeShareable(new FDrawerDoorShape);
		InDesDoorGroup->CopyTo(DesDrawerDoorShape.Get());
		FDrawerDoorShape * SrcDrawerDoorShape = StaticCast<FDrawerDoorShape*>(InSrcDoorSheetShape->GetParentShape());
		DesDrawerDoorShape->GetDefaultHandleInfo().Location = SrcDrawerDoorShape->GetDefaultHandleInfo().Location;
		if (SrcDrawerDoorShape->GetDoorGroupHanleType() == FDrawerDoorShape::EHandleMetalType::E_MT_None)
		{
			DesDrawerDoorShape->SetDoorGroupHanleType(FDrawerDoorShape::EHandleMetalType::E_MT_None);
		}
		// 原始门板是明装拉手或者封边拉手，则需要先确认目标门板是否也有该类型的拉手
		else if (SrcDrawerDoorShape->GetDoorGroupHanleType() != FDrawerDoorShape::EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND)
		{
			const EMetalsType ToMatchType = SrcDrawerDoorShape->GetDoorGroupHanleType() == FDrawerDoorShape::EHandleMetalType::E_MT_SHAKE_HANDS ?
				EMetalsType::MT_SHAKE_HANDS : EMetalsType::MT_SEALING_SIDE_HANDLE;
			const TMap<int32, TSharedPtr<FSCTShape>> & MapRef = DesDrawerDoorShape->GetDrawDoorSheet()->GetOptionShapeMap();
			bool bIsMatch = false;
			bool bHasHanle = false;
			for (const auto & Ref : MapRef)
			{
				if (Ref.Value->GetShapeType() != ST_Accessory) continue;
				if (Ref.Value->GetShapeCategory() == StaticCast<int32>(ToMatchType))
				{
					bIsMatch = true;
					bHasHanle = true;
					break;
				}
				if (Ref.Value->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SHAKE_HANDS) ||
					Ref.Value->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SEALING_SIDE_HANDLE))
				{
					bHasHanle = true;
				}
			}
			if (bIsMatch)
			{
				DesDrawerDoorShape->SetDoorGroupHanleType(SrcDrawerDoorShape->GetDoorGroupHanleType());
			}
			else
			{
				if (bHasHanle)
				{
					const FDrawerDoorShape::EHandleMetalType DesType = SrcDrawerDoorShape->GetDoorGroupHanleType() == FDrawerDoorShape::EHandleMetalType::E_MT_SEALING_SIDE_HANDLE ?
						FDrawerDoorShape::EHandleMetalType::E_MT_SHAKE_HANDS : FDrawerDoorShape::EHandleMetalType::E_MT_SEALING_SIDE_HANDLE;

					DesDrawerDoorShape->SetDoorGroupHanleType(DesType);
				}
				else
				{
					DesDrawerDoorShape->SetDoorGroupHanleType(FDrawerDoorShape::EHandleMetalType::E_MT_None);
				}
			}
		}
		else
		{

			// 判断是否有目标门组是否具有与当前门组相同的箱体拉手
			const int32 ShapeId = SrcDrawerDoorShape->GetDefaultHandleInfo().ID;
			bool bFindLateralBox = false;
			for (auto & Ref : DesDrawerDoorShape->GetOptionShapeMap())
			{
				if (Ref.Key == ShapeId &&  Ref.Value->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_LATERAL_BOX_SHAKE_HAND))
				{
					bFindLateralBox = true;
					break;
				}
			}
			if (!bFindLateralBox)
			{
				return nullptr;
			}

			DesDrawerDoorShape->SetDefaultHandleInfo(SrcDrawerDoorShape->GetDefaultHandleInfo());
			DesDrawerDoorShape->SetDoorGroupHanleType(SrcDrawerDoorShape->GetDoorGroupHanleType());
		}

		
		// 目标门板需要保留原始门板的属性
		const FString XAttriStr = SrcDrawerDoorShape->GetShapePosXAttri()->GetAttributeStr();
		const FString YAttriStr = SrcDrawerDoorShape->GetShapePosYAttri()->GetAttributeStr();
		const FString ZAttriStr = SrcDrawerDoorShape->GetShapePosZAttri()->GetAttributeStr();
		const FString WAttriStr = SrcDrawerDoorShape->GetShapeWidthAttri()->GetAttributeStr();
		const FString HAttriStr = SrcDrawerDoorShape->GetShapeHeightAttri()->GetAttributeStr();
		const FString DAttriStr = SrcDrawerDoorShape->GetShapeDepthAttri()->GetAttributeStr();
		const float UpExtensionValue = SrcDrawerDoorShape->GetUpExtensionValue();
		const float DownExtensionValue = SrcDrawerDoorShape->GetDownExtensionValue();
		const float LeftExtensionValue = SrcDrawerDoorShape->GetLeftExtensionValue();
		const float RightExtensionValue = SrcDrawerDoorShape->GetRightExtensionValue();
		const float UpDownGapValue = SrcDrawerDoorShape->GetUpDownGapValue();
		const float LeftRightGapValue = SrcDrawerDoorShape->GetLeftRightGapValue();

		
		SlidingDrawerShape->AddBaseChildShape(DesDrawerDoorShape);
		DesDrawerDoorShape->GetShapeCopyFromGuid() = SrcDrawerDoorShape->GetShapeCopyFromGuid();
		// 使用原始抽面的位置以及尺寸属性，更新新抽面
		{
		
			DesDrawerDoorShape->SetUpDownGapValue(UpDownGapValue);
			DesDrawerDoorShape->SetLeftRightGapValue(LeftRightGapValue);
			DesDrawerDoorShape->SetShapePosX(XAttriStr);
			DesDrawerDoorShape->SetShapePosY(YAttriStr);
			DesDrawerDoorShape->SetShapePosZ(ZAttriStr);
			DesDrawerDoorShape->SetShapeWidth(WAttriStr);
			DesDrawerDoorShape->SetShapeHeight(HAttriStr);
			DesDrawerDoorShape->SetShapeDepth(DAttriStr);		

			// 
			DesDrawerDoorShape->SetUpExtensionValue(UpExtensionValue);
			DesDrawerDoorShape->SetDownExtensionValue(DownExtensionValue);
			DesDrawerDoorShape->SetLeftExtensionValue(LeftExtensionValue);
			DesDrawerDoorShape->SetRightExtensionValue(RightExtensionValue);

		}
		ASCTShapeActor * DrawerDoorActor = DesDrawerDoorShape->SpawnShapeActor();
		DrawerDoorActor->AttachToActorOverride(SlidingDrawerShape->GetShapeActor(), FAttachmentTransformRules::KeepRelativeTransform);

		if (DesDrawerDoorShape->GetDoorGroupHanleType() == FDrawerDoorShape::EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND)
		{
			DesDrawerDoorShape->ReBuildDoorGroupHandle(DesDrawerDoorShape->GetDefaultHandleInfo().ID);
		}

		if (DesDrawerDoorShape->GetDoorGroupHanleType() == FDrawerDoorShape::EHandleMetalType::E_MT_SEALING_SIDE_HANDLE ||
			DesDrawerDoorShape->GetDoorGroupHanleType() == FDrawerDoorShape::EHandleMetalType::E_MT_SHAKE_HANDS)
		{
			{
				if (SrcDrawerDoorShape->GetDrawDoorSheet()->IsShakeHanlePositionValid())
				{
					const FVariableAreaDoorSheet::FSShakeHanle ShakeHandlePositon = SrcDrawerDoorShape->GetDrawDoorSheet()->GetShakeHanleTypePosition();
					DesDrawerDoorShape->GetDrawDoorSheet()->GetShakeHanleTypePosition() = ShakeHandlePositon;
				}
				if (SrcDrawerDoorShape->GetDrawDoorSheet()->IsSealingSideHandleValid())
				{
					const FVariableAreaDoorSheet::FSSealingSideHandle SealingSideHanlePosition = SrcDrawerDoorShape->GetDrawDoorSheet()->GetSealingSideHandlePosition();
					DesDrawerDoorShape->GetDrawDoorSheet()->GetSealingSideHandlePosition() = SealingSideHanlePosition;
				}
				FDoorSheetShapeBase::FAuxiliaryMetalInfo HnaleInfo = DesDrawerDoorShape->GetDrawDoorSheet()->GetDefaultHandleInfo();
				HnaleInfo.Location = MakeShareable(new FDoorHanlerDefaultLocation);
				bool bIsShankeHanle = DesDrawerDoorShape->GetDoorGroupHanleType() == FDrawerDoorShape::EHandleMetalType::E_MT_SHAKE_HANDS;
				if (bIsShankeHanle)
				{
					if (DesDrawerDoorShape->GetDrawDoorSheet()->IsShakeHanlePositionValid())
					{
						const FVariableAreaDoorSheet::FSShakeHanle & ShakeHandlePositon = DesDrawerDoorShape->GetDrawDoorSheet()->GetShakeHanleTypePosition();
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(HnaleInfo.Location)->Hpos = ShakeHandlePositon.HPositionType;
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(HnaleInfo.Location)->HposVal = ShakeHandlePositon.HPositionValue;
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(HnaleInfo.Location)->Vpos = ShakeHandlePositon.VPositionType;
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(HnaleInfo.Location)->VposVAl = ShakeHandlePositon.VPositionValue;
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(HnaleInfo.Location)->RotateDirection = ShakeHandlePositon.RotateDirection;
					}
					else
					{
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(HnaleInfo.Location)->Hpos = 3;
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(HnaleInfo.Location)->HposVal = 0.0f;
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(HnaleInfo.Location)->Vpos = 3;
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(HnaleInfo.Location)->VposVAl = 0.0f;
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(HnaleInfo.Location)->RotateDirection = 0;
					}
				}
				else
				{
					if (DesDrawerDoorShape->GetDrawDoorSheet()->IsSealingSideHandleValid())
					{
						const FVariableAreaDoorSheet::FSSealingSideHandle & SealingHandlePositon = DesDrawerDoorShape->GetDrawDoorSheet()->GetSealingSideHandlePosition();
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(HnaleInfo.Location)->Vpos = SealingHandlePositon.VPostionType;
					}
					else
					{
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(HnaleInfo.Location)->Vpos = 1;
					}
				}
				DesDrawerDoorShape->GetDrawDoorSheet()->SetDefaultHandleInfo(HnaleInfo);
				DesDrawerDoorShape->GetDrawDoorSheet()->ReBuildHandleOrLock(true);
				if (bIsShankeHanle)
				{
					DesDrawerDoorShape->GetDrawDoorSheet()->RecalHandlePosition();
				}
			}
		}

		SlidingDrawerShape->DeleteBaseChildShape(InSrcDoorSheetShape->GetParentShape());
		if (DesDrawerDoorShape->GetDoorGroupHanleType() == FDrawerDoorShape::EHandleMetalType::E_MT_SHAKE_HANDS)
		{
			DesDrawerDoorShape->GetDrawDoorSheet()->RecalHandlePosition();
		}
		DesDrawerDoorShape->UpdateDoorGroup();
		return DesDrawerDoorShape->GetDrawDoorSheet();
	}	
	else if (bIsSlidingDoor)
	{
		{
			const float CurDoorSeetWidth = InSrcDoorSheetShape->GetShapeWidth();
			TPair<float, float> Range = GetDoorSheetRange(StaticCast<FSlidingDoor*>(InDesDoorGroup)->GetDoorSheets()[0]->GetShapeWidthAttri());
			if (CurDoorSeetWidth > Range.Value || CurDoorSeetWidth < Range.Key)
			{
				return nullptr;
			}
		}
		{
			const float CurDoorSeetHeight = InSrcDoorSheetShape->GetShapeHeight();
			TPair<float, float> Range = GetDoorSheetRange(StaticCast<FSlidingDoor*>(InDesDoorGroup)->GetDoorSheets()[0]->GetShapeHeightAttri());
			if (CurDoorSeetHeight > Range.Value || CurDoorSeetHeight < Range.Key)
			{
				return nullptr;
			}
		}

		// 从给定的门板，找到对应的顶层空间
		auto GetSpecialTopSpaceShape = [](FSCTShape * InShape)->FSCTShape *
		{
			FSCTShape * RetShape = nullptr;
			do
			{
				if (!InShape) break;
				if (InShape->GetShapeType() != ST_Space) break;
				FSpaceShape * SpaceShape = StaticCast<FSpaceShape*>(InShape);
				if (SpaceShape->IsContainDoorGroup() == false) break;
				RetShape = SpaceShape;

			} while (false);
			return 	RetShape;
		};
		FSCTShape * ToFindShape = InSrcDoorSheetShape;
		do
		{
			if (!ToFindShape) break;
			if (GetSpecialTopSpaceShape(ToFindShape))
			{
				break;
			}
			ToFindShape = ToFindShape->GetParentShape();

		} while (true);
		// 查找选中门板在门组中的索引
		FSlidingDoor * SlidingDoor = StaticCast<FSlidingDoor *>(InSrcDoorSheetShape->GetParentShape());
		int32 SrcDoorSheetIndexInDoorGroup = -1;
		for (int32 Index = 0; Index < SlidingDoor->GetDoorSheets().Num(); ++Index)
		{
			if (SlidingDoor->GetDoorSheets()[Index].Get() == InSrcDoorSheetShape)
			{
				SrcDoorSheetIndexInDoorGroup = Index;
				break;
			}
		}
		check(SrcDoorSheetIndexInDoorGroup != -1);

		// 确保根据InSrcDoorSheetShape一定可以找到对应顶层空间
		check(ToFindShape && ToFindShape->GetShapeType() == ST_Space);
		FSpaceShape * SpaceShape = StaticCast<FSpaceShape *>(ToFindShape);
		TSharedPtr<FSlidingDoor> SrcSlidingHungDoor = StaticCastSharedPtr<FSlidingDoor>(SpaceShape->GetDoorShape());
		SpaceShape->RemoveDoorGroup();
		TSharedPtr<FSlidingDoor> DesSlidingDoorGroup = MakeShareable(new FSlidingDoor);
		InDesDoorGroup->CopyTo(DesSlidingDoorGroup.Get());		
		{			
			DesSlidingDoorGroup->SetCorverType(SrcSlidingHungDoor->GetCorverType());
			DesSlidingDoorGroup->SetUpPlateBoardValid(SrcSlidingHungDoor->IsUpPlateBoardValid());
			DesSlidingDoorGroup->SetDownPlateBoardValid(SrcSlidingHungDoor->IsDownPlateBoardValid());
			if (SrcSlidingHungDoor->IsExpectDoorSheetNumValid())
			{
				DesSlidingDoorGroup->SetExpectDoorSheetNum(SrcSlidingHungDoor->GetExpectDoorSheetNum());
			}
			DesSlidingDoorGroup->SetFirstPosition(SrcSlidingHungDoor->GetFirstPosition());
		}
		
		SpaceShape->AddSlidingDoor(DesSlidingDoorGroup, false);
		DesSlidingDoorGroup->GetShapeCopyFromGuid() = SrcSlidingHungDoor->GetShapeCopyFromGuid();
		DesSlidingDoorGroup->UpdateDoorGroup();
		SpaceShape->UpdateSpaceShape();
		return  DesSlidingDoorGroup->GetDoorSheets()[SrcDoorSheetIndexInDoorGroup];
	}
	return nullptr;
}

uint32 FSCTShapeUtilityTool::ReplaceCabinetAllDoorGroup(FSCTShape * InCabinetShape, FSCTShape * InDesDoorGroup)
{
	check(InCabinetShape && InCabinetShape->GetShapeType() == ST_Cabinet);
	check(InDesDoorGroup && InDesDoorGroup->GetShapeType() == ST_SideHungDoor);
	// 查找所有的掩门门组顶层空间，然后进行批量门组替换
	FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
	uint32 RetErrorNumner = 0;
	struct FParseSpaceShape : public FNoncopyable
	{
	public:
		FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape, FSCTShape* & InDesDoorGroup,uint32 & OutReplaceErrorCount) 
			:SpaceShape(InSpaceShape)
			,DesDoorGroup(InDesDoorGroup)
			,ReplaceErrorCount(OutReplaceErrorCount){}
		void operator()()
		{
			if (SpaceShape->GetDoorShape().IsValid())
			{
				// 目前只处理掩门
				if (SpaceShape->GetDoorShape()->GetShapeType() != ST_SideHungDoor) return;
				auto CheckOpenDirectionValid = [](FSCTShape * SrcDoorShape, FSCTShape * DesDoorShape)->bool
				{
					check(SrcDoorShape && DesDoorShape);
					check(SrcDoorShape->GetShapeType() == ST_SideHungDoor && DesDoorShape->GetShapeType() == ST_SideHungDoor);					
					return StaticCast<FSideHungDoor*>(SrcDoorShape)->GetDoorOpenType() == StaticCast<FSideHungDoor*>(DesDoorShape)->GetDoorOpenType();
				};
				if (CheckOpenDirectionValid(SpaceShape->GetDoorShape().Get(), DesDoorGroup))
				{
					const bool FitResult = SpaceShape->FitSideHungDoorGroupToCurSpaceShapeWithoutconfluence(StaticCast<FSideHungDoor*>(DesDoorGroup));
					if (!FitResult)
					{
						++ReplaceErrorCount;
					}
					else
					{
						TSharedPtr<FSideHungDoor> SideHungDoor = StaticCastSharedPtr<FSideHungDoor>(SpaceShape->GetDoorShape());
						ReplaceDoorGroupAndKeepMetalTransform(SideHungDoor->GetDoorSheets()[0].Get(), DesDoorGroup);
					}
				}
				else
				{
					++ReplaceErrorCount;
				}
				
			}
			else if (SpaceShape->GetInsideSpace().IsValid())
			{
				TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
				FParseSpaceShape Exc(InSideShape, DesDoorGroup, ReplaceErrorCount);
				Exc();
			}
			for (auto Ref : SpaceShape->GetChildSpaceShapes())
			{
				FParseSpaceShape Exc(Ref, DesDoorGroup, ReplaceErrorCount);
				Exc();
			}

		}
	private:
		TSharedPtr<FSpaceShape> & SpaceShape;
		FSCTShape* & DesDoorGroup;
		uint32 & ReplaceErrorCount;
	};
	TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
	check(TopSpace.IsValid());
	FParseSpaceShape Exc(TopSpace, InDesDoorGroup, RetErrorNumner);
	Exc();
	return RetErrorNumner;
}

bool FSCTShapeUtilityTool::ReplaceCabinetDoorWithGuid(FSCTShape * InCabinetShape, const FGuid & InGuid, FSCTShape * InDesDoorGroup,const bool bIsCopyGuid /*= true*/)
{
	check(InCabinetShape && InCabinetShape->GetShapeType() == ST_Cabinet);
	check(InDesDoorGroup && (InDesDoorGroup->GetShapeType() == ST_SideHungDoor || InDesDoorGroup->GetShapeType() == ST_DrawerDoor || InDesDoorGroup->GetShapeType() == ST_SlidingDoor));
	// 查找所有的掩门门组顶层空间，然后进行批量门组替换
	FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
	bool RetResult = false;
	struct FParseSpaceShape : public FNoncopyable
	{
	public:
		FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape, FSCTShape* & InDesDoorGroup,const FGuid & InGuid, bool & OutReplaceResult,const bool & InShapeCopyGuid)
			: SpaceShape(InSpaceShape)
			, DesDoorGroup(InDesDoorGroup)
			, RetResult(OutReplaceResult)
		    , SrcDoorGuid(InGuid)
			, bCopyShapeGuid(InShapeCopyGuid){}
		void operator()()
		{
			if (SpaceShape->GetDoorShape().IsValid() && 
				(bCopyShapeGuid ? SpaceShape->GetDoorShape()->GetShapeCopyFromGuid() == SrcDoorGuid :
					SpaceShape->GetDoorShape()->GetShapeGuid() == SrcDoorGuid)
				)
			{				
				if (SpaceShape->GetDoorShape()->GetShapeType() == ST_SideHungDoor)
				{
					const bool FitResult = SpaceShape->FitSideHungDoorGroupToCurSpaceShapeWithoutconfluence(StaticCast<FSideHungDoor*>(DesDoorGroup));
					if (FitResult)
					{
						TSharedPtr<FSideHungDoor> SideHungDoor = StaticCastSharedPtr<FSideHungDoor>(SpaceShape->GetDoorShape());
						ReplaceDoorGroupAndKeepMetalTransform(SideHungDoor->GetDoorSheets()[0].Get(), DesDoorGroup);
						RetResult = true;
					}
				}
				else if(SpaceShape->GetDoorShape()->GetShapeType() == ST_SlidingDoor)
				{
					TSharedPtr<FSlidingDoor> SlidingDoor = StaticCastSharedPtr<FSlidingDoor>(SpaceShape->GetDoorShape());
					ReplaceDoorGroupAndKeepMetalTransform(SlidingDoor->GetDoorSheets()[0].Get(), DesDoorGroup);
					RetResult = true;
				}
			}
			else if (SpaceShape->GetCoveredDrawerGroup().IsValid())
			{
				TSharedPtr<FDrawerGroupShape> CoveredDrawerGroup = SpaceShape->GetCoveredDrawerGroup();
				bool bIsOk = false;
				for (auto & Ref : CoveredDrawerGroup->GetPrimitiveDrawer()->GetDrawerDoorShapes())
				{
					bIsOk = bCopyShapeGuid ? Ref->GetShapeCopyFromGuid() == SrcDoorGuid : Ref->GetShapeGuid() == SrcDoorGuid;
					if (bIsOk)
					{
						ReplaceDoorGroupAndKeepMetalTransform(Ref->GetDrawDoorSheet().Get(), DesDoorGroup);						
						break;
					}
				}
				if (!bIsOk)
				{
					for (auto OutRef : CoveredDrawerGroup->GetCopyDrawerShapes())
					{
						for (auto & Ref : OutRef->GetDrawerDoorShapes())
						{
							bIsOk = bCopyShapeGuid ? Ref->GetShapeCopyFromGuid() == SrcDoorGuid : Ref->GetShapeGuid() == SrcDoorGuid;
							if (bIsOk)
							{
								ReplaceDoorGroupAndKeepMetalTransform(Ref->GetDrawDoorSheet().Get(), DesDoorGroup);								
								break;
							}
						}
						if (bIsOk) break;
					}
				}
			}
			else if (SpaceShape->GetInsideSpace().IsValid())
			{
				TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
				FParseSpaceShape Exc(InSideShape, DesDoorGroup, SrcDoorGuid, RetResult, bCopyShapeGuid);
				Exc();
			}
			for (auto Ref : SpaceShape->GetChildSpaceShapes())
			{
				FParseSpaceShape Exc(Ref, DesDoorGroup, SrcDoorGuid, RetResult, bCopyShapeGuid);
				Exc();
			}

		}
	private:
		TSharedPtr<FSpaceShape> & SpaceShape;
		FSCTShape* & DesDoorGroup;
		bool & RetResult;
		const FGuid & SrcDoorGuid;
		const bool &bCopyShapeGuid;
	};

	TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
	check(TopSpace.IsValid());
	FParseSpaceShape Exc(TopSpace, InDesDoorGroup, InGuid, RetResult, bIsCopyGuid);
	Exc();
	return RetResult;
}

void FSCTShapeUtilityTool::ReplaceCabinetMaterial(UMaterialInterface * InMaterialInterface, FSCTShape * InCabinetShape)
{
	check(InMaterialInterface);
	check(InCabinetShape);
	check(InCabinetShape->GetShapeType() == ST_Cabinet);
	const TArray<EShapeType> IgnoreShapeTypeFilter = { ST_Decoration ,ST_Accessory, ST_SlidingDoor,
		ST_SideHungDoor,ST_DrawerDoor,ST_SpaceDividDoorSheet, ST_VariableAreaDoorSheet ,ST_VariableAreaDoorSheet_ForDrawerDoor, 
		ST_VariableAreaDoorSheet_ForSideHungDoor,ST_SlidingDrawer,ST_DrawerGroup,ST_BoxShape,ST_VeneerdBoardGroup};
	struct FFindShapeOperator
	{		 
	public:
		FFindShapeOperator(FSCTShape * & InShape,TArray<FBoardShape*> & OutShapes, const TArray<EShapeType> & InShapeFilters)
			: Shape(InShape)
			, ReserveShapes(OutShapes)
			, IgnoreShapeTypeFilter(InShapeFilters){}
		void operator()()
		{
			if (!Shape) return;
			if (ExcIgnoreTypeFilter(Shape)) return;
			if (Shape->GetShapeType() == ST_Board)
			{
				ReserveShapes.Emplace(StaticCast<FBoardShape*>(Shape));
			}
			else
			{
				for (const auto & Ref : Shape->GetChildrenShapes())
				{
					FSCTShape * Shape = Ref.Get();
					FFindShapeOperator(Shape, ReserveShapes, IgnoreShapeTypeFilter)();
				}
			}
		}
	private:
		bool ExcIgnoreTypeFilter(FSCTShape * & InShape)
		{
			bool RetResult = false;
			if (IgnoreShapeTypeFilter.Contains(StaticCast<EShapeType>(InShape->GetShapeType())))
			{
				RetResult = true;
			}
			return RetResult;
		}
	private:
		FSCTShape * &Shape;
		TArray<FBoardShape*> & ReserveShapes;
		const TArray<EShapeType> & IgnoreShapeTypeFilter;
	};	
	FSCTShape * ToRecursiveShape = InCabinetShape;
	TArray<FBoardShape*> ToReplaceBoardShpaes;
	FFindShapeOperator(ToRecursiveShape, ToReplaceBoardShpaes, IgnoreShapeTypeFilter)();
	// 执行材质替换
	for(auto & Ref : ToReplaceBoardShpaes)
	{
		ASCTBoardActor * ShapeActor = StaticCast<ASCTBoardActor*>(Ref->GetShapeActor());
		if (!ShapeActor) continue;
		ShapeActor->SetTemporaryMaterial(InMaterialInterface);
		ShapeActor->UpdateActorBoardMaterial();
	}
}

TSharedPtr<FSCTShape> FSCTShapeUtilityTool::ReplaceCabinetAllDoorGroupAndKeepMetalTransform(FSCTShape * InSrcDoorSheetShape, FSCTShape * InDesDoorGroup)
{
	check(InSrcDoorSheetShape && InSrcDoorSheetShape->GetParentShape() && InSrcDoorSheetShape->GetParentShape()->GetShapeType() == ST_SideHungDoor);
	check(InDesDoorGroup && InDesDoorGroup->GetShapeType() == ST_SideHungDoor);

	// 第一步查找柜体空间
	FCabinetShape* CabinetShape = nullptr;
	FSCTShape * TempShape = InSrcDoorSheetShape;
	do
	{
		if (TempShape && TempShape->GetShapeType() == ST_Cabinet)
		{
			CabinetShape = StaticCast<FCabinetShape*>(TempShape);
			break;
		}
		else
		{
			TempShape = TempShape->GetParentShape();
		}

	} while (true);
	check(CabinetShape);

	// 第二步查找输入门板所对应的顶层空间
	auto GetSpecialTopSpaceShape = [](FSCTShape * InShape)->FSCTShape *
	{
		FSCTShape * RetShape = nullptr;
		do
		{
			if (!InShape) break;
			if (InShape->GetShapeType() != ST_Space) break;
			FSpaceShape * SpaceShape = StaticCast<FSpaceShape*>(InShape);
			if (SpaceShape->IsContainDoorGroup() == false) break;
			RetShape = SpaceShape;

		} while (false);
		return 	RetShape;
	};
	FSCTShape * ToFindSpaceShape = InSrcDoorSheetShape;
	do
	{
		if (!ToFindSpaceShape) break;
		if (GetSpecialTopSpaceShape(ToFindSpaceShape))
		{
			break;
		}
		ToFindSpaceShape = ToFindSpaceShape->GetParentShape();

	} while (true);
	check(ToFindSpaceShape && ToFindSpaceShape->GetShapeType() == ST_Space);

	//第三部查找输入门板在指定顶部空间中对应的门组里面，所占的索引 
	FSideHungDoor * SideHungDoor = StaticCast<FSideHungDoor *>(InSrcDoorSheetShape->GetParentShape());
	int32 SrcDoorSheetIndexInDoorGroup = -1;
	for (int32 Index = 0; Index < SideHungDoor->GetDoorSheetNum(); ++Index)
	{
		if (SideHungDoor->GetDoorSheets()[Index].Get() == InSrcDoorSheetShape)
		{
			SrcDoorSheetIndexInDoorGroup = Index;
			break;
		}
	}
	check(SrcDoorSheetIndexInDoorGroup != -1);

	// 第四步，进行整个柜体的门板替换
	ReplaceCabinetAllDoorGroup(CabinetShape, InDesDoorGroup);

	// 返回替换门板后的门组里面的对应的新门板
	FSpaceShape * DesSpaceShape = StaticCast<FSpaceShape*>(ToFindSpaceShape);
	check(DesSpaceShape->GetDoorShape().IsValid());
	check(StaticCastSharedPtr<FSideHungDoor>(DesSpaceShape->GetDoorShape())->GetDoorSheets().IsValidIndex(SrcDoorSheetIndexInDoorGroup));
	return 	StaticCastSharedPtr<FSideHungDoor>(DesSpaceShape->GetDoorShape())->GetDoorSheets()[SrcDoorSheetIndexInDoorGroup];
}

FString FSCTShapeUtilityTool::GetFullCachePathByMaterialTypeAndID(const EMaterialType MaterialType, const int32 InMaterialID)
{
	return FMaterialManagerInstatnce::GetIns().GetMaterialManagerPtr()->GetFullPathFromMaterialTypeAndID(MaterialType,FString::FromInt(InMaterialID));
}

bool FSCTShapeUtilityTool::ReplaceDoorGroupMaterial(FSCTShape * InDoorShape, const FCommonPakData * InMaterialPakData, UMaterialInterface * InMaterialInterface)
{
	check(InMaterialInterface && InDoorShape);
	check(InDoorShape->GetShapeType() == ST_SideHungDoor || InDoorShape->GetShapeType() == ST_DrawerDoor);

	FDoorSheetShapeBase::FDoorPakMetaData DoorPakData;
	{
		DoorPakData.ID = InMaterialPakData->ID;
		DoorPakData.MD5 = InMaterialPakData->MD5;
		DoorPakData.Name = InMaterialPakData->Name;
		DoorPakData.OptimizeParam = InMaterialPakData->OptimizeParam;
		DoorPakData.ThumbnailUrl = InMaterialPakData->ThumbnailUrl;
		DoorPakData.Url = InMaterialPakData->Url;
	}

	switch (InDoorShape->GetShapeType())
	{
		case ST_SideHungDoor:
		{
			FSideHungDoor * SideHungDoor = StaticCast<FSideHungDoor*>(InDoorShape);
			for (auto & Ref : SideHungDoor->GetDoorSheets())
			{
				Ref->SetMaterial(DoorPakData);
				Ref->UpdateMaterial(InMaterialInterface);
			}
			// 如果存在插角封板，则插角封板也跟着变化
			if (SideHungDoor->GetFrontBoardType() != FSideHungDoor::EFrontBoardType::E_None)
			{
				SideHungDoor->SetFrontBoardMaterial(DoorPakData);
			}

		}break;
		case ST_DrawerDoor:
		{
			FDrawerDoorShape * DrawerDoor = StaticCast<FDrawerDoorShape*>(InDoorShape);
			DrawerDoor->GetDrawDoorSheet()->SetMaterial(DoorPakData);
			DrawerDoor->GetDrawDoorSheet()->UpdateMaterial(InMaterialInterface);
		}break;
		default:
		{
			check(false);
		}break;
	}
	return true;
}

uint32 FSCTShapeUtilityTool::ReplacInsertionBoardMaterial(FSCTShape * InShape, const FCommonPakData * const InMaterialPakData, UMaterialInterface * InMaterialInterface)
{
	check(InMaterialInterface);
	check(InShape && InShape->GetShapeType() == ST_Cabinet);
	// 查找所有电器饰条组
	FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InShape);
	uint32 RetSuccessedNumber = 0;
	struct FParseSpaceShape : public FNoncopyable
	{
	public:
		FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape, 
			uint32 & OutReplaceErrorCount, 
			const FCommonPakData * const & InMaterialData, UMaterialInterface * & InMaterialInterface)
			: SpaceShape(InSpaceShape)			
			, ReplaceErrorCount(OutReplaceErrorCount)			
			, InMaterialPakData(InMaterialData)
			, MaterialInterface(InMaterialInterface) {}
		void operator()()
		{
			if (SpaceShape->IsEmbededElectricalGroup())
			{
				// 整柜替换门板的时候，电器饰条组中的饰条材质也要跟着进行替换				
				{
					TSharedPtr<FEmbededElectricalGroup> EmbededElectricalGroup = SpaceShape->GetEmbededElecticalGroup();
					if (EmbededElectricalGroup->GetInsertionBoardShape().IsValid())
					{
						FInsertionBoard::FInsertBoardMaterial InsertBoardMaterial;
						InsertBoardMaterial.ID = InMaterialPakData->ID;
						InsertBoardMaterial.MD5 = InMaterialPakData->MD5;
						InsertBoardMaterial.Name = InMaterialPakData->Name;
						InsertBoardMaterial.OptimizeParam = InMaterialPakData->OptimizeParam;
						InsertBoardMaterial.ThumbnailUrl = InMaterialPakData->ThumbnailUrl;
						InsertBoardMaterial.Url = InMaterialPakData->Url;
						EmbededElectricalGroup->GetInsertionBoardShape()->SetMaterial(InsertBoardMaterial);
						EmbededElectricalGroup->GetInsertionBoardShape()->UpdateBoardShape();
						++ReplaceErrorCount;
					}
				}
			}
			else if (SpaceShape->GetInsideSpace().IsValid())
			{
				TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
				FParseSpaceShape Exc(InSideShape, ReplaceErrorCount, InMaterialPakData, MaterialInterface);
				Exc();
			}
			for (auto Ref : SpaceShape->GetChildSpaceShapes())
			{
				FParseSpaceShape Exc(Ref, ReplaceErrorCount, InMaterialPakData, MaterialInterface);
				Exc();
			}

		}
	private:
		TSharedPtr<FSpaceShape> & SpaceShape;		
		uint32 & ReplaceErrorCount;		
		const FCommonPakData * const & InMaterialPakData;
		UMaterialInterface * & MaterialInterface;
	};

	TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
	check(TopSpace.IsValid());	
	FParseSpaceShape Exc(TopSpace,RetSuccessedNumber,InMaterialPakData, InMaterialInterface);
	Exc();
	return RetSuccessedNumber;
}

uint32 FSCTShapeUtilityTool::ReplaceCabinetAllDoorGroupMaterial(FSCTShape * InShape, const uint32 InDoorGroupId, const FCommonPakData * const InMaterialPakData, UMaterialInterface * InMaterialInterface, const bool bIsDrawer /*= false*/)
{
	check(InMaterialInterface);	
	check(InShape && InShape->GetShapeType() == ST_Cabinet);
	// 查找所有的掩门门组顶层空间，然后进行批量门组替换
	FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InShape);
	uint32 RetErrorNumner = 0;
	struct FParseSpaceShape : public FNoncopyable
	{
	public:
		FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape,const int32 & InDoorGroupId, 
			uint32 & OutReplaceErrorCount,const bool & InIsSideHungDoor,
			const FCommonPakData * const & InMaterialData,UMaterialInterface * & InMaterialInterface)
			:SpaceShape(InSpaceShape)
			, DoorGroupId(InDoorGroupId)
			, ReplaceErrorCount(OutReplaceErrorCount)
			, bIsSideHungDoor(InIsSideHungDoor)
			, InMaterialPakData(InMaterialData)
		    , MaterialInterface(InMaterialInterface){}
		void operator()()
		{
			if (SpaceShape->GetDoorShape().IsValid())
			{				
				if (SpaceShape->GetDoorShape()->GetShapeType() != ST_SideHungDoor && 
					SpaceShape->GetDoorShape()->GetShapeType() != ST_DrawerDoor) return;				
				if (SpaceShape->GetDoorShape()->GetShapeId() != DoorGroupId &&
					(bIsSideHungDoor ? SpaceShape->GetDoorShape()->GetShapeType() == ST_SideHungDoor : SpaceShape->GetDoorShape()->GetShapeType() == ST_DrawerDoor))
				{
					++ReplaceErrorCount;
				}
				else
				{
					ReplaceDoorGroupMaterial(SpaceShape->GetDoorShape().Get(), InMaterialPakData,MaterialInterface);
				}
			}
			else if (SpaceShape->IsEmbededElectricalGroup())
			{
				// 整柜替换门板的时候，电器饰条组中的饰条材质也要跟着进行替换				
				{
					TSharedPtr<FEmbededElectricalGroup> EmbededElectricalGroup = SpaceShape->GetEmbededElecticalGroup();
					if (EmbededElectricalGroup->GetInsertionBoardShape().IsValid())
					{
						FInsertionBoard::FInsertBoardMaterial InsertBoardMaterial;
						InsertBoardMaterial.ID = InMaterialPakData->ID;
						InsertBoardMaterial.MD5 = InMaterialPakData->MD5;
						InsertBoardMaterial.Name = InMaterialPakData->Name;
						InsertBoardMaterial.OptimizeParam = InMaterialPakData->OptimizeParam;
						InsertBoardMaterial.ThumbnailUrl = InMaterialPakData->ThumbnailUrl;
						InsertBoardMaterial.Url = InMaterialPakData->Url;
						EmbededElectricalGroup->GetInsertionBoardShape()->SetMaterial(InsertBoardMaterial);
						EmbededElectricalGroup->GetInsertionBoardShape()->UpdateBoardShape();
					}
				}
			}
			else if (SpaceShape->GetInsideSpace().IsValid())
			{
				TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
				FParseSpaceShape Exc(InSideShape, DoorGroupId, ReplaceErrorCount, bIsSideHungDoor, InMaterialPakData, MaterialInterface);
				Exc();
			}
			for (auto Ref : SpaceShape->GetChildSpaceShapes())
			{
				FParseSpaceShape Exc(Ref, DoorGroupId, ReplaceErrorCount, bIsSideHungDoor, InMaterialPakData, MaterialInterface);
				Exc();
			}

		}
	private:
		TSharedPtr<FSpaceShape> & SpaceShape;
		const int32 &  DoorGroupId;
		uint32 & ReplaceErrorCount;
		const bool & bIsSideHungDoor;
		const FCommonPakData * const & InMaterialPakData;
		UMaterialInterface * & MaterialInterface;
	};

	TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
	check(TopSpace.IsValid());
	const bool bIsSideHungDoor = !bIsDrawer;
	FParseSpaceShape Exc(TopSpace, InDoorGroupId, RetErrorNumner, bIsSideHungDoor, InMaterialPakData, InMaterialInterface);
	Exc();
	return RetErrorNumner;
}

bool FSCTShapeUtilityTool::ReplaceCabinetDoorGroupMaterialWithGuid(FSCTShape * InCabinetShape, const FGuid & InGuid,
	const FCommonPakData * const InMaterialPakData, UMaterialInterface * InMaterialInterface, const bool bIsCopyGuid /*= true*/)
{
	check(InCabinetShape && InCabinetShape->GetShapeType() == ST_Cabinet);
	check(InMaterialPakData && InMaterialInterface);
	// 查找所有的掩门门组顶层空间，然后进行批量门组替换
	FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
	bool RetResult = false;
	struct FParseSpaceShape : public FNoncopyable
	{
	public:
		FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape, const FGuid & InGuid, bool & OutReplaceResult,
			const FCommonPakData * const & InMaterialPakData, UMaterialInterface *& InMaterialInterface,const bool & InShapeCopyGuid)
			: SpaceShape(InSpaceShape)			
			, RetResult(OutReplaceResult)
			, SrcDoorGuid(InGuid)
			, MaterialPakData(InMaterialPakData)
			, MaterialInterface(InMaterialInterface)
			, bCopyShapeGuid(InShapeCopyGuid) {}
		void operator()()
		{
			if (SpaceShape->GetDoorShape().IsValid() &&
				(bCopyShapeGuid ? SpaceShape->GetDoorShape()->GetShapeCopyFromGuid() == SrcDoorGuid :
					SpaceShape->GetDoorShape()->GetShapeGuid() == SrcDoorGuid)
				)
			{				
				TSharedPtr<FSideHungDoor> SideHungDoor = StaticCastSharedPtr<FSideHungDoor>(SpaceShape->GetDoorShape());
				RetResult = ReplaceDoorGroupMaterial(SideHungDoor.Get(), MaterialPakData, MaterialInterface);
			}
			else if (SpaceShape->GetCoveredDrawerGroup().IsValid())
			{
				TSharedPtr<FDrawerGroupShape> CoveredDrawerGroup = SpaceShape->GetCoveredDrawerGroup();
				bool bIsOk = false;
				for (auto & Ref : CoveredDrawerGroup->GetPrimitiveDrawer()->GetDrawerDoorShapes())
				{
					bIsOk = bCopyShapeGuid ? Ref->GetShapeCopyFromGuid() == SrcDoorGuid : Ref->GetShapeGuid() == SrcDoorGuid;
					if (bIsOk)
					{
						RetResult = ReplaceDoorGroupMaterial(Ref.Get(), MaterialPakData, MaterialInterface);
						break;
					}									
				}
				if (!bIsOk)
				{
					for (auto OutRef : CoveredDrawerGroup->GetCopyDrawerShapes())
					{
						for (auto & Ref : OutRef->GetDrawerDoorShapes())
						{
							bIsOk = bCopyShapeGuid ? Ref->GetShapeCopyFromGuid() == SrcDoorGuid : Ref->GetShapeGuid() == SrcDoorGuid;
							if (bIsOk)
							{
								RetResult = ReplaceDoorGroupMaterial(Ref.Get(), MaterialPakData, MaterialInterface);
								break;
							}
						}
						if (bIsOk) break;
					}
				}
			}
			else if (SpaceShape->GetInsideSpace().IsValid())
			{
				TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
				FParseSpaceShape Exc(InSideShape, SrcDoorGuid, RetResult, MaterialPakData, MaterialInterface, bCopyShapeGuid);
				Exc();
			}
			for (auto Ref : SpaceShape->GetChildSpaceShapes())
			{
				FParseSpaceShape Exc(Ref, SrcDoorGuid, RetResult, MaterialPakData, MaterialInterface, bCopyShapeGuid);
				Exc();
			}

		}
	private:
		TSharedPtr<FSpaceShape> & SpaceShape;		
		bool & RetResult;
		const FGuid & SrcDoorGuid;
		const bool &bCopyShapeGuid;
		const FCommonPakData * const & MaterialPakData;
		UMaterialInterface * & MaterialInterface;
	};

	TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
	check(TopSpace.IsValid());
	FParseSpaceShape Exc(TopSpace, InGuid, RetResult, InMaterialPakData, InMaterialInterface, bIsCopyGuid);
	Exc();
	return RetResult;
}

bool FSCTShapeUtilityTool::ReplaceCabinetDoorSheetOpenDoorDirectionWithGuid(FSCTShape * InCabinetShape, const FGuid & InGuid, const int32 InOpenDirection, const bool bIsCopyGuid)
{
	check(InCabinetShape && InCabinetShape->GetShapeType() == ST_Cabinet);	
	// 查找所有的掩门门组顶层空间，然后进行批量门组替换
	FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
	bool RetResult = false;
	struct FParseSpaceShape : public FNoncopyable
	{
	public:
		FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape, const FGuid & InGuid, bool & OutReplaceResult,
			const int32 & InOpenDoorDirection, const bool & InShapeCopyGuid)
			: SpaceShape(InSpaceShape)
			, RetResult(OutReplaceResult)
			, SrcDoorGuid(InGuid)	
			, OpenDoorDirection(InOpenDoorDirection)
			, bCopyShapeGuid(InShapeCopyGuid) {}
		void operator()()
		{
			if (SpaceShape->GetDoorShape().IsValid() && SpaceShape->GetDoorShape()->GetShapeType() == ST_SideHungDoor)
			{
				bool SerarchResult = false;
				TSharedPtr<FSideHungDoor> SideHungDoor = StaticCastSharedPtr<FSideHungDoor>(SpaceShape->GetDoorShape());
				TSharedPtr<FVariableAreaDoorSheet> VarDoorSheet;
				for (const auto & Ref : SideHungDoor->GetDoorSheets())
				{
					SerarchResult = (bCopyShapeGuid ? Ref->GetShapeCopyFromGuid() == SrcDoorGuid :
						SpaceShape->GetDoorShape()->GetShapeGuid() == SrcDoorGuid);
					if (SerarchResult)
					{
						VarDoorSheet = Ref;
						break;
					}
				}
				if (SerarchResult && VarDoorSheet.IsValid())
				{
					VarDoorSheet->SetOpenDoorDirection(StaticCast<FVariableAreaDoorSheet::EDoorOpenDirection>(OpenDoorDirection));
					RetResult = true;
				}							
			}
			else if (SpaceShape->GetInsideSpace().IsValid())
			{
				TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
				FParseSpaceShape Exc(InSideShape, SrcDoorGuid, RetResult, OpenDoorDirection, bCopyShapeGuid);
				Exc();
			}
			for (auto Ref : SpaceShape->GetChildSpaceShapes())
			{
				FParseSpaceShape Exc(Ref, SrcDoorGuid, RetResult, OpenDoorDirection, bCopyShapeGuid);
				Exc();
			}

		}
	private:
		TSharedPtr<FSpaceShape> & SpaceShape;
		bool & RetResult;
		const FGuid & SrcDoorGuid;
		const bool &bCopyShapeGuid;
		const int32 & OpenDoorDirection;
	};

	TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
	check(TopSpace.IsValid());
	FParseSpaceShape Exc(TopSpace, InGuid, RetResult, InOpenDirection, bIsCopyGuid);
	Exc();
	return RetResult;
}

bool FSCTShapeUtilityTool::ReplaceDoorGroupMetal(FSCTShape * InShape, const int32 InMetalId)
{
	check(InShape);
	check(InShape->GetShapeType() == ST_SideHungDoor || InShape->GetShapeType() == ST_DrawerDoor);
	if (InShape->GetShapeType() == ST_SideHungDoor)
	{
		// 先确定是箱体拉手，还是封边拉手，以及明拉手
		FSideHungDoor * SideHungDoor = StaticCast<FSideHungDoor *>(InShape);
		if (SideHungDoor->GetHanleType() == FSideHungDoor::EHandleMetalType::E_MT_None)  return false;
		const bool bIsBoxHanle = SideHungDoor->IsContainSpecialMetal(InMetalId);
		// 如果是箱体拉手
		if (bIsBoxHanle)
		{
			// 如果要替换箱体拉手，则当前门组的拉手类型也必须是箱体拉手才可以
			if (SideHungDoor->GetHanleType() == FSideHungDoor::EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND)
			{
				SideHungDoor->ReBuildHandle(InMetalId);
				SideHungDoor->UpdateDoorGroup();
			}
			else
			{
				return false;
			}
		}
		else
		{
			// 找到指定ID的拉手
			check(SideHungDoor->GetDoorSheets().Num() >= 1);
			const TMap<int32, TSharedPtr<FSCTShape>> & OptionShapesRef = SideHungDoor->GetDoorSheets()[0]->GetOptionShapeMap();
			const bool FindResult = OptionShapesRef.Contains(InMetalId);
			if (!FindResult)
			{
				return false;
			}
			const TSharedPtr<FSCTShape> & MetalShape = OptionShapesRef[InMetalId];
			check(MetalShape.IsValid());
			check(MetalShape->GetShapeType() == ST_Accessory);
			// 封边拉手
			if (MetalShape->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SEALING_SIDE_HANDLE))
			{
				SideHungDoor->SetHanleType(FSideHungDoor::EHandleMetalType::E_MT_SEALING_SIDE_HANDLE);
				for (auto & Ref : SideHungDoor->GetDoorSheets())
				{
					Ref->GetCurrentHandleInfo().ID = MetalShape->GetShapeId();
					Ref->GetCurrentHandleInfo().Name = MetalShape->GetShapeName();
					if (Ref->GetCurrentHandleInfo().Location.IsValid() == false)
					{
						Ref->GetCurrentHandleInfo().Location = MakeShareable(new FDoorHanlerDefaultLocation);
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(Ref->GetCurrentHandleInfo().Location)->Vpos = 1;
					}
					if (Ref->IsSealingSideHandleValid())
					{
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(Ref->GetCurrentHandleInfo().Location)->Vpos = Ref->GetSealingSideHandlePosition().VPostionType;
					}
					else
					{
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(Ref->GetCurrentHandleInfo().Location)->Vpos = 1;
					}
					Ref->ReBuildHandle();
				}
				SideHungDoor->UpdateDoorGroup();
			}
			// 明装拉手
			else if (MetalShape->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SHAKE_HANDS))
			{
				SideHungDoor->SetHanleType(FSideHungDoor::EHandleMetalType::E_MT_SHAKE_HANDS);
				for (auto & Ref : SideHungDoor->GetDoorSheets())
				{
					if (Ref->IsShakeHanlePositionValid() == false) continue;
					Ref->GetCurrentHandleInfo().ID = MetalShape->GetShapeId();
					Ref->GetCurrentHandleInfo().Name = MetalShape->GetShapeName();
					Ref->GetCurrentHandleInfo().Location = MakeShareable(new FDoorHanlerDefaultLocation);
					StaticCastSharedPtr<FDoorHanlerDefaultLocation>(Ref->GetCurrentHandleInfo().Location)->Vpos = Ref->GetShakeHanleTypePosition().VPositionType;
					StaticCastSharedPtr<FDoorHanlerDefaultLocation>(Ref->GetCurrentHandleInfo().Location)->VposVAl = Ref->GetShakeHanleTypePosition().VPositionValue;
					StaticCastSharedPtr<FDoorHanlerDefaultLocation>(Ref->GetCurrentHandleInfo().Location)->Hpos = Ref->GetShakeHanleTypePosition().HPositionType;
					StaticCastSharedPtr<FDoorHanlerDefaultLocation>(Ref->GetCurrentHandleInfo().Location)->HposVal = Ref->GetShakeHanleTypePosition().HPositionValue;
					StaticCastSharedPtr<FDoorHanlerDefaultLocation>(Ref->GetCurrentHandleInfo().Location)->RotateDirection = Ref->GetShakeHanleTypePosition().RotateDirection;
					Ref->ReBuildHandle();
					Ref->RecalHandlePosition();
				}
				SideHungDoor->UpdateDoorGroup();
			}

		}
	}
	else if (InShape->GetShapeType() == ST_DrawerDoor)
	{
		// 先确定是箱体拉手，还是封边拉手，以及明拉手
		FDrawerDoorShape * DrawerDoorShape = StaticCast<FDrawerDoorShape *>(InShape);
		if (DrawerDoorShape->GetDoorGroupHanleType() == FDrawerDoorShape::EHandleMetalType::E_MT_None)  return false;
		const bool bIsBoxHanle = DrawerDoorShape->IsContainSpecialMetal(InMetalId);
		// 如果是箱体拉手
		if (bIsBoxHanle)
		{
			// 如果要替换箱体拉手，则当前门组的拉手类型也必须是箱体拉手才可以
			if (DrawerDoorShape->GetDoorGroupHanleType() == FDrawerDoorShape::EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND)
			{
				DrawerDoorShape->ReBuildDoorGroupHandle(InMetalId);
				DrawerDoorShape->UpdateDoorGroup();
			}
			else
			{
				return false;
			}
		}
		else
		{
			// 找到指定ID的拉手
			const TMap<int32, TSharedPtr<FSCTShape>> & OptionShapesRef = DrawerDoorShape->GetDrawDoorSheet()->GetOptionShapeMap();
			const bool FindResult = OptionShapesRef.Contains(InMetalId);
			if (!FindResult)
			{
				return false;
			}
			const TSharedPtr<FSCTShape> & MetalShape = OptionShapesRef[InMetalId];
			check(MetalShape.IsValid());
			check(MetalShape->GetShapeType() == ST_Accessory);
			// 封边拉手
			if (MetalShape->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SEALING_SIDE_HANDLE))
			{
				DrawerDoorShape->SetDoorGroupHanleType(FDrawerDoorShape::EHandleMetalType::E_MT_SEALING_SIDE_HANDLE);
				auto Ref = DrawerDoorShape->GetDrawDoorSheet();
				{
					FDoorSheetShapeBase::FAuxiliaryMetalInfo HanleInfo = Ref->GetDefaultHandleInfo();
					HanleInfo.ID = MetalShape->GetShapeId();
					HanleInfo.Name = MetalShape->GetShapeName();
					Ref->SetDefaultHandleInfo(HanleInfo);
					if (Ref->GetDefaultHandleInfo().Location.IsValid() == false)
					{
						TSharedPtr<FDoorHanlerDefaultLocation> DoorHandleLocation = MakeShareable(new FDoorHanlerDefaultLocation);
						DoorHandleLocation->Vpos = 1;
						FDoorSheetShapeBase::FAuxiliaryMetalInfo HanleInfo = Ref->GetDefaultHandleInfo();
						HanleInfo.Location = DoorHandleLocation;
						Ref->SetDefaultHandleInfo(HanleInfo);
					}
					if (Ref->IsSealingSideHandleValid())
					{
						FDoorSheetShapeBase::FAuxiliaryMetalInfo HanleInfo = Ref->GetDefaultHandleInfo();
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(HanleInfo.Location)->Vpos = Ref->GetSealingSideHandlePosition().VPostionType;
						Ref->SetDefaultHandleInfo(HanleInfo);
					}
					Ref->ReBuildHandleOrLock(true);
				}
				DrawerDoorShape->UpdateDoorGroup();
			}
			// 明装拉手
			else if (MetalShape->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SHAKE_HANDS))
			{
				DrawerDoorShape->SetDoorGroupHanleType(FDrawerDoorShape::EHandleMetalType::E_MT_SHAKE_HANDS);
				auto Ref = DrawerDoorShape->GetDrawDoorSheet();
				{
					if (Ref->IsShakeHanlePositionValid() == true)
					{
						FDoorSheetShapeBase::FAuxiliaryMetalInfo HanleInfo = Ref->GetDefaultHandleInfo();
						HanleInfo.ID = MetalShape->GetShapeId();
						HanleInfo.Name = MetalShape->GetShapeName();
						HanleInfo.Location = MakeShareable(new FDoorHanlerDefaultLocation);
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(HanleInfo.Location)->Vpos = Ref->GetShakeHanleTypePosition().VPositionType;
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(HanleInfo.Location)->VposVAl = Ref->GetShakeHanleTypePosition().VPositionValue;
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(HanleInfo.Location)->Hpos = Ref->GetShakeHanleTypePosition().HPositionType;
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(HanleInfo.Location)->HposVal = Ref->GetShakeHanleTypePosition().HPositionValue;
						StaticCastSharedPtr<FDoorHanlerDefaultLocation>(HanleInfo.Location)->RotateDirection = Ref->GetShakeHanleTypePosition().RotateDirection;
						Ref->SetDefaultHandleInfo(HanleInfo);
						Ref->ReBuildHandleOrLock(true);
						Ref->RecalHandlePosition();
					}
				}
				DrawerDoorShape->UpdateDoorGroup();
			}
		}		
	}
	return true;
}

bool FSCTShapeUtilityTool::ReplaceCabinetDoorGroupMetalWithGuid(FSCTShape * InCabinetShape, const FGuid & InGuid, const int32 InMetalId, const bool bIsCopyGuid)
{
	check(InCabinetShape && InCabinetShape->GetShapeType() == ST_Cabinet);
	// 查找所有的掩门门组顶层空间，然后进行批量门组替换
	FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
	bool RetResult = false;
	struct FParseSpaceShape : public FNoncopyable
	{
	public:
		FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape, const int32 & InMetalId, const FGuid & InGuid, bool & OutReplaceResult, const bool & InShapeCopyGuid)
			: SpaceShape(InSpaceShape)
			, MetalId(InMetalId)
			, RetResult(OutReplaceResult)
			, SrcDoorGuid(InGuid)
			, bCopyShapeGuid(InShapeCopyGuid) {}
		void operator()()
		{
			if (SpaceShape->GetDoorShape().IsValid() &&
				(bCopyShapeGuid ? SpaceShape->GetDoorShape()->GetShapeCopyFromGuid() == SrcDoorGuid :
					SpaceShape->GetDoorShape()->GetShapeGuid() == SrcDoorGuid)
				)
			{				
				RetResult = ReplaceDoorGroupMetal(SpaceShape->GetDoorShape().Get(), MetalId);
			}
			else if (SpaceShape->GetCoveredDrawerGroup().IsValid())
			{
				TSharedPtr<FDrawerGroupShape> CoveredDrawerGroup = SpaceShape->GetCoveredDrawerGroup();
				bool bIsOk = false;
				for (auto & Ref : CoveredDrawerGroup->GetPrimitiveDrawer()->GetDrawerDoorShapes())
				{
					bIsOk = bCopyShapeGuid ? Ref->GetShapeCopyFromGuid() == SrcDoorGuid : Ref->GetShapeGuid() == SrcDoorGuid;
					if (bIsOk)
					{
						RetResult = ReplaceDoorGroupMetal(Ref.Get(), MetalId);
						break;
					}
				}
				if (!bIsOk)
				{
					for (auto OutRef : CoveredDrawerGroup->GetCopyDrawerShapes())
					{
						for (auto & Ref : OutRef->GetDrawerDoorShapes())
						{
							bIsOk = bCopyShapeGuid ? Ref->GetShapeCopyFromGuid() == SrcDoorGuid : Ref->GetShapeGuid() == SrcDoorGuid;
							if (bIsOk)
							{
								RetResult = ReplaceDoorGroupMetal(Ref.Get(), MetalId);
								break;
							}
						}
						if (bIsOk) break;
					}
				}
			}
			else if (SpaceShape->GetInsideSpace().IsValid())
			{
				TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
				FParseSpaceShape Exc(InSideShape, MetalId, SrcDoorGuid, RetResult, bCopyShapeGuid);
				Exc();
			}
			for (auto Ref : SpaceShape->GetChildSpaceShapes())
			{
				FParseSpaceShape Exc(Ref, MetalId, SrcDoorGuid, RetResult, bCopyShapeGuid);
				Exc();
			}

		}
	private:
		TSharedPtr<FSpaceShape> & SpaceShape;
		const int32& MetalId;
		bool & RetResult;
		const FGuid & SrcDoorGuid;
		const bool &bCopyShapeGuid;
	};

	TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
	check(TopSpace.IsValid());
	FParseSpaceShape Exc(TopSpace, InMetalId,InGuid, RetResult, bIsCopyGuid);
	Exc();
	return RetResult;
}

uint32 FSCTShapeUtilityTool::ReplaceCabinetAllDoorGroupMetal(FSCTShape * InCabinetShape, const int32 InDoorGroupTypeId, const int32 InMetalId)
{
	check(InCabinetShape && InCabinetShape->GetShapeType() == ST_Cabinet);
	// 查找所有的掩门门组顶层空间，然后进行批量门组替换
	FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
	uint32 RetErrorNumner = 0;
	struct FParseSpaceShape : public FNoncopyable
	{
	public:
		FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape, const int32 & InDoorGroupId,
			uint32 & OutReplaceErrorCount, const int32 & InMetalId)
			: SpaceShape(InSpaceShape)
			, DoorGroupId(InDoorGroupId)
			, ReplaceErrorCount(OutReplaceErrorCount)			
			, MetalId(InMetalId) {}
		void operator()()
		{
			if (SpaceShape->GetDoorShape().IsValid())
			{
				// 目前只处理掩门
				if (SpaceShape->GetDoorShape()->GetShapeType() != ST_SideHungDoor &&
					SpaceShape->GetDoorShape()->GetShapeType() != ST_DrawerDoor) return;
				if (SpaceShape->GetDoorShape()->GetShapeId() != DoorGroupId)
				{
					++ReplaceErrorCount;
				}
				else
				{
					ReplaceDoorGroupMetal(SpaceShape->GetDoorShape().Get(), MetalId);
				}
			}
			else if (SpaceShape->GetInsideSpace().IsValid())
			{
				TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
				FParseSpaceShape Exc(InSideShape, DoorGroupId, ReplaceErrorCount, MetalId);
				Exc();
			}
			for (auto Ref : SpaceShape->GetChildSpaceShapes())
			{
				FParseSpaceShape Exc(Ref, DoorGroupId, ReplaceErrorCount, MetalId);
				Exc();
			}

		}
	private:
		TSharedPtr<FSpaceShape> & SpaceShape;
		const int32 &  DoorGroupId;
		uint32 & ReplaceErrorCount;		
		const int32 & MetalId;		
	};

	TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
	check(TopSpace.IsValid());		
	FParseSpaceShape Exc(TopSpace, InDoorGroupTypeId, RetErrorNumner, InMetalId);
	Exc();
	return RetErrorNumner;
}

void FSCTShapeUtilityTool::GetCabinetAllInsertionBoard(FSCTShape * InCabinetShape, TArray<FInsertionBoard*>& OutInsertionBoardArray)
{	
	check(InCabinetShape && InCabinetShape->GetShapeType() == ST_Cabinet);
	OutInsertionBoardArray.Empty();
	// 查找所有电器饰条组
	FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
	uint32 RetSuccessedNumber = 0;
	struct FParseSpaceShape : public FNoncopyable
	{
	public:
		FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape,
			uint32 & OutReplaceErrorCount,
			TArray<FInsertionBoard*> & OutInsertionBoardArray)
			: SpaceShape(InSpaceShape)
			, ReplaceErrorCount(OutReplaceErrorCount)
			, InsertionBoardArray(OutInsertionBoardArray)
			{}
		void operator()()
		{
			if (SpaceShape->IsEmbededElectricalGroup())
			{						
				TSharedPtr<FEmbededElectricalGroup> EmbededElectricalGroup = SpaceShape->GetEmbededElecticalGroup();
				if (EmbededElectricalGroup->GetInsertionBoardShape().IsValid())
				{			
					InsertionBoardArray.Add(EmbededElectricalGroup->GetInsertionBoardShape().Get());
					++ReplaceErrorCount;
				}				
			}
			else if (SpaceShape->GetInsideSpace().IsValid())
			{
				TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
				FParseSpaceShape Exc(InSideShape, ReplaceErrorCount, InsertionBoardArray);
				Exc();
			}
			for (auto Ref : SpaceShape->GetChildSpaceShapes())
			{
				FParseSpaceShape Exc(Ref, ReplaceErrorCount, InsertionBoardArray);
				Exc();
			}

		}
	private:
		TSharedPtr<FSpaceShape> & SpaceShape;
		uint32 & ReplaceErrorCount;
		TArray<FInsertionBoard*> & InsertionBoardArray;
	};

	TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
	check(TopSpace.IsValid());
	FParseSpaceShape Exc(TopSpace, RetSuccessedNumber, OutInsertionBoardArray);
	Exc();	
}

void FSCTShapeUtilityTool::GetCabinetAllDoorSequenceNum(FSCTShape * InCabinetShape, TArray<TPair<FSCTShape*, int32>>& OutDoorSequenceNum)
{
	check(InCabinetShape && InCabinetShape->GetShapeType() == ST_Cabinet);
	OutDoorSequenceNum.Empty();
	// 查找所有门板以及抽面
	FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);	
	struct FParseSpaceShape : public FNoncopyable
	{
	public:
		FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape,			
			TArray<TPair<FSCTShape*, TPair<float, float>>> & OutDoorSheets)
			: SpaceShape(InSpaceShape)			
			, DoorSheets(OutDoorSheets)
		{}
		void operator()()
		{
			if (SpaceShape->GetDoorShape().IsValid())
			{
				if (SpaceShape->GetDoorShape()->GetShapeType() == ST_SideHungDoor)
				{
					TSharedPtr<FSideHungDoor> SideHungDoor = StaticCastSharedPtr<FSideHungDoor>(SpaceShape->GetDoorShape());
					check(SideHungDoor.IsValid());
					TPair<float,float> DoorGroupPosition = TPair<float,float>(SpaceShape->GetSpaceAbsolutePosX() ,SpaceShape->GetSpaceAbsolutePosZ() );					
					DoorSheets.Emplace(TPair<FSCTShape*, TPair<float, float>>(SideHungDoor.Get(), DoorGroupPosition));
				}												 
			}
			else if (SpaceShape->IsDrawerGroupSpace())
			{
				TSharedPtr<FDrawerGroupShape> DrawerGroupShape = SpaceShape->GetCoveredDrawerGroup();								
				check(DrawerGroupShape.IsValid());
				FVector DoorGroupPosition = { SpaceShape->GetSpaceAbsolutePosX() ,SpaceShape->GetSpaceAbsolutePosY(),SpaceShape->GetSpaceAbsolutePosZ() };
				TPair<float, float> XZPosition = TPair<float, float>((DrawerGroupShape->GetPrimitiveDrawer()->GetShapePosX()) + DoorGroupPosition.X, DrawerGroupShape->GetPrimitiveDrawer()->GetShapePosZ() + DoorGroupPosition.Z);
				if (DrawerGroupShape->GetPrimitiveDrawer()->GetDrawerDoorShapes().Num() > 0)
				{
					DoorSheets.Emplace(TPair<FSCTShape*, TPair<float, float>>(DrawerGroupShape->GetPrimitiveDrawer()->GetDrawerDoorShapes()[0].Get(), XZPosition));
				}			
				for (const auto & Ref : DrawerGroupShape->GetCopyDrawerShapes())
				{	
					XZPosition = TPair<float, float>((Ref->GetShapePosX()) + DoorGroupPosition.X, Ref->GetShapePosZ() + DoorGroupPosition.Z);
					if (Ref->GetDrawerDoorShapes().Num() > 0)
					{
						TSharedPtr<FDrawerDoorShape> DrawerDoorShape = Ref->GetDrawerDoorShapes()[0];
						DoorSheets.Emplace(TPair<FSCTShape*, TPair<float, float>>(DrawerDoorShape.Get(), XZPosition));
					}
				}				
			}
			else if (SpaceShape->GetInsideSpace().IsValid())
			{
				TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
				FParseSpaceShape Exc(InSideShape, DoorSheets);
				Exc();
			}
			for (auto Ref : SpaceShape->GetChildSpaceShapes())
			{
				FParseSpaceShape Exc(Ref, DoorSheets);
				Exc();
			}

		}
	private:
		TSharedPtr<FSpaceShape> & SpaceShape;		
		TArray<TPair<FSCTShape*, TPair<float, float>>> & DoorSheets;
	};

	TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
	check(TopSpace.IsValid());
	TArray<TPair<FSCTShape*, TPair<float, float>>> FindedDoorSheets;
	FParseSpaceShape Exc(TopSpace, FindedDoorSheets);
	Exc();

	// 对已经查找到的所有掩门以及抽面按照，上下，左右的顺序进行排序
	FindedDoorSheets.Sort([](const TPair<FSCTShape*, TPair<float, float>> & Lhs, const TPair<FSCTShape*, TPair<float, float>> & Rhs)->bool {

		bool Result = false;
		if (FMath::IsNearlyEqual(Lhs.Value.Value,Rhs.Value.Value))
		{
			Result = Lhs.Value.Key < Rhs.Value.Key;
		}
		else
		{
			Result = Lhs.Value.Value > Rhs.Value.Value;
		}
		return 	Result;
	});
	int32 Index = 0;
	for (const auto & Ref : FindedDoorSheets)
	{
		OutDoorSequenceNum.Emplace(Ref.Key,Index);
	}
}

FString FSCTShapeUtilityTool::GetCabinetAllDoorSequenceNumAsJsonObjectStr(FSCTShape * InCabinetShape)
{
	FString RetStr;
	TArray<TPair<FSCTShape*, int32>> DoorSequenceNum;
	do
	{
		bool bResult = false;
		GetCabinetAllDoorSequenceNum(InCabinetShape, DoorSequenceNum);
		bResult = DoorSequenceNum.Num() != 0;
		if (!bResult) break;

		TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> SaveJsonWriter =
			TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&RetStr);	
		SaveJsonWriter->WriteObjectStart();
		SaveJsonWriter->WriteArrayStart(TEXT("doorItems"));
		for (const auto & Ref : DoorSequenceNum)
		{
			SaveJsonWriter->WriteObjectStart();
			SaveJsonWriter->WriteValue(TEXT("id"), Ref.Key->GetShapeId());
			if (Ref.Key->GetShapeType() == ST_SideHungDoor)
			{
				FSideHungDoor* SideHungDoorShape = StaticCast<FSideHungDoor*>(Ref.Key);				
				int32 Type = 0;
				int32 OpenDoorDirection = 0;
				check(SideHungDoorShape->GetDoorSheetNum() > 0);
				TSharedPtr<FVariableAreaDoorSheet> DoorSheet = SideHungDoorShape->GetDoorSheets()[0];
				FVariableAreaDoorSheet::EDoorOpenDirection DoorDirection = DoorSheet->GetOpenDoorDirection();
				if(SideHungDoorShape->GetDoorSheetNum() == 1)
				{	
					Type = DoorDirection == FVariableAreaDoorSheet::EDoorOpenDirection::E_Top ? 3 : 2;
					OpenDoorDirection = StaticCast<int32>(DoorDirection);					
				}	
				else if(SideHungDoorShape->GetDoorSheetNum() == 2)
				{
					Type = 1;
					OpenDoorDirection = 0;
				}
				SaveJsonWriter->WriteValue(TEXT("materialId"), DoorSheet->GetMaterial().ID);
				SaveJsonWriter->WriteValue(TEXT("openingDirection"), StaticCast<int32>(DoorDirection));
				SaveJsonWriter->WriteValue(TEXT("type"), Type);				
				SaveJsonWriter->WriteValue(TEXT("shapeGuid"), SideHungDoorShape->GetShapeGuid().ToString());
			}
			else if (Ref.Key->GetShapeType() == ST_DrawerDoor)
			{
				FDrawerDoorShape * DrawerDoorShape = StaticCast<FDrawerDoorShape*>(Ref.Key);			
				TSharedPtr<FVariableAreaDoorSheet> DoorSheet = DrawerDoorShape->GetDrawDoorSheet();								
				SaveJsonWriter->WriteValue(TEXT("materialId"), DoorSheet->GetMaterial().ID);	
				SaveJsonWriter->WriteValue(TEXT("type"), 4);
				SaveJsonWriter->WriteValue(TEXT("shapeGuid"), DrawerDoorShape->GetShapeGuid().ToString());
			}
			SaveJsonWriter->WriteObjectEnd();
		}
		SaveJsonWriter->WriteArrayEnd();
		SaveJsonWriter->WriteObjectEnd();
		SaveJsonWriter->Close();

		{
			TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(RetStr);
			TSharedPtr<FJsonObject> JsonObj;
			bResult = FJsonSerializer::Deserialize(JsonReader, JsonObj);
			check(bResult);

			const TArray<TSharedPtr<FJsonValue>> & JsonArrayRef = JsonObj->GetArrayField(TEXT("doorItems"));
			FString JsonArrayStr;
			TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> DoorItemArrayWriter =
				TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonArrayStr);
			FJsonSerializer::Serialize(JsonArrayRef, DoorItemArrayWriter);
			DoorItemArrayWriter->Close();
			RetStr = JsonArrayStr;
		}

	} while (false);
	return RetStr;
}

bool FSCTShapeUtilityTool::ReplaceEmbedElectricalGroup(FSCTShape * InEmbedElectrical, FSCTShape * InDesEmbedElectrical)
{
	check(InEmbedElectrical && InEmbedElectrical->GetShapeType() == ST_EmbeddedElectricalGroup);
	check(InDesEmbedElectrical && InDesEmbedElectrical->GetShapeType() == ST_Accessory && InDesEmbedElectrical->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_EMBEDDED_APPLIANCE));
	TSharedPtr<FAccessoryShape> NewEmbedElectrical = MakeShareable(new FAccessoryShape);
	InDesEmbedElectrical->CopyTo(NewEmbedElectrical.Get());
	NewEmbedElectrical->LoadModelMesh();
	FEmbededElectricalGroup * EmbededElectricalGroup = StaticCast<FEmbededElectricalGroup *>(InEmbedElectrical);
	EmbededElectricalGroup->SetEmbededEletricalShape(NewEmbedElectrical);
	check(EmbededElectricalGroup->GetShapeActor());
	AActor * NewActor = NewEmbedElectrical->SpawnShapeActor();
	NewActor->AttachToActor(EmbededElectricalGroup->GetShapeActor(),FAttachmentTransformRules::KeepRelativeTransform);
	EmbededElectricalGroup->UpdateEmbededEletricalShape();
	return true;
}

bool FSCTShapeUtilityTool::ReplaceEmbedElectricalGroupWithGuid(FSCTShape * InCabinetShape, const FGuid & InGuid, FSCTShape * InDesEmbedElectrical, const bool bIsCopyGuid)
{
	check(InCabinetShape && InCabinetShape->GetShapeType() == ST_Cabinet);	
	check(InDesEmbedElectrical && InDesEmbedElectrical->GetShapeType() == ST_Accessory);
	bool bRetResult = false;
	do
	{
		// 查找所有的嵌入式电器
		FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
		struct FParseSpaceShape : public FNoncopyable
		{
		public:
			FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape,
				FEmbededElectricalGroup * & OutEmbedelectricalGroup,const bool & bInIsCopyGuid,const FGuid & InGuid)
				: SpaceShape(InSpaceShape)
				, EmbedelectricalGroup(OutEmbedelectricalGroup)
				, bIsCopyGuid(bInIsCopyGuid)
				, Guid(InGuid)
			{}
			void operator()()
			{
				if (SpaceShape->IsEmbededElectricalGroup())
				{
					TSharedPtr<FEmbededElectricalGroup> EmbededElectricalGroup = SpaceShape->GetEmbededElecticalGroup();					
					if (EmbededElectricalGroup.IsValid() && (bIsCopyGuid ? (EmbededElectricalGroup->GetShapeCopyFromGuid() == Guid) : (EmbededElectricalGroup->GetShapeGuid() == Guid)) )
					{
						EmbedelectricalGroup = EmbededElectricalGroup.Get();
					}
				}
				else if (SpaceShape->GetInsideSpace().IsValid())
				{
					TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
					FParseSpaceShape Exc(InSideShape, EmbedelectricalGroup, bIsCopyGuid, Guid);
					Exc();
				}
				for (auto Ref : SpaceShape->GetChildSpaceShapes())
				{
					FParseSpaceShape Exc(Ref, EmbedelectricalGroup, bIsCopyGuid, Guid);
					Exc();
				}

			}
		private:
			TSharedPtr<FSpaceShape> & SpaceShape;
			FEmbededElectricalGroup * & EmbedelectricalGroup;
			const bool & bIsCopyGuid;
			const FGuid & Guid;
		};

		TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
		check(TopSpace.IsValid());
		FEmbededElectricalGroup * EmbededElectricalGroup = nullptr;
		FParseSpaceShape Exc(TopSpace, EmbededElectricalGroup,true, InGuid);
		Exc();
		bRetResult = EmbededElectricalGroup != nullptr;
		if (!bRetResult) break;
		ReplaceEmbedElectricalGroup(EmbededElectricalGroup, InDesEmbedElectrical);
	} while (false);
	return bRetResult;
}

void FSCTShapeUtilityTool::GetCabinetAllEmbedElectrical(FSCTShape * InCabinetShape, TArray<FAccessoryShape*>& OutAccessoryArray)
{

	check(InCabinetShape && InCabinetShape->GetShapeType() == ST_Cabinet);
	OutAccessoryArray.Empty();
	// 查找所有的嵌入式电器
	FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
	uint32 RetSuccessedNumber = 0;
	struct FParseSpaceShape : public FNoncopyable
	{
	public:
		FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape,
			uint32 & OutReplaceErrorCount,
			TArray<FAccessoryShape*> & OutInsertionBoardArray)
			: SpaceShape(InSpaceShape)
			, ReplaceErrorCount(OutReplaceErrorCount)
			, AccessShapeArray(OutInsertionBoardArray)
		{}
		void operator()()
		{
			if (SpaceShape->IsEmbededElectricalGroup())
			{
				TSharedPtr<FEmbededElectricalGroup> EmbededElectricalGroup = SpaceShape->GetEmbededElecticalGroup();
				if (EmbededElectricalGroup->GetEmbededEletricalShape().IsValid())
				{
					AccessShapeArray.Add(EmbededElectricalGroup->GetEmbededEletricalShape().Get());
					++ReplaceErrorCount;
				}
			}
			else if (SpaceShape->GetInsideSpace().IsValid())
			{
				TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
				FParseSpaceShape Exc(InSideShape, ReplaceErrorCount, AccessShapeArray);
				Exc();
			}
			for (auto Ref : SpaceShape->GetChildSpaceShapes())
			{
				FParseSpaceShape Exc(Ref, ReplaceErrorCount, AccessShapeArray);
				Exc();
			}

		}
	private:
		TSharedPtr<FSpaceShape> & SpaceShape;
		uint32 & ReplaceErrorCount;
		TArray<FAccessoryShape*> & AccessShapeArray;
	};

	TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
	check(TopSpace.IsValid());
	FParseSpaceShape Exc(TopSpace, RetSuccessedNumber, OutAccessoryArray);
	Exc();
}

bool FSCTShapeUtilityTool::ModifyCabinetFrontBoardSize(FSCTShape * InCabinetShape, const float InStartPoint, const float InEndPoint)
{
	check(InCabinetShape && InCabinetShape->GetShapeType() == ST_Cabinet);
	bool bRetResult = false;
	do	
	{
		const float CabinetWidth = InCabinetShape->GetShapeWidth();
		const bool bIsStartAndEndPointValid = FMath::IsNearlyZero(InStartPoint) ? true : (FMath::IsNearlyEqual(InEndPoint, CabinetWidth) ? true : false);
		check(bIsStartAndEndPointValid);		
		FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
		struct FParseSpaceShape : public FNoncopyable
		{
		public:
			FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape,
				FSCTShape * & OutDoorGroup)
				: SpaceShape(InSpaceShape)
				, DoorGroup(OutDoorGroup)
			{}
			void operator()()
			{
				if (SpaceShape->GetDoorShape().IsValid())
				{
					if (SpaceShape->GetDoorShape()->GetShapeType() == ST_SideHungDoor)
					{
						TSharedPtr<FSideHungDoor> SideHungDoor = StaticCastSharedPtr<FSideHungDoor>(SpaceShape->GetDoorShape());
						check(SideHungDoor.IsValid());
						if (SideHungDoor->GetFrontBoardType() != FSideHungDoor::EFrontBoardType::E_None)
						{
							DoorGroup = SideHungDoor.Get();
							return;
						}
					}
				}				
				else if (SpaceShape->GetInsideSpace().IsValid())
				{
					TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
					FParseSpaceShape Exc(InSideShape, DoorGroup);
					Exc();
				}
				for (auto Ref : SpaceShape->GetChildSpaceShapes())
				{
					FParseSpaceShape Exc(Ref, DoorGroup);
					Exc();
				}

			}
		private:
			TSharedPtr<FSpaceShape> & SpaceShape;
			FSCTShape * & DoorGroup;
		};
		
		TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
		check(TopSpace.IsValid());
		FSCTShape * DoorGroupWithFrontBoard = nullptr;
		FParseSpaceShape Exc(TopSpace, DoorGroupWithFrontBoard);
		Exc();
		bRetResult = DoorGroupWithFrontBoard != nullptr;
		if (!bRetResult) break;
		// 进行遮挡尺寸有效性检查
		{
			const bool bIsDesLeftFrontBoard = FMath::IsNearlyZero(InStartPoint);
			FSideHungDoor * SideHungDoorGroup = StaticCast<FSideHungDoor *>(DoorGroupWithFrontBoard);
			TSharedPtr<FShapeAttribute> WidthAttri = SideHungDoorGroup->GetDoorSheets()[0]->GetShapeWidthAttri();
			float MinW = 0.0f, MaxW = 0.0f;
			bRetResult = WidthAttri->GetAttributeType() == SAT_NumberRange;
			if (!bRetResult) break;
			MinW = StaticCastSharedPtr<FNumberRangeAttri>(WidthAttri)->GetMinValue();
			MaxW = StaticCastSharedPtr<FNumberRangeAttri>(WidthAttri)->GetMaxValue();
			// 计算除了封板之外，门板的最大最小尺寸范围
			const float DoorGroupWidth = SideHungDoorGroup->GetShapeWidth() + SideHungDoorGroup->GetLeftExtensionValue() + SideHungDoorGroup->GetRightExtensionValue();
			// 只处理封板所在侧全盖或者半盖的情况
			bRetResult = bIsDesLeftFrontBoard ? (SideHungDoorGroup->GetLeftCorverType() == 1 || SideHungDoorGroup->GetLeftCorverType() == 2) : (SideHungDoorGroup->GetRightCorverType() == 1 || SideHungDoorGroup->GetRightCorverType() == 2);
			if (!bRetResult) break;
			// 封板区域最大值
			const float AvaliableDoorSheetWidthMax = FMath::Max(DoorGroupWidth - ( 2 * SideHungDoorGroup->GetLeftRightGapValue()  + MinW), SideHungDoorGroup->GetFrontBoardMinWidth());
			const float AvaliableDoorSheetWidthMin = FMath::Max(DoorGroupWidth - (2 * (2 * SideHungDoorGroup->GetLeftRightGapValue() + MaxW)), SideHungDoorGroup->GetFrontBoardMinWidth());
			const float CoverWitdth = bIsDesLeftFrontBoard ?
				(SideHungDoorGroup->GetLeftCorverType() == 1 ? SideHungDoorGroup->GetLeftExtensionValue() : 2.0f * SideHungDoorGroup->GetLeftExtensionValue()) :
				(SideHungDoorGroup->GetRightCorverType() == 1 ? SideHungDoorGroup->GetRightExtensionValue() : 2.0f * SideHungDoorGroup->GetRightExtensionValue());
			const float TempFrontBoardWitht = (InEndPoint - InStartPoint) - CoverWitdth;
			bRetResult = TempFrontBoardWitht <= AvaliableDoorSheetWidthMax && TempFrontBoardWitht >= AvaliableDoorSheetWidthMin;
			if (!bRetResult) break;		
			SideHungDoorGroup->SetFrontBoardPositionType(bIsDesLeftFrontBoard ? FSideHungDoor::EFrontBoardPositionType::E_DoorLeft : FSideHungDoor::EFrontBoardPositionType::E_DoorRight);			
			if (SideHungDoorGroup->GetFrontBoardCornerType() != FSideHungDoor::EFrontBoardCornerType::E_None)
			{
				SideHungDoorGroup->SetFrontBoardCornerType(bIsDesLeftFrontBoard ? FSideHungDoor::EFrontBoardCornerType::E_RithtTopCorner : FSideHungDoor::EFrontBoardCornerType::E_LeftTopCorner);
			}
			bRetResult = SideHungDoorGroup->SetFrontBoardWidth(TempFrontBoardWitht - (bIsDesLeftFrontBoard ? SideHungDoorGroup->GetLeftExtensionValue() : SideHungDoorGroup->GetRightExtensionValue()));
			if (!bRetResult) break;
			
			

			// 插角封板尺寸发生变化后，重新计算门组中门板的数量

			const bool bSingleDoor = SideHungDoorGroup->GetDoorSheetNum() == 1;
			const bool bDoubleDoor = SideHungDoorGroup->GetDoorSheetNum() == 2;
			const float DoorSheetWidthExtenSize = SideHungDoorGroup->GetFrontBoardPositionType() == FSideHungDoor::EFrontBoardPositionType::E_DoorLeft ?
				(SideHungDoorGroup->GetRightExtensionValue()) :
				(SideHungDoorGroup->GetLeftExtensionValue());

			const float DoorSheetsWidth = SideHungDoorGroup->GetShapeWidth() - SideHungDoorGroup->GetFrontBoardWidth() + DoorSheetWidthExtenSize;

			bool bSecondCheckSingleDoor = bSingleDoor;
			bool bSecondCheckDoubleDoor = bDoubleDoor;
			if (bSingleDoor )
			{
				const float TempDoorSheetWidth = DoorSheetsWidth - (2.0f * SideHungDoorGroup->GetLeftRightGapValue());
				bSecondCheckSingleDoor = TempDoorSheetWidth >= MinW && TempDoorSheetWidth <= MaxW;				
			}
			if (bDoubleDoor)
			{
				const float TempDoorSheetWidth = (DoorSheetsWidth -  2.0f * (2.0f * SideHungDoorGroup->GetLeftRightGapValue())) / 2.0f;
				bSecondCheckDoubleDoor = TempDoorSheetWidth >= MinW && TempDoorSheetWidth <= MaxW;
			}
			if (bSingleDoor && !bSecondCheckSingleDoor)
			{
				bRetResult = SideHungDoorGroup->SetDoorSheetNum(2);
				if (!bRetResult) break;
			}
			
			if (bDoubleDoor && !bSecondCheckDoubleDoor)
			{
				bRetResult = SideHungDoorGroup->SetDoorSheetNum(1);
				if (!bRetResult) break;
			}

		}

	} while (false);
	return bRetResult;
}

bool FSCTShapeUtilityTool::GetFrontBoardCabinetDoorModifyInfo(FSCTShape * InCabinetShape, bool & OutSingleDoor, bool & OutDoubleDoor, float & OutDoorSheetsWidth,bool & OutCurIsSingleDoor, bool & OutCurIsLeft)
{
	check(InCabinetShape && InCabinetShape->GetShapeType() == ST_Cabinet);
	bool bRetResult = false;
	do
	{	
		FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
		struct FParseSpaceShape : public FNoncopyable
		{
		public:
			FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape,
				FSCTShape * & OutDoorGroup)
				: SpaceShape(InSpaceShape)
				, DoorGroup(OutDoorGroup)
			{}
			void operator()()
			{
				if (SpaceShape->GetDoorShape().IsValid())
				{
					if (SpaceShape->GetDoorShape()->GetShapeType() == ST_SideHungDoor)
					{
						TSharedPtr<FSideHungDoor> SideHungDoor = StaticCastSharedPtr<FSideHungDoor>(SpaceShape->GetDoorShape());
						check(SideHungDoor.IsValid());
						if (SideHungDoor->GetFrontBoardType() != FSideHungDoor::EFrontBoardType::E_None)
						{
							DoorGroup = SideHungDoor.Get();
							return;
						}
					}
				}
				else if (SpaceShape->GetInsideSpace().IsValid())
				{
					TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
					FParseSpaceShape Exc(InSideShape, DoorGroup);
					Exc();
				}
				for (auto Ref : SpaceShape->GetChildSpaceShapes())
				{
					FParseSpaceShape Exc(Ref, DoorGroup);
					Exc();
				}

			}
		private:
			TSharedPtr<FSpaceShape> & SpaceShape;
			FSCTShape * & DoorGroup;
		};

		TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
		check(TopSpace.IsValid());
		FSCTShape * DoorGroupWithFrontBoard = nullptr;
		FParseSpaceShape Exc(TopSpace, DoorGroupWithFrontBoard);
		Exc();
		bRetResult = DoorGroupWithFrontBoard != nullptr;
		if (!bRetResult) break;
		// 收集可修改值
		{	
			FSideHungDoor * SideHungDoorGroup = StaticCast<FSideHungDoor *>(DoorGroupWithFrontBoard);
			const float DoorSheetWidthExtenSize = SideHungDoorGroup->GetFrontBoardPositionType() == FSideHungDoor::EFrontBoardPositionType::E_DoorLeft ?
				(SideHungDoorGroup->GetRightExtensionValue()) :
				(SideHungDoorGroup->GetLeftExtensionValue()) ;
				
			OutDoorSheetsWidth = SideHungDoorGroup->GetShapeWidth() - SideHungDoorGroup->GetFrontBoardWidth() + DoorSheetWidthExtenSize;

			// 先根据当前门板数量，确定是打开门还是对开门
			const bool bSingleDoor = SideHungDoorGroup->GetDoorSheetNum() == 1;
			const bool bDoubleDoor = SideHungDoorGroup->GetDoorSheetNum() == 2;
			
			TSharedPtr<FShapeAttribute> WidthAttri = SideHungDoorGroup->GetDoorSheets()[0]->GetShapeWidthAttri();
			float MinW = 0.0f, MaxW = 0.0f;
			bRetResult = WidthAttri->GetAttributeType() == SAT_NumberRange;
			if (!bRetResult) break;
			MinW = StaticCastSharedPtr<FNumberRangeAttri>(WidthAttri)->GetMinValue();
			MaxW = StaticCastSharedPtr<FNumberRangeAttri>(WidthAttri)->GetMaxValue();

			// 如果是单开门，则判断是否可以转换成对开门
			bool SecondCheckSingleDoor = bSingleDoor;
			bool SecondCheckDoubleDoor = bDoubleDoor;
			if (bSingleDoor)
			{
				// 计算双开门情况下每扇门门板尺寸
				const float TempDoorSheetWidth = (OutDoorSheetsWidth - 2.0f * (2.0f * SideHungDoorGroup->GetLeftRightGapValue())) / 2.0f;
				if (TempDoorSheetWidth >= MinW && TempDoorSheetWidth <= MaxW)
				{
					SecondCheckDoubleDoor = true;					
				}
			}
			// 如果是对开门，则判断是否可以转换成单开门
			if (bDoubleDoor)
			{
				const float TempDoorSheetWidth = OutDoorSheetsWidth - (2.0f * SideHungDoorGroup->GetLeftRightGapValue());
				if (TempDoorSheetWidth >= MinW && TempDoorSheetWidth <= MaxW)
				{
					SecondCheckSingleDoor = true;					
				}
			}
			
			OutSingleDoor = SecondCheckSingleDoor;
			OutDoubleDoor = SecondCheckDoubleDoor;
			OutCurIsSingleDoor = SideHungDoorGroup->GetDoorSheetNum() == 1;
			OutCurIsLeft = SideHungDoorGroup->GetFrontBoardPositionType() == FSideHungDoor::EFrontBoardPositionType::E_DoorLeft;
		}

	} while (false);
	return bRetResult;
}

bool FSCTShapeUtilityTool::ModifyForntBoardCabinetDoorSheetNum(FSCTShape * InCabinetShape, const int32 InDoorSheetNum)
{
	check(InCabinetShape && InCabinetShape->GetShapeType() == ST_Cabinet);
	check(InDoorSheetNum == 1 || InDoorSheetNum == 2);
	bool bRetResult = false;
	do
	{
		FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
		struct FParseSpaceShape : public FNoncopyable
		{
		public:
			FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape,
				FSCTShape * & OutDoorGroup)
				: SpaceShape(InSpaceShape)
				, DoorGroup(OutDoorGroup)
			{}
			void operator()()
			{
				if (SpaceShape->GetDoorShape().IsValid())
				{
					if (SpaceShape->GetDoorShape()->GetShapeType() == ST_SideHungDoor)
					{
						TSharedPtr<FSideHungDoor> SideHungDoor = StaticCastSharedPtr<FSideHungDoor>(SpaceShape->GetDoorShape());
						check(SideHungDoor.IsValid());
						if (SideHungDoor->GetFrontBoardType() != FSideHungDoor::EFrontBoardType::E_None)
						{
							DoorGroup = SideHungDoor.Get();
							return;
						}
					}
				}
				else if (SpaceShape->GetInsideSpace().IsValid())
				{
					TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
					FParseSpaceShape Exc(InSideShape, DoorGroup);
					Exc();
				}
				for (auto Ref : SpaceShape->GetChildSpaceShapes())
				{
					FParseSpaceShape Exc(Ref, DoorGroup);
					Exc();
				}

			}
		private:
			TSharedPtr<FSpaceShape> & SpaceShape;
			FSCTShape * & DoorGroup;
		};

		TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
		check(TopSpace.IsValid());
		FSCTShape * DoorGroupWithFrontBoard = nullptr;
		FParseSpaceShape Exc(TopSpace, DoorGroupWithFrontBoard);
		Exc();
		bRetResult = DoorGroupWithFrontBoard != nullptr;
		if (!bRetResult) break;
		bRetResult = StaticCast<FSideHungDoor*>(DoorGroupWithFrontBoard)->SetDoorSheetNum(InDoorSheetNum);
		if (!bRetResult) break;
	}
	while (false);
	return bRetResult;
}

bool FSCTShapeUtilityTool::ModifyFrontBoardCabinetDoorWidth(FSCTShape * InCabinetShape, const float InDoorWidth)
{
	check(InCabinetShape && InCabinetShape->GetShapeType() == ST_Cabinet);
	bool bRetResult = false;
	do
	{
		FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
		struct FParseSpaceShape : public FNoncopyable
		{
		public:
			FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape,
				FSCTShape * & OutDoorGroup)
				: SpaceShape(InSpaceShape)
				, DoorGroup(OutDoorGroup)
			{}
			void operator()()
			{
				if (SpaceShape->GetDoorShape().IsValid())
				{
					if (SpaceShape->GetDoorShape()->GetShapeType() == ST_SideHungDoor)
					{
						TSharedPtr<FSideHungDoor> SideHungDoor = StaticCastSharedPtr<FSideHungDoor>(SpaceShape->GetDoorShape());
						check(SideHungDoor.IsValid());
						if (SideHungDoor->GetFrontBoardType() != FSideHungDoor::EFrontBoardType::E_None)
						{
							DoorGroup = SideHungDoor.Get();
							return;
						}
					}
				}
				else if (SpaceShape->GetInsideSpace().IsValid())
				{
					TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
					FParseSpaceShape Exc(InSideShape, DoorGroup);
					Exc();
				}
				for (auto Ref : SpaceShape->GetChildSpaceShapes())
				{
					FParseSpaceShape Exc(Ref, DoorGroup);
					Exc();
				}

			}
		private:
			TSharedPtr<FSpaceShape> & SpaceShape;
			FSCTShape * & DoorGroup;
		};

		TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
		check(TopSpace.IsValid());
		FSCTShape * DoorGroupWithFrontBoard = nullptr;
		FParseSpaceShape Exc(TopSpace, DoorGroupWithFrontBoard);
		Exc();
		bRetResult = DoorGroupWithFrontBoard != nullptr;
		if (!bRetResult) break;
		// 收集可修改值
		{
			FSideHungDoor * SideHungDoorGroup = StaticCast<FSideHungDoor *>(DoorGroupWithFrontBoard);
			const float DoorSheetWidthExtenSize = SideHungDoorGroup->GetFrontBoardPositionType() == FSideHungDoor::EFrontBoardPositionType::E_DoorLeft ?
				(SideHungDoorGroup->GetRightExtensionValue()) :
				(SideHungDoorGroup->GetLeftExtensionValue());

			const float DoorSheetWidthWithoutExten = InDoorWidth - DoorSheetWidthExtenSize;
			bRetResult = (SideHungDoorGroup->GetShapeWidth() - DoorSheetWidthWithoutExten) >= SideHungDoorGroup->GetFrontBoardMinWidth();
			if (!bRetResult) break;

			TSharedPtr<FShapeAttribute> WidthAttri = SideHungDoorGroup->GetDoorSheets()[0]->GetShapeWidthAttri();
			float MinW = 0.0f, MaxW = 0.0f;
			bRetResult = WidthAttri->GetAttributeType() == SAT_NumberRange;
			if (!bRetResult) break;
			MinW = StaticCastSharedPtr<FNumberRangeAttri>(WidthAttri)->GetMinValue();
			MaxW = StaticCastSharedPtr<FNumberRangeAttri>(WidthAttri)->GetMaxValue();

			bool bSingleDoorSheetValid = false;
			bool DoubleDoorSheetValid = false;			
			{
				// 尝试单开门是否可以适应新尺寸
				const float TempDoorSheetWidth = InDoorWidth - (2.0f * SideHungDoorGroup->GetLeftRightGapValue());			
				bSingleDoorSheetValid = (TempDoorSheetWidth >= MinW && TempDoorSheetWidth <= MaxW);
			}			
			{
				// 尝试对开门是否还可以适应新尺寸
				const float TempDoorSheetWidth = (InDoorWidth - 2.0f * (2.0f * SideHungDoorGroup->GetLeftRightGapValue())) / 2.0f;
				DoubleDoorSheetValid = (TempDoorSheetWidth >= MinW && TempDoorSheetWidth <= MaxW);
			}
			bRetResult = bSingleDoorSheetValid || DoubleDoorSheetValid;
			if (!bRetResult) break;
			bRetResult = SideHungDoorGroup->SetFrontBoardWidth(SideHungDoorGroup->GetShapeWidth() - DoorSheetWidthWithoutExten);
			if (!bRetResult) break;
		}

	} while (false);
	return bRetResult;
}

static FSCTShapeUtilityTool::FVariableAreaMeta::EScaleType operator | (const FSCTShapeUtilityTool::FVariableAreaMeta::EScaleType Lhs,const FSCTShapeUtilityTool::FVariableAreaMeta::EScaleType Rhs)
{	
	return StaticCast<FSCTShapeUtilityTool::FVariableAreaMeta::EScaleType>(StaticCast<UINT32>(Lhs) | StaticCast<UINT32>(Rhs));
}

bool FSCTShapeUtilityTool::GetDoorSheetDividBlocks(FSCTShape * InDoorSheetShape, const float InW, const float InH, TArray<FVariableAreaMeta> & OutDividBlocks)
{
	check(InDoorSheetShape);	
	bool bRetResult = false;
	do
	{	
		OutDividBlocks.Empty();
		// 暂定掩门以及抽面
		bRetResult = InDoorSheetShape->GetShapeType() == ST_VariableAreaDoorSheet_ForDrawerDoor || InDoorSheetShape->GetShapeType() == ST_VariableAreaDoorSheet_ForSideHungDoor;
		if (!bRetResult) break;
		class FVariableAreaDoorSheetMofify : public FVariableAreaDoorSheet
		{
		public:
			FVariableAreaDoorSheetMofify(FVariableAreaDoorSheet * InShape)
			{
				StaticCastSharedPtr<FNumberRangeAttri>(Width)->SetMinValue(10.0f);				
				StaticCastSharedPtr<FNumberRangeAttri>(Width)->SetMaxValue(10000.0f);
				StaticCastSharedPtr<FNumberRangeAttri>(Height)->SetMinValue(10.0f);
				StaticCastSharedPtr<FNumberRangeAttri>(Height)->SetMaxValue(10000.0f);				
				for (const auto & Ref : InShape->GetDividBlocks())
				{
					TSharedPtr<FDividBlockShape> NewShape = MakeShareable(new FDividBlockShape);
					Ref->CopyTo(NewShape.Get());
					AddDividBlock(NewShape);
				}
			}		
		};
		FVariableAreaDoorSheet * VariableAreaDoorSheet = StaticCast<FVariableAreaDoorSheet*>(InDoorSheetShape);
		TSharedPtr<FVariableAreaDoorSheetMofify> VariableAreaDoorSheetModify = MakeShareable(new FVariableAreaDoorSheetMofify(VariableAreaDoorSheet));
		const float DesWidth = InW;
		const float DesHeight = InH;
		bRetResult = VariableAreaDoorSheetModify->SetShapeWidth(DesWidth);
		if (!bRetResult) false;
		bRetResult = VariableAreaDoorSheetModify->SetShapeHeight(DesHeight);
		if (!bRetResult) false;

		for (const auto & Ref : VariableAreaDoorSheetModify->GetDividBlocks())
		{
			FVariableAreaMeta Var;
			Var.DividBlock = FBox2D(FVector2D(Ref->GetShapePosX(), Ref->GetShapePosZ()),FVector2D(Ref->GetShapePosX() + Ref->GetShapeWidth(), Ref->GetShapePosZ() + Ref->GetShapeHeight()));
			Var.ScaleType = (Ref->GetShapeWidthAttri()->CheckIfAttributeIsFormula() ? FVariableAreaMeta::EScaleType::E_X_Scale : FVariableAreaMeta::EScaleType::E_None) |
				(Ref->GetShapeHeightAttri()->CheckIfAttributeIsFormula() ? FVariableAreaMeta::EScaleType::E_Z_Scale : FVariableAreaMeta::EScaleType::E_None);
			OutDividBlocks.Emplace(MoveTemp(Var));
		}


	} while (false);
	return bRetResult;
}

void FSCTShapeUtilityTool::GetCabinetSlideringDrawerAccess(FSCTShape * InCabinetShape, TArray<TArray<FAccessoryShape*>>& OutSlidingDrawerAccess)
{
	check(InCabinetShape && InCabinetShape->GetShapeType() == ST_Cabinet);
	OutSlidingDrawerAccess.Empty();
	// 查找所有抽屉里面的五金
	FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
	struct FParseSpaceShape : public FNoncopyable
	{
	public:
		FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape,
			TArray<TArray<FAccessoryShape*>> & OutSlidingDrawerAccess)
			: SpaceShape(InSpaceShape)
			, SlidingDrawerAccess(OutSlidingDrawerAccess)
		{}
		void operator()()
		{
			if (SpaceShape->IsDrawerGroupSpace())
			{
				TSharedPtr<FDrawerGroupShape> DrawerGroupShape = SpaceShape->GetCoveredDrawerGroup();
				check(DrawerGroupShape.IsValid());		
				auto EmplaceSlindDrawerAccess = [this](FSlidingDrawerShape * InSlidingDrawerShape)
				{
					TArray<FAccessoryShape*> AccessShapes;
					for (const auto & Ref : InSlidingDrawerShape->GetAccessoryShapes())
					{
						AccessShapes.Emplace(Ref.Get());
					}
					if (AccessShapes.Num() > 0)
					{
						SlidingDrawerAccess.Emplace(AccessShapes);
					}
				};
				if (DrawerGroupShape->GetPrimitiveDrawer()->GetDrawerDoorShapes().Num() > 0)
				{
					EmplaceSlindDrawerAccess(DrawerGroupShape->GetPrimitiveDrawer().Get());
				}
				for (const auto & Ref : DrawerGroupShape->GetCopyDrawerShapes())
				{
					EmplaceSlindDrawerAccess(Ref.Get());
				}
			}
			else if (SpaceShape->GetInsideSpace().IsValid())
			{
				TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
				FParseSpaceShape Exc(InSideShape, SlidingDrawerAccess);
				Exc();
			}
			for (auto Ref : SpaceShape->GetChildSpaceShapes())
			{
				FParseSpaceShape Exc(Ref, SlidingDrawerAccess);
				Exc();
			}

		}
	private:
		TSharedPtr<FSpaceShape> & SpaceShape;
		TArray<TArray<FAccessoryShape*>> & SlidingDrawerAccess;
	};

	TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
	check(TopSpace.IsValid());	
	FParseSpaceShape Exc(TopSpace, OutSlidingDrawerAccess);
	Exc();
}

void FSCTShapeUtilityTool::GetCabinetAllDoorHanle(FSCTShape * InCabinetShape, TArray<FAccessoryShape*>& OutDoorHanleAccess)
{
	check(InCabinetShape && InCabinetShape->GetShapeType() == ST_Cabinet);
	OutDoorHanleAccess.Empty();
	// 查找所有门板以及抽面
	FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
	struct FParseSpaceShape : public FNoncopyable
	{
	public:
		FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape,
			TArray<FAccessoryShape*> & OutDoorHanleList)
			: SpaceShape(InSpaceShape)
			, DoorHanleList(OutDoorHanleList)
		{}
		void operator()()
		{
			if (SpaceShape->GetDoorShape().IsValid())
			{
				if (SpaceShape->GetDoorShape()->GetShapeType() == ST_SideHungDoor)
				{
					TSharedPtr<FSideHungDoor> SideHungDoor = StaticCastSharedPtr<FSideHungDoor>(SpaceShape->GetDoorShape());
					check(SideHungDoor.IsValid());
					FSideHungDoor::EHandleMetalType HanleType = SideHungDoor->GetHanleType();
					switch (HanleType)
					{									
						case FSideHungDoor::EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND:
						{
							const int32 Id = SideHungDoor->GetCurrentHandleInfo().ID;
							const TArray<TSharedPtr<FAccessoryShape>> & ShapeMapRef = SideHungDoor->GetMetals();
							for (const auto & Ref : ShapeMapRef)
							{
								if (Ref->GetShapeId() == Id)
								{
									DoorHanleList.Emplace(Ref.Get());
									break;
								}
							}						
						}break;
						case FSideHungDoor::EHandleMetalType::E_MT_SHAKE_HANDS:							
						case FSideHungDoor::EHandleMetalType::E_MT_SEALING_SIDE_HANDLE:
						{
							for (const auto & Ref : SideHungDoor->GetDoorSheets())
							{
								const int32 Id = Ref->GetCurrentHandleInfo().ID;
								const TArray<TSharedPtr<FAccessoryShape>> & ShapeMapRef = Ref->GetMetals();
								for (const auto & InnerRef : ShapeMapRef)
								{
									if (InnerRef->GetShapeId() == Id)
									{
										DoorHanleList.Emplace(InnerRef.Get());
										break;
									}
								}
							}
						}break;
						default:
							break;
					}

				}
			}
			else if (SpaceShape->IsDrawerGroupSpace())
			{
				TSharedPtr<FDrawerGroupShape> DrawerGroupShape = SpaceShape->GetCoveredDrawerGroup();
				check(DrawerGroupShape.IsValid());				
				auto GetSildingDrawerHanle = [](TSharedPtr<FSlidingDrawerShape> InShape, TArray<FAccessoryShape*> &OutArray)
				{
					if (InShape->GetDrawerDoorShapes().Num() == 0) return;
					TSharedPtr<FDrawerDoorShape> DrawerShape = InShape->GetDrawerDoorShapes()[0];
					FDrawerDoorShape::EHandleMetalType HanleType = DrawerShape->GetDoorGroupHanleType();
					switch (HanleType)
					{
						case FDrawerDoorShape::EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND:
						{
							const int32 Id = DrawerShape->GetDefaultHandleInfo().ID;							
							const TArray<TSharedPtr<FAccessoryShape>> & ShapeMapRef = DrawerShape->GetMetals();
							for (const auto & InnerRef : ShapeMapRef)
							{
								if (InnerRef->GetShapeId() == Id)
								{
									OutArray.Emplace(InnerRef.Get());
									break;
								}
							}
						}break;
						case FDrawerDoorShape::EHandleMetalType::E_MT_SHAKE_HANDS:
						case FDrawerDoorShape::EHandleMetalType::E_MT_SEALING_SIDE_HANDLE:
						{							
							{
								const int32 Id = DrawerShape->GetDrawDoorSheet()->GetDefaultHandleInfo().ID;
								const TArray<TSharedPtr<FAccessoryShape>> & ShapeMapRef = DrawerShape->GetDrawDoorSheet()->GetMetals();
								for (const auto & InnerRef : ShapeMapRef)
								{
									if (InnerRef->GetShapeId() == Id)
									{
										OutArray.Emplace(InnerRef.Get());
										break;
									}
								}
							}
						}break;
						default:
							break;
					}

				};
				if (DrawerGroupShape->GetPrimitiveDrawer()->GetDrawerDoorShapes().Num() > 0)
				{
					GetSildingDrawerHanle(DrawerGroupShape->GetPrimitiveDrawer(), DoorHanleList);
				}
				for (auto & Ref : DrawerGroupShape->GetCopyDrawerShapes())
				{
					GetSildingDrawerHanle(Ref, DoorHanleList);
				}
			}
			else if (SpaceShape->GetInsideSpace().IsValid())
			{
				TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
				FParseSpaceShape Exc(InSideShape, DoorHanleList);
				Exc();
			}
			for (auto Ref : SpaceShape->GetChildSpaceShapes())
			{
				FParseSpaceShape Exc(Ref, DoorHanleList);
				Exc();
			}

		}
	private:
		TSharedPtr<FSpaceShape> & SpaceShape;
		TArray<FAccessoryShape*> & DoorHanleList;
	};

	TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
	check(TopSpace.IsValid());
	FParseSpaceShape Exc(TopSpace, OutDoorHanleAccess);
	Exc();

}

FSlidingDoor * FSCTShapeUtilityTool::GetFirstSlidingDoorGroupOfCabinet(FSCTShape * InCabinetShape)
{
	check(InCabinetShape && InCabinetShape->GetShapeType() == ST_Cabinet);	
	// 查找移门门组
	FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
	FSlidingDoor * RetValue = nullptr;
	struct FParseSpaceShape : public FNoncopyable
	{
	public:
		FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape, FSlidingDoor * & OutRetValue)
			: SpaceShape(InSpaceShape)
			, SlindDoorGroup(OutRetValue)
		{}
		void operator()()
		{
			if (SpaceShape->GetDoorShape().IsValid())
			{
				if (SpaceShape->GetDoorShape()->GetShapeType() == ST_SlidingDoor)
				{		
					SlindDoorGroup = StaticCast<FSlidingDoor*>(SpaceShape->GetDoorShape().Get());
				}
			}			
			else if (SpaceShape->GetInsideSpace().IsValid())
			{
				TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
				FParseSpaceShape Exc(InSideShape, SlindDoorGroup);
				Exc();
			}
			for (auto Ref : SpaceShape->GetChildSpaceShapes())
			{
				FParseSpaceShape Exc(Ref, SlindDoorGroup);
				Exc();
			}

		}
	private:
		TSharedPtr<FSpaceShape> & SpaceShape;
		FSlidingDoor * & SlindDoorGroup;
	};

	TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
	check(TopSpace.IsValid());
	FParseSpaceShape Exc(TopSpace, RetValue);
	Exc();
	return RetValue;
}

bool FSCTShapeUtilityTool::SetCabinetSlidingDoorDownPlateBoardValid(FSCTShape * InCabinetShape, const bool InVisiable)
{
	bool RetResult = false;
	do
	{
		FSlidingDoor * SlidingDoor = GetFirstSlidingDoorGroupOfCabinet(InCabinetShape);
		if (SlidingDoor == nullptr) break;
		RetResult = (SlidingDoor->IsDownPlateBoardValid() == InVisiable);
		if (RetResult) break;
		SlidingDoor->SetDownPlateBoardValid(InVisiable);
		RetResult = (SlidingDoor->IsDownPlateBoardValid() == InVisiable);
		if (RetResult) break;

	} while (false);
	return RetResult;
}

bool FSCTShapeUtilityTool::GetCabinetSlidingDoorDownPlateBoardIsValid(FSCTShape * InCabinetShape)
{
	bool RetResult = false;
	do
	{
		FSlidingDoor * SlidingDoor = GetFirstSlidingDoorGroupOfCabinet(InCabinetShape);
		if (SlidingDoor == nullptr) break;
		RetResult = SlidingDoor->IsDownPlateBoardValid();
	} while (false);
	return RetResult;
}

void FSCTShapeUtilityTool::GetCabinetSlidingDoorSlidways(FSCTShape * InCabinetShape, TArray<FSlidwayInfo>& Slidways)
{		
	do
	{
		FSlidingDoor * SlidingDoor = GetFirstSlidingDoorGroupOfCabinet(InCabinetShape);
		if (SlidingDoor == nullptr) break;
		int32 CurUpSlidingWayId = -1;
		if (SlidingDoor->GetUpSlideWayShape().IsValid())
		{
			CurUpSlidingWayId = SlidingDoor->GetUpSlideWayShape()->GetShapeId();
		}
		const TArray<TSharedPtr<FAccessoryShape>> & AllAccessShapes = SlidingDoor->GetAllAccessShapes();
		for (const auto & Ref : AllAccessShapes)
		{
			if (Ref->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SD_TOP_RAILS))
			{
				FSlidwayInfo TempInfo;
				if (Ref->GetShapeId() == CurUpSlidingWayId)
				{
					TempInfo.bIsUsed = true;
				}
				else
				{
					TempInfo.bIsUsed = false;
				}
				TempInfo.DisplayName = Ref->GetShapeName();
				TempInfo.Id = Ref->GetShapeId();
				TempInfo.Thumdurl = Ref->GetThumbnailUrl();
				Slidways.Emplace(TempInfo);
			}
		}
	} while (false);		
}

void FSCTShapeUtilityTool::GetCabinetSlidingDoorSlidways(FSCTShape * InCabinetShape, TArray<FAccessoryShape*> & Slidways)
{
	FSlidingDoor * SlidingDoor = GetFirstSlidingDoorGroupOfCabinet(InCabinetShape);
	if (SlidingDoor == nullptr) return;
	if (SlidingDoor->GetUpSlideWayShape().IsValid())
	{
		Slidways.Emplace(SlidingDoor->GetUpSlideWayShape().Get());
	}
	if (SlidingDoor->GetDownSlideWayShape().IsValid())
	{
		Slidways.Emplace(SlidingDoor->GetDownSlideWayShape().Get());
	}		
}

bool FSCTShapeUtilityTool::ChangeCabinetSlidingDoorSlidways(FSCTShape * InCabinetShape, const int32 InTopSlidwayId)
{
	bool bRetResult = false;
	do
	{
		FSlidingDoor * SlidingDoor = GetFirstSlidingDoorGroupOfCabinet(InCabinetShape);
		if (SlidingDoor == nullptr) break;
		int32 CurUpSlidingWayId = -1;
		if (SlidingDoor->GetUpSlideWayShape().IsValid())
		{
			CurUpSlidingWayId = SlidingDoor->GetUpSlideWayShape()->GetShapeId();
		}
		const TArray<TSharedPtr<FAccessoryShape>> & AllAccessShapes = SlidingDoor->GetAllAccessShapes();
		bool bFind = false;
		FString FindTopSlidwayName;
		for (const auto & Ref : AllAccessShapes)
		{
			if (Ref->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SD_TOP_RAILS) && Ref->GetShapeId() == InTopSlidwayId)
			{
				bFind = true;
				FindTopSlidwayName = Ref->GetShapeName();
				break;
			}
		}
		bRetResult = bFind;
		if (!bRetResult) break;
		SlidingDoor->ChangeSlidwayByTopSlidwayName(FindTopSlidwayName);
	} while (false);
	return bRetResult;
}


const FSCTShapeUtilityTool::FTagsMap FSCTShapeUtilityTool::TagsMap = {
	{ TEXT("263E571EF5BA494497B53EFA74D96670"),FSCTShapeUtilityTool::FTagsMap::ETags::SmokeCabinetsSpceTag },
	{ TEXT("4490438C218948B6A63238AF78E1AC4C"),FSCTShapeUtilityTool::FTagsMap::ETags::SameMaterialWithDoor },
	{ TEXT("646E31DB377447FF8EC5774F66838162"),FSCTShapeUtilityTool::FTagsMap::ETags::LeftVenBoard },
	{ TEXT("F32C9FFFBD6B402893A1FE8AC84B18FD"),FSCTShapeUtilityTool::FTagsMap::ETags::RightVenBoard }
};

FSCTShape * FSCTShapeUtilityTool::GetSpaceShapeWithTag(FSCTShape * InCabinetShape, const FTagsMap::ETags InTag)
{
	struct FParseSpaceShape : public FNoncopyable
	{
	public:
		FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape,
			FSpaceShape *  & OutTagSpaceShape, const FGuid & InGuid)
			: SpaceShape(InSpaceShape)
			, TagSpaceShape(OutTagSpaceShape)
			, SpaceTagGuid(InGuid)
		{}
		void operator()()
		{
			if (SpaceShape->GetShapeTagPools().Contains(SpaceTagGuid))
			{
				TagSpaceShape = SpaceShape.Get();
				return;
			}
			else if (SpaceShape->GetInsideSpace().IsValid())
			{
				TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
				FParseSpaceShape Exc(InSideShape, TagSpaceShape, SpaceTagGuid);
				Exc();
			}
			for (auto Ref : SpaceShape->GetChildSpaceShapes())
			{
				FParseSpaceShape Exc(Ref, TagSpaceShape, SpaceTagGuid);
				Exc();
			}
		}
	private:
		TSharedPtr<FSpaceShape> & SpaceShape;
		FSpaceShape * & TagSpaceShape;
		const FGuid & SpaceTagGuid;
	};

	const FString SmokeCabinetSpaceStr = TagsMap[InTag];
	FGuid SmokeCabinetSpaceGuid;
	check(FGuid::Parse(SmokeCabinetSpaceStr, SmokeCabinetSpaceGuid));
	FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
	TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
	check(TopSpace.IsValid());
	FSpaceShape * ResultShape = nullptr;
	FParseSpaceShape Exc(TopSpace, ResultShape, SmokeCabinetSpaceGuid);
	Exc();
	return ResultShape;
}

bool FSCTShapeUtilityTool::GetSmokeCabinetSpaceRange(FSCTShape * InCabinetShape, TPair<TTuple<float, float, float>, TArray<float>> & OutWidthRange,
	TPair<TTuple<float, float, float>, TArray<float>> & OutDepthRAnge, TPair<TTuple<float, float, float>, TArray<float>> & OutHeightRange)
{		
	FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
	FSCTShape * ResultShape = nullptr;
	ResultShape = GetSpaceShapeWithTag(InCabinetShape, FTagsMap::ETags::SmokeCabinetsSpceTag);
	if (!ResultShape) return false;
	check(ResultShape->GetShapeType() == ST_Space);
	FSpaceShape::FRangeAndStandValue TempRange;	
	auto CheckRangeIsValid = [&TempRange]()->bool
	{		
		const bool bTempResult = FMath::IsNearlyZero(TempRange.MinValue) ||
			FMath::IsNearlyZero(TempRange.MaxValue) ||
			(TempRange.MaxValue < TempRange.MinValue);
		return !bTempResult;
	};
	const float CurWidth = ResultShape->GetShapeWidth();
	const float CurDepth = ResultShape->GetShapeDepth();
	const float CurHeight = ResultShape->GetShapeHeight();

	FSpaceShape * ResultSpaceShape = StaticCast<FSpaceShape*>(ResultShape);
	FVector InsideSpacePosition, InsideSpaceDimention;
	const bool bIsFrameSpace = ResultSpaceShape->IsFrameShapeSpace();
	if (bIsFrameSpace)
	{
		ResultSpaceShape->CalUnitShapePosDim(InsideSpacePosition, InsideSpaceDimention);
	}

	switch (ResultSpaceShape->GetDirectType())
	{
		case SDT_XDirect:
		{
			TempRange = ResultSpaceShape->GetWidthExportValue();
			if (CheckRangeIsValid())
			{
				OutWidthRange = TPair<TTuple<float, float,float>,TArray<float>>
					(TTuple<float,float,float>(TempRange.MinValue, TempRange.MaxValue, InsideSpaceDimention.X), TempRange.StandValueList);
			}													 
			else
			{
				OutWidthRange = TPair<TTuple<float, float, float>, TArray<float>>
					(TTuple<float, float, float>(bIsFrameSpace ? InsideSpaceDimention.X : CurWidth, bIsFrameSpace ? InsideSpaceDimention.X : CurWidth, InsideSpaceDimention.X),TArray<float>());
			}			
			OutDepthRAnge = TPair<TTuple<float, float, float>, TArray<float>>
				(TTuple<float, float, float>(bIsFrameSpace ? InsideSpaceDimention.Y : CurDepth, bIsFrameSpace ? InsideSpaceDimention.Y : CurDepth,InsideSpaceDimention.Y), TArray<float>());
			OutHeightRange = TPair<TTuple<float, float, float>, TArray<float>>
				(TTuple<float, float, float>(bIsFrameSpace ? InsideSpaceDimention.Z : CurHeight, bIsFrameSpace ? InsideSpaceDimention.Z : CurHeight, InsideSpaceDimention.Z),TArray<float>());
			break;
		}
		case SDT_YDirect:
		{						
			TempRange = ResultSpaceShape->GetDepthExportValue();
			if (CheckRangeIsValid())
			{
				OutDepthRAnge = TPair<TTuple<float, float, float>, TArray<float>>
					(TTuple<float, float, float>(TempRange.MinValue, TempRange.MaxValue, InsideSpaceDimention.Y), TempRange.StandValueList);
			}
			else
			{
				OutDepthRAnge = TPair<TTuple<float, float, float>, TArray<float>>(TTuple<float, float, float>
					(bIsFrameSpace ? InsideSpaceDimention.Y : CurDepth, bIsFrameSpace ? InsideSpaceDimention.Y : CurDepth, InsideSpaceDimention.Y),TArray<float>());
			}
			OutWidthRange = TPair<TTuple<float, float, float>, TArray<float>>
				(TTuple<float, float, float>(bIsFrameSpace ? InsideSpaceDimention.X : CurWidth, bIsFrameSpace ? InsideSpaceDimention.X : CurWidth, InsideSpaceDimention.X), TArray<float>());
			OutHeightRange = TPair<TTuple<float, float, float>, TArray<float>>
				(TTuple<float, float, float>(bIsFrameSpace ? InsideSpaceDimention.Z : CurHeight, bIsFrameSpace ? InsideSpaceDimention.Z : CurHeight, InsideSpaceDimention.Z), TArray<float>());
			break;
		}
		case SDT_ZDirect:
		{						
			TempRange = ResultSpaceShape->GetHeightExportValue();
			if (CheckRangeIsValid())
			{
				OutHeightRange = TPair<TTuple<float, float, float>, TArray<float>>
					(TTuple<float, float, float>(TempRange.MinValue, TempRange.MaxValue, InsideSpaceDimention.Z), TempRange.StandValueList);
			}
			else
			{
				OutHeightRange = TPair<TTuple<float, float, float>, TArray<float>>(TTuple<float, float, float>
					(bIsFrameSpace ? InsideSpaceDimention.Z : CurHeight, bIsFrameSpace ? InsideSpaceDimention.Z : CurHeight, InsideSpaceDimention.Z), TArray<float>());
			}
			OutWidthRange = TPair<TTuple<float, float, float>, TArray<float>>
				(TTuple<float, float, float>(bIsFrameSpace ? InsideSpaceDimention.X : CurWidth, bIsFrameSpace ? InsideSpaceDimention.X : CurWidth, InsideSpaceDimention.X), TArray<float>());
			OutDepthRAnge = TPair<TTuple<float, float, float>, TArray<float>>
				(TTuple<float, float, float>(bIsFrameSpace ? InsideSpaceDimention.Y : CurDepth, bIsFrameSpace ? InsideSpaceDimention.Y : CurDepth, InsideSpaceDimention.Y), TArray<float>());
			break;
		}
		default:
			return false;
	}
	return true;
}

bool FSCTShapeUtilityTool::ResizeSmokeCabinetSpaceRangeWithExpectValue(FSCTShape * InCabinetShape, const float InW, const float InD, const float InH)
{	
	FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
	FSCTShape * ResultShape = nullptr;
	ResultShape = GetSpaceShapeWithTag(InCabinetShape, FTagsMap::ETags::SmokeCabinetsSpceTag);
	if (!ResultShape) return false;
	check(ResultShape->GetShapeType() == ST_Space);
	FSpaceShape::FRangeAndStandValue TempRange;
	FSpaceShape * ResultSpaceShape = StaticCast<FSpaceShape*>(ResultShape);
	auto CheckRangeIsValid = [&TempRange]()->bool
	{
		const bool bTempResult = FMath::IsNearlyZero(TempRange.MinValue) ||
			FMath::IsNearlyZero(TempRange.MaxValue) ||
			(TempRange.MaxValue < TempRange.MinValue);
		return !bTempResult;
	};

	const bool bIsFrameSpace = ResultSpaceShape->IsFrameShapeSpace();
	FVector InsideSpacePosition, InsideSpaceDimention;
	if (bIsFrameSpace)
	{
		ResultSpaceShape->CalUnitShapePosDim(InsideSpacePosition, InsideSpaceDimention);
	}
	const float CurWidth = bIsFrameSpace ? InsideSpaceDimention.X : ResultSpaceShape->GetShapeWidth();
	const float CurDepth = bIsFrameSpace ? InsideSpaceDimention.Y : ResultSpaceShape->GetShapeDepth();
	const float CurHeight = bIsFrameSpace ? InsideSpaceDimention.Z : ResultSpaceShape->GetShapeHeight();	

	auto GetFirstExpectValue = [&TempRange](const float InExpectValue)->float
	{
		check(TempRange.MinValue <= InExpectValue && TempRange.MaxValue >= InExpectValue);
		float RetValue = InExpectValue;
		TArray<float>  StandValues = TempRange.StandValueList;
		if (StandValues.Num() >= 2)
		{
			StandValues.Sort([](const float & InLeftValue, const float & InRightValue)->bool {return InRightValue > InLeftValue; });
		}
		bool bIsStandValueOk = false;
		for (const auto & Ref : StandValues)
		{
			if (Ref >= InExpectValue)
			{
				RetValue = Ref;
				break;
			}
		}	
		return RetValue;
	};

	switch (ResultSpaceShape->GetDirectType())
	{
		case SDT_XDirect:
		{
			TempRange = ResultSpaceShape->GetWidthExportValue();
			if (CheckRangeIsValid())
			{
				ResultSpaceShape->SetShapeWidth(GetFirstExpectValue(InW));
				if (ResultSpaceShape->GetParentSpaceShape())
				{
					ResultSpaceShape->GetParentSpaceShape()->RebuildSpaceWidth();
				}
			}			
			//check(CurDepth >= InD);
			//check(CurHeight >= InH);
			break;
		}
		case SDT_YDirect:
		{
			TempRange = ResultSpaceShape->GetDepthExportValue();
			if (CheckRangeIsValid())
			{
				ResultSpaceShape->SetShapeDepth(GetFirstExpectValue(InD));
				if (ResultSpaceShape->GetParentSpaceShape())
				{
					ResultSpaceShape->GetParentSpaceShape()->RebuildSpaceDepth();
				}
			}	
			//check(CurWidth >= InW);
			//check(CurHeight >= InH);
			break;
		}
		case SDT_ZDirect:
		{
			TempRange = ResultSpaceShape->GetHeightExportValue();
			if (CheckRangeIsValid())
			{	
				ResultSpaceShape->SetShapeHeight(GetFirstExpectValue(InH));
				if (ResultSpaceShape->GetParentSpaceShape())
				{
					ResultSpaceShape->GetParentSpaceShape()->RebuildSpaceHeight();
				}
			}	
			//check(CurWidth >= InW);
			//check(CurDepth >= InD);
			break;
		}
		default:
			return false;
	}

	return true;
}

TArray<FSCTShapeUtilityTool::FVeneerdSudeBoardInfo> FSCTShapeUtilityTool::GetCabinetAllAddedVeneerdSudeBoards(FSCTShape * InCabinetShape)
{
	
	check(InCabinetShape && InCabinetShape->GetShapeType() == ST_Cabinet);
	TArray<FSCTShapeUtilityTool::FVeneerdSudeBoardInfo> RetResult;
	do
	{
		FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
		if (CabinetShape->GetVeneerdSudeBoardGroup().IsValid() == false) break;

		auto EmplaceVeneerdSudeBoardInfo = [&RetResult](const FSCTShapeUtilityTool::EPosition InPosition,const bool bInValid,
			const int64 InMaterialId = 0, const FString & InMaterialName = TEXT(""),const FString & InMd5 = TEXT(""),const FString & InUrl = TEXT(""), 
			const FString & InMaterialThumbnailUrl = TEXT(""),
			const FString & InOptimizerParam = TEXT(""),const float InBoardValue = 0.0f)
		{
			FSCTShapeUtilityTool::FVeneerdSudeBoardInfo Temp;
			Temp.Position = InPosition;
			Temp.IsActorValid = bInValid;
			Temp.MaterialId = InMaterialId;
			Temp.MaterialName = InMaterialName;
			Temp.PakMd5 = InMd5;
			Temp.PakUrl = InUrl;
			Temp.MaterialThumbnailUrl = InMaterialThumbnailUrl;
			Temp.MaterialOptimiztionParam = InOptimizerParam;
			Temp.BoardValue = InBoardValue;
			RetResult.Emplace(MoveTemp(Temp));
		};
		{
			TSharedPtr<FVeneeredSudeBoardBase> TempPtr = CabinetShape->GetVeneerdSudeBoardGroup()->GetSpecialPositionBoard(FVeneeredSudeBoardBase::EPosition::E_Left);
			if (TempPtr.IsValid())
			{
				EmplaceVeneerdSudeBoardInfo(EPosition::E_Left, TempPtr->GetSpawnActorVallid(),
					TempPtr->GetMaterial().ID, TempPtr->GetMaterial().Name, TempPtr->GetMaterial().MD5, TempPtr->GetMaterial().Url, TempPtr->GetMaterial().ThumbnailUrl,
					TempPtr->GetMaterial().OptimizeParam, TempPtr->GetShapeWidth());
			}
			else
			{	 // 左侧见光板标签guid
				const FString LeftTag = TagsMap[FTagsMap::ETags::LeftVenBoard];
				FGuid LeftTagGuid;
				check(FGuid::Parse(LeftTag, LeftTagGuid));
				const bool LeftVeenTagIsExist = CabinetShape->GetShapeTagPools().Contains(LeftTagGuid);
				if (LeftVeenTagIsExist)
				{
					EmplaceVeneerdSudeBoardInfo(EPosition::E_Left, false);
				}
			}
			TempPtr = CabinetShape->GetVeneerdSudeBoardGroup()->GetSpecialPositionBoard(FVeneeredSudeBoardBase::EPosition::E_Right);
			if (TempPtr.IsValid())
			{
				EmplaceVeneerdSudeBoardInfo(EPosition::E_Right, TempPtr->GetSpawnActorVallid(),
					TempPtr->GetMaterial().ID, TempPtr->GetMaterial().Name, TempPtr->GetMaterial().MD5, TempPtr->GetMaterial().Url, TempPtr->GetMaterial().ThumbnailUrl,
					TempPtr->GetMaterial().OptimizeParam, TempPtr->GetShapeWidth());
			}
			else
			{
				// 右侧见光板guid
				const FString RigthTag = TagsMap[FTagsMap::ETags::RightVenBoard];
				FGuid RightTagGuid;
				check(FGuid::Parse(RigthTag, RightTagGuid));
				const bool RightVeenTagIsExist = CabinetShape->GetShapeTagPools().Contains(RightTagGuid);
				if (RightVeenTagIsExist)
				{
					EmplaceVeneerdSudeBoardInfo(EPosition::E_Right, false);
				}
			}
		}
						
	} while (false);
	return 	RetResult;	
}

FBoardShape * FSCTShapeUtilityTool::GetFirstFindedFrameBoard(FSCTShape * InCabinetShape, const EFrameBoardType InType)
{
	FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
	FBoardShape * RetValue = nullptr;
	struct FParseSpaceShape : public FNoncopyable
	{
	public:
		FParseSpaceShape(TSharedPtr<FSpaceShape> & InSpaceShape, FBoardShape * & OutRetValue
		, const EFrameBoardType InType)
			: SpaceShape(InSpaceShape)
			, ToFindBoardShape(OutRetValue)
			, FrameType(InType)
		{}
		void operator()()
		{
			if (SpaceShape->IsFrameShapeSpace())
			{
				int32 DesIntType = 0;
				if (FrameType == EFrameBoardType::E_Left)
				{
					DesIntType = 2;
				}
				else if (FrameType == EFrameBoardType::E_Right)
				{
					DesIntType = 3;
				}
				ToFindBoardShape = SpaceShape->GetFrameBoard(DesIntType).Get();
				return;
			}
			else if (SpaceShape->GetInsideSpace().IsValid())
			{
				TSharedPtr<FSpaceShape> InSideShape = SpaceShape->GetInsideSpace();
				FParseSpaceShape Exc(InSideShape, ToFindBoardShape, FrameType);
				Exc();
			}
			for (auto Ref : SpaceShape->GetChildSpaceShapes())
			{
				FParseSpaceShape Exc(Ref, ToFindBoardShape, FrameType);
				Exc();
			}

		}
	private:
		TSharedPtr<FSpaceShape> & SpaceShape;
		FBoardShape * & ToFindBoardShape;
		const EFrameBoardType & FrameType;

	};

	TSharedPtr<FSpaceShape> TopSpace = CabinetShape->GetTopSpaceShape();
	check(TopSpace.IsValid());
	FParseSpaceShape Exc(TopSpace, RetValue,InType);
	Exc();
	return RetValue;
}

bool FSCTShapeUtilityTool::ModifyCabinetVeneerdSudeBoardActorValid(FSCTShape * InCabinetShape, const EPosition InPosition, const bool bInValid)
{
	bool bRetResult = false;
	check(InCabinetShape && InCabinetShape->GetShapeType() == ST_Cabinet);
	FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
	auto GetMaterialInfo = [InCabinetShape, CabinetShape](FVeneeredSudeBoardBase::FVeneeredSudeBoardMaterial & OutMaterial)
	{
		TArray< TPair<FSCTShape*, int32>> AllDoor;
		GetCabinetAllDoorSequenceNum(InCabinetShape, AllDoor);
		bool bFind = false;
		if (AllDoor.Num() > 1)
		{
			// 统计所有的掩门,根据掩门优先，抽面次之的类型柜子，以及同类型，下面优先的规则来实现材质的优先选择
			AllDoor.Sort([&bFind](const TPair<FSCTShape*, int32> & LRef, const TPair<FSCTShape*, int32> & RRef)->bool {
				auto GetShapeValue = [&bFind](const EShapeType InType)->int32
				{
					int32 RetValue = 100;
					switch (InType)
					{
						case ST_SideHungDoor:
						{
							RetValue = 0;
							bFind = true;
						}break;
						case ST_DrawerDoor:
						{
							RetValue = 1;
							bFind = true;
						}break;
						default:
							break;
					}
					return RetValue;
				};
				const int32 LeftTypeValue = GetShapeValue(LRef.Key->GetShapeType());
				const int32 RightTypeValue = GetShapeValue(LRef.Key->GetShapeType());
				if (LeftTypeValue == RightTypeValue)
				{
					return LRef.Value > RRef.Value;
				}
				else
				{
					return LeftTypeValue < RightTypeValue;
				}
			});
		}
		else if (AllDoor.Num() == 1)
		{
			if (AllDoor[0].Key->GetShapeType() == ST_SideHungDoor ||
				AllDoor[0].Key->GetShapeType() == ST_DrawerDoor)
			{
				bFind = true;
			}

		}

		// 如果存在抽面以及掩门，则使用门板材质
		if (bFind)
		{
			auto ConvertMaterialData = [&OutMaterial](const FDoorSheetShapeBase::FDoorPakMetaData InMaterial)
			{
				OutMaterial.ID = InMaterial.ID;
				OutMaterial.MD5 = InMaterial.MD5;
				OutMaterial.Name = InMaterial.Name;
				OutMaterial.OptimizeParam = InMaterial.OptimizeParam;
				OutMaterial.ThumbnailUrl = InMaterial.ThumbnailUrl;
				OutMaterial.Url = InMaterial.Url;
			};
			FSCTShape * Shape = AllDoor[0].Key;
			if (Shape->GetShapeType() == ST_SideHungDoor)
			{
				const FDoorSheetShapeBase::FDoorPakMetaData & DoorMateriRef = StaticCast<FSideHungDoor*>(Shape)->GetDoorSheets()[0]->GetMaterial();
				ConvertMaterialData(DoorMateriRef);
			}
			else if (Shape->GetShapeType() == ST_DrawerDoor)
			{
				const FDoorSheetShapeBase::FDoorPakMetaData & DoorMateriRef = StaticCast<FDrawerDoorShape*>(Shape)->GetDrawDoorSheet()->GetMaterial();
				ConvertMaterialData(DoorMateriRef);
			}
		}
		// 如果没有门板，则查找左侧框体板件
		else
		{
			FBoardShape * BoardShape = GetFirstFindedFrameBoard(InCabinetShape,EFrameBoardType::E_Left);
			// 如果找不到左侧板,则去寻找右侧板
			if (!BoardShape)
			{
				BoardShape = GetFirstFindedFrameBoard(InCabinetShape, EFrameBoardType::E_Right);
			}
			// 如果找到了，则使用找到板件的材质
			if (BoardShape)
			{
				OutMaterial.ID = BoardShape->GetMaterialData().ID;
				OutMaterial.Name = BoardShape->GetMaterialData().Name;
				OutMaterial.OptimizeParam = BoardShape->GetMaterialData().OptimizeParam;
				OutMaterial.ThumbnailUrl = BoardShape->GetMaterialData().ThumbnailUrl;
				OutMaterial.Url = BoardShape->GetMaterialData().Url;
				OutMaterial.MD5 = BoardShape->GetMaterialData().MD5;
			}
			// 如果门板以及左右侧框体板件都找不到,则使用型录端后台设置的板件默认材质，改材质会存储在见光板组里面
			else
			{	

				check(CabinetShape->GetVeneerdSudeBoardGroup().IsValid());
				const FVeneeredSudeBoardBase::FVeneeredSudeBoardMaterial & MaterialRef = CabinetShape->GetVeneerdSudeBoardGroup()->GetDefaultVeneerdSudeBoardInof().Material;
				OutMaterial.ID = MaterialRef.ID;
				OutMaterial.Name = MaterialRef.Name;
				OutMaterial.OptimizeParam = MaterialRef.OptimizeParam;
				OutMaterial.ThumbnailUrl = MaterialRef.ThumbnailUrl;
				OutMaterial.Url = MaterialRef.Url;
				OutMaterial.MD5 = MaterialRef.MD5;
			}
		}

	};

	TArray<FSCTShapeUtilityTool::FVeneerdSudeBoardInfo> RetResult;
	do
	{
		
		bRetResult = CabinetShape->GetVeneerdSudeBoardGroup().IsValid();
		if (!bRetResult) break;
				
		TSharedPtr<FVeneeredSudeBoardBase> VeneeredSudeBoard = CabinetShape->GetVeneerdSudeBoardGroup()->GetSpecialPositionBoard(StaticCast<FVeneeredSudeBoardBase::EPosition>(StaticCast<int>(InPosition)));
		if (VeneeredSudeBoard.IsValid() && bInValid == false)
		{
			CabinetShape->GetVeneerdSudeBoardGroup()->DeleteVeneeredSudeBoard(VeneeredSudeBoard.Get());
		}
		if (VeneeredSudeBoard.IsValid() == false && bInValid)
		{
			TSharedPtr<FVeneeredSudeBoardBase> Shape = MakeShareable(new FNoneModelVeneeredSudeBoard);
			FVeneeredSudeBoardBase::FVeneeredSudeBoardMaterial VenMaterial;
			GetMaterialInfo(VenMaterial);
			Shape->SetPosition(StaticCast<FVeneeredSudeBoardBase::EPosition>(StaticCast<int32>(InPosition)));
			Shape->SetMaterial(VenMaterial);	
			// 基材厚度
			Shape->SetSubstateId(CabinetShape->GetVeneerdSudeBoardGroup()->GetDefaultVeneerdSudeBoardInof().SubstrateID);
			Shape->SetSubstrateName(CabinetShape->GetVeneerdSudeBoardGroup()->GetDefaultVeneerdSudeBoardInof().SubstrateName);
			Shape->SetSubstarteType(StaticCast<int32>(ESubstrateType::SsT_Board));
			Shape->SetShapeWidth(CabinetShape->GetVeneerdSudeBoardGroup()->GetDefaultVeneerdSudeBoardInof().substrateHeight);
			// 封边
			FVeneeredSudeBoardBase::FVeneeredSudeBoardMaterial EdgeBandingData;
			EdgeBandingData.ID = CabinetShape->GetVeneerdSudeBoardGroup()->GetDefaultVeneerdSudeBoardInof().EdgeBanding.ID;
			EdgeBandingData.Name = CabinetShape->GetVeneerdSudeBoardGroup()->GetDefaultVeneerdSudeBoardInof().EdgeBanding.Name;
			Shape->SetEdgeMaterial(EdgeBandingData);
			CabinetShape->GetVeneerdSudeBoardGroup()->AddVeneeredSudeBoard(Shape);
			CabinetShape->GetVeneerdSudeBoardGroup()->UpdateGroup();
		}				
		bRetResult = true;

	} while (false);	
	return bRetResult;
}

uint32 FSCTShapeUtilityTool::ReplaceCabinetVeneerdSudeBoardMaterial(FSCTShape * InCabinetShape, const FCommonPakData * const InMaterialPakData, UMaterialInterface * InMaterialInterface)
{
	int RetResult = 0;
	check(InCabinetShape && InCabinetShape->GetShapeType() == ST_Cabinet);
	check(InMaterialPakData && InMaterialInterface);	
	do
	{
		FCabinetShape * CabinetShape = StaticCast<FCabinetShape*>(InCabinetShape);
		if (CabinetShape->GetVeneerdSudeBoardGroup().IsValid() == false) break;				
		auto ConvertMaterial = [&InMaterialPakData](FVeneeredSudeBoardBase::FVeneeredSudeBoardMaterial & OutMaterial)
		{
			OutMaterial.ID = InMaterialPakData->ID;
			OutMaterial.MD5 = InMaterialPakData->MD5;
			OutMaterial.Name = InMaterialPakData->Name;
			OutMaterial.OptimizeParam = InMaterialPakData->OptimizeParam;
			OutMaterial.ThumbnailUrl = InMaterialPakData->ThumbnailUrl;
			OutMaterial.Url = InMaterialPakData->Url;
		};
		TSharedPtr<FVeneeredSudeBoardBase> TempSudeBoard = CabinetShape->GetVeneerdSudeBoardGroup()->GetSpecialPositionBoard(FVeneeredSudeBoardBase::EPosition::E_Left);
		if (TempSudeBoard.IsValid())
		{
			FVeneeredSudeBoardBase::FVeneeredSudeBoardMaterial Material;
			ConvertMaterial(Material);
			TempSudeBoard->SetMaterial(Material);
			TempSudeBoard->UpdateVeneeredSudeBoard();
			++RetResult;
		}		

		TempSudeBoard = CabinetShape->GetVeneerdSudeBoardGroup()->GetSpecialPositionBoard(FVeneeredSudeBoardBase::EPosition::E_Right);
		if (TempSudeBoard.IsValid())
		{
			FVeneeredSudeBoardBase::FVeneeredSudeBoardMaterial Material;
			ConvertMaterial(Material);
			TempSudeBoard->SetMaterial(Material);
			TempSudeBoard->UpdateVeneeredSudeBoard();
			++RetResult;
		}

	} while (false);
	return RetResult;	
}

void FSCTShapeUtilityTool::SetActorTag(AActor * InActor, FName InTag, const bool InRecursion /*= true*/)
{
	if (!InActor) return;
	if (InActor->ActorHasTag(InTag) == false)
	{
		InActor->Tags.Add(InTag);
	}
	if (!InRecursion) return;
	TArray<AActor*> Actors;
	InActor->GetAttachedActors(Actors);
	for (auto & Ref : Actors)
	{
		SetActorTag(Ref, InTag, InRecursion);
	}
}

void FSCTShapeUtilityTool::RemoveActorTag(AActor * InActor, FName InTag, const bool InRecursion)
{
	if (!InActor) return;
	if (InActor->ActorHasTag(InTag))
	{
		InActor->Tags.Remove(InTag);
	}
	if (!InRecursion) return;
	TArray<AActor*> Actors;
	InActor->GetAttachedActors(Actors);
	for (auto & Ref : Actors)
	{
		RemoveActorTag(Ref, InTag, InRecursion);
	}
}
