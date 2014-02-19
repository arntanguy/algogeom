#ifndef CLOUD_SEGMENTATION_H
#define CLOUD_SEGMENTATION_H

#include "types.h"
#include "Ply.hpp"
#include "Visualization_Tools.h"
#include "polyhedron_flo.h"
//#include "triangulation_flo.h"


using std::vector;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using I::misc::Ply;
typedef unsigned char byte;
const double PI = 3.141592;




double distance_Point_d(Point_d pt1,Point_d pt2);
double distance_Point_2d(Point_2d pt1,Point_2d pt2);
bool is_included(std::vector<int> subset, std::vector<int> set);
bool is_similar(std::vector<int> set1, std::vector<int> set2);
int compute_facet_coherence(int t1, std::vector<int> vp1, int t2, std::vector<int> vp2 ,int t3, std::vector<int> vp3);

class ScanLidar
{
	struct HPoint {
		Point_d position;
		bool is_kept;
		bool is_fitted_to_plane;
		bool is_fitted_to_plane_bordering_point;
		bool is_edge;
		bool is_corner;
		int  primitive_index;
		std::vector < int > plane_index_list;
		std::vector < int > list_camera_index;
		float accuracy;
		float weight;

	};


	struct Visibility_patch{
		Point_d eye;
		Vector_3 direction;
		double distance;
		double confidence;
		Vector_3 normal;
	};


	struct Camera{
		int id;
		Point_d center;
		double t[3];
		double R[3][3];
	};

	struct Edge { 
		Line line;
		int p1;
		int p2;
		double angle;
		std::vector<int> edges_index; //index des HPS-points etant edges de l'intersection
		Vector_3 direction_p1;
		Vector_3 direction_p2;
		bool active;
};
	struct Corner {
			int p1;
			int p2;
			int p3;
			int e1;
			int e2;
			int e3;
			Point_d pt;
			bool active;
		};


	struct NCorner {
		std::vector<int> ind_planes;
		std::vector<int> ind_edges;
		std::vector< Vector_3 > edge_direction;
		int nb_corner3;
		Point_d pt;
		bool active;
	};

 

public:

	//ScanLidar() : labeler(tr){}
	ScanLidar(){}
	~ScanLidar(){}


	//General
	vector<HPoint> HPS;
	vector<HPoint> HPS_residus;
	vector < Vector_3 > normal;
	std::vector < std::vector < int > > spherical_neighborhood;
	Polyhedron_Flo P;
	bool IS_PC_CLUSTERED;
	bool is_normal_given;
	bounding_3 BBox_scan2;
	double BBox_diagonal;
	double averaged_length_of_nearest_neighbors;


	//visibility by camera position
	vector<Camera> list_cameras;


	//visibility by ray shooting
	vector<Visibility_patch> vector_of_attach;
	bool shooting_directions_bbox[8];
	double visibility_voxel_size;
	std::list < Vertex_handle_tds > vertex_source_out;
	std::list < Vertex_handle_tds > vertex_source_in;


	//plane extraction
	std::vector < std::vector < int > > plane_point_index;
	vector < int > label_plane;
	std::vector < Plane_3 > extracted_planes;
	std::vector<Point_d> vector_centroids_of_extracted_planes;
	std::vector<Point_d> list_centroid;
	std::vector< double > list_areas;
	int nb_planes;
	
	//structuring
	std::vector<Edge> vector_edges;
	std::vector< Corner > vector_corners;
	std::vector< NCorner > vector_ncorners;
	double SS_plane;
	std::vector < double > SS_edge;
	
	
	//a mettre en argument des fonctions concernées OU merger edge/corner sampling
//	std::vector< std::vector< int > > voisinnage_segment_index;
//	std::vector< std::vector< int > > voisinnage;



	/////    General parameters   //////
	unsigned int ATTACHE_TYPE_X;
	unsigned int ATTACHE_TYPE_Y;
	unsigned int ATTACHE_TYPE_Z;
	bool IS_OBJECT_CLOSE;

	unsigned int NB_NEIGHBORS_MAX;
	double RADIUS_NEIGHBORS_MAX; 
	double EPSILON;
	int NB_LOCAL_NEIGHBORS;  //to be fixed =8

	////    Primitive extraction   /////
	int PLANE_EXTRACTION_TYPE; 
	int PLANE_POINT_NUMBER_MIN;
	double PLANE_FITTING_ERROR_MAX; 


