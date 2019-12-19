#include "ArmyDataTools.h" 
#include "ArmyWallLine.h"
#include "ArmySceneData.h"
#include "ArmyRoom.h"
#include "ArmyMath.h"
#include "ArmyDesignEditor.h"
#include "ResManager.h"
#include "ArmyResourceModule.h"
#include "EngineUtils.h"
#include "ArmyFurnitureActor.h"
#include "ArmyActorConstant.h"
#include "ArmyCommonTools.h"
#include "ArmyRoomEntity.h"
#include "ArmyRegion.h"
#include "ArmyModelEntity.h"
#define Actor_To_Floor 1

int32 FArmyDataTools::BimLayoutIndex = -1;

TMap<int32, TArray<EAIComponentCode>> FArmyDataTools::GlobalRoomCodeMap;
// 初始化静态变量
void FArmyDataTools::InitStaticData()
{
	/*---------------- 主卧 ----------------*/
	TArray<EAIComponentCode> Bedroom_Master;
	Bedroom_Master.Add(AI_SingleBed);       // 单人床
	Bedroom_Master.Add(AI_DoubleBed);       // 双人床
	Bedroom_Master.Add(AI_BedsideTable_L);  // 左床头柜
	Bedroom_Master.Add(AI_BedsideTable_R);  // 右床头柜
	Bedroom_Master.Add(AI_BedChair);        // 床尾凳
	Bedroom_Master.Add(AI_TVBench);         // 电视柜
	Bedroom_Master.Add(AI_BookChair);       // 书椅
	Bedroom_Master.Add(AI_Desk);            // 书桌
	Bedroom_Master.Add(AI_Dresser);         // 梳妆台
	Bedroom_Master.Add(AI_DressingStool);   // 梳妆凳
	Bedroom_Master.Add(AI_Wardrobe);        // 衣柜
	Bedroom_Master.Add(AI_Wardrobe_C);      // 定制衣柜
	Bedroom_Master.Add(AI_CurtainStd);       // 标准窗帘
	Bedroom_Master.Add(AI_Carpet);          // 地毯
	Bedroom_Master.Add(AI_Titanic);         // 挂画
	Bedroom_Master.Add(AI_LampMain);        // 主灯
	Bedroom_Master.Add(AI_LampCeiling);     // 吸顶灯
	Bedroom_Master.Add(AI_LampDrop);        // 吊灯
	Bedroom_Master.Add(AI_LedLight);		// 集成LED灯
	Bedroom_Master.Add(AI_LampTable);       // 台灯
	Bedroom_Master.Add(AI_LampWall);        // 壁灯
	Bedroom_Master.Add(AI_TVWall);          // 壁挂电视
	Bedroom_Master.Add(AI_TVVert);          // 立式电视
	Bedroom_Master.Add(AI_AirConditionedHang); // 壁挂空调
	GlobalRoomCodeMap.Add(RT_Bedroom_Master, Bedroom_Master);

	/*---------------- 次卧 ----------------*/
	TArray<EAIComponentCode> Bedroom_Second;
	Bedroom_Second.Add(AI_SingleBed);       // 单人床
	Bedroom_Second.Add(AI_DoubleBed);       // 双人床
	Bedroom_Second.Add(AI_BedsideTable_L);  // 左床头柜
	Bedroom_Second.Add(AI_BedsideTable_R);  // 右床头柜
	Bedroom_Second.Add(AI_BedChair);        // 床尾凳
	Bedroom_Second.Add(AI_TVBench);         // 电视柜
	Bedroom_Second.Add(AI_BookChair);       // 书椅
	Bedroom_Second.Add(AI_Desk);            // 书桌
	Bedroom_Second.Add(AI_Dresser);         // 梳妆台
	Bedroom_Second.Add(AI_DressingStool);   // 梳妆凳
	Bedroom_Second.Add(AI_Wardrobe);        // 衣柜
	Bedroom_Second.Add(AI_Wardrobe_C);      // 定制衣柜
	Bedroom_Second.Add(AI_CurtainStd);       // 标准窗帘
	Bedroom_Second.Add(AI_Carpet);          // 地毯
	Bedroom_Second.Add(AI_Titanic);         // 挂画
	Bedroom_Second.Add(AI_LampMain);        // 主灯
	Bedroom_Second.Add(AI_LampCeiling);     // 吸顶灯
	Bedroom_Second.Add(AI_LampDrop);        // 吊灯
	Bedroom_Second.Add(AI_LedLight);	    // 集成LED灯
	Bedroom_Second.Add(AI_LampTable);       // 台灯
	Bedroom_Second.Add(AI_LampWall);        // 壁灯
	Bedroom_Second.Add(AI_TVWall);          // 壁挂电视
	Bedroom_Second.Add(AI_TVVert);          // 立式电视
	Bedroom_Second.Add(AI_AirConditionedHang); // 壁挂空调
	GlobalRoomCodeMap.Add(RT_Bedroom_Second, Bedroom_Second);

	/*---------------- 书房 ----------------*/
	TArray<EAIComponentCode> StudyRoom;  
	StudyRoom.Add(AI_Bookcase_S);           // 标准书柜
	StudyRoom.Add(AI_BookChair);            // 书椅
	StudyRoom.Add(AI_Desk);                 // 书桌
	StudyRoom.Add(AI_CurtainStd);            // 标准窗帘
	StudyRoom.Add(AI_Carpet);               // 地毯
	StudyRoom.Add(AI_Titanic);              // 挂画
	StudyRoom.Add(AI_LampTable);            // 台灯
	StudyRoom.Add(AI_LampMain);             // 主灯
	StudyRoom.Add(AI_LampCeiling);          // 吸顶灯
	StudyRoom.Add(AI_LampDrop);             // 吊灯
	StudyRoom.Add(AI_LedLight);			    // 集成LED灯
	StudyRoom.Add(AI_AirConditionedHang);   // 壁挂空调
	GlobalRoomCodeMap.Add(RT_StudyRoom, StudyRoom);

	/*---------------- 客厅 ----------------*/
	TArray<EAIComponentCode> LivingRoom;   
	LivingRoom.Add(AI_TeaTable);            // 茶几
	LivingRoom.Add(AI_SideTable);           // 边几
	LivingRoom.Add(AI_TVBench);             // 电视柜
	LivingRoom.Add(AI_Sofa_Master);         // 主位沙发
	LivingRoom.Add(AI_SofaAuxiliary_L);     // 辅位沙发左
	LivingRoom.Add(AI_SofaAuxiliary_R);     // 辅位沙发右
	LivingRoom.Add(AI_CurtainStd);           // 标准窗帘
	LivingRoom.Add(AI_Carpet);              // 地毯
	LivingRoom.Add(AI_Titanic);             // 挂画
	LivingRoom.Add(AI_LampFloorDown);       // 落地灯
	LivingRoom.Add(AI_GreenPlants);         // 绿植
	LivingRoom.Add(AI_LampTable);           // 台灯
	LivingRoom.Add(AI_LampMain);            // 主灯
	LivingRoom.Add(AI_LampCeiling);         // 吸顶灯
	LivingRoom.Add(AI_LampDrop);            // 吊灯
	LivingRoom.Add(AI_LedLight);			// 集成LED灯
	LivingRoom.Add(AI_LampWall);            // 壁灯
	LivingRoom.Add(AI_TVWall);              // 壁挂电视
	LivingRoom.Add(AI_TVVert);              // 立式电视
	LivingRoom.Add(AI_AirConditionedHang);  // 壁挂空调
	LivingRoom.Add(AI_AirConditionedVert);  // 立式空调
	GlobalRoomCodeMap.Add(RT_LivingRoom, LivingRoom);

	/*---------------- 餐厅 ----------------*/
	TArray<EAIComponentCode> DiningRoom;   
	DiningRoom.Add(AI_Sideboard);           // 餐边柜
	DiningRoom.Add(AI_DiningTable);         // 餐桌
	DiningRoom.Add(AI_DiningChair);         // 餐椅
	DiningRoom.Add(AI_Dinnerware);         // 餐具
	DiningRoom.Add(AI_CurtainStd);           // 标准窗帘
	DiningRoom.Add(AI_Carpet);              // 地毯
	DiningRoom.Add(AI_Titanic);             // 挂画
	DiningRoom.Add(AI_LampMain);            // 主灯
	DiningRoom.Add(AI_LampCeiling);         // 吸顶灯
	DiningRoom.Add(AI_LampDrop);            // 吊灯
	DiningRoom.Add(AI_LedLight);			// 集成LED灯
	GlobalRoomCodeMap.Add(RT_DiningRoom, DiningRoom);

	/*---------------- 玄关 ----------------*/
	TArray<EAIComponentCode> Hallway;      
	Hallway.Add(AI_Carpet);                 // 地毯
	Hallway.Add(AI_EnterArk_C);             // 入户柜
	Hallway.Add(AI_LampMain);               // 主灯
	Hallway.Add(AI_LampCeiling);            // 吸顶灯
	Hallway.Add(AI_LampDrop);               // 吊灯
	Hallway.Add(AI_LedLight);			    // 集成LED灯
	GlobalRoomCodeMap.Add(RT_Hallway, Hallway);

	/*---------------- 阳台 ----------------*/
	TArray<EAIComponentCode> Balcony;
	Balcony.Add(AI_SideTable);              // 边几
	Balcony.Add(AI_LeisureChair);           // 休闲椅
	Balcony.Add(AI_BalconyCabinet_S);       // 标准阳台柜
	Balcony.Add(AI_GreenPlants);            // 绿植
	Balcony.Add(AI_LampMain);               // 主灯
	Balcony.Add(AI_LampCeiling);            // 吸顶灯
	Balcony.Add(AI_LampDrop);               // 吊灯
	Balcony.Add(AI_LedLight);			    // 集成LED灯
	Balcony.Add(AI_ClotheslinePole);        // 晾衣杆
	GlobalRoomCodeMap.Add(RT_Balcony, Balcony);

	/*---------------- 卫生间 ----------------*/
	TArray<EAIComponentCode> Bathroom;     
	Bathroom.Add(AI_Closestool);            // 马桶
	Bathroom.Add(AI_BathroomArk);           // 浴室柜-有脚
	Bathroom.Add(AI_BathroomArk_Wall);      // 浴室柜-无脚
	Bathroom.Add(AI_Sprinkler);             // 花洒
	Bathroom.Add(AI_ShowerRoom);            // 淋浴房
	Bathroom.Add(AI_WashingMachine);        // 洗衣机
	//Bathroom.Add(AI_Bathtub);             // 浴缸

	Bathroom.Add(AI_Blind);                 // 百叶帘
	Bathroom.Add(AI_LampMain);              // 主灯
	Bathroom.Add(AI_LampCeiling);           // 吸顶灯
	Bathroom.Add(AI_LampDrop);              // 吊灯
	Bathroom.Add(AI_LedLight);				// 集成LED灯
	Bathroom.Add(AI_BathTowelHolder);		// 浴巾架
	Bathroom.Add(AI_FaceclothHolder);		// 毛巾架
	Bathroom.Add(AI_ToiletPaperHolder);		// 厕纸架
	GlobalRoomCodeMap.Add(RT_Bathroom, Bathroom);

	/*---------------- 厨房 ----------------*/
	TArray<EAIComponentCode> Kitchenroom;
	Kitchenroom.Add(AI_LampMain);              // 主灯
	Kitchenroom.Add(AI_LampCeiling);           // 吸顶灯
	Kitchenroom.Add(AI_LampDrop);              // 吊灯
	Kitchenroom.Add(AI_LedLight);			   // 集成LED灯
	GlobalRoomCodeMap.Add(RT_Kitchen, Kitchenroom);

}
 
