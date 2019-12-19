// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Public/Actor/GizmoPrimitive.h"
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
	UClass* Z_Construct_UClass_UGizmoPrimitive()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			static UObject* (*const DependentSingletons[])() = {
				(UObject* (*)())Z_Construct_UClass_UPrimitiveComponent,
				(UObject* (*)())Z_Construct_UPackage__Script_SCTShapeModule,
			};
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
				{ "HideCategories", "Mobility Trigger" },
				{ "IncludePath", "Actor/GizmoPrimitive.h" },
				{ "ModuleRelativePath", "Public/Actor/GizmoPrimitive.h" },
			};
#endif
			static const FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
				TCppClassTypeTraits<UGizmoPrimitive>::IsAbstract,
			};
			static const UE4CodeGen_Private::FClassParams ClassParams = {
				&UGizmoPrimitive::StaticClass,
				DependentSingletons, ARRAY_COUNT(DependentSingletons),
				0x00B00080u,
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
	IMPLEMENT_CLASS(UGizmoPrimitive, 452478087);
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGizmoPrimitive(Z_Construct_UClass_UGizmoPrimitive, &UGizmoPrimitive::StaticClass, TEXT("/Script/SCTShapeModule"), TEXT("UGizmoPrimitive"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGizmoPrimitive);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
