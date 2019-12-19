#include "ArmyAutoCad.h"
#include "StringConv.h"
#include "Factory.h"
#include "AttributeEntity.h"
#include "ArmyConstructionFrame.h"
#include "ArmyWallLine.h"
#include "ArmyBeam.h"
#include "ComponentReader.h"
#include "ArmyPrimitive.h"
#include "ArmyPoint.h"
#include "ArmyCompass.h"
#include "ArmyWindow.h"
#include "ArmyFloorWindow.h"
#include "ArmyRectBayWindow.h"
#include "ArmyTrapeBayWindow.h"
#include "ArmyCornerBayWindow.h"
#include "ArmyPillar.h"
#include "ArmyBeam.h"
#include "ArmyAirFlue.h"
#include "ArmySingleDoor.h"
#include "ArmySlidingDoor.h"
#include "ArmySecurityDoor.h"
#include "ArmyDimensions.h"
#include "ArmyTextLabel.h"
#include "ArmyDownLeadLabel.h"
#include "ArmyEntrance.h"
#include "ArmyBoardSplitline.h"
#include "ArmyLampControlLines.h"
#include "ArmyPackPipe.h"
#include "ArmyPass.h"
#include "ArmyNewPass.h"
#include "ArmyPunch.h"
#include "ArmySymbol.h"
#include "ArmyCustomDefine.h"
#include "ArmyWHCTableObject.h"
#include "ArmyWHCDoorSheet.h"


#define   EXTEND_VALUE  10.f 


#pragma region block辅助类
"FArmyCadBundle::"FArmyCadBundle()
{
	BasePoint = FVector::ZeroVector;
	IsDefaultDesc = true;
}

void "FArmyCadBundle::AddPoint(const TSharedPtr<"FArmyPoint> &Point, const FString &LinesType, LineWeight Weight)
{
	Points.Add(Point);
	PointsWeight.Add(Weight);
	PointsLineName.Add(LinesType);
}
void "FArmyCadBundle::AddLine(const TSharedPtr<"FArmyLine>&Line, const FString &LinesType, LineWeight Weight )
{
	Lines.Add(Line);
	LinesWeight.Add(Weight);
	LinesLineName.Add(LinesType);
}
void "FArmyCadBundle::AddPolygon(const TSharedPtr<"FArmyPolygon>&Polygon, const FString &LinesType , LineWeight Weight )
{
	Polygons.Add(Polygon);
	PolygonsWeight.Add(Weight);
	PolygonsLineName.Add(LinesType);
}
void "FArmyCadBundle::AddCircle(const TSharedPtr<"FArmyCircle>&Circle, const FString &LinesType , LineWeight Weight )
{
	Circles.Add(Circle);
	CirclesWeight.Add(Weight);
	CirclesLineName.Add(LinesType);
}
void "FArmyCadBundle::AddArcLine(const TSharedPtr<"FArmyArcLine>&ArcLine, const FString &LinesType , LineWeight Weight )
{
	ArcLines.Add(ArcLine);
	ArcLinesWeight.Add(Weight);
	ArcLinesLineName.Add(LinesType);
}
void "FArmyCadBundle::AddPolyline(const TSharedPtr<"FArmyPolyline>&Polyline, const FString &LinesType , LineWeight Weight )
{
	Polylines.Add(Polyline);
	PolylinesWeight.Add(Weight);
	PolylinesLineName.Add(LinesType);
}
 
/**
* 添加文字
* @param Polyline - const "FArmyPolyline & -
* @return void -
*/
void "FArmyCadBundle::AddCanvasText(const FScaleCanvasText CanvasText, const FString &LinesType, LineWeight Weight)
{
	CanvasTexts.Add(CanvasText);
	CanvasTextsWeight.Add(Weight);
	CanvasTextsLineName.Add(LinesType);
}
void "FArmyCadBundle::AddKey(int32 InKey)
{
	this->Key = InKey;
}
 
#pragma endregion block辅助类

#pragma region 部分辅助方法
//最大值
FVector MaxVector (const FVector &Left, const FVector &Right)
{
	return FVector(FMath::Max(Left.X, Right.X), FMath::Max(Left.Y, Right.Y), FMath::Max(Left.Z, Right.Z));
}

//最小值
FVector MinVector(const FVector &Left, const FVector &Right)
{
	return FVector(FMath::Min(Left.X, Right.X),FMath::Min(Left.Y, Right.Y), FMath::Min(Left.Z, Right.Z));
}

Double4 operator+(const Double4&left, const Double4&right)
{

	return Double4(left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w);
}

#pragma endregion 部分辅助方法


const TSharedRef<"FArmyAutoCad>& "FArmyAutoCad::Get()
{
	static const TSharedRef<"FArmyAutoCad> Instance = MakeShareable(new "FArmyAutoCad());
	return Instance;
}

"FArmyAutoCad::"FArmyAutoCad()
{
	InitializeArmyAutoCad();
}

"FArmyAutoCad::~"FArmyAutoCad()
{
	EntityFactory->release();
	Canvas->release();
	ClearBlocks();
}


///////////////////////CAD绘制公开方法////////////////////////
#pragma region CAD绘制公开方法

//清理画布
void "FArmyAutoCad::ClearCanvas()
{
	Canvas->clear();
	for (auto block : TempBlock)
		block->release();
	TempBlock.Empty();
	BlockRecord.Empty();
	// 清空图框管理器
	LayerManager.Empty();
	for (auto frame : FrameManager)
		frame.Value.block->release();

	FrameManager.Empty();
	InitIncrementBlockCounter();

	// 清空自定义块管理器
	DefineBlockMap.Empty();
	RepeatBlock.Empty();
	//Add System
	Canvas->addBlock(*SysBlockMap.Find("ArchTick"));
	Canvas->addBlock(*SysBlockMap.Find("_DotSmall"));
	
	FrameStepLength = 0;
}

// 创建图层
bool "FArmyAutoCad::CreateLayer(TMap<int32, TSharedPtr<LayerInfo>> LayerMap)
{
	for (auto It : LayerMap)
	{
		AttributeDesc LayerDesc;
	 
		FMemory::Memzero(&LayerDesc, sizeof(AttributeDesc));
		LayerDesc.colorType = Fixed;
		LayerDesc.red = It.Value->LayerColor.R;
		LayerDesc.green = It.Value->LayerColor.G;
		LayerDesc.blue = It.Value->LayerColor.B;
		LayerDesc.lineType = TCHAR_TO_UTF8(*It.Value->LineType);
		LayerDesc.weight = (LineWeight)It.Value->LayerLineWeight;

		AttributeEntity*Layer = EntityFactory->createLayer(Utf82Gbk(TCHAR_TO_UTF8(*It.Value->Name)).c_str(), LayerFlag::LayerDefault);
		Layer->setEntityAttribute(LayerDesc);
		Canvas->addLayer(Layer);

		LayerManager.Add(It.Value->Name, true);
	}

	return true;
}

// 通过文件添加构件
bool "FArmyAutoCad::AddComponent(int32 Key, FString Path)
{
	if (!ComponentManager.Contains(Key))
	{
		ComponentReader reader;
		if (!reader.readBlockFile(TCHAR_TO_UTF8(*Path)))
			return false;
		Double4 vector = reader.getBenchmarkPoint();
		auto entityArray = reader.getEntity();

		FString blockName = L"Component_" + FString::FromInt(Key);
		IBlock*  Comblock = EntityFactory->createBlockCanvas(Utf82Gbk(TCHAR_TO_UTF8(*blockName)).c_str(), Double4());

		for (auto entity : entityArray)
			Comblock->drawEntity(entity);

		BlockExt Component(Comblock, FVector(vector.x, vector.y, vector.z), blockName);
		ComponentManager.Add(Key, Component);
	}
	return true;
}

// 通过bim数据添加构件
bool "FArmyAutoCad::AddComponent(int32 Key, TSharedPtr<class "FArmyFurniture> Furniture)
{
	if (!ComponentManager.Contains(Key))
	{
		int count = Furniture->GetElementVertex().Num();
		FString blockName = L"Component_" + FString::FromInt(Key);
		IBlock*  Comblock = EntityFactory->createBlockCanvas(Utf82Gbk(TCHAR_TO_UTF8(*blockName)).c_str(), Double4());

		FVector PivotPoint = Furniture->GetPivortPoint();
		for (auto It : Furniture->GetPolys())
		{
			int count = It->GetAllVertices().Num();
			if (count < 2)
			{
				continue;
			}

			AttributeDesc   Desc;
			FMemory::Memzero(&Desc, sizeof(AttributeDesc));
			Desc.red = DefaultColor.R;
			Desc.green = DefaultColor.G;
			Desc.blue = DefaultColor.B;
			Desc.lineType = "CONTINUOUS";
			Desc.weight = Weight005;

			if (It->GetLineType() == 1)
			{
				for (int i = 1; i < count; i++)
				{
					FVector StartPoint = It->GetAllVertices()[i - 1] - PivotPoint;
					FVector EndPoint = It->GetAllVertices()[i] - PivotPoint;
					auto attributeEntity = EntityFactory->createLine(GetCadPos(StartPoint), GetCadPos(EndPoint));
					attributeEntity->setEntityAttribute(Desc);
					Comblock->drawEntity(attributeEntity);
				}
			}
			else if (It->GetLineType() == 0)
			{
				for (int i = 1; i < count; i += 2)
				{
					FVector StartPoint = It->GetAllVertices()[i - 1] - PivotPoint;
					FVector EndPoint = It->GetAllVertices()[i] - PivotPoint;
					auto attributeEntity = EntityFactory->createLine(GetCadPos(StartPoint), GetCadPos(EndPoint));
					attributeEntity->setEntityAttribute(Desc);
					Comblock->drawEntity(attributeEntity);
				}
			}
			else if (It->GetLineType() == 2)
			{
				for (int i = 0; i < count; i++)
				{
					FVector StartPoint = It->GetAllVertices()[i%count] - PivotPoint;
					FVector EndPoint = It->GetAllVertices()[(i + 1) % count] - PivotPoint;
					auto attributeEntity = EntityFactory->createLine(GetCadPos(StartPoint), GetCadPos(EndPoint));
					attributeEntity->setEntityAttribute(Desc);
					Comblock->drawEntity(attributeEntity);
				}
			}
		}

		BlockExt Component(Comblock, PivotPoint, blockName);
		ComponentManager.Add(Key, Component);
	}
	return true;
}

// 通过文件添加家具
bool "FArmyAutoCad::AddFurniture(int32 Key, FString Path)
{
	if (FurnitureManager.Contains(Key))
		return false;
	ComponentReader reader;
	if (!reader.readBlockFile(TCHAR_TO_UTF8(*Path)))
		return false;
	Double4 vector = reader.getBenchmarkPoint();
	auto entityArray = reader.getEntity();

	FString blockName = L"Furniture_" + FString::FromInt(Key);
	IBlock* Comblock = EntityFactory->createBlockCanvas(Utf82Gbk(TCHAR_TO_UTF8(*blockName)).c_str(), Double4());

	for (auto entity : entityArray)
		Comblock->drawEntity(entity);

	BlockExt Furniture(Comblock, FVector(vector.x, vector.y, vector.z), blockName);
	FurnitureManager.Add(Key, Furniture);
	return true;
}

// 通过bim数据添加家具
bool "FArmyAutoCad::AddFurniture(int32 Key, TSharedPtr<class "FArmyFurniture> Furniture)
{
	if (!FurnitureManager.Contains(Key))
	{
		int count = Furniture->GetElementVertex().Num();
		FString blockName = L"Furniture_" + FString::FromInt(Key);
		IBlock*  Comblock = EntityFactory->createBlockCanvas(Utf82Gbk(TCHAR_TO_UTF8(*blockName)).c_str(), Double4());

		FVector PivotPoint = Furniture->GetPivortPoint();
		for (auto It : Furniture->GetPolys())
		{
			int count = It->GetAllVertices().Num();
			if (count < 2)
			{
				continue;
			}

			AttributeDesc   Desc;
			FMemory::Memzero(&Desc, sizeof(AttributeDesc));
			Desc.red = DefaultColor.R;
			Desc.green = DefaultColor.G;
			Desc.blue = DefaultColor.B;
			Desc.lineType = "CONTINUOUS";
			Desc.weight = Weight000;

			if (It->GetLineType() == 1)
			{
				for (int i = 1; i < count; i++)
				{
					FVector StartPoint = It->GetAllVertices()[i - 1] - PivotPoint;
					FVector EndPoint = It->GetAllVertices()[i] - PivotPoint;
					auto attributeEntity = EntityFactory->createLine(GetCadPos(StartPoint), GetCadPos(EndPoint));
					attributeEntity->setEntityAttribute(Desc);
					Comblock->drawEntity(attributeEntity);
				}
			}
			else if (It->GetLineType() == 0)
			{
				for (int i = 1; i < count; i += 2)
				{
					FVector StartPoint = It->GetAllVertices()[i - 1] - PivotPoint;
					FVector EndPoint = It->GetAllVertices()[i] - PivotPoint;
					auto attributeEntity = EntityFactory->createLine(GetCadPos(StartPoint), GetCadPos(EndPoint));
					attributeEntity->setEntityAttribute(Desc);
					Comblock->drawEntity(attributeEntity);
				}
			}
			else if (It->GetLineType() == 2)
			{
				for (int i = 0; i < count; i++)
				{
					FVector StartPoint = It->GetAllVertices()[i%count] - PivotPoint;
					FVector EndPoint = It->GetAllVertices()[(i + 1) % count] - PivotPoint;
					auto attributeEntity = EntityFactory->createLine(GetCadPos(StartPoint), GetCadPos(EndPoint));
					attributeEntity->setEntityAttribute(Desc);
					Comblock->drawEntity(attributeEntity);
				}
			}
		}

		BlockExt Component(Comblock, PivotPoint, blockName);
		FurnitureManager.Add(Key, Component);
	}
	return true;
}

// 在指定的图层上绘制图框
bool "FArmyAutoCad::DrawFrame(const FString LayerName, int index, const TSharedPtr<class "FArmyConstructionFrame>& InFrame)
{
	if (!LayerManager.Contains(LayerName))
		return false;

	//New Block
	FString blockName = L"Frame_" + FString::FromInt(index);

	auto newBlock = EntityFactory->createBlockCanvas(TCHAR_TO_UTF8(*blockName), Double4());
	//计算图框偏移
	FBox  Box = CalculateBoxMin(InFrame);
	FTransform LocalTransform = InFrame->GetLocalTransform();
	float length = (Box.Max - Box.Min).X;
	if (FrameStepLength < length / 6)
	{
		FrameStepLength = length / 6;
	}
	float NewLength = GetFrameOffset(index);
	if (NewLength < KINDA_SMALL_NUMBER)
	{
		NewLength = FrameManager.Num()*(length + length / 6);
	}

	//auto newX = FrameManager.Num()*(length + length / 6);

	FVector   dist = FVector(NewLength, 0, 0);
	FVector   offset = dist - Box.Min;

	BlockExt frame(newBlock, offset, blockName,length);

	TArray<TSharedPtr<"FArmyLine>> BaseLines;
	InFrame->GetFrameBaseLines(BaseLines);
	for (auto line : BaseLines)
	{
		AttributeDesc   Desc;
		FMemory::Memzero(&Desc, sizeof(AttributeDesc));
		Desc.red = DefaultColor.R;
		Desc.green = DefaultColor.G;
		Desc.blue = DefaultColor.B;
		Desc.lineType = "CONTINUOUS";
		Desc.weight = Weight000;

		FVector StartPos = LocalTransform.TransformPosition(line->GetStart());
		FVector EndPos = LocalTransform.TransformPosition(line->GetEnd());

		auto attributeEntity = EntityFactory->createLine(GetCadPos(StartPos), GetCadPos(EndPos));
		attributeEntity->setEntityAttribute(Desc);
		newBlock->drawEntity(attributeEntity);
	}

	TMap<int32, "FArmyConstructionFrame::FillAreaInfo> fillAreasMap;
	InFrame->GetFillAreasMap(fillAreasMap);
	for (auto area : fillAreasMap)
	{
		"FArmyConstructionFrame::FillAreaInfo &info = area.Value;

		MTextDesc textDesc;
		FVector AreaPos = LocalTransform.TransformPosition(info.RectArea->Pos);

		auto Scale= LocalTransform.GetScale3D();
		textDesc.alignmentPoint = GetCadPos(AreaPos);
		
		textDesc.drawDirection = LeftToRight;
		textDesc.hAlign = HAlignLeft;
		textDesc.vAlign = VAlignTop;
		textDesc.lineSpaceStyle = Exact;

		textDesc.text = CorrectString(Utf82Gbk(TCHAR_TO_UTF8(*info.TextContent)));
		textDesc.angle = 0;
		textDesc.style = "standard";
		textDesc.width = GetCadLength(info.RectArea->Width*0.8)*Scale.X;
		textDesc.height = GetCadLength(info.FontSize)*Scale.Y*0.8;
		textDesc.lineSpacingFactor = 1.;

		auto mTextEntity = EntityFactory->createMText(textDesc);

		AttributeDesc   Desc;
		FMemory::Memzero(&Desc, sizeof(AttributeDesc));
		Desc.red = DefaultColor.R;
		Desc.green = DefaultColor.G;
		Desc.blue = DefaultColor.B;
		Desc.lineType = "CONTINUOUS";
		Desc.weight = Weight000;

		mTextEntity->setEntityAttribute(Desc);

		newBlock->drawEntity(mTextEntity);
	}

	//TMap<int32, TSharedPtr<"FArmyFurniture>> fillAreaDxfMap;
	//InFrame->GetFillAreaDxfMap(fillAreaDxfMap);
	for (auto area : fillAreasMap)
	{
		TSharedPtr<"FArmyFurniture> Furniture = area.Value.DXFData;
		if (!Furniture.IsValid())
		{
			continue;
		}

		TArray<struct FSimpleElementVertex> ElementVertexList = Furniture->GetElementVertex();
		int count = ElementVertexList.Num();
		for (int i = 1; i < count; i+=2)
		{
			if (count < 2)
			{
				break;
			}

			AttributeDesc   Desc;
			FMemory::Memzero(&Desc, sizeof(AttributeDesc));
			Desc.red = DefaultColor.R;
			Desc.green = DefaultColor.G;
			Desc.blue = DefaultColor.B;
			Desc.lineType = "CONTINUOUS";
			Desc.weight = Weight000;

			FVector StartPoint = ElementVertexList[i - 1].Position;
			FVector EndPoint = ElementVertexList[i].Position;
			auto attributeEntity = EntityFactory->createLine(GetCadPos(StartPoint), GetCadPos(EndPoint));
			attributeEntity->setEntityAttribute(Desc);
			newBlock->drawEntity(attributeEntity);
		}
	}
	FrameManager.Add(index, frame);

	Canvas->addBlock(newBlock);


	auto blockRef = EntityFactory->createBlockReference(TCHAR_TO_UTF8(*blockName), VectorHelper(offset).asDouble3(), VectorHelper(FVector::OneVector).asDouble3(), 0, 1, 1, 0, 0);

	AttributeDesc Desc = GetComAttributeDesc(LayerName);
	blockRef->setEntityAttribute(Desc);
	Canvas->drawEntity(blockRef);

	return true;
}

