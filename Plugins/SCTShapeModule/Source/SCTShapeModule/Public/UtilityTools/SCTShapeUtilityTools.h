/**
* Copyright 2019 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SCTShapeUtilityTools.h
* @Description 针对Shape提供的一些辅助工具函数集
*
* @Author 欧石楠
* @Date 2019年1月8日
* @Version 1.0
*/
#pragma once

#include "CoreMinimal.h"

/** 前向声明 */
class FSCTShape;
class FBoardShape;
class FAccessoryShape;
class FInsertionBoard;
class AActor;
class UMaterialInterface;
enum  class EMaterialType;
class FSideHungDoor;
struct FCommonPakData;

/**
 * @brief 针对Shape提供的辅助函数集，
 *		不要去实例化
 */
class SCTSHAPEMODULE_API FSCTShapeUtilityTool
{

public:
	/** 为Actor打Tag */
	static void SetActorTag(AActor * InActor, FName InTag, const bool InRecursion = true);

	/** 移除指定名称的标签 */
	static void RemoveActorTag(AActor * InActor, FName InTag, const bool InRecursion = true);

	/**
	 * @brief 通过选中的门板查找对应的顶层空间，然后替换底层空间对应的门组，并且保留之前的五金位置
	 * @parma[in] InSrcDoorSheetShape 选中的门板
	 * @parma[in] InDesDoorGroup 指定的门组shape
	 * @return 返回新门组中与原始选中门板相对应的门板Shape
	 * @note  内部实现流程是将原始的门组删除掉，然后替换到新的门组，删除之前记录对应的五金位置
	 */
	static TSharedPtr<FSCTShape> ReplaceDoorGroupAndKeepMetalTransform(FSCTShape* InSrcDoorSheetShape, FSCTShape * InDesDoorGroup);
	

	/**
	 * @brief 使用指定的门组替换柜体里面的所有门组
	 * @param[in]  InCabinetShape 进行门组替换的柜子
	 * @param[in]  InDesDoorGroup 要被替换成的门组
	 * @return 返回替换失败的门组个数
	 */
	static uint32 ReplaceCabinetAllDoorGroup(FSCTShape* InCabinetShape,FSCTShape * InDesDoorGroup);


	/**
	 * @brief 替换柜体中指定guid的门板
	 * @param[in] InCabinetShape 进行替换的柜体Shape
	 * @param[in] InGuid 指定门板的guid
	 * @parma[in] InDesDoorGroup 	要被替换成的门组
	 * @param[in] bIsCopyGuid 是否是Shape之间拷贝得到的Guid，默认为true
	 * @return 返回true成功
	 */
	static bool ReplaceCabinetDoorWithGuid(FSCTShape* InCabinetShape, const FGuid & InGuid, FSCTShape * InDesDoorGroup,const bool bIsCopyGuid = true);


	/**
	* @brief  使用指定的门组替换柜体里面的所有门组，并返回选中的指定门板所对应的新门板
	* @parma[in] InSrcDoorSheetShape 选中的门板
	* @parma[in] InDesDoorGroup 指定的门组shape
	* @return 返回新门组中与原始选中门板相对应的门板Shape
	* @note  内部实现流程是将原始的门组删除掉，然后替换到新的门组，删除之前记录对应的五金位置
	*/
	static TSharedPtr<FSCTShape> ReplaceCabinetAllDoorGroupAndKeepMetalTransform(FSCTShape* InSrcDoorSheetShape, FSCTShape* InDesDoorGroup);


	/**
	*  @brief 替换柜子材质
	*  @param[in] InMaterialInterface 材质实例
	*  @param[in] InCabinetShape柜体shape
	*  @note 替换所有板子的材质 - 目前只排除门组相关的板件
	*/
	static void ReplaceCabinetMaterial(UMaterialInterface * InMaterialInterface, FSCTShape* InCabinetShape);


	/**
	 * @brief 通过材质类型以及材质ID获取材质的存储路径
	 * @param[in] MaterialType 材质类型, 在MaterialManager.h中有定义
	 * @param[in] InMaterialID 材质ID 
	 * @return 	返回材质的本地缓冲路径
	 * @note 后期应该把MD5加入到材质路径生成中，这样可以很好地对比本地缓冲文件，与服务器上文件是否一致
	 */
    static FString  GetFullCachePathByMaterialTypeAndID(const EMaterialType MaterialType, const int32 InMaterialID);
	

