// Fill out your copyright notice in the Description page of Project Settings.

#include "SCTFormulaModule.h"
#include "Modules/ModuleManager.h"
#include "Formula.h"

IMPLEMENT_MODULE(SCTFormulaModule, SCTFormulaModule);

void SCTFormulaModule::StartupModule()
{
	//初始化Formula的全局资源
	FFormula::InitGlobalResource();
}

void SCTFormulaModule::ShutdownModule()
{
	
}