#ifndef _SCENE_H_
#define _SCENE_H_

// std
#include <fstream>
#include <algorithm>
#include <list>

// Qt
#include <QtOpenGL>

// CGAL
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>


#undef min
#undef max

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;

class Scene
{
public:
	typedef Kernel::FT FT;
	typedef Kernel::Point_2 Point;


private:
	// input points and hull
	std::list<Point> m_points;
	std::list<Point> m_hull;
	std::list<Point> m_lower_hull;
	std::list<Point> m_upper_hull;

	Point m_mouse_pos;

public:
	Scene()
	{
	}

	~Scene()
	{
	}

	void clear()
	{
		m_points.clear();
	}

	void add_point(const Point& point)
	{
		m_points.push_back(point);

		// recompute convex hull
		convex_hull();
        lower_hull();
        upper_hull();
	}

	void set_mouse_pos(const Point& pos) {  m_mouse_pos = pos; }


	void render()
	{
		// render points
		::glColor3ub(0, 0, 0); // black color
		::glPointSize(2.0f);
		::glBegin(GL_POINTS);
		for(auto& p : m_points)
		{
			::glVertex2d(p.x(), p.y());
		}
		::glEnd();

		// render hull
		::glColor3ub(255, 0, 0); // red color
		::glLineWidth(3.0f);
		::glBegin(GL_LINE_LOOP);
		for(auto& p : m_hull)
		{
			::glVertex2d(p.x(), p.y());
		}
		::glEnd();

		// render lower hull
		::glColor3ub(0, 0, 255); // blue color
		::glLineWidth(3.0f);
		::glBegin(GL_LINE_STRIP);
		for(auto& p : m_lower_hull)
		{
			::glVertex2d(p.x(), p.y());
		}
		::glEnd();

		// render lower hull
		::glColor3ub(0, 255, 0); // green color
		::glLineWidth(3.0f);
		::glBegin(GL_LINE_STRIP);
		for(auto& p : m_upper_hull)
		{
			::glVertex2d(p.x(), p.y());
		}
		::glEnd();
	}

	//-------------------------------------------------------
	// TO COMPLETE
	//-------------------------------------------------------

	void convex_hull()
	{
		// TODO: recompute hull from points
        m_hull.clear();
        CGAL::convex_hull_2(m_points.begin(), m_points.end(), std::inserter(m_hull, m_hull.begin()));
	}

    void lower_hull()
    {
		// TODO: recompute hull from points
        m_lower_hull.clear();
        CGAL::lower_hull_points_2(m_points.begin(), m_points.end(), std::inserter(m_lower_hull, m_lower_hull.begin()));
    }

    void upper_hull()
    {
		// TODO: recompute hull from points
        m_upper_hull.clear();
        CGAL::upper_hull_points_2(m_points.begin(), m_points.end(), std::inserter(m_upper_hull, m_upper_hull.begin()));
    }
};

#endif // _SCENE_H_
