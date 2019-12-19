#include "ArmySingleDoorDetail.h"
#include "ArmyDetailBuilder.h"
#include "SArmyComboBox.h"
#include "SArmyEditableTextBox.h"
#include "SScrollWrapBox.h"
#include "ArmyCommonTools.h"
#include "ArmyResourceModule.h"
#include "DownloadTypes.h"
#include "ArmyDownloadModule.h"
#include "ArmySingleDoor.h"
#include "ArmyGameInstance.h"
#include "SArmyEditableNumberBox.h"
#include "SArmyToolTip.h"
#include "SComboBox.h"
#include "ArmyTypes.h"

#include "SArmyMulitCategory.h"
#include "ArmyLayoutModeDetail.h"

FArmySingleDoorDetail::FArmySingleDoorDetail()
    : DetialBuilder(MakeShareable(new FArmyDetailBuilder))
{
	SingleDoorMatList = MakeShareable(new FArmyComboBoxArray);
	SingleDoorMatList->Add(MakeShareable(new FArmyKeyValue(0, TEXT("轻体砖"))));
	SingleDoorMatList->Add(MakeShareable(new FArmyKeyValue(1, TEXT("红砖"))));
	SingleDoorMatList->Add(MakeShareable(new FArmyKeyValue(2, TEXT("轻钢龙骨隔墙"))));
	SingleDoorMatList->Add(MakeShareable(new FArmyKeyValue(3, TEXT("保温板+石膏板保温"))));
	SingleDoorMatList->Add(MakeShareable(new FArmyKeyValue(4, TEXT("轻体砖保温填充材质"))));
}

