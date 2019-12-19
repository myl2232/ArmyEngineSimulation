#include "ArmyWHCDoorSheet.h"
#include "ArmyFurniture.h"


FArmyWHCDoorSheet::FArmyWHCDoorSheet()
{
	BoundingBox.Init();
	ObjectType = OT_DoorSheetFace;
}
void FArmyWHCDoorSheet::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
    if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
    {
		PDI->DrawLines(DrawArray,SDPG_Foreground);
    }
}
void FArmyWHCDoorSheet::SetDataAddBlocks(TSharedPtr<FArmyFurniture> InObj,const TArray<TSharedPtr<FDxfSpliteBlock>>& InSplitBlockArray)
{
	InObj->SetTransform(FTransform());
	BaseArray = InObj->GetElementVertex();
	BlockArray = InSplitBlockArray;
	Init();
	UpdateRelationship();
	Update();
}
void FArmyWHCDoorSheet::SetCurrentSize(float InXlength, float InYLength)
{
	CurrentXLenth = InXlength;
	CurrentYLenth = InYLength;
}
void FArmyWHCDoorSheet::Init()
{
	for (int32 i = 0; i < BaseArray.Num(); i++)
	{
		FVector2D P(BaseArray[i].Position.X, BaseArray[i].Position.Y);
		bool IsUsed = false;
		for (auto Block : BlockArray)
		{
			if ((Block->BlockBoxBase.IsInside(P) || Block->BlockBoxBase.GetClosestPointTo(P) == P) && !IsUsed)
			{
				Block->ContainIndexArray.AddUnique(i);
				IsUsed = true;
			}
		}
	}
}
void FArmyWHCDoorSheet::Update()
{
	if (BlockRelationshipArray.Num() > 0)
	{
		//把所有最左侧的块排在最前边，因为X的关联关系是从左到右（X从小到大）排序的
		BlockRelationshipArray.Sort([&](const FBlockRelationship& A, const FBlockRelationship& B) {
			return A.MainBlock->BlockBoxBase.Min.X < B.MainBlock->BlockBoxBase.Min.X;
		});

		for (auto& Rs : BlockRelationshipArray)
		{
			if (!Rs.MainBlock->XChanged)
			{
				TArray<TSharedPtr<FDxfSpliteBlock>> XBlockArray;
				XBorderScaleFun(Rs, XBlockArray);
			}
		}

		//把所有最上边的块排在最前边，因为Y的关联关系是从上到下（Y从小到大）排序的
		BlockRelationshipArray.Sort([&](const FBlockRelationship& A, const FBlockRelationship& B) {
			return A.MainBlock->BlockBoxBase.Min.Y < B.MainBlock->BlockBoxBase.Min.Y;
		});

		for (auto& Rs : BlockRelationshipArray)
		{
			if (!Rs.MainBlock->YChanged)
			{
				TArray<TSharedPtr<FDxfSpliteBlock>> YBlockArray;
				YBorderScaleFun(Rs, YBlockArray);
			}
		}
		UpdateDrawVertex();
	}
}
void FArmyWHCDoorSheet::UpdateDrawVertex()
{
	DrawArray = BaseArray;
	for (auto Block : BlockArray)
	{
		for (uint32 Index : Block->ContainIndexArray)
		{
			FVector Pos = DrawArray[Index].Position;
			FVector2D PostPositon = Block->BlockBoxPost.Min + (FVector2D(Pos) - Block->BlockBoxBase.Min) * FVector2D(Block->XScale, Block->YScale);
			DrawArray[Index].Position = LocalTransform.TransformPosition(FVector(PostPositon,Pos.Z));
		} 
	}
}
void FArmyWHCDoorSheet::ApplyTransform(const FTransform& Trans)
{
	LocalTransform = Trans;
	Update();
}

void FArmyWHCDoorSheet::GetTransForm(FRotator& InRot, FVector& InTran, FVector& InScale) const
{
	InTran = LocalTransform.GetLocation();
	InRot = LocalTransform.Rotator();
	InScale = LocalTransform.GetScale3D();
}

const FBox FArmyWHCDoorSheet::GetBounds()
{
	FBox V(ForceInitToZero);
	V += LocalTransform.TransformPosition(BoundingBox.Min);
	V += LocalTransform.TransformPosition(BoundingBox.Max);
	V += LocalTransform.TransformPosition(FVector(BoundingBox.Min.X, BoundingBox.Max.Y, 0));
	V += LocalTransform.TransformPosition(FVector(BoundingBox.Max.X, BoundingBox.Min.Y, 0));
	return V;
}

const FBox FArmyWHCDoorSheet::GetPreBounds()
{
	return BoundingBox;
}

const FVector FArmyWHCDoorSheet::GetBasePos()
{
	return LocalTransform.GetTranslation();
}

