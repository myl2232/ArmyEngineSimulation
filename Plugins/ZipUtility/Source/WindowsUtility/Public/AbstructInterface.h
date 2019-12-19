#pragma once

#include "CoreMinimal.h"

enum class WINDOWSFILEUTILITY_API EDelegateInterfaceType
{
	LIST_INTERFACE = 0,
	WATCH_INTERFACE = 1
};

class WINDOWSFILEUTILITY_API IAbstructInterface
{
public:
	IAbstructInterface(const EDelegateInterfaceType InType)
		:Type(InType){}
public:
	
	EDelegateInterfaceType GetType() { return Type; }
protected:
	EDelegateInterfaceType Type;
};

