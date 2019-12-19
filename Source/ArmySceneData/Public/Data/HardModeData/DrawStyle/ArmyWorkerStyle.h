#pragma once
#include "ArmyBaseEditStyle.h"

class ARMYSCENEDATA_API FArmyWorkerStyle :public FArmyBaseEditStyle
{
public:
	FArmyWorkerStyle();
	~FArmyWorkerStyle(){}

	void CalculateClipperBricks(const TArray<FVector>& OutAreaVertices, const TArray<TArray<FVector>>& InneraHoles)override;


};