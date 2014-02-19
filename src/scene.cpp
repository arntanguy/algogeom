#include "scene.h"
#include "Ply.hpp"

using namespace std;
using I::misc::Ply;

Scene::Scene() 
{
}
Scene::~Scene() 
{
}

Scene::~Scene()
{
}

//void loadPLYcertis(const std::string& );
bool Scene::loadPLY(const std::string& path)
{
	vpoint.clear();
	vpoint_indice.clear();
	std::vector< Point_d > points_temp;
	std::vector< Point_d > color_temp;
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
				 ||(!ply.read(element.property(5), b)))
				{
					cerr << "error while reading (pos) vertex " 
						<< i+1 << endl;
					ply.close();
					return false;
				}
				for(int k=6;k<element.num_properties();k++)
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
	vpoint.reserve(points_temp.size());
	vpoint_indice.reserve(points_temp.size());
	vcolor_point.reserve(points_temp.size());
	for(int i=0;i<points_temp.size();i++)
	{
		vpoint.push_back(points_temp[i]);
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

   // ply.close();
