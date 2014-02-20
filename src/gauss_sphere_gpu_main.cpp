#include <CL/cl.hpp>
#include <chrono>

#include <iostream>
#include <list>

#include "openCLUtilities.hpp"
#include "gauss_sphere.hpp"
#include "Utils.h"
#include "scene.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/ml/ml.hpp>


using namespace std;
using namespace cv;


int main(int argc, char **argv)
{
    std::srand(std::time(0)); // use current time as seed for random generator

    //get all platforms (drivers)
    cout << "=========" << endl;
    cout << "Platform" << endl;
    cout << "=========" << endl;
    vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);
    if (all_platforms.size() == 0)
    {
        cout << "No platforms found. Check OpenCL installation!" << endl;
        exit(1);
    }

    for (cl::Platform p : all_platforms)
    {
        cout << "Available platform: " << p.getInfo<CL_PLATFORM_NAME>() << endl;
    }

    cl::Platform default_platform;
    default_platform = all_platforms[0];
    cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << endl;

    //get default device of the default platform
    vector<cl::Device> all_devices;
    default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    if (all_devices.size() == 0)
    {
        cout << " No devices found. Check OpenCL installation!" << endl;
        exit(1);
    }
    for (cl::Device d : all_devices)
    {
        cout << "Available platform: " << d.getInfo<CL_DEVICE_NAME>() << endl;
    }
    cl::Device default_device = all_devices[0];
    cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";
    cout << "Max workgroup size: " << default_device.getInfo< CL_DEVICE_MAX_WORK_GROUP_SIZE >() << "\n";
    cout << endl << endl;

    cl::Context context({default_device});

    cl::Program program = buildProgramFromSource(context, "../src/gauss_sphere.cl");



    //create queue to which we will push commands for the device.
    cl::CommandQueue queue(context, default_device);


    cl::Kernel gauss_sphere_kernel(program, "gauss_sphere");
    GaussSphere gf(context, queue);

    const int NB_NORMALS = 10000000;
    std::vector<glm::vec4> normals(NB_NORMALS);
    for(int i=0; i<normals.size(); i++) {
        normals[i] = glm::vec4(1., -1., 0., 0.);
    }

	Scene s;
	s.loadPLY("../build/result.ply");
	s.load_cam("../data/laser/maison_Mougins/Mougins_scan_centers.txt");
	s.compute_Knearest_neighbors(10);
	s.compute_normal();
	std::vector<float> normal;
	s.get_all_normal_vec4(normal);


    const float d = 1.;
    const float p = 200.;
    const int tex_res = 2*(1+ceil(p/d));
    cout << "resolution: " << tex_res << endl;

    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    cout << "Setting up kernel" << endl;
    gf.setupBuffers(normals, tex_res, tex_res);
    gf.runKernel(gauss_sphere_kernel);
    gf.readNormals(normals);

//    cout << "Printing normals after kernel" << endl;
//    for(auto vec : normals) {
//        std::cout  << vec << ", ";
//    }
//    cout << endl;


    std::vector<cl_int> northHemisphere;
    gf.readNorthHemisphere(northHemisphere);
    std::cout << "North hemisphere: " << northHemisphere[49724] << endl;

    std::vector<cl_int> southHemisphere;
    gf.readSouthHemisphere(southHemisphere);
    std::cout << "South hemisphere: " << southHemisphere[49724] << endl;

	cv::Mat_<float> mhn(tex_res, tex_res);
	cv::Mat_<float> mhs(tex_res, tex_res);

	auto it_vhn = northHemisphere.begin();
	auto it_vhs = southHemisphere.begin();
	std::size_t max(0);
    auto &hn = northHemisphere;
    auto &hs = southHemisphere;
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
	cv::namedWindow("north", CV_WINDOW_NORMAL);
	cv::imshow("north", mhs);
	//vmhn.push_back(mhn.clone());
	//vmhs.push_back(mhs.clone());

    end = std::chrono::system_clock::now();


    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << "\n\nfinished computation at " << std::ctime(&end_time)
        << "elapsed time: " << elapsed_seconds.count() << "s\n";

	while(cv::waitKey()!='q');

    return 0;
}
