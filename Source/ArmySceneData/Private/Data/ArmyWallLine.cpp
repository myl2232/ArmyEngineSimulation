#include "ArmyWallLine.h"
#include "ArmyLine.h"
#include "ArmyEngineModule.h"
#include "ArmySceneData.h"
#include "ArmyRoom.h"

FArmyWallLine::FArmyWallLine()
{
	ObjectType = OT_WallLine;
	SetPropertyFlag(FLAG_COLLISION, true);
    SetName(TEXT("墙线"));
	CoreLine = MakeShareable(new FArmyLine);

    SetColor(FArmySceneData::Get()->GetWallLinesColor());
}

FArmyWallLine::FArmyWallLine(const TSharedPtr<FArmyEditPoint> InStart, const TSharedPtr<FArmyEditPoint> InEnd)
{
	ObjectType = OT_WallLine;
	SetPropertyFlag(FLAG_COLLISION, true);
	SetName(TEXT("墙线"));
	CoreLine = MakeShareable(new FArmyLine);
	CoreLine->SetStartPointer(InStart);
	CoreLine->SetEndPointer(InEnd);

    SetColor(FArmySceneData::Get()->GetWallLinesColor());
}

FArmyWallLine::FArmyWallLine(const FVector& InStart, const FVector& InEnd)
{
	ObjectType = OT_WallLine;
	SetPropertyFlag(FLAG_COLLISION, true);
	SetName(TEXT("墙线"));
	CoreLine = MakeShareable(new FArmyLine);
	CoreLine->SetStart(InStart);
	CoreLine->SetEnd(InEnd);

    SetColor(FArmySceneData::Get()->GetWallLinesColor());
}

FArmyWallLine::FArmyWallLine(FArmyWallLine* Copy)
{
	ObjectType = Copy->GetType();
	SetPropertyFlag(Copy->GetPropertyFlag());
	SetName(Copy->GetName());
	CoreLine = MakeShareable(new FArmyLine(Copy->GetCoreLine().Get()));

    SetColor(FArmySceneData::Get()->GetWallLinesColor());
}

FArmyWallLine::FArmyWallLine(class FArmyLine* Copy)
{
	ObjectType = OT_WallLine;
	SetPropertyFlag(FLAG_COLLISION, true);
	SetName(TEXT("墙线"));
	CoreLine = MakeShareable(new FArmyLine(Copy));

    SetColor(FArmySceneData::Get()->GetWallLinesColor());
}

void FArmyWallLine::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FLAG_VISIBILITY))
	{
		CoreLine->Draw(PDI, View);
	}
}

void FArmyWallLine::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
}

void FArmyWallLine::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);
}

void FArmyWallLine::SetColor(const FLinearColor& InColor)
{
	CoreLine->SetBaseColor(InColor);
    CoreLine->SetNormalColor(InColor);
}

void FArmyWallLine::SetWidth(const float InWidth)
{
	CoreLine->SetLineWidth(InWidth);
}

TSharedPtr<FArmyLine> FArmyWallLine::GetCoreLine() const
{
	return CoreLine;
}

void FArmyWallLine::GetAppendObjects(TArray<FObjectWeakPtr> & InObjects)
{
	InObjects = AppendObjects;
}

void FArmyWallLine::PushAppendObject(FObjectWeakPtr InObject)
{
	AppendObjects.AddUnique(InObject);
}

void FArmyWallLine::RemoveAppendObject(FObjectWeakPtr InObject)
{
	AppendObjects.Remove(InObject);
}

const FVector& FArmyWallLine::GetNormal() const
{
	//由于空间闭合后，可能还会拖拽墙线引起法线改变，所以要获取时计算
	TArray<TWeakPtr<FArmyObject>> CurrentRoomList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, CurrentRoomList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_OutRoom, CurrentRoomList);
	for (auto It : CurrentRoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (Room.IsValid())
		{
			TArray<TSharedPtr<class FArmyWallLine>> WallLines = Room->GetWallLines();
			for (auto ItLine : WallLines)
			{
				if (ItLine == this->AsShared())
				{
					return Room->GetWallLineNormal(ItLine);
				}
			}
		}
	}

	return FVector::ZeroVector;
}

FVector FArmyWallLine::GetDirection() const
{
	return CoreLine->GetDirectionNormal();
}

void FArmyWallLine::SetRelateRoom(TSharedPtr<class FArmyRoom> InRoom)
{
	RelatedRoom = InRoom;
}

TWeakPtr<class FArmyRoom> FArmyWallLine::GetRelatedRoom() const
{
	return RelatedRoom;
}