//获取模型指定方向上一定距离的位置, 目标模型方向与原始模型保持一致
void FArmyDataTools::ModelOnDirectionOfModel(const AActor& SrcModel, float Distance, AActor &DestModel, EDirection InDir, int ModelType /*= 0*/)
{
	FVector gap = GetVecByEDirection(InDir)*Distance;

	FTransform src_transform = SrcModel.GetTransform();
	FTransform dest_transform = DestModel.GetTransform();

	auto Rotator = src_transform.Rotator();
	gap = Rotator.RotateVector(gap);
	gap += src_transform.GetLocation();
	dest_transform.SetLocation(gap);
	dest_transform.SetRotation(src_transform.GetRotation());

	DestModel.SetActorTransform(dest_transform);
}

//获取模型指定方向上一定距离的位置
void FArmyDataTools::PosOnDirectionOfModel(const AActor& SrcModel, float Distance, FVector &OutPos, EDirection InDir, int ModelType /*= 0*/)
{
	FVector gap = GetVecByEDirection(InDir)*Distance;

	FTransform src_transform = SrcModel.GetTransform();
	auto Rotator = src_transform.Rotator();
	gap = Rotator.RotateVector(gap);
	gap += src_transform.GetLocation();
	OutPos = gap;
}

///** 获取模型左边一定距离的位置,目标模型方向与原始模型保持一致 */
//void FArmyDataTools::ModelOnLeftOfModel(const AActor& SrcModel, float Distance, AActor &DestModel, int ModelType)
//{
//	FVector gap = AI_MODEL_LEFT*Distance;
//
//	FTransform src_transform = SrcModel.GetTransform();
//	FTransform dest_transform = DestModel.GetTransform();
//
//	auto Rotator= src_transform.Rotator();
//	gap = Rotator.RotateVector(gap);
//	gap += src_transform.GetLocation();
//	dest_transform.SetLocation(gap);
//	dest_transform.SetRotation(src_transform.GetRotation());
//
//	DestModel.SetActorTransform(dest_transform);
//}
//
///** 获取模型右边一定距离的位置,目标模型方向与原始模型保持一致 */
// void FArmyDataTools::ModelOnRightOfModel(const AActor& SrcModel, float Distance, AActor &DestModel, int ModelType)
//{
//	 FVector gap = AI_MODEL_RIGHT*Distance;
//
//	 FTransform src_transform = SrcModel.GetTransform();
//	 FTransform dest_transform = DestModel.GetTransform();
//
//	 auto Rotator = src_transform.Rotator();
//	 gap = Rotator.RotateVector(gap);
//	 gap += src_transform.GetLocation();
//	 dest_transform.SetLocation(gap);
//	 dest_transform.SetRotation(src_transform.GetRotation());
//
//	 DestModel.SetActorTransform(dest_transform);
//}
//
///** 获取模型上边一定距离的位置,目标模型方向与原始模型保持一致 */
// void FArmyDataTools::ModelOnUpOfModel(const AActor&SrcModel, float Distance, AActor&DestModel, int ModelType)
//{
//	 FVector gap = AI_MODEL_UP*Distance;
//
//	 FTransform src_transform = SrcModel.GetTransform();
//	 FTransform dest_transform = DestModel.GetTransform();
//
//	 auto Rotator = src_transform.Rotator();
//	 gap = Rotator.RotateVector(gap);
//	 gap += src_transform.GetLocation();
//	 dest_transform.SetLocation(gap);
//	 dest_transform.SetRotation(src_transform.GetRotation());
//
//	 DestModel.SetActorTransform(dest_transform);
//}
//
///** 获取模型下边一定距离的位置,目标模型方向与原始模型保持一致 */
// void FArmyDataTools::ModelOnDownOfModel(const AActor&SrcModel, float Distance, AActor&DestModel, int ModelType)
//{
//	 FVector gap = AI_MODEL_DOWN*Distance;
//
//	 FTransform src_transform = SrcModel.GetTransform();
//	 FTransform dest_transform = DestModel.GetTransform();
//
//	 auto Rotator = src_transform.Rotator();
//	 gap = Rotator.RotateVector(gap);
//	 gap += src_transform.GetLocation();
//	 dest_transform.SetLocation(gap);
//	 dest_transform.SetRotation(src_transform.GetRotation());
//
//	 DestModel.SetActorTransform(dest_transform);
//}
//
//
///** 获取模型前边一定距离的位置,目标模型方向与原始模型保持一致 */
// void FArmyDataTools::ModelOnForwardOfModel(const AActor&SrcModel, float Distance, AActor&DestModel, int ModelType)
// {
//	 FVector gap = AI_MODEL_FORWARD*Distance;
//
//	 FTransform src_transform = SrcModel.GetTransform();
//	 FTransform dest_transform = DestModel.GetTransform();
//
//	 auto Rotator = src_transform.Rotator();
//	 gap = Rotator.RotateVector(gap);
//	 gap += src_transform.GetLocation();
//	 dest_transform.SetLocation(gap);
//	 dest_transform.SetRotation(src_transform.GetRotation());
//
//	 DestModel.SetActorTransform(dest_transform);
//
//}
//
///** 获取模型后边一定距离的位置,目标模型方向与原始模型保持一致 */
//void FArmyDataTools::ModelOnBackwardOfModel(const AActor&SrcModel, float Distance, AActor&DestModel, int ModelType)
// {
//	FVector gap = AI_MODEL_BACKWARD*Distance;
//
//	FTransform src_transform = SrcModel.GetTransform();
//	FTransform dest_transform = DestModel.GetTransform();
//
//	auto Rotator = src_transform.Rotator();
//	gap = Rotator.RotateVector(gap);
//	gap += src_transform.GetLocation();
//	dest_transform.SetLocation(gap);
//	dest_transform.SetRotation(src_transform.GetRotation());
//
//	DestModel.SetActorTransform(dest_transform);
// }
//
///** 获取模型左面一定距离的位置 */
//void FArmyDataTools::PosOnLeftOfModel(const AActor& SrcModel, float Distance, FVector &OutPos, int ModelType)
//{
//	FVector gap = AI_MODEL_LEFT*Distance;
//
//	FTransform src_transform = SrcModel.GetTransform();
//	auto Rotator = src_transform.Rotator();
//	gap = Rotator.RotateVector(gap);
//	gap += src_transform.GetLocation();
//	OutPos = gap;
//}
//
///** 获取模型右面一定距离的位置 */
//void FArmyDataTools::PosOnRightOfModel(const AActor& SrcModel, float Distance, FVector &OutPos, int ModelType)
//{
//	FVector gap = AI_MODEL_RIGHT*Distance;
//
//	FTransform src_transform = SrcModel.GetTransform();
//	auto Rotator = src_transform.Rotator();
//	gap = Rotator.RotateVector(gap);
//	gap += src_transform.GetLocation();
//	OutPos = gap;
//}
//
///** 获取模型上面一定距离的位置 */
//void FArmyDataTools::PosOnUpOfModel(const AActor& SrcModel, float Distance, FVector &OutPos, int ModelType)
//{
//	FVector gap = AI_MODEL_UP*Distance;
//
//	FTransform src_transform = SrcModel.GetTransform();
//	auto Rotator = src_transform.Rotator();
//	gap = Rotator.RotateVector(gap);
//	gap += src_transform.GetLocation();
//	OutPos = gap;
//}
//
///** 获取模型下面一定距离的位置 */
// void FArmyDataTools::PosOnDownOfModel(const AActor& SrcModel, float Distance, FVector &OutPos, int ModelType)
// {
//	 FVector gap = AI_MODEL_DOWN*Distance;
//
//	 FTransform src_transform = SrcModel.GetTransform();
//	 auto Rotator = src_transform.Rotator();
//	 gap = Rotator.RotateVector(gap);
//	 gap += src_transform.GetLocation();
//	 OutPos = gap;
// }
//
///** 获取模型前面一定距离的位置 */
// void FArmyDataTools::PosOnForwardOfModel(const AActor& SrcModel, float Distance, FVector &OutPos, int ModelType)
// {
//	 FVector gap = AI_MODEL_FORWARD*Distance;
//
//	 FTransform src_transform = SrcModel.GetTransform();
//	 auto Rotator = src_transform.Rotator();
//	 gap = Rotator.RotateVector(gap);
//	 gap += src_transform.GetLocation();
//	 OutPos = gap;
// }
//
///** 获取模型后面一定距离的位置 */
// void FArmyDataTools::PosOnBackwardOfModel(const AActor& SrcModel, float Distance, FVector &OutPos, int ModelType)
// {
//
//	 FVector gap = AI_MODEL_BACKWARD*Distance;
//
//	 FTransform src_transform = SrcModel.GetTransform();
//	 auto Rotator = src_transform.Rotator();
//	 gap = Rotator.RotateVector(gap);
//	 gap += src_transform.GetLocation();
//	 OutPos = gap;
// }

