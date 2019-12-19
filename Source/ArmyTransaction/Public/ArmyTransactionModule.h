/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File ArmyTransactionModule.h
* @Description 撤销重做模块
*
* @Author 欧石楠
* @Date 2018年8月1日
* @Version 1.0
*/

#pragma once

#include "ArmyObjectMinimal.h"
#include "ArmyTransaction.h"
#include "Modules/ModuleInterface.h"

class FArmyTransactionModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	ARMYTRANSACTION_API static FArmyTransactionModule& Get();

	/** 当前的撤销事务 */
	TSharedPtr<FArmyTransaction> Undo;

private:
	static FArmyTransactionModule* Singleton;
};