#include "ArmyMulitCategorySlate.h"
#include "ArmyDetailBuilder.h"
#include "Json.h"
#include "SCheckBox.h"
#include "ArmyStringComboBoxSlate.h"
#include "ArmyStyle.h"
#include "ArmyUser.h"

SArmyMulitCategory::SArmyMulitCategory()
{
	DetailBuilders = MakeShareable(new FArmyDetailBuilder);
	NullSpaceDetailBuilders = MakeShareable(new FArmyDetailBuilder);
	CurrentDetailBuilders = MakeShareable(new FArmyDetailBuilder);
	OtherSpaceDetailBuilders = MakeShareable(new FArmyDetailBuilder);
	TempConstructionItemObj = nullptr;
}

SArmyMulitCategory::~SArmyMulitCategory()
{
}

void SArmyMulitCategory::Init(TSharedPtr<FJsonObject> InJsonData, int InSpaceID)
{
	//清空历史缓存数据
	Clear();

	if (!InJsonData.IsValid())
	{
		return;
	}
	//解析数据
	ParseData(InJsonData);
	//生成UI
	GenerateUI(InSpaceID);
}

void SArmyMulitCategory::Init(TArray<TSharedPtr<FJsonObject>> InJsonData, int InSpaceID)
{
	Clear();
	//解析多个json数据
	for (auto It : InJsonData)
	{
		if (!It.IsValid())
		{
			continue;
		}
		ParseData(It);
	}
	//生成UI
	GenerateUI(InSpaceID);
}


void SArmyMulitCategory::Clear()
{
	TM_CheckBoxList.Empty();
	TM_ComboBoxList.Empty();
	TM_ConstructionItemData.Empty();
	TM_ConstructionPropertyData.Empty();
	TM_Property.Empty();
	TM_PropertyValue.Empty();
	TM_CheckedID.Empty();
	TM_NodesData.Empty();
	DetailNodes.Empty();
	AllTopDetailNodes.Empty();
	CachedDefaultCheckedData.Empty();
	TM_ConstructionItemName.Empty();
	TM_ConstructionAvaliableSpaceID.Empty();
	ConstructionStateChangeDelegate.Unbind();
}

TSharedPtr<SWidget> SArmyMulitCategory::GetDetailWidget()
{
	return DetailWidget;
}

TArray<TSharedPtr<FArmyDetailNode>> SArmyMulitCategory::GetDetailNodes()
{
	return AllTopDetailNodes;
}

void SArmyMulitCategory::RefreshCheckData(TSharedPtr<FArmyConstructionItemInterface> InObj)
{	
	TempConstructionItemObj = InObj;

	if (TempConstructionItemObj.IsValid() && (!TempConstructionItemObj->bHasSetted) && bAutoAssignValue)//是否启用框架自动赋值
	{
		TempConstructionItemObj->SetConstructionItemCheckedId(CachedDefaultCheckedData);
		TempConstructionItemObj->bHasSetted = true;
	}
	
	if (InObj.IsValid())
	{
		for (auto It : TM_CheckBoxList)
		{
			It.Value->SetIsChecked(ECheckBoxState::Unchecked);
		}
	}

	if (TempConstructionItemObj.IsValid())
	{		
		for (auto It : TempConstructionItemObj->CheckedId)
		{
			if (TM_CheckBoxList.Contains(It.Key))
			{
				TM_CheckBoxList[It.Key]->SetIsChecked(ECheckBoxState::Checked);

				//在对应品类下属性中查找
				for (auto ConIt : TM_ConstructionPropertyData)
				{
					for (auto ValueIt : It.Value)
					{
						//确认属性ID和下拉框控件都存在
						if (ConIt.Value.Contains(ValueIt.Key) && TM_ComboBoxList.Contains(It.Key))
						{
							//确认该属性ID在下拉框控件中存在
							if (TM_ComboBoxList[It.Key].Contains(ValueIt.Key))
							{
								if (ConIt.Value[ValueIt.Key].Contains(ValueIt.Value))
								{									
									TM_ComboBoxList[It.Key][ValueIt.Key]->SetSelectedItem(ConIt.Value[ValueIt.Key][ValueIt.Value]);
								}								
							}
						}
					}					
				}
			}			
		}
		RefreshItemExpand(nullptr);
	}	
}