void FArmyWHCDoorSheet::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	
	JsonWriter->WriteValue("LocalTransform", LocalTransform.ToString());
	JsonWriter->WriteValue("LocalTransform", LocalTransform.ToString());
	JsonWriter->WriteValue("CurrentXLenth", CurrentXLenth);
	JsonWriter->WriteValue("CurrentYLenth", CurrentYLenth);

	JsonWriter->WriteArrayStart("BaseArray");
	for (auto& V : BaseArray)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("Position", V.Position.ToString());
		JsonWriter->WriteValue("TextureCoordinate", V.TextureCoordinate.ToString());
		JsonWriter->WriteValue("Color", V.Color.ToString());
		JsonWriter->WriteValue("HitProxyIdColor", V.HitProxyIdColor.ToString());
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteArrayStart("BlockArray");
	for (auto& Block : BlockArray)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("BlockBoxBaseMinP", Block->BlockBoxBase.Min.ToString());
		JsonWriter->WriteValue("BlockBoxBaseMaxP", Block->BlockBoxBase.Max.ToString());//初始化的posbox 和basebox一样
		JsonWriter->WriteValue("BlockModel", Block->BlockModel);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	SERIALIZEREGISTERCLASS(JsonWriter, FArmyWHCDoorSheet)
}
void FArmyWHCDoorSheet::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);
	
	LocalTransform.InitFromString(InJsonData->GetStringField("LocalTransform"));
	CurrentXLenth = InJsonData->GetNumberField("CurrentXLenth");
	CurrentYLenth = InJsonData->GetNumberField("CurrentYLenth");

	BaseArray.Empty();
	const TArray< TSharedPtr<FJsonValue> >* OutBaseArrayArray = NULL;
	if (InJsonData->TryGetArrayField("BaseVertexArray", OutBaseArrayArray))
	{
		for (auto JsonV : *OutBaseArrayArray)
		{
			const TSharedPtr<FJsonObject> VObj = JsonV->AsObject();
			if (VObj.IsValid())
			{
				int32 Index = BaseArray.Add(FSimpleElementVertex());
				FSimpleElementVertex& V = BaseArray[Index];
				V.Position.InitFromString(VObj->GetStringField("Position"));
				V.TextureCoordinate.InitFromString(VObj->GetStringField("TextureCoordinate"));
				V.Color.InitFromString(VObj->GetStringField("Color"));
				V.HitProxyIdColor.InitFromString(VObj->GetStringField("HitProxyIdColor"));
			}
		}
	}

	BlockArray.Empty();
	const TArray< TSharedPtr<FJsonValue> >* OutArray = NULL;
	if (InJsonData->TryGetArrayField("BlockArray", OutArray))
	{
		for (auto JsonV : *OutArray)
		{
			const TSharedPtr<FJsonObject> VObj = JsonV->AsObject();
			if (VObj.IsValid())
			{
				TSharedPtr<FDxfSpliteBlock> Block = MakeShareable(new FDxfSpliteBlock);

				FVector2D MinP, MaxP;
				MinP.InitFromString(VObj->GetStringField("BlockBoxBaseMinP"));
				MaxP.InitFromString(VObj->GetStringField("BlockBoxBaseMaxP"));
				Block->BlockBoxBase = FBox2D(MinP,MaxP);
				Block->BlockBoxPost = FBox2D(MinP, MaxP);
				int32 BlockModel = 0;
				VObj->TryGetNumberField("BlockModel", BlockModel);
				Block->BlockModel = FDxfSpliteBlock::EBlockModel(BlockModel);
				BlockArray.Add(Block);
			}
		}
	}
	Update();
}

