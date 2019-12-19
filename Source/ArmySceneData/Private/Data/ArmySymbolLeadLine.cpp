#include "ArmySymbolLeadLine.h"
#include "CoreMinimal.h"
#include "ArmyEngineResource.h"

FArmySymbolLeadLine::FArmySymbolLeadLine()
{

}

FArmySymbolLeadLine::~FArmySymbolLeadLine()
{

}

void FArmySymbolLeadLine::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);

	JsonWriter->WriteValue("BasePos", BasePos.ToString());
	JsonWriter->WriteValue("TargetPos", TargetPos.ToString());
	JsonWriter->WriteValue("BaseColor", BaseColor.ToString());
	JsonWriter->WriteValue("CurrentRoomBoundMin", m_BCurrentRoomBound.Min.ToString());
	JsonWriter->WriteValue("CurrentRoomBoundMax", m_BCurrentRoomBound.Max.ToString());
	JsonWriter->WriteValue("SymbolType", SymbolType);
	JsonWriter->WriteValue("EditorModel", EditorModel);

	SERIALIZEREGISTERCLASS(JsonWriter, FArmySymbolLeadLine)
}

void FArmySymbolLeadLine::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);

	BasePos.InitFromString(InJsonData->GetStringField("BasePos"));
	TargetPos.InitFromString(InJsonData->GetStringField("TargetPos"));
	BaseColor.InitFromString(InJsonData->GetStringField("BaseColor"));
	m_BCurrentRoomBound.Min.InitFromString(InJsonData->GetStringField("CurrentRoomBoundMin"));
	m_BCurrentRoomBound.Max.InitFromString(InJsonData->GetStringField("CurrentRoomBoundMax"));
	SymbolType = InJsonData->GetNumberField("SymbolType");
	EditorModel = InJsonData->GetBoolField("EditorModel");

	Update();
}
void FArmySymbolLeadLine::Refresh()
{

}
void FArmySymbolLeadLine::Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (LineVertexArray.Num() > 1)
	{
		for (int32 i = 0; i < LineVertexArray.Num() - 1; ++i)
		{
			DrawDashedLine(PDI, LineVertexArray[i], LineVertexArray[i + 1], BaseColor, 3, SDPG_Foreground);
		}
		if (SymbolType & EST_LEFT && SymbolType & EST_RIGHT)
		{
			DrawBox(PDI, FTransform(BasePos).ToMatrixWithScale(), FVector(SectionPointSize, SectionPointSize / 3, 1), UMaterial::GetDefaultMaterial(MD_Surface)->GetRenderProxy(false), SDPG_World);
		}
		else if (SymbolType & EST_LEFT)
		{
			DrawBox(PDI, FTransform(BasePos - FVector((SectionPointSize * 2) / 3 - SectionPointSize / 3, 0, 0)).ToMatrixWithScale(), FVector((SectionPointSize * 2) / 3, SectionPointSize / 3, 1), UMaterial::GetDefaultMaterial(MD_Surface)->GetRenderProxy(false), SDPG_World);
		}
		else if (SymbolType & EST_RIGHT)
		{
			DrawBox(PDI, FTransform(BasePos + FVector((SectionPointSize * 2) / 3 - SectionPointSize / 3, 0, 0)).ToMatrixWithScale(), FVector((SectionPointSize * 2 / 3), SectionPointSize / 3, 1), UMaterial::GetDefaultMaterial(MD_Surface)->GetRenderProxy(false), SDPG_World);
		}

		if (SymbolType & EST_TOP && SymbolType & EST_BOTTOM)
		{
			DrawBox(PDI, FTransform(BasePos).ToMatrixWithScale(), FVector(SectionPointSize / 3, SectionPointSize, 1), UMaterial::GetDefaultMaterial(MD_Surface)->GetRenderProxy(false), SDPG_World);
		}
		else if (SymbolType & EST_TOP)
		{
			DrawBox(PDI, FTransform(BasePos - FVector(0, (SectionPointSize * 2) / 3 - SectionPointSize / 3, 0)).ToMatrixWithScale(), FVector(SectionPointSize / 3, (SectionPointSize * 2) / 3, 1), UMaterial::GetDefaultMaterial(MD_Surface)->GetRenderProxy(false), SDPG_World);
		}
		else if (SymbolType & EST_BOTTOM)
		{
			DrawBox(PDI, FTransform(BasePos + FVector(0, (SectionPointSize * 2) / 3 - SectionPointSize / 3, 0)).ToMatrixWithScale(), FVector(SectionPointSize / 3, (SectionPointSize * 2) / 3, 1), UMaterial::GetDefaultMaterial(MD_Surface)->GetRenderProxy(false), SDPG_World);
		}
	}
	if (EditorModel)
	{
		DrawBox(PDI, FTransform(TargetPos - FVector(Offset + CheckBoxSize / 2, 0, 0)).ToMatrixWithScale(), FVector(CheckBoxSize, CheckBoxSize, 1), FArmyEngineModule::Get().GetEngineResource()->GetCheckBoxMat((SymbolType & EST_LEFT) > 0)->GetRenderProxy(false), SDPG_World);

		DrawBox(PDI, FTransform(TargetPos + FVector(Offset + CheckBoxSize / 2, 0, 0)).ToMatrixWithScale(), FVector(CheckBoxSize, CheckBoxSize, 1), FArmyEngineModule::Get().GetEngineResource()->GetCheckBoxMat((SymbolType & EST_RIGHT) > 0)->GetRenderProxy(false), SDPG_World);

		DrawBox(PDI, FTransform(TargetPos - FVector(0, Offset + CheckBoxSize / 2, 0)).ToMatrixWithScale(), FVector(CheckBoxSize, CheckBoxSize, 1), FArmyEngineModule::Get().GetEngineResource()->GetCheckBoxMat((SymbolType & EST_TOP) > 0)->GetRenderProxy(false), SDPG_World);

		DrawBox(PDI, FTransform(TargetPos + FVector(0, Offset + CheckBoxSize / 2, 0)).ToMatrixWithScale(), FVector(CheckBoxSize, CheckBoxSize, 1), FArmyEngineModule::Get().GetEngineResource()->GetCheckBoxMat((SymbolType & EST_BOTTOM) > 0)->GetRenderProxy(false), SDPG_World);
	}
}
void FArmySymbolLeadLine::DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas)
{

}
const FVector FArmySymbolLeadLine::GetBasePos()
{
	return BasePos;
}
const FVector FArmySymbolLeadLine::GetTargetPos() const
{
	return TargetPos;
}
const FBox FArmySymbolLeadLine::GetBounds()
{
	return FBox();
}

