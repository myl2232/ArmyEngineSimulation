// Fill out your copyright notice in the Description page of Project Settings.

#include "ArmyLevelAssetBoardActor.h"
#include "Components/MaterialBillboardComponent.h"

AArmyLevelAssetBoardActor::AArmyLevelAssetBoardActor(const FObjectInitializer& ObjectInitializer)
{
	RootCOM = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootCOM"));
	MatBillboardCOM = ObjectInitializer.CreateDefaultSubobject<UMaterialBillboardComponent>(this, TEXT("MatBillboardCOM"));

	RootComponent = RootCOM;
	MatBillboardCOM->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AArmyLevelAssetBoardActor::BeginPlay()
{
	Super::BeginPlay();
	
	Tilte = FText::FromString(TEXT("标签名称"));
	Des0 = FText::FromString(TEXT("品牌：请输入"));
	Des1 = FText::FromString(TEXT("型号：请输入"));
	Des2 = FText::FromString(TEXT("规格：请输入"));
	Des3 = FText::FromString(TEXT("材质：请输入"));
	Des4 = FText::FromString(TEXT("备注：请输入"));
}

// Called every frame
void AArmyLevelAssetBoardActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AArmyLevelAssetBoardActor::SetVisibility(bool _bVisible)
{
	RootCOM->SetVisibility(_bVisible, true);
}

void AArmyLevelAssetBoardActor::UpdateData(FText _Title, FText _Des0, FText _Des1, FText _Des2, FText _Des3, FText _Des4)
{
	Tilte = _Title;
	Des0 = _Des0;
	Des1 = _Des1;
	Des2 = _Des2;
	Des3 = _Des3;
	Des4 = _Des4;
}

