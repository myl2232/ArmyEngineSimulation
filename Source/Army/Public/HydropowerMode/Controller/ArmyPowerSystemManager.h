#pragma once

#include "ArmyOperation.h"
#include "ArmyHydropowerPowerSystem.h"
/**
* 配电箱控制系统
*/
class FArmyPowerSystemManager
{
public:
	FArmyPowerSystemManager();
	virtual ~FArmyPowerSystemManager ();

	void Init();

	void ShowUI();
	/**网络消息获取数据*/
	void GetPowerSystemProductInfo();

	FArmyComboBoxArray& AddNewLoop( int32 LoopType);

	void Save(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter);

	void Load(TSharedPtr<FJsonObject> Data);

	void Cleanup();

	const TSharedPtr<FHydropowerPowerSytem> GetCurrentPowerSystem() const { return CurrentPowerSystem; }
private:
	/**
	* 获取所有房间
	*/
	void InitRoom();

	void OnConfirmClicked();
	void OnCloseClicked();

	void OnSave(TSharedPtr<struct FHydropowerPowerSytem> & InPowerSystem);
	void OnCannel();
private: 
	TSharedPtr<class SArmyHydropowerPowerSystem> PowerSystemView; // 配电系统
	TArray<FObjectWeakPtr> RoomObjectes;
	FArmyComboBoxArray HouseholdCableList;
	FArmyComboBoxArray LoopList;
	TArray<TSharedPtr<struct FArmyObjectIDValue>> SpaceNameList;

	TMap<int32 ,TArray<TSharedPtr<struct FArmyObjectIDValue > > > LoopTypeSpaceNames;

	FArmyComboBoxArray CircuitbreakerNameList;
	FArmyComboBoxArray PipeLineNameList;
	FString HouseholdCableName;

	TMultiMap<int32,TSharedPtr<FArmyKeyValue> > PowerSystemKeyValueMap;//1 断路器 2 照明 3 插座 4 空调 5 动力

	TSharedPtr<FHydropowerPowerSytem> CurrentPowerSystem;
	TSharedPtr<FHydropowerPowerSytem> PrePowerSystem;
};