TSharedPtr<FArmyWallLine> FArmyDataTools::GetWallLineByModelInDir(const AActor&SrcModel, EDirection ModelDir/* = EDirection::AI_DT_MODEL_BACKWARD*/)
 {
	if (ModelDir == AI_DT_MODEL_UP || ModelDir == AI_DT_MODEL_DOWN)
	{
		//上和下不存在命中的墙线
		return nullptr;
	}
	 TArray<TWeakPtr<FArmyObject>>   objectsArray;
	 FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, EObjectType::OT_InternalRoom, objectsArray);

	 //获取包含模型的房间,不存在  LinePtr = null
	 FVector  position = SrcModel.GetActorLocation();
	 TSharedPtr<FArmyRoom> Room;
	 for (auto &object : objectsArray)
	 {
		 TSharedPtr<FArmyRoom> Temp = StaticCastSharedPtr<FArmyRoom>(object.Pin());
		 if (Temp->IsPointInRoom(position))
		 {
			 Room = Temp;
			 break;
		 }
	 }
	 TSharedPtr<FArmyWallLine> WallLine;
	 if (!Room.IsValid())
		 return WallLine;
	
	 //后方向转到正规坐标系
	 position += FArmyDataTools::GetVecByEDirection(ModelDir) * -1 * 0.1;//修复模型原点在墙上的时候，不能得到相交的墙
	 auto  Rotator = SrcModel.GetActorRotation();
	 auto  BehindV = FArmyDataTools::GetVecByEDirection(ModelDir) * MAX_RAY_LIMITATION;
	 BehindV = Rotator.RotateVector(BehindV);
	 BehindV += position;

	 //获取命中墙体
	 TArray< TSharedPtr<FArmyWallLine>> WallLinesArray = Room->GetWallLines();
	 TArray<TPair<int, float>> distMinimum;
	 for (int i = 0; i < WallLinesArray.Num(); ++i)
	 {
		  auto CoreLine = WallLinesArray[i]->GetCoreLine();
		  auto LineStart =  CoreLine->GetStart();
		  auto LineEnd  =  CoreLine->GetEnd();
		  FVector Out;
		  if (FArmyMath::SegmentIntersection2D(BehindV, position, LineStart, LineEnd, Out))
		  {
			  distMinimum.Add(TPair<int, float>(i, (position - Out).Size())); 
		  }
	 }
	 float min = FLT_MAX;
	 int	 index = -1;
	 for (int i = 0; i < distMinimum.Num(); ++i)
	 {
		 if (distMinimum[i].Value < min)
		 {
			 index = distMinimum[i].Key;
			 min = distMinimum[i].Value;
		 }
	 }

	 if(index!=-1)
		 WallLine =  WallLinesArray[index];
	 return WallLine;
 }

/** 获取模型所在房间
*   @ 如果房间,返回nullptr(特殊情况可能没有命中房间)
*/
TSharedPtr<FArmyRoom>	 FArmyDataTools::GetRoomByModel(const AActor&SrcModel)
{
	TArray<TWeakPtr<FArmyObject>>   objectsArray;
	FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, EObjectType::OT_InternalRoom, objectsArray);

	//获取包含模型的房间,不存在  LinePtr = null
	FVector  position = SrcModel.GetActorLocation();
	TSharedPtr<FArmyRoom> Room;
	for (auto &object : objectsArray)
	{
		TSharedPtr<FArmyRoom> Temp = StaticCastSharedPtr<FArmyRoom>(object.Pin());
		if (Temp->IsPointInRoom(position))
		{
			Room = Temp;
			break;
		}
	}
	return Room;
}

/** 获取房间所有的模型
*	@ 软装
*/
void  FArmyDataTools::GetAllModelsByRoom(const TSharedPtr<class FArmyRoom>&Room, TArray<TSharedPtr<class FArmyModelEntity>> &OutArray)
{
	TArray < TWeakPtr<FArmyObject>> ObjectArray;
	TArray<FMoveableMeshSaveData> Save_MoveableMeshList;
	UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();

	// 遍历场景中所有的有效保存Actor
	for (TActorIterator<AActor> ActorItr(GVC->GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* TempActor = *ActorItr;
		if (TempActor->Tags.Contains("HydropowerActor") || TempActor->Tags.Contains("HydropowerPipeActor") || TempActor->Tags.Contains(XRActorTag::OriginalPoint) || TempActor->Tags.Contains("AutoDesignPoint"))
		{
			continue;
		}

		// 查找Actor的ItemID
		TSharedPtr<FContentItemSpace::FContentItem> ResultSynData = ResMgr->GetContentItemFromID((*ActorItr)->GetSynID());
		EActorType ActorType = ResMgr->GetActorType(*ActorItr);

		int32 ItemID = 0;
		if (!ResultSynData.IsValid())
		{
			continue;
		}
		else
		{
			ItemID = ResultSynData->ID;
			
		}

		//@
		/*if (ActorType != EActorType::Moveable )
		{
			continue;
		}*/

		FVector  Position = TempActor->GetActorLocation();
		if (!Room->IsPointInRoom(Position))
			continue;

		TSharedPtr<FArmyModelEntity> ModelEntity = MakeShareable(new FArmyModelEntity(ResultSynData));
		ModelEntity->Actor = TempActor;
		ModelEntity->Location = TempActor->GetActorLocation();
		ModelEntity->Rotation = TempActor->GetActorRotation();
		ModelEntity->Scale3D = TempActor->GetActorScale();
		// 设置施工项
		ModelEntity->ConstructionItemData = FArmySceneData::Get()->GetConstructionItemDataByActorUniqueID(TempActor->GetUniqueID());

		/*TSharedPtr<FArmyActorItem> ActorItem = MakeShared<FArmyActorItem>(ResultSynData, TempActor);
		ActorItem->Location = TempActor->GetActorLocation();
		ActorItem->Rotation = TempActor->GetActorRotation();
		ActorItem->Scale = TempActor->GetActorScale();*/

		OutArray.Add(ModelEntity);
	}
}

bool FArmyDataTools::PointIntervalGroundOnDirProjPoint(const AActor&SrcModel, const float DistToGround, FVector&Out,EDirection InDir/* = AI_DT_MODEL_BACKWARD*/)
{
	if (!GetIntersectionPointBetweenModelWithWall(SrcModel, Out, InDir))
		return false;
	Out.Z = AI_MODEL_UP.Z*DistToGround;
	return true;
}

/** 模型中线到背面墙体.离地一定距离的模型
*  @(如果没有命中墙体,返回错误) √
*/
bool FArmyDataTools::ModelIntervalGroundOnBackProjPoint(const AActor&SrcModel, const float DistToGround, AActor&DestModel)
{
	FVector Out;
	if (!GetIntersectionPointBetweenModelWithWall(SrcModel, Out))
		return false;
	//FVector Extent = GVC->GetActorCollisionBox(&DestModel).GetExtent();
	Out.Z = AI_MODEL_UP.Z*DistToGround /*+ Extent.Z*/;
	DestModel.SetActorLocation(Out);
	return true;
}

/**模型中线到背面墙体左边一定距离.离地一定距离的模型
*@(如果没有命中墙体,返回错误) √
*/
bool FArmyDataTools::PointIntervalGroundOnLeftOfBackProjPoint(const AActor&SrcModel, const float DistToLeft, const float DistToGround, FVector &Out)
{
	FVector LeftNormal;
	TSharedPtr<FArmyWallLine> WallLine;
	if (!GetIntersectionPointWallLeftDirection(SrcModel, Out, WallLine, LeftNormal))
		return false;

	Out += LeftNormal*DistToLeft;
	Out.Z = 0;
	Out.Z += DistToGround;

	return true;

}


/**模型中线到背面墙体左边一定距离.离地一定距离的模型
*@(如果没有命中墙体,返回错误) √
*/
bool FArmyDataTools::ModelIntervalGroundOnLeftOfBackProjPoint(const AActor&SrcModel, const float DistToLeft, const float DistToGround, AActor&DestModel)
{
	FVector Out;
	FVector LeftNormal;
	TSharedPtr<FArmyWallLine> WallLine;
	if (!GetIntersectionPointWallLeftDirection(SrcModel, Out, WallLine, LeftNormal))
		return false;

	Out += LeftNormal*DistToLeft;
	//FVector Extent = GVC->GetActorCollisionBox(&DestModel).GetExtent();
	Out.Z = AI_MODEL_UP.Z*DistToGround /*+ Extent.Z*/;
	DestModel.SetActorLocation(Out);
	return true;
}



/**模型中线到背面墙体左边一定距离.离地一定距离的点
*@(如果没有命中墙体,返回错误)
*/
bool FArmyDataTools::PointIntervalGroundOnRightOfBackProjPoint(const AActor&SrcModel, const float DistToLeft, const float DistToGround,FVector &Out)
{
	 
	FVector LeftNormal;
	TSharedPtr<FArmyWallLine> WallLine;
	if (!GetIntersectionPointWallRightDirection(SrcModel, Out, WallLine, LeftNormal))
		return false;

	Out += LeftNormal*DistToLeft;
	Out.Z = 0;
	Out.Z += DistToGround;
	return true;
}

/**模型中线到背面墙体左边一定距离.离地一定距离的模型
*@(如果没有命中墙体,返回错误)
*/
bool FArmyDataTools::ModelIntervalGroundOnRightOfBackProjPoint(const AActor&SrcModel, const float DistToLeft, const float DistToGround, AActor&DestModel)
{
	FVector Out;
	FVector LeftNormal;
	TSharedPtr<FArmyWallLine> WallLine;
	if (!GetIntersectionPointWallRightDirection(SrcModel, Out, WallLine, LeftNormal))
		return false;

	Out += LeftNormal*DistToLeft;
	//FVector Extent = GVC->GetActorCollisionBox(&DestModel).GetExtent();
	Out.Z = AI_MODEL_UP.Z*DistToGround/* + Extent.Z*/;
	DestModel.SetActorLocation(Out);
	return true;
}

/**
*   离地一定距离,离线段开始点一定距离的点(指向EndPoint)
*/
FVector FArmyDataTools::PointIntervalGroundDistWithStartPoint(const FArmyLine&Line, const float distToGround, float distToStartPoint)
{
	auto Beg = Line.GetStart();
	auto End = Line.GetEnd();

	auto direction = End - Beg;
	direction.Normalize();

	Beg.Z += distToGround;
	Beg += direction*distToStartPoint;
	return Beg;
}

/**
*   离地一定距离,离线段开始点一定距离的模型(指向EndPoint)
*/
void FArmyDataTools::ModelIntervalGroundDistWithEndPoint(const FArmyLine&Line, const float distToGround, float distToStartPoint, AActor &actor)
{
	auto Beg = Line.GetStart();
	auto End = Line.GetEnd();

	auto direction = Beg - End;
	direction.Normalize();

	//FVector Extent = GVC->GetActorCollisionBox(&actor).GetExtent();
	Beg.Z = AI_MODEL_UP.Z /*+ Extent.Z*/;
	Beg += direction*distToStartPoint;

	actor.SetActorLocation(Beg);
}

/**
*    模型背面墙体的左方向，方向依靠墙体
*/
bool FArmyDataTools::ModelProjPointOnWallLeftDirection(const AActor&SrcModel, FVector &diretion)
{
 
	TArray<TWeakPtr<FArmyObject>>   objectsArray;
	FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, EObjectType::OT_InternalRoom, objectsArray);

	//获取包含模型的房间,不存在  LinePtr = null
	FVector  position = SrcModel.GetActorLocation();
	TSharedPtr<FArmyRoom> Room;
	for (auto &object : objectsArray)
	{
		TSharedPtr<FArmyRoom> Temp = StaticCastSharedPtr<FArmyRoom>(object.Pin());
		if (Temp->IsPointInRoom(position))
		{
			Room = Temp;
			break;
		}
	}
	TSharedPtr<FArmyWallLine> WallLine;
	if (!Room.IsValid())
		return false;


	//后方向转到正规坐标系
	auto  Rotator = SrcModel.GetActorRotation();
	auto  BehindV = AI_MODEL_BACKWARD*MAX_RAY_LIMITATION;
	BehindV = Rotator.RotateVector(BehindV);
	BehindV += position;

	bool hit = false;

	//获取命中墙体
	TArray< TSharedPtr<FArmyWallLine>> WallLinesArray = Room->GetWallLines();
	
 
	for (int i = 0; i < WallLinesArray.Num(); ++i)
	{
		auto CoreLine = WallLinesArray[i]->GetCoreLine();
		auto LineStart = CoreLine->GetStart();
		auto LineEnd = CoreLine->GetEnd();
		FVector Point;
		if (FArmyMath::SegmentIntersection2D(BehindV, position, LineStart, LineEnd, Point))
		{
			WallLine = WallLinesArray[i];
			hit = true;
			break;
		}
	}
	if (!hit)
		return false;
	 
	FVector normal = GetWallNormalInRoom(Room, WallLine);
	FVector beg = WallLine->GetCoreLine()->GetStart();
	FVector end = WallLine->GetCoreLine()->GetEnd();
	FVector centre = (beg + end)*0.5f;

 
 
	FVector left = beg - centre;
	FVector Right= end - centre;
	diretion = FVector::CrossProduct(left, normal).Z < 0 ? left.GetSafeNormal() : Right.GetSafeNormal();
	/*if (FVector::CrossProduct(left, normal).Z < 0)
		diretion = left.Normalize();
	return Right.Normalize();*/
	return true;
}

