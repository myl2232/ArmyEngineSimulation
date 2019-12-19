/**
 * Copyright 2019 北京伯睿科技有限公司.
 * All Rights Reserved.
 *  
 *
 * @File ArmyCommonTools.h
 * @Description 全局常用的工具函数
 *
 * @Author 欧石楠
 * @Date 2018年4月3日
 * @Version 1.0
 */

#pragma once

#include "CoreMinimal.h"
#include "ArmyCommonTypes.h"
#include "JsonValue.h"

using namespace FContentItemSpace;

class ARMY_USER_API FArmyCommonTools
{
public:
	FArmyCommonTools();

	static void InitStaticData();

	static FString GetComponentCodeByLocalID(EComponentID InID);

	static FString GetCategoryCodeByLocalID(EComponentID InID);

	//@欧石楠
	//根据商品品类获取默认的构件ID，
	static EComponentID GetDefaultComponentLocalIDByCategoryID(const FString& CategoryID);

	static EComponentID GetComponentLocalIDByCode(const FString& InCode);
	/**
	 * 根据文件名，文件类型，获取对应的文件保存路径
	 * @param InResourceType - EResourceType - 文件类型
	 * @param InFileName - const FString & - 文件名
	 * @return FString - 保存路径（包含文件名本身）
	 */
	static FString GetPathFromFileName(EResourceType InResourceType, const FString& InFileName);

	/** 移除扩展名 */
	static bool RemoveFileNameExtension(FString& _FileName);

	/** 获取扩展名 */
	static FString GetFileExtension(FString& _FileName);

	/** 获取资源的保存路径 */
    static FString GetCommonDir();
	static FString GetDrawHomeDir();
	static FString GetConstructionDir();
	static FString GetCookedHomeDir();
	static FString GetModelDir();
	static FString GetGroupPlanDir();
	static FString GetAutoDesignGroupPlanDir();
	static FString GetLevelPlanDir();
	static FString GetFloorTextureDir();
	static FString GetComponentDir();
    static FString GetFacsimilesDir();

	/**
	 * 根据文件路径得出文件名称
	 * @param InFilePath - FString & - ../../../Army/Game/ZKCache/LR/100_XXX.pak
	 * @return FString - 100_XXX.pak
	 */
	static FString GetFileNameFromPath(FString& InFilePath);

	/** 输入 ../../../Army/Game/ZKCache/LR/LR_0.pak  输出  ../../../Army/Game/ZKCache/LR/ */
	static FString GetFolderFromPath(FString& _FilePath);

	/** 输入 100_XXXXX.pak 输出 100 */
	static int32 GetFileIDFromName(FString _FileNameWithMD5);

	/** 检查字段合法性 */
	static bool CheckTeleNumberValid(FString _str);
	static bool CheckNumberValid(FString _str);
	static bool CheckIDCodeValid(FString _str);
	static bool CheckPasswordValid(FString _str);
	static bool CheckEmpty(FString _str);
	static bool CheckAreaValid(FString _str);
	
	/** 为数字添加逗号，输入10000，输出10,000 */
	static FString AddCommaForNumber(FString _Number);
	static FString AddCommaForNumber(float _Number);

	/** @欧石楠 把字符串版本号转为int，比如1.3.0 -> 130 */
	static int32 ConvertVersionStrToInt(FString InVersionStr);

	/**
	 *  根据ID删除所有以此ID为开头的文件。比如某文件夹下有 99_XXX.json  99_WWW.json，此时传入99则会一起删除这两个文件
	 * @param InID - int32 - 文件ID
	 * @param InType - EResourceType - 文件类型
	 * @return void - 
	 */
	static void DeleteFiles(int32 InID, EResourceType InType);


	static void ParseObjectGroupsFromJson(TSharedPtr<FJsonObject> InJsonObject , TArray<TSharedPtr<FObjectGroupContentItem>>& OutArray);

	static void ParseObjectGroupsFromJson(TSharedPtr<FJsonObject> InJsonObject,FString& ModeName,FString& ItemPath, TArray<TSharedPtr<FObjectGroupContentItem>>& OutArray);

	/**
	 * 解析传入的JsonObject，构建项目单元数据
	 * @param InJsonObject - TSharedPtr<FJsonObject> - 
	 * @return TSharedPtr<FProjectContentItem> - 
	 */
	//static TSharedPtr<FProjectContentItem> ParseProjectContentItemFromJson(TSharedPtr<FJsonObject> InJsonObject);

	/**
	 * 解析传入的JsonObject，构建资源管理器类目分类结构数据
	 * @param InJsonObject - TSharedPtr<FJsonObject> - 传入的JObject
	 * @return FCategoryData - 资源管理器的类目层级数据
	 */
	static FCategoryContentItem ParseCategoryFromJson(TSharedPtr<FJsonObject> InJsonObject);


	/**
	 * 解析传入的JsonObject，构建资源管理器的单元数据
	 * @param InJsonObject - TSharedPtr<FJsonObject> - 
	 * @return TSharedPtr<FContentItemSpace::FContentItem> - 
	 */
	static TSharedPtr<FContentItemSpace::FContentItem> ParseContentItemFromJson(TSharedPtr<FJsonObject> InJsonObject);

