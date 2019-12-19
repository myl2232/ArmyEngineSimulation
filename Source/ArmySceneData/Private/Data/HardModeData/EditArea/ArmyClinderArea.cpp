#include "ArmyClinderArea.h"
#include "ArmyResourceModule.h"
#include "ResManager.h"
#include "ArmyWallActor.h"
#include "ArmyEngineModule.h"
#include "ArmySeamlessStyle.h"
#include "ArmyBaseEditStyle.h"
#include "ArmyExtrusionActor.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyExtrusionActor.h"
#include "ArmyActorConstant.h"
#include "ArmyWallActor.h"
#include "ArmyViewportClient.h"

FArmyClinderArea::FArmyClinderArea()
{
	UMaterial* LineMaterial = FArmyEngineModule::Get().GetEngineResource()->GetDefaultFloorTextureMat();
	MI_FloorTextureMat = UMaterialInstanceDynamic::Create(LineMaterial, NULL);
	MI_FloorTextureMat->AddToRoot();
	MI_FloorTextureMat->SetTextureParameterValue("MainTex", FArmyEngineModule::Get().GetEngineResource()->GetDefaultWhiteTexture());
	MatStyle = MakeShareable(new FArmySeamlessStyle());

	ObjectType = OT_ClinderArea;

}

FArmyClinderArea::FArmyClinderArea(FVector InPos, float InRadius, float InHeight)
{
	UMaterial* LineMaterial = FArmyEngineModule::Get().GetEngineResource()->GetDefaultFloorTextureMat();
	MI_FloorTextureMat = UMaterialInstanceDynamic::Create(LineMaterial, NULL);
	MI_FloorTextureMat->AddToRoot();
	SetClinderPositionInfo(InPos, InRadius, InHeight);
	ObjectType = OT_ClinderArea;
}

FArmyClinderArea::~FArmyClinderArea()
{
	if (!ExtrudedxfFilePath.IsEmpty())
	{
		if (ExturesionActor->IsValidLowLevel())
		{
			ExturesionActor->Destroy();
			ExturesionActor = NULL;
		}
	}
}



void FArmyClinderArea::SetClinderPositionInfo(const FVector& InCenter, const float InRadius, const float InHeight)
{
	Position = InCenter;
	Radius = InRadius;
	M_ClinderHeight = InHeight;

	RefreshPolyVertices();
}
void FArmyClinderArea::GetClinderInfo(FVector& OutCenter, float& OutRadius, float& OutHeight)
{
	OutCenter = Position;
	OutRadius = Radius;
	OutHeight = M_ClinderHeight;
}
void FArmyClinderArea::Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
}

void FArmyClinderArea::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	JsonWriter->WriteValue("Position", Position.ToString());
	JsonWriter->WriteValue("Radius", Radius);
	JsonWriter->WriteValue("ClinderHeight", M_ClinderHeight);
	JsonWriter->WriteValue("planeNormal", GetPlaneNormal().ToString());
	JsonWriter->WriteValue("planeOrignPos", GetPlaneCenter().ToString());
	JsonWriter->WriteValue("planeXDir", GetXDir().ToString());
	JsonWriter->WriteValue("planeYDir", GetYDir().ToString());
	JsonWriter->WriteValue("mainTextureUrl", TextureUrl);

}

void FArmyClinderArea::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);
	Position.InitFromString(InJsonData->GetStringField("Position"));
	Radius = InJsonData->GetNumberField("Radius");
	M_ClinderHeight = InJsonData->GetNumberField("ClinderHeight");
	TextureUrl = InJsonData->GetStringField("mainTextureUrl");
	PlaneNormal.InitFromString(InJsonData->GetStringField("planeNormal"));
	PlaneOrignPos.InitFromString(InJsonData->GetStringField("planeOrignPos"));
	PlaneXDir.InitFromString(InJsonData->GetStringField("planeXDir"));
	PlaneYDir.InitFromString(InJsonData->GetStringField("planeYDir"));
	RefreshPolyVertices();

	/** @梁晓菲 商品 .pak */
	if (TextureUrl.EndsWith(TEXT(".pak")))
	{
		UXRResourceManager * RM = FArmyResourceModule::Get().GetResourceManager();
		UMaterialInterface * MI = RM->CreateCustomMaterial(TextureUrl);
		MI_FloorTextureMat = dynamic_cast<UMaterialInstanceDynamic*>(MI);
		if (!MI_FloorTextureMat)
		{
			FVector planeNormal = PlaneXDir ^PlaneYDir;
			if (planeNormal.Equals(FVector(0, 0, 1)))
			{
				MI_FloorTextureMat = FArmyEngineModule::Get().GetEngineResource()->GetFloorMaterial();
				MI_FloorTextureMat->AddToRoot();
			}
			else
			{
				MI_FloorTextureMat = FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial();
				MI_FloorTextureMat->AddToRoot();
			}
		}
	}
	/** @梁晓菲 默认材质 .uasset 根据默认材质的路径反序列化*/
	else
	{
		MI_FloorTextureMat->SetTextureParameterValue("MainTex", Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *TextureUrl, NULL, LOAD_None, NULL)));
		if (MI_FloorTextureMat == NULL)
		{
			MI_FloorTextureMat = FArmyEngineModule::Get().GetEngineResource()->GetMaterialByURL(TextureUrl);
			MI_FloorTextureMat->AddToRoot();
		}
	}
}