// 在指定的图框指定的图层上绘制构件
bool "FArmyAutoCad::DrawComponent(const int32 FrameIndex, const FString LayerName, int32 Key, const TSharedPtr<class "FArmyFurniture> InFurniture)
{

	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	if(!InFurniture.IsValid())
	{
		return false;
	}
	// 如果构件不存在则添加构件
	if (!ComponentManager.Contains(Key))
	{
		if (!AddComponent(Key, InFurniture))
		{
			return false;
		}
	}

	auto ComponentBlock = *ComponentManager.Find(Key);
	auto CadPos = GetCadPosOfFrame(InFurniture->LocalTransform.GetLocation(), FrameIndex);
	FVector Scale = InFurniture->LocalTransform.GetScale3D();
	auto  Angle = InFurniture->LocalTransform.GetRotation().Euler().Z / 180.0*M_PI;

	if (!BlockRecord.Contains(Key)) {
		Canvas->addBlock(ComponentBlock.block);
		BlockRecord.Add(Key);
	}
	auto Ref = EntityFactory->createBlockReference(Utf82Gbk(TCHAR_TO_UTF8(*ComponentBlock.blockName)).c_str(), CadPos, VectorHelper(Scale).asDouble3(), -Angle, 1, 1, 0, 0);

	AttributeDesc	Desc = GetComAttributeDesc(LayerName);
	// 设置图例的宽度为0.05
	Desc.weight = LineWeight::Weight005;
	Ref->setEntityAttribute(Desc);

	Canvas->drawEntity(Ref);
	return true;
}

// 在指定的图框指定的图层上绘制家具
bool "FArmyAutoCad::DrawFurniture(const int32 FrameIndex, const FString LayerName, int32 Key, const FVector&Postion, const FVector &Scale, const float Angle)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;
	if (!FurnitureManager.Contains(Key))
		return false;
	auto FurnitureBlock = *FurnitureManager.Find(Key);
	if (!BlockRecord.Contains(Key)) {
		Canvas->addBlock(FurnitureBlock.block);
		BlockRecord.Add(Key);
	}

	auto CadPos = GetCadPosOfFrame(Postion, FrameIndex);
	auto Ref = EntityFactory->createBlockReference(Utf82Gbk(TCHAR_TO_UTF8(*FurnitureBlock.blockName)).c_str(), CadPos, VectorHelper(Scale).asDouble3(), Angle, 1, 1, 0, 0);
	AttributeDesc	Desc = GetComAttributeDesc(LayerName);
	Ref->setEntityAttribute(Desc);

	Canvas->drawEntity(Ref);
	return true;
}

// 在指定的图框指定的图层上绘制线段
bool "FArmyAutoCad::DrawLine(const int32 FrameIndex, const FString LayerName, const FVector &Beg, const FVector &End)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	AttributeEntity* entity = EntityFactory->createLine(GetCadPosOfFrame(Beg, FrameIndex), GetCadPosOfFrame(End, FrameIndex));
	AttributeDesc Desc = GetComAttributeDesc(LayerName);
	entity->setEntityAttribute(Desc);
	Canvas->drawEntity(entity);

	return true;
}

/**
* 在指定的图框指定的图层上绘制线段
* @param FrameIndex - const int32 - 图框名称
* @param InLayer - const TSharedPtr<LayerInfo> - 图层信息
* @param WallLine - const TSharedPtr<class "FArmyLine> & - 墙线
* @return bool - true 表示绘制成功
*/
bool "FArmyAutoCad::DrawLine(const int32 FrameIndex, TSharedPtr<LayerInfo> InLayer, const TSharedPtr<class "FArmyLine>& WallLine)
{
	if (!InLayer.IsValid())
	{
		return false;
	}

	if (!CheckDraw(FrameIndex, InLayer->Name))
		return false;

	AttributeEntity* entity = EntityFactory->createLine(GetCadPosOfFrame(WallLine->GetStart(), FrameIndex), GetCadPosOfFrame(WallLine->GetEnd(), FrameIndex));
	AttributeDesc Desc = GetAttributeDescByLayer(InLayer);
	entity->setEntityAttribute(Desc);
	Canvas->drawEntity(entity);

	return true;
}

// 在指定的图框指定的图层上绘制线段
bool "FArmyAutoCad::DrawLine(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyLine>& WallLine)
{
	return DrawLine(FrameIndex, LayerName, WallLine->GetStart(), WallLine->GetEnd());
}

