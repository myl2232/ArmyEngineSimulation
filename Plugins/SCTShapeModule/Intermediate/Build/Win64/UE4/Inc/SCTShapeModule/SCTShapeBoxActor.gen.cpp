// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Public/Actor/SCTShapeBoxActor.h"
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
	UClass* Z_Construct_UClass_ASCTBoundingBoxActor()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			static UObject* (*const DependentSingletons[])() = {
				(UObject* (*)())Z_Construct_UClass_AActor,
				(UObject* (*)())Z_Construct_UPackage__Script_SCTShapeModule,
			};
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
				{ "IncludePath", "Actor/SCTShapeBoxActor.h" },
				{ "ModuleRelativePath", "Public/Actor/SCTShapeBoxActor.h" },
			};
#endif
			static const FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
				TCppClassTypeTraits<ASCTBoundingBoxActor>::IsAbstract,
			};
			static const UE4CodeGen_Private::FClassParams ClassParams = {
				&ASCTBoundingBoxActor::StaticClass,
				DependentSingletons, ARRAY_COUNT(DependentSingletons),
				0x00900080u,
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
	IMPLEMENT_CLASS(ASCTBoundingBoxActor, 2276176985);
	static FCompiledInDefer Z_CompiledInDefer_UClass_ASCTBoundingBoxActor(Z_Construct_UClass_ASCTBoundingBoxActor, &ASCTBoundingBoxActor::StaticClass, TEXT("/Script/SCTShapeModule"), TEXT("ASCTBoundingBoxActor"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(ASCTBoundingBoxActor);
	void ASCTWireframeActor::StaticRegisterNativesASCTWireframeActor()
	{
	}
	UClass* Z_Construct_UClass_ASCTWireframeActor_NoRegister()
	{
		return ASCTWireframeActor::StaticClass();
	}
	UClass* Z_Construct_UClass_ASCTWireframeActor()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			static UObject* (*const DependentSingletons[])() = {
				(UObject* (*)())Z_Construct_UClass_AActor,
				(UObject* (*)())Z_Construct_UPackage__Script_SCTShapeModule,
			};
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
				{ "IncludePath", "Actor/SCTShapeBoxActor.h" },
				{ "ModuleRelativePath", "Public/Actor/SCTShapeBoxActor.h" },
			};
#endif
			static const FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
				TCppClassTypeTraits<ASCTWireframeActor>::IsAbstract,
			};
			static const UE4CodeGen_Private::FClassParams ClassParams = {
				&ASCTWireframeActor::StaticClass,
				DependentSingletons, ARRAY_COUNT(DependentSingletons),
				0x00900080u,
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
	IMPLEMENT_CLASS(ASCTWireframeActor, 3207208491);
	static FCompiledInDefer Z_CompiledInDefer_UClass_ASCTWireframeActor(Z_Construct_UClass_ASCTWireframeActor, &ASCTWireframeActor::StaticClass, TEXT("/Script/SCTShapeModule"), TEXT("ASCTWireframeActor"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(ASCTWireframeActor);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