TMap<int32/*施工项ID*/, TMap<int32/*属性ID*/, int32/*属性值ID*/>>& SArmyMulitCategory::GetCheckedData()
{
	TM_CheckedID.Reset();
	for (auto It : TM_CheckBoxList)
	{
		if (It.Value->GetCheckedState() == ECheckBoxState::Checked)
		{
			TMap<int32, int32> TempComboID;
			int count = 0;					
			
			//根据被勾选的施工项ID，查找该施工项所有的下拉框中的属性
			for (auto ComboIt : TM_ComboBoxList[It.Key])
			{
				++count;
				//在对应施工项下属性中查找
				for (auto ProIt : TM_ConstructionPropertyData[It.Key])
				{
					for (auto ValueIt : ProIt.Value)
					{
						if (ValueIt.Value == ComboIt.Value->GetSelectedItem())
						{
							if (!TempComboID.Contains(ProIt.Key))
							{
								TempComboID.Add(ProIt.Key, ValueIt.Key);
								break;
							}							
						}
					}
					if (TempComboID.Num() >= count)
					{
						break;
					}
				}				
			}
			TM_CheckedID.Add(It.Key, TempComboID);
		}
	}
	return TM_CheckedID;
}

void SArmyMulitCategory::RefreshItemExpand(TSharedPtr<STreeView<TSharedPtr<FArmyDetailNode>>> TreeView)
{
	if (!TreeView.IsValid() && !CachedTreeView.IsValid())
	{
		return;
	}

	if (TreeView.IsValid())
	{
		CachedTreeView = TreeView;
	}				

	for (auto &It : AllTopDetailNodes)
	{		
		CachedTreeView->SetItemExpansion(It, true);
	}
	
	for (auto& It : DetailNodes)
	{
		if (TM_NodesData.FindKey(It))
		{
			int32 TempKey = *TM_NodesData.FindKey(It);
			CachedTreeView->SetItemExpansion(It, TM_CheckBoxList.Find(TempKey)->Get()->IsChecked());			
		}
	}
}

