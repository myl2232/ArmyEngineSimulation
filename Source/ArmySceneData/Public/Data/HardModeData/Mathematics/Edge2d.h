#pragma once

class ARMYSCENEDATA_API Edge2d
{
public:
	Edge2d();
	Edge2d(int i0, int i1);
	bool operator <(const Edge2d& edge)const;
	int I0, I1;
};