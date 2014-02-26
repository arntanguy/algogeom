#include "scene.h"
#include "Ply.hpp"
#include<fstream>
using namespace std;
using I::misc::Ply;

Scene::Scene() 
{
}
Scene::~Scene() 
{
}

//void loadPLYcertis(const std::string& );
bool Scene::load_cam(const std::string& path)
{
	std::ifstream ifs;
	ifs.open(path);
	if(!ifs.is_open())
		return false;
	std::size_t nb_cam;
	ifs >> nb_cam;
	cam.reserve(nb_cam);
	double x,y,z;
	for(int i=0; i<6; i++)
	{
		ifs >> x >> y >> z;
		cam.emplace_back(x,y,z);
	}
	ifs.close();
	return true;
}
bool Scene::loadPLY(const std::string& path)
{

	vhpoint.clear();
	vpoint_indice.clear();
	std::vector< Point_d > points_temp;
	std::vector< Point_d > color_temp;
	std::vector<size_t> camera_index;
	Ply ply;
	if (!ply.open(path)){ return false;}
	for (Ply::ElementsIterator it = ply.elements_begin(); it != ply.elements_end(); ++it)
	{
		const Ply::Element& element = *it;
		if (element.name() != "vertex")
		{
			if (!ply.skip(element)){ ply.close(); return false;}
			continue;
		}
		size_t num_vertices = element.count();
		points_temp.resize(num_vertices);
		color_temp.resize(num_vertices);
		camera_index.resize(num_vertices);
		if (element.num_properties()>6)
		{
			for (size_t i=0; i<num_vertices; i++)
			{
				double x,y,z,r,g,b;
				if((!ply.read(element.property(0), x))
				 ||(!ply.read(element.property(1), y))
				 ||(!ply.read(element.property(2), z))
				 ||(!ply.read(element.property(3), r))
				 ||(!ply.read(element.property(4), g))
			 	 ||(!ply.read(element.property(5), b))
				 ||(!ply.read(element.property(6), camera_index[i])))
				{
					cerr << "error while reading (pos) vertex " 
						<< i+1 << endl;
					ply.close();
					return false;
				}
				for(int k=7;k<element.num_properties();k++)
				{
					double tempp;
					if(!ply.read(element.property(k), tempp))
					{
						cerr << "error while reading attribut "
							<< i+1 << endl;
						ply.close();
						return false;
					}
				}
				points_temp[i] = Point_d(x,y,z);
				color_temp[i] = Point_d(r/255.0,g/255.0,b/255.0);
			}
		}

		else
		{
			cerr << "Unexpected vertex properties in the PLY file" << endl;
			ply.close();
			return false;
		}
	}
	vhpoint.reserve(points_temp.size());
	vpoint_indice.reserve(points_temp.size());
	vcolor_point.reserve(points_temp.size());
	for(int i=0;i<points_temp.size();i++)
	{
		vhpoint.push_back(HPoint(points_temp[i],camera_index[i]));
		vpoint_indice.push_back({points_temp[i],i});
		vcolor_point.push_back(color_temp[i]);
//		if(is_normal_given) normal.push_back(normals_temp[i]);
	}
//		if(label_plane.size()>0) IS_PC_CLUSTERED=true;
//		else IS_PC_CLUSTERED=false;

	points_temp.clear();
	color_temp.clear();
	ply.close();
	return true;
}
//void Scene::loadPLYcertis(const std::string& path)
//{
//	sl.load_points_Certis(path);
//}
   // Ply ply;

   // // Read PLY header
   // if (!ply.open(path)) {
   //     // ...
   // }

   // // Iterate over elements
   // for (Ply::ElementsIterator it = ply.elements_begin();
   //         it != ply.elements_end(); ++it) {
   //     const Ply::Element& element = *it;

   //     if (element.name() == "some_element_to_skip") {
   //         if (!ply.skip(element))
   //             // ...
   //         else
   //             continue;
   //     } else {
   //         const size_type& num_elements = element.count();

   //         for (size_type i = 0; i != num_elements; ++i) {
   //             for (Ply::PropertiesIterator it2 =
   //                     element.properties_begin();
   //                     it2 = element.properties_end(); ++it2) {
   //                 const Ply::Property& property = *it2;

   //                 if (it2->name() == "some_property_to_read") {
   //                     double d;
   //                     ply.read(*it2, d);
   //                     // ...
   //                 } else {
   //                     if (!ply.skip(*it2))
   //                         // ...
   //                     else
   //                         continue;
   //                 }
   //             }
   //         }
   //     }
   // }

   //
