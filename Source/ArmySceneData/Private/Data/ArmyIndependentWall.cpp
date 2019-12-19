#include "ArmyIndependentWall.h"
#include "FArmyConstructionItemInterface.h"
#include "ArmySceneData.h"
#include "ArmyPolygon.h"
#include "ArmyMath.h"
#include "ArmyHardWare.h"

FArmyIndependentWall::FArmyIndependentWall()
{
	ObjectType = OT_IndependentWall;
	SetName(TEXT("独立墙"));
}

void FArmyIndependentWall::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (FArmyObject::GetDrawModel(MODE_CEILING) || FArmyObject::GetDrawModel(MODE_TOPVIEW))
	{
		TArray<FVector> outverts;
		GetVertexes(outverts);

		int number = outverts.Num();
		for (int i = 0; i < number; i++)
		{
			PDI->DrawLine(outverts[i%number] + FVector(0, 0, FArmySceneData::WallHeight + 21.0f), outverts[(i + 1) % number] + FVector(0, 0, FArmySceneData::WallHeight + 21.0f), FLinearColor::Black, 0);
		}
		Polygon->MaterialRenderProxy = FArmyEngineModule::Get().GetEngineResource()->GetFloorMaterial()->GetRenderProxy(false);
		Polygon->Draw(PDI, View);
	}
	else
	{
		FLinearColor CurrentColor;
		if (FArmySceneData::Get()->bIsDisplayDismantle)
		{
			Polygon->MaterialRenderProxy = MI_Line->GetRenderProxy(false);
		}
		else
		{
			Polygon->MaterialRenderProxy = nullptr;
		}
		//Polygon->MaterialRenderProxy = MI_Line->GetRenderProxy(false);
		FArmyAddWall::Draw(PDI, View);
	}
	FLinearColor CurrentColor;
	if (FArmySceneData::Get()->bIsDisplayDismantle)
	{
		Polygon->MaterialRenderProxy = MI_Line->GetRenderProxy(false);
		CurrentColor = FLinearColor::Green;
	}
	else
	{
		Polygon->MaterialRenderProxy = nullptr;
		CurrentColor = FLinearColor::White;
	}


	if (State != OS_Selected)
		for (auto line : PolyLines)
		{
			line->SetBaseColor(CurrentColor);
		}
}

void FArmyIndependentWall::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	JsonWriter->WriteArrayStart(TEXT("vertexes"));
	TArray<FVector>& Vertexes = Polygon->Vertices;
	for (auto& Vertex : Vertexes)
	{
		JsonWriter->WriteValue(Vertex.ToString());
	}
	JsonWriter->WriteArrayEnd();
	JsonWriter->WriteValue("MatType", GetMatType());
	JsonWriter->WriteValue("Height", GetHeight());
	JsonWriter->WriteValue("bOffsetGenerate", bOffsetGenerate);
	JsonWriter->WriteValue("bIsHalfWall", bIsHalfWall);
	JsonWriter->WriteValue("bInternalExtrusion", bInternalExtrusion);

	/**@欧石楠 存储施工项*/
	ConstructionItemData->SerializeToJson(JsonWriter);

	SERIALIZEREGISTERCLASS(JsonWriter, FArmyIndependentWall)
}

void FArmyIndependentWall::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyAddWall::Deserialization(InJsonData);
}



FVector FArmyIndependentWall::GetWallNormal(TSharedPtr<FArmyLine> InLine)
{
	for (auto It : PolyLines)
	{
		if (It == InLine)
		{
			FVector CurrentNormal = InLine->GetDirectionNormal() ^ FVector(0, 0, 1);
			FVector OffsetPoint = (InLine->GetStart() + InLine->GetEnd()) / 2
				+ CurrentNormal*0.5;
			if (FArmyMath::IsPointInPolygon2D(OffsetPoint, Polygon->Vertices))
				return -CurrentNormal;
			else
				return CurrentNormal;
		}
	}

	return FVector::ZeroVector;
}
