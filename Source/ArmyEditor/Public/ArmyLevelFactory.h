#pragma once
#include "Object.h"
#include "ArmyLevelFactory.generated.h"


UCLASS()
class UArmyLevelFactory : public UObject
{
	GENERATED_BODY()

public:
	// Begin UFactory Interface
	virtual UObject* FactoryCreateText(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const TCHAR*& Buffer, const TCHAR* BufferEnd, FFeedbackContext* Warn);
	// Begin UFactory Interface	
};