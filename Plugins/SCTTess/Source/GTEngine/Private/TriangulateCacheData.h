#pragma once

#include "CoreMinimal.h"


/**
 *@brief ����������������
 */
class FTriangulateCacheData
{	

private:

	/** �������ݻ��� */
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
	 *@brief ����������������ֻ��Ҫ�洢���������б�Ϳ���
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
	 *@brief ������������������Ҫ�洢�����б�����Ҫ�洢�����б�
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
	 *@brief �������߸��²�����������������������
	 *@param[in] InUUID ���� ID
	 *@param[in] InIndexArray ���������б�
	 */
	void CreateOrUpdateCacheData(const FString & InUUID,const TArray<uint16> & InIndexArray);


	/**
	 *@brief �������߸��´������������������ݡ�
	 *@param[In] InUUID ���� ID
	 *@param[in] InVertexArray ���������б�
	 *@param[in] InIndexArray ���������б�
	 */
	void CreateOrUpdateCacheData(const FString & InUUID, const TArray<FVector> & InVertexArray, const TArray<uint16> InIndexArray);



	/**
	 * @brief ��ȡ������������������
	 * @param[in] InUUID 
	 * @param[out] OutIndexArray �洢���������б�
	 * @return ����Ѿ������򷵻�true�����򷵻�false
	 */
	bool GetCacheData(const FString & InUUID,TArray<uint16> & OutIndexArray) const;


	/**
	 *@brief ��ȡ����������������
	 *@param[in] InUUID 
	 *@param[out] OutIndexArray �洢���������б�
	 *@param[out] OutVertexArray  �洢���������б�
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