/**
*    模型背面墙体的右方向，方向依靠墙体
*/
bool FArmyDataTools::ModelProjPointOnWallRightDirection(const AActor&SrcModel, FVector &diretion)
{
	TArray<TWeakPtr<FArmyObject>>   objectsArray;
	FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, EObjectType::OT_InternalRoom, objectsArray);

	//获取包含模型的房间,不存在  LinePtr = null
	FVector  position = SrcModel.GetActorLocation();
	TSharedPtr<FArmyRoom> Room;
	for (auto &object : objectsArray)
	{
		TSharedPtr<FArmyRoom> Temp = StaticCastSharedPtr<FArmyRoom>(object.Pin());
		if (Temp->IsPointInRoom(position))
		{
			Room = Temp;
			break;
		}
	}
	TSharedPtr<FArmyWallLine> WallLine;
	if (!Room.IsValid())
		return false;


	//后方向转到正规坐标系
	auto  Rotator = SrcModel.GetActorRotation();
	auto  BehindV = AI_MODEL_BACKWARD*MAX_RAY_LIMITATION;
	BehindV = Rotator.RotateVector(BehindV);
	BehindV += position;

	bool hit = false;

	//获取命中墙体
	TArray< TSharedPtr<FArmyWallLine>> WallLinesArray = Room->GetWallLines();


	for (int i = 0; i < WallLinesArray.Num(); ++i)
	{
		auto CoreLine = WallLinesArray[i]->GetCoreLine();
		auto LineStart = CoreLine->GetStart();
		auto LineEnd = CoreLine->GetEnd();
		FVector Point;
		if (FArmyMath::SegmentIntersection2D(BehindV, position, LineStart, LineEnd, Point))
		{
			WallLine = WallLinesArray[i];
			hit = true;
			break;
		}
	}
	if (!hit)
		return false;

	FVector normal = GetWallNormalInRoom(Room, WallLine);
	FVector beg = WallLine->GetCoreLine()->GetStart();
	FVector end = WallLine->GetCoreLine()->GetEnd();
	FVector centre = (beg + end)*0.5f;



	FVector left = beg - centre;
	FVector Right = end - centre;
	diretion = FVector::CrossProduct(left, normal).Z > 0 ? left.GetSafeNormal() : Right.GetSafeNormal();
	/*if (FVector::CrossProduct(left, normal).Z > 0)
		return left.Normalize();
	return Right.Normalize();*/

	return true;
}


/**获取模型背部与墙体焦点
*@(如果没有命中墙体,返回错误)
*/
bool FArmyDataTools::GetIntersectionPointBetweenModelWithWall(const AActor&SrcModel, FVector &Intersection, EDirection InDir /*= AI_DT_MODEL_BACKWARD*/)
{
	TArray<TWeakPtr<FArmyObject>>   objectsArray;
	FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, EObjectType::OT_InternalRoom, objectsArray);

	//获取包含模型的房间,不存在  LinePtr = null
	FVector  position = SrcModel.GetActorLocation();
	TSharedPtr<FArmyRoom> Room;
	for (auto &object : objectsArray)
	{
		TSharedPtr<FArmyRoom> Temp = StaticCastSharedPtr<FArmyRoom>(object.Pin());
		if (Temp->IsPointInRoom(position))
		{
			Room = Temp;
			break;
		}
	}
	TSharedPtr<FArmyWallLine> WallLine;
	if (!Room.IsValid())
		return false;
	//后方向转到正规坐标系
	auto  Rotator = SrcModel.GetActorRotation();
	auto  BehindV = FArmyDataTools::GetVecByEDirection(InDir) * MAX_RAY_LIMITATION;
	BehindV = Rotator.RotateVector(BehindV);
	BehindV += position;

	bool hit = false;

	//获取命中墙体
	TArray< TSharedPtr<FArmyWallLine>> WallLinesArray = Room->GetWallLines();
	for (int i = 0; i < WallLinesArray.Num(); ++i)
	{
		auto CoreLine = WallLinesArray[i]->GetCoreLine();
		auto LineStart = CoreLine->GetStart();
		auto LineEnd = CoreLine->GetEnd();
		if (FArmyMath::SegmentIntersection2D(BehindV, position, LineStart, LineEnd, Intersection))
		{
			hit = true;
			break;
		}
	}
	if (!hit)
		return false;
	return true;
}


bool FArmyDataTools::GetIntersectionPointWallLeftDirection(const AActor&SrcModel, FVector &Vector, TSharedPtr<FArmyWallLine>&Line, FVector &Direction)
{
	Line = GetWallLineByModelInDir(SrcModel);
	if (!Line.IsValid())
		return false;
	TArray<TWeakPtr<FArmyObject>>   objectsArray;
	FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, EObjectType::OT_InternalRoom, objectsArray);

	//获取包含模型的房间,不存在  LinePtr = null
	FVector  position = SrcModel.GetActorLocation();
	TSharedPtr<FArmyRoom> Room;
	for (auto &object : objectsArray)
	{
		TSharedPtr<FArmyRoom> Temp = StaticCastSharedPtr<FArmyRoom>(object.Pin());
		if (Temp->IsPointInRoom(position))
		{
			Room = Temp;
			break;
		}
	}

	if (!Room.IsValid())
		return false;
	//后方向转到正规坐标系
	auto  Rotator = SrcModel.GetActorRotation();
	auto  BehindV = AI_MODEL_BACKWARD*MAX_RAY_LIMITATION;
	BehindV = Rotator.RotateVector(BehindV);
	BehindV += position;

	bool hit = false;

	//获取命中墙体
	TArray< TSharedPtr<FArmyWallLine>> WallLinesArray = Room->GetWallLines();
	for (int i = 0; i < WallLinesArray.Num(); ++i)
	{
		auto CoreLine = WallLinesArray[i]->GetCoreLine();
		auto LineStart = CoreLine->GetStart();
		auto LineEnd = CoreLine->GetEnd();
		if (FArmyMath::SegmentIntersection2D(BehindV, position, LineStart, LineEnd, Vector))
		{
			Line = WallLinesArray[i];
			hit = true;
			break;
		}
	}
	if (!hit)
		return false;

	//计算墙体左面区域
	auto Normal = GetWallNormalInRoom(Room, Line);
	Direction = GetLeftDirectionByNormal(Normal, Line);
	return true;
}


/** 计算模型背部墙体,焦点,右方向
* @(如果没有命中墙体,返回错误)
*/
bool FArmyDataTools::GetIntersectionPointWallRightDirection(const AActor&SrcModel, FVector &Vector, TSharedPtr<FArmyWallLine>&Line, FVector &Direction)
{

	Line = GetWallLineByModelInDir(SrcModel);
	if (!Line.IsValid())
		return false;
	TArray<TWeakPtr<FArmyObject>>   objectsArray;
	FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, EObjectType::OT_InternalRoom, objectsArray);

	//获取包含模型的房间,不存在  LinePtr = null
	FVector  position = SrcModel.GetActorLocation();
	TSharedPtr<FArmyRoom> Room;
	for (auto &object : objectsArray)
	{
		TSharedPtr<FArmyRoom> Temp = StaticCastSharedPtr<FArmyRoom>(object.Pin());
		if (Temp->IsPointInRoom(position))
		{
			Room = Temp;
			break;
		}
	}
	

	if (!Room.IsValid())
		return false;
	//后方向转到正规坐标系
	auto  Rotator = SrcModel.GetActorRotation();
	auto  BehindV = AI_MODEL_BACKWARD*MAX_RAY_LIMITATION;
	BehindV = Rotator.RotateVector(BehindV);
	BehindV += position;

	bool hit = false;

	//获取命中墙体
	TArray< TSharedPtr<FArmyWallLine>> WallLinesArray = Room->GetWallLines();
	for (int i = 0; i < WallLinesArray.Num(); ++i)
	{
		auto CoreLine = WallLinesArray[i]->GetCoreLine();
		auto LineStart = CoreLine->GetStart();
		auto LineEnd = CoreLine->GetEnd();
		if (FArmyMath::SegmentIntersection2D(BehindV, position, LineStart, LineEnd, Vector))
		{
			Line = WallLinesArray[i];
			hit = true;
			break;
		}
	}
	if (!hit)
		return false;

	//计算墙体左面区域
	auto Normal = GetWallNormalInRoom(Room, Line);
	Direction = GetRightDirectionByNormal(Normal, Line);
	return true;
}