/**
* 在指定的图框指定的图层上绘制多边形线
* @param FrameIndex - const int32 - 图框名称
* @param LayerName - const int32 - 图层名称
* @param InPolygon - const TSharedPtr<class "FArmyPolygon>&  - 多边形
* @return bool - true 表示绘制成功
*/
bool "FArmyAutoCad::DrawPolygon(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyPolygon>& InPolygon)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	if (!InPolygon.IsValid())
	{
		return false;
	}

	TArray<"FArmyLine> LineList;
	int32 count = InPolygon->Vertices.Num();
	if (count < 2)
	{
		return false;
	}
	for (int32 index = 0; index < count; index++)
	{
		if (!DrawLine(FrameIndex, LayerName, InPolygon->Vertices[index%count], InPolygon->Vertices[(index + 1) % count]))
		{
			return false;
		}
	}

	return true;
}
// 在指定的图框指定的图层上绘制标准窗户
bool "FArmyAutoCad::DrawWindow(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyWindow> InWindow)
{
	//return DrawWindow2(FrameIndex, LayerName, InWindow);
	if (!InWindow.IsValid())
	{
		return false;
	}

	TArray<TSharedPtr<"FArmyLine>> LineArray;
	TArray<TPair<FVector, FVector>> ObjLineList;
	InWindow->GetCadLineArray(ObjLineList);
	for (TPair<FVector, FVector> ObjLineIt : ObjLineList)
	{
		LineArray.Emplace(MakeShareable(new "FArmyLine(ObjLineIt.Key, ObjLineIt.Value)));
	}
	return DrawComWindow(FrameIndex, LayerName, LineArray);
}

bool "FArmyAutoCad::DrawWindow2(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyWindow> InWindow)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;
	if (!InWindow.IsValid())
		return false;
	FString Key = CalculateWindowKey(InWindow);
	float reverse =   CalculateWindowAngle(InWindow);
	FVector basePoint = CalculateWindowOffset(InWindow);
	//draw ref
	BlockExt ext;
	if (!RepeatBlock.Contains(Key))
	{
		auto WindowBounle = GetCadBounleByWindows(InWindow, basePoint, -reverse);
		WindowBounle->BlockName = Key;
		WindowBounle->BasePoint = basePoint;
		DrawComBlockWithoutRef(WindowBounle, ext);
		RepeatBlock.Add(Key, ext);
	}
	else
		ext.offset = basePoint;

	DrawComBlockRef(FrameIndex, LayerName, ext.blockName, GetCadPosOfFrame(ext.offset, FrameIndex), reverse);
	return true;
}

// 在指定的图框指定的图层上绘制落地窗
bool "FArmyAutoCad::DrawFloorWindow(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyFloorWindow> InFloorWindow)
{
	//return	DrawFloorWindow2(FrameIndex, LayerName, InFloorWindow);
	if (!InFloorWindow.IsValid())
	{
		return false;
	}

	TArray<TSharedPtr<"FArmyLine>> LineArray;
	TArray<TPair<FVector, FVector>> ObjLineList;
	InFloorWindow->GetCadLineArray(ObjLineList);
	for (TPair<FVector, FVector> ObjLineIt : ObjLineList)
	{
		LineArray.Emplace(MakeShareable(new "FArmyLine(ObjLineIt.Key, ObjLineIt.Value)));
	}
	return DrawComWindow(FrameIndex, LayerName, LineArray);
}
bool "FArmyAutoCad::DrawFloorWindow2(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyFloorWindow> InFloorWindow)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;
	if (!InFloorWindow.IsValid())
		return false;
	FString Key = CalculateFloorWindowKey(InFloorWindow);
	float reverse = CalculateFloorWindowAngle(InFloorWindow);
	FVector basePoint = CalculateFloorWindowOffset(InFloorWindow);
	//draw ref
	BlockExt ext;
	if (!RepeatBlock.Contains(Key))
	{
		auto WindowBounle = GetCadBounleByWindows(InFloorWindow, basePoint, -reverse);
		WindowBounle->BlockName = Key;
		WindowBounle->BasePoint = basePoint;
		DrawComBlockWithoutRef(WindowBounle, ext);
		RepeatBlock.Add(Key, ext);
	}
	else
		ext.offset = basePoint;

	DrawComBlockRef(FrameIndex, LayerName, ext.blockName, GetCadPosOfFrame(ext.offset, FrameIndex), reverse);
	return true;
}
// 在指定的图框指定的图层上绘矩形飘窗
bool "FArmyAutoCad::DrawRectBayWindow(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyRectBayWindow> InRectBayWindow)
{
	//return DrawRectBayWindow2(FrameIndex, LayerName, InRectBayWindow);
	if (!InRectBayWindow.IsValid())
	{
		return false;
	}

	TArray<TSharedPtr<"FArmyLine>> LineArray;
	TArray<TPair<FVector, FVector>> ObjLineList;
	InRectBayWindow->GetCadLineArray(ObjLineList);
	for (TPair<FVector, FVector> ObjLineIt : ObjLineList)
	{
		LineArray.Emplace(MakeShareable(new "FArmyLine(ObjLineIt.Key, ObjLineIt.Value)));
	}
	return DrawComWindow(FrameIndex, LayerName, LineArray);
}

bool "FArmyAutoCad::DrawRectBayWindow2(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyRectBayWindow> InRectBayWindow)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;
	if (!InRectBayWindow.IsValid())
		return false;
	FString Key = CalculateRectBayWindowKey(InRectBayWindow);
	float reverse = CalculateRectBayWindowAngle(InRectBayWindow);
	FVector basePoint = CalculateRectBayWindowOffset(InRectBayWindow);
	//draw ref
	BlockExt ext;
	if (!RepeatBlock.Contains(Key))
	{
		auto WindowBounle = GetCadBounleByWindows(InRectBayWindow, basePoint, -reverse);
		WindowBounle->BlockName = Key;
		WindowBounle->BasePoint = basePoint;
		DrawComBlockWithoutRef(WindowBounle, ext);
		RepeatBlock.Add(Key, ext);
	}
	else
		ext.offset = basePoint;

	DrawComBlockRef(FrameIndex, LayerName, ext.blockName, GetCadPosOfFrame(ext.offset, FrameIndex), reverse);
 
	return true;
}

// 在指定的图框指定的图层上绘制梯形飘窗
bool "FArmyAutoCad::DrawTrapeBayWindow(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyTrapeBayWindow> InTrapeBayWindow)
{
	//return DrawTrapeBayWindow2(FrameIndex, LayerName, InTrapeBayWindow);
	if (!InTrapeBayWindow.IsValid())
	{
		return false;
	}

	TArray<TSharedPtr<"FArmyLine>> LineArray;
	TArray<TPair<FVector, FVector>> ObjLineList;
	InTrapeBayWindow->GetCadLineArray(ObjLineList);
	for (TPair<FVector, FVector> ObjLineIt : ObjLineList)
	{
		LineArray.Emplace(MakeShareable(new "FArmyLine(ObjLineIt.Key, ObjLineIt.Value)));
	}
	return DrawComWindow(FrameIndex, LayerName, LineArray);
}
bool  "FArmyAutoCad::DrawTrapeBayWindow2(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyTrapeBayWindow> InTrapeBayWindow)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;
	if (!InTrapeBayWindow.IsValid())
		return false;
	FString Key = CalculateTrapeBayWindowKey(InTrapeBayWindow);
	float reverse = CalculateTrapeBayWindowAngle(InTrapeBayWindow);
	FVector basePoint = CalculateTrapeBayWindowOffset(InTrapeBayWindow);
	//draw ref
	BlockExt ext;
	if (!RepeatBlock.Contains(Key))
	{
		auto WindowBounle = GetCadBounleByWindows(InTrapeBayWindow, basePoint, -reverse);
		WindowBounle->BlockName = Key;
		WindowBounle->BasePoint = basePoint;
		DrawComBlockWithoutRef(WindowBounle, ext);
		RepeatBlock.Add(Key, ext);
	}
	else
		ext.offset = basePoint;
	DrawComBlockRef(FrameIndex, LayerName, ext.blockName, GetCadPosOfFrame(ext.offset, FrameIndex), reverse);
	return true;
}
// 在指定的图框指定的图层上绘制拐角飘窗
bool "FArmyAutoCad::DrawCornerBayWindow(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyCornerBayWindow> InCornerBayWindow)
{
	//return DrawCornerBayWindow2(FrameIndex, LayerName, InCornerBayWindow);
	if (!InCornerBayWindow.IsValid())
	{
		return false;
	}

	TArray<TSharedPtr<"FArmyLine>> LineArray;
	TArray<TPair<FVector, FVector>> ObjLineList;
	InCornerBayWindow->GetCadLineArray(ObjLineList);
	for (TPair<FVector, FVector> ObjLineIt : ObjLineList)
	{
		LineArray.Emplace(MakeShareable(new "FArmyLine(ObjLineIt.Key, ObjLineIt.Value)));
	}
	return DrawComWindow(FrameIndex, LayerName, LineArray);
}
bool "FArmyAutoCad::DrawCornerBayWindow2(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyCornerBayWindow> InCornerBayWindow)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;
	if (!InCornerBayWindow.IsValid())
		return false;
	FString Key = CalculateCornerBayWindowKey(InCornerBayWindow);
	float reverse = CalculateCornerBayWindowAngle(InCornerBayWindow);
	FVector basePoint = CalculateCornerBayWindowOffset(InCornerBayWindow);
	//draw ref
	BlockExt ext;
	if (!RepeatBlock.Contains(Key))
	{
		auto WindowBounle = GetCadBounleByWindows(InCornerBayWindow, basePoint, -reverse);
		WindowBounle->BlockName = Key;
		WindowBounle->BasePoint = basePoint;
		DrawComBlockWithoutRef(WindowBounle, ext);
		RepeatBlock.Add(Key, ext);
	}
	else
		ext.offset = basePoint;
	DrawComBlockRef(FrameIndex, LayerName, ext.blockName, GetCadPosOfFrame(ext.offset, FrameIndex), reverse);
	return true;
}
// 指定的图框指定的图层上绘制柱子
bool "FArmyAutoCad::DrawPillar(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyPillar> InPillar)
{
	if (!InPillar.IsValid())
	{
		return false;
	}

	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	TSharedPtr<"FArmyCadBundle> PillarBounle = MakeShareable(new "FArmyCadBundle());
	TArray<TPair<FVector, FVector>> ObjLineList;
	InPillar->GetCadLineArray(ObjLineList);
	for (TPair<FVector, FVector> ObjLineIt : ObjLineList)
	{
		PillarBounle->AddLine(MakeShareable(new "FArmyLine(ObjLineIt.Key, ObjLineIt.Value)));
	}


	PillarBounle->BlockName = L"Pillar_" + FString::FromInt(GetBlockValue(IBT_Pillar));
	if (DrawComBlock(FrameIndex, LayerName, PillarBounle))
	{
		UpdateIncrementBlockCounter(IBT_Pillar);
		return true;
	}
	else
	{
		return false;
	}
}

// 在指定的图框指定的图层上绘制顶面梁
bool "FArmyAutoCad::DrawBeam(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyBeam> InBeam)
{
	if (!InBeam.IsValid())
	{
		return false;
	}

	if (!CheckDraw(FrameIndex, LayerName))
		return false;


	TSharedPtr<"FArmyCadBundle> BeamBounle = MakeShareable(new "FArmyCadBundle());
	TArray<TPair<FVector, FVector>> ObjLineList;
	InBeam->GetCadLineArray(ObjLineList);
	for (TPair<FVector, FVector> ObjLineIt : ObjLineList)
	{
		BeamBounle->AddLine(MakeShareable(new "FArmyLine(ObjLineIt.Key, ObjLineIt.Value)), L"DASHEDX2");
	}

	BeamBounle->BlockName = L"Beam_" + FString::FromInt(GetBlockValue(IBT_Beam));
	if (DrawComBlock(FrameIndex, LayerName, BeamBounle))
	{
		UpdateIncrementBlockCounter(IBT_Beam);
		return true;
	}
	else
	{
		return false;
	}
}

// 在指定的图框指定的图层上绘制风道
bool "FArmyAutoCad::DrawAirFlue(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyAirFlue>  InAirFlue)
{
	if (!InAirFlue.IsValid())
	{
		return false;
	}

	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	TSharedPtr<"FArmyCadBundle> AirFlueBounle = MakeShareable(new "FArmyCadBundle());
	TArray<TPair<FVector, FVector>> ObjLineList;
	InAirFlue->GetCadLineArray(ObjLineList);
	for (TPair<FVector, FVector> ObjLineIt : ObjLineList)
	{
		AirFlueBounle->AddLine(MakeShareable(new "FArmyLine(ObjLineIt.Key, ObjLineIt.Value)));
	}

	AirFlueBounle->BlockName = L"AirFlue_" + FString::FromInt(GetBlockValue(IBT_AirFlue));
	if (DrawComBlock(FrameIndex, LayerName, AirFlueBounle))
	{
		UpdateIncrementBlockCounter(IBT_AirFlue);
		return true;
	}
	else
	{
		return false;
	}
}

// 在指定的图框指定的图层上绘制标准门
bool "FArmyAutoCad::DrawSingleDoor(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmySingleDoor> InSingleDoor)
{
	//return DrawSingleDoor2(FrameIndex, LayerName, InSingleDoor);
	if (!InSingleDoor.IsValid())
	{
		return false;
	}

	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	TSharedPtr<"FArmyCadBundle> DoorBounle = MakeShareable(new "FArmyCadBundle());
	TArray<TPair<FVector, FVector>> ObjLineList;
	InSingleDoor->GetCadLineArray(ObjLineList);
	for (TPair<FVector, FVector> ObjLineIt : ObjLineList)
	{
		DoorBounle->AddLine(MakeShareable(new "FArmyLine(ObjLineIt.Key, ObjLineIt.Value)));
	}

	TSharedPtr<"FArmyArcLine> DoorArc = InSingleDoor->GetDoorArcLine();
	if (DoorArc.IsValid())
	{
		DoorBounle->AddArcLine(DoorArc, "DASHED2");
	}

	DoorBounle->BlockName = L"Door_" + FString::FromInt(GetBlockValue(IBT_Door));
	DoorBounle->IsDefaultDesc = false;
	AttributeDesc Desc = GetComAttributeDesc(LayerName);
	Desc.lineType = "ByBlock";
	DoorBounle->Desc = Desc;
	if (DrawComBlock(FrameIndex, LayerName, DoorBounle))
	{
		UpdateIncrementBlockCounter(IBT_Door);
		return true;
	}
	else
	{
		return false;
	}

}
bool "FArmyAutoCad::DrawSingleDoor2(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmySingleDoor> InSingleDoor)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;
	if (!InSingleDoor.IsValid())
		return false;
	FString Key = CalculateSingleDoorKey(InSingleDoor);
	float reverse = CalculateSingleDoorAngle(InSingleDoor);
	FVector basePoint = CalculateSingleDoorOffset(InSingleDoor);
	//draw ref
	BlockExt ext;
	if (!RepeatBlock.Contains(Key))
	{
		auto WindowBounle = GetCadBounleByDoor(InSingleDoor, basePoint, -reverse);
		WindowBounle->BlockName = Key;
		WindowBounle->BasePoint = basePoint;
		DrawComBlockWithoutRef(WindowBounle, ext);
		RepeatBlock.Add(Key, ext);
	}
	else
		ext.offset = basePoint;
	DrawComBlockRef(FrameIndex, LayerName, ext.blockName, GetCadPosOfFrame(ext.offset, FrameIndex), reverse);
	return true;
}
// 在指定的图框指定的图层上绘制推拉门
bool "FArmyAutoCad::DrawSlidingDoor(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmySlidingDoor> InSlidingDoor)
{
	//return DrawSlidingDoor2(FrameIndex, LayerName, InSlidingDoor);
	if (!InSlidingDoor.IsValid())
	{
		return false;
	}

	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	TSharedPtr<"FArmyCadBundle> DoorBounle = MakeShareable(new "FArmyCadBundle());
	TArray<TPair<FVector, FVector>> ObjLineList;
	InSlidingDoor->GetCadLineArray(ObjLineList);
	for (TPair<FVector, FVector> ObjLineIt : ObjLineList)
	{
		DoorBounle->AddLine(MakeShareable(new "FArmyLine(ObjLineIt.Key, ObjLineIt.Value)));
	}


	DoorBounle->BlockName = L"Door_" + FString::FromInt(GetBlockValue(IBT_Door));
	DoorBounle->IsDefaultDesc = false;
	AttributeDesc Desc = GetComAttributeDesc(LayerName);
	Desc.lineType = "ByBlock";
	DoorBounle->Desc = Desc;
	if (DrawComBlock(FrameIndex, LayerName, DoorBounle))
	{
		UpdateIncrementBlockCounter(IBT_Door);
		return true;
	}
	else
	{
		return false;
	}
}
bool "FArmyAutoCad::DrawSlidingDoor2(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmySlidingDoor> InSlidingDoor)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;
	if (!InSlidingDoor.IsValid())
		return false;
	FString Key = CalculateSlidingDoorKey(InSlidingDoor);
	float reverse = CalculateSlidingDoorAngle(InSlidingDoor);
	FVector basePoint = CalculateSlidingDoorOffset(InSlidingDoor);
	//draw ref
	BlockExt ext;
	if (!RepeatBlock.Contains(Key))
	{
		auto WindowBounle = GetCadBounleByWindows(InSlidingDoor, basePoint, -reverse);
		WindowBounle->BlockName = Key;
		WindowBounle->BasePoint = basePoint;
		DrawComBlockWithoutRef(WindowBounle, ext);
		RepeatBlock.Add(Key, ext);
	}
	else
		ext.offset = basePoint;
	DrawComBlockRef(FrameIndex, LayerName, ext.blockName, GetCadPosOfFrame(ext.offset, FrameIndex), reverse);
	return true;
}
// 在指定的图框指定的图层上绘制入户门
bool "FArmyAutoCad::DrawSecurityDoor(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmySecurityDoor> InSecurityDoor)
{
	//return DrawSecurityDoor2(FrameIndex, LayerName, InSecurityDoor);
	if (!InSecurityDoor.IsValid())
	{
		return false;
	}

	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	TSharedPtr<"FArmyCadBundle> DoorBounle = MakeShareable(new "FArmyCadBundle());
	TArray<TPair<FVector, FVector>> ObjLineList;
	InSecurityDoor->GetCadLineArray(ObjLineList);
	for (TPair<FVector, FVector> ObjLineIt : ObjLineList)
	{
		DoorBounle->AddLine(MakeShareable(new "FArmyLine(ObjLineIt.Key, ObjLineIt.Value)));
	}

	TSharedPtr<"FArmyArcLine> DoorArc = InSecurityDoor->GetDoorArcLine();
	if (DoorArc.IsValid())
	{
		DoorBounle->AddArcLine(DoorArc, "DASHED2");
	}

	DoorBounle->BlockName = L"Door_" + FString::FromInt(GetBlockValue(IBT_Door));
	DoorBounle->IsDefaultDesc = false;
	AttributeDesc Desc = GetComAttributeDesc(LayerName);
	Desc.lineType = "ByBlock";
	DoorBounle->Desc = Desc;
	if (DrawComBlock(FrameIndex, LayerName, DoorBounle))
	{
		UpdateIncrementBlockCounter(IBT_Door);
		return true;
	}
	else
	{
		return false;
	}
}
bool  "FArmyAutoCad::DrawSecurityDoor2(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmySecurityDoor> InSecurityDoor)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;
	if (!InSecurityDoor.IsValid())
		return false;
	FString Key = CalculateSecurityDoorKey(InSecurityDoor);
	float reverse = CalculateSecurityDoorAngle(InSecurityDoor);
	FVector basePoint = CalculateSecurityDoorOffset(InSecurityDoor);
	//draw ref
	BlockExt ext;
	if (!RepeatBlock.Contains(Key))
	{
		auto WindowBounle = GetCadBounleByDoor(InSecurityDoor, basePoint, -reverse);
		WindowBounle->BlockName = Key;
		WindowBounle->BasePoint = basePoint;
		DrawComBlockWithoutRef(WindowBounle, ext);
		RepeatBlock.Add(Key, ext);
	}
	else
		ext.offset = basePoint;


	DrawComBlockRef(FrameIndex, LayerName, Key, GetCadPosOfFrame(ext.offset, FrameIndex), reverse);
	return true;
}
// 在指定的图框指定的图层上绘制实体填充 
bool "FArmyAutoCad::DrawHatchSolid(const int32 FrameIndex, const FString LayerName, const TSharedPtr<"FArmyPolygon> &Polygon)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;
	TArray<"FArmyLine> LineList;
	int32 count = Polygon->Vertices.Num();
	for (int32 index = 0; index < count; index++)
	{
		LineList.Emplace("FArmyLine(Polygon->Vertices[index%count], Polygon->Vertices[(index + 1) % count]));
	}

	AttributeDesc Desc = GetComAttributeDesc(LayerName);
	AttributeEntity* 	Hacth = EntityFactory->createHatch(1, true, 100, 0, "0");
	Hacth->setEntityAttribute(Desc);
	IBlock*	HacthContainer = dynamic_cast<IBlock*>(Hacth);
	IBlock* block = EntityFactory->createBlockCanvas("HatchBlock", Double4());
	for (auto i : LineList)
	{
		block->drawEntity(EntityFactory->createLine(GetCadPosOfFrame(i.GetStart(), FrameIndex), GetCadPosOfFrame(i.GetEnd(), FrameIndex)));
	}
	HacthContainer->addBlock(block);
	Canvas->drawEntity(Hacth);
	return true;
}

// 在指定的图框指定的图层上绘制样式填充
bool "FArmyAutoCad::DrawHatch(const int32 FrameIndex, const FString LayerName, const FString&Style, double scale, const TSharedPtr<"FArmyPolygon> &Polygon)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	TArray<"FArmyLine> LineList;
	int32 count = Polygon->Vertices.Num();
	if (count < 2)
	{
		return false;
	}
	for (int32 index = 0; index < count; index++)
	{
		LineList.Emplace("FArmyLine(Polygon->Vertices[index%count], Polygon->Vertices[(index + 1) % count]));
	}

	AttributeDesc Desc = GetComAttributeDesc(LayerName);
	AttributeEntity* 	Hacth = EntityFactory->createHatch(1, false, scale, 0, TCHAR_TO_UTF8(*Style));
	Hacth->setEntityAttribute(Desc);
	IBlock*	HacthContainer = dynamic_cast<IBlock*>(Hacth);
	IBlock* block = EntityFactory->createBlockCanvas("HatchBlock", Double4());
	for (auto i : LineList)
	{
		block->drawEntity(EntityFactory->createLine(GetCadPosOfFrame(i.GetStart(), FrameIndex), GetCadPosOfFrame(i.GetEnd(), FrameIndex)));
	}
	HacthContainer->addBlock(block);
	Canvas->drawEntity(Hacth);
	return true;
}

//在指定的图框指定的图层上画线形成块
bool "FArmyAutoCad::DrawLinesAsBlock(const int32 FrameIndex, const FString LayerName, TArray<TSharedPtr<"FArmyLine>> &Lines)
{
	if (!Lines.Num()|| !CheckDraw(FrameIndex, LayerName))
		return false;
	TSharedPtr<"FArmyCadBundle>  Bounle = MakeShareable(new "FArmyCadBundle());
	for (auto line : Lines)
		Bounle->AddLine(line);
	Bounle->BlockName = L"chatch_" + FString::FromInt(GetBlockValue(IBT_CustomHatch));
	AttributeDesc Desc = GetComAttributeDesc(LayerName);
	if (!DrawComBlock(FrameIndex, LayerName, Bounle))
		return false;
	 UpdateIncrementBlockCounter(IBT_CustomHatch);
	 return true;
}

// 在指定的图框指定的图层上绘制样式填充
bool "FArmyAutoCad::DrawHatch(const int32 FrameIndex, const FString LayerName, const FString&Style, double scale, const TArray<TSharedPtr<"FArmyPolygon>> PolygonList)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	TArray<"FArmyLine> LineList;
	for (auto Polygon : PolygonList)
	{
		if(!Polygon.IsValid())
		{
			continue;
		}

		int32 count = Polygon->Vertices.Num();
		for (int32 index = 0; index < count; index++)
		{
			LineList.Emplace("FArmyLine(Polygon->Vertices[index%count], Polygon->Vertices[(index + 1) % count]));
		}
	}
	
	AttributeDesc Desc = GetComAttributeDesc(LayerName);
	AttributeEntity* 	Hacth = EntityFactory->createHatch(1, false, scale, 0, TCHAR_TO_UTF8(*Style));
	Hacth->setEntityAttribute(Desc);
	IBlock*	HacthContainer = dynamic_cast<IBlock*>(Hacth);
	IBlock* block = EntityFactory->createBlockCanvas("HatchBlock", Double4());
	for (auto i : LineList)
	{
		block->drawEntity(EntityFactory->createLine(GetCadPosOfFrame(i.GetStart(), FrameIndex), GetCadPosOfFrame(i.GetEnd(), FrameIndex)));
	}
	HacthContainer->addBlock(block);
	Canvas->drawEntity(Hacth);
	return true;
}
// 在指定的图框指定的图层上绘制标注
bool "FArmyAutoCad::DrawDimensions(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyDimensions> InDimensions)
{
	if (!InDimensions.IsValid())
	{
		return false;
	}

	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	const FVector RightPos = InDimensions->RightExtentPoint->GetPos() + (InDimensions->RightStaticPoint->GetPos() - InDimensions->RightExtentPoint->GetPos()).GetSafeNormal()*15.0f;
	const FVector LeftPos = InDimensions->LeftExtentPoint->GetPos() + (InDimensions->LeftStaticPoint->GetPos() - InDimensions->LeftExtentPoint->GetPos()).GetSafeNormal()*15.0f;
	return DrawAlignedDimension(FrameIndex, LayerName, LeftPos, RightPos, InDimensions->RightExtentPoint->GetPos());
}

// 在指定的图框指定的图层上绘制文本
bool "FArmyAutoCad::DrawTextLabel(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyTextLabel> InTextLabel)
{
	if (!InTextLabel.IsValid())
	{
		return false;
	}

	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	

	// 绘制文字
	AttributeDesc Desc = GetComAttributeDesc(LayerName);
	MTextDesc textDesc;
	textDesc.alignmentPoint = GetCadPosOfFrame(InTextLabel->GetTextPosition(), FrameIndex);
	textDesc.drawDirection = LeftToRight;
	textDesc.hAlign = HAlignCenter;
	textDesc.vAlign = VAlignTop;
	textDesc.lineSpaceStyle = Exact;

	textDesc.text = CorrectString(Utf82Gbk(TCHAR_TO_UTF8(*InTextLabel->GetLabelContent().ToString())));
	textDesc.angle = 0;
	textDesc.style = "standard";
	textDesc.width = 1;
	textDesc.height = GetCadLength(InTextLabel->GetTextSize());
	textDesc.lineSpacingFactor = 1.;

	AttributeEntity* 	textEntity = EntityFactory->createMText(textDesc);
	textEntity->setEntityAttribute(Desc);
	Canvas->drawEntity(textEntity);


	FString blockName = L"Label_" + FString::FromInt(GetBlockValue(IBT_Lable));
	IBlock*  Comblock = EntityFactory->createBlockCanvas(Utf82Gbk(TCHAR_TO_UTF8(*blockName)).c_str(), Double4());

	// 创建线段
	TArray<TSharedPtr<"FArmyLine>> LineArray;
    InTextLabel->GetDrawLines().GenerateValueArray(LineArray);
	bool LineFlag = false;
	for (auto It : LineArray)
	{
		Double4 StartPoint = GetCadPosOfFrame(It->GetStart(), FrameIndex);
		Double4 EndPoint = GetCadPosOfFrame(It->GetEnd(), FrameIndex);
		auto s_entity = EntityFactory->createLine(StartPoint, EndPoint);
		s_entity->setEntityAttribute(Desc);
		Comblock->drawEntity(s_entity);
		LineFlag = true;
	}

	if (LineFlag)
	{
		Canvas->addBlock(Comblock);
		auto Ref = EntityFactory->createBlockReference(Utf82Gbk(TCHAR_TO_UTF8(*blockName)).c_str(), Double4(), VectorHelper(FVector::OneVector).asDouble3(), 0, 1, 1, 0, 0);
		Ref->setEntityAttribute(Desc);
		Canvas->drawEntity(Ref);
		UpdateIncrementBlockCounter(IBT_Lable);
	}
	
	return true;

}

// 在指定的图框指定的图层上绘制手动标注
bool "FArmyAutoCad::DrawDownLeadLabel(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyDownLeadLabel> InDownLeadLabel)
{
	if (!InDownLeadLabel.IsValid())
	{
		return false;
	}

	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	//获取标注线段和文字，找到公共点
	TSharedPtr<"FArmyLine> LeadLine = InDownLeadLabel->GetLeadLine();
	TSharedPtr<"FArmyLine> MainLine = InDownLeadLabel->GetMainLine();
	auto Text = CorrectString(Utf82Gbk(TCHAR_TO_UTF8(*InDownLeadLabel->GetLabelContent().ToString())));

	FVector Start = FVector::ZeroVector;
	FVector Inflection = FVector::ZeroVector;
	FVector Base = FVector::ZeroVector;

	FVector LeadLineBeg = LeadLine->GetStart();
	FVector LeadLineEnd = LeadLine->GetEnd();
	FVector MainLineBeg = MainLine->GetStart();
	FVector MainLineEnd = MainLine->GetEnd();
	if (LeadLineBeg == MainLineBeg)
	{
		Base = MainLineEnd;
		Inflection = LeadLineBeg;
		Start = LeadLineEnd;
	}
	else if (LeadLineBeg == MainLineEnd)
	{
		Base = MainLineBeg;
		Inflection = LeadLineBeg;
		Start = LeadLineEnd;
	}
	else if (LeadLineEnd == MainLineBeg)
	{
		Base = MainLineEnd;
		Inflection = LeadLineEnd;
		Start = LeadLineBeg;
	}
	else if (LeadLineEnd == MainLineEnd)
	{
		Base = MainLineBeg;
		Inflection = LeadLineEnd;
		Start = LeadLineBeg;
	}

	//转换到局部坐标
	Double4 LocalBeg = GetCadPosOfFrame(Start, FrameIndex);
	Double4 LocalInflection = GetCadPosOfFrame(Inflection, FrameIndex);
	Double4 LocalBase = GetCadPosOfFrame(Base, FrameIndex);
	double   LocalFontSize = (double)GetCadLength(InDownLeadLabel->GetTextSize());
	Double4	TextCentre = GetCadPosOfFrame(InDownLeadLabel->GetTextLocation(), FrameIndex);
	FVector   Direction = InDownLeadLabel->GetTextDirection();
	Direction.Normalize();
	Double4 TextDirection2 = Double4(Direction.X, -Direction.Y, Direction.Z);


	//标注实体
	AttributeEntity *DimMLeader = EntityFactory->createDimMLeader(LocalBase, LocalInflection, LocalBeg, TextCentre, TextDirection2,LocalFontSize, 10, Text.c_str());
	AttributeDesc Desc = GetComAttributeDesc(LayerName);
	DimMLeader->setEntityAttribute(Desc);

	Canvas->drawEntity(DimMLeader);
	return true;
}
// 绘制指北针
bool "FArmyAutoCad::DrawCompass(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyCompass> ComPass)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	if (!ComPass.IsValid())
	{
		return false;
	}

	FVector BasePoint = ComPass->GetCircleInfo()->Position;
	// 在自定义块中是否存在
	if (!DefineBlockMap.Contains(DBT_Compass))
	{
		// 创建块
		FString blockName = L"Compass";
		auto newBlock = EntityFactory->createBlockCanvas(TCHAR_TO_UTF8(*blockName), Double4());

		// 画圆
		TSharedPtr<"FArmyCircle> Circle = ComPass->GetCircleInfo();
		AttributeDesc   Desc;
		FMemory::Memzero(&Desc, sizeof(AttributeDesc));
		Desc.red = DefaultColor.R;
		Desc.green = DefaultColor.G;
		Desc.blue = DefaultColor.B;
		Desc.lineType = "CONTINUOUS";
		Desc.weight = Weight000;
		auto attributeEntity = EntityFactory->createCircle(GetCadPos(Circle->Position - BasePoint), GetCadLength(Circle->Radius));
		attributeEntity->setEntityAttribute(Desc);
		newBlock->drawEntity(attributeEntity);

		// 画填充多边形
		TArray<"FArmyLine> LineList;
		int32 count = ComPass->GetArrows()->Vertices.Num();
		for (int32 index = 0; index < count; index++)
		{
			FVector StartPoint = ComPass->GetArrows()->Vertices[index%count] - BasePoint;
			FVector EndPoint = ComPass->GetArrows()->Vertices[(index + 1) % count] - BasePoint;
			LineList.Emplace("FArmyLine(StartPoint, EndPoint));
		}
		auto Hacth = EntityFactory->createHatch(1, true, 100, 0, "0");
		Hacth->setEntityAttribute(Desc);
		IBlock*	HacthContainer = dynamic_cast<IBlock*>(Hacth);
		IBlock* block = EntityFactory->createBlockCanvas("HatchBlock", Double4());
		for (auto i : LineList)
		{
			block->drawEntity(EntityFactory->createLine(GetCadPos(i.GetStart()), GetCadPos(i.GetEnd())));
		}
		HacthContainer->addBlock(block);
		newBlock->drawEntity(Hacth);

		// 画文字
		FScaleCanvasText CanvasText = ComPass->GetCanvasText();
		MTextDesc textDesc;
		float TextHeight = GetCadLength(CanvasText.GetScaleTextSize() * CanvasText.BaseScale.Y);
		FVector Pos = CanvasText.GetPosition() - BasePoint;
		textDesc.alignmentPoint = GetCadPos(Pos);
		textDesc.drawDirection = LeftToRight;
		textDesc.hAlign = HAlignLeft;
		textDesc.vAlign = VAlignTop;
		textDesc.lineSpaceStyle = Exact;
		textDesc.text = CorrectString(Utf82Gbk(TCHAR_TO_UTF8(*CanvasText.Text.ToString())));
		textDesc.angle = 0;
		textDesc.style = "standard";
		textDesc.width = 100;
		textDesc.height = TextHeight;
		textDesc.lineSpacingFactor = 1.;

		AttributeEntity* 	textEntity = EntityFactory->createMText(textDesc);
		textEntity->setEntityAttribute(Desc);
		newBlock->drawEntity(textEntity);

		Canvas->addBlock(newBlock);
		BlockExt CompassBlock(newBlock, FVector(0, 0, 0), blockName);
		DefineBlockMap.Add(DBT_Compass, CompassBlock);

	}

	BlockExt CompassBlock = *DefineBlockMap.Find(DBT_Compass);
	auto blockRef = EntityFactory->createBlockReference(TCHAR_TO_UTF8(*CompassBlock.blockName), GetCadPosOfFrame(BasePoint, FrameIndex), VectorHelper(FVector::OneVector).asDouble3(), 0, 1, 1, 0, 0);

	AttributeDesc Desc = GetComAttributeDesc(LayerName);
	blockRef->setEntityAttribute(Desc);
	Canvas->drawEntity(blockRef);

 
	return true;

}

/**
* 在指定的图框指定的图层上绘制入户门标志
* @param FrameIndex - const int32 - 图框名称
* @param LayerName - const FString - 图层名称
* @param Entrance -  const TSharedPtr<class "FArmyEntrance> - 指北针对象
* @return bool -  true 表示绘制成功
*/
bool "FArmyAutoCad::DrawEntrance(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyEntrance> Entrance)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	if (!Entrance.IsValid())
	{
		return false;
	}

	FVector BasePoint = Entrance->GetOriginPosition();
	FTransform Transform = Entrance->GetTransform();
	// 在自定义块中是否存在
	if (!DefineBlockMap.Contains(DBT_Entrance))
	{
		// 创建块
		FString blockName = L"Entrance";
		IBlock*  newBlock = EntityFactory->createBlockCanvas(Utf82Gbk(TCHAR_TO_UTF8(*blockName)).c_str(), Double4());
		AttributeDesc   Desc;
		FMemory::Memzero(&Desc, sizeof(AttributeDesc));
		Desc.red = DefaultColor.R;
		Desc.green = DefaultColor.G;
		Desc.blue = DefaultColor.B;
		Desc.lineType = "CONTINUOUS";
		Desc.weight = Weight000;
		Desc.colorType = ByBlock;

		// 绘制线段
		TArray<TSharedPtr<"FArmyLine>> LineArray;
		Entrance->GetOriginBoundLineList(LineArray);
		for (auto It : LineArray)
		{
			FVector StartPoint = It->GetStart() - BasePoint;
			FVector EndPoint = It->GetEnd() - BasePoint;

			AttributeEntity* entity = EntityFactory->createLine(GetCadPos(StartPoint), GetCadPos(EndPoint));
			entity->setEntityAttribute(Desc);
			newBlock->drawEntity(entity);
		}
		// 绘制填充
		TArray<TPair<bool, TSharedPtr<class "FArmyPolygon>>> PolygonMap;
		Entrance->GetOriginPolygon(PolygonMap);
		for (auto It : PolygonMap)
		{
			if (It.Key == true)
			{
				TArray<"FArmyLine> LineList;
				int32 count = It.Value->Vertices.Num();
				for (int32 index = 0; index < count; index++)
				{
					FVector StartPoint = It.Value->Vertices[index%count] - BasePoint;
					FVector EndPoint = It.Value->Vertices[(index + 1) % count] - BasePoint;

					LineList.Emplace("FArmyLine(StartPoint, EndPoint));
				}
				AttributeEntity* 	Hacth = EntityFactory->createHatch(1, true, 100, 0, "0");
				Hacth->setEntityAttribute(Desc);
				IBlock*	HacthContainer = dynamic_cast<IBlock*>(Hacth);
				IBlock* block = EntityFactory->createBlockCanvas("HatchBlock", Double4());
				for (auto i : LineList)
				{
					block->drawEntity(EntityFactory->createLine(GetCadPos(i.GetStart()), GetCadPos(i.GetEnd())));
				}
				HacthContainer->addBlock(block);
				newBlock->drawEntity(Hacth);
				break;
			}
		}

		Canvas->addBlock(newBlock);
		BlockExt EntranceBlock(newBlock, FVector(0, 0, 0), blockName);
		DefineBlockMap.Add(DBT_Entrance, EntranceBlock);

	}
	
	float Angle = Transform.GetRotation().GetAngle();
	if (Transform.GetRotation().Z > 0)
	{
		Angle = Angle *-1;
	}

	BlockExt EntranceBlock = *DefineBlockMap.Find(DBT_Entrance);
	auto blockRef = EntityFactory->createBlockReference(TCHAR_TO_UTF8(*EntranceBlock.blockName), GetCadPosOfFrame(Transform.GetLocation(), FrameIndex), VectorHelper(Transform.GetScale3D()).asDouble3(), Angle, 1, 1, 0, 0);

	AttributeDesc Desc = GetComAttributeDesc(LayerName);
	blockRef->setEntityAttribute(Desc);
	Canvas->drawEntity(blockRef);

	return true;
}

// 在指定的图框指定的图层上绘制台面割线
bool "FArmyAutoCad::DrawBoardSplitline(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyBoardSplitline> BoardSplitline)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	if (!BoardSplitline.IsValid())
	{
		return false;
	}

	TSharedPtr<"FArmyCadBundle> SplitlineBounle = MakeShareable(new "FArmyCadBundle());
	TArray<TPair<FVector, FVector>> ObjLineList;
	BoardSplitline->GetCadLineArray(ObjLineList);
	for (TPair<FVector, FVector> ObjLineIt : ObjLineList)
	{
		SplitlineBounle->AddLine(MakeShareable(new "FArmyLine(ObjLineIt.Key, ObjLineIt.Value)));
	}
	SplitlineBounle->BlockName = L"Splitline_" + FString::FromInt(GetBlockValue(IBT_BoardSpliteline));
	if (DrawComBlock(FrameIndex, LayerName, SplitlineBounle))
	{
		UpdateIncrementBlockCounter(IBT_BoardSpliteline);
		return true;
	}
	else
	{
		return false;
	}
}

// 在指定的图框指定的图层上绘制包立管
bool "FArmyAutoCad::DrawPackPipe(const int32 FrameIndex, const FString LayerName, const TSharedPtr <class "FArmyPackPipe>PackPipe)
{
	if (!PackPipe.IsValid())
		return false;
	TArray<TSharedPtr<"FArmyLine>> Lines;
	PackPipe->GetLines(Lines);
	BlockExt blockExt;

	if (!RepeatBlock.Contains(PackPipe->GetUniqueID().ToString()))
	{
		FVector minPoint=FVector(FLT_MAX, FLT_MAX, FLT_MAX);
		FVector maxPoint = -minPoint;
		for (auto line : Lines) {
			minPoint = minPoint.ComponentMin(line->GetStart());
			minPoint = minPoint.ComponentMin(line->GetEnd());
			maxPoint = maxPoint.ComponentMax(line->GetStart());
			maxPoint = maxPoint.ComponentMax(line->GetEnd());
		}
    	FVector MidPoint = (maxPoint + minPoint)/2.f;
		TSharedPtr<class "FArmyCadBundle>  CadBounle = MakeShareable(new "FArmyCadBundle());
		for (auto &line : Lines) {
			TSharedPtr<"FArmyLine> newLine = MakeShareable(new "FArmyLine(line->GetStart() - MidPoint, line->GetEnd() - MidPoint));
			 CadBounle->AddLine(newLine);
		}
		CadBounle->BlockName = L"ContainerPipe" + FString::FromInt(GetBlockValue(IBT_PackPipe));
		CadBounle->BasePoint = MidPoint;
		UpdateIncrementBlockCounter(IBT_PackPipe);
		DrawComBlockWithoutRef(CadBounle,blockExt);
		RepeatBlock.Add(PackPipe->GetUniqueID().ToString(), blockExt);
	}
	else
	{
		blockExt = RepeatBlock[PackPipe->GetUniqueID().ToString()];
	}
	if (!DrawComBlockRef(FrameIndex, LayerName, blockExt.blockName, GetCadPosOfFrame(blockExt.offset, FrameIndex), 0))
		return false;
	return  true;
}

