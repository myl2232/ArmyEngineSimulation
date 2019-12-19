/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File MaterialManager.h
* @Description 材质管理接口
*
* @Author 欧石楠
* @Date 2018年8月8日
* @Version 1.0
*/

#pragma once

#include "CoreMinimal.h"


/** 前向声明 */
class FMaterialManagerInstatnce;
class UMaterialInterface;


/** 材质类型 */
enum class EMaterialType
{
	/** 板件材质 */
	Board_Material = 0,
	/** 封边材质 */
	Edge_Material  = 1,
	/** 包边材质 */
	Covered_Material = 2,
	/** 铝箔材质 */
	AluminumFoil_Material = 3,
	/** 门材质*/
	Door_Material = 4
};


/**
 * @brief 材质下载、创建之后，会调用该回调，可以在此回调中执行网格材质的替换,返回材质实例指针以及错误信息
 */
DECLARE_DELEGATE_TwoParams(FMaterialLoadCallback, UMaterialInterface *, const FString &);


class SCTSHAPEMODULE_API FMaterialManager
{	
	friend class FMaterialManagerInstatnce;
private:		
	FMaterialManager();
	~FMaterialManager();

private:
	class FMaterialRequest
	{
		DECLARE_MULTICAST_DELEGATE_TwoParams(FMaterialCallbackMultcast, UMaterialInterface *, const FString &);
		friend class FMaterialManager;
	private:
		FMaterialRequest(const EMaterialType InMaterialType,const FString & InFileId);
	public:
		~FMaterialRequest();
	private:
		static UINT64 GetUUIDFromMaterialTypeAndFileID(const EMaterialType InMaterialType, const FString & InFileId);
		FDelegateHandle BindCallback(const FMaterialLoadCallback InCallback);
	private:
		FMaterialCallbackMultcast CallbackMc;		
		FString RequestID;
		EMaterialType RequestType;
		UINT64 UUID;		 
	};
public:
	/**
	 * @brief 获取或者创建材质实例
	 * @param[in] MaterialType 材质类型，不同类型的材质会缓存到不同的磁盘路径
	 * @param[in] InUrl 材质后台url
	 * @param[in] MaterialID 材质ID
	 * @param[in] InMD5
	 * @param[in] InCallback 因为存在网络下载的情况，所以材质实例有有可能并不会直接返回，所以此处采用了回调方式
	 * @note 这里的材质ID，并非真正存储到材质列表中的ID，中间会再有一层转换与组合
	 * 以排除封边材质、板件材质ID冲突
	 */
	void GetOrCreateMaterialInterface(const EMaterialType MaterialType,const FString & InUrl,
		const FString & InMaterialID,const FString & InMD5,const FMaterialLoadCallback & InCallback);


	/** @copydoc GetOrCreateMaterialInterface
	 *  @param[in] InParameter pak关联的材质参数字符串，json格式的
	 *  例如： {\"D\":[[\"SN:1-V7_Cloth_Simple_MT\",\"100*0.03-0.50-0.00\"]]}
	 */
	void GetOrCreateMaterialInterfaceWithParameter(const EMaterialType MaterialType, const FString & InUrl,
		const FString & InMaterialID, const FString & InMD5, const FString & InParameter, const FMaterialLoadCallback & InCallback);


	/**
	 * @brief 根据材质类型以及材质ID获取材质实例
	 * @param[in] InMaterialType 材质类型
	 * @param[in] InMaterialID 材质ID
	 * @return 如果已经下载过，则可以直接创建，否则返回nullptr
	 * @note 该接口无法实现，在对应的材质pak文件不存在的情况下去下载，如果需要在材质文件不存在的情况下，先去下载然后创建的话，可以使用
	 *   GetOrCreateMaterialInterface
	 */
	UMaterialInterface * GetMaterialInterface(const EMaterialType InMaterialType,const FString & InMaterialID) const;
	UMaterialInterface * GetMaterialInterfaceWithParam(const EMaterialType InMaterialType, const FString & InMaterialID, const FString & InParameter) const;
	/** 获取板件材质存储目录 */
	const FString & GetBoardMaterialCacheDir() const { return BoardMaterialDir; }
	/** 获取封边材质存储目录 */
	const FString & GetEdgeMaterialCacheDir() const { return EdgeMaterialDir; }

	/** 默认材质 */
	void SetLitLitMaterial(UMaterialInterface* InMaterial) { LitLitMaterial = InMaterial; }
	UMaterialInterface* GetLitLitMaterial() { return LitLitMaterial; }
	void SetColorUnlitTMaterial(UMaterialInterface* InMaterial) { ColorUnlitTMaterial = InMaterial; }
	UMaterialInterface* GetColorUnlitTMaterial() { return ColorUnlitTMaterial; }
	void SetForegroundColorMaterial(UMaterialInterface* InMaterial) { ForegroundColorMaterial = InMaterial; }
	UMaterialInterface* GetForegroundColorMaterial() { return ForegroundColorMaterial; }

	/**
	* @brief 通过材质类型以及材质ID，组合出来一个真正的磁盘缓冲路径
	* @param[in] MaterialType
	* @param[in] InMaterialID
	* @return 材质文件全路径
	*/
	const FString & GetFullPathFromMaterialTypeAndID(const EMaterialType MaterialType, const FString & InMaterialID) const;

private:

	FDelegateHandle BindCallbackToMateralRequest(const EMaterialType MaterialType, const FString & InMaterialID, const FMaterialLoadCallback & InCallback);

	bool ExcCallbackFromMaterialTypeAndMaterialID(const EMaterialType MaterialType, const FString & InMaterialID, UMaterialInterface * InMaterial, const FString & InErrorMsg);

	bool RemoveCallbackFromMaterialTypeAndMaterialID(const EMaterialType MaterialType, const FString & InMaterialID, const FDelegateHandle InHandle);

private:
	FString BoardMaterialDir;
	FString EdgeMaterialDir;		
	FString CoverdMaterialDir;
	FString AluminumFoilMaterialDir;
	FString DoorMaterialDir;
	TMap<UINT64, TSharedPtr<FMaterialRequest> > CallbackRequestMap;

	//默认材质
	UPROPERTY()
	UMaterialInterface* LitLitMaterial = nullptr;
	UPROPERTY()
	UMaterialInterface* ColorUnlitTMaterial = nullptr;
	UPROPERTY()
	UMaterialInterface* ForegroundColorMaterial = nullptr;

	static FMaterialManager * InstancePtr;
};

/**
 *@brief 为MaterialManager提供单例封装
 */
class SCTSHAPEMODULE_API FMaterialManagerInstatnce
{
private:
	FMaterialManagerInstatnce();
	~FMaterialManagerInstatnce();
public:	
  static FMaterialManagerInstatnce & GetIns();
  static void ReleaseIns();

public:

	/** 在此函数里面进行UMaterialManager的初始化 */
	bool StartUp();

	/** 在此函数里面进行UMaterialManager的反初始化 */
	bool ShutDown();

	FMaterialManager & GetMaterialManagerRef();

	FMaterialManager * GetMaterialManagerPtr();
private:	
	FMaterialManager * MaterialMgr ;
	static FMaterialManagerInstatnce* Singleton;
};