void FArmySymbolLeadLine::SetBasePos(const FVector& InPos)
{
	BasePos = InPos;
	Update();
}

void FArmySymbolLeadLine::SetTargetPos(const FVector& InPos)
{
	TargetPos = InPos;
	Update();
}

void FArmySymbolLeadLine::SetTargetType(uint8 InType, bool InUse)
{
	if (InUse)
	{
		SymbolType |= InType;
	}
	else
	{
		SymbolType &= ~InType;
	}
	Update();
}

bool FArmySymbolLeadLine::GetTargetType(uint8 InType) const
{
	return (SymbolType & InType) > 0;
}

void FArmySymbolLeadLine::SetColor(const FLinearColor& InColor)
{
	BaseColor = InColor;
}
bool FArmySymbolLeadLine::Pick(const FVector& InPos, FArmySymbolLeadLine::ESymbolType& OutType)
{
	FVector LeftMin = TargetPos - FVector(Offset + CheckBoxSize, CheckBoxSize / 2, 0);
	FVector LeftMax = TargetPos - FVector(Offset, -CheckBoxSize / 2, 0);
	if (FBox(LeftMin, LeftMax).IsInsideOrOn(InPos))
	{
		if (SymbolType & EST_LEFT)
		{
			SymbolType &= ~EST_LEFT;
		}
		else
		{
			SymbolType |= EST_LEFT;

			OutType = EST_LEFT;
		}
		//SymbolType & EST_LEFT ? SymbolType &= ~EST_LEFT : SymbolType |= EST_LEFT;
		return true;
	}

	FVector RightMin = TargetPos + FVector(Offset, -CheckBoxSize / 2, 0);
	FVector RightMax = TargetPos + FVector(Offset + CheckBoxSize, CheckBoxSize / 2, 0);
	if (FBox(RightMin, RightMax).IsInsideOrOn(InPos))
	{
		if (SymbolType & EST_RIGHT)
		{
			SymbolType &= ~EST_RIGHT;
		}
		else
		{
			SymbolType |= EST_RIGHT;

			OutType = EST_RIGHT;
		}
		//SymbolType & EST_RIGHT ? SymbolType &= ~EST_RIGHT : SymbolType |= EST_RIGHT;
		return true;
	}

	FVector TopMin = TargetPos - FVector(CheckBoxSize / 2, Offset + CheckBoxSize, 0);
	FVector TopMax = TargetPos - FVector(-CheckBoxSize / 2, Offset, 0);
	if (FBox(TopMin, TopMax).IsInsideOrOn(InPos))
	{
		if (SymbolType & EST_TOP)
		{
			SymbolType &= ~EST_TOP;
		}
		else
		{
			SymbolType |= EST_TOP;

			OutType = EST_TOP;
		}

		//SymbolType & EST_TOP ? SymbolType &= ~EST_TOP : SymbolType |= EST_TOP;
		return true;
	}

	FVector BottomMin = TargetPos + FVector(-CheckBoxSize / 2, Offset, 0);
	FVector BottomMax = TargetPos + FVector(CheckBoxSize / 2, Offset + CheckBoxSize, 0);
	if (FBox(BottomMin, BottomMax).IsInsideOrOn(InPos))
	{
		if (SymbolType & EST_BOTTOM)
		{
			SymbolType &= ~EST_BOTTOM;
		}
		else
		{
			SymbolType |= EST_BOTTOM;

			OutType = EST_BOTTOM;
		}

		//SymbolType & EST_BOTTOM ? SymbolType &= ~EST_BOTTOM : SymbolType |= EST_BOTTOM;
		return true;
	}
	return false;
}
void FArmySymbolLeadLine::SetCurrentRoomBoundBox(const FBox RoomBox)
{
	m_BCurrentRoomBound = RoomBox;
}
void FArmySymbolLeadLine::Update()
{
	LineVertexArray.Empty();
	
	if (TargetPos == BasePos) return;

	LineVertexArray.Add(BasePos);

	//@杨云鹤 计算第二个点
	FVector SecondPointPos;
	FVector ThirdPointPos;
	//判断末端点
	auto CertainLastPoint = [&]()
	{
		float Left_Right = TargetPos.X - SecondPointPos.X;
		float Top_Bottom = TargetPos.Y - SecondPointPos.Y;
		if (Top_Bottom == 0 && Left_Right > Offset && (SymbolType & EST_LEFT || EditorModel))
		{
			LineVertexArray.Add(TargetPos - FVector(Offset, 0, 0));
		}
		else if (Top_Bottom == 0 && Left_Right < -Offset && (SymbolType & EST_RIGHT || EditorModel))
		{
			LineVertexArray.Add(TargetPos + FVector(Offset, 0, 0));
		}
		else if (Left_Right == 0 && Top_Bottom < -Offset && (SymbolType & EST_BOTTOM || EditorModel))
		{
			LineVertexArray.Add(TargetPos + FVector(0, Offset, 0));
		}
		else if (Left_Right == 0 && Top_Bottom > Offset && (SymbolType & EST_TOP || EditorModel))
		{
			LineVertexArray.Add(TargetPos - FVector(0, Offset, 0));
		}
		else
		{
			LineVertexArray.Add(TargetPos);
		}
	};

	if (TargetPos.X < m_BCurrentRoomBound.Min.X && TargetPos.X < m_BCurrentRoomBound.Max.X
	  &&TargetPos.Y > m_BCurrentRoomBound.Min.Y && TargetPos.Y < m_BCurrentRoomBound.Max.Y)	//在屋子左侧 先画X轴
	{
		if (EditorModel)
		{
			SecondPointPos = FVector(TargetPos.X + Offset, BasePos.Y, BasePos.Z);
			ThirdPointPos = FVector(TargetPos.X + Offset, TargetPos.Y, BasePos.Z);
			LineVertexArray.Add(SecondPointPos);
			LineVertexArray.Add(ThirdPointPos);
		}
		else
		{
			if (SymbolType & EST_RIGHT)
			{
				SecondPointPos = FVector(TargetPos.X + Offset, BasePos.Y, BasePos.Z);
				ThirdPointPos = FVector(TargetPos.X + Offset, TargetPos.Y, BasePos.Z);
				LineVertexArray.Add(SecondPointPos);
				LineVertexArray.Add(ThirdPointPos);
			}
			else
			{
				if (FMath::Abs(TargetPos.Y - BasePos.Y) <= Offset)
				{
					if ((TargetPos.Y - BasePos.Y > 0 && !(SymbolType & EST_TOP))
						|| (TargetPos.Y - BasePos.Y < 0 && !(SymbolType & EST_BOTTOM)))
					{
						SecondPointPos = FVector(TargetPos.X, BasePos.Y, BasePos.Z);
						LineVertexArray.Add(SecondPointPos);
						CertainLastPoint();
					}
					else
					{
						SecondPointPos = FVector(TargetPos.X + Offset, BasePos.Y, BasePos.Z);
						ThirdPointPos = FVector(TargetPos.X + Offset, TargetPos.Y, BasePos.Z);
						LineVertexArray.Add(SecondPointPos);
						LineVertexArray.Add(ThirdPointPos);
						LineVertexArray.Add(TargetPos);
					}
				}
				else
				{
					SecondPointPos = FVector(TargetPos.X, BasePos.Y, BasePos.Z);
					LineVertexArray.Add(SecondPointPos);
					CertainLastPoint();
				}
			}
		}
	
	}
	else if (TargetPos.X > m_BCurrentRoomBound.Min.X && TargetPos.X > m_BCurrentRoomBound.Max.X
		   &&TargetPos.Y > m_BCurrentRoomBound.Min.Y && TargetPos.Y < m_BCurrentRoomBound.Max.Y)//在屋子右侧 先画X轴
	{
		if (EditorModel)
		{
			SecondPointPos = FVector(TargetPos.X - Offset, BasePos.Y, BasePos.Z);
			ThirdPointPos = FVector(TargetPos.X - Offset, TargetPos.Y, BasePos.Z);
			LineVertexArray.Add(SecondPointPos);
			LineVertexArray.Add(ThirdPointPos);
		}
		else
		{
			if (SymbolType & EST_LEFT)
			{
				SecondPointPos = FVector(TargetPos.X - Offset, BasePos.Y, BasePos.Z);
				ThirdPointPos = FVector(TargetPos.X - Offset, TargetPos.Y, BasePos.Z);
				LineVertexArray.Add(SecondPointPos);
				LineVertexArray.Add(ThirdPointPos);
			}
			else
			{
				if (FMath::Abs(TargetPos.Y - BasePos.Y) <= Offset)
				{
					if ((TargetPos.Y - BasePos.Y > 0 && !(SymbolType & EST_TOP))
						|| (TargetPos.Y - BasePos.Y < 0 && !(SymbolType & EST_BOTTOM)))
					{
						SecondPointPos = FVector(TargetPos.X, BasePos.Y, BasePos.Z);
						LineVertexArray.Add(SecondPointPos);
						CertainLastPoint();
					}
					else
					{
						SecondPointPos = FVector(TargetPos.X - Offset, BasePos.Y, BasePos.Z);
						ThirdPointPos = FVector(TargetPos.X - Offset, TargetPos.Y, BasePos.Z);
						LineVertexArray.Add(SecondPointPos);
						LineVertexArray.Add(ThirdPointPos);
						LineVertexArray.Add(TargetPos);
					}
				}
				else
				{
					SecondPointPos = FVector(TargetPos.X, BasePos.Y, BasePos.Z);
					LineVertexArray.Add(SecondPointPos);
					CertainLastPoint();
				}
			}
		}
	}
	else if (TargetPos.Y < m_BCurrentRoomBound.Min.Y && TargetPos.Y < m_BCurrentRoomBound.Max.Y
		   &&TargetPos.X > m_BCurrentRoomBound.Min.X && TargetPos.X < m_BCurrentRoomBound.Max.X)//在屋子上侧 先画Y轴
	{
		//是否是编辑模式
		if (EditorModel)
		{
			SecondPointPos = FVector(BasePos.X, TargetPos.Y + Offset, BasePos.Z);
			ThirdPointPos = FVector(TargetPos.X, TargetPos.Y + Offset, BasePos.Z);
			LineVertexArray.Add(SecondPointPos);
			LineVertexArray.Add(ThirdPointPos);
		}
		else
		{
			//是否选择下框
			if (SymbolType & EST_BOTTOM)
			{
				SecondPointPos = FVector(BasePos.X, TargetPos.Y + Offset, BasePos.Z);
				ThirdPointPos = FVector(TargetPos.X, TargetPos.Y + Offset, BasePos.Z);
				LineVertexArray.Add(SecondPointPos);
				LineVertexArray.Add(ThirdPointPos);
			}
			else
			{
				//目标点和基点的水平距离是否小于位移量
				if (FMath::Abs(TargetPos.X - BasePos.X) <= Offset)
				{
					//目标点在基点左侧并且左侧没有选中 或者在右侧并且右侧没有选中时
					if ((TargetPos.X - BasePos.X < 0 && !(SymbolType & EST_RIGHT))
						|| (TargetPos.X - BasePos.X > 0 && !(SymbolType & EST_LEFT)))
					{
						SecondPointPos = FVector(BasePos.X, TargetPos.Y, BasePos.Z);
						LineVertexArray.Add(SecondPointPos);
						CertainLastPoint();
					}
					else
					{
						SecondPointPos = FVector(BasePos.X, TargetPos.Y + Offset, BasePos.Z);
						ThirdPointPos = FVector(TargetPos.X, TargetPos.Y + Offset, BasePos.Z);
						LineVertexArray.Add(SecondPointPos);
						LineVertexArray.Add(ThirdPointPos);
						LineVertexArray.Add(TargetPos);
					}
				}
				else
				{
					SecondPointPos = FVector(BasePos.X, TargetPos.Y, BasePos.Z);
					LineVertexArray.Add(SecondPointPos);
					CertainLastPoint();
				}
			}
		}	
	}
	else if (TargetPos.Y > m_BCurrentRoomBound.Min.Y && TargetPos.Y > m_BCurrentRoomBound.Max.Y
		   &&TargetPos.X > m_BCurrentRoomBound.Min.X && TargetPos.X < m_BCurrentRoomBound.Max.X)//在屋子下侧 先画Y轴
	{
		if (EditorModel)
		{
			SecondPointPos = FVector(BasePos.X, TargetPos.Y - Offset, BasePos.Z);
			ThirdPointPos = FVector(TargetPos.X, TargetPos.Y - Offset, BasePos.Z);
			LineVertexArray.Add(SecondPointPos);
			LineVertexArray.Add(ThirdPointPos);
		}
		else
		{
			if (SymbolType & EST_TOP)
			{
				SecondPointPos = FVector(BasePos.X, TargetPos.Y - Offset, BasePos.Z);
				ThirdPointPos = FVector(TargetPos.X, TargetPos.Y - Offset, BasePos.Z);
				LineVertexArray.Add(SecondPointPos);
				LineVertexArray.Add(ThirdPointPos);
			}
			else
			{
				if (FMath::Abs(TargetPos.X - BasePos.X) <= Offset)
				{
					if ((TargetPos.X - BasePos.X < 0 && !(SymbolType & EST_RIGHT))
						|| (TargetPos.X - BasePos.X > 0 && !(SymbolType & EST_LEFT)))
					{
						SecondPointPos = FVector(BasePos.X, TargetPos.Y, BasePos.Z);
						LineVertexArray.Add(SecondPointPos);
						CertainLastPoint();
					}
					else
					{
						SecondPointPos = FVector(BasePos.X, TargetPos.Y - Offset, BasePos.Z);
						ThirdPointPos = FVector(TargetPos.X, TargetPos.Y - Offset, BasePos.Z);
						LineVertexArray.Add(SecondPointPos);
						LineVertexArray.Add(ThirdPointPos);
						LineVertexArray.Add(TargetPos);
					}
				}
				else
				{
					SecondPointPos = FVector(BasePos.X, TargetPos.Y, BasePos.Z);
					LineVertexArray.Add(SecondPointPos);
					CertainLastPoint();
				}
			}
		}
	}
	else //其他情况以长优先
	{
		auto horizontalLength = TargetPos.X - BasePos.X;
		auto verticalLength = TargetPos.Y - BasePos.Y;
		horizontalLength = horizontalLength > 0 ? horizontalLength : -horizontalLength;
		verticalLength = verticalLength > 0 ? verticalLength : -verticalLength;
		if (horizontalLength < verticalLength)
		{
			SecondPointPos = FVector(BasePos.X, TargetPos.Y, BasePos.Z);
			LineVertexArray.Add(SecondPointPos);
			CertainLastPoint();
		}
		else
		{
			SecondPointPos = FVector(TargetPos.X, BasePos.Y, BasePos.Z);
			LineVertexArray.Add(SecondPointPos);
			CertainLastPoint();
		}
	}

}