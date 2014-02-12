#ifndef _SCENE_H_
#define _SCENE_H_

// std
#include <fstream>
#include <algorithm>
#include <list>
#include <string>
#include<vector>
#include<algorithm>
// Qt
#include <QtOpenGL>


//CGAL
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
//#include <CGAL/IO/Color.h>



//#include"cloud_segmentation.h"
#include"types.h"


typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Triangulation_vertex_base_3<Kernel> Vertex_base;

typedef CGAL::Triangulation_data_structure_3<Vertex_base> Tds;
typedef CGAL::Delaunay_triangulation_3<Kernel, Tds> Delaunay;
class Scene
{
public:
//	ScanLidar sl;
	Delaunay delaunay;
	std::vector<Point_d> vpoint;
private:

public:
	Scene();

	~Scene()
	{
	}

	void clear()
	{
	}

	void render()
	{
	}

    bool loadPLY(const std::string& );
    //void loadPLYcertis(const std::string& );
    void compute_delaunay()
    {
	    delaunay.insert(vpoint.begin(),vpoint.end());
	    std::cout << vpoint.size() << std::endl;
//	for(const auto& hpoint : vpoint)
//	{
//		delaunay.insert(hpoint.position);
//	}
    }
};

#endif // _SCENE_H_