bool "FArmyAutoCad::DrawPassEdge(const int32 FrameIndex, const FString LayerName, const TSharedPtr <class "FArmyPass>Pass)
{
	if (!CheckDraw(FrameIndex, LayerName)|| !Pass.IsValid())
		return false;
 
	TSharedPtr<"FArmyPolyline> StartPass = Pass->GetStartPass();
	auto  StartVectices = StartPass->GetAllVertices();
	FVector StartPassPovitPoint = StartPass->GetBasePoint();
 
	FString Name1 = L"PassEdgeL" + Pass->GetUniqueID().ToString();
	BlockExt blockExt;
	if (!RepeatBlock.Contains(Name1)) {
		TSharedPtr<"FArmyCadBundle> Begin = MakeShareable(new "FArmyCadBundle);
		if (StartVectices.Num() >= 2)
		{
			if (StartPass->GetLineType() == "FArmyPolyline::ELineType::LineStrip)
			{
				for (int i = 1; i < StartVectices.Num(); i++)
				{
					auto Line = MakeShareable(new "FArmyLine(StartPass->GetTransform().TransformPosition(StartVectices[i - 1] - StartPassPovitPoint),
						StartPass->GetTransform().TransformPosition(StartVectices[i] - StartPassPovitPoint))
					);
					Begin->AddLine(Line);
				}
			}
			else if (StartPass->GetLineType() == "FArmyPolyline::ELineType::Lines)
			{
				for (int i = 1; i < StartVectices.Num(); i += 2)
				{
					auto Line = MakeShareable(new "FArmyLine(StartPass->GetTransform().TransformPosition(StartVectices[i - 1] - StartPassPovitPoint),
						StartPass->GetTransform().TransformPosition(StartVectices[i] - StartPassPovitPoint))
					);
					Begin->AddLine(Line);
				}
			}
			else if (StartPass->GetLineType() == "FArmyPolyline::ELineType::Line_Loop)
			{
				int32 Size = StartVectices.Num();
				for (int i = 0; i < Size; i++)
				{
					int IndexNext = (i == Size - 1) ? (i + 1) % Size : i + 1;
					auto Line = MakeShareable(new "FArmyLine(StartPass->GetTransform().TransformPosition(StartVectices[i] - StartPassPovitPoint),
						StartPass->GetTransform().TransformPosition(StartVectices[IndexNext] - StartPassPovitPoint))
					);
					Begin->AddLine(Line);
				}
			}
		}
		Begin->BlockName = L"PassEdge" + FString::FromInt(GetBlockValue(IBT_PassEdge));
		UpdateIncrementBlockCounter(IBT_PassEdge);
		Begin->BasePoint = FVector::ZeroVector;	 
		DrawComBlockWithoutRef(Begin, blockExt);
		RepeatBlock.Add(Name1, blockExt);
	}
	else
	{
		blockExt = RepeatBlock[Name1];
	}
	if (!DrawComBlockRef(FrameIndex, LayerName, blockExt.blockName, GetCadPosOfFrame(blockExt.offset, FrameIndex), 0))
		return false;

	TSharedPtr<"FArmyPolyline> EndPass = Pass->GetEndPass();
	auto  EndVectices = EndPass->GetAllVertices();
	FVector EndPassPovitPoint = StartPass->GetBasePoint();
	FString Name2 = L"PassEdgeR" + Pass->GetUniqueID().ToString();
 
	if (!RepeatBlock.Contains(Name2)) {
		TSharedPtr<"FArmyCadBundle> Begin = MakeShareable(new "FArmyCadBundle);
		if (EndVectices.Num() >= 2)
		{
			if (EndPass->GetLineType() == "FArmyPolyline::ELineType::LineStrip)
			{
				for (int i = 1; i < EndVectices.Num(); i++)
				{
					auto Line = MakeShareable(new "FArmyLine(EndPass->GetTransform().TransformPosition(EndVectices[i - 1] - EndPassPovitPoint),
						EndPass->GetTransform().TransformPosition(EndVectices[i] - EndPassPovitPoint))
					);
					Begin->AddLine(Line);
				}
			}
			else if (EndPass->GetLineType() == "FArmyPolyline::ELineType::Lines)
			{
				for (int i = 1; i < EndVectices.Num(); i += 2)
				{
					auto Line = MakeShareable(new "FArmyLine(EndPass->GetTransform().TransformPosition(EndVectices[i - 1] - EndPassPovitPoint),
						EndPass->GetTransform().TransformPosition(EndVectices[i] - EndPassPovitPoint))
					);
					Begin->AddLine(Line);
				}
			}
			else if (EndPass->GetLineType() == "FArmyPolyline::ELineType::Line_Loop)
			{
				int32 Size = EndVectices.Num();
				for (int i = 0; i < Size; i++)
				{
					int IndexNext = (i == Size - 1) ? (i + 1) % Size : i + 1;
					auto Line = MakeShareable(new "FArmyLine(EndPass->GetTransform().TransformPosition(EndVectices[i] - EndPassPovitPoint),
						EndPass->GetTransform().TransformPosition(EndVectices[IndexNext] - EndPassPovitPoint))
					);
					Begin->AddLine(Line);
				}
			}
		}
		Begin->BlockName = L"PassEdge" + FString::FromInt(GetBlockValue(IBT_PassEdge));
		UpdateIncrementBlockCounter(IBT_PassEdge);
		Begin->BasePoint = FVector::ZeroVector;
		DrawComBlockWithoutRef(Begin, blockExt);
		RepeatBlock.Add(Name2, blockExt);
	}
	else
	{
		blockExt = RepeatBlock[Name2];
	}
	if (!DrawComBlockRef(FrameIndex, LayerName, blockExt.blockName, GetCadPosOfFrame(blockExt.offset, FrameIndex), 0))
		return false;
	return true;
}
 
