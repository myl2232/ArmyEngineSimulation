#pragma once

#include "SArmyWindow.h"
#include "ArmyDesignEditor.h"

#include "Engine/GameInstance.h"
#include "ArmyGameInstance.generated.h"

#define MASTER_PLAN 1

#define SCOPE_TRANSACTION(Description) TSharedPtr<FArmyScopedTransaction> Trans = MakeShareable(new FArmyScopedTransaction(GGI->Window->GetActiveController()->TransMgr, Description))

UCLASS()
class UXRGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UXRGameInstance();
	virtual void Init() override;
	virtual void StartGameInstance() override;
	void OnPostLoadMap(UWorld* InWorld);
	void InitUICommand();
	void InitLocalData();

    /** 重置所有撤销重做队列 */
    void ResetTransactions();

private:
    void OnApplicationClosed();
    void OnApplicationCloseConfirmed();
    void OnGotoHomePage();
    void OnGotoHomePageConfirmed();

	//@郭子阳
	//查看方案估价
	void OnShowPlanPrice();

public:
    /** 主窗口 */
    TSharedPtr<SArmyWindow> Window;

	/**@欧石楠 登录窗口*/
	TSharedPtr<class SArmyLoginWindow> LoginWindow;

    /** 设计界面管理器 */
    TSharedPtr<FArmyDesignEditor> DesignEditor;

    /** 窗口层叠，通过此处添加widget不会影响视口操作 */
    TSharedPtr<SOverlay> WindowOverlay;
};

extern XR_API UXRGameInstance* GGI;