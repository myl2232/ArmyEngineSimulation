// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SCTShapeModule/Public/Actor/SCTShapeActor.h"
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
	struct Z_Construct_UClass_ASCTShapeActor_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_ASCTShapeActor_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AActor,
		(UObject* (*)())Z_Construct_UPackage__Script_SCTShapeModule,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_ASCTShapeActor_Statics::Class_MetaDataParams[] = {
		{ "Comment", "//Actor\xe5\x9f\xba\xe7\xb1\xbb\n" },
		{ "IncludePath", "Actor/SCTShapeActor.h" },
		{ "ModuleRelativePath", "Public/Actor/SCTShapeActor.h" },
		{ "ToolTip", "Actor\xe5\x9f\xba\xe7\xb1\xbb" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_ASCTShapeActor_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ASCTShapeActor>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_ASCTShapeActor_Statics::ClassParams = {
		&ASCTShapeActor::StaticClass,
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
		METADATA_PARAMS(Z_Construct_UClass_ASCTShapeActor_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_ASCTShapeActor_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_ASCTShapeActor()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_ASCTShapeActor_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(ASCTShapeActor, 815927834);
	template<> SCTSHAPEMODULE_API UClass* StaticClass<ASCTShapeActor>()
	{
		return ASCTShapeActor::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_ASCTShapeActor(Z_Construct_UClass_ASCTShapeActor, &ASCTShapeActor::StaticClass, TEXT("/Script/SCTShapeModule"), TEXT("ASCTShapeActor"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(ASCTShapeActor);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
