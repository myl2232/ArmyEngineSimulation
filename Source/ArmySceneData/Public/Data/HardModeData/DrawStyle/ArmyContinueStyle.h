#pragma once
#include "ArmyBaseEditStyle.h"

class ARMYSCENEDATA_API FArmyContinueStyle :public FArmyBaseEditStyle
{
public:
	FArmyContinueStyle();
	FArmyContinueStyle(FArmyContinueStyle* Copy);
	void CalculateClipperBricks(const TArray<FVector>& OutAreaVertices, const TArray<TArray<FVector>>& InneraHoles)override;
	TSharedPtr<FArmyBaseEditStyle> CopyTo();
	// ~FArmyContinueStyle() {}


};

class ARMYSCENEDATA_API FArmySlopeContinueStyle : public FArmyContinueStyle
{
public:
	FArmySlopeContinueStyle();
	// FArmySlopeContinueStyle(FArmySlopeContinueStyle *Copy);
	TSharedPtr<FArmyBaseEditStyle> CopyTo();
	// ~FArmySlopeContinueStyle() {}
};