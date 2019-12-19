// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SCTXRResource/Public/SCTResManager.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSCTResManager() {}
// Cross Module References
	SCTXRRESOURCE_API UScriptStruct* Z_Construct_UScriptStruct_FSCTVRSObject();
	UPackage* Z_Construct_UPackage__Script_SCTXRResource();
	SCTXRRESOURCE_API UScriptStruct* Z_Construct_UScriptStruct_FSCTModelMaterialData();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject_NoRegister();
	SCTXRRESOURCE_API UClass* Z_Construct_UClass_USCTXRResourceManager_NoRegister();
	SCTXRRESOURCE_API UClass* Z_Construct_UClass_USCTXRResourceManager();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	ENGINE_API UClass* Z_Construct_UClass_UTexture_NoRegister();
	SCTXRRESOURCE_API UScriptStruct* Z_Construct_UScriptStruct_FSCTCommonMaterialData();
// End Cross Module References
class UScriptStruct* FSCTVRSObject::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern SCTXRRESOURCE_API uint32 Get_Z_Construct_UScriptStruct_FSCTVRSObject_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FSCTVRSObject, Z_Construct_UPackage__Script_SCTXRResource(), TEXT("SCTVRSObject"), sizeof(FSCTVRSObject), Get_Z_Construct_UScriptStruct_FSCTVRSObject_Hash());
	}
	return Singleton;
}
template<> SCTXRRESOURCE_API UScriptStruct* StaticStruct<FSCTVRSObject>()
{
	return FSCTVRSObject::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FSCTVRSObject(FSCTVRSObject::StaticStruct, TEXT("/Script/SCTXRResource"), TEXT("SCTVRSObject"), false, nullptr, nullptr);
static struct FScriptStruct_SCTXRResource_StaticRegisterNativesFSCTVRSObject
{
	FScriptStruct_SCTXRResource_StaticRegisterNativesFSCTVRSObject()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("SCTVRSObject")),new UScriptStruct::TCppStructOps<FSCTVRSObject>);
	}
} ScriptStruct_SCTXRResource_StaticRegisterNativesFSCTVRSObject;
	struct Z_Construct_UScriptStruct_FSCTVRSObject_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_MaterialList_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_MaterialList;
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_MaterialList_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Objects_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_Objects;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_Objects_Inner;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FSCTVRSObject_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/SCTResManager.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FSCTVRSObject_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FSCTVRSObject>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FSCTVRSObject_Statics::NewProp_MaterialList_MetaData[] = {
		{ "ModuleRelativePath", "Public/SCTResManager.h" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UScriptStruct_FSCTVRSObject_Statics::NewProp_MaterialList = { "MaterialList", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FSCTVRSObject, MaterialList), METADATA_PARAMS(Z_Construct_UScriptStruct_FSCTVRSObject_Statics::NewProp_MaterialList_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FSCTVRSObject_Statics::NewProp_MaterialList_MetaData)) };
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FSCTVRSObject_Statics::NewProp_MaterialList_Inner = { "MaterialList", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UScriptStruct_FSCTModelMaterialData, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FSCTVRSObject_Statics::NewProp_Objects_MetaData[] = {
		{ "Comment", "//\xe6\x95\xb0\xe7\xbb\x84\xe5\x85\x83\xe7\xb4\xa0\xe5\x86\x85\xe5\xae\xb9\xe5\xae\x9a\xe4\xb9\x89\xe5\xa6\x82\xe4\xb8\x8b\x09""0:UStaticMesh/UMaterialInterface/UBlueprintClass  1:SkeltalMesh/UStaticMesh(Lamp)  2:UAnimSequence\n" },
		{ "ModuleRelativePath", "Public/SCTResManager.h" },
		{ "ToolTip", "\xe6\x95\xb0\xe7\xbb\x84\xe5\x85\x83\xe7\xb4\xa0\xe5\x86\x85\xe5\xae\xb9\xe5\xae\x9a\xe4\xb9\x89\xe5\xa6\x82\xe4\xb8\x8b      0:UStaticMesh/UMaterialInterface/UBlueprintClass  1:SkeltalMesh/UStaticMesh(Lamp)  2:UAnimSequence" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UScriptStruct_FSCTVRSObject_Statics::NewProp_Objects = { "Objects", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FSCTVRSObject, Objects), METADATA_PARAMS(Z_Construct_UScriptStruct_FSCTVRSObject_Statics::NewProp_Objects_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FSCTVRSObject_Statics::NewProp_Objects_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FSCTVRSObject_Statics::NewProp_Objects_Inner = { "Objects", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UClass_UObject_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FSCTVRSObject_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FSCTVRSObject_Statics::NewProp_MaterialList,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FSCTVRSObject_Statics::NewProp_MaterialList_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FSCTVRSObject_Statics::NewProp_Objects,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FSCTVRSObject_Statics::NewProp_Objects_Inner,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FSCTVRSObject_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_SCTXRResource,
		nullptr,
		&NewStructOps,
		"SCTVRSObject",
		sizeof(FSCTVRSObject),
		alignof(FSCTVRSObject),
		Z_Construct_UScriptStruct_FSCTVRSObject_Statics::PropPointers,
		UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FSCTVRSObject_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000201),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FSCTVRSObject_Statics::Struct_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FSCTVRSObject_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FSCTVRSObject()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FSCTVRSObject_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_SCTXRResource();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("SCTVRSObject"), sizeof(FSCTVRSObject), Get_Z_Construct_UScriptStruct_FSCTVRSObject_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FSCTVRSObject_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FSCTVRSObject_Hash() { return 358646763U; }
	void USCTXRResourceManager::StaticRegisterNativesUSCTXRResourceManager()
	{
	}
	UClass* Z_Construct_UClass_USCTXRResourceManager_NoRegister()
	{
		return USCTXRResourceManager::StaticClass();
	}
	struct Z_Construct_UClass_USCTXRResourceManager_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ObjList_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_ObjList;
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_ObjList_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_NormalMapList_Leather_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_NormalMapList_Leather;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_NormalMapList_Leather_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_NormalMapList_Wood_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_NormalMapList_Wood;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_NormalMapList_Wood_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_NormalMapList_Metal_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_NormalMapList_Metal;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_NormalMapList_Metal_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_NormalMapList_Cloth_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_NormalMapList_Cloth;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_NormalMapList_Cloth_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_IESList_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_IESList;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_IESList_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_EmptyTexture_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_EmptyTexture;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_StaticCommonMaterialList_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_StaticCommonMaterialList;
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_StaticCommonMaterialList_Inner;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_USCTXRResourceManager_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_SCTXRResource,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_USCTXRResourceManager_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * @brief \xe8\xb5\x84\xe6\xba\x90\xe7\xae\xa1\xe7\x90\x86\xe5\x8a\xa0\xe8\xbd\xbd\xe7\xbc\x93\xe5\xad\x98\xe6\xa8\xa1\xe5\x9d\x97\xef\xbc\x8c\xe6\x9c\xac\xe6\xa8\xa1\xe5\x9d\x97\xe6\x9c\x89\xe9\xa9\xac\xe4\xba\x91\xe9\xbe\x99\xe6\x8f\x90\xe4\xbe\x9b\n * @note  \xe4\xbf\xae\xe6\x94\xb9\xe8\xae\xb0\xe5\xbd\x95\xef\xbc\x9a\n *\x09\x09\x09""2018\xe5\xb9\xb4""8\xe6\x9c\x88""7\xe6\x97\xa5:\xe6\xac\xa7\xe7\x9f\xb3\xe6\xa5\xa0\n *\x09\x09\x09\x09\x09""1.\xe7\xa7\x81\xe6\x9c\x89\xe5\x8c\x96\xe6\x9e\x84\xe9\x80\xa0\xe5\x87\xbd\xe6\x95\xb0\xef\xbc\x8c\xe6\x9e\x90\xe6\x9e\x84\xe5\x87\xbd\xe6\x95\xb0\xe9\x80\x9a\xe8\xbf\x87""FSCTXRResourceManagerInstatnce\xe6\x9d\xa5\xe6\x8f\x90\xe4\xbe\x9b\xe5\xa4\x96\xe9\x83\xa8\xe5\x8d\x95\xe4\xbe\x8b\xe5\x8c\x96\n */" },
		{ "IncludePath", "SCTResManager.h" },
		{ "ModuleRelativePath", "Public/SCTResManager.h" },
		{ "ToolTip", "@brief \xe8\xb5\x84\xe6\xba\x90\xe7\xae\xa1\xe7\x90\x86\xe5\x8a\xa0\xe8\xbd\xbd\xe7\xbc\x93\xe5\xad\x98\xe6\xa8\xa1\xe5\x9d\x97\xef\xbc\x8c\xe6\x9c\xac\xe6\xa8\xa1\xe5\x9d\x97\xe6\x9c\x89\xe9\xa9\xac\xe4\xba\x91\xe9\xbe\x99\xe6\x8f\x90\xe4\xbe\x9b\n@note  \xe4\xbf\xae\xe6\x94\xb9\xe8\xae\xb0\xe5\xbd\x95\xef\xbc\x9a\n                    2018\xe5\xb9\xb4""8\xe6\x9c\x88""7\xe6\x97\xa5:\xe6\xac\xa7\xe7\x9f\xb3\xe6\xa5\xa0\n                                    1.\xe7\xa7\x81\xe6\x9c\x89\xe5\x8c\x96\xe6\x9e\x84\xe9\x80\xa0\xe5\x87\xbd\xe6\x95\xb0\xef\xbc\x8c\xe6\x9e\x90\xe6\x9e\x84\xe5\x87\xbd\xe6\x95\xb0\xe9\x80\x9a\xe8\xbf\x87""FSCTXRResourceManagerInstatnce\xe6\x9d\xa5\xe6\x8f\x90\xe4\xbe\x9b\xe5\xa4\x96\xe9\x83\xa8\xe5\x8d\x95\xe4\xbe\x8b\xe5\x8c\x96" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_ObjList_MetaData[] = {
		{ "ModuleRelativePath", "Public/SCTResManager.h" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_ObjList = { "ObjList", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(USCTXRResourceManager, ObjList), METADATA_PARAMS(Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_ObjList_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_ObjList_MetaData)) };
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_ObjList_Inner = { "ObjList", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UScriptStruct_FSCTVRSObject, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Leather_MetaData[] = {
		{ "ModuleRelativePath", "Public/SCTResManager.h" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Leather = { "NormalMapList_Leather", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(USCTXRResourceManager, NormalMapList_Leather), METADATA_PARAMS(Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Leather_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Leather_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Leather_Inner = { "NormalMapList_Leather", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UClass_UTexture_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Wood_MetaData[] = {
		{ "ModuleRelativePath", "Public/SCTResManager.h" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Wood = { "NormalMapList_Wood", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(USCTXRResourceManager, NormalMapList_Wood), METADATA_PARAMS(Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Wood_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Wood_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Wood_Inner = { "NormalMapList_Wood", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UClass_UTexture_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Metal_MetaData[] = {
		{ "ModuleRelativePath", "Public/SCTResManager.h" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Metal = { "NormalMapList_Metal", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(USCTXRResourceManager, NormalMapList_Metal), METADATA_PARAMS(Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Metal_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Metal_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Metal_Inner = { "NormalMapList_Metal", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UClass_UTexture_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Cloth_MetaData[] = {
		{ "ModuleRelativePath", "Public/SCTResManager.h" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Cloth = { "NormalMapList_Cloth", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(USCTXRResourceManager, NormalMapList_Cloth), METADATA_PARAMS(Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Cloth_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Cloth_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Cloth_Inner = { "NormalMapList_Cloth", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UClass_UTexture_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_IESList_MetaData[] = {
		{ "ModuleRelativePath", "Public/SCTResManager.h" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_IESList = { "IESList", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(USCTXRResourceManager, IESList), METADATA_PARAMS(Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_IESList_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_IESList_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_IESList_Inner = { "IESList", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UClass_UTexture_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_EmptyTexture_MetaData[] = {
		{ "ModuleRelativePath", "Public/SCTResManager.h" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_EmptyTexture = { "EmptyTexture", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(USCTXRResourceManager, EmptyTexture), Z_Construct_UClass_UTexture_NoRegister, METADATA_PARAMS(Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_EmptyTexture_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_EmptyTexture_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_StaticCommonMaterialList_MetaData[] = {
		{ "Comment", "//Common9\xe4\xb8\xad\xe7\x9a\x84\xe6\x96\xb0\xe6\x9d\x90\xe8\xb4\xa8\xe7\x90\x83\xef\xbc\x88V7\xe7\x89\x88\xef\xbc\x89\xe5\x90\x8d\xe7\xa7\xb0\xe4\xbf\xa1\xe6\x81\xaf\xe5\xaf\xb9\xe7\x85\xa7\xe8\xa1\xa8 \n" },
		{ "ModuleRelativePath", "Public/SCTResManager.h" },
		{ "ToolTip", "Common9\xe4\xb8\xad\xe7\x9a\x84\xe6\x96\xb0\xe6\x9d\x90\xe8\xb4\xa8\xe7\x90\x83\xef\xbc\x88V7\xe7\x89\x88\xef\xbc\x89\xe5\x90\x8d\xe7\xa7\xb0\xe4\xbf\xa1\xe6\x81\xaf\xe5\xaf\xb9\xe7\x85\xa7\xe8\xa1\xa8" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_StaticCommonMaterialList = { "StaticCommonMaterialList", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(USCTXRResourceManager, StaticCommonMaterialList), METADATA_PARAMS(Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_StaticCommonMaterialList_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_StaticCommonMaterialList_MetaData)) };
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_StaticCommonMaterialList_Inner = { "StaticCommonMaterialList", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UScriptStruct_FSCTCommonMaterialData, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_USCTXRResourceManager_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_ObjList,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_ObjList_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Leather,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Leather_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Wood,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Wood_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Metal,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Metal_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Cloth,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_NormalMapList_Cloth_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_IESList,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_IESList_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_EmptyTexture,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_StaticCommonMaterialList,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_USCTXRResourceManager_Statics::NewProp_StaticCommonMaterialList_Inner,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_USCTXRResourceManager_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<USCTXRResourceManager>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_USCTXRResourceManager_Statics::ClassParams = {
		&USCTXRResourceManager::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_USCTXRResourceManager_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_USCTXRResourceManager_Statics::PropPointers),
		0,
		0x001000A0u,
		METADATA_PARAMS(Z_Construct_UClass_USCTXRResourceManager_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_USCTXRResourceManager_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_USCTXRResourceManager()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_USCTXRResourceManager_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(USCTXRResourceManager, 3355918114);
	template<> SCTXRRESOURCE_API UClass* StaticClass<USCTXRResourceManager>()
	{
		return USCTXRResourceManager::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_USCTXRResourceManager(Z_Construct_UClass_USCTXRResourceManager, &USCTXRResourceManager::StaticClass, TEXT("/Script/SCTXRResource"), TEXT("USCTXRResourceManager"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(USCTXRResourceManager);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