// ply.close();
void Scene::compute_Knearest_neighbors(const std::size_t& K)
{
//		cout<<"Computing the K-nearest neighbors";

		//1-Neighborhood computation and reset the attributes of the structure points
		std::map<Point_d,std::size_t> map_indice_point;
		std::list<Point_d> list_points;

		for(std::size_t i=0;i<vhpoint.size();++i){
			map_indice_point[vhpoint[i].position]=i;
			list_points.push_back(vhpoint[i].position);
		}
		Tree tree(list_points.begin(), list_points.end());

		for(std::size_t i=0;i<vhpoint.size();++i){
			Point_d query=vhpoint[i].position;
			Neighbor_search search(tree, query, K+1);

			std::vector<std::size_t> index_of_neighbors;
			for(Neighbor_search::iterator it = search.begin(); it != search.end(); ++it){
				//if(std::sqrt(it->second)<0.5){
					std::map<Point_d,std::size_t>::iterator iter =map_indice_point.begin();
					iter= map_indice_point.find(it->first);
					if( iter != map_indice_point.end() && iter->second!=i ) index_of_neighbors.push_back(iter->second);
				//}else{break;}
			}
			spherical_neighborhood.push_back(index_of_neighbors);
		}

		//2-creation of the bounding box
//		BBox_scan = CGAL::bounding_box(list_points.begin(), list_points.end());
//		BBox_diagonal=sqrt( pow(BBox_scan.xmax()-BBox_scan.xmin(),2) +  pow(BBox_scan.ymax()-BBox_scan.ymin(),2) +  pow(BBox_scan.zmax()-BBox_scan.zmin(),2) );
//	
//		cout<<endl<<endl;
//
//		return true;
}
void Scene::compute_normal()
{
		const std::size_t nb_max_neighbors=10;
		for(std::size_t k=0;k<vhpoint.size();++k){

			std::list<Point_d> list_pts;
			list_pts.push_back(vhpoint[k].position);

			for(std::size_t n=0,
					end=std::min(nb_max_neighbors,spherical_neighborhood[k].size());
					n<end;++n){
					list_pts.push_back(vhpoint[spherical_neighborhood[k][n]].position);
			}
			
			Plane_3 plane_temp;
			double average_distance=linear_least_squares_fitting_3(list_pts.begin(),list_pts.end(),plane_temp, CGAL::Dimension_tag<0>());

			Vector_3 normal_temp=plane_temp.orthogonal_vector();

		//	int 
		/*	for(int n=0;n<(int)spherical_neighborhood[k].size();n++){
				if(k>spherical_neighborhood[k][n] && n<nb_max_neighbors){
					int index_neighbor=spherical_neighborhood[k][n];
					if(vhpoint[index_neighbor].normal * normal_temp <0 ) normal_temp=-normal_temp;
				break;
				}
				else break;
			}*/

			FT normal_temp_norm=1/sqrt(normal_temp.squared_length());
			normal_temp = normal_temp*normal_temp_norm;
		//normal re-orientation
		//using camera position
		//         /                     |
		//camera <[  -  - - - - -    <---|  the normal "watch" the cam
		//         \                     |
		//le produit scalaire de la normal avec le vectrue point/cam doit etre positif
		//si negatif, alors *=-1
			if(normal_temp*(cam[vhpoint[k].camera_index]-vhpoint[k].position)<0)
				normal_temp = normal_temp*-1;
			vhpoint[k].normal = normal_temp;
		}


}
void Scene::get_all_normal(std::vector<float>& normal_vec3f)
{
	normal_vec3f.clear();
	normal_vec3f.reserve(3*vhpoint.size());
	for(const HPoint& hp : vhpoint)
	{
		for(auto begin = hp.normal.cartesian_begin(),end=hp.normal.cartesian_end();
				begin!=end;++begin)
		{
			normal_vec3f.push_back(*begin);
		}
	}
}
void Scene::get_all_normal_vec4(std::vector<float>& normal)
{
	normal.clear();
	normal.reserve(4*vhpoint.size());
	for(const HPoint& hp : vhpoint)
	{
		for(auto begin = hp.normal.cartesian_begin(),end=hp.normal.cartesian_end();
				begin!=end;++begin)
		{
			normal.push_back(*begin);
		}
		normal.push_back(0);
	}
}

