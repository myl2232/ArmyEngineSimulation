#pragma once
#include "ILineType.h"
#include <vector>
#include <string.h>
class LineType :
	public ILineType
{
public:
	LineType();
	virtual ~LineType();
public:
	friend class Canvas;

	virtual void  setLineType(const char* lineName, int flag = 0);
 
	virtual void setCustomLineType(const char*lineName, const char*lineDesc, int dashNum, double*patternNum, int patternLength, int flag = 0);
 
	virtual void release();
private:
	/** 线名称 */
	std::string		lineName;
	/** 线性描述 */
	std::string		lineDesc;
	/** 线标志*/
	int				flag;
	/** 点数量 */
	int				dashNum;

	/** 图案 */
	std::vector<double> pattern;

	/** 是不是自定义的线 */
	bool				custom;
};

