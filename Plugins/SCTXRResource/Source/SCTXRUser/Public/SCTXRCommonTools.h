/**
 * Copyright 2018 ����������(���)�Ƽ����޹�˾.
 * All Rights Reserved.
 * 
 *
 * @File XRCommonTools.h
 * @Description ȫ�ֳ��õĹ��ߺ���
 *
 * @Author ������
 * @Date 2018��4��3��
 * @Version 1.0
 */

#pragma once
#include "CoreMinimal.h"
#include "SCTXRCommonTypes.h"

class SCTXRUSER_API FSCTXRCommonTools
{
public:
	FSCTXRCommonTools();

	/**
	 * �����ļ������ļ����ͣ���ȡ��Ӧ���ļ�����·��
	 * @param InResourceType - ESCTResourceType - �ļ�����
	 * @param InFileName - const FString & - �ļ���
	 * @return FString - ����·���������ļ�������
	 */
	static FString GetPathFromFileName(ESCTResourceType InResourceType, const FString& InFileName);

	/** �Ƴ���չ�� */
	static bool RemoveFileNameExtension(FString& _FileName);

	/** ��ȡ��չ�� */
	static FString GetFileExtension(FString& _FileName);

	/** ��ȡ��Դ�ı���·�� */
	static FString GetDrawHomeDir();
	static FString GetConstructionDir();
	static FString GetCookedHomeDir();
	static FString GetModelDir();
	static FString GetGroupPlanDir();
	static FString GetAutoDesignGroupPlanDir();
	static FString GetLevelPlanDir();
	static FString GetFloorTextureDir();

	/**
	 * �����ļ�·���ó��ļ�����
	 * @param InFilePath - FString & - ../../../XR/Game/DBJCache/LR/100_XXX.pak
	 * @return FString - 100_XXX.pak
	 */
	static FString GetFileNameFromPath(FString& InFilePath);

	/** ���� ../../../XR/Game/DBJCache/LR/LR_0.pak  ���  ../../../XR/Game/DBJCache/LR/ */
	static FString GetFolderFromPath(FString& _FilePath);

	/** ���� 100_XXXXX.pak ��� 100 */
	static int32 GetFileIDFromName(FString _FileNameWithMD5);

	/** ����ֶκϷ��� */
	static bool CheckTeleNumberValid(FString _str);
	static bool CheckNumberValid(FString _str);
	static bool CheckIDCodeValid(FString _str);
	static bool CheckPasswordValid(FString _str);
	static bool CheckEmpty(FString _str);
	static bool CheckAreaValid(FString _str);
	
	/** Ϊ������Ӷ��ţ�����10000�����10,000 */
	static FString AddCommaForNumber(FString _Number);
	static FString AddCommaForNumber(float _Number);

	/**
	 *  ����IDɾ�������Դ�IDΪ��ͷ���ļ�������ĳ�ļ������� 99_XXX.json  99_WWW.json����ʱ����99���һ��ɾ���������ļ�
	 * @param InID - int32 - �ļ�ID
	 * @param InType - ESCTResourceType - �ļ�����
	 * @return void - 
	 */
	static void DeleteFiles(int32 InID, ESCTResourceType InType);


	/**
	 * ���������JsonObject��������Ŀ��Ԫ����
	 * @param InJsonObject - TSharedPtr<FJsonObject> - 
	 * @return TSharedPtr<FProjectContentItem> - 
	 */
	//static TSharedPtr<FProjectContentItem> ParseProjectContentItemFromJson(TSharedPtr<FJsonObject> InJsonObject);

	/**
	 * ���������JsonObject��������Դ��������Ŀ����ṹ����
	 * @param InJsonObject - TSharedPtr<FJsonObject> - �����JObject
	 * @return FCategoryData - ��Դ����������Ŀ�㼶����
	 */
	static FSCTCategoryContentItem ParseCategoryFromJson(TSharedPtr<FJsonObject> InJsonObject);

	/**
	 * ���������JsonObject��������Դ��������Ŀ����ṹ����,��Ҫ�����ذ�ƴ�����ݽṹ
	 * @param InJsonObject - TSharedPtr<FJsonObject> - �����JObject
	 * @return FSCTCategoryContentItem - �����JObject
	 */
	
	static TSharedPtr<FSCTContentItemSpace::FContentItem> ParseTextureContentItemFromJson(TSharedPtr<FJsonObject> InJsonObject, int32 categoryID, ESCTPavingMethodType PavingType);
	/**
	 * ��Json�����滻��Ʒһ��ContentItem��
	 * @param InJsonObject - TSharedPtr<FJsonValue> - Json����
	 * @param categoryID - int32 - ��Ŀ
	 * @param PavingType - ESCTPavingMethodType - �̷�
	 * @return TSharedPtr<FSCTContentItemSpace::FContentItem> - ����һ��ContentItem
	 */
	static TSharedPtr<FSCTContentItemSpace::FContentItem> ParseTextureReplaceContentItemFromJson(TSharedPtr<FJsonValue> InJsonObject, int32 categoryID, ESCTPavingMethodType PavingType);

	/**
	 * ���������JsonObject��������Դ�������ĵ�Ԫ����
	 * @param InJsonObject - TSharedPtr<FJsonObject> - 
	 * @return TSharedPtr<FSCTContentItemSpace::FContentItem> - 
	 */
	static TSharedPtr<FSCTContentItemSpace::FContentItem> ParseContentItemFromJson(TSharedPtr<FJsonObject> InJsonObject);
private:
	/**
	 * ��Json�������ݵľ�����Ϣ�����������̷�������ͼ�����Ϣ��
	 * @param InJsonObject - TSharedPtr<FJsonValue> - JSon����
	 * @param categoryID - int32 - ��Ŀ
	 * @param resourceType - ESCTResourceType - ��Դ����
	 * @return TSharedPtr<FSCTContentItemSpace::FArmyTextureRes::TextureStruct> - ����һ����Դ�Ľṹ��
	 */
	static TSharedPtr<FSCTContentItemSpace::FArmyTextureRes::TextureStruct> ParseTextureStructItemFromJson(TSharedPtr<FJsonValue> InJsonObject, int32 categoryID, ESCTResourceType resourceType);

};
