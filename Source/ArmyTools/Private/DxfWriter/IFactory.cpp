#include "IFactory.h"
#include "Factory.h"
IFactory::IFactory()
{

}
IFactory::~IFactory()
{

}

IFactory*	IFactory::createFactory()
{
	return new Factory();
}