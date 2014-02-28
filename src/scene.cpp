#include "scene.h"
#include "Ply.hpp"
#include <fstream>
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
		if(element.num_properties()==3)
		{
			points_temp.resize(num_vertices);
			for (size_t i=0; i<num_vertices; i++)
			{
				double x,y,z;
				if((!ply.read(element.property(0), x))
				 ||(!ply.read(element.property(1), y))
				 ||(!ply.read(element.property(2), z)))
				{
					cerr << "error while reading (pos) vertex " 
						<< i+1 << endl;
					ply.close();
					return false;
				}
				points_temp[i] = Point_d(x,y,z);
			}
		}
		else if (element.num_properties()>6)
		{
			points_temp.resize(num_vertices);
			color_temp.resize(num_vertices);
			camera_index.resize(num_vertices);
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
	if(camera_index.size()&&vcolor_point.size())
	{
		for(int i=0;i<points_temp.size();i++)
		{
			vhpoint.push_back(HPoint(points_temp[i],camera_index[i]));
			vpoint_indice.push_back({points_temp[i],i});
			vcolor_point.push_back(color_temp[i]);
	//		if(is_normal_given) normal.push_back(normals_temp[i]);
		}
	}
	else
	{
		for(int i=0;i<points_temp.size();i++)
		{
			vhpoint.push_back(HPoint(points_temp[i],0));
			vpoint_indice.push_back({points_temp[i],i});
//			if(is_normal_given) normal.push_back(normals_temp[i]);
		}
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
			if(cam.size()!=0)
			{
				if(normal_temp*(cam[vhpoint[k].camera_index]-vhpoint[k].position)<0)
					normal_temp = normal_temp*-1;
			}
			else
			{
				if(normal_temp*Vector_3(0,0,1)<0)
					normal_temp = normal_temp*-1;
			}
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
}void Scene::compute_gauss2(std::vector<std::size_t>& north_hemisphere,
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
	north_hemisphere_idx.clear();
	south_hemisphere_idx.clear();
	//add a ref to the normal
	north_hemisphere_idx.resize(north_hemisphere.size());
	south_hemisphere_idx.resize(south_hemisphere.size());
	for(int i=0; i<north_hemisphere.size(); ++i)
	{
		north_hemisphere_idx[i].reserve(north_hemisphere[i]);
		south_hemisphere_idx[i].reserve(south_hemisphere[i]);
	}

	std::size_t cpt=0;
	auto it_is_south = is_south.begin();
	auto it_vcase = vcase.begin();
	for(const std::size_t end=vcase.size(); cpt<end;++cpt)
	{
		if(*it_is_south)
		{
			south_hemisphere_idx[*it_vcase].push_back(cpt);
		}
		else
		{
			north_hemisphere_idx[*it_vcase].push_back(cpt);
//		if(cpt==266979&&beta==180&&alpha==1)
//		{
//			std::cout << "aaaaaaa " << *it_is_south<<' ' <<*it_vcase << ' ' << rowrow_s_2<< ' ' << vhpoint[cpt].normal.z()<<' '<<vhpoint[*north_hemisphere_idx[*it_vcase].rbegin()].normal.z()<<std::endl;
//			std::cout << cpt << ' ' << *north_hemisphere_idx[*it_vcase].rbegin()<<std::endl<<std::endl;
//		}
//			if(*it_vcase==rowrow_s_2)
//			{
//				std::cout << vhpoint[cpt].normal.x() << ' '
//				<< vhpoint[cpt].normal.y() << ' '
//				<< vhpoint[cpt].normal.z() << std::endl;
//
//				if(!(vhpoint[cpt].normal.z()>.7))
//				{
//					std::cout << "azertyuiopqsdfghjklm"<<std::endl;
//					exit(0);
//				}
//			}
		}

		++it_is_south;
		++it_vcase;
	}
	std::cout << "cpt_max " << cpt<<std::endl;
}
void Scene::compute_gauss3(std::vector<std::size_t>& north_hemisphere,
						  std::vector<std::vector<std::size_t>>& north_hemisphere_idx,
						  const std::vector<float>& normal,
						  const std::size_t& rows,
						  const std::size_t& cols,
						  const double& beta,
						  const double& alpha)
{
		north_hemisphere.clear();
		north_hemisphere.resize(pow(rows,2),0);

		std::vector<std::size_t> vcase;
		vcase.reserve(normal.size()/3);
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
			case_tmp = rows*floor(tmp*-y)+rowrow_s_2+floor(tmp*-x);
    	} else {
			tmp = alphabeta/(alpha+z);
			case_tmp = rows*floor(tmp*y)+rowrow_s_2+floor(tmp*x);
    	}
		north_hemisphere[case_tmp]++;
		vcase.push_back(case_tmp);
	}
	north_hemisphere_idx.clear();
	//add a ref to the normal
	north_hemisphere_idx.resize(north_hemisphere.size());
	for(int i=0; i<north_hemisphere.size(); ++i)
	{
		north_hemisphere_idx[i].reserve(north_hemisphere[i]);
	}

	std::size_t cpt=0;
	auto it_vcase = vcase.begin();
	for(const std::size_t end=vcase.size(); cpt<end;++cpt)
	{
		north_hemisphere_idx[*it_vcase].push_back(cpt);
		++it_vcase;
	}
}
void Scene::get_distribution_plan(const std::vector<std::size_t>& v_normal_idx, const std::vector<std::vector<std::size_t>>& v_vidx, const std::vector<std::vector<std::size_t>>& north, std::vector<std::size_t>& normal_dist, const std::size_t& nb_bins, const std::size_t& nb_min_dot_per_bin)
{
	std::ostringstream oss;
	std::ofstream ofs;
	double tmp;
	double max;
	double min;
	for(std::size_t indice_normal = 0,indice_normal_end=v_normal_idx.size();indice_normal!=indice_normal_end;++indice_normal)
	{
		const auto& normal_idx = v_normal_idx[indice_normal];
		const auto& vidx = v_vidx[indice_normal];
	
		max=std::numeric_limits<decltype(max)>::min();
		min=std::numeric_limits<decltype(min)>::max();
		if(north[normal_idx].size()==0)
		{
			std::cout << "bad normal_idx" << std::endl;
			exit(0);
		}
		const auto& normal = vhpoint[north[normal_idx][0]].normal;
		std::cout << normal.x() << " " << normal.y() << ' ' << normal.z()<<std::endl;
		for(const auto&	idx : vidx)
		{
			for(const auto& p_idx : north[idx])
			{
				//tmp = normal * (vhpoint[p_idx].position - CGAL::ORIGIN);
				tmp = normal * (vhpoint[p_idx].position - Point_d(0,0,0));
	//			std::cout<<vhpoint[p_idx].position.z()<<std::endl;
				if(tmp<min)
					min=tmp;
				if(tmp>max)
					max=tmp;
			}
		}
		normal_dist.clear();
		normal_dist.resize(nb_bins,0);
	//	max+=std::numeric_limits<decltype(max)>::epsilon();//to open the interval
		const double ratio = (nb_bins-1)*1./(max-min);
		//return;
		for(const auto&	idx : vidx)
		{
			for(const auto& p_idx : north[idx])
			{
	//			std::cout <<floor((normal * (vhpoint[p_idx].position - CGAL::ORIGIN)-min)*ratio)<<std::endl;;
				//normal_dist[floor((normal * (vhpoint[p_idx].position - CGAL::ORIGIN)-min)*ratio)]++;
				normal_dist[floor((normal * (vhpoint[p_idx].position - Point_d(0,0,0))-min)*ratio)]++;
			}
		}
	//	for(auto& a : normal_dist)
	//	{
	//		if(a<nb_min_dot_per_bin)
	//			a=0;
	//	}
	//	//XXX
		
	//	return;
	
	//#define mon_transform(src,dst,lambda) std::transform(src.begin(), src.end(), dst.begin(), lambda);
		std::cout << min << ' ' << max << std::endl;
		std::vector<float> normalized_histo(normal_dist.size());
		double ratio_hist = 1./ *std::max_element(normal_dist.begin(), normal_dist.end());
		std::vector<float> normal_max(normal_dist.size());
		double seuil = .1;
		//normalize the histogram
		std::transform(normal_dist.begin(), normal_dist.end(), normalized_histo.begin(), [&ratio_hist](decltype(*normal_dist.begin()) val){return val*ratio_hist;});
		//seuil it
		std::transform(normalized_histo.begin(),normalized_histo.end(), normal_max.begin(), [&seuil](decltype(*normalized_histo.begin()) val){return val<seuil ? 0 : val;});
		//get the max value of all the blob
	//	std::replace_if(normal_max.begin(), normal_max.end(), 
		auto previews_value = normal_max.begin();
		for(auto it = normal_max.begin(),end=normal_max.end(); it!=end; ++it)
		{
			if(*it==0)
			{
				previews_value = it;
				continue;
			}
			if(*it>*previews_value)
			{
				*previews_value = 0;
				previews_value=it;
			}
			else
			{
				*it=0;
			}
		}
	
		std::map<std::size_t,std::size_t> correspondance;
		for(std::size_t i=0,j=0,end=normal_max.size(); i!=end;++i)
		{
			if(normal_max[i])
			{
				correspondance[i]=j++;
			}
		}
		std::vector<std::array<float, 4>> dist(correspondance.size(),
				{std::numeric_limits<float>::max(),
				std::numeric_limits<float>::min(),
				std::numeric_limits<float>::max(),
				std::numeric_limits<float>::min()});
		
		std::remove_const<std::remove_reference<decltype(normal)>::type>::type vect_tmp(normal.y(),-normal.x(),0);
		const auto nu = vect_tmp*(1./sqrt(vect_tmp.squared_length()));
	//	assert(nu.squared_length()==1);
		vect_tmp = CGAL::cross_product(normal,nu);
		const auto eta = vect_tmp*(1./sqrt(vect_tmp.squared_length()));
	//	assert(eta.squared_length()==1);
		//(normal,nu,eta) = repere on the plan
		std::cout << "||nu|| = " << nu.squared_length() << std::endl;
		std::cout << "||eta|| = " << eta.squared_length() << std::endl;
		std::cout << "||normal|| = " << normal.squared_length() << std::endl;
		for(const auto&	idx : vidx)
		{
			for(const auto& p_idx : north[idx])
			{
				const auto& pos = vhpoint[p_idx].position;
				const auto vpos = pos - Point_d(0,0,0);
				std::size_t i = floor(((normal * vpos)-min)*ratio);
				if(normal_max[i]!=0)
				{
					auto& array =  dist[correspondance[i]];
					auto& eta_min = array[0];
					auto& eta_max = array[1];
					auto& nu_min = array[2];
					auto& nu_max = array[3];
					const auto eta_val = eta * vpos; 
					const auto nu_val  = nu  * vpos; 
					if(eta_val>eta_max)
					{
						eta_max=eta_val;
					}
					if(eta_val<eta_min)
					{
						eta_min=eta_val;
					}
					if(nu_val>nu_max)
					{
						nu_max=nu_val;
	//					std::cout << nu_max << ' ' << vpos.squared_length() << ' ' << nu.squared_length()<<std::endl;
					}
					if(nu_val<nu_min)
					{
						nu_min=nu_val;
					}
				}
			}
		}
		std::vector<float> normal_val(dist.size());
		for(const auto& a : correspondance)
		{
			normal_val[a.second] = a.first/ratio+min;
		}
		std::vector<std::array<std::remove_const<decltype(eta)>::type,4>> plan;
		plan.reserve(normal_val.size());
		for(std::size_t i=0; i<normal_val.size(); ++i)
		{
			auto& p = plan[i];
			const auto& etanu = dist[i];
			const auto& nval = normal_val[i];
			std::cout << etanu[0] << ' ' << etanu[1] << ' ' << etanu[2] << ' ' << etanu[3] << std::endl;
			plan.push_back({
			normal * nval + eta * etanu[0] + nu * etanu[2],
			normal * nval + eta * etanu[0] + nu * etanu[3],
			normal * nval + eta * etanu[1] + nu * etanu[3],
			normal * nval + eta * etanu[1] + nu * etanu[2]});
		}
		oss.str("");
		oss << "test_plan_" << indice_normal << ".off";
		ofs.open(oss.str().c_str());
//		ofs.open("test_plan.off");
		ofs << "OFF" << std::endl << plan.size()*4 << ' ' << plan.size() << ' ' << 0 << std::endl;
		for(const auto& a : plan)
		{
			for(const auto& b : a)
			{
				ofs << b.x() << ' ' << b.y() << ' ' << b.z() << std::endl;
			}
		}
		for(int i=0,end=plan.size()*4;i!=end;i+=4)
		{
			ofs << 4 << ' ' << i << ' ' << i+1 << ' ' << i+2 << ' ' << i+3 << ' '  << fabs(normal.x()) << ' ' << fabs(normal.y()) << ' ' << fabs(normal.z())<<std::endl;//àno color
		}
		ofs.close();
	//	for(const auto& a:  normal_max)
	//	{
	//		if(a!=0)
	//		{
	//			for(const auto&	idx : vidx)
	//			{
	//				for(const auto& p_idx : north[idx])
	//				{
	//					if(i == floor((normal * (vhpoint[p_idx].position - Point_d(0,0,0))-min)*ratio))
	//					{
	//
	//					}
	//				}
	//			}
	//		}
	//		++i;
	//	}
		
		
	
	//	normal_max.reserve(normal_dist.size());
	//	auto ptr_pred = normal_dist.begin();
	//	auto ptr_curr = ptr_pred+1;
	//	auto ptr_next = ptr_curr+1;
	//	auto ptr_end = normal_dist.end();
	//	normal_max.push_back((*ptr_pred*2+*ptr_curr)/3);
	//	while(ptr_next!=ptr_end)
	//	{
	//		normal_max.push_back((*ptr_pred+2**ptr_curr+*ptr_next)/4);
	//		++ptr_next;
	//		++ptr_curr;
	//		++ptr_pred;
	//	}
	//	normal_max.push_back((*ptr_pred+*ptr_curr*2)/3);
	//	std::size_t previews_val(0);
	//	for(auto& a : normal_max)
	//	{
	//		if(previews_val>a)
	//		{
	//			previews_val=a;
	//			a=0;
	//		}
	//		else
	//		{
	//			previews_val=a;
	//		}
	//	}
		//
		//
	//	std::ofstream ofs;
		oss.str("");
		oss << "normal_dist_" << indice_normal << ".csv";
		ofs.open(oss.str().c_str());
		std::size_t i = 0, j = 0;
	//	for(const auto& a : normal_dist)
		for(const auto& a : normalized_histo)
		{
			while(normal_max[j]==0&&j<normal_max.size())
			{
				++j;
			}
			if(j<normal_max.size())
			{
				ofs << (i*(max-min)/(nb_bins-1)+min) << ';' << a << ";"
					<< (j*(max-min)/(nb_bins-1)+min) << ';' << normal_max[j]<<std::endl;
				++j;
			}
			else
			{
				ofs << (i*(max-min)/(nb_bins-1)+min) << ';' << a << std::endl;
			}
			++i;
		}
		ofs.close();
	}
}
   