bool "FArmyAutoCad::DrawNewPassEdge(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyNewPass> NewPass)
{
	if (!CheckDraw(FrameIndex, LayerName) || !NewPass.IsValid())
		return false;

	TSharedPtr<"FArmyPolyline> StartPass = NewPass->GetStartPass();
	auto  StartVectices = StartPass->GetAllVertices();
	FVector StartPassPovitPoint = StartPass->GetBasePoint();

	FString Name1 = L"NewPassEdgeL" + NewPass->GetUniqueID().ToString();
	BlockExt blockExt;
	if (!RepeatBlock.Contains(Name1)) {
		TSharedPtr<"FArmyCadBundle> Begin = MakeShareable(new "FArmyCadBundle);
		if (StartVectices.Num() >= 2)
		{
			if (StartPass->GetLineType() == "FArmyPolyline::ELineType::LineStrip)
			{
				for (int i = 1; i < StartVectices.Num(); i++)
				{
					auto Line = MakeShareable(new "FArmyLine(StartPass->GetTransform().TransformPosition(StartVectices[i - 1] - StartPassPovitPoint),
						StartPass->GetTransform().TransformPosition(StartVectices[i] - StartPassPovitPoint))
					);
					Begin->AddLine(Line);
				}
			}
			else if (StartPass->GetLineType() == "FArmyPolyline::ELineType::Lines)
			{
				for (int i = 1; i < StartVectices.Num(); i += 2)
				{
					auto Line = MakeShareable(new "FArmyLine(StartPass->GetTransform().TransformPosition(StartVectices[i - 1] - StartPassPovitPoint),
						StartPass->GetTransform().TransformPosition(StartVectices[i] - StartPassPovitPoint))
					);
					Begin->AddLine(Line);
				}
			}
			else if (StartPass->GetLineType() == "FArmyPolyline::ELineType::Line_Loop)
			{
				int32 Size = StartVectices.Num();
				for (int i = 0; i < Size; i++)
				{
					int IndexNext = (i == Size - 1) ? (i + 1) % Size : i + 1;
					auto Line = MakeShareable(new "FArmyLine(StartPass->GetTransform().TransformPosition(StartVectices[i] - StartPassPovitPoint),
						StartPass->GetTransform().TransformPosition(StartVectices[IndexNext] - StartPassPovitPoint))
					);
					Begin->AddLine(Line);
				}
			}
		}
		Begin->BlockName = L"NewPassEdge" + FString::FromInt(GetBlockValue(IBT_PassEdge));
		UpdateIncrementBlockCounter(IBT_PassEdge);
		Begin->BasePoint = FVector::ZeroVector;
		DrawComBlockWithoutRef(Begin, blockExt);
		RepeatBlock.Add(Name1, blockExt);
	}
	else
	{
		blockExt = RepeatBlock[Name1];
	}
	auto Ref1 = EntityFactory->createBlockReference(Utf82Gbk(TCHAR_TO_UTF8(*blockExt.blockName)).c_str(), GetCadPosOfFrame(blockExt.offset, FrameIndex), VectorHelper(FVector::OneVector).asDouble3(), 0, 1, 1, 0, 0);
	AttributeDesc Desc1 = GetComAttributeDesc(LayerName);
	Ref1->setEntityAttribute(Desc1);
	Canvas->drawEntity(Ref1);

	TSharedPtr<"FArmyPolyline> EndPass = NewPass->GetEndPass();
	auto  EndVectices = EndPass->GetAllVertices();
	FVector EndPassPovitPoint = StartPass->GetBasePoint();
	FString Name2 = L"NewPassEdgeR" + NewPass->GetUniqueID().ToString();
	 
	if (!RepeatBlock.Contains(Name2)) {
		TSharedPtr<"FArmyCadBundle> Begin = MakeShareable(new "FArmyCadBundle);
		if (EndVectices.Num() >= 2)
		{
			if (EndPass->GetLineType() == "FArmyPolyline::ELineType::LineStrip)
			{
				for (int i = 1; i < EndVectices.Num(); i++)
				{
					auto Line = MakeShareable(new "FArmyLine(EndPass->GetTransform().TransformPosition(EndVectices[i - 1] - EndPassPovitPoint),
						EndPass->GetTransform().TransformPosition(EndVectices[i] - EndPassPovitPoint))
					);
					Begin->AddLine(Line);
				}
			}
			else if (EndPass->GetLineType() == "FArmyPolyline::ELineType::Lines)
			{
				for (int i = 1; i < EndVectices.Num(); i += 2)
				{
					auto Line = MakeShareable(new "FArmyLine(EndPass->GetTransform().TransformPosition(EndVectices[i - 1] - EndPassPovitPoint),
						EndPass->GetTransform().TransformPosition(EndVectices[i] - EndPassPovitPoint))
					);
					Begin->AddLine(Line);
				}
			}
			else if (EndPass->GetLineType() == "FArmyPolyline::ELineType::Line_Loop)
			{
				int32 Size = EndVectices.Num();
				for (int i = 0; i < Size; i++)
				{
					int IndexNext = (i == Size - 1) ? (i + 1) % Size : i + 1;
					auto Line = MakeShareable(new "FArmyLine(EndPass->GetTransform().TransformPosition(EndVectices[i] - EndPassPovitPoint),
						EndPass->GetTransform().TransformPosition(EndVectices[IndexNext] - EndPassPovitPoint))
					);
					Begin->AddLine(Line);
				}
			}
		}
		Begin->BlockName = L"NewPassEdge" + FString::FromInt(GetBlockValue(IBT_PassEdge));
		UpdateIncrementBlockCounter(IBT_PassEdge);
		Begin->BasePoint = FVector::ZeroVector;
		DrawComBlockWithoutRef(Begin, blockExt);
		RepeatBlock.Add(Name2, blockExt);
	}
	else
	{
		blockExt = RepeatBlock[Name2];
	}
	auto Ref2 = EntityFactory->createBlockReference(Utf82Gbk(TCHAR_TO_UTF8(*blockExt.blockName)).c_str(), GetCadPosOfFrame(blockExt.offset, FrameIndex), VectorHelper(FVector::OneVector).asDouble3(), 0, 1, 1, 0, 0);
	AttributeDesc Desc2 = GetComAttributeDesc(LayerName);
	Ref2->setEntityAttribute(Desc2);
	Canvas->drawEntity(Ref2);
	return true;
}

// 在指定的图框指定的图层上绘制灯控线
bool "FArmyAutoCad::DrawLampControlLines(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyLampControlLines> LampControlLines)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	if (!LampControlLines.IsValid())
	{
		return false;
	}

	TArray<TSharedPtr<"FArmyArcLine>>  ArcLines;
	LampControlLines->GetArcLines(ArcLines);
	for (auto &Arc : ArcLines)
	{
		FVector Angle = CalculateArcAngular(Arc->GetPosition(), Arc->GetStartPos(), Arc->GetEndPos());
		FVector Beg = Arc->GetStartPos() - Arc->GetPosition();
		FVector End = Arc->GetEndPos() - Arc->GetPosition();
		if (FVector::CrossProduct(Beg, End).Z < 0)
			Swap(Angle.X, Angle.Y);
		AttributeEntity* entity = EntityFactory->createArc(GetCadPosOfFrame(Arc->GetPosition(), FrameIndex), GetCadLength(Arc->GetRadius()), Angle.X, Angle.Y);
		AttributeDesc Desc = GetComAttributeDesc(LayerName);
		entity->setEntityAttribute(Desc);
		Canvas->drawEntity(entity);
	}
	return true;
}

/**
* 在指定的图框指定的图层上绘制开阳台
* @param FrameIndex - const int32 - 图框名称
* @param LayerName - const FString - 图层名称
* @param InPunch -  const TSharedPtr<class "FArmyPunch> - 开阳台
* @return bool -  true 表示绘制成功DrawComBlockWithoutRef
*/
bool "FArmyAutoCad::DrawPunch(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyPunch> InPunch)
{
	if (!InPunch.IsValid())
	{
		return false;
	}

	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	TSharedPtr<"FArmyCadBundle> PunchBounle = MakeShareable(new "FArmyCadBundle());
	TArray<TPair<FVector, FVector>> ObjLineList;
	InPunch->GetDrawCadLines(ObjLineList);
	for (TPair<FVector, FVector> ObjLineIt : ObjLineList)
	{
		PunchBounle->AddLine(MakeShareable(new "FArmyLine(ObjLineIt.Key, ObjLineIt.Value)));
	}

	PunchBounle->BlockName = L"Punch_" + FString::FromInt(GetBlockValue(IBT_Punch));
	if (DrawComBlock(FrameIndex, LayerName, PunchBounle))
	{
		UpdateIncrementBlockCounter(IBT_Punch);
		return true;
	}
	else
	{
		return false;
	}
}

/**
* 在指定的图框指定的图层上厨柜索引标志
* @param FrameIndex - const int32 - 图框名称
* @param LayerName - const int32 - 图层名称
* @param InAirFlue - const TSharedPtr<"FArmySymbol>  - 索引
* @return bool - true 表示绘制成功
*/
bool "FArmyAutoCad::DrawSymbol(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmySymbol>  InSymbol)
{
	if (!InSymbol.IsValid())
	{
		return false;
	}

	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	
	TSharedPtr<"FArmyCadBundle> SymbolBounle = MakeShareable(new "FArmyCadBundle());
	// 绘制线段
	if (InSymbol->GetIsDrawArrows())
	{
		TArray<TPair<FVector, FVector>> ObjLineList;
		InSymbol->GetDrawCadLines(ObjLineList);
		for (TPair<FVector, FVector> ObjLineIt : ObjLineList)
		{
			SymbolBounle->AddLine(MakeShareable(new "FArmyLine(ObjLineIt.Key, ObjLineIt.Value)));
		}
	}

	// 绘制圆
	TSharedPtr<"FArmyCircle> SymbolCircle = InSymbol->GetDrawCadCircle();
	if (SymbolCircle.IsValid())
	{
		SymbolBounle->AddCircle(SymbolCircle);
	}

	// 绘制文字
	FScaleCanvasText CanvasText = InSymbol->GetCanvasText();
	SymbolBounle->AddCanvasText(CanvasText);

	SymbolBounle->BlockName = L"Symbol_" + FString::FromInt(GetBlockValue(IBT_Symbol));
	if (DrawComBlock(FrameIndex, LayerName, SymbolBounle))
	{
		UpdateIncrementBlockCounter(IBT_Symbol);
		return true;
	}
	else
	{
		return false;
	}
}

/**
* 在指定的图框指定的图层上绘制柜体
* @param FrameIndex - const int32 - 图框名称
* @param LayerName - const int32 - 图层名称
* @param InAirFlue - const TSharedPtr<"FArmySymbol>  - 索引
* @return bool - true 表示绘制成功
*/
bool "FArmyAutoCad::DrawCabinet(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyFurniture>  CabinetFurniture)
{
	if (!CabinetFurniture.IsValid())
	{
		return false;
	}

	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	int count = CabinetFurniture->GetElementVertex().Num();
	FString blockName = L"Cabinet_" + FString::FromInt(GetBlockValue(IBT_Cabinet));
	IBlock*  Cabinetblock = EntityFactory->createBlockCanvas(Utf82Gbk(TCHAR_TO_UTF8(*blockName)).c_str(), Double4());
	FVector PivotPoint = CabinetFurniture->GetPivortPoint();
	for (auto It : CabinetFurniture->GetPolys())
	{
		int count = It->GetAllVertices().Num();
		if (count < 2)
		{
			continue;
		}

		AttributeDesc   Desc = GetComAttributeDesc(LayerName);
		if (It->GetLineType() == 1)
		{
			for (int i = 1; i < count; i++)
			{
				FVector StartPoint = It->GetAllVertices()[i - 1] - PivotPoint;
				FVector EndPoint = It->GetAllVertices()[i] - PivotPoint;
				auto attributeEntity = EntityFactory->createLine(GetCadPos(StartPoint), GetCadPos(EndPoint));
				attributeEntity->setEntityAttribute(Desc);
				Cabinetblock->drawEntity(attributeEntity);
			}
		}
		else if (It->GetLineType() == 0)
		{
			for (int i = 1; i < count; i += 2)
			{
				FVector StartPoint = It->GetAllVertices()[i - 1] - PivotPoint;
				FVector EndPoint = It->GetAllVertices()[i] - PivotPoint;
				auto attributeEntity = EntityFactory->createLine(GetCadPos(StartPoint), GetCadPos(EndPoint));
				attributeEntity->setEntityAttribute(Desc);
				Cabinetblock->drawEntity(attributeEntity);
			}
		}
		else if (It->GetLineType() == 2)
		{
			for (int i = 0; i < count; i++)
			{
				FVector StartPoint = It->GetAllVertices()[i%count] - PivotPoint;
				FVector EndPoint = It->GetAllVertices()[(i + 1) % count] - PivotPoint;
				auto attributeEntity = EntityFactory->createLine(GetCadPos(StartPoint), GetCadPos(EndPoint));
				attributeEntity->setEntityAttribute(Desc);
				Cabinetblock->drawEntity(attributeEntity);
			}
		}
	}

	Canvas->addBlock(Cabinetblock);
	auto CadPos = GetCadPosOfFrame(CabinetFurniture->LocalTransform.GetLocation(), FrameIndex);
	FVector Scale = CabinetFurniture->LocalTransform.GetScale3D();
	auto  Angle = CabinetFurniture->LocalTransform.GetRotation().Euler().Z / 180.0*M_PI;
	auto Ref = EntityFactory->createBlockReference(Utf82Gbk(TCHAR_TO_UTF8(*blockName)).c_str(), CadPos, VectorHelper(Scale).asDouble3(), -Angle, 1, 1, 0, 0);
	AttributeDesc	Desc = GetComAttributeDesc(LayerName);
	Ref->setEntityAttribute(Desc);
	Canvas->drawEntity(Ref);
	UpdateIncrementBlockCounter(IBT_Cabinet);

	return true;
}

/**
* 在指定的图框指定的图层上绘制自定义封板
* @param FrameIndex - const int32 - 图框名称
* @param LayerName - const int32 - 图层名称
* @param CustomDefine - const TSharedPtr<"FArmyCustomDefine>  - 自定义
* @return bool - true 表示绘制成功
*/
bool "FArmyAutoCad::DrawCustomDefine(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyCustomDefine>  CustomDefine)
{
	if (!CustomDefine.IsValid())
	{
		return false;
	}

	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	// 绘制线段
	TSharedPtr<"FArmyCadBundle> CustomDefineBounle = MakeShareable(new "FArmyCadBundle());
	TArray<TSharedPtr<"FArmyLine>> ObjLineList;
	CustomDefine->GetDrawCadLines(ObjLineList);
	for (auto It:ObjLineList)
	{
		CustomDefineBounle->AddLine(It);
	}

	CustomDefineBounle->BlockName = L"CustomDefine_" + FString::FromInt(GetBlockValue(IBT_CustomDefine));
	if (DrawComBlock(FrameIndex, LayerName, CustomDefineBounle))
	{
		UpdateIncrementBlockCounter(IBT_CustomDefine);
		return true;
	}
	else
	{
		return false;
	}
}

/**
* 在指定的图框指定的图层上绘制橱柜上面的构件
* @param FrameIndex - const int32 - 图框名称
* @param LayerName - const int32 - 图层名称
* @param ComponentInfo - const FComponentInfo  - 自定义
* @return bool - true 表示绘制成功
*/
bool "FArmyAutoCad::DrawWHCComponent(const int32 FrameIndex, const FString LayerName, "FArmyWHCTableObject::FComponentInfo  ComponentInfo)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	TArray<struct FSimpleElementVertex> ElementVertexList = ComponentInfo.DrawArray;
	int count = ElementVertexList.Num();
	if (count < 2)
	{
		return false;
	}
	TArray<TSharedPtr<"FArmyLine>> LineArray;
	for (int i = 1; i < count; i += 2)
	{
		FVector StartVector = ElementVertexList[i-1].Position;
		FVector EndVector = ElementVertexList[i].Position;
		LineArray.Add(MakeShareable(new "FArmyLine(StartVector, EndVector)));
	}

	// 绘制线段
	TSharedPtr<"FArmyCadBundle> WHCBounle = MakeShareable(new "FArmyCadBundle());
	for (auto It : LineArray)
	{
		WHCBounle->AddLine(It);
	}

	WHCBounle->BlockName = L"WHC_" + FString::FromInt(GetBlockValue(IBT_WHCComponent));
	if (DrawComBlock(FrameIndex, LayerName, WHCBounle))
	{
		UpdateIncrementBlockCounter(IBT_WHCComponent);
		return true;
	}
	else
	{
		return false;
	}
}

