#include "ArmyDxfModel.h"
#include "ArmyFurniture.h"
#include "Primitive2D/XRPoint.h"
#include "Primitive2D/XRPolyline.h"

//FArmyDxfLayer::FArmyDxfLayer()
//{
//
//}
//
//FArmyDxfLayer::~FArmyDxfLayer()
//{
//	TotalPoints.Empty();
//}
//
//void FArmyDxfLayer::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
//{
//	for (auto PointItr : TotalPoints)
//	{
//		PointItr.Draw(PDI, View);
//	}
//	for (auto PolyLineItr : TotalPolyLines)
//	{
//		PolyLineItr.Draw(PDI, View);
//	}
//}
//
//void FArmyDxfLayer::AddPolyLine(FArmyPolyline polyLine)
//{
//	TotalPolyLines.Push(polyLine);
//}

FArmyDxfModel::FArmyDxfModel()
{
	
}


FArmyDxfModel::~FArmyDxfModel()
{
}

void FArmyDxfModel::AddLayer(TSharedPtr<FArmyFurniture> layer)
{
	AllLayers.Push(layer);
}

void FArmyDxfModel::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	for (auto layer : AllLayers)
	{
		layer->Draw(PDI, View);
	}
}

FArmyDxfLayer::FArmyDxfLayer()
{

}

FArmyDxfLayer::~FArmyDxfLayer()
{

}
