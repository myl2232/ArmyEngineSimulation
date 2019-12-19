// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/**
* Copyright 2018 ������Ƽ����޹�˾.
* All Rights Reserved.
* 
*
* @File FArmyHydropowerDataManager.h
* @Description ˮ��ģʽ ���ݹ�����
*
* @Author ��������
* @Date 2018��11��12��
* @Version 1.0
*/
#pragma once

#include "CoreMinimal.h"
#include "ArmyCommonTypes.h"
#include "ArmyObject.h"
#include "ArmyCommonTypes.h"
#include "ArmyObject.h"

/**
* ��·����
*/
struct FPowerSystemLoop
{
	int32 id;
	FString Name;
	int32 LoopType;
	int32 spaceRepeat;
	int32 spaceMultiSelect;

	FPowerSystemLoop()
	{

	}
	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);
};

/**
* Loop Sample Data Structure
* Doc URL: http://101.201.45.88:3000/project/88/interface/api/5794
*/
//Electric Breaker And Cable Infomation Structure
struct FPowerSystemJoinHouseWire
{
	int32 jhwBreakerGoodId;//Breaker
	FString jhwBreakerGoodName;
	int32 jhwCompanyId;
	int32 jhwId;
	int32 jhwIncomingCableType;//Cable
	FString jhwIncomingCableTypeName;
	int32 jhwPowerType;
	int32 jhwSettingId;
};
//Loop Space Infomation Structure
struct FPowerSystemLoopSpace
{
	int32 sid;
	FString sname;
};
//Electric Loop Information Structure
struct FPowerSystemLoopData
{
	int32 lBreakerGoodsId;
	FString lbreakerGoodsName;
	int32 lid;
	int32 lloopType;
	FString lloopTypeCode;
	FString lloopTypeName;
	int32 lsettingId;
	bool lspaceAllSelect;
	int32 lwireGoodsId;
	FString lwireGoodsName;
	TArray<int32> lspaceIdArr;

	TArray<TSharedPtr<FPowerSystemLoopSpace>> lspaceArr;
};
// Core Loop Sample Data Structure
struct FPowerSystemLoopSample
{
	//Base Infomation
	FString otcode;
	int32 otid;
	FString otname;
	FString otremarks;
	int32 otmealid;
	FString otmealname;
	FString otwiretypename;
	int32 otwiretypeid;
	
	TSharedPtr<FPowerSystemJoinHouseWire> joinHouseWire;//Electric Breaker And Cable Infomation Instance Pointer

	TArray<TSharedPtr<FPowerSystemLoopData>> loopSettingArr;//Electric Loop Samples Array
	//Above is Loop Sample Data Structure Definition.

	FPowerSystemLoopSample()
	{
		
	}

	void Deserialization(const TSharedPtr<FJsonObject>& dataObj);
};

class FArmyHydropowerDataManager : public TSharedFromThis<FArmyHydropowerDataManager>
{
public:
	/**
	* ��ȡ����Ʒ����
	*/
	enum PowerSystemProductType
	{
		PSP_NONE,
		PSP_CB = 1, //��·��
		PSP_WL = 2, //����
		PSP_WX = 3, //����
		PSP_WK = 4, //�յ�
		PSP_WP = 5, //����
		PSP_WB = 6, //����
		PSP_Max
	};
public:

	FArmyHydropowerDataManager();

	virtual ~FArmyHydropowerDataManager();

	void Initialize();
	
	static TSharedPtr<FArmyHydropowerDataManager>& Get();

	void GetProductInfo(int32 ProductType,TArray<TSharedPtr<class FContentItemSpace::FContentItem>>& OutArray);

	TArray<TSharedPtr<FPowerSystemLoop>>& GetPowerSystemLoopes(){return PowerSystemLoopes;}

	TSharedPtr<FPowerSystemLoop> GetPowerSystemLoop(int32 InID);

	/**Init Loop Sample Data*/
	void InitPowerSystemLoopSamples();
	TSharedPtr<FPowerSystemLoopSample> GetPowerSystemLoopSample();

	void GetProductKeys(TArray<int32>& OutArray);

	void SetCurrentOpacity(int32 InOpacity){CurrentOpacity = InOpacity;}
	const int32 GetCurrentOpcity() const { return CurrentOpacity; }

	/**
	*������λ
	*/
	TSharedPtr<class FArmyPipePoint> MakeShareablePoint(FVector Location,EObjectType InType,FColor Color,FColor ReformColor,bool bTrans = false);

	//��������
	TSharedPtr<class FArmyPipeline> CreatePipeline (TSharedPtr<FArmyPipePoint> StartPoint,TSharedPtr<FArmyPipePoint> EndPoint,TSharedPtr<class FContentItemSpace::FArmyPipeRes>& Res,FObjectPtr ParentObject = nullptr);
private:
	/**������Ϣ��ȡ����*/
	void GetPowerSystemProductInfo(int32 ProductType);
	void Callback_GetPowerSystemProductInfo(struct FArmyHttpResponse Response, int32 productType);

	/**������Ϣ��ȡ*/
	void GetPowerSystemLoopType(int32 LoopType = 2);
	void Callback_GetPowerSystemLoopType(struct FArmyHttpResponse Response, int32 LoopType);

	/**Get Loop Sample Data From Server*/
	void GetPowerSystemLoopSamples(int32 MealId);
	void Callback_GetPowerSystemLoopSamples(FArmyHttpResponse Response, int32 MealId);

private:
	TMultiMap<int32,TSharedPtr<class FContentItemSpace::FContentItem>> PowerSystemProductMap;//1 ��·�� 2 ���� 3 ���� 4 �յ� 5 ����
	TArray<TSharedPtr<FPowerSystemLoop>> PowerSystemLoopes;

	int32 CurrentOpacity;

	// Loop Sample Instance Pointer
	int32 lastMealId;
	TSharedPtr<FPowerSystemLoopSample> PowerSystemLoopSample;
};
