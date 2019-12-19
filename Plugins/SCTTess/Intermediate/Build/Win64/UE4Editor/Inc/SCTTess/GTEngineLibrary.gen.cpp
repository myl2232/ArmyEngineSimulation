// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GTEngine/Public/GTEngineLibrary.h"
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
	struct Z_Construct_UClass_UGTEngineLibrary_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGTEngineLibrary_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UBlueprintFunctionLibrary,
		(UObject* (*)())Z_Construct_UPackage__Script_SCTTess,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGTEngineLibrary_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "GTEngineLibrary.h" },
		{ "ModuleRelativePath", "Public/GTEngineLibrary.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGTEngineLibrary_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGTEngineLibrary>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGTEngineLibrary_Statics::ClassParams = {
		&UGTEngineLibrary::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x001000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UGTEngineLibrary_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UGTEngineLibrary_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGTEngineLibrary()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGTEngineLibrary_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGTEngineLibrary, 2386383703);
	template<> SCTTESS_API UClass* StaticClass<UGTEngineLibrary>()
	{
		return UGTEngineLibrary::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGTEngineLibrary(Z_Construct_UClass_UGTEngineLibrary, &UGTEngineLibrary::StaticClass, TEXT("/Script/SCTTess"), TEXT("UGTEngineLibrary"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGTEngineLibrary);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