	/**
	* @brief 更新门组的材质
	* @param[in] InDoorShape 门组shape
	* @param[in] InMaterialPakData 材质pak数据
	* @param[in] UMaterialInterface 材质实例
	* @return 是否替换成功
	* @note 目前只支持掩门门组、抽面门组的材质替换
	*/
	static bool ReplaceDoorGroupMaterial(FSCTShape * InDoorShape,const FCommonPakData * const InMaterialPakData, UMaterialInterface * InMaterialInterface);


	/**
	* @brief 更新柜体中所有电器饰条组中饰条的材质
	* @param[in] InShape 柜体Shape	
	* @param[in] InMaterialPakData 材质pak数据
	* @param[in] InMaterialInterface 材质实例	
	* @return 替换成功的电器饰条组个数
	*/
	static uint32 ReplacInsertionBoardMaterial(FSCTShape * InShape, const FCommonPakData * const InMaterialPakData, UMaterialInterface * InMaterialInterface);


	/**
	* @brief 更新门组的材质
	* @param[in] InDoorShape 柜体Shape
	* @param[in] InDoorGroupId 门组ID
	* @param[in] InMaterialPakData 材质pak数据
	* @param[in] InMaterialInterface 材质实例
	* @param[in] bIsDrawer 是否是抽面，默认是false
	* @return 失败门组的个数
	* @note 目前只支持掩门门组、抽面门组的材质替换
	*/
	static uint32 ReplaceCabinetAllDoorGroupMaterial(FSCTShape * InShape,const uint32 InDoorGroupId , const FCommonPakData * const InMaterialPakData,UMaterialInterface * InMaterialInterface,const bool bIsDrawer = false);


	/**
	* @brief 更新门组的材质
	* @param[in] InDoorShape 柜体Shape
	* @param[in] InGuid 门组guid
	* @param[in] InMaterialPakData 材质pak数据
	* @param[in] InMaterialInterface 材质实例	
	* @param[in] bIsCopyGuid 是否是Shape之间拷贝得到的Guid，默认为true
	* @return 如果成功，返回true
	* @note 目前只支持掩门门组、抽面门组的材质替换
	*/
	static bool ReplaceCabinetDoorGroupMaterialWithGuid(FSCTShape * InCabinetShape, const FGuid & InGuid, 
		const FCommonPakData * const InMaterialPakData, UMaterialInterface * InMaterialInterface, const bool bIsCopyGuid = true);

	/**
	* @brief 更换门板的开门方向
	* @param[in] InCabinetShape 柜体Shape
	* @param[in] InGuid 门板guid
	* @param[in] InOpenDirection 要设置的开门方向	
	* @param[in] bIsCopyGuid 是否是Shape之间拷贝得到的Guid，默认为true
	* @return 如果成功，返回true
	* @note 目前只支持掩门门板开门方向的替换
	*/
	static bool ReplaceCabinetDoorSheetOpenDoorDirectionWithGuid(FSCTShape * InCabinetShape, const FGuid & InGuid,const int32 InOpenDirection,const bool bIsCopyGuid = true);

	/**
	 * @brief  替换门组的五金
	 * @param[in]	InShape 门组Shape
	 * @param[in] 要替换的五金ID
	 * @return 如果成功，返回true
	 * @note 暂时只支持门组替换把手五金
	 */
	static bool ReplaceDoorGroupMetal(FSCTShape * InShape, const int32 InMetalId);


	/**	
	 * @brief 替换柜体中指定guid门组的五金
	 * @param[in] InCabinetShape 柜体shape
	 * @param[in] InGuid 门组guid
	 * @param[in] InMetalId 五金ID
	 * @param[in] bIsCopyGuid 是否是Shape之间拷贝得到的Guid，默认为true
	 * @return 返回是否成功
	 * @note 暂时只支持替换掩门、抽面把手，且箱体拉手的只能替换箱体拉手，封边拉手与明装拉手之间可以互换
	 */
	static bool ReplaceCabinetDoorGroupMetalWithGuid(FSCTShape * InCabinetShape, const FGuid & InGuid, const int32 InMetalId, const bool bIsCopyGuid = true);

	/**
	* @brief 替换柜体中，所有指定门组类型的门板的五金
	* @param[in] InCabinetShape 柜体shape
	* @param[in] InDoorGroupTypeId 门组类型id
	* @param[in] InMetalId 五金ID	
	* @return 返回失败的门组个数
	* @note 暂时只支持替换掩门、抽面把手，且箱体拉手的只能替换箱体拉手，封边拉手与明装拉手之间可以互换
	*/
	static uint32 ReplaceCabinetAllDoorGroupMetal(FSCTShape * InCabinetShape,const int32 InDoorGroupTypeId,const int32 InMetalId);


