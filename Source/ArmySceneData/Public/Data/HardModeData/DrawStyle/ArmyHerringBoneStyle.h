#pragma once
#include "ArmyBaseEditStyle.h"

class ARMYSCENEDATA_API FArmyHerringBoneStyle :public FArmyBaseEditStyle
{
public :
	FArmyHerringBoneStyle();
	~FArmyHerringBoneStyle(){}
	void CalculateClipperBricks(const TArray<FVector>& OutAreaVertices, const TArray<TArray<FVector>>& InneraHoles);
};