// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Public/Actor/SCTDoorSheetActor.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSCTDoorSheetActor() {}
// Cross Module References
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_ASCTDoorSheetActor_NoRegister();
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_ASCTDoorSheetActor();
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_ASCTCompActor();
	UPackage* Z_Construct_UPackage__Script_SCTShapeModule();
// End Cross Module References
	void ASCTDoorSheetActor::StaticRegisterNativesASCTDoorSheetActor()
	{
	}
	UClass* Z_Construct_UClass_ASCTDoorSheetActor_NoRegister()
	{
		return ASCTDoorSheetActor::StaticClass();
	}
	UClass* Z_Construct_UClass_ASCTDoorSheetActor()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			static UObject* (*const DependentSingletons[])() = {
				(UObject* (*)())Z_Construct_UClass_ASCTCompActor,
				(UObject* (*)())Z_Construct_UPackage__Script_SCTShapeModule,
			};
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
				{ "IncludePath", "Actor/SCTDoorSheetActor.h" },
				{ "ModuleRelativePath", "Public/Actor/SCTDoorSheetActor.h" },
			};
#endif
			static const FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
				TCppClassTypeTraits<ASCTDoorSheetActor>::IsAbstract,
			};
			static const UE4CodeGen_Private::FClassParams ClassParams = {
				&ASCTDoorSheetActor::StaticClass,
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
	IMPLEMENT_CLASS(ASCTDoorSheetActor, 617363083);
	static FCompiledInDefer Z_CompiledInDefer_UClass_ASCTDoorSheetActor(Z_Construct_UClass_ASCTDoorSheetActor, &ASCTDoorSheetActor::StaticClass, TEXT("/Script/SCTShapeModule"), TEXT("ASCTDoorSheetActor"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(ASCTDoorSheetActor);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
