// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SCTShapeModule/Public/Actor/SCTModelActor.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSCTModelActor() {}
// Cross Module References
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_ASCTModelActor_NoRegister();
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_ASCTModelActor();
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_ASCTShapeActor();
	UPackage* Z_Construct_UPackage__Script_SCTShapeModule();
// End Cross Module References
	void ASCTModelActor::StaticRegisterNativesASCTModelActor()
	{
	}
	UClass* Z_Construct_UClass_ASCTModelActor_NoRegister()
	{
		return ASCTModelActor::StaticClass();
	}
	struct Z_Construct_UClass_ASCTModelActor_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_ASCTModelActor_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_ASCTShapeActor,
		(UObject* (*)())Z_Construct_UPackage__Script_SCTShapeModule,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_ASCTModelActor_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "Actor/SCTModelActor.h" },
		{ "ModuleRelativePath", "Public/Actor/SCTModelActor.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_ASCTModelActor_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ASCTModelActor>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_ASCTModelActor_Statics::ClassParams = {
		&ASCTModelActor::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x009000A4u,
		METADATA_PARAMS(Z_Construct_UClass_ASCTModelActor_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_ASCTModelActor_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_ASCTModelActor()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_ASCTModelActor_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(ASCTModelActor, 1896971358);
	template<> SCTSHAPEMODULE_API UClass* StaticClass<ASCTModelActor>()
	{
		return ASCTModelActor::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_ASCTModelActor(Z_Construct_UClass_ASCTModelActor, &ASCTModelActor::StaticClass, TEXT("/Script/SCTShapeModule"), TEXT("ASCTModelActor"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(ASCTModelActor);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
