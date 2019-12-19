#pragma once

class FPrimitiveDrawInterface;
class FSceneView;
class FArmyPoint;
class FArmyLine;
class FArmyPolyline;
class FArmyFurniture;

class FArmyLWPolyLine
{
public:
	struct LinePointInfo
	{
		LinePointInfo()
		{
			pos = FVector::ZeroVector;
			_convexAngle = 0.0f;
		}
		LinePointInfo(FVector InPos, float InAngle)
		{
			pos = InPos;
			_convexAngle = InAngle;
		}
		FVector pos;
		float  _convexAngle;
	};
	TArray<LinePointInfo> Vertices;
};

class FArmyDxfLayer
{
public:
	FArmyDxfLayer();
	~FArmyDxfLayer();


	TArray<FArmyLWPolyLine> PloyLines;
};

class ARMYSCENEDATA_API FArmyDxfModel
{
public:
	FArmyDxfModel();
	~FArmyDxfModel();
	void AddLayer(TSharedPtr<FArmyFurniture> layer);
	void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);
	TArray<TSharedPtr<FArmyFurniture>> GetAllLayers() { return AllLayers; }
	FBox boundingBox;
protected:

	TArray<TSharedPtr<FArmyFurniture>> AllLayers;
};

