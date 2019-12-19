#pragma once
#include "ArmyBaseEditStyle.h"

class ARMYSCENEDATA_API FArmyWhirlwindStyle : public FArmyBaseEditStyle
{
public:
	FArmyWhirlwindStyle();
	~FArmyWhirlwindStyle(){}

	void CalculateClipperBricks(const TArray<FVector>& OutAreaVertices, const TArray<TArray<FVector>>& InneraHoles)override;

};