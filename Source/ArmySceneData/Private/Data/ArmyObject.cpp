#include "ArmyObject.h" 
#include "ArmyActor.h"
#include "Math/RotationMatrix.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyEngineModule.h"
#include "ArmyTransactionModule.h"
#include "ArmySceneData.h"

TMap<FString, AActor*> FArmyObject::DynamicActorMap = TMap<FString, AActor*>();

GetObjectByID FArmyObject::OnGetObjectByID = GetObjectByID();

//bool FArmyObject::ConstructioMode = false;
uint32 FArmyObject::ObjectDrawMode = MODE_NORAMAL;

FScaleCanvasText::FScaleCanvasText()
	:WorldPosition(FVector(0, 0, 0)), FCanvasTextItem(FVector2D(0, 0), FText(), FArmyEngineModule::Get().GetEngineResource()->GetDefaultFont(), FLinearColor(1, 1, 1))
{
}
void FScaleCanvasText::DrawHUD(UArmyEditorViewportClient* InViewPortClient, const FSceneView* View, FCanvas* InCanvas)
{
	FVector2D PixPos;
	InViewPortClient->WorldToPixel(WorldPosition, PixPos);

	float UniformScale = View->WorldToScreen(WorldPosition).W * (4.0f / View->ViewRect.Width() / View->ViewMatrices.GetProjectionMatrix().M[0][0]);
	float TempScale = UniformScale / 2.0f;

	int32 FontPreSize = FArmyEngineModule::Get().GetEngineResource()->GetDefaultFont()->LegacyFontSize;
	FArmyEngineModule::Get().GetEngineResource()->GetDefaultFont()->LegacyFontSize = TextSize / TempScale * BaseScale.X;
	//float FontScale = float(TextSize) / float(Font->GetLegacySlateFontInfo().Size);

	//this->Scale = FVector2D(1 / TempScale, 1 / TempScale) * BaseScale;

	FRotationMatrix RotMatrix(Rotation * -1);
	FMatrix FinalTransform = RotMatrix * FTranslationMatrix(FVector(PixPos, 0));

	InCanvas->PushAbsoluteTransform(FinalTransform);
	FCanvasTextItem::Draw(InCanvas);
	InCanvas->PopTransform();
	FArmyEngineModule::Get().GetEngineResource()->GetDefaultFont()->LegacyFontSize = FontPreSize;
}