	/// Visibility
	double ANGLE_MAX;//=cos(PI/3);//sqrt(1.5)/2+1e-3;
	double CONFIDENCE_PLANAR_PATH;//=0.02;
	bool HARD_VISIBILiTY_CONSTRAINT; // true= hard constraints when XYZ=3, a ray has to be crossed AND validated twice in both directions 
	int CELL_SIZE;//=3;

	////    Consolidation   /////
	int MINIMAL_NUMBER_CONNEX_POINTS_UNFITTED;
	double DISTANCE_ATTRACTION;

	///////     file saving      ////////
	bool SAVE_VISIBILITY;
	bool SAVE_PRIMITIVES;
	bool SAVE_CONSOLIDATION_POINT_SET;
	bool SAVE_DELAUNAY_FACET_SOUP;
	bool SAVE_3D_MODEL;
	double DISPLAY_Z_PLANE;



bool load_points(string filename_o)
	{

		is_normal_given=false;

		std::stringstream ss;
		ss<<filename_o<<".ply";
		string filename = ss.str();

		cout <<filename;


		HPS.clear();
		nb_planes=0;
		std::vector< Vector_3 > normals_temp;
		std::vector< HPoint > points_temp;
		Ply ply;

		if (!ply.open(filename,true)){ return false;}

		for (Ply::ElementsIterator it = ply.elements_begin(); it != ply.elements_end(); ++it){
			const Ply::Element& element = *it;

			if (element.name() != "vertex"){
				if (!ply.skip(element)){ ply.close(); return false;}
				continue;
			}

			/* Check the properties are what we expect */
			if ((element.property(0).name() != "x")||(element.property(1).name() != "y")||(element.property(2).name() != "z")){cerr << "Unexpected vertex properties in the PLY file" << endl; ply.close(); return false;}


			size_t num_vertices = element.count();
			points_temp.resize(num_vertices);

			if (element.num_properties() == 3) {
				for (size_t i=0; i<num_vertices; i++){
					double x,y,z;

					if ((!ply.read(element.property(0), x))||(!ply.read(element.property(1), y))||(!ply.read(element.property(2), z))){cerr << "error while reading (pos) vertex " << i+1 << endl; ply.close(); return false;}	

					points_temp[i].position = Point_d(x,y,z);

				}
			}

			else if (element.num_properties() == 4) {
				for (size_t i=0; i<num_vertices; i++){
					double x,y,z;
					int cluster;

					if ((!ply.read(element.property(0), x))||(!ply.read(element.property(1), y))||(!ply.read(element.property(2), z))){cerr << "error while reading (pos) vertex " << i+1 << endl; ply.close(); return false;}	
					if (!ply.read(element.property(3), cluster)){cerr << "error while reading attribut " << i+1 << endl; ply.close(); return false;}

					points_temp[i].position = Point_d(x,y,z);
					label_plane.push_back(cluster);
					if(cluster>nb_planes-1) nb_planes=cluster+1;
				}
			}

			else if (element.num_properties() == 5) {
				for (size_t i=0; i<num_vertices; i++){
					double x,y,z;
					double ampli;
					int echo;

					if ((!ply.read(element.property(0), x))||(!ply.read(element.property(1), y))||(!ply.read(element.property(2), z))){cerr << "error while reading (pos) vertex " << i+1 << endl; ply.close(); return false;}
					if ((!ply.read(element.property(3), ampli))||(!ply.read(element.property(4), echo))){cerr << "error while reading attribut " << i+1 << endl; ply.close(); return false;}

					points_temp[i].position = Point_d(x,y,z);

				}
			}

			else if (element.num_properties() == 6) {
				for (size_t i=0; i<num_vertices; i++){
					double x,y,z;
					double nx,ny,nz;

					if ((!ply.read(element.property(0), x))||(!ply.read(element.property(1), y))||(!ply.read(element.property(2), z))){cerr << "error while reading (pos) vertex " << i+1 << endl; ply.close(); return false;}	
					if ((!ply.read(element.property(3), nx))||(!ply.read(element.property(4), ny))||(!ply.read(element.property(5), nz))){cerr << "error while reading attribut " << i+1 << endl; ply.close(); return false; }

					points_temp[i].position = Point_d(x,y,z);
					normals_temp.push_back(Vector_3(nx,ny,nz));
				}
				is_normal_given=true;
			}

			else if (element.num_properties()>6) {
				for (size_t i=0; i<num_vertices; i++){
					double x,y,z;

					if ((!ply.read(element.property(0), x))||(!ply.read(element.property(1), y))||(!ply.read(element.property(2), z))){cerr << "error while reading (pos) vertex " << i+1 << endl; ply.close(); return false;}		
					for(int k=3;k<element.num_properties();k++){double tempp; if(!ply.read(element.property(k), tempp)){cerr << "error while reading attribut " << i+1 << endl; ply.close(); return false; }}

					points_temp[i].position = Point_d(x,y,z);

				}
			}

			else {cerr << "Unexpected vertex properties in the PLY file" << endl; ply.close(); return false;}
		}



		for(int i=0;i<points_temp.size();i++){
			HPS.push_back(points_temp[i]);
			if(is_normal_given) normal.push_back(normals_temp[i]);
		}

		if(label_plane.size()>0) IS_PC_CLUSTERED=true;
		else IS_PC_CLUSTERED=false;

		points_temp.clear();
		ply.close();


		


	/*	std::vector< Point_d > points_go2;
			std::vector< CGAL::Color > colors_go2;
			for(int i=0;i<HPS.size();i++){
			//	if(i%8==0){
					points_go2.push_back(HPS[i].position); 
					colors_go2.push_back(CGAL::BLACK);
			//	}
			}
			char *outputtp2= "input_clean.ply";
			colorpointset2ply(outputtp2,points_go2,colors_go2);
*/




		//ADD NOISE OR OUTLIERS
		bool is_outliers=false;
		int nb_outliers=0;
		bool is_noise=false;
		double noise_level=0.;

		
		if(is_outliers || is_noise){
			
			std::list<Point_d> list_pt;
			for( int i=0; i<HPS.size();i++) list_pt.push_back(HPS[i].position);
			bounding_3 BBox_sc = CGAL::bounding_box(list_pt.begin(), list_pt.end());
			
			if(is_outliers){

				for( int i=0; i<nb_outliers;i++){
					HPoint hpttemp;
					double x=(double)(BBox_sc.xmax()-BBox_sc.xmin())*rand()/RAND_MAX;
					//double x=(double)(BBox_sc.xmax()-BBox_sc.xmin())/2*rand()/RAND_MAX;
					double y=(double)(BBox_sc.ymax()-BBox_sc.ymin())*rand()/RAND_MAX;
					double z=(double)(BBox_sc.zmax()-BBox_sc.zmin())*rand()/RAND_MAX;
					hpttemp.position=Point_d(x+BBox_sc.xmin(),y+BBox_sc.ymin(),z+BBox_sc.zmin());
					//hpttemp.position=Point_d(x,y+BBox_sc.ymin(),z+BBox_sc.zmin());
					HPS.push_back(hpttemp);
					label_plane.push_back(-1);
				}
			}
			if(is_noise){
				for( int i=0; i<HPS.size();i++){
					//if(HPS[i].position.x()<0){
					double x=(double)noise_level*rand()/RAND_MAX-noise_level/2;
					double y=(double)noise_level*rand()/RAND_MAX-noise_level/2;
					double z=(double)noise_level*rand()/RAND_MAX-noise_level/2;
					HPS[i].position=Point_d(HPS[i].position.x()+x,HPS[i].position.y()+y,HPS[i].position.z()+z);
					//}
				}
			}

/*
			std::vector< HPoint > HPS_temp;
			std::vector< bool > is_kept;
			for( int i=0; i<HPS.size();i++) { HPS_temp.push_back(HPS[i]); is_kept.push_back(true);}
			HPS.clear();
			label_plane.clear();
			
			std::vector< double > vector_rand;
			for( int i=0; i<HPS_temp.size();i++) vector_rand.push_back((double)rand()/RAND_MAX);

			for( int i=0; i<HPS_temp.size();i++){
				
				
				double scoreX= 1.-std::max( (HPS_temp[i].position.x()-BBox_sc.xmin())/(BBox_sc.xmax()-BBox_sc.xmin()), 0.2);
				double scoreY=std::min( (HPS_temp[i].position.y()-BBox_sc.ymin())/(BBox_sc.ymax()-BBox_sc.ymin()), 0.8);
				if(pow((scoreX+scoreY)/2,2.)<vector_rand[i]) is_kept[i]=false; 
			}

			for( int i=0; i<HPS_temp.size();i++){
				if(is_kept[i]){
					HPS.push_back(HPS_temp[i]);
					label_plane.push_back(-1);
				}
			}

*/


			std::vector< Point_d > points_go;
			std::vector< CGAL::Color > colors_go;
			for(int i=0;i<HPS.size();i++){points_go.push_back(HPS[i].position); colors_go.push_back(CGAL::BLACK);}
			const char* outputtp= "input_noise_outliers.ply";
			colorpointset2ply(outputtp,points_go,colors_go);
		}






		return true;
	}





bool load_points_Autodesk(string filename_o)
	{

		is_normal_given=false;


		std::stringstream ss;
		ss<<filename_o<<".ply";
		string filename = ss.str();

		cout <<filename;


		HPS.clear();
		nb_planes=0;
		std::vector< Vector_3 > normals_temp;
		std::vector< HPoint > points_temp;
		Ply ply;

		if (!ply.open(filename,true)){ return false;}

		for (Ply::ElementsIterator it = ply.elements_begin(); it != ply.elements_end(); ++it){
			const Ply::Element& element = *it;

			if (element.name() != "vertex"){
				if (!ply.skip(element)){ ply.close(); return false;}
				continue;
			}

			/* Check the properties are what we expect */
			if ((element.property(0).name() != "x")||(element.property(1).name() != "y")||(element.property(2).name() != "z")||(element.property(3).name() != "weight") || (element.property(4).name() != "accuracy")||(element.property(5).name() != "visibility")){cerr << "Unexpected vertex properties in the PLY file" << endl; ply.close(); return false;}


			size_t num_vertices = element.count();
			points_temp.resize(num_vertices);

			
			if (element.num_properties() == 6) {
				for (size_t i=0; i<num_vertices; i++){
					double x,y,z;
					double weight,accuracy;
					size_t camera_count;
					std::vector< int > list_camera_index;

					if ((!ply.read(element.property(0), x))||(!ply.read(element.property(1), y))||(!ply.read(element.property(2), z))){cerr << "error while reading (pos) vertex " << i+1 << endl; ply.close(); return false;}
					if ((!ply.read(element.property(3), weight))||(!ply.read(element.property(4), accuracy))){cerr << "error while reading attribut " << i+1 << endl; ply.close(); return false;}
					if ( !ply.read_count(element.property(5), camera_count) ){cerr << "error while reading camera index " << i+1 << endl; ply.close(); return false;}

					while (camera_count--){
						int camera_index;
						if (!ply.read_value(element.property(5), camera_index)){cerr << "error while reading camera index " << i+1 << endl; ply.close(); return false;}
						list_camera_index.push_back(camera_index);
					}

					points_temp[i].position = Point_d(x,y,z);
					points_temp[i].weight=weight;
					points_temp[i].accuracy=accuracy;
					points_temp[i].list_camera_index=list_camera_index;

				/*	cout<<endl<<i<<": ";
					for(int p=0;p<list_camera_index.size();p++) cout<<" "<<list_camera_index[p];*/

				}
			}

			

			else {cerr << "Unexpected vertex properties in the PLY file" << endl; ply.close(); return false;}
		}



		for(int i=0;i<points_temp.size();i++){
			HPS.push_back(points_temp[i]);
			if(is_normal_given) normal.push_back(normals_temp[i]);
		}

		if(label_plane.size()>0) IS_PC_CLUSTERED=true;
		else IS_PC_CLUSTERED=false;

		points_temp.clear();
		ply.close();


		

/*
		std::vector< Point_d > points_go2;
			std::vector< CGAL::Color > colors_go2;
			std::vector< CGAL::Color > colors_go3;
			for(int i=0;i<HPS.size();i++){
			//	if(i%8==0){
				CGAL::Color col(255,255*HPS[i].accuracy,255*HPS[i].accuracy);
				CGAL::Color col2(255*(1-HPS[i].weight),255*(1-HPS[i].weight),255);
					points_go2.push_back(HPS[i].position); 
					//colors_go2.push_back(CGAL::BLACK);
					colors_go2.push_back(col);
					colors_go3.push_back(col2);
			//	}
			}
			char *outputtp2= "input_acc.ply";
			colorpointset2ply(outputtp2,points_go2,colors_go2);
			char *outputtp3= "input_weight.ply";
			colorpointset2ply(outputtp3,points_go2,colors_go3);
*/



		return true;
	}





bool load_points_Certis(string filename_o)
	{

		is_normal_given=false;


		std::stringstream ss;
		ss<<filename_o<<".ply";
		string filename = ss.str();

		cout <<filename;


		HPS.clear();
		nb_planes=0;
	//	std::vector< Vector_3 > normals_temp;
	//	std::vector< HPoint > points_temp;
		Ply ply;

		if (!ply.open(filename,false)){ return false;}

		for (Ply::ElementsIterator it = ply.elements_begin(); it != ply.elements_end(); ++it){
			const Ply::Element& element = *it;

			if (element.name() != "vertex"){
				if (!ply.skip(element)){ ply.close(); return false;}
				continue;
			}

			/* Check the properties are what we expect */
			if ((element.property(0).name() != "x")||(element.property(1).name() != "y")||(element.property(2).name() != "z")||(element.property(3).name() != "red") || (element.property(4).name() != "green")||(element.property(5).name() != "blue") || (element.property(6).name() != "confidence")||(element.property(7).name() != "visibility") ){cerr << "Unexpected vertex properties in the PLY file" << endl; ply.close(); return false;}


			size_t num_vertices = element.count();
			HPS.resize(num_vertices);

			
			if (element.num_properties() == 8) {
				for (size_t i=0; i<num_vertices; i++){
					double x,y,z;
					int col1,col2,col3;
					double confidence;
					size_t camera_count;
					std::vector< int > list_camera_index;

					if ((!ply.read(element.property(0), x))||(!ply.read(element.property(1), y))||(!ply.read(element.property(2), z))){cerr << "error while reading (pos) vertex " << i+1 << endl; ply.close(); return false;}
					if ((!ply.read(element.property(3), col1))||(!ply.read(element.property(4), col2))||(!ply.read(element.property(5), col3))){cerr << "error while reading colors " << i+1 << endl; ply.close(); return false;}
					if ( !ply.read(element.property(6), confidence) ){cerr << "error while reading confidence " << i+1 << endl; ply.close(); return false;}
					if ( !ply.read_count(element.property(7), camera_count) ){cerr << "error while reading camera index " << i+1 << endl; ply.close(); return false;}

					while (camera_count--){
						int camera_index;
						if (!ply.read_value(element.property(7), camera_index)){cerr << "error while reading camera index " << i+1 << endl; ply.close(); return false;}
						list_camera_index.push_back(camera_index);
					}

					HPS[i].position = Point_d(x,y,z);
					HPS[i].weight=confidence;
					HPS[i].accuracy=1.;
					HPS[i].list_camera_index=list_camera_index;

				}
			}

			

			else {cerr << "Unexpected vertex properties in the PLY file" << endl; ply.close(); return false;}
		}

		
		if(label_plane.size()>0) IS_PC_CLUSTERED=true;
		else IS_PC_CLUSTERED=false;

		ply.close();


		
/*

		std::vector< Point_d > points_go2;
			std::vector< CGAL::Color > colors_go2;
			std::vector< CGAL::Color > colors_go3;
			for(int i=0;i<HPS.size();i++){
			//	if(i%8==0){
				CGAL::Color col(255,255*HPS[i].accuracy,255*HPS[i].accuracy);
				CGAL::Color col2(255*(1-HPS[i].weight),255*(1-HPS[i].weight),255);
					points_go2.push_back(HPS[i].position); 
					//colors_go2.push_back(CGAL::BLACK);
					colors_go2.push_back(col);
					colors_go3.push_back(col2);
			//	}
			}
			char *outputtp2= "input_acc.ply";
			colorpointset2ply(outputtp2,points_go2,colors_go2);
			char *outputtp3= "input_weight.ply";
			colorpointset2ply(outputtp3,points_go2,colors_go3);
*/



		return true;
	}





bool load_camera_parameters_Autodesk(char* filename)
{

	std::stringstream ss;
	ss<<filename<<"_cam.txt";
	char *name = new char[ ss.str().size() + 1 ];
	strncpy( name, ss.str().c_str(), ss.str().size() + 1 );

	std::ifstream fic;
	fic.open(name);
	if(!fic.is_open()) std::cout<<"cannot find "<<name<<std::endl;
	else std::cout<<"can OK read "<<name<<std::endl;
	
	int camera_id;
	double t[3];
	double R[3][3];

		 string toto;
		 getline(fic,toto); 

     while( !fic.eof() ){

		 char buffer[20];
		 std::size_t length = toto.copy(buffer,toto.length()-1,1); buffer[length]='\0';
		 camera_id=atoi(buffer); 
		 toto.clear();

		 getline(fic,toto); toto.clear(); 
		 getline(fic,toto); toto.clear();

		 fic>>R[0][0]>>R[1][0]>>R[2][0]>>t[0]>>R[0][1]>>R[1][1]>>R[2][1]>>t[1]>>R[0][2]>>R[1][2]>>R[2][2]>>t[2];

		 getline(fic,toto); toto.clear();
		 getline(fic,toto); toto.clear();
		 getline(fic,toto); 


		//computation optical center
		 double R_inv[3][3];
		 double det= R[0][0]*(R[1][1]*R[2][2]-R[2][1]*R[1][2])
                        -R[0][1]*(R[1][0]*R[2][2]-R[1][2]*R[2][0])
                        +R[0][2]*(R[1][0]*R[2][1]-R[1][1]*R[2][0]);

		double invdet = 1/det;
		R_inv[0][0] =  (R[1][1]*R[2][2]-R[2][1]*R[1][2])*invdet;
		R_inv[1][0] = -(R[0][1]*R[2][2]-R[0][2]*R[2][1])*invdet;
		R_inv[2][0] =  (R[0][1]*R[1][2]-R[0][2]*R[1][1])*invdet;
		R_inv[0][1] = -(R[1][0]*R[2][2]-R[1][2]*R[2][0])*invdet;
		R_inv[1][1] =  (R[0][0]*R[2][2]-R[0][2]*R[2][0])*invdet;
		R_inv[2][1] = -(R[0][0]*R[1][2]-R[1][0]*R[0][2])*invdet;
		R_inv[0][2] =  (R[1][0]*R[2][1]-R[2][0]*R[1][1])*invdet;
		R_inv[1][2] = -(R[0][0]*R[2][1]-R[2][0]*R[0][1])*invdet;
		R_inv[2][2] =  (R[0][0]*R[1][1]-R[1][0]*R[0][1])*invdet;

		double a=-(R_inv[0][0]*t[0]+R_inv[0][1]*t[1]+R_inv[0][2]*t[2]);
		double b=-(R_inv[1][0]*t[0]+R_inv[1][1]*t[1]+R_inv[1][2]*t[2]);
		double c=-(R_inv[2][0]*t[0]+R_inv[2][1]*t[1]+R_inv[2][2]*t[2]);

		Point_d center(a,b,c);

		 //copy data in camera structure
		Camera cam_temp;
		cam_temp.id=camera_id;
		cam_temp.center=center;
		list_cameras.push_back(cam_temp);


	 }
	 fic.close();



std::vector<Polyhedron> vector_polyhedron;
std::vector<CGAL::Color> vector_color;

double radius_ball=0.25;

for(int i=0; i<list_cameras.size();i++){
	
		Point_d pt=list_cameras[i].center;
		Polyhedron P2=createSphere(pt,radius_ball);
		vector_polyhedron.push_back(P2);
		vector_color.push_back(CGAL::RED); 
}

std::stringstream ss4;
ss4<<filename<<"_camera_center.ply";
char *ss_name4 = new char[ ss4.str().size() + 1 ];
strncpy( ss_name4, ss4.str().c_str(), ss4.str().size() + 1 );
save_listpolyhedron2ply(vector_polyhedron,ss_name4, vector_color);


	

	return true;
}






bool load_camera_parameters_Certis(char* filename)
{

	std::stringstream ss;
	ss<<filename<<"_views.txt";
	char *name = new char[ ss.str().size() + 1 ];
	strncpy( name, ss.str().c_str(), ss.str().size() + 1 );

	std::ifstream fic;
	fic.open(name);
	if(!fic.is_open()) std::cout<<"cannot find "<<name<<std::endl;
	else std::cout<<"can OK read "<<name<<std::endl;
	

	string container;
	int NB_cameras;
	getline(fic,container);  container.clear();
	getline(fic,container); container.clear();
	getline(fic,container); NB_cameras=atoi(container.c_str()); cout<<endl<<endl<<NB_cameras<<endl;

	// while( !fic.eof() ){
	for(int i=0;i<NB_cameras;i++){

		 int a,b,c,d;
		 string e,f;
		 fic>>e>>a>>b>>f>>c>>d;

		std::stringstream ss_cam;
		ss_cam<<filename<<"_cameras/"<<f;
		char *name_cam = new char[ ss_cam.str().size() + 1 ];
		strncpy( name_cam, ss_cam.str().c_str(), ss_cam.str().size() + 1 );

		std::ifstream fic_cam;
		fic_cam.open(name_cam);
		if(!fic_cam.is_open()) std::cout<<"cannot find camera files "<<name_cam<<std::endl;

		double t[3];
		double R[3][3];
		fic_cam>>R[0][0]>>R[1][0]>>R[2][0]>>t[0]>>R[0][1]>>R[1][1]>>R[2][1]>>t[1]>>R[0][2]>>R[1][2]>>R[2][2]>>t[2];
		fic_cam.close();

		//computation optical center
		 double R_inv[3][3];
		 double det= R[0][0]*(R[1][1]*R[2][2]-R[2][1]*R[1][2])
                        -R[0][1]*(R[1][0]*R[2][2]-R[1][2]*R[2][0])
                        +R[0][2]*(R[1][0]*R[2][1]-R[1][1]*R[2][0]);

		double invdet = 1/det;
		R_inv[0][0] =  (R[1][1]*R[2][2]-R[2][1]*R[1][2])*invdet;
		R_inv[1][0] = -(R[0][1]*R[2][2]-R[0][2]*R[2][1])*invdet;
		R_inv[2][0] =  (R[0][1]*R[1][2]-R[0][2]*R[1][1])*invdet;
		R_inv[0][1] = -(R[1][0]*R[2][2]-R[1][2]*R[2][0])*invdet;
		R_inv[1][1] =  (R[0][0]*R[2][2]-R[0][2]*R[2][0])*invdet;
		R_inv[2][1] = -(R[0][0]*R[1][2]-R[1][0]*R[0][2])*invdet;
		R_inv[0][2] =  (R[1][0]*R[2][1]-R[2][0]*R[1][1])*invdet;
		R_inv[1][2] = -(R[0][0]*R[2][1]-R[2][0]*R[0][1])*invdet;
		R_inv[2][2] =  (R[0][0]*R[1][1]-R[1][0]*R[0][1])*invdet;

		double ap=-(R_inv[0][0]*t[0]+R_inv[0][1]*t[1]+R_inv[0][2]*t[2]);
		double bp=-(R_inv[1][0]*t[0]+R_inv[1][1]*t[1]+R_inv[1][2]*t[2]);
		double cp=-(R_inv[2][0]*t[0]+R_inv[2][1]*t[1]+R_inv[2][2]*t[2]);

		Point_d center(ap,bp,cp);

		 //copy data in camera structure
		Camera cam_temp;
		cam_temp.id=i;
		cam_temp.center=center;
		list_cameras.push_back(cam_temp);

	 }
	 fic.close();



std::vector<Polyhedron> vector_polyhedron;
std::vector<CGAL::Color> vector_color;

double radius_ball=0.25;

for(int i=0; i<list_cameras.size();i++){
	
		Point_d pt=list_cameras[i].center;
		Polyhedron P2=createSphere(pt,radius_ball);
		vector_polyhedron.push_back(P2);
		vector_color.push_back(CGAL::RED); 
}

std::stringstream ss4;
ss4<<filename<<"_camera_center.ply";
char *ss_name4 = new char[ ss4.str().size() + 1 ];
strncpy( ss_name4, ss4.str().c_str(), ss4.str().size() + 1 );
save_listpolyhedron2ply(vector_polyhedron,ss_name4, vector_color);


	return true;
}






















protected: 


};

double distance_Point_d(Point_d pt1,Point_d pt2){
	double d=std::sqrt(pow(pt1.x()-pt2.x(),2)+pow(pt1.y()-pt2.y(),2)+pow(pt1.z()-pt2.z(),2));
	return d;
}

double distance_Point_2d(Point_2d pt1,Point_2d pt2){
	double d=std::sqrt(pow(pt1.x()-pt2.x(),2)+pow(pt1.y()-pt2.y(),2));
	return d;
}


#endif 