void Scene::normalize_gauss(const std::vector<std::size_t> &hn, const std::vector<std::size_t>& hs, cv::Mat_<float>& mhn, cv::Mat_<float>& mhs)
{
	auto it_vhn = hn.begin();
	auto it_vhs = hs.begin();
	std::size_t max(0);
	for(auto vend = hn.end();it_vhn!=vend;++it_vhn,++it_vhs)
	{
		if(*it_vhn>max)
			max=*it_vhn;
		if(*it_vhs>max)
			max=*it_vhs;
	}
	it_vhn = hn.begin();
	it_vhs = hs.begin();
	double inv_max = 1.0/max;
	for(auto ithn = mhn.begin(),iths=mhs.begin(),end=mhn.end();ithn!=end;
			++ithn,++iths,++it_vhn,++it_vhs)
	{
		*ithn = *it_vhn * inv_max;
		*iths = *it_vhs * inv_max;
	}
}

cv::Rect Scene::compute_bounding_box(const std::vector<cv::Point> &contour)
{
    assert(contour.size() > 0);
    int minx=contour[0].x, maxx = contour[0].x, miny = contour[0].y, maxy = contour[0].y;
    for(auto& p : contour) {
        minx = std::min(minx, p.x);
        maxx = std::max(maxx, p.x);
        miny = std::min(miny, p.y);
        maxy = std::max(maxy, p.y);
    } 
    return cv::Rect(minx, miny, maxx-minx+1, maxy-miny+1);
}

void fillRect(const cv::Mat& binary_mask, cv::Mat_<float>& mat, const cv::Rect& rect) {
    if(rect.width > 0 && rect.height > 0) {
        for(int x=rect.x; x<=rect.x+rect.width; x++) {
            for(int y=rect.y; y<=rect.y+ rect.height; y++) {
                if(binary_mask.at<uchar>(x, y) != 0)
                    mat.at<float>(x, y) = 0.f;
            }
        }
    }
}

void fillRect(cv::Mat_<float>& mat, const cv::Rect& rect) {
    for(int x=rect.x; x<rect.x+rect.width; x++) {
        for(int y=rect.y; y<rect.y+ rect.height; y++) {
            mat.at<float>(x, y) = 0.f;
        }
    }
}

void normal_from_blob(const cv::Mat_<float>& gauss, const cv::Mat& thresholded_bin, const cv::Rect& bounding_box, int& normal, float& max)
{
    const cv::Mat& g = gauss(bounding_box);
    const cv::Mat& b = thresholded_bin(bounding_box);
    float m;
    cv::Point mpoint;
    for(int x=0; x<g.rows; x++) {
        for(int y=0; y<g.cols; y++) {
            if(b.at<uchar>(x,y) != 0) {
                m = std::max(m, g.at<float>(x,y));
                mpoint.x = x; mpoint.y = y;
            }
        }
    }
    cv::Point normal_pos = bounding_box.tl() + mpoint;
    normal = normal_pos.y * gauss.cols + normal_pos.x;
    max = m;
}

cv::Rect create_rect(int x, int y, int width, int height, int max_x, int max_y)
{
    cv::Rect r;
    r.x = (x<0) ? 0 : x;
    r.y = (y<0) ? 0 : y;
    r.width = (r.x + width > max_x) ? max_x-r.x : width;
    r.height = (r.y + height > max_y) ? max_y-r.y : height;
    return r;
}

float compute_mean(const cv::Mat_<float>& gauss_map)
{
    float mean = 0;
    std::for_each(begin(gauss_map), end(gauss_map), [&mean](float v) {
            mean += v;
            });
    return mean / (gauss_map.cols*gauss_map.rows);
}

float compute_var(const cv::Mat_<float>& gauss_map, const float& mean)
{
    float var = 0;
    std::for_each(begin(gauss_map), end(gauss_map), [&var, &mean](float v) {
            var += pow(v-mean, 2);
            });
    return var / (gauss_map.cols*gauss_map.rows);
}

void Scene::normals_from_gauss(const cv::Mat_<float>& gauss_map, std::vector<std::size_t>& found_normals, std::vector<std::vector<std::size_t>>& found_normals_clusters)
{
    found_normals.clear();
    found_normals_clusters.clear();

    cv::Mat_<float> ghn = gauss_map.clone();
    cv::Mat result(cv::Size(gauss_map.rows, gauss_map.cols), CV_32FC3); 
    gauss_map.convertTo(result, CV_32FC3);
    cv::cvtColor(result, result, CV_GRAY2RGB);
    cv::transpose(result, result);

    float mean = compute_mean(gauss_map);
    float var = compute_var(gauss_map, mean);

    std::vector<float> maxf;
    int found = 0;
    const int localsize = 10;
    while (found++ < 5) {
        auto max_it = std::max_element(std::begin(ghn), std::end(ghn));
        maxf.push_back(*max_it);
        size_t index = std::distance(begin(ghn), max_it);
        cv::Point pos(index/ghn.cols, index%ghn.rows);

        //XXX compute real bounding box
        cv::Rect bbox = create_rect(pos.x-localsize/2, pos.y-localsize/2, localsize, localsize, ghn.cols, ghn.rows);

        found_normals.push_back(index);
        found_normals_clusters.push_back({ index });

        cv::rectangle(result, bbox, cv::Scalar(0., 255. , 0.));
        fillRect(ghn, bbox);
    }

    std::for_each(begin(maxf), end(maxf), [&mean](float v) {
            mean += v;
            });
    mean /= 100; 
    std::for_each(begin(maxf), end(maxf), [&var](float v) {
            var += v;
            });
    var /= 100;
    
    auto new_end = std::remove_if(begin(maxf), end(maxf), [&mean](const float& val) {
                return val < mean;
            });
    maxf.erase(new_end, end(maxf));
    auto distance = std::distance(begin(maxf), new_end);
    cout << "distance: " << distance << endl;
    found_normals.erase(begin(found_normals)+distance, end(found_normals));
    found_normals_clusters.erase(begin(found_normals_clusters)+distance, end(found_normals_clusters));
    cout << "Max size: " << maxf.size() << endl;

    cv::namedWindow("result");
    cv::imshow("result", result);
    cv::imwrite("result.png", result);
    cv::waitKey();

    cout << "Mean value: " << mean << endl;
    cout << "Var value: " << var << endl;
    cout << "Nb Normals: " << found_normals.size() << endl;
}

//void Scene::normals_from_gauss(const cv::Mat_<float>& gauss_north, const cv::Mat_<float>& gauss_south, std::set<std::size_t>& found_normals, std::vector<std::list<std::size_t>> found_normals_clusters, const float& threshold)
//{
//    found_normals.clear();
//    found_normals_clusters.clear();
//    cv::Mat_<float> ghn = gauss_north.clone();
//    cv::Mat_<float> ghs = gauss_south.clone();
//    cv::Mat result(cv::Size(gauss_north.rows, gauss_north.cols), CV_32FC3); 
//
//    cout << "Computing best normals from gauss, with threshold " << threshold << endl;
//    vector<vector<cv::Point> > contours;
//    vector<cv::Vec4i> hierarchy;
//    int nb_found=0;
//    int nb_iter=0;
//    while(nb_found < 5 && nb_iter++ < 10) {
//        contours.clear();
//        hierarchy.clear();
//
//        cv::Mat thn(cv::Size(gauss_north.rows, gauss_north.cols), CV_8UC1);
//        cv::Mat ths(cv::Size(gauss_south.rows, gauss_south.cols), CV_8UC1);
//        //threshold_gauss(gauss_north, thn, threshold, 10);
//        //threshold_gauss(ghn, thn, threshold);
//
//        cv::Mat t;
//        ghn.clone().convertTo(t, CV_8UC1, 255);
//        cv::namedWindow("converted");
//        cv::imshow("converted", t);
//        int seuil = cv::threshold(t, thn, threshold*255, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
//        //int seuil = cv::threshold(t, thn, 0.1*255, 255, cv::THRESH_BINARY);
//        cout << "seuil: " << seuil << endl;
//        cv::imshow("thresholded north", thn);
//
//        // Find connex components
//        cv::findContours(thn.clone(), contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
//
//        cout << "Found " << contours.size() << " components" << endl;
//        if(contours.size() == 0) break; 
//
//        float max = 0;
//        int normal = 0;
//        cv::Rect max_blob_bbox;
//        int n = 0;
//        float m = 0;
//        for(auto& contour: contours) {
//            cv::Rect blob_bounding_box = compute_bounding_box(contour);
//            // Work on original image
//            normal_from_blob(gauss_north, thn, blob_bounding_box, normal, max);
//            cout << "Normal: " << normal << " with max: " << max << endl;
//            if(m > max) {
//                max = m;
//                normal = n;
//                max_blob_bbox = blob_bounding_box;
//            }
//        }
//
//        fillRect(thn, ghn, max_blob_bbox);
//        cv::rectangle(result, max_blob_bbox, cv::Scalar(255., 0. , 0.));
//
//        cv::normalize(ghn.clone(), ghn, 0, 255, cv::NORM_MINMAX, ghn.type());
//        
//        cout << "Inserting normal " << normal << endl;
//        found_normals.insert(normal);
//        cv::namedWindow("ghn");
//        cv::imshow("ghn", ghn);
//        nb_found = found_normals.size();
//
//    }
//    cout << "Found " << found_normals.size() << " normals" << endl;
//    for(auto& n:found_normals) {
//        std::cout << n << "\t";
//        cv::Rect bbox(n - n/gauss_north.cols, n%gauss_north.rows, 3, 3);
//        cv::rectangle(result, bbox, cv::Scalar(0., 255. , 0.));
//    }
//    cv::namedWindow("result");
//    cv::imshow("result", result);
//}