void FArmyObject::SetPropertyFlag(PropertyType InType, bool InUse)
{
	if (InUse)
	{
		PropertyFlag |= InType;
	}
	else
	{
		PropertyFlag &= ~InType;
	}
}
bool FArmyObject::GetPropertyFlag(PropertyType InType)
{
	return (PropertyFlag & InType) > 0;
}
const FString& FArmyObject::GetName() const
{
	return ObjectName;
}
void FArmyObject::SetDrawModel(EObjectDrawMode InType, bool InUse)
{
	if (InUse)
	{
		ObjectDrawMode |= InType;
	}
	else
	{
		ObjectDrawMode &= ~InType;
	}
}
bool FArmyObject::GetDrawModel(EObjectDrawMode InType)
{
	return (ObjectDrawMode & InType) > 0;
}
void FArmyObject::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	JsonWriter->WriteValue("objectId", GetUniqueID().ToString());
	JsonWriter->WriteValue("objectName", GetName());
	JsonWriter->WriteValue("propertyFlag", int32(GetPropertyFlag()));
	/**@��¼�˶����ǲ��ǿ�������*/
	JsonWriter->WriteValue("bIsCopyData", bIsCopyData);

	JsonWriter->WriteArrayStart("ParentList");
	for (auto P : ParentList)
	{	
		if (P.IsValid())
		{
			JsonWriter->WriteValue(P.Pin()->GetUniqueID().ToString());
		}
	}
	JsonWriter->WriteArrayEnd();
	JsonWriter->WriteArrayStart("Children");
	for (auto C : Children)
	{
		JsonWriter->WriteValue(C->GetUniqueID().ToString());
	}
	JsonWriter->WriteArrayEnd();

	if (ConstructionData.IsValid() && ConstructionData->bHasSetted)
	{
		ConstructionData->SerializeToJson(JsonWriter);
	}
}
void FArmyObject::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FGuid::Parse(InJsonData->GetStringField("objectId"),ObjID);
	InJsonData->TryGetStringField("objectName", ObjectName);
	InJsonData->TryGetNumberField("propertyFlag", PropertyFlag);
	InJsonData->TryGetBoolField("bIsCopyData", bIsCopyData);

	const TArray< TSharedPtr<FJsonValue> >& JsonParents = InJsonData->GetArrayField("ParentList");
	for (auto JsonV : JsonParents)
	{
		FGuid TempID;
		FGuid::Parse(JsonV->AsString(), TempID);
		FObjectWeakPtr Obj = OnGetObjectByID.Execute(TempID);
		if (Obj.IsValid())
		{
			Obj.Pin()->AddChild(this->AsShared());
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const TArray< TSharedPtr<FJsonValue> >& JsonChildren = InJsonData->GetArrayField("Children");
	for (auto JsonV : JsonChildren)
	{
		FGuid TempID;
		FGuid::Parse(JsonV->AsString(), TempID);
		FObjectWeakPtr Obj = OnGetObjectByID.Execute(TempID);
		if (Obj.IsValid())
		{
			AddChild(Obj.Pin());
		}
	}

	//@������ Ǩ��ʩ����洢λ�� �汾1.5 ���ȴ������汾��ɾ��
	//ConstructionData->Deserialization(InJsonData);

}

void FArmyObject::Create()
{
    SaveTransaction(TT_Create);

	//PushHomeModeModifyData(TT_Create);
}

void FArmyObject::Delete()
{
    SaveTransaction(TT_Delete);

	PushHomeModeModifyData(TT_Delete);
}

void FArmyObject::Modify()
{
	return;

    SaveTransaction(TT_Modify);

	//PushHomeModeModifyData(TT_Modify);
}

//void FArmyObject::SaveTransaction(ETransType TransType)
//{
//    if (FArmyTransactionModule::Get().Undo.IsValid())
//    {
//        FArmyTransactionModule::Get().Undo->SaveXRObject(TransType, this->AsShared(), RecordBeforeModify, Record);
//    }
//}
//
//void FArmyObject::PushHomeModeModifyData(ETransType TransType)
//{
//	if(ObjectType == OT_InternalRoom
//		|| ObjectType == OT_OutRoom
//		|| ObjectType == OT_Door
//		|| ObjectType == OT_SecurityDoor
//		|| ObjectType == OT_SlidingDoor
//		|| ObjectType == OT_Pass
//		|| ObjectType == OT_DoorHole
//		|| ObjectType == OT_Window
//		|| ObjectType == OT_FloorWindow
//		|| ObjectType == OT_RectBayWindow
//		|| ObjectType == OT_TrapeBayWindow
//		|| ObjectType == OT_CornerBayWindow
//		|| ObjectType == OT_Punch
//		|| ObjectType == OT_WallLine
//		|| ObjectType == OT_Beam
//		|| ObjectType == OT_Pillar
//		|| ObjectType == OT_AirFlue)
//		FArmySceneData::Get()->PushModifyDataMap(this->AsShared(), TransType);
//}

FObjectPtr FArmyObject::CopySelf(bool bCopyGUID)
{
	FObjectPtr CopyObj = nullptr;
	FString ObjectJStr;
	TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > ObjJsonWriter 
		= TJsonWriterFactory< TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&ObjectJStr);
	ObjJsonWriter->WriteObjectStart();
	SerializeToJson(ObjJsonWriter);
	ObjJsonWriter->WriteObjectEnd();
	ObjJsonWriter->Close();

	TSharedPtr<FJsonObject> JsonObject;
	if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(ObjectJStr), JsonObject))
	{
		FString ClassName = JsonObject->GetStringField(TEXT("RegisterClass"));
        // ����RegisterClass���е�Class������������߼�
        if (!ClassName.IsEmpty())
        {
            CopyObj = ClassFactory::GetInstance(ClassName);
            FGuid ObjID = CopyObj->GetUniqueID();
            CopyObj->bIsCopyData = true;
            CopyObj->Deserialization(JsonObject);
            if (!bCopyGUID)
            {
                CopyObj->SetUniqueID(ObjID);
            }
        }
	}

	return CopyObj;
}