	/**
	 * @brief 获取柜体所有饰条
	 * @param[in] InCabinetShape 柜体shape
	 * @param[out] OutInsertionBoardArray 柜体中的所有饰条
	 */
	static void GetCabinetAllInsertionBoard(FSCTShape * InCabinetShape, TArray<FInsertionBoard*> & OutInsertionBoardArray);


	/**
	 *  @brief 获取柜体所有门板，包括抽面的编码顺序，采用上下左右的顺序依次排列
	 *  @param[in] InCabinetShape 柜体Shape
	 *  @param[out] OutDoorSequenceNum 所有门板按照上下左右的顺序排列规则进行排序之后的组合
	 */
	static void GetCabinetAllDoorSequenceNum(FSCTShape * InCabinetShape,TArray<TPair<FSCTShape *, int32>> & OutDoorSequenceNum);


	/**
	 * @brief 获取柜体所有门板，包括抽面的编码顺序，采用上下左右的顺序依次排列
	 * @param[in]  InCabinetShape 柜体Shape
	 * @return 将所有门板编码整合成一个JsonObject 字符串返回
	 * @note 
	 *   返回一个[{},{},{}]形式的字符串
	 *   建议以如下方式合并到其他Json里面：
	 *   const FString DoorSequenceNum = FSCTShapeUtilityTool::GetCabinetAllDoorSequenceNumAsJsonObjectStr(RootShape.Get());
	 *   if (DoorSequenceNum.IsEmpty() == false)
	 *   {
	 *		SaveJsonWriter->WriteIdentifierPrefix(TEXT("doorItems"));
	 *		SaveJsonWriter->WriteRawJSONValue(DoorSequenceNum);
	 *	 }
	 */
	static FString GetCabinetAllDoorSequenceNumAsJsonObjectStr(FSCTShape * InCabinetShape);


	/**
	 * @brief 替换电器组里面的嵌入式电器
	 * @param[in] InEmbedElectrical 电器组
	 * @param[in] InDesEmbedElectrical 用来替换的电器
	 * @return 如果替换成功，则返回true
	 */
	static bool ReplaceEmbedElectricalGroup(FSCTShape * InEmbedElectrical, FSCTShape * InDesEmbedElectrical);



	/**
	* @brief 替换柜体中指定guid电器组的中的电器
	* @param[in] InCabinetShape 柜体shape
	* @param[in] InGuid 嵌入式电器组guid
	* @param[in] InDesEmbedElectrical 用于替换的嵌入式电器
	* @param[in] bIsCopyGuid 是否是Shape之间拷贝得到的Guid，默认为true
	* @return 返回是否成功	
	*/
	static bool ReplaceEmbedElectricalGroupWithGuid(FSCTShape * InCabinetShape, const FGuid & InGuid, FSCTShape * InDesEmbedElectrical, const bool bIsCopyGuid = true);


	/**
	* @brief 获取柜体所有饰条
	* @param[in] InCabinetShape 柜体shape
	* @param[out] OutAccessoryArray 柜体中所有的嵌入式电器
	*/
	static void GetCabinetAllEmbedElectrical(FSCTShape * InCabinetShape, TArray<FAccessoryShape*> & OutAccessoryArray);


	/**
	 * @brief 根据遮挡尺寸调整前封板尺寸
	 * @param[in]	InCabinetShape	 柜体shape
	 * @param[in]	InStartPoint 起始距离点
	 * @param[in]	InEndPoint	 终止距离点
	 * @return 如果成功，返回true
	 * @note 起始距离点与终止距离点之间的距离为前封板的尺寸，柜体从左向右依次为 0 ---柜体W，根据距离点来判断前封板处在左侧，还是右侧
	 */
	static bool ModifyCabinetFrontBoardSize(FSCTShape * InCabinetShape,const float InStartPoint,const float InEndPoint);

	
	/**
	 * @brief 获取插角封板柜的修改信息(单开门，对开门，外漏门板宽度)
	 * @param[in] InCabinetShape 柜体Shape
	 * @param[out] OutSingleDoor 是否支持单开门
	 * @param[out] OutDoubleDoor 是否支持对开门
	 * @param[out] OutDoorSheetsWidth 外漏门板尺寸
	 * @param[out] OutCurIsSingleDoor 返回当前是否是打开门，true ： 单开门 false: 对开门
	 * @param[out] OutCurIsLeft 返回当前的插角柜是否是左插角，如果是返回true，否则返回false
	 * @return 返回是否是插角封板柜子，如果不是插角封板柜子，返回false，并且其他可修改信息则不可用
	 */
	static bool GetFrontBoardCabinetDoorModifyInfo(FSCTShape * InCabinetShape, bool & OutSingleDoor, bool & OutDoubleDoor, float & OutDoorSheetsWidth,bool & OutCurIsSingleDoor,bool & OutCurIsLeft);

