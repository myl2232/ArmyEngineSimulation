// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Public/GTEngineLibrary.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGTEngineLibrary() {}
// Cross Module References
	SCTTESS_API UClass* Z_Construct_UClass_UGTEngineLibrary_NoRegister();
	SCTTESS_API UClass* Z_Construct_UClass_UGTEngineLibrary();
	ENGINE_API UClass* Z_Construct_UClass_UBlueprintFunctionLibrary();
	UPackage* Z_Construct_UPackage__Script_SCTTess();
// End Cross Module References
	void UGTEngineLibrary::StaticRegisterNativesUGTEngineLibrary()
	{
	}
	UClass* Z_Construct_UClass_UGTEngineLibrary_NoRegister()
	{
		return UGTEngineLibrary::StaticClass();
	}
	UClass* Z_Construct_UClass_UGTEngineLibrary()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			static UObject* (*const DependentSingletons[])() = {
				(UObject* (*)())Z_Construct_UClass_UBlueprintFunctionLibrary,
				(UObject* (*)())Z_Construct_UPackage__Script_SCTTess,
			};
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
				{ "IncludePath", "GTEngineLibrary.h" },
				{ "ModuleRelativePath", "Public/GTEngineLibrary.h" },
			};
#endif
			static const FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
				TCppClassTypeTraits<UGTEngineLibrary>::IsAbstract,
			};
			static const UE4CodeGen_Private::FClassParams ClassParams = {
				&UGTEngineLibrary::StaticClass,
				DependentSingletons, ARRAY_COUNT(DependentSingletons),
				0x00100080u,
				nullptr, 0,
				nullptr, 0,
				nullptr,
				&StaticCppClassTypeInfo,
				nullptr, 0,
				METADATA_PARAMS(Class_MetaDataParams, ARRAY_COUNT(Class_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUClass(OuterClass, ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGTEngineLibrary, 54659984);
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGTEngineLibrary(Z_Construct_UClass_UGTEngineLibrary, &UGTEngineLibrary::StaticClass, TEXT("/Script/SCTTess"), TEXT("UGTEngineLibrary"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGTEngineLibrary);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
