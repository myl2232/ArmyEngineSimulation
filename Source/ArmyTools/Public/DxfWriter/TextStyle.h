#pragma once
#include "ITextStyle.h"
#include "dl_entities.h"
class TextStyle:
	public ITextStyle
{
public:
	friend class Canvas;
	TextStyle();
	virtual ~TextStyle();
public:
	virtual void setTextStyle(const TextStyleDesc& Desc);

	virtual void release() { delete this; };
protected:
	DL_StyleData	 data;
};