	/**
	* @brief 设置柜体插角封板门组中门板的数量
	* @param[in] InCabinetShape 柜体Shape
	* @param[in] InDoorSheetNum 门板数量		
	* @return 如果设置成功，则返回true，否则为false
	* @note   InDoorSheetNum 门板数量只能是1 或者2，其他值会触发断言
	*/
	static bool ModifyForntBoardCabinetDoorSheetNum(FSCTShape * InCabinetShape, const int32 InDoorSheetNum);


	/**
	 * @beief 设置柜体插角封板门组中外漏门板宽度
	 * @param[in] InCabinetShape 柜体Shape
	 * @param[in] InDoorWidth 柜体尺寸
	 * @return 如果修改成功，则返回true，否则返回false，调整外漏门板尺寸的过程中，插角封板的尺寸也会随着发生变化
	 */
	static bool ModifyFrontBoardCabinetDoorWidth(FSCTShape * InCabinetShape, const float InDoorWidth);



	struct FVariableAreaMeta
	{
		enum class EScaleType : uint8
		{
			E_None		= 0,					 /*!< 横向纵向都不能进行缩放 */
			E_X_Scale	= 1 << 0,				 /*!< 仅横向可以缩放 */
			E_Z_Scale	= 1 << 1,				 /*!< 仅纵向可以缩放 */
			E_XZ_Scale	= E_X_Scale | E_Z_Scale, /*!< 横向纵向都可以缩放*/			
		};		
		
		FBox2D DividBlock;    /*!<  存储切块的原点坐标以及尺寸信息 */
		EScaleType ScaleType; /*!< 存储切块的的缩放类型 */
	};
	
	/**
	 * @brief 获取指定板门的砌块信息，暂时仅支持可变区域门板（掩门以及抽面）
	 * @param[in] InDoorSheetShape 门板Shape
	 * @param[in] InW 目标尺寸宽
	 * @param[in] InH 目标尺寸高
	 * @param[out] OutDividBlocks 门板切块
	 * @return 如果是暂时支持的门板类型，则返回true，否则返回false 当前情况下如果传入移门门板，则会返回false
	 */
	static bool GetDoorSheetDividBlocks(FSCTShape * InDoorSheetShape,const float InW,const float InH,TArray<FVariableAreaMeta> & OutDividBlocks);


	/**
	 * @brief 获取柜体所有抽屉中的五金
	 * @param[in] InCabinetShape 柜体shape
	 * @param[out] OutSlidingDrawerAccess  抽屉TArray<五金TArray<>>
	 * @note 抽屉里面的非门板上面附属的五金
	 */
	static void GetCabinetSlideringDrawerAccess(FSCTShape * InCabinetShape,TArray<TArray<FAccessoryShape*>> & OutSlidingDrawerAccess);


	/**
	 * @brief 获取柜体所有正在使用的门把手集合
	 * @param[in] InCabinetShape 柜体shape
	 * @param[out] OutDoorHanleAccess 柜体所有拉手集合
	 */
	static void GetCabinetAllDoorHanle(FSCTShape * InCabinetShape, TArray<FAccessoryShape*> & OutDoorHanleAccess);


	/**
	 * @beif 获取移门中第一个被找到的移门 - 通过空间深度遍历第一个被找到的
	 * @param[in]  InCabinetShape 柜体Shape
	 * @return 如果找到返回正确的指针，否则返回nullptr
	 */
	static class FSlidingDoor * GetFirstSlidingDoorGroupOfCabinet(FSCTShape * InCabinetShape);


	/**
	 * @brief 设置柜体中移门下垫板是否显示
	 * @param[in]  InCabinetShape
	 * @param[in]  InVisiable	true代表显示，false代表隐藏
	 * @return 如果在柜体中找到了移门，并且成功设置成功的话返回ture,其他情况返回false
	 * @note 查找柜体中第一个移门，然后去设置下垫板的有效性
	 */
	static bool SetCabinetSlidingDoorDownPlateBoardValid(FSCTShape * InCabinetShape,const bool InVisiable);

