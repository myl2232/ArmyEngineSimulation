#include "ArmyScopedTransaction.h"
#include "ArmyTransactionMgr.h"

FArmyScopedTransaction::FArmyScopedTransaction(TSharedPtr<FArmyTransactionMgr> InTransMgr, const FString& Description)
{
    check(InTransMgr.IsValid());
    TransMgr = InTransMgr;

    TransMgr->BeginTransaction(Description);
}

FArmyScopedTransaction::~FArmyScopedTransaction()
{
    TransMgr->EndTransaction();
}