/**
* 在指定的图框指定的图层上绘制橱柜的门板
* @param FrameIndex - const int32 - 图框名称
* @param LayerName - const int32 - 图层名称
* @param DoorSheet -  TSharedPtr<class "FArmyWHCDoorSheet>  - 门板
* @return bool - true 表示绘制成功
*/
bool "FArmyAutoCad::DrawDoorSheet(const int32 FrameIndex, const FString LayerName, TSharedPtr<class "FArmyWHCDoorSheet> DoorSheet)
{
	if (!DoorSheet.IsValid())
	{
		return false;
	}

	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	TArray<struct FSimpleElementVertex> ElementVertexList = DoorSheet->GetDrawArray();
	int count = ElementVertexList.Num();
	if (count < 2)
	{
		return false;
	}
	TArray<TSharedPtr<"FArmyLine>> LineArray;
	for (int i = 1; i < count; i += 2)
	{
		FVector StartVector = ElementVertexList[i - 1].Position;
		FVector EndVector = ElementVertexList[i].Position;
		LineArray.Add(MakeShareable(new "FArmyLine(StartVector, EndVector)));
	}

	// 绘制线段
	TSharedPtr<"FArmyCadBundle> WHCBounle = MakeShareable(new "FArmyCadBundle());
	for (auto It : LineArray)
	{
		WHCBounle->AddLine(It);
	}

	WHCBounle->BlockName = L"WHC_" + FString::FromInt(GetBlockValue(IBT_WHCComponent));
	if (DrawComBlock(FrameIndex, LayerName, WHCBounle))
	{
		UpdateIncrementBlockCounter(IBT_WHCComponent);
		return true;
	}
	else
	{
		return false;
	}
}
// 保存文件
bool "FArmyAutoCad::Save(const FString& FileName)
{
	std::string TmpName = Utf82Gbk(TCHAR_TO_UTF8(*FileName));
	return Canvas->asFile(TmpName.c_str());
}

// 测试
bool "FArmyAutoCad::TestCad(const int32 FrameIndex, const FString LayerName)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	TSharedPtr<"FArmyCadBundle> WindowBounle = MakeShareable(new "FArmyCadBundle);
	TSharedPtr<"FArmyCircle>  Circle = MakeShareable(new "FArmyCircle);
	Circle->Position = FVector::ZeroVector;
	Circle->Radius = 100;
	//WindowBounle->AddCircle(Circle);

	//TSharedPtr<"FArmyArcLine>  ArcLine = MakeShareable(new "FArmyArcLine);
	//ArcLine->Position = FVector(0,0,0);
	//ArcLine->Radius = 200;
	//WindowBounle->AddArcLine(ArcLine);


	TSharedPtr<"FArmyPolyline>  Polyline = MakeShareable(new "FArmyPolyline);
	Polyline->LineVertices.Add(FVector(0, 0, 0));
	Polyline->LineVertices.Add(FVector(100, 0, 0));
	Polyline->LineVertices.Add(FVector(200, 100, 0));
	Polyline->LineVertices.Add(FVector(300, 400, 0));
	Polyline->LineVertices.Add(FVector(400, 500, 0));
	//WindowBounle->AddPolyline(Polyline);


	TSharedPtr<"FArmyPolygon>  Polygon = MakeShareable(new "FArmyPolygon);
	Polygon->Vertices.Add(FVector(0, 0, 0));
	Polygon->Vertices.Add(FVector(100, 100, 0));
	Polygon->Vertices.Add(FVector(100, 200, 0));
	Polygon->Vertices.Add(FVector(50, 150, 0));
	Polygon->Vertices.Add(FVector(0, 200, 0));

	//WindowBounle->AddPolygon(Polygon);


	return true;
}

#pragma endregion CAD绘制公开方法


///////////////////////CAD绘制私有方法////////////////////////
#pragma region CAD绘制私有方法

// 绘制通用块
bool "FArmyAutoCad::DrawComBlock(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyCadBundle> &CadBounle)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	if (!CadBounle.IsValid())
	{
		return false;
	}

	TArray<AttributeEntity*> entityArray;
	for (int i = 0; i < CadBounle->Points.Num(); ++i)
	{
		auto  point = CadBounle->Points[i];
		Double4 CurrentPoint = GetCadPosOfFrame(point->GetPos(), FrameIndex);
		auto pointEntity = EntityFactory->createPoint(CurrentPoint);
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.red = DefaultColor.R;
		Desc.green = DefaultColor.G;
		Desc.blue = DefaultColor.B;
		Desc.weight = CadBounle->PointsWeight[i];
		std::string lineType = Utf82Gbk(TCHAR_TO_UTF8(*(CadBounle->PointsLineName[i])));
		Desc.lineType = (char*)lineType.c_str();

		pointEntity->setEntityAttribute(Desc);
		entityArray.Add(pointEntity);
	}

	for (int i = 0; i < CadBounle->Lines.Num(); ++i)
	{
		auto  Line = CadBounle->Lines[i];
		Double4 StartPoint = GetCadPosOfFrame(Line->GetStart(), FrameIndex);
		Double4 EndPoint = GetCadPosOfFrame(Line->GetEnd(), FrameIndex);
		auto s_entity = EntityFactory->createLine(StartPoint, EndPoint);

		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.red = Line->GetBaseColor().R * 255;
		Desc.green = Line->GetBaseColor().G * 255;
		Desc.blue = Line->GetBaseColor().B * 255;
		Desc.weight = CadBounle->LinesWeight[i];
		std::string lineType = Utf82Gbk(TCHAR_TO_UTF8(*(CadBounle->LinesLineName[i])));
		Desc.lineType = (char*)lineType.c_str();

		s_entity->setEntityAttribute(Desc);
		entityArray.Add(s_entity);
	}

	for (int i = 0; i < CadBounle->Polygons.Num(); ++i)
	{
		TArray<Double4> Vertexes;
		auto pointList = CadBounle->Polygons[i]->Vertices;

		for (auto point : pointList)
		{
			Vertexes.Add(GetCadPosOfFrame(point, FrameIndex));
		}
		auto s_entity = EntityFactory->createPolygon(&Vertexes[0], Vertexes.Num(), P_CLOSE);
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.red = CadBounle->Polygons[i]->GetBaseColor().R * 255;
		Desc.green = CadBounle->Polygons[i]->GetBaseColor().G * 255;
		Desc.blue = CadBounle->Polygons[i]->GetBaseColor().B * 255;
		Desc.weight = CadBounle->PolygonsWeight[i];
		std::string lineType = Utf82Gbk(TCHAR_TO_UTF8(*(CadBounle->PolygonsLineName[i])));
		Desc.lineType = (char*)lineType.c_str();

		s_entity->setEntityAttribute(Desc);
		entityArray.Add(s_entity);
	}

	for (int i = 0; i < CadBounle->Circles.Num(); ++i)
	{
		auto circle = CadBounle->Circles[i];
		auto s_entity = EntityFactory->createCircle(GetCadPosOfFrame(circle->Position, FrameIndex), GetCadLength(circle->Radius));
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.red = circle->GetBaseColor().R * 255;
		Desc.green = circle->GetBaseColor().G * 255;
		Desc.blue = circle->GetBaseColor().B * 255;
		Desc.weight = CadBounle->CirclesWeight[i];
		std::string lineType = Utf82Gbk(TCHAR_TO_UTF8(*(CadBounle->CirclesLineName[i])));
		Desc.lineType = (char*)lineType.c_str();

		s_entity->setEntityAttribute(Desc);
		entityArray.Add(s_entity);
	}

	for (int i = 0; i < CadBounle->ArcLines.Num(); ++i)
	{
		auto arcLine = CadBounle->ArcLines[i];
		FVector AngleVector = CalculateArcAngular(arcLine->GetPosition(), arcLine->GetStartPos(), arcLine->GetEndPos());
		if (!arcLine->IsClockwise())
		{
			Swap(AngleVector.X, AngleVector.Y);
		}		
		auto s_entity = EntityFactory->createArc(GetCadPosOfFrame(arcLine->GetPosition(), FrameIndex), GetCadLength(arcLine->GetRadius()),
			AngleVector.X , AngleVector.Y 
		);
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.red = arcLine->GetBaseColor().R * 255;
		Desc.green = arcLine->GetBaseColor().G * 255;
		Desc.blue = arcLine->GetBaseColor().B * 255;
		Desc.weight = CadBounle->ArcLinesWeight[i];
		std::string lineType = Utf82Gbk(TCHAR_TO_UTF8(*(CadBounle->ArcLinesLineName[i])));
		Desc.lineType = (char*)lineType.c_str();
		Desc.lineScale = 12.0;

		s_entity->setEntityAttribute(Desc);
		entityArray.Add(s_entity);
	}

	for (int i = 0; i < CadBounle->Polylines.Num(); ++i)
	{
		auto poly = CadBounle->Polylines[i];
		auto vertices = poly->LineVertices;
		TArray<Double4> vertexes;
		for (auto it : vertices)
		{
			vertexes.Add(GetCadPosOfFrame(it, FrameIndex));
		}

		auto s_entity = EntityFactory->createPolyline(&vertexes[0], vertexes.Num());
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.red = CadBounle->Polygons[i]->GetBaseColor().R * 255;
		Desc.green = CadBounle->Polygons[i]->GetBaseColor().G * 255;
		Desc.blue = CadBounle->Polygons[i]->GetBaseColor().B * 255;
		Desc.weight = CadBounle->PolylinesWeight[i];
		std::string lineType = Utf82Gbk(TCHAR_TO_UTF8(*(CadBounle->PolylinesLineName[i])));
		Desc.lineType = (char*)lineType.c_str();

		s_entity->setEntityAttribute(Desc);
		entityArray.Add(s_entity);
	}

	// 文字
	for (int i = 0; i < CadBounle->CanvasTexts.Num(); ++i)
	{
		auto CanvasText = CadBounle->CanvasTexts[i];
		
		// 绘制文字
		MTextDesc textDesc;
		textDesc.alignmentPoint = GetCadPosOfFrame(CanvasText.GetPosition(), FrameIndex);
		textDesc.drawDirection = LeftToRight;
		textDesc.hAlign = HAlignCenter;
		textDesc.vAlign = VAlignTop;
		textDesc.lineSpaceStyle = Exact;

		textDesc.text = CorrectString(Utf82Gbk(TCHAR_TO_UTF8(*CanvasText.Text.ToString())));
		textDesc.angle = 0;
		textDesc.style = "standard";
		textDesc.width = 1;
		textDesc.height = GetCadLength(CanvasText.GetCanvasTextSize());
		textDesc.lineSpacingFactor = 1.;

		AttributeDesc Desc = GetComAttributeDesc(LayerName);
		Desc.weight = CadBounle->CanvasTextsWeight[i];
		std::string lineType = Utf82Gbk(TCHAR_TO_UTF8(*(CadBounle->CanvasTextsLineName[i])));
		Desc.lineType = (char*)lineType.c_str();

		auto s_entity  = EntityFactory->createMText(textDesc);
		s_entity->setEntityAttribute(Desc);
		entityArray.Add(s_entity);
	}

	IBlock* Comblock = EntityFactory->createBlockCanvas(Utf82Gbk(TCHAR_TO_UTF8(*CadBounle->BlockName)).c_str(), Double4());
	for (auto entity : entityArray)
	{
		Comblock->drawEntity(entity);
	}
	Canvas->addBlock(Comblock);

	auto Ref = EntityFactory->createBlockReference(Utf82Gbk(TCHAR_TO_UTF8(*CadBounle->BlockName)).c_str(), Double4(), VectorHelper(FVector::OneVector).asDouble3(), 0, 1, 1, 0, 0);
	if (CadBounle->IsDefaultDesc)
	{
		AttributeDesc Desc = GetComAttributeDesc(LayerName);
		Ref->setEntityAttribute(Desc);
	}
	else
	{
		Ref->setEntityAttribute(CadBounle->Desc);
	}

	Canvas->drawEntity(Ref);
	return true;
 
}

bool "FArmyAutoCad::DrawComBlockWithoutRef(const TSharedPtr< "FArmyCadBundle> &CadBounle, BlockExt&blockExt)
{
	if (!CadBounle.IsValid())
	{
		return false;
	}
 
	TArray<AttributeEntity*> entityArray;
	for (int i = 0; i < CadBounle->Points.Num(); ++i)
	{
		auto  point = CadBounle->Points[i];
		Double4 CurrentPoint = GetCadPos(point->GetPos());
		auto pointEntity = EntityFactory->createPoint(CurrentPoint);
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.red = DefaultColor.R;
		Desc.green = DefaultColor.G;
		Desc.blue = DefaultColor.B;
		Desc.weight = CadBounle->PointsWeight[i];
		std::string lineType = Utf82Gbk(TCHAR_TO_UTF8(*(CadBounle->PointsLineName[i])));
		Desc.lineType = (char*)lineType.c_str();

		pointEntity->setEntityAttribute(Desc);
		entityArray.Add(pointEntity);
	}

	for (int i = 0; i < CadBounle->Lines.Num(); ++i)
	{
		auto  Line = CadBounle->Lines[i];
		Double4 StartPoint = GetCadPos(Line->GetStart());
		Double4 EndPoint = GetCadPos(Line->GetEnd());
		auto s_entity = EntityFactory->createLine(StartPoint, EndPoint);

		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.red = Line->GetBaseColor().R * 255;
		Desc.green = Line->GetBaseColor().G * 255;
		Desc.blue = Line->GetBaseColor().B * 255;
		Desc.weight = CadBounle->LinesWeight[i];
		std::string lineType = Utf82Gbk(TCHAR_TO_UTF8(*(CadBounle->LinesLineName[i])));
		Desc.lineType = (char*)lineType.c_str();

		s_entity->setEntityAttribute(Desc);
		entityArray.Add(s_entity);
	}

	for (int i = 0; i < CadBounle->Polygons.Num(); ++i)
	{
		TArray<Double4> Vertexes;
		auto pointList = CadBounle->Polygons[i]->Vertices;

		for (auto point : pointList)
		{
			Vertexes.Add(GetCadPos(point));
		}
		auto s_entity = EntityFactory->createPolygon(&Vertexes[0], Vertexes.Num(), P_CLOSE);
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.red = CadBounle->Polygons[i]->GetBaseColor().R * 255;
		Desc.green = CadBounle->Polygons[i]->GetBaseColor().G * 255;
		Desc.blue = CadBounle->Polygons[i]->GetBaseColor().B * 255;
		Desc.weight = CadBounle->PolygonsWeight[i];
		std::string lineType = Utf82Gbk(TCHAR_TO_UTF8(*(CadBounle->PolygonsLineName[i])));
		Desc.lineType = (char*)lineType.c_str();

		s_entity->setEntityAttribute(Desc);
		entityArray.Add(s_entity);
	}

	for (int i = 0; i < CadBounle->Circles.Num(); ++i)
	{
		auto circle = CadBounle->Circles[i];
		auto s_entity = EntityFactory->createCircle(GetCadPos(circle->Position), GetCadLength(circle->Radius));
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.red = circle->GetBaseColor().R * 255;
		Desc.green = circle->GetBaseColor().G * 255;
		Desc.blue = circle->GetBaseColor().B * 255;
		Desc.weight = CadBounle->CirclesWeight[i];
		std::string lineType = Utf82Gbk(TCHAR_TO_UTF8(*(CadBounle->CirclesLineName[i])));
		Desc.lineType = (char*)lineType.c_str();

		s_entity->setEntityAttribute(Desc);
		entityArray.Add(s_entity);
	}

	for (int i = 0; i < CadBounle->ArcLines.Num(); ++i)
	{
		auto arcLine = CadBounle->ArcLines[i];
		FVector AngleVector = CalculateArcAngular(arcLine->GetPosition(), arcLine->GetStartPos(), arcLine->GetEndPos());
		if (!arcLine->IsClockwise())
		{
			Swap(AngleVector.X, AngleVector.Y);
		}
		auto s_entity = EntityFactory->createArc(GetCadPos(arcLine->GetPosition()), GetCadLength(arcLine->GetRadius()),
			AngleVector.X, AngleVector.Y
		);
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.red = arcLine->GetBaseColor().R * 255;
		Desc.green = arcLine->GetBaseColor().G * 255;
		Desc.blue = arcLine->GetBaseColor().B * 255;
		Desc.weight = CadBounle->ArcLinesWeight[i];
		std::string lineType = Utf82Gbk(TCHAR_TO_UTF8(*(CadBounle->ArcLinesLineName[i])));
		Desc.lineType = (char*)lineType.c_str();
		Desc.lineScale = 12.0;

		s_entity->setEntityAttribute(Desc);
		entityArray.Add(s_entity);
	}

	for (int i = 0; i < CadBounle->Polylines.Num(); ++i)
	{
		auto poly = CadBounle->Polylines[i];
		auto vertices = poly->LineVertices;
		TArray<Double4> vertexes;
		for (auto it : vertices)
		{
			vertexes.Add(GetCadPos(it));
		}

		auto s_entity = EntityFactory->createPolyline(&vertexes[0], vertexes.Num());
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.red = CadBounle->Polygons[i]->GetBaseColor().R * 255;
		Desc.green = CadBounle->Polygons[i]->GetBaseColor().G * 255;
		Desc.blue = CadBounle->Polygons[i]->GetBaseColor().B * 255;
		Desc.weight = CadBounle->PolylinesWeight[i];
		std::string lineType = Utf82Gbk(TCHAR_TO_UTF8(*(CadBounle->PolylinesLineName[i])));
		Desc.lineType = (char*)lineType.c_str();

		s_entity->setEntityAttribute(Desc);
		entityArray.Add(s_entity);
	}

	IBlock* Comblock = EntityFactory->createBlockCanvas(Utf82Gbk(TCHAR_TO_UTF8(*CadBounle->BlockName)).c_str(), Double4());
	for (auto entity : entityArray)
	{
		Comblock->drawEntity(entity);
	}
	Canvas->addBlock(Comblock);
	blockExt.blockName = CadBounle->BlockName;
	blockExt.offset = CadBounle->BasePoint;
	blockExt.block = Comblock;
	return true;
}

