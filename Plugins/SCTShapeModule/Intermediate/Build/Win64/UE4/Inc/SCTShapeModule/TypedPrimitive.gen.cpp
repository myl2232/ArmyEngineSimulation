// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Public/Actor/TypedPrimitive.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeTypedPrimitive() {}
// Cross Module References
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_UPNTPrimitive_NoRegister();
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_UPNTPrimitive();
	ENGINE_API UClass* Z_Construct_UClass_UMeshComponent();
	UPackage* Z_Construct_UPackage__Script_SCTShapeModule();
	ENGINE_API UClass* Z_Construct_UClass_UInterface_CollisionDataProvider_NoRegister();
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_UPNTUVPrimitive_NoRegister();
	SCTSHAPEMODULE_API UClass* Z_Construct_UClass_UPNTUVPrimitive();
// End Cross Module References
	void UPNTPrimitive::StaticRegisterNativesUPNTPrimitive()
	{
	}
	UClass* Z_Construct_UClass_UPNTPrimitive_NoRegister()
	{
		return UPNTPrimitive::StaticClass();
	}
	UClass* Z_Construct_UClass_UPNTPrimitive()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			static UObject* (*const DependentSingletons[])() = {
				(UObject* (*)())Z_Construct_UClass_UMeshComponent,
				(UObject* (*)())Z_Construct_UPackage__Script_SCTShapeModule,
			};
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
				{ "HideCategories", "Mobility Trigger" },
				{ "IncludePath", "Actor/TypedPrimitive.h" },
				{ "ModuleRelativePath", "Public/Actor/TypedPrimitive.h" },
			};
#endif
			static const UE4CodeGen_Private::FImplementedInterfaceParams InterfaceParams[] = {
				{ Z_Construct_UClass_UInterface_CollisionDataProvider_NoRegister, (int32)VTABLE_OFFSET(UPNTPrimitive, IInterface_CollisionDataProvider), false },
			};
			static const FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
				TCppClassTypeTraits<UPNTPrimitive>::IsAbstract,
			};
			static const UE4CodeGen_Private::FClassParams ClassParams = {
				&UPNTPrimitive::StaticClass,
				DependentSingletons, ARRAY_COUNT(DependentSingletons),
				0x00B00080u,
				nullptr, 0,
				nullptr, 0,
				nullptr,
				&StaticCppClassTypeInfo,
				InterfaceParams, ARRAY_COUNT(InterfaceParams),
				METADATA_PARAMS(Class_MetaDataParams, ARRAY_COUNT(Class_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUClass(OuterClass, ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UPNTPrimitive, 4045382467);
	static FCompiledInDefer Z_CompiledInDefer_UClass_UPNTPrimitive(Z_Construct_UClass_UPNTPrimitive, &UPNTPrimitive::StaticClass, TEXT("/Script/SCTShapeModule"), TEXT("UPNTPrimitive"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UPNTPrimitive);
	void UPNTUVPrimitive::StaticRegisterNativesUPNTUVPrimitive()
	{
	}
	UClass* Z_Construct_UClass_UPNTUVPrimitive_NoRegister()
	{
		return UPNTUVPrimitive::StaticClass();
	}
	UClass* Z_Construct_UClass_UPNTUVPrimitive()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			static UObject* (*const DependentSingletons[])() = {
				(UObject* (*)())Z_Construct_UClass_UMeshComponent,
				(UObject* (*)())Z_Construct_UPackage__Script_SCTShapeModule,
			};
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
				{ "HideCategories", "Mobility Trigger" },
				{ "IncludePath", "Actor/TypedPrimitive.h" },
				{ "ModuleRelativePath", "Public/Actor/TypedPrimitive.h" },
			};
#endif
			static const UE4CodeGen_Private::FImplementedInterfaceParams InterfaceParams[] = {
				{ Z_Construct_UClass_UInterface_CollisionDataProvider_NoRegister, (int32)VTABLE_OFFSET(UPNTUVPrimitive, IInterface_CollisionDataProvider), false },
			};
			static const FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
				TCppClassTypeTraits<UPNTUVPrimitive>::IsAbstract,
			};
			static const UE4CodeGen_Private::FClassParams ClassParams = {
				&UPNTUVPrimitive::StaticClass,
				DependentSingletons, ARRAY_COUNT(DependentSingletons),
				0x00B00080u,
				nullptr, 0,
				nullptr, 0,
				nullptr,
				&StaticCppClassTypeInfo,
				InterfaceParams, ARRAY_COUNT(InterfaceParams),
				METADATA_PARAMS(Class_MetaDataParams, ARRAY_COUNT(Class_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUClass(OuterClass, ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UPNTUVPrimitive, 1719451210);
	static FCompiledInDefer Z_CompiledInDefer_UClass_UPNTUVPrimitive(Z_Construct_UClass_UPNTUVPrimitive, &UPNTUVPrimitive::StaticClass, TEXT("/Script/SCTShapeModule"), TEXT("UPNTUVPrimitive"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UPNTUVPrimitive);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
