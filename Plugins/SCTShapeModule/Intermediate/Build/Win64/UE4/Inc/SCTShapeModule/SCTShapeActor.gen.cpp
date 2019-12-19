// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Public/Actor/SCTShapeActor.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSCTShapeActor() {}
// Cross Module References
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_ASCTShapeActor_NoRegister();
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_ASCTShapeActor();
	ENGINE_API UClass* Z_Construct_UClass_AActor();
	UPackage* Z_Construct_UPackage__Script_SCTShapeModule();
// End Cross Module References
	void ASCTShapeActor::StaticRegisterNativesASCTShapeActor()
	{
	}
	UClass* Z_Construct_UClass_ASCTShapeActor_NoRegister()
	{
		return ASCTShapeActor::StaticClass();
	}
	UClass* Z_Construct_UClass_ASCTShapeActor()
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
				{ "IncludePath", "Actor/SCTShapeActor.h" },
				{ "ModuleRelativePath", "Public/Actor/SCTShapeActor.h" },
				{ "ToolTip", "Actor\xe5\x9f\xba\xe7\xb1\xbb" },
			};
#endif
			static const FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
				TCppClassTypeTraits<ASCTShapeActor>::IsAbstract,
			};
			static const UE4CodeGen_Private::FClassParams ClassParams = {
				&ASCTShapeActor::StaticClass,
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
	IMPLEMENT_CLASS(ASCTShapeActor, 1090314351);
	static FCompiledInDefer Z_CompiledInDefer_UClass_ASCTShapeActor(Z_Construct_UClass_ASCTShapeActor, &ASCTShapeActor::StaticClass, TEXT("/Script/SCTShapeModule"), TEXT("ASCTShapeActor"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(ASCTShapeActor);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
