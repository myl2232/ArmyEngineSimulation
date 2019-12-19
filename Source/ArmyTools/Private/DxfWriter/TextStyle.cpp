#include "TextStyle.h"



TextStyle::TextStyle()
{
}


TextStyle::~TextStyle()
{
}

void TextStyle::setTextStyle(const TextStyleDesc& Desc)
{
	 data.bigFontFile = Desc.bigFontFile;
	 data.bold = Desc.bold;
	 data.fixedTextHeight = Desc.fixedTextHeight;
	 data.flags = Desc.flags;
	 data.italic = Desc.italic;
	 data.lastHeightUsed = Desc.lastHeightUsed;
	 data.name = Desc.name;
	 data.obliqueAngle = Desc.obliqueAngle;
 }

 