void FArmyObject::AddChild(FObjectPtr InObj)
{
	if (!Children.Contains(InObj))
	{
		Children.AddUnique(InObj);
		InObj->AttachToParent(AsShared());
	}
}
void FArmyObject::RemoveChild(FObjectPtr InObj)
{
	if (InObj.IsValid() && Children.Contains(InObj))
	{
		Children.Remove(InObj);
		//InObj->RemoveFromParents();
	}
}
void FArmyObject::ClearChildren()
{
	for (auto C : Children)
	{
		C->RemoveFromParents(AsShared());
	}
	Children.Empty();
}
const TArray<FObjectPtr>& FArmyObject::GetChildren() const
{
	return Children;
}
void FArmyObject::AttachToParent(FObjectPtr InParent)
{
	if (InParent.IsValid())
	{
		InParent->AddChild(AsShared());
		ParentList.AddUnique(InParent);
	}
}
void FArmyObject::RemoveFromParents(FObjectPtr InParent)
{
	if (InParent.IsValid())
	{
		InParent->RemoveChild(AsShared());
		ParentList.Remove(InParent.ToSharedRef());
	}
	else
	{
		for (auto P : ParentList)
		{
			if (P.IsValid())
			{
				P.Pin()->RemoveChild(AsShared());
			}
		}
		ParentList.Empty();
	}
}

//�ǲ���ˮ��ģʽ�Ĺ���

 bool FArmyObject::IsPipeLine()
 { 
	 static TArray<EObjectType> Types = {
		 OT_Drain_Point,//��ˮ���ܵ�
		 OT_Gas_MainPipe,//ȼ������
		 OT_StrongElectricity_25,            //ǿ��2.5
		 OT_StrongElectricity_4,            //ǿ��4
		 OT_StrongElectricity_Single,            //ǿ�絥��
		 OT_StrongElectricity_Double,            //ǿ��˫��
		 OT_WeakElectricity_TV,//���� ������
		 OT_WeakElectricity_Net,//���磬 ����
		 OT_WeakElectricity_Phone,//���磬�绰��
		 OT_ColdWaterTube,//��ˮ��
		 OT_HotWaterTube,//��ˮ��
		 OT_Drain,//��ˮ��
		 OT_StrongElectricity_6,
		 OT_LightElectricity, //������·
	 };

	 return Types.Contains(GetType());
 }
 bool FArmyObject::IsPipeLineLinker()
 {
	 static TArray<EObjectType> Types = {
		 OT_Drain_Linker,
		 OT_StrongElectricity_25_Linker,            //ǿ��2.5
		 OT_StrongElectricity_4_Linker,            //ǿ��4
		 OT_StrongElectricity_Single_Linker,            //ǿ�絥��
		 OT_StrongElectricity_Double_Linker,            //ǿ��˫��
		 OT_WeakElectricity_TV_Linker,//���� ������
		 OT_WeakElectricity_Net_Linker,//���磬 ����
		 OT_WeakElectricity_Phone_Linker,//���磬�绰��
		 OT_ColdWaterTube_Linker,//��ˮ��
		 OT_HotWaterTube_Linker,//��ˮ��
		 OT_Drain_Linker,//��ˮ��

		OT_WaterLinker,   //ˮ��������
		 OT_ElectricLinker,   //����������
		 OT_StrongElectricity_6_Linker,  //@������          //ǿ��6
		 OT_LightElectricity_Linker,  //@������          ///������·
	 };

	 return Types.Contains(GetType());
 }
 

const TArray<TWeakPtr<FArmyObject>>& FArmyObject::GetParents() const
{
	return ParentList;
}
void FArmyObject::SetRelevanceActor(AActor* InActor)
{
	RelevanceActor = InActor;
}

/**	��ȡ���ж���*/

void FArmyObject::GetVertexes(TArray<FVector>& OutVertexes)
{
	TArray< TSharedPtr<FArmyLine> > Lines;
	GetLines(Lines);
	
	FVector Point;
	for (auto Line : Lines)
	{
		OutVertexes.AddUnique(Line->GetStart());
		OutVertexes.AddUnique(Line->GetEnd());
	}
}

