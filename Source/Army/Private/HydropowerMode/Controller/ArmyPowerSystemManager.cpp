#include "ArmyPowerSystemManager.h"
#include "ArmyHydropowerPowerSystem.h"
#include "ArmyGameInstance.h"
#include "Delegate.h"
#include "ArmyCommonTools.h"
#include "ArmyCommonTypes.h"
#include "ArmyHydropowerDataManager.h"

FArmyPowerSystemManager::FArmyPowerSystemManager():
	CurrentPowerSystem(MakeShareable(new FHydropowerPowerSytem))
{
}

FArmyPowerSystemManager::~FArmyPowerSystemManager()
{
	
}
void FArmyPowerSystemManager::Init()
{
	if (!PowerSystemView.IsValid())
	{
		InitRoom();

		GetPowerSystemProductInfo();

		HouseholdCableList.Reset();
		HouseholdCableList.Add(MakeShareable(new FArmyKeyValue(1, TEXT("WDZBYJ-3x10-PC32-WC,CC"))));
		HouseholdCableList.Add(MakeShareable(new FArmyKeyValue(2, TEXT("WDZBYJ-5x10-PC32-WC,CC"))));

		LoopList.Reset();
		TArray<TSharedPtr<FPowerSystemLoop>> Loops = FArmyHydropowerDataManager::Get()->GetPowerSystemLoopes();

		for (int32 i = 0 ;i< Loops.Num();i++)
		{
			TSharedPtr<FArmyKeyValue> KeyValue = MakeShareable(new FArmyKeyValue);
			KeyValue->Key = Loops[i]->id;
			KeyValue->Value = Loops[i]->Name;
			LoopList.Add(KeyValue);
			TArray<TSharedPtr<FArmyObjectIDValue>> Arr;
			Arr.Append(SpaceNameList);
			LoopTypeSpaceNames.Add(KeyValue->Key,Arr);
		}

		CircuitbreakerNameList.Reset();
		TArray<TSharedPtr<FArmyKeyValue>> CircuitList;
		PowerSystemKeyValueMap.MultiFind(1, CircuitList);
		CircuitbreakerNameList.Array.Append(CircuitList);

		TSharedPtr<FArmyKeyValue> HouseholdCableName = HouseholdCableList.FindByKey(1);

		SAssignNew(PowerSystemView, SArmyHydropowerPowerSystem)
			.HouseholdCableList(HouseholdCableList)
			.LoopList(Loops)
			.SpaceNameList(SpaceNameList)
			.CircuitBreakerList(CircuitbreakerNameList);
		PowerSystemView->LoopDelegate.BindRaw(this, &FArmyPowerSystemManager::AddNewLoop);
		PowerSystemView->OnSave.BindRaw(this, &FArmyPowerSystemManager::OnSave);
		PowerSystemView->OnCannel.BindRaw(this, &FArmyPowerSystemManager::OnCannel);
	}
}
void FArmyPowerSystemManager::ShowUI()
{
	Init();

	PowerSystemView->LoadPowerSystem(CurrentPowerSystem);

	GGI->Window->PresentModalDialog(
		TEXT("配电箱系统设置"),
		PowerSystemView->AsShared(),
		nullptr/*FSimpleDelegate::CreateRaw(this,&FArmyPowerSystemManager::OnConfirmClicked)*/,
		FSimpleDelegate::CreateRaw(this, &FArmyPowerSystemManager::OnCloseClicked),
		FSimpleDelegate::CreateRaw(this, &FArmyPowerSystemManager::OnCloseClicked)
	);
}

void FArmyPowerSystemManager::Cleanup()
{
	CurrentPowerSystem->reset();
}


void FArmyPowerSystemManager::InitRoom()
{
	SpaceNameList.Reset();
	RoomObjectes.Empty();
	TArray<TWeakPtr<FArmyObject>> InObjects;
	FArmySceneData::Get ()->GetObjects (E_LayoutModel,OT_InternalRoom,InObjects);
	if (InObjects.Num ()==0)
		return;
	for (int32 i = 0;i<InObjects.Num ();i++)
	{
		FObjectPtr object = InObjects[i].Pin ();
		if (object->GetType()!=OT_InternalRoom)
			continue;
		RoomObjectes.AddUnique(object);

		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(object);
		SpaceNameList.Add(MakeShareable(new FArmyObjectIDValue(Room->GetUniqueID(), Room->GetSpaceName(), Room->GetSpaceId())));
		/*if ((Room->GetSpaceId() != RT_NoName) && !(Room->GetSpaceName().Contains(TEXT("")) || Room->GetSpaceName().Contains(TEXT("未命名"))))
		{
			SpaceNameList.Add(MakeShareable(new FArmyObjectIDValue(Room->GetUniqueID(), Room->GetSpaceName(), Room->GetSpaceId())));
		}*/
	}
}

void FArmyPowerSystemManager::OnConfirmClicked()
{

}

void FArmyPowerSystemManager::OnCloseClicked()
{
	GGI->Window->DismissModalDialog();
	PowerSystemView.Reset();
}

void FArmyPowerSystemManager::GetPowerSystemProductInfo()
{
	PowerSystemKeyValueMap.Reset();
	TArray<int32> Keys;
	FArmyHydropowerDataManager::Get()->GetProductKeys(Keys);
	for (int32 i = 0 ;i< Keys.Num();i++)
	{
		int32 Key = Keys[i];
		TArray<TSharedPtr<class FContentItemSpace::FContentItem>> OutArray;
		FArmyHydropowerDataManager::Get()->GetProductInfo(Key,OutArray);
		for (auto ContentItem : OutArray)
		{
			int32 ID = ContentItem->ID;
			FString Name = ContentItem->Name;
			TSharedPtr<FArmyKeyValue> KeyValue = MakeShareable(new FArmyKeyValue( ID,Name));
			PowerSystemKeyValueMap.Add(Key,KeyValue);
		}
	}
}

FArmyComboBoxArray& FArmyPowerSystemManager::AddNewLoop(int32 LoopType)
{
	PipeLineNameList.Reset();


	TArray<TSharedPtr<FArmyKeyValue>> OutArray;
	PowerSystemKeyValueMap.MultiFind(LoopType, OutArray);
	PipeLineNameList.Array.Append(OutArray);

	return PipeLineNameList;
}

void FArmyPowerSystemManager::OnSave(TSharedPtr<FHydropowerPowerSytem>& InPowerSystem)
{
	CurrentPowerSystem = InPowerSystem;
	OnCloseClicked();
}
void FArmyPowerSystemManager::OnCannel()
{
	//if (PowerSystemView.IsValid())
	//	PowerSystemView->LoadPowerSystem(CurrentPowerSystem);
	OnCloseClicked();
}

void FArmyPowerSystemManager::Save(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter)
{
	// 写入开始
	JsonWriter->WriteObjectStart(TEXT("PowerSystem"));
	CurrentPowerSystem->SerializeToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();
}

void FArmyPowerSystemManager::Load(TSharedPtr<FJsonObject> Data)
{
	if (Data.IsValid())
	{
		TSharedPtr<FJsonObject> PowerSystemData = Data->GetObjectField("PowerSystem");
		CurrentPowerSystem->reset();
		CurrentPowerSystem->Deserialization(PowerSystemData);
	}
}