#include "scene.h"
#include<cmath>
#include<sstream>
#include<chrono>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/ml/ml.hpp>
int main()
{
	Scene s;
	s.loadPLY("../build/result.ply");
	s.load_cam("../data/laser/maison_Mougins/Mougins_scan_centers.txt");
	s.compute_Knearest_neighbors(10);
	s.compute_normal();
	std::vector<float> normal;
	s.get_all_normal(normal);
//	std::cout << normal.size() << ' ' << (s.vhpoint.size()*3) << ' ' << s.vhpoint.size() <<std::endl;

	

	std::vector<cv::Mat_<float>> vmhn;
	std::vector<cv::Mat_<float>> vmhs;

	
	std::size_t rows;
	std::vector<std::size_t> hn;
	std::vector<std::size_t> hs;
	for(const std::size_t& beta : {10,15,20,25,30,45,90,135,180,360,1000})
	{
		std::size_t rows = 2*(beta+1)+1;
		hn.clear();
		hn.resize(pow(rows,2));
		hs.clear();
		hs.resize(pow(rows,2));
	
	
//	normal.clear();
//	normal.insert(normal.end(),{1,0,0});
//	normal.insert(normal.end(),{0,0,1});
//	normal.insert(normal.end(),{sqrt(2)/2,sqrt(2)/2,0});
//	normal.insert(normal.end(),{-1,0,0});
//	normal.insert(normal.end(),{0,-1,0});
//	normal.insert(normal.end(),{0,0,-1});
	
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
	
	s.compute_gauss(hn,hs,normal,rows,rows,beta);

    end = std::chrono::system_clock::now();
	std::cout << normal.size() << std::endl;

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << "\n\nfinished computation at " << std::ctime(&end_time)
        << "elapsed time: " << elapsed_seconds.count() << "s\n";



	




	cv::Mat_<float> mhn(rows,rows);
	cv::Mat_<float> mhs(rows,rows);

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
	vmhn.push_back(mhn.clone());
	vmhs.push_back(mhs.clone());
	}
	std::ostringstream oss;
	std::string str;
	char c;
	for(std::size_t i = 0; i<vmhn.size();)
	{
		oss.str("");
//		oss << "hn" << i;
		oss << "hn";
		str = oss.str().c_str();
		cv::namedWindow(str, CV_WINDOW_NORMAL);
		cv::imshow(str, vmhn[i]*8);
		oss.str("");
//		oss << "hs" << i;
		oss << "hs";
		str = oss.str().c_str();
		cv::namedWindow(str, CV_WINDOW_NORMAL);
		cv::imshow(str, vmhs[i]*8);
		c=cv::waitKey();
		switch(c)
		{
			case 'e':if(i<vmhn.size()-1)++i;break;
			case 'a':if(i!=0)--i;break;
			case 'q':i=vmhn.size();break;

		}
		//while((c=cv::waitKey())!='q');
	}
	//while(cv::waitKey()!='q');

}