EObjectType FArmyObject::GetPipeType(EObjectType PipeLinkerType)
{
	static TArray<EObjectType> LinkerTypes = {

		OT_StrongElectricity_25_Linker,            //ǿ��2.5
		OT_StrongElectricity_4_Linker,            //ǿ��4
		OT_StrongElectricity_Single_Linker,            //ǿ�絥��
		OT_StrongElectricity_Double_Linker,            //ǿ��˫��
		OT_WeakElectricity_TV_Linker,//���� ������
		OT_WeakElectricity_Net_Linker,//���磬 ����
		OT_WeakElectricity_Phone_Linker,//���磬�绰��
		OT_ColdWaterTube_Linker,//��ˮ��
		OT_HotWaterTube_Linker,//��ˮ��
		OT_Drain_Linker,
		//OT_WaterLinker,   //ˮ��������
		//OT_ElectricLinker,   //����������
		OT_StrongElectricity_6_Linker,  //@������          //ǿ��6
		OT_LightElectricity_Linker,  //@������          ///������·
	};

	//�������LinkerTypesһһ��Ӧ
	static TArray<EObjectType> PipesTypes = {
		//OT_Drain_Point,//��ˮ���ܵ�
		OT_StrongElectricity_25,            //ǿ��2.5
		OT_StrongElectricity_4,            //ǿ��4
		OT_StrongElectricity_Single,            //ǿ�絥��
		OT_StrongElectricity_Double,            //ǿ��˫��
		OT_WeakElectricity_TV,//���� ������
		OT_WeakElectricity_Net,//���磬 ����
		OT_WeakElectricity_Phone,//���磬�绰��
		OT_ColdWaterTube,//��ˮ��
		OT_HotWaterTube,//��ˮ��
		OT_Drain,//��ˮ��
		OT_StrongElectricity_6,
		OT_LightElectricity, //������·

		//OT_Gas_MainPipe,//ȼ������
	};
	int32 index;
	LinkerTypes.Find(PipeLinkerType, index);

	if (index == INDEX_NONE)
	{
		return EObjectType();
	}

	return PipesTypes[index];

}

EObjectType FArmyObject::GetPipeLinkerType(EObjectType PipeType)
{
	static TArray<EObjectType> LinkerTypes = {

		OT_StrongElectricity_25_Linker,            //ǿ��2.5
		OT_StrongElectricity_4_Linker,            //ǿ��4
		OT_StrongElectricity_Single_Linker,            //ǿ�絥��
		OT_StrongElectricity_Double_Linker,            //ǿ��˫��
		OT_WeakElectricity_TV_Linker,//���� ������
		OT_WeakElectricity_Net_Linker,//���磬 ����
		OT_WeakElectricity_Phone_Linker,//���磬�绰��
		OT_ColdWaterTube_Linker,//��ˮ��
		OT_HotWaterTube_Linker,//��ˮ��
		OT_Drain_Linker,
		//OT_WaterLinker,   //ˮ��������
		//OT_ElectricLinker,   //����������
		OT_StrongElectricity_6_Linker,  //@������          //ǿ��6
		OT_LightElectricity_Linker,  //@������          ///������·
	};

	//�������LinkerTypesһһ��Ӧ
	static TArray<EObjectType> PipesTypes = {
		//OT_Drain_Point,//��ˮ���ܵ�
		OT_StrongElectricity_25,            //ǿ��2.5
		OT_StrongElectricity_4,            //ǿ��4
		OT_StrongElectricity_Single,            //ǿ�絥��
		OT_StrongElectricity_Double,            //ǿ��˫��
		OT_WeakElectricity_TV,//���� ������
		OT_WeakElectricity_Net,//���磬 ����
		OT_WeakElectricity_Phone,//���磬�绰��
		OT_ColdWaterTube,//��ˮ��
		OT_HotWaterTube,//��ˮ��
		OT_Drain,//��ˮ��
		OT_StrongElectricity_6,
		OT_LightElectricity, //������·

							 //OT_Gas_MainPipe,//ȼ������
	};
	int32 index;
	PipesTypes.Find(PipeType, index);

	if (index == INDEX_NONE)
	{
		if (PipeType == OT_Drain_Point)
		{
			return OT_Drain_Linker;
		}

		return EObjectType();
	}

	return LinkerTypes[index];
}