bool FArmyClinderArea::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* ViwePortClient, float& dist)
{
	FVector MWorldStart, MWorldDir;
	FVector WordCenter = Position.X * PlaneXDir + Position.Y * PlaneYDir + PlaneOrignPos;
	ViwePortClient->DeprojectFVector2D(FVector2D(Pos.X, Pos.Y), MWorldStart, MWorldDir);
	MWorldDir = MWorldDir.GetSafeNormal();

	float a = (MWorldDir.X * MWorldDir.X) + (MWorldDir.Y* MWorldDir.Y);
	float b = 2 * (MWorldDir.X * (MWorldStart.X - WordCenter.X) + (MWorldStart.Y - WordCenter.Y) * MWorldDir.Y);
	float c = (MWorldStart.X - WordCenter.X)*(MWorldStart.X - WordCenter.X) + (MWorldStart.Y - WordCenter.Y) * (MWorldStart.Y - WordCenter.Y) - Radius* Radius;

	float delta = b* b - 4 * a * c;
	if (delta > 0)
	{
		float t0 = (-b + FMath::Sqrt(delta)) / (2 * a);
		float t1 = (-b - FMath::Sqrt(delta)) / (2 * a);
		FVector intersection0 = MWorldStart + t0 * MWorldDir;
		float minHeight = PlaneNormal.Equals(FVector(0, 0, -1)) ? (WordCenter + M_ClinderHeight * PlaneNormal).Z : WordCenter.Z;
		float maxHeight = PlaneNormal.Equals(FVector(0, 0, -1)) ? WordCenter.Z : (WordCenter + M_ClinderHeight * PlaneNormal).Z;
		if (intersection0.Z > minHeight && intersection0.Z < maxHeight)
		{
			dist = t0;
			return true;
		}
		else
		{
			return false;
		}

	}
	else
		return false;
}

void FArmyClinderArea::SetStyle(TSharedPtr<class FArmyBaseEditStyle> InStyle)
{
	MatStyle = InStyle;
	TextureUrl = InStyle->GetMainTextureUrl();
	MI_FloorTextureMat = InStyle->GetMaterial();
	if (WallActor)
		WallActor->SetMaterial(MI_FloorTextureMat);
}

