#include "scene.h"
#include "Ply.hpp"

Scene::Scene() 
{
}

void Scene::loadPLY(const std::string& path)
{
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
}
