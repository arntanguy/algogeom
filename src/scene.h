#ifndef _SCENE_H_
#define _SCENE_H_

// std
#include <fstream>
#include <algorithm>
#include <list>
#include <string>
#include<vector>
#include <set>
#include<algorithm>


//CGAL
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>
//#include <CGAL/IO/Color.h>



//#include"cloud_segmentation.h"
#include"types.h"
#include"HPoint.h"
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Triangulation_vertex_base_with_info_3<unsigned int, Kernel> Vertex_base;
//typedef CGAL::Triangulation_vertex_base_3<Kernel> Vertex_base;

typedef CGAL::Triangulation_data_structure_3<Vertex_base> Tds;
typedef CGAL::Delaunay_triangulation_3<Kernel, Tds> Delaunay;


#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/ml/ml.hpp>


class Scene
{
public:
//	ScanLidar sl;
	Delaunay delaunay;
//	std::vector<Point_d> vpoint;
	std::vector<Point_d> vcolor_point;
	std::vector<Point_d> cam;
        std::vector<std::pair<Point_d, unsigned int>> vpoint_indice;
	std::vector<HPoint> vhpoint;
	std::vector<std::vector<std::size_t>> spherical_neighborhood;
private:

public:
	Scene();
	~Scene();
	void clear()
	{
	}

	void render()
	{
	}

    bool loadPLY(const std::string& path_to_ply);
    bool load_cam(const std::string& path_to_cam);
	void compute_Knearest_neighbors(const std::size_t& K);
	void compute_normal();
	void get_all_normal_vec4(std::vector<float>& normal);
	void get_all_normal(std::vector<float>& normal_vec3f);
	//using stereographic projection to save the gauss sphere
	void compute_gauss(std::vector<std::size_t>& hn,
						  std::vector<std::size_t>& hs,
						  const std::vector<float>& normal,
						  const std::size_t& rows,
						  const std::size_t& cols,
						  const double& beta,
						  const double& alpha);
	void compute_gauss2(std::vector<std::size_t>& north_hemisphere,
						  std::vector<std::size_t>& south_hemisphere,
						  std::vector<std::vector<std::size_t>>& north_hemisphere_idx,
						  std::vector<std::vector<std::size_t>>& south_hemisphere_idx,
						  const std::vector<float>& normal,
						  const std::size_t& rows,
						  const std::size_t& cols,
						  const double& beta,
						  const double& alpha);
	void compute_gauss3(std::vector<std::size_t>& north_hemisphere,
						  std::vector<std::vector<std::size_t>>& north_hemisphere_idx,
						  const std::vector<float>& normal,
						  const std::size_t& rows,
						  const std::size_t& cols,
						  const double& beta,
						  const double& alpha);
//void get_distribution_plan(const std::size_t& normal_idx, const std::vector<std::size_t>& vidx, const std::vector<std::vector<std::size_t>>& north, std::vector<std::size_t>& normal_dist, const std::size_t& nb_bins, const std::size_t& nb_min_dot_per_bin);
void get_distribution_plan(const std::vector<std::size_t>& v_normal_idx, const std::vector<std::vector<std::size_t>>& v_vidx, const std::vector<std::vector<std::size_t>>& north, std::vector<std::size_t>& normal_dist, const std::size_t& nb_bins, const std::size_t& nb_min_dot_per_bin);
    void normalize_gauss(const std::vector<std::size_t> &hn, const std::vector<std::size_t>& hs, cv::Mat_<float>& mhn, cv::Mat_<float>& mhs);
    void normals_from_gauss(const cv::Mat_<float>& gauss_map, std::vector<std::size_t>& found_normals, std::vector<std::vector<std::size_t>>& found_normals_clusters);
    void threshold_gauss(const cv::Mat_<float>& src, cv::Mat& dst, const float& threshold);
    void threshold_gauss(const cv::Mat_<float>& src, cv::Mat& dst, const float& threshold, const int& localsize);
    cv::Rect compute_bounding_box(const std::vector<cv::Point> &contour);

    //void loadPLYcertis(const std::string& );
    void compute_delaunay()
    {
	    delaunay.insert(vpoint_indice.begin(),vpoint_indice.end());
//	    std::cout << vpoint.size() << std::endl;
	    std::ofstream ofs;
	    ofs.open("test.off");
	    ofs << "OFF" << std::endl;
	    ofs << vpoint_indice.size() << ' ' << delaunay.number_of_finite_facets() << " 0" << std::endl;
	    for(const auto& a : vpoint_indice)
		    ofs << a.first.x() << ' ' << a.first.y()
			    << ' ' << a.first.z() << std::endl;
	    unsigned int nb_face =0;
	    for(decltype(delaunay.finite_facets_begin()) facet = delaunay.finite_facets_begin(),
			    end = delaunay.finite_facets_end();
			    facet!=end;++facet)
	    {
//		std::cerr << facet->second << " ";
//		it->first->vertex( (it->second+k)%4 )->point(), k=1,2,3//
//		std::cerr << facet->first->vertex(0)->point().x() << " ";
//		std::cerr << facet->first->vertex(1)->point().x() << " ";
//		std::cerr << facet->first->vertex(3)->point().x() << " ";
//
//		std::cout << facet->first->vertex(0)->info()<<std::endl;
//

		    const auto& p1 = facet->first->vertex( (facet->second+1)%4 )->point();
		    const auto& p2 = facet->first->vertex( (facet->second+2)%4 )->point();
		    const auto& p3 = facet->first->vertex( (facet->second+3)%4 )->point();
//			    std::cerr << CGAL::cross_product(p2-p1,p3-p1).squared_length()<<" ";
		    if(CGAL::cross_product(p2-p1,p3-p1).squared_length()<.0001)
		    {
			++nb_face;
			const int& i1 = facet->first->vertex( (facet->second+1)%4 )->info();
			const int& i2 = facet->first->vertex( (facet->second+2)%4 )->info();
			const int& i3 = facet->first->vertex( (facet->second+3)%4 )->info();
			const Point_d& p1 = vcolor_point[i1];
			const Point_d& p2 = vcolor_point[i2];
			const Point_d& p3 = vcolor_point[i3];
		ofs << 3 << ' '
			 << i1 << ' '
			 << i2 << ' '
			 << i3 << ' '
			 << 1.0 << ' '
			 << (p1.z()) << ' '
			 << (p1.y()) << ' '
			 << (p1.x()) << ' '
			 //<< ((p1.z()+p2.z()+p3.z())/3.0) << ' '
			 //<< ((p1.y()+p2.y()+p3.y())/3.0) << ' '
			 //<< ((p1.x()+p2.x()+p3.x())/3.0) << ' '
			 <<std::endl;
		}
	    }
		    std::cerr << nb_face << std::endl;
	    ofs.close();
//	for(const auto& hpoint : vpoint)
//	{
//		delaunay.insert(hpoint.position);
//	}
    }
};

#endif // _SCENE_H_


