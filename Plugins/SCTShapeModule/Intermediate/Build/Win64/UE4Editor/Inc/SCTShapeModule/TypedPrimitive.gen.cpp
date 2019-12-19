// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SCTShapeModule/Public/Actor/TypedPrimitive.h"
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
	struct Z_Construct_UClass_UPNTPrimitive_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FImplementedInterfaceParams InterfaceParams[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UPNTPrimitive_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UMeshComponent,
		(UObject* (*)())Z_Construct_UPackage__Script_SCTShapeModule,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UPNTPrimitive_Statics::Class_MetaDataParams[] = {
		{ "HideCategories", "Mobility Trigger" },
		{ "IncludePath", "Actor/TypedPrimitive.h" },
		{ "ModuleRelativePath", "Public/Actor/TypedPrimitive.h" },
	};
#endif
		const UE4CodeGen_Private::FImplementedInterfaceParams Z_Construct_UClass_UPNTPrimitive_Statics::InterfaceParams[] = {
			{ Z_Construct_UClass_UInterface_CollisionDataProvider_NoRegister, (int32)VTABLE_OFFSET(UPNTPrimitive, IInterface_CollisionDataProvider), false },
		};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UPNTPrimitive_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UPNTPrimitive>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UPNTPrimitive_Statics::ClassParams = {
		&UPNTPrimitive::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		InterfaceParams,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		UE_ARRAY_COUNT(InterfaceParams),
		0x00B000A4u,
		METADATA_PARAMS(Z_Construct_UClass_UPNTPrimitive_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UPNTPrimitive_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UPNTPrimitive()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UPNTPrimitive_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UPNTPrimitive, 2741853025);
	template<> SCTSHAPEMODULE_API UClass* StaticClass<UPNTPrimitive>()
	{
		return UPNTPrimitive::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UPNTPrimitive(Z_Construct_UClass_UPNTPrimitive, &UPNTPrimitive::StaticClass, TEXT("/Script/SCTShapeModule"), TEXT("UPNTPrimitive"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UPNTPrimitive);
	void UPNTUVPrimitive::StaticRegisterNativesUPNTUVPrimitive()
	{
	}
	UClass* Z_Construct_UClass_UPNTUVPrimitive_NoRegister()
	{
		return UPNTUVPrimitive::StaticClass();
	}
	struct Z_Construct_UClass_UPNTUVPrimitive_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const UE4CodeGen_Private::FImplementedInterfaceParams InterfaceParams[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UPNTUVPrimitive_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UMeshComponent,
		(UObject* (*)())Z_Construct_UPackage__Script_SCTShapeModule,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UPNTUVPrimitive_Statics::Class_MetaDataParams[] = {
		{ "HideCategories", "Mobility Trigger" },
		{ "IncludePath", "Actor/TypedPrimitive.h" },
		{ "ModuleRelativePath", "Public/Actor/TypedPrimitive.h" },
	};
#endif
		const UE4CodeGen_Private::FImplementedInterfaceParams Z_Construct_UClass_UPNTUVPrimitive_Statics::InterfaceParams[] = {
			{ Z_Construct_UClass_UInterface_CollisionDataProvider_NoRegister, (int32)VTABLE_OFFSET(UPNTUVPrimitive, IInterface_CollisionDataProvider), false },
		};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UPNTUVPrimitive_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UPNTUVPrimitive>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UPNTUVPrimitive_Statics::ClassParams = {
		&UPNTUVPrimitive::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		InterfaceParams,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		UE_ARRAY_COUNT(InterfaceParams),
		0x00B000A4u,
		METADATA_PARAMS(Z_Construct_UClass_UPNTUVPrimitive_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UPNTUVPrimitive_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UPNTUVPrimitive()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UPNTUVPrimitive_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UPNTUVPrimitive, 675167954);
	template<> SCTSHAPEMODULE_API UClass* StaticClass<UPNTUVPrimitive>()
	{
		return UPNTUVPrimitive::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UPNTUVPrimitive(Z_Construct_UClass_UPNTUVPrimitive, &UPNTUVPrimitive::StaticClass, TEXT("/Script/SCTShapeModule"), TEXT("UPNTUVPrimitive"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UPNTUVPrimitive);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
