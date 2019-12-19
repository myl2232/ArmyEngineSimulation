#include "LineType.h"
#include <string.h>


LineType::LineType()
{
}


LineType::~LineType()
{
}

void  LineType::setLineType(const char* lineName,int flag)
{
	 
	this->lineName = lineName;
	this->flag = flag;
	this->custom = false;
	
}

void LineType::setCustomLineType(const char*lineName, const char*lineDesc, int dashNum, double*patternNum, int patternLength, int flag)
{
	this->lineName = lineName;
	this->flag = flag;
	this->lineDesc = lineDesc;
	this->dashNum = dashNum;
	this->pattern.resize(patternLength);
	memcpy(&this->pattern[0], patternNum, sizeof(double)*patternLength);
	this->custom = true;
}


void LineType::release()
{
	delete this;
}