FVector FArmyDataTools::GetVecByEDirection(EDirection InEDir)
{
	switch (InEDir)
	{
	case EDirection::AI_DT_None:
			return FVector::ZeroVector;
	case EDirection::AI_DT_MODEL_LEFT:
		return AI_MODEL_LEFT;
	case EDirection::AI_DT_MODEL_RIGHT:
		return AI_MODEL_RIGHT;
	case EDirection::AI_DT_MODEL_UP:
		return AI_MODEL_UP;
	case EDirection::AI_DT_MODEL_DOWN:
		return AI_MODEL_DOWN;
	case EDirection::AI_DT_MODEL_FORWARD:
		return AI_MODEL_FORWARD;
	case EDirection::AI_DT_MODEL_BACKWARD:
		return AI_MODEL_BACKWARD;
	default:
		break;
	}
	return FVector::ZeroVector;
}

/**
*获取墙体向内的法线
*/
FVector FArmyDataTools::GetWallNormalInRoom(const TSharedPtr<FArmyRoom>&Room, const TSharedPtr<FArmyWallLine>&WallLine)
{
	FVector2D vector2d;
	auto CoreLine = WallLine->GetCoreLine();
	auto Beg = CoreLine->GetStart();
	auto End = CoreLine->GetEnd();

	auto Vector = End - Beg;
	vector2d.X = cosf(PI / 2)*Vector.X - sinf(PI / 2)*Vector.Y;
	vector2d.Y = sinf(PI / 2)*Vector.X + cosf(PI / 2)*Vector.Y;
	vector2d.Normalize();
	Vector = FVector(vector2d, End.Z);

	auto centre = (Beg + End)*0.5f;
	centre += Vector*50.f;
	if (!Room->IsPointInRoom(centre))
		Vector = -Vector;
	return Vector;
}

/***
*获取法线的左方向(实际为右面)
*/
FVector FArmyDataTools::GetLeftDirectionByNormal(const FVector& Normal, const TSharedPtr<FArmyWallLine>&WallLine)
{
	FVector2D vector2d;
	auto CoreLine = WallLine->GetCoreLine();
	auto Beg = CoreLine->GetStart();
	auto End = CoreLine->GetEnd();

	auto Vector = End - Beg;
	auto Centre = (Beg + End)*0.5f;

	auto V1 = Beg - Centre;
	auto V2 = End - Centre;

	Vector = FVector::CrossProduct(V1, Normal);
	//实际为墙体坐标,右面
	if (Vector.Z < 0)
	{
		V1.Normalize();
		return V1;
	}
	V2.Normalize();
	return V2;
}
/**
*获取法线的右方向(实际为左面)
*/
FVector FArmyDataTools::GetRightDirectionByNormal(const FVector& Normal, const TSharedPtr<FArmyWallLine>&WallLine)
{
	FVector2D vector2d;
	auto CoreLine = WallLine->GetCoreLine();
	auto Beg = CoreLine->GetStart();
	auto End = CoreLine->GetEnd();

	auto Vector = End - Beg;
	auto Centre = (Beg + End)*0.5f;

	auto V1 = Beg - Centre;
	auto V2 = End - Centre;

	Vector = FVector::CrossProduct(V1, Normal);
	//实际为墙体坐标,左面
	if (Vector.Z > 0)
	{
		V1.Normalize();
		return V1;
	}
	V2.Normalize();
	return V2;
}

/** 门窗所在的的墙线*/
TSharedPtr<class FArmyWallLine> FArmyDataTools::GetWallLineByHardware(const TSharedPtr<class FArmyHardware> RefHardware, const TSharedPtr<class FArmyRoom> RefRoom)
{
	TArray<TSharedPtr<class FArmyWallLine>> WallLineList = RefRoom->GetWallLines();
	TSharedPtr<FArmyWallLine> WallLine;
	for (auto & Line : WallLineList)
	{
		float distance = FMath::PointDistToSegment(RefHardware->Point->GetPos(), Line->GetCoreLine()->GetStart(), Line->GetCoreLine()->GetEnd());
		if (FMath::Abs(distance - RefHardware->GetWidth()/2.0f) < 2.0f)
		{
			WallLine = Line;
			break;
		}
	}

	return WallLine;
}

/** 开门后与门同侧的墙体（普通门）*/
TSharedPtr<class FArmyWallLine> FArmyDataTools::GetSameSideWallByHardware(const TSharedPtr<class FArmyHardware> RefHardware, const TSharedPtr<class FArmyRoom> RefRoom, bool NormalDoor)
{
	TSharedPtr<FArmyWallLine> TmpWallLine;
	if (NormalDoor)
	{
		if (RefHardware->GetType() != OT_SecurityDoor)
		{
			return TmpWallLine;
		}
	}
	// 获得门所在的墙
	TSharedPtr<FArmyWallLine> WallLine = GetWallLineByHardware(RefHardware, RefRoom);
	if (WallLine.IsValid())
	{
		TmpWallLine = MakeShared<FArmyWallLine>();
		FVector StartPoint = WallLine->GetCoreLine()->GetStart();
		FVector EndPoint = WallLine->GetCoreLine()->GetEnd();
		FVector HardPos = FArmyMath::GetProjectionPoint(RefHardware->Point->GetPos(), StartPoint, EndPoint);
		FVector CrossVector = FVector::CrossProduct(RefHardware->GetDirection(), EndPoint - StartPoint);
		if (CrossVector.Z < 0)
		{
			if (RefHardware->GetIsRightOpen())
			{
				FVector normal = (EndPoint - HardPos).GetSafeNormal()*RefHardware->GetLength()/2.0f;
				FVector NewPoint = normal + HardPos;
				TmpWallLine->GetCoreLine()->SetStart(NewPoint);
				TmpWallLine->GetCoreLine()->SetEnd(EndPoint);
			}
			else
			{
				FVector normal = (StartPoint - HardPos).GetSafeNormal()*RefHardware->GetLength()/2.0f;
				FVector NewPoint = normal + HardPos;
				TmpWallLine->GetCoreLine()->SetStart(NewPoint);
				TmpWallLine->GetCoreLine()->SetEnd(StartPoint);
			}
		}
		else
		{
			if (!RefHardware->GetIsRightOpen())
			{
				FVector normal = (EndPoint - HardPos).GetSafeNormal()*RefHardware->GetLength()/2.0f;
				FVector NewPoint = normal + HardPos;
				TmpWallLine->GetCoreLine()->SetStart(NewPoint);
				TmpWallLine->GetCoreLine()->SetEnd(EndPoint);
			}
			else
			{
				FVector normal = (StartPoint - HardPos).GetSafeNormal()*RefHardware->GetLength()/2.0f;
				FVector NewPoint = normal + HardPos;
				TmpWallLine->GetCoreLine()->SetStart(NewPoint);
				TmpWallLine->GetCoreLine()->SetEnd(StartPoint);
			}
		}
	}

	return TmpWallLine;
}

/** 开门后与门不同侧的墙体（普通门）*/
TSharedPtr<class FArmyWallLine> FArmyDataTools::GetNotSameSideWallByHardware(const TSharedPtr<class FArmyHardware> RefHardware, const TSharedPtr<class FArmyRoom> RefRoom, bool NormalDoor)
{
	TSharedPtr<FArmyWallLine> TmpWallLine;
	if (NormalDoor)
	{
		if (RefHardware->GetType() != OT_SecurityDoor)
		{
			return TmpWallLine;
		}
	}

	// 获得门所在的墙
	TSharedPtr<FArmyWallLine> WallLine = GetWallLineByHardware(RefHardware, RefRoom);
	if (WallLine.IsValid())
	{
		TmpWallLine = MakeShared<FArmyWallLine>();
		FVector StartPoint = WallLine->GetCoreLine()->GetStart();
		FVector EndPoint = WallLine->GetCoreLine()->GetEnd();
		FVector HardPos = FArmyMath::GetProjectionPoint(RefHardware->Point->GetPos(), StartPoint, EndPoint);
		FVector CrossVector = FVector::CrossProduct(RefHardware->GetDirection(), EndPoint - StartPoint);
		if (CrossVector.Z < 0)
		{
			if (!RefHardware->GetIsRightOpen())
			{
				FVector normal = (EndPoint - HardPos).GetSafeNormal()*RefHardware->GetLength() / 2.0f;
				FVector NewPoint = normal + HardPos;
				TmpWallLine->GetCoreLine()->SetStart(NewPoint);
				TmpWallLine->GetCoreLine()->SetEnd(EndPoint);
			}
			else
			{
				FVector normal = (StartPoint - HardPos).GetSafeNormal()*RefHardware->GetLength() / 2.0f;
				FVector NewPoint = normal + HardPos;
				TmpWallLine->GetCoreLine()->SetStart(NewPoint);
				TmpWallLine->GetCoreLine()->SetEnd(StartPoint);
			}
		}
		else
		{
			if (RefHardware->GetIsRightOpen())
			{
				FVector normal = (EndPoint - HardPos).GetSafeNormal()*RefHardware->GetLength() / 2.0f;
				FVector NewPoint = normal + HardPos;
				TmpWallLine->GetCoreLine()->SetStart(NewPoint);
				TmpWallLine->GetCoreLine()->SetEnd(EndPoint);
			}
			else
			{
				FVector normal = (StartPoint - HardPos).GetSafeNormal()*RefHardware->GetLength() / 2.0f;
				FVector NewPoint = normal + HardPos;
				TmpWallLine->GetCoreLine()->SetStart(NewPoint);
				TmpWallLine->GetCoreLine()->SetEnd(StartPoint);
			}
		}
	}

	return TmpWallLine;
}