TSharedPtr<SWidget> FArmySingleDoorDetail::CreateDetailWidget(TAttribute< TWeakPtr<FArmyObject> > InSelectedObject, TSharedPtr<class SArmyMulitCategory> CategoryDetail)
{
    SelectedObject = InSelectedObject;

    FArmyDetailNode& DetailCategory = DetialBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

    DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateNameWidget()).ValueContentOverride();
    DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
    DetailCategory.AddChildNode("Width", TEXT("宽度(mm)"), CreateWidthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
    DetailCategory.AddChildNode("OpenDirection", TEXT("开门方向"), CreateOpenDirectionWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	if (SelectedObject.Get().IsValid())
	{
		TSharedPtr<FArmySingleDoor> TempSingleDoor = StaticCastSharedPtr<FArmySingleDoor>(SelectedObject.Get().Pin());
		if (TempSingleDoor.IsValid() && !TempSingleDoor->bHasConnectPass && TempSingleDoor->InWallType == 0)
		{
			DetailCategory.AddChildNode("MatType", TEXT("材质"), CreateSingleDoorMatTypeWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		}		
	}	

	if (CategoryDetail.IsValid())
	{
		FArmyDetailNode& ConstructionItemCategory = DetialBuilder->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : CategoryDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}

    return DetialBuilder->BuildDetail();
}

TSharedPtr<SWidget> FArmySingleDoorDetail::CreateNameWidget()
{
    return
        SNew(SArmyPropertyEditableTextBox)
        .Text(this, &FArmySingleDoorDetail::GetName)
        .IsReadOnly(true);
}

TSharedPtr<SWidget> FArmySingleDoorDetail::CreateHeightWidget()
{
    return
        SNew(SArmyEditableNumberBox)
        .Text(this, &FArmySingleDoorDetail::GetHeight)
        .OnTextCommitted_Raw(this, &FArmySingleDoorDetail::OnHeightChanged)
        .Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
        .IsFloatOnly(true)
        /*.TipText(MAKE_TEXT("当前高度与商品实际尺寸不符"))*/;
}

TSharedPtr<SWidget> FArmySingleDoorDetail::CreateWidthWidget()
{
    return
        SNew(SArmyEditableNumberBox)
        .Text(this, &FArmySingleDoorDetail::GetLength)
        .OnTextCommitted_Raw(this, &FArmySingleDoorDetail::OnLengthChanged)
        .Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
        .IsFloatOnly(true)
        /*.TipText(MAKE_TEXT("当前宽度与商品实际尺寸不符"))*/;
}

TSharedPtr<SWidget> FArmySingleDoorDetail::CreateOpenDirectionWidget()
{
    if (!OpenDirectionList.IsValid())
    {
        OpenDirectionList = MakeShareable(new FArmyComboBoxArray);
        OpenDirectionList->Add(MakeShareable(new FArmyKeyValue(0, TEXT("请选择开门方向"))));
        OpenDirectionList->Add(MakeShareable(new FArmyKeyValue(1, TEXT("内开/左开"))));
        OpenDirectionList->Add(MakeShareable(new FArmyKeyValue(2, TEXT("内开/右开"))));
        OpenDirectionList->Add(MakeShareable(new FArmyKeyValue(3, TEXT("外开/左开"))));
        OpenDirectionList->Add(MakeShareable(new FArmyKeyValue(4, TEXT("外开/右开"))));
    }

    return
        SNew(SArmyPropertyComboBox)
        .OptionsSource(OpenDirectionList)
        .OnSelectionChanged_Lambda([this](const int32 Key, const FString& Value) {
            TSharedPtr<FArmySingleDoor> SingleDoor = StaticCastSharedPtr<FArmySingleDoor>(SelectedObject.Get().Pin());
	        if (SingleDoor.IsValid())
	        {
                SingleDoor->SetOpenDirectionID(Key);
	        }
        })
        .Value_Lambda([this]() {
            TSharedPtr<FArmySingleDoor> SingleDoor = StaticCastSharedPtr<FArmySingleDoor>(SelectedObject.Get().Pin());
            if (SingleDoor.IsValid())
            {
                TSharedPtr<FArmyKeyValue> Selection = OpenDirectionList->FindByKey(SingleDoor->GetOpenDirectionID());
                if (Selection.IsValid())
                {
                    return FText::FromString(Selection->Value);
                }
            }

            return FText();
        });
}

void FArmySingleDoorDetail::OnHeightChanged(const FText& InText, ETextCommit::Type CommitType)
{
    TSharedPtr<FArmySingleDoor> TempDoor = StaticCastSharedPtr<FArmySingleDoor>(SelectedObject.Get().Pin());
    if (TempDoor.IsValid())
    {
        float TempHeight = FCString::Atof(*InText.ToString());
        TempDoor->SetHeight(TempHeight / 10);

        if (GGI->DesignEditor->GetCurrentDesignMode().Equals(TEXT("原始户型")))
        {
            FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
        }
        else if (GGI->DesignEditor->GetCurrentDesignMode().Equals(TEXT("拆改户型")))
        {
			/** @欧石楠 如果门时放在门洞上的，则影响门洞的拆改项*/
			TempDoor->UpdataConnectedDoorHole();			
            FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();
        }
    }
}

void FArmySingleDoorDetail::OnLengthChanged(const FText& InText, ETextCommit::Type CommitType)
{
    TSharedPtr<FArmySingleDoor> TempDoor = StaticCastSharedPtr<FArmySingleDoor>(SelectedObject.Get().Pin());
    if (TempDoor.IsValid())
    {
        float TempLength = FCString::Atof(*InText.ToString());
        TempDoor->SetLength(TempLength / 10);

        if (GGI->DesignEditor->GetCurrentDesignMode().Equals(TEXT("原始户型")))
        {
            FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
        }
        else if (GGI->DesignEditor->GetCurrentDesignMode().Equals(TEXT("拆改户型")))
        {
            FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();
        }
    }
}

FText FArmySingleDoorDetail::GetName() const
{
    return SelectedObject.Get().IsValid() ? FText::FromString(SelectedObject.Get().Pin()->GetName()) : FText();
}

FText FArmySingleDoorDetail::GetHeight() const
{	
	TSharedPtr<FArmySingleDoor> TempDoor = StaticCastSharedPtr<FArmySingleDoor>(SelectedObject.Get().Pin());
	if (TempDoor.IsValid())
	{
        FString HeightStr = FString::Printf(TEXT("%d"), FMath::RoundToInt(TempDoor->GetHeight() * 10));
        return FText::FromString(HeightStr);
	}	
	return FText::FromString("");
}

FText FArmySingleDoorDetail::GetLength() const
{
	TSharedPtr<FArmySingleDoor> TempDoor = StaticCastSharedPtr<FArmySingleDoor>(SelectedObject.Get().Pin());
	if (TempDoor.IsValid())
	{
        FString HeightStr = FString::Printf(TEXT("%d"), FMath::RoundToInt(TempDoor->GetLength() * 10));
        return FText::FromString(HeightStr);
	}	
	return FText::FromString("");
}

FText FArmySingleDoorDetail::GetThickness() const
{
	TSharedPtr<FArmySingleDoor> TempDoor = StaticCastSharedPtr<FArmySingleDoor>(SelectedObject.Get().Pin());
	if (TempDoor.IsValid())
	{
        FString HeightStr = FString::Printf(TEXT("%d"), FMath::RoundToInt(TempDoor->GetWidth() * 10));
        return FText::FromString(HeightStr);
	}	
	return FText::FromString("");
}

TSharedPtr<SWidget> FArmySingleDoorDetail::CreateSingleDoorMatTypeWidget()
{
	return
		SAssignNew(CB_SingleDoorMatType, SArmyPropertyComboBox)
		.OptionsSource(SingleDoorMatList)
		.OnSelectionChanged(this, &FArmySingleDoorDetail::OnSlectedSingleDoorMatTypeChanged)
		.Value(this, &FArmySingleDoorDetail::GetSingleDoorWallMatType);
}

FText FArmySingleDoorDetail::GetSingleDoorWallMatType() const
{
	TSharedPtr<FArmySingleDoor> Pass = StaticCastSharedPtr<FArmySingleDoor>(SelectedObject.Get().Pin());
	if (Pass.IsValid())
	{
		return FText::FromString(Pass->GetMatType());
	}
	return FText();
}

void FArmySingleDoorDetail::OnSlectedSingleDoorMatTypeChanged(const int32 Key, const FString& Value)
{
	TSharedPtr<FArmySingleDoor> Pass = StaticCastSharedPtr<FArmySingleDoor>(SelectedObject.Get().Pin());
	if (Pass.IsValid() && Pass->GetType() == OT_Door)
	{
		Pass->SetMatType(Value);	
		if (LayoutDetail.IsValid())
		{
			LayoutDetail->ShowSelectedDetial(Pass);
		}
	}
}
