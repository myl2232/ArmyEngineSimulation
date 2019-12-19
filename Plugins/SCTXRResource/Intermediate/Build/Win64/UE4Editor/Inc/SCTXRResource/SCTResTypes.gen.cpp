// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SCTXRResource/Public/SCTResTypes.h"
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
		extern SCTXRRESOURCE_API uint32 Get_Z_Construct_UScriptStruct_FSCTModelMaterialData_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FSCTModelMaterialData, Z_Construct_UPackage__Script_SCTXRResource(), TEXT("SCTModelMaterialData"), sizeof(FSCTModelMaterialData), Get_Z_Construct_UScriptStruct_FSCTModelMaterialData_Hash());
	}
	return Singleton;
}
template<> SCTXRRESOURCE_API UScriptStruct* StaticStruct<FSCTModelMaterialData>()
{
	return FSCTModelMaterialData::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FSCTModelMaterialData(FSCTModelMaterialData::StaticStruct, TEXT("/Script/SCTXRResource"), TEXT("SCTModelMaterialData"), false, nullptr, nullptr);
static struct FScriptStruct_SCTXRResource_StaticRegisterNativesFSCTModelMaterialData
{
	FScriptStruct_SCTXRResource_StaticRegisterNativesFSCTModelMaterialData()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("SCTModelMaterialData")),new UScriptStruct::TCppStructOps<FSCTModelMaterialData>);
	}
} ScriptStruct_SCTXRResource_StaticRegisterNativesFSCTModelMaterialData;
	struct Z_Construct_UScriptStruct_FSCTModelMaterialData_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_NormalMap_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_NormalMap;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_DIM_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_DIM;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FSCTModelMaterialData_Statics::Struct_MetaDataParams[] = {
		{ "Comment", "//\xe6\xa8\xa1\xe5\x9e\x8b\xe5\xa4\x96\xe6\x8e\xa5\xe4\xbf\xa1\xe6\x81\xaf\n" },
		{ "ModuleRelativePath", "Public/SCTResTypes.h" },
		{ "ToolTip", "\xe6\xa8\xa1\xe5\x9e\x8b\xe5\xa4\x96\xe6\x8e\xa5\xe4\xbf\xa1\xe6\x81\xaf" },
	};
