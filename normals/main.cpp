
#include <iostream>
#include <vector>
#include <cstring>


#include "types.h"
#include "cloud_segmentation.h"





int main(int argc, char **argv)
{
	
	clock_t t;
// -------  Loading data and structure initialization -------//
// ----------------------------------------------------------//
Cloud_segmentation C;

//points and normals
if(!C.load_points(argv[1])) return 0;


char *name_output5= "PC_ball.ply";
C.save_PC_ball(name_output5,0.05,60);


//Compute K-nearest neighbors 
C.Compute_Knearest_neighbors(20);
if (!C.normal_provided) C.compute_normal();

/*
char *name_pcsave= "PC_save.ply";
save_PC_with_normals(C,name_pcsave);

char *name_pcsavecol= "PC_save_col.ply";
save_PC_with_colors(C,name_pcsavecol);
*/
//C.Cloud_segmentation::sort_point_by_planarity();

// ----------------------------------------------------------//
// ----------------------------------------------------------//







// --------------------- Region Growing ---------------------//
// ----------------------------------------------------------//
bool redo=true;

while(redo){

	double epsilon;
	double Nmin;
	cout<<endl<<"Give value for epsilon: ";
	std::cin>>epsilon;
	cout<<"Give minimal number of inliers: ";
	std::cin>>Nmin;
	cout<<endl;


	//Region Growing HERE
	t = clock();
	C.region_growing(epsilon,Nmin);

	cout<<endl<<"TIME "<<((float)clock()-t)/CLOCKS_PER_SEC<<" sec "<< endl;

	//saving geometric primitives
	char *name1= "planes";
	if(C.plane_point_index.size()>0) save_envelops(C,name1);


	double epsilon_regularities=0.01;
	double tolerance_coplanarity=0.1;
	C.detection_regularities_new(epsilon_regularities,tolerance_coplanarity);

	char *name2= "planes_regularized";
	if(C.plane_point_index.size()>0) save_envelops(C,name2);

	cout<<endl<<"Relaunch ? (yes=1, no=0): ";
	std::cin>>redo;
}


// ----------------------------------------------------------//
// ----------------------------------------------------------//




cout << endl<< "END" << endl;

	return 0;
}