/** 开门后与门垂直的墙体（普通门）*/
TSharedPtr<class FArmyWallLine> FArmyDataTools::GetVerticalWallByHardware(const TSharedPtr<class FArmyHardware> RefHardware, const TSharedPtr<class FArmyRoom> RefRoom)
{
	TSharedPtr<FArmyWallLine> TmpWallLine;
	// 开门后与门同侧的墙体（普通门）
	TSharedPtr<FArmyWallLine> SameSideWall = GetSameSideWallByHardware(RefHardware, RefRoom);
	if (!SameSideWall.IsValid())
	{
		return TmpWallLine;
	}

	// 找到公用点
	FVector SamePoint = FVector::ZeroVector;
	bool SameFlag = false;
	for (auto Point : RefRoom->GetWorldPoints())
	{
		if ((Point - SameSideWall->GetCoreLine()->GetStart()).IsNearlyZero())
		{
			SamePoint = SameSideWall->GetCoreLine()->GetStart();
			SameFlag = true;
		}

		if ((Point - SameSideWall->GetCoreLine()->GetEnd()).IsNearlyZero())
		{
			SamePoint = SameSideWall->GetCoreLine()->GetStart();
			SameFlag = true;
		}
	}
	if (!SameFlag)
	{
		return TmpWallLine;
	}

	// 找到公点的所有墙体
	TArray<TSharedPtr<FArmyWallLine>> SameWallLines;
	RefRoom->GetSamePointWalls(SamePoint, SameWallLines);
	if (SameWallLines.Num() < 2)
	{
		return TmpWallLine;
	}

	for (auto SameLine : SameWallLines)
	{
		if (!FArmyMath::IsLinesCollineationAndIntersection(SameLine->GetCoreLine()->GetStart(), SameLine->GetCoreLine()->GetEnd(), SameSideWall->GetCoreLine()->GetStart(), SameSideWall->GetCoreLine()->GetEnd(), false))
		{
			TmpWallLine = SameLine;
			break;
		}
	}

	return TmpWallLine;
}

/** 开门后与门背后的墙体（普通门）*/
TSharedPtr<class FArmyWallLine> FArmyDataTools::GetBackWallByHardware(const TSharedPtr<class FArmyHardware> RefHardware, const TSharedPtr<class FArmyRoom> RefRoom)
{
	TSharedPtr<FArmyWallLine> TmpWallLine;
	// 开门后与门不同侧的墙体（普通门）
	TSharedPtr<FArmyWallLine> NotSameSideWall = GetNotSameSideWallByHardware(RefHardware, RefRoom);
	if (!NotSameSideWall.IsValid())
	{
		return TmpWallLine;
	}

	// 找到公用点
	FVector SamePoint = FVector::ZeroVector;
	bool SameFlag = false;
	for (auto Point : RefRoom->GetWorldPoints())
	{
		if ((Point - NotSameSideWall->GetCoreLine()->GetStart()).IsNearlyZero())
		{
			SamePoint = NotSameSideWall->GetCoreLine()->GetStart();
			SameFlag = true;
		}

		if ((Point - NotSameSideWall->GetCoreLine()->GetEnd()).IsNearlyZero())
		{
			SamePoint = NotSameSideWall->GetCoreLine()->GetStart();
			SameFlag = true;
		}
	}
	if (!SameFlag)
	{
		return TmpWallLine;
	}

	// 找到公点的所有墙体
	TArray<TSharedPtr<FArmyWallLine>> SameWallLines;
	RefRoom->GetSamePointWalls(SamePoint, SameWallLines);
	if (SameWallLines.Num() < 2)
	{
		return TmpWallLine;
	}

	for (auto SameLine : SameWallLines)
	{
		if (!FArmyMath::IsLinesCollineationAndIntersection(SameLine->GetCoreLine()->GetStart(), SameLine->GetCoreLine()->GetEnd(), NotSameSideWall->GetCoreLine()->GetStart(), NotSameSideWall->GetCoreLine()->GetEnd(), false))
		{
			TmpWallLine = SameLine;
			break;
		}
	}

	return TmpWallLine;
}

/** 门所在墙左边的墙体（推拉门）*/
TSharedPtr<class FArmyWallLine> FArmyDataTools::GetLeftWallByHardware(const TSharedPtr<class FArmyHardware> RefHardware, const TSharedPtr<class FArmyRoom> RefRoom)
{
	TSharedPtr<FArmyWallLine> TmpWallLine;
	if (RefHardware->GetType() != OT_SlidingDoor)
	{
		return TmpWallLine;
	}

	return GetSameSideWallByHardware(RefHardware, RefRoom, false);
}
/** 门所在墙右边的墙体（推拉门）*/
TSharedPtr<class FArmyWallLine> FArmyDataTools::GetRightWallByHardware(const TSharedPtr<class FArmyHardware> RefHardware, const TSharedPtr<class FArmyRoom> RefRoom)
{
	TSharedPtr<FArmyWallLine> TmpWallLine;
	if (RefHardware->GetType() != OT_SlidingDoor)
	{
		return TmpWallLine;
	}

	return GetNotSameSideWallByHardware(RefHardware, RefRoom, false);
}

/** 获取实际大小 */
  FVector FArmyDataTools::GetContextItemSize(FContentItemPtr Pointer)
{
	  FVector Vector = FVector::ZeroVector;
	  if (Pointer->ProObj.IsValid())
	  {
		  Vector.X = Pointer->ProObj->Length*0.1f;
		  Vector.Y = Pointer->ProObj->Width*0.1f;
		  Vector.Z = Pointer->ProObj->Height*0.1f;
	  }
	  //for (TSharedPtr<FResObj> resObj : Pointer->ResObjArr)
	  //{
		 // //临时修改TODO
		 // if (resObj->ResourceType == EResourceType::ComponentDXF)
		 // {
			//  TSharedPtr<FComponentRes> componet = StaticCastSharedPtr<FComponentRes>(resObj);
			//  Vector.X = componet->ProductLength*0.1f;
			//  Vector.Y= componet->ProductWidth*0.1f;
			//  Vector.Z = componet->ProductHeight*0.1f;
			//  break;
		 // }
	  //}
	  return Vector;
}

  /** 获取模型前向量，归一化*/
 FVector FArmyDataTools::GetModelForwardVector(AActor*actor)
{
	 FVector direction = AI_MODEL_FORWARD;

	 FTransform src_transform = actor->GetTransform();
	 auto Rotator = src_transform.Rotator();
	 direction = Rotator.RotateVector(direction);
	 direction.Normalize();
	 return direction;
}

/** 获取模型左向量，归一化*/
FVector FArmyDataTools::GetModelLeftVector(AActor*actor)
{
	FVector direction = AI_MODEL_LEFT;
	FTransform src_transform = actor->GetTransform();
	auto Rotator = src_transform.Rotator();
	direction = Rotator.RotateVector(direction);
	direction.Normalize();
	return direction;
}