void FArmyClinderArea::GenerateExtrudeActor(UWorld* InWorld, TSharedPtr<FContentItemSpace::FContentItem> ContentItem)
{
	if (!ContentItem.IsValid())
		return;
	TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ContentItem->GetResObjNoComponent();
	if (resArr.Num() < 1)
		return;
	ContentItemList.Empty();
	ContentItemList.Add(ContentItem);
	FString InPath = resArr[0]->FilePath;

	for (FVector& iter : TempTotalVertices)
	{
		iter = iter.X * PlaneXDir + iter.Y * PlaneYDir + PlaneOrignPos + PlaneNormal * M_ClinderHeight;
	}
	if (FArmyMath::IsClockWise(TempTotalVertices))
		FArmyMath::ReversePointList(TempTotalVertices);
	if (ExturesionActor == NULL)
	{
		ExturesionActor = InWorld->SpawnActor<AArmyExtrusionActor>(AArmyExtrusionActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
		ExturesionActor->bIsSelectable = false;
		ExturesionActor->Tags.Add(XRActorTag::Immovable);
		ExturesionActor->Tags.Add(XRActorTag::HelpArea);
		ExturesionActor->AttachSurfaceArea = StaticCastSharedRef<FArmyBaseArea>(this->AsShared());
	}
	ExturesionActor->GenerateMeshFromFilePath(InPath, TempTotalVertices, true);

	ExturesionActor->dxfName = ContentItem->Name;
	ExturesionActor->SetActorLabel(ContentItem->Name);
	ExturesionActor->dxfLength = ContentItem->ProObj->Length;
	ExturesionActor->dxfWidth = ContentItem->ProObj->Width;
	ExturesionActor->dxfHeight = ContentItem->ProObj->Height;
	UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
	UMaterialInterface* MI = ResMgr->CreateContentItemMaterial(ContentItem);
	UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(MI);
	ExturesionActor->SetMaterial(MID);

	ExtrudedxfFilePath = InPath;
}

void FArmyClinderArea::CaculateClinderWallActor()
{
	if (WallActor == NULL)
	{
		WallActor = GVC->GetWorld()->SpawnActor<AXRWallActor>(AXRWallActor::StaticClass(), FTransform::Identity);
		WallActor->MeshComponent->bBuildStaticLighting = true;
		WallActor->AttachSurface = StaticCastSharedRef<FArmyBaseArea>(this->AsShared());
		WallActor->Tags.Add(XRActorTag::Immovable);
		WallActor->Tags.Add(XRActorTag::HelpArea);
		WallActor->Tags.Add(TEXT("CanNotDelete"));

        if (SurfaceType == 0)//地
        {
            WallActor->SetActorLabel(FArmyActorLabel::MakeFloorLabel());
            WallActor->SetFolderPath(FArmyActorPath::GetFloorPath());
        }
        else if (SurfaceType == 1)//墙
        {
            WallActor->SetActorLabel(FArmyActorLabel::MakeWallLabel());
            WallActor->SetFolderPath(FArmyActorPath::GetWallPath());
        }
        else if (SurfaceType == 2)//顶
        {
            WallActor->SetActorLabel(FArmyActorLabel::MakeRoofLabel());
            WallActor->SetFolderPath(FArmyActorPath::GetRoofPath());
        }
	}
	WallActor->ResetMeshTriangles();
	WallActor->AddVerts(TotalVertices);
	WallActor->UpdateAllVetexBufferIndexBuffer();
	WallActor->SetMaterial(MI_FloorTextureMat);
}

void FArmyClinderArea::RefreshPolyVertices()
{
	TotalVertices.Empty();
	TempTotalVertices.Empty();

	const FVector& startPos = FVector(Position.X, Position.Y, 0.0f) + FVector(-1, 0, 0)*Radius;

	int number = 64;
	float deltaAngle = 360.0f / number;
	float startAngle = 0.0f;
	for (int i = 0; i < number; i++)
	{
		FVector tempPoint = FRotator(0, startAngle, 0).RotateVector(startPos - FVector(Position.X, Position.Y, 0.0f)) + FVector(Position.X, Position.Y, 0.0f);
		startAngle += deltaAngle;
		TempTotalVertices.Push(tempPoint);
	}
	float length = 0;
	bool ReversePoint = true;
	if (PlaneNormal.Equals(FVector(0, 0, 1)) && m_deltaHeight < 0)
		ReversePoint = false;
	if (PlaneNormal.Equals(FVector(0, 0, -1)) && m_deltaHeight < 0)
		ReversePoint = false;
	for (int i = 0; i < number; i++)
	{
		FVector startPos = TempTotalVertices[i%number];
		FVector endPos = TempTotalVertices[(i + 1) % number];
		startPos = startPos.X * PlaneXDir + startPos.Y *PlaneYDir + PlaneOrignPos;
		endPos = endPos.X *PlaneXDir + endPos.Y * PlaneYDir + PlaneOrignPos;
		FVector startHighPos = startPos + PlaneNormal * M_ClinderHeight;
		FVector endHighPos = endPos + PlaneNormal * M_ClinderHeight;
		FVector PlaneTangent = (endPos - startPos).GetSafeNormal();
		FVector ClinderNormal = PlaneTangent.RotateAngleAxis(90, PlaneNormal);
		float U = length / XRepeatDis;
		float U1 = (length + (endPos - startPos).Size()) / XRepeatDis;
		float V = M_ClinderHeight / YRepeatDis;
		if (ReversePoint)
		{
			TotalVertices.Push(FDynamicMeshVertex(endPos, PlaneTangent, ClinderNormal, FVector2D(U1, 0.0f), FColor::White));
			TotalVertices.Push(FDynamicMeshVertex(endHighPos, PlaneTangent, ClinderNormal, FVector2D(U1, V), FColor::White));
			TotalVertices.Push(FDynamicMeshVertex(startHighPos, PlaneTangent, ClinderNormal, FVector2D(U, V), FColor::White));
			TotalVertices.Push(FDynamicMeshVertex(endPos, PlaneTangent, ClinderNormal, FVector2D(U1, 0.0f), FColor::White));
			TotalVertices.Push(FDynamicMeshVertex(startHighPos, PlaneTangent, ClinderNormal, FVector2D(U, V), FColor::White));
			TotalVertices.Push(FDynamicMeshVertex(startPos, PlaneTangent, ClinderNormal, FVector2D(U, 0.0f), FColor::White));
		}
		else
		{
			TotalVertices.Push(FDynamicMeshVertex(startPos, PlaneTangent, ClinderNormal, FVector2D(U, 0.0f), FColor::White));
			TotalVertices.Push(FDynamicMeshVertex(startHighPos, PlaneTangent, ClinderNormal, FVector2D(U, V), FColor::White));
			TotalVertices.Push(FDynamicMeshVertex(endPos, PlaneTangent, ClinderNormal, FVector2D(U1, 0.0f), FColor::White));
			TotalVertices.Push(FDynamicMeshVertex(startHighPos, PlaneTangent, ClinderNormal, FVector2D(U, V), FColor::White));
			TotalVertices.Push(FDynamicMeshVertex(endHighPos, PlaneTangent, ClinderNormal, FVector2D(U1, V), FColor::White));
			TotalVertices.Push(FDynamicMeshVertex(endPos, PlaneTangent, ClinderNormal, FVector2D(U1, 0.0f), FColor::White));
		}

		length += (endPos - startPos).Size();
	}
	CaculateClinderWallActor();
}

