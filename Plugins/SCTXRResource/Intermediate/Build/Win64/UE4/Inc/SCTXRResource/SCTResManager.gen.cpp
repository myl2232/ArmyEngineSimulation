// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Public/SCTResManager.h"
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
		extern SCTXRRESOURCE_API uint32 Get_Z_Construct_UScriptStruct_FSCTVRSObject_CRC();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FSCTVRSObject, Z_Construct_UPackage__Script_SCTXRResource(), TEXT("SCTVRSObject"), sizeof(FSCTVRSObject), Get_Z_Construct_UScriptStruct_FSCTVRSObject_CRC());
	}
	return Singleton;
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FSCTVRSObject(FSCTVRSObject::StaticStruct, TEXT("/Script/SCTXRResource"), TEXT("SCTVRSObject"), false, nullptr, nullptr);
static struct FScriptStruct_SCTXRResource_StaticRegisterNativesFSCTVRSObject
{
	FScriptStruct_SCTXRResource_StaticRegisterNativesFSCTVRSObject()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("SCTVRSObject")),new UScriptStruct::TCppStructOps<FSCTVRSObject>);
	}
} ScriptStruct_SCTXRResource_StaticRegisterNativesFSCTVRSObject;
	UScriptStruct* Z_Construct_UScriptStruct_FSCTVRSObject()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FSCTVRSObject_CRC();
		UPackage* Outer = Z_Construct_UPackage__Script_SCTXRResource();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("SCTVRSObject"), sizeof(FSCTVRSObject), Get_Z_Construct_UScriptStruct_FSCTVRSObject_CRC(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
				{ "ModuleRelativePath", "Public/SCTResManager.h" },
			};
#endif
			auto NewStructOpsLambda = []() -> void* { return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FSCTVRSObject>(); };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_MaterialList_MetaData[] = {
				{ "ModuleRelativePath", "Public/SCTResManager.h" },
			};