	/**
	* 解析传入的JsonObject，构建大师方案单元数据
	* @param InJsonObject - TSharedPtr<FJsonObject> -
	* @return TSharedPtr<FContentItemSpace::FContentItem> -
	*/
	static TSharedPtr<FContentItemSpace::FContentItem> ParseMakterPlanContentItemFromJson(TSharedPtr<FJsonObject> InJsonObject);


	static TSharedPtr<FContentItemSpace::FContentItem> ParseHardModeContemItemFromJson(TSharedPtr<FJsonObject> InJsonObject);

	
	/**
	* 从Json解析数据的具体信息（不包括带铺法的缩略图相关信息）
	* @param InJsonObject - TSharedPtr<FJsonValue> - JSon对象
	* @param resourceType - EResourceType - 资源类型
	* @param Categry - Categry1Type & - 最底层类目
	* @return TSharedPtr<FContentItemSpace::FArmyHardModeRes::ModelStruct> - 返回一个资源的结构体
	*/
	//static TSharedPtr<FContentItemSpace::FArmyHardModeRes::ModelStruct> ParseTextureStructItemFromJson(TSharedPtr<FJsonObject> ArrayIt, EResourceType resourceType, int32 &categoryId, PlacePosition &ePP, FString &codeStr, TSharedPtr<FContentItemSpace::FComponentRes>& ComponentPtr);
	/**
	* 解析传入的JsonObject，构建资源管理器类目分类结构数据,主要解析水电模块数据
	* @param InJsonObject - TSharedPtr<FJsonObject> - 传入的JObject
	* @return FCategoryContentItem - 传入的JObject
	*/
	static TSharedPtr<FContentItemSpace::FContentItem> ParseHydropowerContentItemFromJson(TSharedPtr<FJsonObject> InJsonObject);

	//根据构件生成类型
	static bool ParseComponentToObjectType(uint32 _identity, uint32 &OutObjectType);

	/**
	* 解析传入的JsonObject，构建资源管理器类目分类结构数据
	* @param InJsonObject - TSharedPtr<FJsonObject> - 传入的JObject
	* @return FCategoryData - 资源管理器的类目层级数据
	*/


	/**
	 * 解析套餐、品牌、规格等筛选条件键值对
	 * @param InJsonObject - TSharedPtr<FJsonObject> - 传入的Json对象
	 * @return TSharedPtr<struct FArmyKeyValue> - 返回解析到的键值对
	 */
	static TSharedPtr<struct FArmyKeyValue> ParseSearchFilterFromJson(TSharedPtr<FJsonObject> InJsonObject);

	/**
	* 从构件获取关联的白模数据
	* @param InObj - TSharedPtr<FJsonObject> -
	* @return TSharedPtr<FContentItemSpace::FContentItem> -
	*/
	static TSharedPtr<FContentItemSpace::FContentItem> ParsePakFromDxfJson(TSharedPtr<FJsonObject> InJsonObject);
private:
	//根据类目生成类型
	static bool ParseCategoryToObjectType(FString& InCode,uint32 &OutObjectType);
	//根据类目生成类型名称
	static bool ParseCategoryToClassName(FString& InCode,FString& OutClassName);
	//根据类目选择管线还是水电素材，是管线就返回OutCatory0=5， 否则OutCatory0=4
	static bool ParseCategoryToCategory0(FString& InCode,int32 _componentID, uint32 &OutCatory0);
	//根据类目设置管线颜色
	static bool ParseCategoryToColor(FString& InCode,FColor &OutColor);
	//根据类目设置管线断电改颜色
	static bool ParseCategoryToReformColor(FString& InCode,FColor &OutColor);
	//根据类目设置管线节点颜色（直接，三通，弯头）
	static bool ParseCategoryToPointColor(FString& InCode,FColor &OutColor);
	//根据类目设置管线节点断电改颜色（直接，三通，弯头）
	static bool ParseCategoryToPointReformColor(FString& InCode, FColor &OutColor);
	/**
	 * 通过类目结构获取商品的最底层类目结构，如果有铺法会传出铺法
	 * @param _InArr - TArray<FString> - 传入的level数组 level结构为（1，6，3）
	 * @param outCategryType - Categry1Type - 对应的最基层类目
	 * @return bool - 执行是否成功
	 */


	/**
	 * 根据类目ID转化为对应枚举变量
	 * @param _identity - int32 - 类目ID
	 * @param outCategryType - CategryType & - 转化后的枚举值
	 * @return bool - 转化是否成功
	 */
	static bool ParseLevelToResourceCategryType(int32 _identity,CategryType &outCategryType);

	/**
	 * 由铺法ID获取对应的铺法
	 * @param _parquetId - int32 - 铺法ID
	 * @param outCategryType - CategryApplicationType & - 得到的铺法
	 * @return bool - 执行是否成功
	 */
	static bool ParseLevelToResourceCategryApplicationType(int32 _parquetId, CategryApplicationType &outCategryType);

	///**
	// * 解析模型数据
	// * @param InResourceType - int32 - 资源类型
	// * @param InModelType - int32 - 代表模块 1代表立面 2代表水电
	// * @return FContentItemSpace::FModelRes - 
	// */
	//static TSharedPtr<FContentItemSpace::FModelRes> ParsePakFromJson(TSharedPtr<FJsonObject> InObj, int32 InID, int32 InResourceType,int32 InModelType);
	//



};
