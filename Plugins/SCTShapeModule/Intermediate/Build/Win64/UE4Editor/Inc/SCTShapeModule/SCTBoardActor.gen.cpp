// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SCTShapeModule/Public/Actor/SCTBoardActor.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSCTBoardActor() {}
// Cross Module References
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_ASCTBoardActor_NoRegister();
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_ASCTBoardActor();
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_ASCTShapeActor();
	UPackage* Z_Construct_UPackage__Script_SCTShapeModule();
// End Cross Module References
	void ASCTBoardActor::StaticRegisterNativesASCTBoardActor()
	{
	}
	UClass* Z_Construct_UClass_ASCTBoardActor_NoRegister()
	{
		return ASCTBoardActor::StaticClass();
	}
	struct Z_Construct_UClass_ASCTBoardActor_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_ASCTBoardActor_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_ASCTShapeActor,
		(UObject* (*)())Z_Construct_UPackage__Script_SCTShapeModule,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_ASCTBoardActor_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "Actor/SCTBoardActor.h" },
		{ "ModuleRelativePath", "Public/Actor/SCTBoardActor.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_ASCTBoardActor_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ASCTBoardActor>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_ASCTBoardActor_Statics::ClassParams = {
		&ASCTBoardActor::StaticClass,
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
		METADATA_PARAMS(Z_Construct_UClass_ASCTBoardActor_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_ASCTBoardActor_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_ASCTBoardActor()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_ASCTBoardActor_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(ASCTBoardActor, 2527085158);
	template<> SCTSHAPEMODULE_API UClass* StaticClass<ASCTBoardActor>()
	{
		return ASCTBoardActor::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_ASCTBoardActor(Z_Construct_UClass_ASCTBoardActor, &ASCTBoardActor::StaticClass, TEXT("/Script/SCTShapeModule"), TEXT("ASCTBoardActor"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(ASCTBoardActor);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif