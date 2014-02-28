#include "scene.h"
#include<cmath>
#include<sstream>
#include<chrono>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/ml/ml.hpp>

void gray_to_color(const cv::Mat_<float>& src, cv::Mat_<cv::Vec<float,3>>& dst)
{
	dst.release();
	dst.create(src.rows, src.cols);
	auto v3f = dst.begin()-1;
	for(auto f = src.begin(),end=src.end();f!=end;++f)
	{
		const auto& ff = *f;
		auto& color = *++v3f;
		if(ff==0)
		{
			color[0]=color[1]=color[2]=0;
		}
		else if(ff<.5)
		{
			color[0]=1-ff*2;
			color[1]=ff*2;
			color[2]=0;
		}
		else
		{
			color[0]=0;
			color[1]=2-ff*2;
			color[2]=ff*2-1;
		}
	}
}
int main(int argc, char** argv)
{
	std::vector<std::string> ply = {
		"../build/mougin.ply",
		"../build/appartment.ply",
		"../build/khan.ply",
		"../data/Espresso.ply",
		"../data/saint-jean.ply",
		"../data/eglise-fontaine.ply",
		"../data/mvs/P25/cloud.ply"
	};
	std::vector<std::string> cam = {
		"../data/laser/maison_Mougins/Mougins_scan_centers.txt",
		"../data/laser/appartement/Appartment_scan_centers.txt",
		"../data/laser/Kahn/Kahn_outside_scan_centers.txt",
		"",
		"",
		"",
		""
	};
	char idx = (argc==1 ? 0 : argv[1][0] - '0');
	Scene s;
	s.loadPLY(ply[idx]);
	s.load_cam(cam[idx]);
	std::cout << "data loaded"<<std::endl;
	s.compute_Knearest_neighbors(10);
	std::cout << "nearest neihgbors computed"<<std::endl;
	s.compute_normal();
	std::cout << "normal computed"<<std::endl;
	std::vector<float> normal;
	s.get_all_normal(normal);
//	std::cout << normal.size() << ' ' << (s.vhpoint.size()*3) << ' ' << s.vhpoint.size() <<std::endl;

	

	std::vector<cv::Mat_<cv::Vec3f>> vmhn;
	std::vector<cv::Mat_<cv::Vec3f>> vmhs;

	
	std::size_t rows;
	std::vector<std::size_t> hn;
	std::vector<std::size_t> hs;
	std::vector<std::vector<std::size_t>> hn2;
	std::vector<std::vector<std::size_t>> hs2;

    std::vector<std::size_t> found_normals;
    std::vector<std::vector<std::size_t>> found_normals_clusters;

	const double alpha = 1.;
    const float& threshold = 0.1;

	for(const std::size_t& beta : {10,15,20,25,30,45,90,135,180,360,1000})
	{
	for(const double& alpha : {1,2,3,4,5})
	{
		//std::size_t rows = 2*(beta/alpha+1)+1;
		std::size_t rows = 2*ceil(((alpha+beta)/alpha))+1;
		std::cout << rows << std::endl;
	//	hn.clear();
	//	hn.resize(pow(rows,2),0);
	//	hs.clear();
	//	hs.resize(pow(rows,2),0);
	
	
//	normal.clear();
//	normal.insert(normal.end(),{1,0,0});
//	normal.insert(normal.end(),{0,0,1});
//	normal.insert(normal.end(),{sqrt(2)/2,sqrt(2)/2,0});
//	normal.insert(normal.end(),{-1,0,0});
//	normal.insert(normal.end(),{0,-1,0});
//	normal.insert(normal.end(),{0,0,-1});
	
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
	
	s.compute_gauss(hn,hs,normal,rows,rows,beta,alpha);

    end = std::chrono::system_clock::now();
	std::cout << normal.size() << std::endl;

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << "\n\nfinished computation at " << std::ctime(&end_time)
        << "elapsed time: " << elapsed_seconds.count() << "s\n";
	//	hn.clear();
	//	hn.resize(pow(rows,2),0);
	//	hs.clear();
	//	hs.resize(pow(rows,2),0);


    start = std::chrono::system_clock::now();
	
	s.compute_gauss2(hn,hs,hn2,hs2,normal,rows,rows,beta,alpha);

    end = std::chrono::system_clock::now();
	std::cout << normal.size() << std::endl;

    elapsed_seconds = end - start;

    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

	




	cv::Mat_<float> mhn(rows,rows);
	cv::Mat_<float> mhs(rows,rows);
    s.normalize_gauss(hn, hs, mhn, mhs);
//    s.normals_from_gauss(mhn, mhs, found_normals, found_normals_clusters, threshold);

	cv::Mat_<cv::Vec3f>	color_mat;
	gray_to_color(mhn,color_mat);
	vmhn.push_back(color_mat.clone());
	gray_to_color(mhs,color_mat);
	vmhs.push_back(color_mat.clone());
	}
	}
	std::ostringstream oss;
	std::string str;
	char c;
	for(std::size_t i = 0; i<vmhn.size();)
	{
		oss.str("");
		oss << "hn";
		str = oss.str().c_str();
		cv::namedWindow(str, CV_WINDOW_NORMAL);
		cv::imshow(str, vmhn[i]);
		//cv::imshow(str, vmhn[i]*16);
		oss.str("");
		oss << "hs";
		str = oss.str().c_str();
		cv::namedWindow(str, CV_WINDOW_NORMAL);
		cv::imshow(str, vmhs[i]);
//		cv::imshow(str, vmhs[i]*16);
		c=cv::waitKey();
		switch(c)
		{
			case 'e':if(i<vmhn.size()-1)++i;break;
			case 'a':if(i!=0)--i;break;
			case 'q':i=vmhn.size();break;
		}
	}
	float beta2 = 180;
	rows = 2*ceil(((beta2+1)/1.))+1;
//	s.compute_gauss(hn,hs,normal,rows,rows,180,1);
	s.compute_gauss3(hn,hn2,normal,rows,rows,beta2,1);
//	std::vector<std::vector<std::size_t>> vidx = {{rows*rows/2,
//									rows*rows/2+1, rows*rows/2-1,
//									rows*rows/2+rows, rows*rows/2-rows}};
	std::vector<std::size_t> normal_dist;
	//std::vector<std::size_t> vnormal={rows*rows/2};
	std::vector<std::size_t> vnormal;
	std::vector<std::vector<std::size_t>> vidx = {{rows*rows/2,
									rows*rows/2+1, rows*rows/2-1,
									rows*rows/2+rows, rows*rows/2-rows}};
	//std::cout << "rowrowss2 "<<floor(rows*rows/2)<<std::endl;	
	//for(int i=0; i<10; ++i)
	//{
	//	std::cout << hn2[floor(rows*rows/2)][i] << std::endl;
	//	const auto& nn = s.vhpoint[hn2[floor(rows*rows/2)][i]].normal;
	//	std::cout << nn.x() << ' ' << nn.y() << ' ' << nn.z() << std::endl;
	//}
	std::cout <<"azertgfdeerefgfgdferefdfbvvdfze"<<std::endl;
	cv::Mat_<float> mhn(rows,rows);
	cv::Mat_<float> mhs(rows,rows);
    s.normalize_gauss(hn, mhn);
	s.normals_from_gauss(mhn,vnormal,vidx);
	mhn.release();
	mhs.release();
	s.get_distribution_plan(vnormal,vidx,hn2,normal_dist,500,20);
	
	
	//std::ofstream ofs;
	//ofs.open("normal_dist.csv");
	//for(const auto& a : normal_dist)
	//{
	//	ofs << a << std::endl;
	//}
	//ofs.close();
}
