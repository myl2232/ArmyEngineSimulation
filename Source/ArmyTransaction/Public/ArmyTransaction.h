/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File ArmyTransaction.h
 * @Description 撤销/重做事务类
 *
 * @Author 欧石楠
 * @Date 2018年8月1日
 * @Version 1.0
 */

#pragma once

#include "ArmyTypes.h"

/**
 * 事务类型
 */
enum ETransType
{
    /** 创建 */
    TT_Create,

    /** 删除 */
    TT_Delete,

    /** 修改 */
    TT_Modify,
};

class FArmyTransaction
{
public:
    FArmyTransaction(const FString& InDescription);

    /** XRObject对象记录 */
    class FArmyObjectRecord
    {
    public:
        FArmyObjectRecord(
            ETransType InTransType,
            TWeakPtr<class FArmyObject> InPrimaryObject,
            const FString& InRecordBeforeModify,
            const FString& InRecord,
            ArmyArgument InArgs
        )
            : TransType(InTransType)
            , PrimaryObject(InPrimaryObject)
            , RecordBeforeModify(InRecordBeforeModify)
            , Record(InRecord)
            , Args(InArgs)
        {}

    public:
        ETransType TransType;

        /** 事务操作的主对象 */
        TWeakPtr<FArmyObject> PrimaryObject;

        /** json记录 */
        FString RecordBeforeModify;
        FString Record;

        /** 创建XRObject对象需要的参数 */
		ArmyArgument Args;
    };

    /** 撤销 */
    void Undo();

    /** 重做 */
    void Redo();

    /** 保存XRObject */
    ARMYTRANSACTION_API void SaveXRObject(ETransType TransType, TWeakPtr<FArmyObject> PrimaryObject, FString& RecordBeforeModify, FString& Record);

private:
    /** 重做可能遇到之前的记录中PrimaryObject指针已经失效了，需要重新定位 */
    void UpdatePrimaryObjectPtr(TSharedPtr<FArmyObjectRecord> Record, TSharedPtr<class FJsonObject> JsonObject);

private:
    /** 对象的json序列化字符串记录 */
    TArray< TSharedPtr<FArmyObjectRecord> > Records;

    /** 事务描述 */
    FString Description;
};