TMap<int32, TMap<int32, int32>> SArmyMulitCategory::ParseData(TSharedPtr<FJsonObject> InJsonData)
{
	if (!InJsonData.IsValid())
	{
		return CachedDefaultCheckedData;
	}

	const TArray<TSharedPtr<FJsonValue>> JArray = InJsonData->GetArrayField("data");

	for (auto& ArrayIt : JArray)
	{
		TSharedPtr<FJsonObject> JObject = ArrayIt->AsObject();

		if (!JObject.IsValid())
		{
			return CachedDefaultCheckedData;
		}

		//获取此施工项品类下的属性和属性值集合
		int32 CategoryID = JObject->GetIntegerField("categoryId");

		//获取施工项
		const TArray<TSharedPtr<FJsonValue>> ProjectJArray = JObject->GetArrayField("clientQuotaVOList");
		TArray<int32> TempConstructionItemData;
		for (auto & ProjectIt : ProjectJArray)
		{
			TSharedPtr<FJsonObject> ProjectJObject = ProjectIt->AsObject();
			//临时施工项可用空间ID列表
			TArray<int32> TmepAvaliableSpaceID;
			//施工项代码
			FString ConstructionCode = ProjectJObject->GetStringField("code");
			//施工项ID
			int32 ConstructionID = ProjectJObject->GetIntegerField("id");
			//施工项名称
			FString ConstructionName = ProjectJObject->GetStringField("name");
			//施工项是否默认勾选
			bool bIfDefaultCheck = ProjectJObject->GetBoolField("acquiescent");
			//获取空间ID列表
			TArray<FString> TempStringArray;
			ProjectJObject->TryGetStringArrayField("spaceIdList", TempStringArray);
			for (auto& It : TempStringArray)
			{
				TmepAvaliableSpaceID.Add(FCString::Atoi(*It));
			}			

			//施工项ID与名称对应
			TM_ConstructionItemName.Add(ConstructionID, ConstructionName);
			//存储施工项ID
			TempConstructionItemData.Add(ConstructionID);
			//存储施工项ID对应的可用空间ID
			TM_ConstructionAvaliableSpaceID.Add(ConstructionID, TmepAvaliableSpaceID);

			TMap<int32/*属性ID*/, TArray<TSharedPtr<FString>>> TempPropertyValueData;
			TMap<int32, FString> TempPropertyName;
			//属性集合
			TMap<int32, TMap<int32, FString>> TempPropertyData;			
			//获取施工项的默认属性值对应关系表
			TMap<int32, int32> TempPropertyValueRelation;
			const TArray<TSharedPtr<FJsonValue>> PropertyValueRelationJArray = ProjectJObject->GetArrayField("propertyValueRelationList");
			for (auto & PropertyValueRelationIt : PropertyValueRelationJArray)
			{
				TSharedPtr<FJsonObject> PropertyValueRelationJObject = PropertyValueRelationIt->AsObject();
				//属性ID
				int32 PropertyID = PropertyValueRelationJObject->GetIntegerField("propertyId");
				//属性名称
				FString PropertyName = PropertyValueRelationJObject->GetStringField("propertyName");
				//默认属性值ID
				int32 PropertyDefaultValueID = PropertyValueRelationJObject->GetIntegerField("defaultValueId");								
				//存储属性--默认值键值对
				TempPropertyValueRelation.Add(PropertyID, PropertyDefaultValueID);
				//存储属性ID--属性名称
				TempPropertyName.Add(PropertyID, PropertyName);				
				//获取每个施工项下的每个属性及属性下拉列表数据
				const TArray<TSharedPtr<FJsonValue>> PropertyJArray = PropertyValueRelationJObject->GetArrayField("propertyValues");
				//暂存属性值ID--属性值名称数据
				TMap<int32, FString> TempPropertyValueNameData;
				//暂存该属性下全部属性值，用于下拉菜单
				TArray<TSharedPtr<FString>> TempArrayString;
				for (auto& PropertyArrayIt : PropertyJArray)
				{
					//获取属性名称和ID
					TSharedPtr<FJsonObject> PropertyJObject = PropertyArrayIt->AsObject();
					int32 PropertyValueID = PropertyJObject->GetIntegerField("propertyValueId");
					FString PropertyValue = PropertyJObject->GetStringField("value");
					//存储一个属性值ID--属性名
					TempPropertyValueNameData.Add(PropertyValueID, PropertyValue);
					//存储属性值名称
					TempArrayString.Add(MakeShareable(new FString(PropertyValue)));
				}
				//将属性--属性值组存下来
				TempPropertyValueData.Add(PropertyID, TempArrayString);
				//存储施工项下所有属性ID和属性值ID以及属性值
				TempPropertyData.Add(PropertyID, TempPropertyValueNameData);
			}			
			//存储施工项下所有属性ID和对应的全部属性值--用于下拉控件数据源
			TM_PropertyValue.Add(ConstructionID, TempPropertyValueData);
			//存储该施工项下所有属性ID及对应属性名称
			TM_Property.Add(ConstructionID, TempPropertyName);
			//存储该施工项下所有属性及对应的全部属性值ID和属性值
			TM_ConstructionPropertyData.Add(ConstructionID, TempPropertyData);

			//存储默认勾选
			if (bIfDefaultCheck)
			{
				CachedDefaultCheckedData.Add(ConstructionID, TempPropertyValueRelation);
			}			
		}
		//存储品类下所有的施工项ID
		TM_ConstructionItemData.Add(CategoryID, TempConstructionItemData);
	}	

	return CachedDefaultCheckedData;
}

