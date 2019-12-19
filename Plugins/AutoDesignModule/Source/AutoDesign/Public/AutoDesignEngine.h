#pragma once
#include "CoreMinimal.h"
#include "AutoDesignModule.h"

class AUTODESIGNMODULE_API AutoDesignEngine
{
public:
	AutoDesignEngine();
	 ~AutoDesignEngine();
public:
	//自动设计
	FString AIDesign(FString HouseData, FString DesignData);
};