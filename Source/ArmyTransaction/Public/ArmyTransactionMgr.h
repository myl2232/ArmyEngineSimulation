/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File ArmyTransactionMgr.h
* @Description 事务管理类，管理一个撤销/重做队列
*
* @Author 欧石楠
* @Date 2018年8月1日
* @Version 1.0
*/

#pragma once
#include "CoreMinimal.h"

#define TRANSACTION_SIZE 100

class FArmyTransactionMgr
{
public:
	/**
	* 开始一个事务
	* @param Description - const FString & - 事务的文字描述信息
	*/
	void BeginTransaction(const FString& Description);

	/** 结束一个事务 */
	void EndTransaction();

	/** 撤销 */
	ARMYTRANSACTION_API void Undo();

	/** 重做 */
	ARMYTRANSACTION_API void Redo();

	/** 当前是否可撤销 */
	ARMYTRANSACTION_API bool CanUndo();

	/** 当前是否可重做 */
	ARMYTRANSACTION_API bool CanRedo();

	/** 重置 */
	ARMYTRANSACTION_API void Reset();

private:
	/** 如果超过最大记录数量限制，将最早的记录移除 */
	void Resize();

private:
	/** 撤销队列 */
	TArray< TSharedPtr<class FArmyTransaction> > UndoList;

	/** 重做队列 */
	TArray< TSharedPtr<class FArmyTransaction> > RedoList;
};