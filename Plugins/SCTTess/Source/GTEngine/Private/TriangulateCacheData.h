#pragma once

#include "CoreMinimal.h"


/**
 *@brief 缓存三角网化数据
 */
class FTriangulateCacheData
{	

private:

	/** 缓存数据基类 */
	class FCacheData
	{
	public:
		FCacheData() {}
		FCacheData(const FString & InUUID)
			:UUID(InUUID){}
	public :		
		void SetUUID(const FString & InUUID) { UUID = InUUID; }
		const FString & GetUUID() const { return UUID; }
	protected:
		FString UUID;
	};

	/**
	 *@brief 不带洞的三角网化只需要存储顶点索引列表就可以
	 */
	class FCacheDataWithoutHole : public FCacheData
	{
		friend class FTriangulateCacheData;	
		friend class FCacheDataWihtHole;
	private:
		FCacheDataWithoutHole(const FString & InUUID )
			:FCacheData(InUUID){}
	protected:
		TArray<uint16> IndexArray;
		
	};

	/**
	 *@brief 带洞的三角网化不需要存储索引列表，还需要存储顶点列表
	 */
	class FCacheDataWihtHole : public FCacheDataWithoutHole
	{
		friend class FTriangulateCacheData;
	private:
		FCacheDataWihtHole(const FString & InUUID)
			: FCacheDataWithoutHole(InUUID) {}
	private:
		TArray<FVector> VertexArray;
	};

public:
	/**
	 *@brief 创建或者更新不带洞的三角网化缓存数据
	 *@param[in] InUUID 缓冲 ID
	 *@param[in] InIndexArray 顶点索引列表
	 */
	void CreateOrUpdateCacheData(const FString & InUUID,const TArray<uint16> & InIndexArray);


	/**
	 *@brief 创建或者更新带洞的三角网缓存数据、
	 *@param[In] InUUID 缓冲 ID
	 *@param[in] InVertexArray 顶点数据列表
	 *@param[in] InIndexArray 顶点索引列表
	 */
	void CreateOrUpdateCacheData(const FString & InUUID, const TArray<FVector> & InVertexArray, const TArray<uint16> InIndexArray);



	/**
	 * @brief 获取不带三角网缓存数据
	 * @param[in] InUUID 
	 * @param[out] OutIndexArray 存储顶点索引列表
	 * @return 如果已经缓冲则返回true，否则返回false
	 */
	bool GetCacheData(const FString & InUUID,TArray<uint16> & OutIndexArray) const;


	/**
	 *@brief 获取带洞三角网存数据
	 *@param[in] InUUID 
	 *@param[out] OutIndexArray 存储顶点索引列表
	 *@param[out] OutVertexArray  存储顶点坐标列表
	 */
	bool GetCacheData(const FString & InUUID, TArray<FVector> & OutVertexArray,TArray<uint16> & OutIndexArray) const;

private:
	TMap<FString, TSharedPtr<FCacheData> > CacheData;


public:
	static FTriangulateCacheData & GetInstance();

	static bool StartUp();

	static bool ShutDown();

private:
	static FTriangulateCacheData *  Instance;

};