/** 获得模型的构件编码*/
EComponentID FArmyDataTools::GetComponentId(FContentItemPtr ContentItem)
{
	TSharedPtr<FComponentRes> Component = ContentItem->GetComponent();
	EComponentID OutId = EComponentID::EC_None;
	if (Component.IsValid())
	{
		OutId =  EComponentID(Component->ComponentID);
	}

	return OutId;
}

  // 房间名称变换
  ERoomType FArmyDataTools::GetRoomType(int32 SpaceId)
  {

	  ERoomType OutRoomType = ERoomType::RT_NoName;
	  switch (SpaceId)
	  {
	  case (int32)RT_NoName:
		  OutRoomType = RT_NoName;
		  break;
	  case (int32)RT_Bedroom_Master:
		  OutRoomType = RT_Bedroom_Master;
		  break;
	  case (int32)RT_Bedroom_Second:
	  case (int32)RT_ChildrenRoom:
	  case (int32)RT_Bedroom_Quest:
		  OutRoomType = RT_Bedroom_Second;
		  break;
	  case (int32)RT_StudyRoom:
		  OutRoomType = RT_StudyRoom;
		  break;
	  case (int32)RT_LivingDiningRoom:
	  case (int32)RT_LivingRoom:
		  OutRoomType = RT_LivingRoom;
		  break;
	  case (int32)RT_StorageRoom:
	  case (int32)RT_WashingRoom:
	  case (int32)RT_BathroomDryArea:
		  OutRoomType = RT_Other;
		  break;
	  case (int32)RT_Bathroom:
	  case (int32)RT_Bathroom_Mester:
	  case (int32)RT_Bathroom_Quest:
		  OutRoomType = RT_Bathroom;
		  break;
	  case (int32)RT_Balcony:
	  case (int32)RT_Balcony_Bedroom:
	  case (int32)RT_Balcony_After:
	  case (int32)RT_Balcony_Habit:
	  case (int32)RT_Balcony_Kitchen:
		  OutRoomType = RT_Balcony;
		  break;
	  case (int32)RT_Kitchen:
	  case (int32)RT_ClosedKitchen:
	  case (int32)RT_OpenKitchen:
		  OutRoomType = RT_Kitchen;
		  break;
	  case (int32)RT_DiningRoom:
		  OutRoomType = RT_DiningRoom;
		  break;
	  case (int32)RT_Hallway:
		  OutRoomType = RT_Hallway;
		  break;
	  case (int32)RT_Corridor:
	  case (int32)RT_Aisle:
		  OutRoomType = RT_Corridor;
		  break;
	  default:
		  break;
	  }
	
	  return OutRoomType;
  }
  // 获得与门有关的房间
  TArray<TSharedPtr<class FArmyRoom>> FArmyDataTools::GetRoomsByHardware(TSharedPtr<class FArmyHardware> Hardware)
  {
	  TArray<TSharedPtr<FArmyRoom>> ResultArray;
	  TArray<TWeakPtr<FArmyObject>> RoomList;
	  FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, OT_InternalRoom, RoomList);
	  for (auto &It : RoomList)
	  {
		  TSharedPtr<FArmyRoom> TempRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		  // 初始化门窗数据
		  TArray<TSharedPtr<FArmyHardware>> HardwareArray;
		  TempRoom->GetHardwareListRelevance(HardwareArray);
		  for (auto It2 : HardwareArray)
		  {
			  if (It2->GetUniqueID() == Hardware->GetUniqueID())
			  {
				  ResultArray.Add(TempRoom);
				  break;
			  }
		  }
	  }

	  return ResultArray;
  }

  // 获得当前户型的房间类型列表
  TSet<int32> FArmyDataTools::GetRoomTypeArray()
  {
	  TArray<TWeakPtr<FArmyObject>> RoomList;
	  FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, OT_InternalRoom, RoomList);
	  TSet<int32> RoomTypeArray;
	  for (auto It : RoomList)
	  {
		  TSharedPtr<FArmyRoom> TempRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		  TSharedPtr<FArmyRoomEntity> RoomEntity = MakeShareable(new FArmyRoomEntity(TempRoom));
		  if (RoomEntity->GetIsRegion())
		  {
			  for (auto Region : RoomEntity->GetRegionArray())
			  {
				  RoomTypeArray.Add(FArmyDataTools::GetRoomType(Region->GetRegionId()));
			  }
		  }
		  else
		  {
			  RoomTypeArray.Add(RoomEntity->GetAiRoomType());
		  }
	  }

	  return RoomTypeArray;
  }

  // 计算Z轴坐标位置
  float FArmyDataTools::CalculateLocationZ(TSharedPtr<class FArmyModelEntity> Model)
  {
	  if (!Model.IsValid())
	  {
		  return 0;
	  }

	  return Model->Location.Z;
  }

  //计算模型包围盒与墙的最近距离
  float FArmyDataTools::CalModelFromWallDistance(AActor * InActor, TSharedPtr<FArmyWallLine> InWallLine)
  {
	  float MinDis = 99999999.f;
	  if (InActor && InWallLine.IsValid())
	  {
		  FVector ModelLoc = InActor->GetActorLocation();
		  FVector ProPos = FArmyMath::GetProjectionPoint(ModelLoc, InWallLine->GetCoreLine()->GetStart(), InWallLine->GetCoreLine()->GetEnd());
		  FBox ModelBox = GVC->GetActorCollisionBox(InActor);

		  //保证与模型在同一高度，提高计算精度
		  ProPos.Z = ModelLoc.Z;

		  FVector ToModelClosestPos = ModelBox.GetClosestPointTo(ProPos);
		  MinDis = FVector::Dist2D(ProPos, ToModelClosestPos);
	  }
	  return MinDis;
  }

  //获得模型类型  0:硬装  1:软装 2：配饰  3：全局硬装  9其他 -1 无效
  int32 FArmyDataTools::GetLayoutModelType(TSharedPtr<class FArmyModelEntity> ModelEntity,int32 SpaceId)
  {
	  if (GlobalRoomCodeMap.Contains(SpaceId))
	  {
		  TArray<EAIComponentCode> AiCodeList = GlobalRoomCodeMap.FindRef(SpaceId);
		  if (AiCodeList.Contains(ModelEntity->ComponentId))
		  {
			  if (ModelEntity->ComponentId > 1000 && ModelEntity->ComponentId < 3000)
			  {
				  return 1;
			  }
			  else if (ModelEntity->ComponentId > 3000 && ModelEntity->ComponentId < 5000)
			  {
				  return 2;
			  }
			  else if (ModelEntity->ComponentId > 5000 && ModelEntity->ComponentId < 10000)
			  {
				  return 9;
			  }
			  else
			  {
				  return -1;
			  }
		  }
	  }
	  
	  return -1;
  }

  bool FArmyDataTools::CalModelIntersectWithOtherUnWallModel(AActor * InActor, TSharedPtr<FArmyRoomEntity> InRoomTable, const TSet<AActor*> & IgnoreActorArr)
  {
	  TArray < TSharedPtr<FArmyModelEntity>> ModelArr;
	  InRoomTable->GetAllModels(ModelArr);
	  //TMultiMap<EComponentID, TSharedPtr<class FArmyActorItem> > ActorMap = InRoomTable->GetModelArrayByComponentId();
	  FBox InActorBox = GVC->GetActorCollisionBox(InActor);

	  //是否存在其他模型，如衣柜等
	  for (TSharedPtr<FArmyModelEntity>& MapPair : ModelArr)
	  {
		  AActor* ModelActor = MapPair->Actor;
		  if (ModelActor && !IgnoreActorArr.Contains(ModelActor))
		  {
			  FBox ModelActorBox = GVC->GetActorCollisionBox(ModelActor);
			  if (InActorBox.Intersect(ModelActorBox))
			  {
				  return true;
			  }
		  }
	  }

	  return false;
  }
  
  bool FArmyDataTools::CalModelIntersectWithOtherUnWindowsModel(AActor * InActor, TSharedPtr<FArmyRoomEntity> InRoomTable, const TSet<AActor*> & IgnoreActorArr)
  {
	  TArray <TSharedPtr<FArmyModelEntity>> ModelArr;
	  InRoomTable->GetAllModels(ModelArr);
	  //TMultiMap<EComponentID, TSharedPtr<class FArmyActorItem> > ActorMap = InRoomTable->GetModelArrayByComponentId();
	  FBox InActorBox = GVC->GetActorCollisionBox(InActor);

	  //是否存在其他模型，如衣柜等
	  for (TSharedPtr<FArmyModelEntity>& MapPair : ModelArr)
	  {
		  AActor* ModelActor = MapPair->Actor;
		  if (ModelActor && !IgnoreActorArr.Contains(ModelActor))
		  {
			  FBox ModelActorBox = GVC->GetActorCollisionBox(ModelActor);
			  if (InActorBox.Intersect(ModelActorBox))
			  {
				  return true;
			  }
		  }
	  }

	  //计算是否存在门
	  TArray<TSharedPtr<FArmyHardware> > HardwareDoorArr;
	  HardwareDoorArr = InRoomTable->GetDoorsActor();
	  for (TSharedPtr<FArmyHardware> & HardwareIt : HardwareDoorArr)
	  {
		  TArray<FVector> WindowVec1 = HardwareIt->RectImagePanel->GetVertices();
		  for (FVector & VecIT : WindowVec1)
		  {
			  VecIT.Z = HardwareIt->GetHeightToFloor();
		  }
		  TArray<FVector> WindowVec2 = WindowVec1;
		  for (FVector & VecIT : WindowVec2)
		  {
			  VecIT.Z += HardwareIt->GetHeight();
		  }
		  WindowVec1.Append(WindowVec2);
		  FBox ModelActorBox(WindowVec1);
		  ModelActorBox = ModelActorBox.ExpandBy(2);//将窗的包围盒适当增大，以便和模型进行碰撞检测
		  if (InActorBox.Intersect(ModelActorBox) && !IgnoreActorArr.Contains(HardwareIt->GetRelevanceActor()))
		  {
			  return true;
		  }
	  }

	  return false;
  }

  bool FArmyDataTools::CalModelIntersectWithOtherModel(AActor * InActor, TSharedPtr<FArmyRoomEntity> InRoomTable, const TSet<AActor*> & IgnoreActorArr)
  {
	  TArray < TSharedPtr<FArmyModelEntity>> ModelArr;
	  InRoomTable->GetAllModels(ModelArr);
	  //TMultiMap<EComponentID, TSharedPtr<class FArmyActorItem> > ActorMap = InRoomTable->GetModelArrayByComponentId();
	  FBox InActorBox = GVC->GetActorCollisionBox(InActor);

	  //是否存在其他模型，如衣柜等
	  for (TSharedPtr<FArmyModelEntity>& MapPair : ModelArr)
	  {
		  AActor* ModelActor = MapPair->Actor;
		  if (ModelActor && !IgnoreActorArr.Contains(ModelActor))
		  {
			  FBox ModelActorBox = GVC->GetActorCollisionBox(ModelActor);
			  if (InActorBox.Intersect(ModelActorBox))
			  {
				  return true;
			  }
		  }
	  }

	  //计算是否存在窗户、门等
	  TArray<TSharedPtr<FArmyHardware> > HardwareDoorArr;
	  TArray<TSharedPtr<FArmyHardware> > HardwareWinArr;
	  HardwareDoorArr = InRoomTable->GetDoorsActor();
	  HardwareWinArr = InRoomTable->GetWindowsActor();
	  HardwareDoorArr.Append(HardwareWinArr);
	  for (TSharedPtr<FArmyHardware> & HardwareIt : HardwareDoorArr)
	  {
		  TArray<FVector> WindowVec1 = HardwareIt->RectImagePanel->GetVertices();
		  for (FVector & VecIT : WindowVec1)
		  {
			  VecIT.Z = HardwareIt->GetHeightToFloor();
		  }
		  TArray<FVector> WindowVec2 = WindowVec1;
		  for (FVector & VecIT : WindowVec2)
		  {
			  VecIT.Z += HardwareIt->GetHeight();
		  }
		  WindowVec1.Append(WindowVec2);
		  FBox ModelActorBox(WindowVec1);
		  ModelActorBox = ModelActorBox.ExpandBy(2);//将窗的包围盒适当增大，以便和模型进行碰撞检测
		  if (InActorBox.Intersect(ModelActorBox) && !IgnoreActorArr.Contains(HardwareIt->GetRelevanceActor()))
		  {
			  return true;
		  }
	  }

	  return false;
  }


TSharedPtr<class FArmyWallLine> FArmyDataTools::GetWallLineByPoint(const FVector& InPoint, const float& Limitattion, const TSharedPtr<FArmyRoomEntity>& InRoomEntity, EDirection ModelDir)
{
	TSharedPtr<FArmyWallLine> WallLine;
	if (!InRoomEntity.IsValid())
		return WallLine;

	//后方向转到正规坐标系
	FRotator  Rotator = FRotator::ZeroRotator;
	FVector  BehindV = FArmyDataTools::GetVecByEDirection(ModelDir) * Limitattion;
	BehindV = Rotator.RotateVector(BehindV);
	BehindV += InPoint;

	//获取命中墙体
	TArray< TSharedPtr<FArmyWallLine>> WallLinesArray = InRoomEntity->GetRoom()->GetWallLines();
	TArray<TPair<int, float>> distMinimum;
	for (int32 i = 0; i < WallLinesArray.Num(); ++i)
	{
		auto CoreLine = WallLinesArray[i]->GetCoreLine();
		auto LineStart = CoreLine->GetStart();
		auto LineEnd = CoreLine->GetEnd();
		FVector Out;
		if (FArmyMath::SegmentIntersection2D(BehindV, InPoint, LineStart, LineEnd, Out))
		{
			distMinimum.Add(TPair<int, float>(i, (InPoint - Out).Size()));
		}
	}
	float min = FLT_MAX;
	int	 index = -1;
	for (int32 i = 0; i < distMinimum.Num(); ++i)
	{
		if (distMinimum[i].Value < min)
		{
			index = distMinimum[i].Key;
			min = distMinimum[i].Value;
		}
	}

	if (index != -1)
		WallLine = WallLinesArray[index];
	return WallLine;
}

