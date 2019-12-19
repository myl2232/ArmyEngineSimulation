// Fill out your copyright notice in the Description page of Project Settings.

#include "SCTShapeModule.h"
#include "Modules/ModuleManager.h"
#include "MaterialManager.h"
#include "SCTGlobalDataForShape.h"

void SCTShapeModule::StartupModule()
{
	FMaterialManagerInstatnce::GetIns().StartUp();		
}

void SCTShapeModule::ShutdownModule()
{
	FMaterialManagerInstatnce::GetIns().ShutDown();	
}


IMPLEMENT_MODULE(SCTShapeModule, SCTShapeModule);
