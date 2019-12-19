

#ifndef DXF_SECTIONBASE
#define DXF_SECTIONBASE 1



class dxfFile;
class codeValue;

/// abstract base class for sections. see dxfSection.h
class dxfSectionBase 
{
public:
	dxfSectionBase() {}
	virtual ~dxfSectionBase() {}
	virtual void assign(dxfFile* dxf, codeValue& cv) = 0;
};

#endif
