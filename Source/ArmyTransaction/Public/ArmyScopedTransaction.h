/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRScopedTransaction.h
 * @Description 在作用范围内记录一段事务
 *
 * @Author 欧石楠
 * @Date 2018年8月1日
 * @Version 1.0
 */

#pragma once
 #include "CoreMinimal.h"

class ARMYTRANSACTION_API FArmyScopedTransaction
{
public:
    FArmyScopedTransaction(TSharedPtr<class FArmyTransactionMgr> InTransMgr, const FString& Description);

    ~FArmyScopedTransaction();

private:
    TSharedPtr<class FArmyTransactionMgr> TransMgr;
};