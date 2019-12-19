// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Public/SCTResTypes.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSCTResTypes() {}
// Cross Module References
	SCTXRRESOURCE_API UScriptStruct* Z_Construct_UScriptStruct_FSCTModelMaterialData();
	UPackage* Z_Construct_UPackage__Script_SCTXRResource();
	ENGINE_API UClass* Z_Construct_UClass_UTexture_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_UMaterialInstanceDynamic_NoRegister();
	SCTXRRESOURCE_API UScriptStruct* Z_Construct_UScriptStruct_FSCTCommonMaterialData();
	ENGINE_API UClass* Z_Construct_UClass_UMaterialInterface_NoRegister();
// End Cross Module References
class UScriptStruct* FSCTModelMaterialData::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern SCTXRRESOURCE_API uint32 Get_Z_Construct_UScriptStruct_FSCTModelMaterialData_CRC();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FSCTModelMaterialData, Z_Construct_UPackage__Script_SCTXRResource(), TEXT("SCTModelMaterialData"), sizeof(FSCTModelMaterialData), Get_Z_Construct_UScriptStruct_FSCTModelMaterialData_CRC());
	}
	return Singleton;
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FSCTModelMaterialData(FSCTModelMaterialData::StaticStruct, TEXT("/Script/SCTXRResource"), TEXT("SCTModelMaterialData"), false, nullptr, nullptr);
static struct FScriptStruct_SCTXRResource_StaticRegisterNativesFSCTModelMaterialData
{
	FScriptStruct_SCTXRResource_StaticRegisterNativesFSCTModelMaterialData()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("SCTModelMaterialData")),new UScriptStruct::TCppStructOps<FSCTModelMaterialData>);
	}
} ScriptStruct_SCTXRResource_StaticRegisterNativesFSCTModelMaterialData;
	UScriptStruct* Z_Construct_UScriptStruct_FSCTModelMaterialData()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FSCTModelMaterialData_CRC();
		UPackage* Outer = Z_Construct_UPackage__Script_SCTXRResource();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("SCTModelMaterialData"), sizeof(FSCTModelMaterialData), Get_Z_Construct_UScriptStruct_FSCTModelMaterialData_CRC(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
				{ "ModuleRelativePath", "Public/SCTResTypes.h" },
				{ "ToolTip", "\xe6\xa8\xa1\xe5\x9e\x8b\xe5\xa4\x96\xe6\x8e\xa5\xe4\xbf\xa1\xe6\x81\xaf" },
			};
#endif
			auto NewStructOpsLambda = []() -> void* { return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FSCTModelMaterialData>(); };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_NormalMap_MetaData[] = {
				{ "ModuleRelativePath", "Public/SCTResTypes.h" },
			};
