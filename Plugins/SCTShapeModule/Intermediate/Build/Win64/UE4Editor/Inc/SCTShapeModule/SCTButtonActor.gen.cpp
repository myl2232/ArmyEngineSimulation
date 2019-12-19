// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SCTShapeModule/Public/Actor/SCTButtonActor.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSCTButtonActor() {}
// Cross Module References
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_ASCTButtonActor_NoRegister();
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_ASCTButtonActor();
	ENGINE_API UClass* Z_Construct_UClass_AActor();
	UPackage* Z_Construct_UPackage__Script_SCTShapeModule();
// End Cross Module References
	void ASCTButtonActor::StaticRegisterNativesASCTButtonActor()
	{
	}
	UClass* Z_Construct_UClass_ASCTButtonActor_NoRegister()
	{
		return ASCTButtonActor::StaticClass();
	}
	struct Z_Construct_UClass_ASCTButtonActor_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_ASCTButtonActor_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AActor,
		(UObject* (*)())Z_Construct_UPackage__Script_SCTShapeModule,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_ASCTButtonActor_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "Actor/SCTButtonActor.h" },
		{ "ModuleRelativePath", "Public/Actor/SCTButtonActor.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_ASCTButtonActor_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ASCTButtonActor>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_ASCTButtonActor_Statics::ClassParams = {
		&ASCTButtonActor::StaticClass,
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
		METADATA_PARAMS(Z_Construct_UClass_ASCTButtonActor_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_ASCTButtonActor_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_ASCTButtonActor()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_ASCTButtonActor_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(ASCTButtonActor, 2989261634);
	template<> SCTSHAPEMODULE_API UClass* StaticClass<ASCTButtonActor>()
	{
		return ASCTButtonActor::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_ASCTButtonActor(Z_Construct_UClass_ASCTButtonActor, &ASCTButtonActor::StaticClass, TEXT("/Script/SCTShapeModule"), TEXT("ASCTButtonActor"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(ASCTButtonActor);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