void Scene::compute_gauss(std::vector<std::size_t>& north_hemisphere,
						  std::vector<std::size_t>& south_hemisphere,
						  const std::vector<float>& normal,
						  const std::size_t& rows,
						  const std::size_t& cols,
						  const double& beta,
						  const double& alpha)
{
	//
	//
	//       x z = + alpha : centre  of projection
	//       |
	//       |
	// ______|
	//        \
	//       | \
	//          \
	//		 |
	//____________________
	//       |            \
	//                     \
	//       |              \
	//       x               \  z = -beta : plan sur lequel on projette la sphere
	//                        \  (projection stereographique) 
	//_________________________\
	//
	// on projette la sphere unite sur le plan:
	//
	// soit M = (x,y,z) un point de la sphere.
	//      P = (0,0,alpha) le centre de projection.
	//      delta = P + lambda(M-P) la droite PM
	//      PI = -beta le plan
	//
	//      I \in \delta\inter\PI
	//      I verifie l'equation Pz + lambda(Mz-Pz) = -beta
	//      D'ou alpha + lambda(z-alpha) = -beta
	//      D'ou lambda = - (beta+alpha) / (z-alpha)
	//      			= (alpha+beta)/(alpha-z)
	//
	//      D'ou I = P + (alpha+beta)/(alpha-z) * (M-P);
	//
	//		Si on remplace alpha par -alpha et beta par -beta,
	//		on obtient:
	//         lambda =  (-alpha-beta) / (-alpha-z)
	//      			= (alpha+beta)/(alpha+z)
	//      D'ou I = P + (alpha+beta)/(alpha+z) * (M-P);
	//
	//
	//		
	//
	//
	//
		north_hemisphere.clear();
		north_hemisphere.resize(pow(rows,2),0);
		south_hemisphere.clear();
		south_hemisphere.resize(pow(rows,2),0);
	double tmp;
	for(auto begin = normal.begin(),end=normal.end();begin!=end;++begin)
	{
		const float& x = *begin;
		const float& y = *++begin;
		const float& z = *++begin;
//		std::cout << x << ' ' << y << ' ' << z << std::endl;
		if(z<0)
		{
			tmp = (beta+alpha)/(alpha-z);
			south_hemisphere[rows*floor(tmp*y)+rows*rows/2+floor(tmp*x)]++;
//			imageAtomicAdd(south_hemisphere, ivec2(floor((beta+1f)/(1f-normal.z)*normal.xy)), 1u);
    	} else {
			tmp = (beta+alpha)/(alpha+z);
			north_hemisphere[rows*floor(tmp*y)+floor(tmp*x)+rows*rows/2]++;
	    	//imageAtomicAdd(north_hemisphere, floor((beta+1f)/(1f+normal.z)*normal.xy),1u);
    	}
	}
}
void Scene::compute_gauss2(std::vector<std::size_t>& north_hemisphere,
						  std::vector<std::size_t>& south_hemisphere,
						  std::vector<std::vector<std::size_t>>& north_hemisphere_idx,
						  std::vector<std::vector<std::size_t>>& south_hemisphere_idx,
						  const std::vector<float>& normal,
						  const std::size_t& rows,
						  const std::size_t& cols,
						  const double& beta,
						  const double& alpha)
{
		north_hemisphere.clear();
		north_hemisphere.resize(pow(rows,2),0);
		south_hemisphere.clear();
		south_hemisphere.resize(pow(rows,2),0);

		std::vector<std::size_t> vcase;
		vcase.reserve(normal.size()/3);
		std::vector<bool> is_south;
		is_south.reserve(normal.size()/3);
	double tmp;
	std::size_t case_tmp;
	//compute the gauss sphere
	const std::size_t rowrow_s_2 = rows*rows/2;
	const double alphabeta = alpha+beta;
	for(auto begin = normal.begin(),end=normal.end();begin!=end;++begin)
	{
		const float& x = *begin;
		const float& y = *++begin;
		const float& z = *++begin;
		if(z<0)
		{
			tmp = alphabeta/(alpha-z);
			case_tmp = rows*floor(tmp*y)+rowrow_s_2+floor(tmp*x);
			south_hemisphere[case_tmp]++;
			vcase.push_back(case_tmp);
			is_south.push_back(true);
    	} else {
			tmp = alphabeta/(alpha+z);
			case_tmp = rows*floor(tmp*y)+rowrow_s_2+floor(tmp*x);
			north_hemisphere[case_tmp]++;
			vcase.push_back(case_tmp);
			is_south.push_back(false);
    	}
	}

	//add a ref to the normal
	north_hemisphere_idx.resize(north_hemisphere.size());
	south_hemisphere_idx.resize(south_hemisphere.size());
	for(int i=0; i<north_hemisphere.size(); ++i)
	{
		north_hemisphere_idx[i].reserve(north_hemisphere[i]);
		south_hemisphere_idx[i].reserve(south_hemisphere[i]);
	}

	std::size_t cpt=0;
	auto it_is_south = is_south.begin()-1;
	auto it_vcase = vcase.begin()-1;
	for(const std::size_t end=vcase.size(); cpt<end;++cpt)
	{
		if(*++it_is_south)
		{
			south_hemisphere_idx[*++it_vcase].push_back(cpt);
		}
		else
		{
			north_hemisphere_idx[*++it_vcase].push_back(cpt);
		}
	}
}
