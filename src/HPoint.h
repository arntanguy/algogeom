#ifndef HPOINT_HEADER__
#define HPOINT_HEADER__


#include"types.h"


class HPoint
{
	public:
	Point_d position;
	int camera_index;
	Vector_3 normal;
//	private:
	public:
	HPoint():camera_index(-1)
	{
	}
	HPoint(const Point_d& p, int camera_idx = -1):position(p), camera_index(camera_idx)
												  //,normal(nullptr)
	{

	}

};


#endif

