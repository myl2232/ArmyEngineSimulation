// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Public/Actor/SCTModelActor.h"
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
	UClass* Z_Construct_UClass_ASCTModelActor()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			static UObject* (*const DependentSingletons[])() = {
				(UObject* (*)())Z_Construct_UClass_ASCTShapeActor,
				(UObject* (*)())Z_Construct_UPackage__Script_SCTShapeModule,
			};
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
				{ "IncludePath", "Actor/SCTModelActor.h" },
				{ "ModuleRelativePath", "Public/Actor/SCTModelActor.h" },
			};
#endif
			static const FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
				TCppClassTypeTraits<ASCTModelActor>::IsAbstract,
			};
			static const UE4CodeGen_Private::FClassParams ClassParams = {
				&ASCTModelActor::StaticClass,
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
	IMPLEMENT_CLASS(ASCTModelActor, 3374911872);
	static FCompiledInDefer Z_CompiledInDefer_UClass_ASCTModelActor(Z_Construct_UClass_ASCTModelActor, &ASCTModelActor::StaticClass, TEXT("/Script/SCTShapeModule"), TEXT("ASCTModelActor"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(ASCTModelActor);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