	/**
	* @brief 获取柜体中移门下垫板是否显示
	* @param[in]  InCabinetShape	
	* @return 如果在柜体中找到了移门，并且下垫板你有效的话返回ture,其他情况返回false
	* @note 查找柜体中第一个移门，然后获取其下垫板是否显示
	*/
	static bool GetCabinetSlidingDoorDownPlateBoardIsValid(FSCTShape * InCabinetShape);


	/** 对外返回导轨信息的数据结构 */
	struct FSlidwayInfo
	{
		int32 Id = -1;			/*!< 导轨ID*/
		FString DisplayName;	/*!< 导轨显示名称*/
		FString Thumdurl;		/*!< 导轨缩略图url*/
		bool bIsUsed = false;	/*!< 是否当前再用*/
	};
	/**
	 * @brief 获取柜体中移门的所有可替换导轨
	 * @param[in] InCabinetShape
	 * @param[out] 可替换导轨组
	 * @note 查找柜体中的第一个移门组，返回其所有可替换的导轨项(仅仅包含导轨组中的上轨)
	 */
	static void GetCabinetSlidingDoorSlidways(FSCTShape * InCabinetShape,TArray<FSlidwayInfo> & Slidways);

	/**
	 *@brief 获取柜体中所有的移门导轨shape
	 *@param[in] InCabinetShape 柜体Shape
	 *@param[out] Slidways 返回上下滑轨组
	 */
	static void GetCabinetSlidingDoorSlidways(FSCTShape * InCabinetShape, TArray<FAccessoryShape*> & Slidways);

	/**
	 * @brief 改变柜体中移门的导轨
	 * @param[in] InCabinetShape 柜体Shape
	 * @param[in] InTopSlidwayId  导轨组中上轨的ID
	 * @return 查找柜体中的第一个一门组，设置其当前导轨，如果设置成功，返回true,其他情况返回false
	 */
	static bool ChangeCabinetSlidingDoorSlidways(FSCTShape * InCabinetShape,const int32 InTopSlidwayId);
	
	
	/** 标签映射 */
	struct FTagsMap
	{
	public:
		enum class ETags
		{
			SmokeCabinetsSpceTag = 0,	    /*!< 烟机柜空间，可动态调整空间大小*/
			SameMaterialWithDoor = 1 << 0,  /*!< 与门板同材质*/
			LeftVenBoard         = 1 << 1,  /*!< 柜体可与添加左侧见光板*/
			RightVenBoard        = 1 << 2,  /*!< 柜体可以添加右侧见光板*/
		};
		struct FTagMeta
		{
		public:
			FTagMeta(const FString & InGuidStr, const ETags InTag)
				: Guid(InGuidStr)
				, Tag(InTag) {}
		public:
			FString Guid;
			ETags Tag;
		};
		FTagsMap(const std::initializer_list<FTagMeta>&& InVlues)
		{
			for (const auto & Ref : InVlues)
			{
				InteralTagsMap.Add(Ref.Guid, Ref.Tag);
			}
		}
		ETags operator[](const FString & InStr) const
		{
			check(InteralTagsMap.Contains(InStr));   
			return InteralTagsMap[InStr];
		}
		const FString & operator[](const ETags InTag) const
		{
			const FString * FindPtr = InteralTagsMap.FindKey(InTag);
			check(FindPtr);
			return *FindPtr;
		}
	private:
		FTagsMap() = delete;
		FTagsMap(const FTagsMap &) = delete;
		FTagsMap(const FTagsMap &&) = delete;
		FTagsMap & operator=(const FTagsMap &) = delete;
		FTagsMap & operator=(const FTagsMap &&) = delete;
	private:
		TMap<FString,ETags> InteralTagsMap;
	};
	const static FTagsMap TagsMap;


