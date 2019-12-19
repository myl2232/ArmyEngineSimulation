// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SCTShapeModule/Public/Actor/SCTShapeBoxActor.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSCTShapeBoxActor() {}
// Cross Module References
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_ASCTBoundingBoxActor_NoRegister();
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_ASCTBoundingBoxActor();
	ENGINE_API UClass* Z_Construct_UClass_AActor();
	UPackage* Z_Construct_UPackage__Script_SCTShapeModule();
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_ASCTWireframeActor_NoRegister();
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_ASCTWireframeActor();
// End Cross Module References
	void ASCTBoundingBoxActor::StaticRegisterNativesASCTBoundingBoxActor()
	{
	}
	UClass* Z_Construct_UClass_ASCTBoundingBoxActor_NoRegister()
	{
		return ASCTBoundingBoxActor::StaticClass();
	}
	struct Z_Construct_UClass_ASCTBoundingBoxActor_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_ASCTBoundingBoxActor_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AActor,
		(UObject* (*)())Z_Construct_UPackage__Script_SCTShapeModule,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_ASCTBoundingBoxActor_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "Actor/SCTShapeBoxActor.h" },
		{ "ModuleRelativePath", "Public/Actor/SCTShapeBoxActor.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_ASCTBoundingBoxActor_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ASCTBoundingBoxActor>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_ASCTBoundingBoxActor_Statics::ClassParams = {
		&ASCTBoundingBoxActor::StaticClass,
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
		METADATA_PARAMS(Z_Construct_UClass_ASCTBoundingBoxActor_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_ASCTBoundingBoxActor_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_ASCTBoundingBoxActor()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_ASCTBoundingBoxActor_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(ASCTBoundingBoxActor, 1984259084);
	template<> SCTSHAPEMODULE_API UClass* StaticClass<ASCTBoundingBoxActor>()
	{
		return ASCTBoundingBoxActor::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_ASCTBoundingBoxActor(Z_Construct_UClass_ASCTBoundingBoxActor, &ASCTBoundingBoxActor::StaticClass, TEXT("/Script/SCTShapeModule"), TEXT("ASCTBoundingBoxActor"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(ASCTBoundingBoxActor);
	void ASCTWireframeActor::StaticRegisterNativesASCTWireframeActor()
	{
	}
	UClass* Z_Construct_UClass_ASCTWireframeActor_NoRegister()
	{
		return ASCTWireframeActor::StaticClass();
	}
	struct Z_Construct_UClass_ASCTWireframeActor_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_ASCTWireframeActor_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AActor,
		(UObject* (*)())Z_Construct_UPackage__Script_SCTShapeModule,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_ASCTWireframeActor_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "Actor/SCTShapeBoxActor.h" },
		{ "ModuleRelativePath", "Public/Actor/SCTShapeBoxActor.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_ASCTWireframeActor_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ASCTWireframeActor>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_ASCTWireframeActor_Statics::ClassParams = {
		&ASCTWireframeActor::StaticClass,
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
		METADATA_PARAMS(Z_Construct_UClass_ASCTWireframeActor_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_ASCTWireframeActor_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_ASCTWireframeActor()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_ASCTWireframeActor_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(ASCTWireframeActor, 846404516);
	template<> SCTSHAPEMODULE_API UClass* StaticClass<ASCTWireframeActor>()
	{
		return ASCTWireframeActor::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_ASCTWireframeActor(Z_Construct_UClass_ASCTWireframeActor, &ASCTWireframeActor::StaticClass, TEXT("/Script/SCTShapeModule"), TEXT("ASCTWireframeActor"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(ASCTWireframeActor);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
