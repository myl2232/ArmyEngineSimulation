#include "Edge2d.h"
Edge2d::Edge2d()
{
	I0 = -1;
	I1 = -1;
}

Edge2d::Edge2d(int i0, int i1)
{
	I0 = i0;
	I1 = i1;
}


bool Edge2d::operator<(const Edge2d& edge) const
{
	if (I1 < edge.I1)
		return true;
	if (I1 > edge.I1)
		return false;

	return I0 < edge.I0;
}