	/**
	 * @brief 查找具有指定标签的空间shape
	 * @param[in] InCabinetShape 柜体Shape
	 * @param[in] InTag 指定标签对应的枚举
	 * @return 如果找到了返回对应的shape，否则返回nullptr
	 */
	static FSCTShape * GetSpaceShapeWithTag(FSCTShape * InCabinetShape,const FTagsMap::ETags InTag);

	
	/**
	 * @brief 获取烟机柜指定标签空间的可变区域范围
	 * @param[in]  InCabinetShape 柜体Shape
	 * @param[out] OutWidthRange 宽度范围以及标准值列表
	 * @param[out] OutDepthRAnge 深度范围以及标准值列表
	 * @param[out] OutHeightRange 高度范围以及标准值列表
	 * @return 如果找到指定标签的烟机空间，则返回true，否则返回false
	 * @note TPair<TTuple<float,float,float>,TArray<float>> 解释为 TPair<TTuple范围值<最小值，最大值，当前值>,标准值列表>
	 */
	static bool GetSmokeCabinetSpaceRange(FSCTShape * InCabinetShape,TPair<TTuple<float,float,float>,TArray<float>> & OutWidthRange, 
		TPair<TTuple<float, float, float>, TArray<float>> & OutDepthRAnge, TPair<TTuple<float, float, float>, TArray<float>> & OutHeightRange);


	/**
	 * @brief 调整烟机柜指定空间的尺寸
	 * @param[in] InCabinetShape 柜体Shape
	 * @param[in] InW 期望宽度
	 * @param[in] InD 期望深度
	 * @param[in] InH 期望高度
	 * @return 如果设置成功，则返回true，否则返回false
	 * @note 空间最终改变成的尺寸并不一定等于期望的尺寸，因为尺寸的设置要参考设定的标准值
	 */
	static bool ResizeSmokeCabinetSpaceRangeWithExpectValue(FSCTShape * InCabinetShape,const float InW,const float InD,const float InH);


	/** 见光板位置 */
	enum class EPosition : uint8
	{
		E_Left  = 0,		/*!< 左侧*/
		E_Right = 1 << 0,	/*!< 右侧*/
		E_Back  = 1 << 1	/*!< 后面*/
	};

	/**  返回给BIM前端的见光板信息 */
	struct FVeneerdSudeBoardInfo
	{	
		EPosition Position = EPosition::E_Left;
		bool IsActorValid = true;		   /**< 板件是否可用 */
		float BoardValue = 0.0f;           /**< 板件厚度 */

		int64 MaterialId = 0;			   /**< 材质ID */
		FString MaterialName;			   /**< 材质名称*/
		FString PakMd5;					   /**< 材质pak MD5 */
		FString PakUrl;					   /**< 材质pak url */
		FString MaterialThumbnailUrl;      /**< 材质pak 缩略图url*/
		FString MaterialOptimiztionParam;  /**< 材质优化参数*/

	};

	/** 
	 * @brief 获取柜体所有在型录端添加的见光板
	 * @param[in]  InCabinetShape 柜体shape
	 * @return 返回所有的见光板，如果为空，则证明在型录端一个也没有添加
	 */
	static TArray<FVeneerdSudeBoardInfo> GetCabinetAllAddedVeneerdSudeBoards(FSCTShape * InCabinetShape);


	/**
	 * @brief 修改柜体指定位置见光板Actor的有效性
	 * @param[in] InCabinetShape 柜体Shape
	 * @param[in] InPosition 见光板的位置
	 * @param[in] bInValid 见光板Actor是否有效	 
	 * @return 如果存在，且设置成功，则返回true，否则返回false
	 */
	static bool ModifyCabinetVeneerdSudeBoardActorValid(FSCTShape * InCabinetShape, const EPosition InPosition,const bool bInValid);
	

	/**
	 * @brief 修改见光板材质
	 * @param[in] InCabinetShape 柜体Shape
	 * @param[in] InMaterialPakData Pak资源数据
	 * @param[in] InMaterialInterface 材质实例
	 * @return 返回替换了材质的见光板个数
	 */
	static uint32 ReplaceCabinetVeneerdSudeBoardMaterial(FSCTShape * InCabinetShape, const FCommonPakData * const InMaterialPakData, UMaterialInterface * InMaterialInterface);


	enum class EFrameBoardType : uint8
	{
		E_Left,  /*!< 左侧框体板件*/
		E_Right	 /*!< 右侧框体板件*/
	};
	/** 
	 * @brief 查找柜体中第一个被找到的指定类型的框体板件
	 * @param[in] InCabinetShape 柜体
	 * @param[in] InType 类型
	 * @return 如果找到了，返回一个有效的板件指针，否则返回nulllptr
	 */
	static class FBoardShape * GetFirstFindedFrameBoard(FSCTShape * InCabinetShape, const EFrameBoardType InType);
private:																																		 
	FSCTShapeUtilityTool() = delete;
};