bool "FArmyAutoCad::AddRealTimeBlock(const TSharedPtr< "FArmyCadBundle> &CadBounle, const FVector&offsetW, const FString	&BlockName)
{
	if (!CadBounle.IsValid())
		return false;
	TArray<AttributeEntity*> entityArray;
	for (int i = 0; i < CadBounle->Points.Num(); ++i)
	{
		auto  point = CadBounle->Points[i];
		Double4 CurrentPoint = GetCadPos(point->GetPos());
		auto pointEntity = EntityFactory->createPoint(CurrentPoint);
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.red = DefaultColor.R;
		Desc.green = DefaultColor.G;
		Desc.blue = DefaultColor.B;
		Desc.weight = CadBounle->PointsWeight[i];
		std::string lineType = Utf82Gbk(TCHAR_TO_UTF8(*(CadBounle->PointsLineName[i])));
		Desc.lineType = (char*)lineType.c_str();

		pointEntity->setEntityAttribute(Desc);
		entityArray.Add(pointEntity);
	}

	for (int i = 0; i < CadBounle->Lines.Num(); ++i)
	{
		auto  Line = CadBounle->Lines[i];
		Double4 StartPoint = GetCadPos(Line->GetStart());
		Double4 EndPoint = GetCadPos(Line->GetEnd());
		auto s_entity = EntityFactory->createLine(StartPoint, EndPoint);

		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.red = Line->GetBaseColor().R * 255;
		Desc.green = Line->GetBaseColor().G * 255;
		Desc.blue = Line->GetBaseColor().B * 255;
		Desc.weight = CadBounle->LinesWeight[i];
		std::string lineType = Utf82Gbk(TCHAR_TO_UTF8(*(CadBounle->LinesLineName[i])));
		Desc.lineType = (char*)lineType.c_str();

		s_entity->setEntityAttribute(Desc);
		entityArray.Add(s_entity);
	}

	for (int i = 0; i < CadBounle->Polygons.Num(); ++i)
	{
		TArray<Double4> Vertexes;
		auto pointList = CadBounle->Polygons[i]->Vertices;

		for (auto point : pointList)
		{
			Vertexes.Add(GetCadPos(point));
		}
		auto s_entity = EntityFactory->createPolygon(&Vertexes[0], Vertexes.Num(), P_CLOSE);
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.red = CadBounle->Polygons[i]->GetBaseColor().R * 255;
		Desc.green = CadBounle->Polygons[i]->GetBaseColor().G * 255;
		Desc.blue = CadBounle->Polygons[i]->GetBaseColor().B * 255;
		Desc.weight = CadBounle->PolygonsWeight[i];
		std::string lineType = Utf82Gbk(TCHAR_TO_UTF8(*(CadBounle->PolygonsLineName[i])));
		Desc.lineType = (char*)lineType.c_str();

		s_entity->setEntityAttribute(Desc);
		entityArray.Add(s_entity);
	}

	for (int i = 0; i < CadBounle->Circles.Num(); ++i)
	{
		auto circle = CadBounle->Circles[i];
		auto s_entity = EntityFactory->createCircle(GetCadPos(circle->Position), GetCadLength(circle->Radius));
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.red = circle->GetBaseColor().R * 255;
		Desc.green = circle->GetBaseColor().G * 255;
		Desc.blue = circle->GetBaseColor().B * 255;
		Desc.weight = CadBounle->CirclesWeight[i];
		std::string lineType = Utf82Gbk(TCHAR_TO_UTF8(*(CadBounle->CirclesLineName[i])));
		Desc.lineType = (char*)lineType.c_str();

		s_entity->setEntityAttribute(Desc);
		entityArray.Add(s_entity);
	}

	for (int i = 0; i < CadBounle->ArcLines.Num(); ++i)
	{
		auto arcLine = CadBounle->ArcLines[i];
		FVector AngleVector = CalculateArcAngular(arcLine->GetPosition(), arcLine->GetStartPos(), arcLine->GetEndPos());
		if (!arcLine->IsClockwise())
		{
			Swap(AngleVector.X, AngleVector.Y);
		}
		auto s_entity = EntityFactory->createArc(GetCadPos(arcLine->GetPosition()), GetCadLength(arcLine->GetRadius()),
			AngleVector.X, AngleVector.Y
		);
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.red = arcLine->GetBaseColor().R * 255;
		Desc.green = arcLine->GetBaseColor().G * 255;
		Desc.blue = arcLine->GetBaseColor().B * 255;
		Desc.weight = CadBounle->ArcLinesWeight[i];
		std::string lineType = Utf82Gbk(TCHAR_TO_UTF8(*(CadBounle->ArcLinesLineName[i])));
		Desc.lineType = (char*)lineType.c_str();
		Desc.lineScale = 12.0;

		s_entity->setEntityAttribute(Desc);
		entityArray.Add(s_entity);
	}

	for (int i = 0; i < CadBounle->Polylines.Num(); ++i)
	{
		auto poly = CadBounle->Polylines[i];
		auto vertices = poly->LineVertices;
		TArray<Double4> vertexes;
		for (auto it : vertices)
		{
			vertexes.Add(GetCadPos(it));
		}

		auto s_entity = EntityFactory->createPolyline(&vertexes[0], vertexes.Num());
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.red = CadBounle->Polygons[i]->GetBaseColor().R * 255;
		Desc.green = CadBounle->Polygons[i]->GetBaseColor().G * 255;
		Desc.blue = CadBounle->Polygons[i]->GetBaseColor().B * 255;
		Desc.weight = CadBounle->PolylinesWeight[i];
		std::string lineType = Utf82Gbk(TCHAR_TO_UTF8(*(CadBounle->PolylinesLineName[i])));
		Desc.lineType = (char*)lineType.c_str();

		s_entity->setEntityAttribute(Desc);
		entityArray.Add(s_entity);
	}

	IBlock* Comblock = EntityFactory->createBlockCanvas(Utf82Gbk(TCHAR_TO_UTF8(*BlockName)).c_str(), Double4());
	for (auto entity : entityArray)
	{
		Comblock->drawEntity(entity);
	}
	Canvas->addBlock(Comblock);
	auto offset = GetCadPos(offsetW);
	BlockExt blockExt(Comblock, FVector(offset.x, offset.y, offset.z), BlockName);
	RepeatBlock.Add(BlockName, blockExt);
	return true;
}

bool "FArmyAutoCad::DrawRealTimeBlock(const int32 FrameIndex, const FString LayerName, const FString&BlockName, const FVector &offset, const float  Angle)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;
	auto Ref = EntityFactory->createBlockReference(Utf82Gbk(TCHAR_TO_UTF8(*BlockName)).c_str(), GetCadPosOfFrame(offset, FrameIndex), VectorHelper(FVector::OneVector).asDouble3(), -Angle, 1, 1, 0, 0);
	AttributeDesc Desc = GetComAttributeDesc(LayerName);
	Ref->setEntityAttribute(Desc);
	Canvas->drawEntity(Ref);
	return true;
}

bool  "FArmyAutoCad::DrawComBlockRef(const int32 FrameIndex, const FString LayerName, const FString&BlockName, const Double4 &offset, const float  Angle)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;
	auto Ref = EntityFactory->createBlockReference(Utf82Gbk(TCHAR_TO_UTF8(*BlockName)).c_str(), offset, VectorHelper(FVector::OneVector).asDouble3(), -Angle, 1, 1, 0, 0);
	AttributeDesc Desc = GetComAttributeDesc(LayerName);
	Ref->setEntityAttribute(Desc);
	Canvas->drawEntity(Ref);
	return true;
}

// 在指定的图层指定的图框上地绘制通用窗户
bool "FArmyAutoCad::DrawComWindow(const int32 FrameIndex, const FString LayerName, const TArray< TSharedPtr<"FArmyLine>> LineArray)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	TSharedPtr<"FArmyCadBundle> WindowBounle = MakeShareable(new "FArmyCadBundle());
	for (auto It : LineArray)
	{
		WindowBounle->AddLine(It);
	}

	WindowBounle->BlockName = L"Window_" + FString::FromInt(GetBlockValue(IBT_Window));
	if (DrawComBlock(FrameIndex, LayerName, WindowBounle))
	{
		UpdateIncrementBlockCounter(IBT_Window);
		return true;
	}
	else
	{
		return false;
	}

}

// 在指定的图框指定的图层上绘制文字
bool "FArmyAutoCad::DrawMultipleText(const int32 FrameIndex, const FString LayerName, const FVector&AlignmentPoint, const FString&Text, float TextHeight, float BoxWidth, float Angle, bool isLeftToRight)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;

	AttributeDesc Desc = GetComAttributeDesc(LayerName);

	MTextDesc textDesc;
	textDesc.alignmentPoint = GetCadPosOfFrame(AlignmentPoint, FrameIndex);
	textDesc.drawDirection = isLeftToRight ? LeftToRight : TopToBottom;
	textDesc.hAlign = HAlignCenter;
	textDesc.vAlign = VAlignTop;
	textDesc.lineSpaceStyle = Exact;

	textDesc.text = CorrectString(Utf82Gbk(TCHAR_TO_UTF8(*Text)));
	textDesc.angle = Angle;
	textDesc.style = "standard";
	textDesc.width = BoxWidth;
	textDesc.height = GetCadLength(TextHeight);
	textDesc.lineSpacingFactor = 1.;

	AttributeEntity* 	textEntity = EntityFactory->createMText(textDesc);
	textEntity->setEntityAttribute(Desc);
	Canvas->drawEntity(textEntity);
	return true;
}

// 在指定的图框指定的图层上绘制标注
bool "FArmyAutoCad::DrawAlignedDimension(const int32 FrameIndex, const FString LayerName, const FVector&MesureStart, const FVector&MesureEnd, const FVector &LinePostion)
{
	if (!CheckDraw(FrameIndex, LayerName))
		return false;
	auto Postion = GetCadVectorOfFrame(LinePostion, FrameIndex);
	auto  Start = GetCadVectorOfFrame(MesureStart, FrameIndex);
	auto  End = GetCadVectorOfFrame(MesureEnd, FrameIndex);
	DimensionDesc	desc;
	FMemory::Memzero(desc);
	desc.definitionPoint = VectorHelper(Postion).asDouble3();
	desc.midTextPoint = VectorHelper((Start + End) / 2).asDouble3();
	desc.type = 1;
	desc.attachmentPoint = 8;
	desc.lineSpacingStyle = 2;
	desc.lineSpacingFactor = 10.0;
	desc.angle = 0;
	desc.linearFactor = 1.0;
	desc.dimScale = 1.0;
	desc.arrow1Flipped = false;
	desc.arrow2Flipped = false;
	AttributeDesc Desc = GetComAttributeDesc(LayerName);
	auto entity = EntityFactory->createDimAligned(desc, VectorHelper(Start).asDouble3(), VectorHelper(End).asDouble3());
	entity->setEntityAttribute(Desc);
	Canvas->drawEntity(entity);
	return true;
}

#pragma endregion CAD绘制公开方法


///////////////////////数据相关私有方法////////////////////////
#pragma region 数据相关私有方法

//清理图块
void "FArmyAutoCad::ClearBlocks()
{
 
	for (auto iter : ComponentManager)
	{
		iter.Value.block->release();
	}
	ComponentManager.Empty();
	for (auto iter : FurnitureManager)
	{
		iter.Value.block->release();
	}

	for (auto iter : SysBlockMap)
	{
		iter.Value->release();
	}
	SysBlockMap.Empty();
	FurnitureManager.Empty();
	FrameStepLength = 0;
}

// 初始化数据
void "FArmyAutoCad::InitializeArmyAutoCad()
{
	EntityFactory = IFactory::createFactory();
	Canvas = EntityFactory->createCanvas();
	DefaultColor = FColor(0XFFFFFFFF);
	InitSystemBlock();

}

// 初始化块计数器
void "FArmyAutoCad::InitIncrementBlockCounter()
{
	// 图块计数器
	IncrementBlockCounterMap.Add(IBT_Window, 0);
	IncrementBlockCounterMap.Add(IBT_Beam, 0);
	IncrementBlockCounterMap.Add(IBT_AirFlue, 0);
	IncrementBlockCounterMap.Add(IBT_Pillar, 0);
	IncrementBlockCounterMap.Add(IBT_Door, 0);
	IncrementBlockCounterMap.Add(IBT_Lable, 0);
	IncrementBlockCounterMap.Add(IBT_DimLable, 0);
	IncrementBlockCounterMap.Add(IBT_BoardSpliteline, 0);
	IncrementBlockCounterMap.Add(IBT_CustomHatch, 0);
	IncrementBlockCounterMap.Add(IBT_PackPipe, 0);
	IncrementBlockCounterMap.Add(IBT_PassEdge, 0);
	IncrementBlockCounterMap.Add(IBT_NewPassEdge, 0);
	IncrementBlockCounterMap.Add(IBT_Punch, 0);
	IncrementBlockCounterMap.Add(IBT_Symbol, 0);
	IncrementBlockCounterMap.Add(IBT_Cabinet, 0);
	IncrementBlockCounterMap.Add(IBT_CustomDefine, 0);
	IncrementBlockCounterMap.Add(IBT_WHCComponent, 0);
}

// 更计数器
void "FArmyAutoCad::UpdateIncrementBlockCounter(EIncrementBlockType IncrementBlockType)
{
	if (!IncrementBlockCounterMap.Contains(IncrementBlockType))
	{
		return;
	}
	int value = *IncrementBlockCounterMap.Find(IncrementBlockType) + 1;
	IncrementBlockCounterMap.Add(IncrementBlockType, value);
}

// 获得下一个块的计数值
int32 "FArmyAutoCad::GetBlockValue(EIncrementBlockType IncrementBlockType)
{
	if (!IncrementBlockCounterMap.Contains(IncrementBlockType))
	{
		return 0;
	}

	return *IncrementBlockCounterMap.Find(IncrementBlockType) + 1;

}

// UTF-8转GBK
std::string  "FArmyAutoCad::Utf82Gbk(const char*utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}

// 根据图框进行坐标装换方法
Double4 "FArmyAutoCad::GetCadPosOfFrame(const FVector &SrcVector, int32 FrameIndex)
{
	FVector NewVector = FVector::ZeroVector;
	if (FrameManager.Contains(FrameIndex))
	{
		BlockExt *CurrentFrame = FrameManager.Find(FrameIndex);
		NewVector = GetCadVector(SrcVector);
		NewVector = NewVector + CurrentFrame->offset;
	}

	return Double4(NewVector.X, NewVector.Y, NewVector.Z);
}

