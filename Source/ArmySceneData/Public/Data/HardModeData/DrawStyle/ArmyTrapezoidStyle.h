#pragma once
#include "ArmyBaseEditStyle.h"

class ARMYSCENEDATA_API FArmyTrapezoidStyle :public FArmyBaseEditStyle
{
public:
	FArmyTrapezoidStyle();
	~FArmyTrapezoidStyle(){}
	void CalculateClipperBricks(const TArray<FVector>& OutAreaVertices, const TArray<TArray<FVector>>& InneraHoles);

};