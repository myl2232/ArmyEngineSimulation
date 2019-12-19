#include "ArmyTransactionMgr.h"
#include "ArmyTransactionModule.h"
#include "ArmyToolsModule.h"
#include "ArmyRectSelect.h"

void FArmyTransactionMgr::BeginTransaction(const FString& Description)
{
	// 每当有新的操作时，之前的重做记录要清除
	RedoList.Empty();

	// 记录可撤销事务
	UndoList.Add(MakeShareable(new FArmyTransaction(Description)));
	FArmyTransactionModule::Get().Undo = UndoList.Last();
}

void FArmyTransactionMgr::EndTransaction()
{
	FArmyTransactionModule::Get().Undo = NULL;
}

ARMYTRANSACTION_API void FArmyTransactionMgr::Undo()
{
	if (CanUndo())
	{
		TSharedPtr<FArmyTransaction> Trans = UndoList.Last();
		Trans->Undo();
		RedoList.Push(Trans);
		UndoList.Pop();
		/**@于业顺 当撤销或者重做时清空选中列表*/
		FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	}
}

ARMYTRANSACTION_API void FArmyTransactionMgr::Redo()
{
	if (CanRedo())
	{
		TSharedPtr<FArmyTransaction> Trans = RedoList.Last();
		Trans->Redo();
		UndoList.Push(Trans);
		RedoList.Pop();
		/**@于业顺 当撤销或者重做时清空选中列表*/
		FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	}
}

ARMYTRANSACTION_API bool FArmyTransactionMgr::CanUndo()
{
	return UndoList.Num() > 0;
}

ARMYTRANSACTION_API bool FArmyTransactionMgr::CanRedo()
{
	return RedoList.Num() > 0;
}

ARMYTRANSACTION_API void FArmyTransactionMgr::Reset()
{
	UndoList.Reset();
	RedoList.Reset();
	//FArmyTransactionModule::Get().Undo = nullptr;
}

void FArmyTransactionMgr::Resize()
{
	if (UndoList.Num() > TRANSACTION_SIZE)
	{
		UndoList.RemoveAt(0);
	}
}