#endif
			static const UE4CodeGen_Private::FObjectPropertyParams NewProp_NormalMap = { UE4CodeGen_Private::EPropertyClass::Object, "NormalMap", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000000, 1, nullptr, STRUCT_OFFSET(FSCTModelMaterialData, NormalMap), Z_Construct_UClass_UTexture_NoRegister, METADATA_PARAMS(NewProp_NormalMap_MetaData, ARRAY_COUNT(NewProp_NormalMap_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_DIM_MetaData[] = {
				{ "ModuleRelativePath", "Public/SCTResTypes.h" },
				{ "ToolTip", "\xe6\xad\xa3\xe5\x9c\xa8\xe4\xbd\xbf\xe7\x94\xa8\xe7\x9a\x84\xe5\x8a\xa8\xe6\x80\x81\xe6\x9d\x90\xe8\xb4\xa8" },
			};
#endif
			static const UE4CodeGen_Private::FObjectPropertyParams NewProp_DIM = { UE4CodeGen_Private::EPropertyClass::Object, "DIM", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000000, 1, nullptr, STRUCT_OFFSET(FSCTModelMaterialData, DIM), Z_Construct_UClass_UMaterialInstanceDynamic_NoRegister, METADATA_PARAMS(NewProp_DIM_MetaData, ARRAY_COUNT(NewProp_DIM_MetaData)) };
			static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[] = {
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_NormalMap,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_DIM,
			};
			static const UE4CodeGen_Private::FStructParams ReturnStructParams = {
				(UObject* (*)())Z_Construct_UPackage__Script_SCTXRResource,
				nullptr,
				&UE4CodeGen_Private::TNewCppStructOpsWrapper<decltype(NewStructOpsLambda)>::NewCppStructOps,
				"SCTModelMaterialData",
				RF_Public|RF_Transient|RF_MarkAsNative,
				EStructFlags(0x00000001),
				sizeof(FSCTModelMaterialData),
				alignof(FSCTModelMaterialData),
				PropPointers, ARRAY_COUNT(PropPointers),
				METADATA_PARAMS(Struct_MetaDataParams, ARRAY_COUNT(Struct_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FSCTModelMaterialData_CRC() { return 3029753875U; }
class UScriptStruct* FSCTCommonMaterialData::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern SCTXRRESOURCE_API uint32 Get_Z_Construct_UScriptStruct_FSCTCommonMaterialData_CRC();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FSCTCommonMaterialData, Z_Construct_UPackage__Script_SCTXRResource(), TEXT("SCTCommonMaterialData"), sizeof(FSCTCommonMaterialData), Get_Z_Construct_UScriptStruct_FSCTCommonMaterialData_CRC());
	}
	return Singleton;
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FSCTCommonMaterialData(FSCTCommonMaterialData::StaticStruct, TEXT("/Script/SCTXRResource"), TEXT("SCTCommonMaterialData"), false, nullptr, nullptr);
static struct FScriptStruct_SCTXRResource_StaticRegisterNativesFSCTCommonMaterialData
{
	FScriptStruct_SCTXRResource_StaticRegisterNativesFSCTCommonMaterialData()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("SCTCommonMaterialData")),new UScriptStruct::TCppStructOps<FSCTCommonMaterialData>);
	}
} ScriptStruct_SCTXRResource_StaticRegisterNativesFSCTCommonMaterialData;
	UScriptStruct* Z_Construct_UScriptStruct_FSCTCommonMaterialData()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FSCTCommonMaterialData_CRC();
		UPackage* Outer = Z_Construct_UPackage__Script_SCTXRResource();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("SCTCommonMaterialData"), sizeof(FSCTCommonMaterialData), Get_Z_Construct_UScriptStruct_FSCTCommonMaterialData_CRC(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
				{ "ModuleRelativePath", "Public/SCTResTypes.h" },
				{ "ToolTip", "\xe8\xae\xb0\xe5\xbd\x95\xe4\xba\x86""Common9\xe4\xb8\xad\xe7\x9a\x84\xe6\x96\xb0\xe6\x9d\x90\xe8\xb4\xa8\xe7\xb3\xbb\xe7\xbb\x9fV7\xe7\x89\x88\xe6\x9c\xac\xe7\x9a\x84\xe6\x9d\x90\xe8\xb4\xa8\xe7\x90\x83\xe5\x90\x8d\xe7\xa7\xb0\xe5\x8f\x8a\xe5\x85\xb6\xe6\x89\x80\xe8\xa6\x81\xe6\x9a\xb4\xe9\x9c\xb2\xe7\x9a\x84\xe5\x8f\x82\xe6\x95\xb0\xe4\xbf\xa1\xe6\x81\xaf" },
			};
#endif
			auto NewStructOpsLambda = []() -> void* { return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FSCTCommonMaterialData>(); };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_MI_MetaData[] = {
				{ "ModuleRelativePath", "Public/SCTResTypes.h" },
			};
#endif
			static const UE4CodeGen_Private::FObjectPropertyParams NewProp_MI = { UE4CodeGen_Private::EPropertyClass::Object, "MI", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000000, 1, nullptr, STRUCT_OFFSET(FSCTCommonMaterialData, MI), Z_Construct_UClass_UMaterialInterface_NoRegister, METADATA_PARAMS(NewProp_MI_MetaData, ARRAY_COUNT(NewProp_MI_MetaData)) };
			static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[] = {
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_MI,
			};
			static const UE4CodeGen_Private::FStructParams ReturnStructParams = {
				(UObject* (*)())Z_Construct_UPackage__Script_SCTXRResource,
				nullptr,
				&UE4CodeGen_Private::TNewCppStructOpsWrapper<decltype(NewStructOpsLambda)>::NewCppStructOps,
				"SCTCommonMaterialData",
				RF_Public|RF_Transient|RF_MarkAsNative,
				EStructFlags(0x00000001),
				sizeof(FSCTCommonMaterialData),
				alignof(FSCTCommonMaterialData),
				PropPointers, ARRAY_COUNT(PropPointers),
				METADATA_PARAMS(Struct_MetaDataParams, ARRAY_COUNT(Struct_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FSCTCommonMaterialData_CRC() { return 1923837215U; }
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