#endif
	void* Z_Construct_UScriptStruct_FSCTModelMaterialData_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FSCTModelMaterialData>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FSCTModelMaterialData_Statics::NewProp_NormalMap_MetaData[] = {
		{ "ModuleRelativePath", "Public/SCTResTypes.h" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FSCTModelMaterialData_Statics::NewProp_NormalMap = { "NormalMap", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FSCTModelMaterialData, NormalMap), Z_Construct_UClass_UTexture_NoRegister, METADATA_PARAMS(Z_Construct_UScriptStruct_FSCTModelMaterialData_Statics::NewProp_NormalMap_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FSCTModelMaterialData_Statics::NewProp_NormalMap_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FSCTModelMaterialData_Statics::NewProp_DIM_MetaData[] = {
		{ "Comment", "//\xe6\xad\xa3\xe5\x9c\xa8\xe4\xbd\xbf\xe7\x94\xa8\xe7\x9a\x84\xe5\x8a\xa8\xe6\x80\x81\xe6\x9d\x90\xe8\xb4\xa8\n" },
		{ "ModuleRelativePath", "Public/SCTResTypes.h" },
		{ "ToolTip", "\xe6\xad\xa3\xe5\x9c\xa8\xe4\xbd\xbf\xe7\x94\xa8\xe7\x9a\x84\xe5\x8a\xa8\xe6\x80\x81\xe6\x9d\x90\xe8\xb4\xa8" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FSCTModelMaterialData_Statics::NewProp_DIM = { "DIM", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FSCTModelMaterialData, DIM), Z_Construct_UClass_UMaterialInstanceDynamic_NoRegister, METADATA_PARAMS(Z_Construct_UScriptStruct_FSCTModelMaterialData_Statics::NewProp_DIM_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FSCTModelMaterialData_Statics::NewProp_DIM_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FSCTModelMaterialData_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FSCTModelMaterialData_Statics::NewProp_NormalMap,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FSCTModelMaterialData_Statics::NewProp_DIM,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FSCTModelMaterialData_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_SCTXRResource,
		nullptr,
		&NewStructOps,
		"SCTModelMaterialData",
		sizeof(FSCTModelMaterialData),
		alignof(FSCTModelMaterialData),
		Z_Construct_UScriptStruct_FSCTModelMaterialData_Statics::PropPointers,
		UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FSCTModelMaterialData_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FSCTModelMaterialData_Statics::Struct_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FSCTModelMaterialData_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FSCTModelMaterialData()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FSCTModelMaterialData_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_SCTXRResource();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("SCTModelMaterialData"), sizeof(FSCTModelMaterialData), Get_Z_Construct_UScriptStruct_FSCTModelMaterialData_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FSCTModelMaterialData_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FSCTModelMaterialData_Hash() { return 3918071464U; }
class UScriptStruct* FSCTCommonMaterialData::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern SCTXRRESOURCE_API uint32 Get_Z_Construct_UScriptStruct_FSCTCommonMaterialData_Hash();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FSCTCommonMaterialData, Z_Construct_UPackage__Script_SCTXRResource(), TEXT("SCTCommonMaterialData"), sizeof(FSCTCommonMaterialData), Get_Z_Construct_UScriptStruct_FSCTCommonMaterialData_Hash());
	}
	return Singleton;
}
template<> SCTXRRESOURCE_API UScriptStruct* StaticStruct<FSCTCommonMaterialData>()
{
	return FSCTCommonMaterialData::StaticStruct();
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FSCTCommonMaterialData(FSCTCommonMaterialData::StaticStruct, TEXT("/Script/SCTXRResource"), TEXT("SCTCommonMaterialData"), false, nullptr, nullptr);
static struct FScriptStruct_SCTXRResource_StaticRegisterNativesFSCTCommonMaterialData
{
	FScriptStruct_SCTXRResource_StaticRegisterNativesFSCTCommonMaterialData()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("SCTCommonMaterialData")),new UScriptStruct::TCppStructOps<FSCTCommonMaterialData>);
	}
} ScriptStruct_SCTXRResource_StaticRegisterNativesFSCTCommonMaterialData;
	struct Z_Construct_UScriptStruct_FSCTCommonMaterialData_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_MI_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_MI;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FSCTCommonMaterialData_Statics::Struct_MetaDataParams[] = {
		{ "Comment", "//\xe8\xae\xb0\xe5\xbd\x95\xe4\xba\x86""Common9\xe4\xb8\xad\xe7\x9a\x84\xe6\x96\xb0\xe6\x9d\x90\xe8\xb4\xa8\xe7\xb3\xbb\xe7\xbb\x9fV7\xe7\x89\x88\xe6\x9c\xac\xe7\x9a\x84\xe6\x9d\x90\xe8\xb4\xa8\xe7\x90\x83\xe5\x90\x8d\xe7\xa7\xb0\xe5\x8f\x8a\xe5\x85\xb6\xe6\x89\x80\xe8\xa6\x81\xe6\x9a\xb4\xe9\x9c\xb2\xe7\x9a\x84\xe5\x8f\x82\xe6\x95\xb0\xe4\xbf\xa1\xe6\x81\xaf\n" },
		{ "ModuleRelativePath", "Public/SCTResTypes.h" },
		{ "ToolTip", "\xe8\xae\xb0\xe5\xbd\x95\xe4\xba\x86""Common9\xe4\xb8\xad\xe7\x9a\x84\xe6\x96\xb0\xe6\x9d\x90\xe8\xb4\xa8\xe7\xb3\xbb\xe7\xbb\x9fV7\xe7\x89\x88\xe6\x9c\xac\xe7\x9a\x84\xe6\x9d\x90\xe8\xb4\xa8\xe7\x90\x83\xe5\x90\x8d\xe7\xa7\xb0\xe5\x8f\x8a\xe5\x85\xb6\xe6\x89\x80\xe8\xa6\x81\xe6\x9a\xb4\xe9\x9c\xb2\xe7\x9a\x84\xe5\x8f\x82\xe6\x95\xb0\xe4\xbf\xa1\xe6\x81\xaf" },
	};
#endif
	void* Z_Construct_UScriptStruct_FSCTCommonMaterialData_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FSCTCommonMaterialData>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FSCTCommonMaterialData_Statics::NewProp_MI_MetaData[] = {
		{ "ModuleRelativePath", "Public/SCTResTypes.h" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FSCTCommonMaterialData_Statics::NewProp_MI = { "MI", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(FSCTCommonMaterialData, MI), Z_Construct_UClass_UMaterialInterface_NoRegister, METADATA_PARAMS(Z_Construct_UScriptStruct_FSCTCommonMaterialData_Statics::NewProp_MI_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FSCTCommonMaterialData_Statics::NewProp_MI_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FSCTCommonMaterialData_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FSCTCommonMaterialData_Statics::NewProp_MI,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FSCTCommonMaterialData_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_SCTXRResource,
		nullptr,
		&NewStructOps,
		"SCTCommonMaterialData",
		sizeof(FSCTCommonMaterialData),
		alignof(FSCTCommonMaterialData),
		Z_Construct_UScriptStruct_FSCTCommonMaterialData_Statics::PropPointers,
		UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FSCTCommonMaterialData_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FSCTCommonMaterialData_Statics::Struct_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FSCTCommonMaterialData_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FSCTCommonMaterialData()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FSCTCommonMaterialData_Hash();
		UPackage* Outer = Z_Construct_UPackage__Script_SCTXRResource();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("SCTCommonMaterialData"), sizeof(FSCTCommonMaterialData), Get_Z_Construct_UScriptStruct_FSCTCommonMaterialData_Hash(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FSCTCommonMaterialData_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FSCTCommonMaterialData_Hash() { return 4233719746U; }
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