// 进行坐标装换方法
Double4 "FArmyAutoCad::GetCadPos(const FVector &SrcVector)
{
	FVector NewVector = SrcVector*EXTEND_VALUE;
	return Double4(NewVector.X, -NewVector.Y, NewVector.Z);
}

// 进行坐标装换方法
FVector "FArmyAutoCad::GetCadVector(const FVector &SrcVector)
{
	FVector NewVector = SrcVector*EXTEND_VALUE;
	NewVector.Y = -NewVector.Y;
	return NewVector;
}

// 根据图框进行坐标装换方法
FVector "FArmyAutoCad::GetCadVectorOfFrame(const FVector &SrcVector, int32 FrameIndex)
{
	FVector NewVector = FVector::ZeroVector;
	if (FrameManager.Contains(FrameIndex))
	{
		BlockExt *CurrentFrame = FrameManager.Find(FrameIndex);
		NewVector = GetCadVector(SrcVector);
		NewVector = NewVector + CurrentFrame->offset;
	}

	return NewVector;
}

// 获得CAD长度
float "FArmyAutoCad::GetCadLength(const float SrcLength)
{
	return SrcLength*EXTEND_VALUE;
}

// 矫正String
std::string  "FArmyAutoCad::CorrectString(const std::string&string)
{
	std::string result;
	for (int i = 0; i < string.size(); ++i)
	{
		if (string[i] == '\r')
		{
			if (i + 1 < string.size() && string[i + i] == '\n')
				++i;
			result += "\\P";
		}
		else if (string[i] == '\n')
			result += "\\P";
		else
			result += string[i];
	}
	return result;
}

// 获取AABB
FBox "FArmyAutoCad::CalculateBoxMin(const TSharedPtr<class "FArmyConstructionFrame>& frame)
{
	auto Min = FVector(FLT_MAX, FLT_MAX, FLT_MAX);
	auto Max = -Min;

	TArray<TSharedPtr<"FArmyLine>> BaseLines;
	frame->GetFrameBaseLines(BaseLines);
	FTransform LocalTransform = frame->GetLocalTransform();
	for (auto line : BaseLines)
	{
		auto Beg = LocalTransform.TransformPosition(line->GetStart());
		auto End = LocalTransform.TransformPosition(line->GetEnd());
		Min = MinVector(GetCadVector(Beg), Min);
		Max = MaxVector(GetCadVector(Beg), Max);

		Min = MinVector(GetCadVector(End), Min);
		Max = MaxVector(GetCadVector(End), Max);
	}

	FBox Box;
	Box.Max = Max;
	Box.Min = Min;
	return Box;
}

// 是否可以进行绘制
bool "FArmyAutoCad::CheckDraw(int32 FrameIndex, FString LayerName)
{
	return LayerManager.Contains(LayerName) && FrameManager.Contains(FrameIndex);
}

// 获得通用绘制属性
AttributeDesc "FArmyAutoCad::GetComAttributeDesc(FString LayerName)
{
	AttributeDesc Desc;
	memset(&Desc, 0, sizeof(AttributeDesc));
	Desc.lineType = "ByLayer";
	Desc.weight = LineWeight::WeightByLayer;
	CurrentLayerName = Utf82Gbk(TCHAR_TO_UTF8(*LayerName));
	Desc.layerName = (char*)CurrentLayerName.c_str();
	Desc.colorType = ByLayer;

	return Desc;
}

// 获得绘制属性
AttributeDesc "FArmyAutoCad::GetAttributeDescByLayer(TSharedPtr<LayerInfo> InLayer)
{
	AttributeDesc Desc;
	memset(&Desc, 0, sizeof(AttributeDesc));

	//Desc.lineType = TCHAR_TO_UTF8(*InLayer->LineType);
	Desc.lineType = "DASHED2";

	Desc.weight = (LineWeight)InLayer->LayerLineWeight;
	CurrentLayerName = Utf82Gbk(TCHAR_TO_UTF8(*InLayer->Name));
	Desc.layerName = (char*)CurrentLayerName.c_str();
	Desc.colorType = Fixed;
	Desc.red = InLayer->LayerColor.R;
	Desc.green = InLayer->LayerColor.G;
	Desc.blue = InLayer->LayerColor.B;

	return Desc;
}

// 计算图框偏移绘制
float "FArmyAutoCad::GetFrameOffset(int32 FrameIndex)
{
	float NewLength = 0;
	if (FrameIndex == 0)
	{
		return NewLength;
	}
	if (FrameManager.Num() == FrameIndex)
	{
		for (auto It : FrameManager)
		{
			NewLength = NewLength + It.Value.FrameLength+FrameStepLength;
		}
	}
	
	return NewLength;
}
//获取圆弧参数,返回X  起始角度 ，Y终止角度 (0-2*pi)
FVector "FArmyAutoCad::CalculateArcAngular(const FVector&centre, const FVector&startPoint, const FVector&endPoint)
{
	auto start = startPoint - centre;
	auto end = endPoint - centre;

	FVector Xaxis = FVector(1, 0, 0);
	start.Normalize();
	end.Normalize();

	auto startAngle = FMath::Acos(start.X);
	auto endAngle = FMath::Acos(end.X);


	auto point = FVector::CrossProduct(Xaxis, start).Z;
	if (point < 0)
	{
		startAngle = 2 * PI - startAngle;
	}

	point = FVector::CrossProduct(Xaxis, end).Z;
	if (point < 0)
	{
		endAngle = 2 * PI - endAngle;
	}

	return FVector((2 * PI - endAngle) / PI * 180, (2 * PI - startAngle) / PI * 180, 0);

}

//初始化系统快
void "FArmyAutoCad::InitSystemBlock()
{
	IBlock *architecture = EntityFactory->createBlockCanvas("ArchTick", Double4(0, 0, 0));
	AttributeEntity*hatch = EntityFactory->createHatch(1, true, 1, 0, "NULL");
	AttributeDesc   hDesc;
	memset(&hDesc, 0, sizeof(AttributeDesc));
	hDesc.red = 255;
	hDesc.green = 255;
	hDesc.blue = 255;
	hDesc.lineType = "CONTINUOUS";
	hDesc.weight = Weight000;
	hatch->setEntityAttribute(hDesc);
	IBlock*  HacthContainer = dynamic_cast<IBlock*>(hatch);
	IBlock* loopBlock = EntityFactory->createBlockCanvas("", Double4());
	loopBlock->drawEntity(EntityFactory->createLine(Double4(-0.55, -0.44), Double4(-0.44, -0.55)));
	loopBlock->drawEntity(EntityFactory->createLine(Double4(-0.44, -0.55), Double4(0.55, 0.44)));
	loopBlock->drawEntity(EntityFactory->createLine(Double4(0.55, 0.44), Double4(0.44, 0.55)));
	loopBlock->drawEntity(EntityFactory->createLine(Double4(0.44, 0.55), Double4(-0.55, -0.44)));
	HacthContainer->addBlock(loopBlock);
	architecture->drawEntity(hatch);

	SysBlockMap.Add("ArchTick", architecture);


	IBlock* mLeaderArrow = EntityFactory->createBlockCanvas("_DotSmall", Double4(0, 0, 0));
	AttributeEntity*hatch2 = EntityFactory->createHatch(1, true, 1, 0, "NULL");
	hatch2->setEntityAttribute(hDesc);
	IBlock*  HacthContainer2 = dynamic_cast<IBlock*>(hatch2);
	IBlock* loopBlock2= EntityFactory->createBlockCanvas("", Double4());
	 
	loopBlock2->drawEntity(EntityFactory->createArc(Double4(0, 0), 0.125, 0, M_PI));
	loopBlock2->drawEntity(EntityFactory->createArc(Double4(0, 0), 0.125, M_PI, 2 * M_PI));

	HacthContainer2->addBlock(loopBlock2);
	mLeaderArrow->drawEntity(hatch2);
	SysBlockMap.Add("_DotSmall", mLeaderArrow);


}

// 通过线段数组获得多边形线段
TSharedPtr<class "FArmyPolyline> "FArmyAutoCad::GetPolylineByLines(TArray<TSharedPtr<class "FArmyLine>> InLineArray)
{
	TSharedPtr<"FArmyPolyline> Polyline = MakeShareable(new "FArmyPolyline);
	for (auto It : InLineArray)
	{
		if (!Polyline->LineVertices.Contains(It->GetStart()))
		{
			Polyline->LineVertices.Add(It->GetStart());
		}

		if (!Polyline->LineVertices.Contains(It->GetEnd()))
		{
			Polyline->LineVertices.Add(It->GetEnd());
		}

	}
	
	return Polyline;
}

FString	 "FArmyAutoCad::CalculateWindowKey(const TSharedPtr< "FArmyWindow>&window)
{
	FString Key = L"ERT_Window";
	Key+= FString::FromInt(window->GetWindowType());
	Key += "_";
	Key+= FString::FromInt(window->GetLength());
	return Key;
 
}

FString "FArmyAutoCad::CalculateFloorWindowKey(const TSharedPtr<class "FArmyFloorWindow>&window)
{
	FString Key = L"ERT_FloorWindow";
	Key += FString::FromInt(window->GetWindowType());
	Key += FString::SanitizeFloat(window->GetLength());
	return Key;
}

FString "FArmyAutoCad::CalculateRectBayWindowKey(const TSharedPtr<class "FArmyRectBayWindow>&window)
{
 
	FString Key = L"ERT_RectBayWindow";
	Key += FString::FromInt(window->GetWindowType());
	Key += FString::SanitizeFloat(window->GetLength());
	return Key;
 
}

FString "FArmyAutoCad::CalculateTrapeBayWindowKey(const TSharedPtr<class "FArmyTrapeBayWindow>&window)
{
	FString Key = L"ERT_TrapeBayWindow";
	Key += FString::FromInt(window->GetWindowType());
	Key += FString::SanitizeFloat(window->GetLength());
	return Key;
}

FString "FArmyAutoCad::CalculateCornerBayWindowKey(const TSharedPtr<class "FArmyCornerBayWindow>&window)
{
	FString Key = L"ERT_CornerBayWindow";
	Key += FString::FromInt(window->GetWindowCornerType());
	Key += FString::SanitizeFloat(window->GetLeftWindowLength());
	Key += FString::SanitizeFloat(window->GetRightWindowLength());
	return Key;
}


FString "FArmyAutoCad::CalculateSingleDoorKey(const TSharedPtr<class "FArmySingleDoor> door)
{
	FString Key = L"ERT_SingleDoor";
	Key += FString::FromInt(door->IsRightOpen());
	Key += FString::FromInt(door->GetLength());
	return Key;
}

FString "FArmyAutoCad::CalculateSlidingDoorKey(const TSharedPtr<class "FArmySlidingDoor> door)
{
	FString Key = L"ERT_SlidingDoor";
	Key += FString::FromInt(door->GetLength());
	return Key;
}

FString "FArmyAutoCad::CalculateSecurityDoorKey(const TSharedPtr<class "FArmySecurityDoor> door)
{
	FString Key = L"ERT_SecurityDoor";
	Key += FString::FromInt(door->IsRightOpen());
	Key += FString::FromInt(door->GetLength());
	return Key;
}


FVector	"FArmyAutoCad::CalculateWindowOffset(const TSharedPtr<class "FArmyWindow>&window)
{
	return FVector((window->GetStartPos() + window->GetEndPos()) / 2.f);
}
FVector	"FArmyAutoCad::CalculateFloorWindowOffset(const TSharedPtr<class "FArmyFloorWindow>&window)
{
	return FVector((window->GetStartPos() + window->GetEndPos()) / 2.f);
}
FVector	"FArmyAutoCad::CalculateRectBayWindowOffset(const TSharedPtr<class "FArmyRectBayWindow>&window)
{
	return FVector((window->GetStartPos() + window->GetEndPos()) / 2.f);
}
FVector	"FArmyAutoCad::CalculateTrapeBayWindowOffset(const TSharedPtr<class "FArmyTrapeBayWindow>&window)
{
	return FVector((window->GetStartPos() + window->GetEndPos()) / 2.f);
}
FVector   "FArmyAutoCad::CalculateCornerBayWindowOffset(const TSharedPtr<class "FArmyCornerBayWindow>&window)
{
	return window->GetPos();
}

FVector "FArmyAutoCad::CalculateSingleDoorOffset(const TSharedPtr<class "FArmySingleDoor> door)
{
	return FVector((door->GetStartPos() + door->GetEndPos()) / 2.f);
}
FVector "FArmyAutoCad::CalculateSlidingDoorOffset(const TSharedPtr<class "FArmySlidingDoor> door)
{
	return FVector((door->GetStartPos() + door->GetEndPos()) / 2.f);
}
FVector "FArmyAutoCad::CalculateSecurityDoorOffset(const TSharedPtr<class "FArmySecurityDoor> door)
{
	return FVector((door->GetStartPos() + door->GetEndPos()) / 2.f);
}

float	    "FArmyAutoCad::CalculateWindowAngle(const TSharedPtr<class "FArmyWindow>&window)
{
	// End - Start  = Direction

	//FVector(1,0,0)	-> 0  Angle

	auto Diretion = window->GetEndPos()- window->GetStartPos();
 

	FVector Xaxis = FVector(1, 0, 0);
	Diretion.Normalize();
 

	auto startAngle = FMath::Acos(Diretion.X);
	auto point = FVector::CrossProduct(Xaxis, Diretion).Z;
	if (point < 0)
	{
		startAngle = 2 * PI - startAngle;
	}
	return startAngle;
}
float		"FArmyAutoCad::CalculateFloorWindowAngle(const TSharedPtr<class "FArmyFloorWindow>&window)
{
	// End - Start  = Direction

	//FVector(1,0,0)	-> 0  Angle

	auto Diretion = window->GetEndPos() - window->GetStartPos();


	FVector Xaxis = FVector(1, 0, 0);
	Diretion.Normalize();


	auto startAngle = FMath::Acos(Diretion.X);
	auto point = FVector::CrossProduct(Xaxis, Diretion).Z;
	if (point < 0)
	{
		startAngle = 2 * PI - startAngle;
	}
	return startAngle;
}
float		"FArmyAutoCad::CalculateRectBayWindowAngle(const TSharedPtr<class "FArmyRectBayWindow>&window)
{
	// End - Start  = Direction

	//FVector(1,0,0)	-> 0  Angle

	auto Diretion = window->GetEndPos() - window->GetStartPos();


	FVector Xaxis = FVector(1, 0, 0);
	Diretion.Normalize();


	auto startAngle = FMath::Acos(Diretion.X);
	auto point = FVector::CrossProduct(Xaxis, Diretion).Z;
	if (point < 0)
	{
		startAngle = 2 * PI - startAngle;
	}
	return startAngle;
}
float		"FArmyAutoCad::CalculateTrapeBayWindowAngle(const TSharedPtr<class "FArmyTrapeBayWindow>&window)
{
	// End - Start  = Direction

	//FVector(1,0,0)	-> 0  Angle

	auto Diretion = window->GetEndPos() - window->GetStartPos();


	FVector Xaxis = FVector(1, 0, 0);
	Diretion.Normalize();


	auto startAngle = FMath::Acos(Diretion.X);
	auto point = FVector::CrossProduct(Xaxis, Diretion).Z;
	if (point < 0)
	{
		startAngle = 2 * PI - startAngle;
	}
	return startAngle;
}
float		"FArmyAutoCad::CalculateCornerBayWindowAngle(const TSharedPtr<class "FArmyCornerBayWindow>&window)
{
	return 0;
}

float "FArmyAutoCad::CalculateSingleDoorAngle(const TSharedPtr<class "FArmySingleDoor> door)
{
	// End - Start  = Direction

	//FVector(1,0,0)	-> 0  Angle

	auto Diretion = door->GetDirection();


	FVector Xaxis = FVector(1, 0, 0);
	Diretion.Normalize();


	auto startAngle = FMath::Acos(Diretion.X);
	auto point = FVector::CrossProduct(Xaxis, Diretion).Z;
	if (point < 0)
	{
		startAngle = 2 * PI - startAngle;
	}
	return startAngle;
}
float "FArmyAutoCad::CalculateSlidingDoorAngle(const TSharedPtr<class "FArmySlidingDoor> door)
{
	auto Diretion = door->GetDirection();


	FVector Xaxis = FVector(1, 0, 0);
	Diretion.Normalize();


	auto startAngle = FMath::Acos(Diretion.X);
	auto point = FVector::CrossProduct(Xaxis, Diretion).Z;
	if (point < 0)
	{
		startAngle = 2 * PI - startAngle;
	}
	return startAngle;
}
float "FArmyAutoCad::CalculateSecurityDoorAngle(const TSharedPtr<class "FArmySecurityDoor> door)
{
	auto Diretion = door->GetDirection();


	FVector Xaxis = FVector(1, 0, 0);
	Diretion.Normalize();


	auto startAngle = FMath::Acos(Diretion.X);
	auto point = FVector::CrossProduct(Xaxis, Diretion).Z;
	if (point < 0)
	{
		startAngle = 2 * PI - startAngle;
	}
	return startAngle;
}
#pragma endregion 数据相关私有方法