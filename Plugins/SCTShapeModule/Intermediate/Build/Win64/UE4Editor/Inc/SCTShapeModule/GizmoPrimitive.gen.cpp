// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SCTShapeModule/Public/Actor/GizmoPrimitive.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGizmoPrimitive() {}
// Cross Module References
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_UGizmoPrimitive_NoRegister();
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_UGizmoPrimitive();
	ENGINE_API UClass* Z_Construct_UClass_UPrimitiveComponent();
	UPackage* Z_Construct_UPackage__Script_SCTShapeModule();
// End Cross Module References
	void UGizmoPrimitive::StaticRegisterNativesUGizmoPrimitive()
	{
	}
	UClass* Z_Construct_UClass_UGizmoPrimitive_NoRegister()
	{
		return UGizmoPrimitive::StaticClass();
	}
	struct Z_Construct_UClass_UGizmoPrimitive_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGizmoPrimitive_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UPrimitiveComponent,
		(UObject* (*)())Z_Construct_UPackage__Script_SCTShapeModule,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGizmoPrimitive_Statics::Class_MetaDataParams[] = {
		{ "HideCategories", "Mobility Trigger" },
		{ "IncludePath", "Actor/GizmoPrimitive.h" },
		{ "ModuleRelativePath", "Public/Actor/GizmoPrimitive.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGizmoPrimitive_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGizmoPrimitive>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGizmoPrimitive_Statics::ClassParams = {
		&UGizmoPrimitive::StaticClass,
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
		0x00B000A4u,
		METADATA_PARAMS(Z_Construct_UClass_UGizmoPrimitive_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UGizmoPrimitive_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGizmoPrimitive()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGizmoPrimitive_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGizmoPrimitive, 2444140897);
	template<> SCTSHAPEMODULE_API UClass* StaticClass<UGizmoPrimitive>()
	{
		return UGizmoPrimitive::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGizmoPrimitive(Z_Construct_UClass_UGizmoPrimitive, &UGizmoPrimitive::StaticClass, TEXT("/Script/SCTShapeModule"), TEXT("UGizmoPrimitive"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGizmoPrimitive);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