void FArmyWHCDoorSheet::UpdateRelationship()
{
	for (TSharedPtr<FDxfSpliteBlock> Block : BlockArray)
	{
		int32 Index = BlockRelationshipArray.AddUnique(FBlockRelationship(Block));
		FBlockRelationship& RelationShip = BlockRelationshipArray[Index];

		for (TSharedPtr<FDxfSpliteBlock> NextBlock : BlockArray)
		{
			if (Block != NextBlock)
			{
				if (FMath::IsNearlyEqual(Block->BlockBoxBase.Max.X,NextBlock->BlockBoxBase.Min.X, 0.001f) &&
					!(Block->BlockBoxBase.Max.Y <= NextBlock->BlockBoxBase.Min.Y ||
						Block->BlockBoxBase.Min.Y >= NextBlock->BlockBoxBase.Max.Y))
				{
					RelationShip.XFrontBorderArray.AddUnique(NextBlock);
				}

				if (FMath::IsNearlyEqual(Block->BlockBoxBase.Max.Y, NextBlock->BlockBoxBase.Min.Y, 0.001f) &&
					!(Block->BlockBoxBase.Max.X <= NextBlock->BlockBoxBase.Min.X ||
						Block->BlockBoxBase.Min.X >= NextBlock->BlockBoxBase.Max.X))
				{
					RelationShip.YFrontBorderArray.AddUnique(NextBlock);
				}
			}
		}
	}
}
void FArmyWHCDoorSheet::XBorderScaleFun(const FBlockRelationship& InCurrentBlock, TArray<TSharedPtr<FDxfSpliteBlock>>& OutXBlockArray)
{
	OutXBlockArray.AddUnique(InCurrentBlock.MainBlock);

	if (InCurrentBlock.XFrontBorderArray.Num() > 0)
	{
		for (auto XNextBlock : InCurrentBlock.XFrontBorderArray)
		{
			int32 RIndex = BlockRelationshipArray.Find(FBlockRelationship(XNextBlock));
			if (RIndex != INDEX_NONE)
			{
				XBorderScaleFun(BlockRelationshipArray[RIndex], OutXBlockArray);
			}
		}
		OutXBlockArray.Remove(InCurrentBlock.MainBlock);
	}
	else
	{
		float XScaleBaseLen = 0;
		float TempXlenth = CurrentXLenth;//test
		for (auto XBlock : OutXBlockArray)
		{
			if (XBlock->BlockModel == FDxfSpliteBlock::BM_XSCALE || XBlock->BlockModel == FDxfSpliteBlock::BM_XYSCALE)
			{
				XScaleBaseLen += XBlock->BlockBoxBase.GetSize().X;
			}
			else
			{
				TempXlenth -= XBlock->BlockBoxBase.GetSize().X;
			}
		}
		float XScale = TempXlenth / XScaleBaseLen;

		if (OutXBlockArray.Num() > 0)
		{
			float StartX = OutXBlockArray[0]->BlockBoxBase.Min.X;
			for (auto XBlock : OutXBlockArray)
			{
				if (XBlock->BlockModel == FDxfSpliteBlock::BM_XSCALE || XBlock->BlockModel == FDxfSpliteBlock::BM_XYSCALE)
				{
					XBlock->XScale = XScale;
					XBlock->BlockBoxPost = FBox2D(FVector2D(StartX, XBlock->BlockBoxPost.Min.Y), FVector2D(XBlock->BlockBoxBase.GetSize().X * XScale + StartX, XBlock->BlockBoxPost.Max.Y));
				}
				else
				{
					XBlock->BlockBoxPost = FBox2D(FVector2D(StartX, XBlock->BlockBoxPost.Min.Y), FVector2D(StartX + XBlock->BlockBoxBase.GetSize().X, XBlock->BlockBoxPost.Max.Y));
				}
				StartX = XBlock->BlockBoxPost.Max.X;
				XBlock->XChanged = true;
			}
		}
		OutXBlockArray.Remove(InCurrentBlock.MainBlock);
	}
};
void FArmyWHCDoorSheet::YBorderScaleFun(const FBlockRelationship& InCurrentBlock, TArray<TSharedPtr<FDxfSpliteBlock>>& OutYBlockArray)
{
	OutYBlockArray.AddUnique(InCurrentBlock.MainBlock);

	if (InCurrentBlock.YFrontBorderArray.Num() > 0)
	{
		for (auto YNextBlock : InCurrentBlock.YFrontBorderArray)
		{
			int32 RIndex = BlockRelationshipArray.Find(FBlockRelationship(YNextBlock));
			if (RIndex != INDEX_NONE)
			{
				YBorderScaleFun(BlockRelationshipArray[RIndex], OutYBlockArray);
			}
		}
		OutYBlockArray.Remove(InCurrentBlock.MainBlock);
	}
	else
	{
		float YScaleBaseLen = 0;
		float TempYlenth = CurrentYLenth;//test
		for (auto YBlock : OutYBlockArray)
		{
			if (YBlock->BlockModel == FDxfSpliteBlock::BM_YSCALE || YBlock->BlockModel == FDxfSpliteBlock::BM_XYSCALE)
			{
				YScaleBaseLen += YBlock->BlockBoxBase.GetSize().Y;
			}
			else
			{
				TempYlenth -= YBlock->BlockBoxBase.GetSize().Y;
			}
		}
		float YScale = TempYlenth / YScaleBaseLen;

		if (OutYBlockArray.Num() > 0)
		{
			float StartY = OutYBlockArray[0]->BlockBoxBase.Min.Y;
			for (auto YBlock : OutYBlockArray)
			{
				if (YBlock->BlockModel == FDxfSpliteBlock::BM_YSCALE || YBlock->BlockModel == FDxfSpliteBlock::BM_XYSCALE)
				{
					YBlock->YScale = YScale;
					YBlock->BlockBoxPost = FBox2D(FVector2D(YBlock->BlockBoxPost.Min.X, StartY), FVector2D(YBlock->BlockBoxPost.Max.X, YBlock->BlockBoxBase.GetSize().Y * YScale + StartY));
				}
				else
				{
					YBlock->BlockBoxPost = FBox2D(FVector2D(YBlock->BlockBoxPost.Min.X, StartY), FVector2D(YBlock->BlockBoxPost.Max.X, YBlock->BlockBoxBase.GetSize().Y + StartY));
				}
				StartY = YBlock->BlockBoxPost.Max.Y;
				YBlock->YChanged = true;
			}
		}
		OutYBlockArray.Remove(InCurrentBlock.MainBlock);
	}
};