bool FArmyDataTools::CalModelAdjustToRoom(TSharedPtr<FArmyModelEntity>& InModelEntity, TSharedPtr<FArmyRoomEntity>& InRoomEntity)
{
	/** @纪仁泽 */
	/**********************
	*
	*   RoomPoints ModelPoints
	*   1------------2
	*	|            |
	*   |   center   |
	*   |            |
	*   4------------3
	*
	***********************/
	/** 初始化方向 */
	TArray<EDirection> ModelDir;
	ModelDir.Emplace(AI_DT_MODEL_LEFT);
	ModelDir.Emplace(AI_DT_MODEL_FORWARD);
	ModelDir.Emplace(AI_DT_MODEL_RIGHT);
	ModelDir.Emplace(AI_DT_MODEL_BACKWARD);

	/** 初始化房间的墙点 */
	TArray<FVector> wall_points = InRoomEntity->GetRoom()->GetWorldPoints(true);

	/** 初始化误差 */
	const float dist_error = 1.0f;

	/**
	* 检测Model的四个点
	* 只检测一遍，每检测一个重新刷新，若还在房间外，排除
	*/
	for (int32 refresh_num = 0; refresh_num < 4; ++refresh_num)
	{
		FBox ModelBox = GVC->GetActorCollisionBox(InModelEntity->Actor);

		/** Model的四个点 */
		TArray<FVector> temp_model_points;
		FVector temp_size = ModelBox.GetSize();
		temp_model_points.Emplace(ModelBox.Min);
		temp_model_points.Emplace(FVector(ModelBox.Min.X + temp_size.X, ModelBox.Min.Y, ModelBox.Min.Z));
		temp_model_points.Emplace(ModelBox.Max);
		temp_model_points.Emplace(FVector(ModelBox.Min.X, ModelBox.Min.Y + temp_size.Y, ModelBox.Min.Z));

		/** 初始化房间外的点 */
		FVector out_room_point = FVector::ZeroVector;

		/** 是否在房间内 (初始为房间内) */
		bool b_in_room = true;

		/** 检测Model是否在房间内 */
		for (int32 model_point_num = 0; model_point_num < temp_model_points.Num(); ++model_point_num)
		{
			if (!FArmyMath::IsPointInOrOnPolygon2D(temp_model_points[model_point_num], wall_points))
			{
				out_room_point = temp_model_points[model_point_num];
				b_in_room = false;
				break;
			}
		}
		if (b_in_room==true)
		{
			/** 修改位置，返回在房间内的结果 */
			InModelEntity->Location = ModelBox.GetCenter();
			return true;
		}

		/** 在房间外 */
		if (b_in_room == false)
		{
			for (auto& model_dir : ModelDir)
			{
				/** 通过房间外的点在指定方向上获取墙体 */
				TSharedPtr<class FArmyWallLine> temp_wall = GetWallLineByPoint(out_room_point, (InModelEntity->Width < InModelEntity->Length) ? InModelEntity->Width * 0.5f : InModelEntity->Length * 0.5f, InRoomEntity, model_dir);
				if (temp_wall.IsValid())
				{
					float temp_dist = FArmyMath::CalcPointToLineDistance(out_room_point, temp_wall->GetCoreLine()->GetStart(), temp_wall->GetCoreLine()->GetEnd()) + dist_error;

					/** 点的左面的墙 */
					if (model_dir == EDirection::AI_DT_MODEL_LEFT)
					{
						FVector temp_location = ModelBox.GetCenter() - FVector(temp_dist, 0, 0);
						InModelEntity->Actor->SetActorLocation(temp_location);
					}

					/** 点的右面的墙 */
					else if (model_dir == EDirection::AI_DT_MODEL_RIGHT)
					{
						FVector temp_location = ModelBox.GetCenter() + FVector(temp_dist, 0, 0);
						InModelEntity->Actor->SetActorLocation(temp_location);
					}

					/** 点的前面的墙 */
					else if (model_dir == EDirection::AI_DT_MODEL_FORWARD)
					{
						FVector temp_location = ModelBox.GetCenter() + FVector(0, temp_dist, 0);
						InModelEntity->Actor->SetActorLocation(temp_location);
					}

					/** 点的后面的墙 */
					else if (model_dir == EDirection::AI_DT_MODEL_BACKWARD)
					{
						FVector temp_location = ModelBox.GetCenter() - FVector(0, temp_dist, 0);
						InModelEntity->Actor->SetActorLocation(temp_location);
					}
				}
			}
		}
	}

	/** 表示为删除 */
	return false;
}

// 通过后台返回的两个aicode，形成新的aicode
int32 FArmyDataTools::GetNewAiCode(int32 AiCode1, int32 AiCode2)
{
	// 一字型淋浴房
	if (AiCode1 == AI_ShowerRoom && AiCode2 == EAT_OneFont)
	{
		return AI_ShowerRoomFirst;
	}

	// 壁挂电视
	if (AiCode1 == AI_TVVert && AiCode2 == EAT_GuaBi)
	{
		return AI_TVWall;
	}

	// 壁挂空调
	if (AiCode1 == AI_AirConditionedVert && AiCode2 == EAT_BiGua)
	{
		return AI_AirConditionedHang;
	}

	// 燃气热水器
	if (AiCode1 == AI_Electric_Heater && AiCode2 == EAT_RanQi)
	{
		return AI_Gas_Heater;
	}

	// 智能马桶
	if (AiCode1 == AI_Closestool && AiCode2 == EAT_ZNMaTong)
	{
		return AI_IntelClosestool;
	}

	return AiCode1;

}
// 通过商品id返回aicode
EAIComponentCode FArmyDataTools::GetAiCodeByItemId(int32 ItemId)
{
	TSharedPtr<FContentItem> TmpContentItem = FArmyResourceModule::Get().GetResourceManager()->GetContentItemFromID(ItemId);
	if (TmpContentItem.IsValid())
	{
		return TmpContentItem->AiCode;
	}
	else
	{
		return AI_None;
	}
}

// 通过商品id返回新的位置信息
FVector FArmyDataTools::GetNewLocationByAiCode(TSharedPtr<FArmyModelEntity>& InModelEntity)
{
	if (!InModelEntity.IsValid())
	{
		return FVector::ZeroVector;
	}

	FVector NewLoaction = InModelEntity->Location;
	if (InModelEntity->ComponentId > 1000 && InModelEntity->ComponentId < 3000)
	{
		NewLoaction.Z = NewLoaction.Z+ Actor_To_Floor;
	}

	return NewLoaction;
}

void FArmyDataTools::GetMaxQuadInPolygon(const TArray<FVector>& Polygon, FVector& Center, TArray<FVector>& PointArr, FBox& Box)
{
	float Area = 0;
	for (int i = 0; i < Polygon.Num(); i++)
	{
		int prePoint = i - 1;
		prePoint += prePoint < 0 ? Polygon.Num() : 0;
		int nextPoint = (i + 1) % Polygon.Num();
		//求出另外的一个点
		FVector vecCP = Polygon[prePoint] - Polygon[i];
		FVector vecCN = Polygon[nextPoint] - Polygon[i];
		FVector nnPoint = Polygon[i] + (vecCP + vecCN);
		bool inside = FArmyMath::IsPointInOrOnPolygon2D(nnPoint, Polygon);
		inside &= FArmyMath::IsPointInOrOnPolygon2D((nnPoint + Polygon[i]) / 2, Polygon);
		if (!inside)
		{
			continue;
		}
		float width = vecCN.Size();
		float height = FArmyMath::Distance(Polygon[prePoint], Polygon[i], Polygon[nextPoint]);
		auto areaTemp = width * height;
		if (areaTemp > Area)
		{
			Area = areaTemp;
			Center = (nnPoint + Polygon[i]) / 2;
			//
			PointArr.Empty();
			PointArr.Add(Polygon[prePoint]);
			PointArr.Add(Polygon[i]);
			PointArr.Add(Polygon[nextPoint]);
			//
			TArray<FVector> BoxPointArr;
			BoxPointArr.Append(PointArr);
			BoxPointArr.Add(nnPoint);
			Box = FBox(BoxPointArr);
		}
	}
}
/** 柱子风道是否是组成房间的一部分
* @(如果是,返回true)
*/
bool FArmyDataTools::IsObjectOfRoom(const TSharedPtr<FArmyRoom>&InRoom, const TSharedPtr<FArmyObject> InObj)
{
	if (!InRoom.IsValid() || !InObj.IsValid())
	{
		return false;
	}

	if (InObj->GetType() == OT_Pillar || InObj->GetType() == OT_AirFlue || InObj->GetType() == OT_PackPipe)
	{
		TArray<FVector> RoomVertexes;
		TArray<FVector> ObjVertexes;
		InRoom->GetVertexes(RoomVertexes);
		InObj->GetVertexes(ObjVertexes);
		for (auto It : RoomVertexes)
		{
			if (FArmyMath::IsPointInOrOnPolygon2D(It, ObjVertexes,0.1f))
			{
				return true;
			}
		}

	}
	else
	{
		return false;
	}

	return false;
}
/** 柱子风道包立管关联的空间
* @(返回相关联的房间列表)
*/
TArray<TSharedPtr<FArmyRoom>>  FArmyDataTools::GetObjectRelateRooms(const TSharedPtr<FArmyObject>& InObj)
{
	TArray<TSharedPtr<FArmyRoom>> OutRooms;
	if (!InObj.IsValid())
	{
		return OutRooms;
	}

	if (InObj->GetType() == OT_Pillar || InObj->GetType() == OT_AirFlue || InObj->GetType() == OT_PackPipe)
	{
		TArray<TWeakPtr<FArmyObject>> RoomList;
		FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, OT_InternalRoom, RoomList);
		for (auto It : RoomList)
		{
			TSharedPtr<FArmyRoom> TempRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
			if (TempRoom->IsPointInRoom(InObj->GetBounds().GetCenter()))
			{
				OutRooms.Add(TempRoom);
			}
			else
			{
				if (FArmyDataTools::IsObjectOfRoom(TempRoom, InObj))
				{
					OutRooms.Add(TempRoom);
				}
			}
		}
	}


	return OutRooms;
}