void SArmyMulitCategory::OnCheckBoxStateChanged(ECheckBoxState InNewState)
{
	RefreshItemExpand(nullptr);
	OnListChanged();	
}

void SArmyMulitCategory::OnListChanged()
{
	//特别注意，不同的slate控件（UE4的或者自己写的），onchanged类函数的代理发送时机、数据赋值时机不同
	//有的slate默认值进去或者直接赋值都可能激活代理，需要注意否则这里很容易出问题
	if (TempConstructionItemObj.IsValid() && bAutoAssignValue)//是否使用框架内部自动赋值
	{
		TempConstructionItemObj->SetConstructionItemCheckedId(GetCheckedData());
		TempConstructionItemObj->bHasSetted = true;
	}		
	ConstructionStateChangeDelegate.ExecuteIfBound();
}

void SArmyMulitCategory::GenerateUI(int InSpaceID)
{
	//记录与空间无关联的施工项
	TArray<int32> TempNullSpaceConstruction;
	TArray<int32> TempCurrentSpaceConstruction;
	TArray<int32> TempOtherSpaceConstruction;

	for (auto CategoryIt : TM_ConstructionItemData)
	{
		for (auto ConstructionIt : CategoryIt.Value)
		{
			if (TM_ConstructionAvaliableSpaceID.Contains(ConstructionIt))
			{
				if (InSpaceID == -1)//如果是-1直接列为无关联
				{
					TempNullSpaceConstruction.Add(ConstructionIt);
				}
				else if (TM_ConstructionAvaliableSpaceID[ConstructionIt].Num() > 0)
				{
					if (TM_ConstructionAvaliableSpaceID[ConstructionIt].Contains(InSpaceID))
					{
						TempCurrentSpaceConstruction.Add(ConstructionIt);//当前关联空间的施工项
					}
					else
					{
						TempOtherSpaceConstruction.Add(ConstructionIt);//其他空间的施工项
																	   //其他空间的施工项如果被默认勾选，则取消掉
						if (CachedDefaultCheckedData.Contains(ConstructionIt))
						{
							CachedDefaultCheckedData.Remove(ConstructionIt);
						}
					}
				}
				else//不存在空间ID列表则表示与空间无关联
				{
					TempNullSpaceConstruction.Add(ConstructionIt);
				}
			}
		}
	}

	//不跟空间关联的顶部节点
	FArmyDetailNode& NullSpaceCategoryNode = NullSpaceDetailBuilders->AddCategory("NullSpaceCategoryNode", TEXT("不跟空间关联的施工项"), nullptr, true);
	//跟当前空间关联的顶部节点
	FString TempSpaceStr;
	if (TempCurrentSpaceConstruction.Num() > 0 && FArmyUser::Get().SpaceTypeList->FindByKey(InSpaceID).IsValid())
	{
		TempSpaceStr = FArmyUser::Get().SpaceTypeList->FindByKey(InSpaceID).Get()->Value + TEXT("施工项");
	}
	FArmyDetailNode& CurrentSpaceCategoryNode = CurrentDetailBuilders->AddCategory("CurrentSpaceCategoryNode", TempSpaceStr, nullptr, true);
	//其他空间的顶部节点
	FArmyDetailNode& OtherSpaceCategoryNode = OtherSpaceDetailBuilders->AddCategory("OtherSpaceCategoryNode", TEXT("其他施工项"), nullptr, true);

	if (TempNullSpaceConstruction.Num() > 0)
	{
		AllTopDetailNodes.Add(NullSpaceCategoryNode.AsShared());
	}
	if (TempCurrentSpaceConstruction.Num() > 0)
	{
		AllTopDetailNodes.Add(CurrentSpaceCategoryNode.AsShared());
	}
	if (TempOtherSpaceConstruction.Num() > 0)
	{
		AllTopDetailNodes.Add(OtherSpaceCategoryNode.AsShared());
	}

	//每个施工品类下的全部施工项UI生成
	for (auto CategoryIt : TM_ConstructionItemData)
	{
		for (auto ConstructionIt : CategoryIt.Value)
		{
			TSharedPtr<SCheckBox> TempCheckBox;
			FArmyDetailNode& CategoryNode = DetailBuilders->AddCategory(FName(*("CategoryNode" + FString::FromInt(ConstructionIt))), TM_ConstructionItemName[ConstructionIt],
				SAssignNew(TempCheckBox, SCheckBox)
				.OnCheckStateChanged(this, &SArmyMulitCategory::OnCheckBoxStateChanged)
				.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"))
				.IsChecked(CachedDefaultCheckedData.Contains(ConstructionIt) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked));
			CategoryNode.ValueContentOverride().HAlign(HAlign_Right).Padding(FMargin(0, 0, 4, 0));
			TM_CheckBoxList.Add(ConstructionIt, TempCheckBox);
			TM_ComboBoxList.Add(ConstructionIt);

			//根据属性ID和属性值生成对应下拉
			for (auto& PropertyIt : TM_Property[ConstructionIt])
			{
				FString TempItem = "";
				if (CachedDefaultCheckedData.Contains(ConstructionIt) && CachedDefaultCheckedData[ConstructionIt].Contains(PropertyIt.Key))
				{
					if (TM_ConstructionPropertyData[ConstructionIt][PropertyIt.Key].Contains(CachedDefaultCheckedData[ConstructionIt][PropertyIt.Key]))
					{
						TempItem = TM_ConstructionPropertyData[ConstructionIt][PropertyIt.Key][CachedDefaultCheckedData[ConstructionIt][PropertyIt.Key]];
					}
				}
				TSharedPtr<SArmyStringComboBox> TempComboBox;
				SAssignNew(TempComboBox, SArmyStringComboBox)
					.OnSelected(this, &SArmyMulitCategory::OnListChanged)
					.OptionsSource(TM_PropertyValue[ConstructionIt][PropertyIt.Key])
					.DefaultItem(TempItem);
				//如果下拉属性值只有一条则不显示
				if (TM_PropertyValue[ConstructionIt][PropertyIt.Key].Num() > 1)
				{
					CategoryNode.AddChildNode(FName(*PropertyIt.Value), PropertyIt.Value,
						TempComboBox).ValueContentOverride().HAlign(HAlign_Fill);
				}
				TM_ComboBoxList[ConstructionIt].Add(PropertyIt.Key, TempComboBox);
			}

			if (TempNullSpaceConstruction.Contains(ConstructionIt))
			{
				NullSpaceCategoryNode.AddChildNode(CategoryNode.AsShared());
			}
			else if (TempCurrentSpaceConstruction.Contains(ConstructionIt))
			{
				CurrentSpaceCategoryNode.AddChildNode(CategoryNode.AsShared());
			}
			else if (TempOtherSpaceConstruction.Contains(ConstructionIt))
			{
				OtherSpaceCategoryNode.AddChildNode(CategoryNode.AsShared());
			}

			TM_NodesData.Add(ConstructionIt, CategoryNode.AsShared());
			DetailNodes.Add(CategoryNode.AsShared());
		}
	}

	NullSpaceDetailWidget = NullSpaceDetailBuilders->BuildDetail();
	CurrentSpaceDetailWidget = CurrentDetailBuilders->BuildDetail();
	OtherSpaceDetailWidget = OtherSpaceDetailBuilders->BuildDetail();

	DetailWidget =
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			NullSpaceDetailWidget->AsShared()
		]
	+ SVerticalBox::Slot()
		[
			CurrentSpaceDetailWidget->AsShared()
		]
	+ SVerticalBox::Slot()
		[
			OtherSpaceDetailWidget->AsShared()
		];
}