/**
 * If mean of surrounding area > threshold, value=1, discard otherwise
 * Returns a mask for each area of interest
 **/
void Scene::threshold_gauss(const cv::Mat_<float>& src, cv::Mat& dst, const float& threshold, const int& localsize)
{
    int halfsize = localsize/2;
    int minr, maxr, minc, maxc;
    for(int r=0; r<src.rows; r++) {
        for(int c=0; c<src.cols; c++)   {
            minr = (r-halfsize<0) ? 0 : r-halfsize;
            minc = (c-halfsize<0) ? 0 : c-halfsize;
            maxr = (r+halfsize>src.rows) ? src.rows : r+halfsize;
            maxc = (c+halfsize>src.cols) ? src.cols : c+halfsize;
            float val = 0;
            for(int lr = minr; lr <= maxr; lr++) {
                for(int lc = minc; lc <= maxc; lc++) {
                    val += src.at<float>(lr, lc);
                }
            }
            val = val / ((maxr-minr)*(maxc-minc));
            if(val > threshold)
                dst.at<uchar>(r,c) = 255; //src.at<float>(r,c);
            else
                dst.at<uchar>(r,c) = 0; 
        }
    }
}

void Scene::threshold_gauss(const cv::Mat_<float>& src, cv::Mat& dst, const float& threshold)
{
    for(int r=0; r<src.rows; r++) {
        for(int c=0; c<src.cols; c++)   {
            auto val = src.at<float>(r, c); 
            if(val > threshold)
                dst.at<uchar>(r,c) = 255; //src.at<float>(r,c);
            else
                dst.at<uchar>(r,c) = 0; 
        }
    }
}