#endif
			static const UE4CodeGen_Private::FArrayPropertyParams NewProp_MaterialList = { UE4CodeGen_Private::EPropertyClass::Array, "MaterialList", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000000, 1, nullptr, STRUCT_OFFSET(FSCTVRSObject, MaterialList), METADATA_PARAMS(NewProp_MaterialList_MetaData, ARRAY_COUNT(NewProp_MaterialList_MetaData)) };
			static const UE4CodeGen_Private::FStructPropertyParams NewProp_MaterialList_Inner = { UE4CodeGen_Private::EPropertyClass::Struct, "MaterialList", RF_Public|RF_Transient|RF_MarkAsNative, 0x0000000000000000, 1, nullptr, 0, Z_Construct_UScriptStruct_FSCTModelMaterialData, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Objects_MetaData[] = {
				{ "ModuleRelativePath", "Public/SCTResManager.h" },
				{ "ToolTip", "\xe6\x95\xb0\xe7\xbb\x84\xe5\x85\x83\xe7\xb4\xa0\xe5\x86\x85\xe5\xae\xb9\xe5\xae\x9a\xe4\xb9\x89\xe5\xa6\x82\xe4\xb8\x8b      0:UStaticMesh/UMaterialInterface/UBlueprintClass  1:SkeltalMesh/UStaticMesh(Lamp)  2:UAnimSequence" },
			};
#endif
			static const UE4CodeGen_Private::FArrayPropertyParams NewProp_Objects = { UE4CodeGen_Private::EPropertyClass::Array, "Objects", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000000, 1, nullptr, STRUCT_OFFSET(FSCTVRSObject, Objects), METADATA_PARAMS(NewProp_Objects_MetaData, ARRAY_COUNT(NewProp_Objects_MetaData)) };
			static const UE4CodeGen_Private::FObjectPropertyParams NewProp_Objects_Inner = { UE4CodeGen_Private::EPropertyClass::Object, "Objects", RF_Public|RF_Transient|RF_MarkAsNative, 0x0000000000000000, 1, nullptr, 0, Z_Construct_UClass_UObject_NoRegister, METADATA_PARAMS(nullptr, 0) };
			static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[] = {
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_MaterialList,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_MaterialList_Inner,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_Objects,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_Objects_Inner,
			};
			static const UE4CodeGen_Private::FStructParams ReturnStructParams = {
				(UObject* (*)())Z_Construct_UPackage__Script_SCTXRResource,
				nullptr,
				&UE4CodeGen_Private::TNewCppStructOpsWrapper<decltype(NewStructOpsLambda)>::NewCppStructOps,
				"SCTVRSObject",
				RF_Public|RF_Transient|RF_MarkAsNative,
				EStructFlags(0x00000201),
				sizeof(FSCTVRSObject),
				alignof(FSCTVRSObject),
				PropPointers, ARRAY_COUNT(PropPointers),
				METADATA_PARAMS(Struct_MetaDataParams, ARRAY_COUNT(Struct_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FSCTVRSObject_CRC() { return 2141437179U; }
	void USCTXRResourceManager::StaticRegisterNativesUSCTXRResourceManager()
	{
	}
	UClass* Z_Construct_UClass_USCTXRResourceManager_NoRegister()
	{
		return USCTXRResourceManager::StaticClass();
	}
	UClass* Z_Construct_UClass_USCTXRResourceManager()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			static UObject* (*const DependentSingletons[])() = {
				(UObject* (*)())Z_Construct_UClass_UObject,
				(UObject* (*)())Z_Construct_UPackage__Script_SCTXRResource,
			};
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
				{ "IncludePath", "SCTResManager.h" },
				{ "ModuleRelativePath", "Public/SCTResManager.h" },
				{ "ToolTip", "@brief \xe8\xb5\x84\xe6\xba\x90\xe7\xae\xa1\xe7\x90\x86\xe5\x8a\xa0\xe8\xbd\xbd\xe7\xbc\x93\xe5\xad\x98\xe6\xa8\xa1\xe5\x9d\x97\xef\xbc\x8c\xe6\x9c\xac\xe6\xa8\xa1\xe5\x9d\x97\xe6\x9c\x89\xe9\xa9\xac\xe4\xba\x91\xe9\xbe\x99\xe6\x8f\x90\xe4\xbe\x9b\n@note  \xe4\xbf\xae\xe6\x94\xb9\xe8\xae\xb0\xe5\xbd\x95\xef\xbc\x9a\n                    2018\xe5\xb9\xb4""8\xe6\x9c\x88""7\xe6\x97\xa5:\xe9\x97\xab\xe7\x9b\xb8\xe4\xbc\x9f\n                                    1.\xe7\xa7\x81\xe6\x9c\x89\xe5\x8c\x96\xe6\x9e\x84\xe9\x80\xa0\xe5\x87\xbd\xe6\x95\xb0\xef\xbc\x8c\xe6\x9e\x90\xe6\x9e\x84\xe5\x87\xbd\xe6\x95\xb0\xe9\x80\x9a\xe8\xbf\x87""FSCTXRResourceManagerInstatnce\xe6\x9d\xa5\xe6\x8f\x90\xe4\xbe\x9b\xe5\xa4\x96\xe9\x83\xa8\xe5\x8d\x95\xe4\xbe\x8b\xe5\x8c\x96" },
			};
#endif
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ObjList_MetaData[] = {
				{ "ModuleRelativePath", "Public/SCTResManager.h" },
			};
#endif
			static const UE4CodeGen_Private::FArrayPropertyParams NewProp_ObjList = { UE4CodeGen_Private::EPropertyClass::Array, "ObjList", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000000, 1, nullptr, STRUCT_OFFSET(USCTXRResourceManager, ObjList), METADATA_PARAMS(NewProp_ObjList_MetaData, ARRAY_COUNT(NewProp_ObjList_MetaData)) };
			static const UE4CodeGen_Private::FStructPropertyParams NewProp_ObjList_Inner = { UE4CodeGen_Private::EPropertyClass::Struct, "ObjList", RF_Public|RF_Transient|RF_MarkAsNative, 0x0000000000000000, 1, nullptr, 0, Z_Construct_UScriptStruct_FSCTVRSObject, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_NormalMapList_Leather_MetaData[] = {
				{ "ModuleRelativePath", "Public/SCTResManager.h" },
			};
#endif
			static const UE4CodeGen_Private::FArrayPropertyParams NewProp_NormalMapList_Leather = { UE4CodeGen_Private::EPropertyClass::Array, "NormalMapList_Leather", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000000, 1, nullptr, STRUCT_OFFSET(USCTXRResourceManager, NormalMapList_Leather), METADATA_PARAMS(NewProp_NormalMapList_Leather_MetaData, ARRAY_COUNT(NewProp_NormalMapList_Leather_MetaData)) };
			static const UE4CodeGen_Private::FObjectPropertyParams NewProp_NormalMapList_Leather_Inner = { UE4CodeGen_Private::EPropertyClass::Object, "NormalMapList_Leather", RF_Public|RF_Transient|RF_MarkAsNative, 0x0000000000000000, 1, nullptr, 0, Z_Construct_UClass_UTexture_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_NormalMapList_Wood_MetaData[] = {
				{ "ModuleRelativePath", "Public/SCTResManager.h" },
			};
#endif
			static const UE4CodeGen_Private::FArrayPropertyParams NewProp_NormalMapList_Wood = { UE4CodeGen_Private::EPropertyClass::Array, "NormalMapList_Wood", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000000, 1, nullptr, STRUCT_OFFSET(USCTXRResourceManager, NormalMapList_Wood), METADATA_PARAMS(NewProp_NormalMapList_Wood_MetaData, ARRAY_COUNT(NewProp_NormalMapList_Wood_MetaData)) };
			static const UE4CodeGen_Private::FObjectPropertyParams NewProp_NormalMapList_Wood_Inner = { UE4CodeGen_Private::EPropertyClass::Object, "NormalMapList_Wood", RF_Public|RF_Transient|RF_MarkAsNative, 0x0000000000000000, 1, nullptr, 0, Z_Construct_UClass_UTexture_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_NormalMapList_Metal_MetaData[] = {
				{ "ModuleRelativePath", "Public/SCTResManager.h" },
			};
#endif
			static const UE4CodeGen_Private::FArrayPropertyParams NewProp_NormalMapList_Metal = { UE4CodeGen_Private::EPropertyClass::Array, "NormalMapList_Metal", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000000, 1, nullptr, STRUCT_OFFSET(USCTXRResourceManager, NormalMapList_Metal), METADATA_PARAMS(NewProp_NormalMapList_Metal_MetaData, ARRAY_COUNT(NewProp_NormalMapList_Metal_MetaData)) };
			static const UE4CodeGen_Private::FObjectPropertyParams NewProp_NormalMapList_Metal_Inner = { UE4CodeGen_Private::EPropertyClass::Object, "NormalMapList_Metal", RF_Public|RF_Transient|RF_MarkAsNative, 0x0000000000000000, 1, nullptr, 0, Z_Construct_UClass_UTexture_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_NormalMapList_Cloth_MetaData[] = {
				{ "ModuleRelativePath", "Public/SCTResManager.h" },
			};
#endif
			static const UE4CodeGen_Private::FArrayPropertyParams NewProp_NormalMapList_Cloth = { UE4CodeGen_Private::EPropertyClass::Array, "NormalMapList_Cloth", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000000, 1, nullptr, STRUCT_OFFSET(USCTXRResourceManager, NormalMapList_Cloth), METADATA_PARAMS(NewProp_NormalMapList_Cloth_MetaData, ARRAY_COUNT(NewProp_NormalMapList_Cloth_MetaData)) };
			static const UE4CodeGen_Private::FObjectPropertyParams NewProp_NormalMapList_Cloth_Inner = { UE4CodeGen_Private::EPropertyClass::Object, "NormalMapList_Cloth", RF_Public|RF_Transient|RF_MarkAsNative, 0x0000000000000000, 1, nullptr, 0, Z_Construct_UClass_UTexture_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_IESList_MetaData[] = {
				{ "ModuleRelativePath", "Public/SCTResManager.h" },
			};
#endif
			static const UE4CodeGen_Private::FArrayPropertyParams NewProp_IESList = { UE4CodeGen_Private::EPropertyClass::Array, "IESList", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000000, 1, nullptr, STRUCT_OFFSET(USCTXRResourceManager, IESList), METADATA_PARAMS(NewProp_IESList_MetaData, ARRAY_COUNT(NewProp_IESList_MetaData)) };
			static const UE4CodeGen_Private::FObjectPropertyParams NewProp_IESList_Inner = { UE4CodeGen_Private::EPropertyClass::Object, "IESList", RF_Public|RF_Transient|RF_MarkAsNative, 0x0000000000000000, 1, nullptr, 0, Z_Construct_UClass_UTexture_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_EmptyTexture_MetaData[] = {
				{ "ModuleRelativePath", "Public/SCTResManager.h" },
			};
#endif
			static const UE4CodeGen_Private::FObjectPropertyParams NewProp_EmptyTexture = { UE4CodeGen_Private::EPropertyClass::Object, "EmptyTexture", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000000, 1, nullptr, STRUCT_OFFSET(USCTXRResourceManager, EmptyTexture), Z_Construct_UClass_UTexture_NoRegister, METADATA_PARAMS(NewProp_EmptyTexture_MetaData, ARRAY_COUNT(NewProp_EmptyTexture_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_StaticCommonMaterialList_MetaData[] = {
				{ "ModuleRelativePath", "Public/SCTResManager.h" },
				{ "ToolTip", "Common9\xe4\xb8\xad\xe7\x9a\x84\xe6\x96\xb0\xe6\x9d\x90\xe8\xb4\xa8\xe7\x90\x83\xef\xbc\x88V7\xe7\x89\x88\xef\xbc\x89\xe5\x90\x8d\xe7\xa7\xb0\xe4\xbf\xa1\xe6\x81\xaf\xe5\xaf\xb9\xe7\x85\xa7\xe8\xa1\xa8" },
			};
#endif
			static const UE4CodeGen_Private::FArrayPropertyParams NewProp_StaticCommonMaterialList = { UE4CodeGen_Private::EPropertyClass::Array, "StaticCommonMaterialList", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000000, 1, nullptr, STRUCT_OFFSET(USCTXRResourceManager, StaticCommonMaterialList), METADATA_PARAMS(NewProp_StaticCommonMaterialList_MetaData, ARRAY_COUNT(NewProp_StaticCommonMaterialList_MetaData)) };
			static const UE4CodeGen_Private::FStructPropertyParams NewProp_StaticCommonMaterialList_Inner = { UE4CodeGen_Private::EPropertyClass::Struct, "StaticCommonMaterialList", RF_Public|RF_Transient|RF_MarkAsNative, 0x0000000000000000, 1, nullptr, 0, Z_Construct_UScriptStruct_FSCTCommonMaterialData, METADATA_PARAMS(nullptr, 0) };
			static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[] = {
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_ObjList,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_ObjList_Inner,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_NormalMapList_Leather,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_NormalMapList_Leather_Inner,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_NormalMapList_Wood,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_NormalMapList_Wood_Inner,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_NormalMapList_Metal,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_NormalMapList_Metal_Inner,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_NormalMapList_Cloth,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_NormalMapList_Cloth_Inner,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_IESList,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_IESList_Inner,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_EmptyTexture,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_StaticCommonMaterialList,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_StaticCommonMaterialList_Inner,
			};
			static const FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
				TCppClassTypeTraits<USCTXRResourceManager>::IsAbstract,
			};
			static const UE4CodeGen_Private::FClassParams ClassParams = {
				&USCTXRResourceManager::StaticClass,
				DependentSingletons, ARRAY_COUNT(DependentSingletons),
				0x00100080u,
				nullptr, 0,
				PropPointers, ARRAY_COUNT(PropPointers),
				nullptr,
				&StaticCppClassTypeInfo,
				nullptr, 0,
				METADATA_PARAMS(Class_MetaDataParams, ARRAY_COUNT(Class_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUClass(OuterClass, ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(USCTXRResourceManager, 4198951089);
	static FCompiledInDefer Z_CompiledInDefer_UClass_USCTXRResourceManager(Z_Construct_UClass_USCTXRResourceManager, &USCTXRResourceManager::StaticClass, TEXT("/Script/SCTXRResource"), TEXT("USCTXRResourceManager"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(USCTXRResourceManager);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
