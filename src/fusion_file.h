#ifndef FUSION_FILE_HEADER___
#define FUSION_FILE_HEADER___

#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<list>
#include<iostream>


void fusion_file_ply(const char* file_scan_center, const char* output, const unsigned int point_to_ignore)
{
	std::ifstream ifs;
	std::ofstream ofs;
	ifs.open(file_scan_center);
	int n;
	ifs>>n;
	ifs.close();
	std::string str = file_scan_center;
	str.erase(str.begin()+str.find("scan_centers.txt"),str.end());
	std::ostringstream oss;
	std::string tmp;
	int read_line;
	ifs.close();
	std::vector<int> nb_vertex_(n);
	size_t nb_vertex(0);
	size_t nb_vertex_tmp;
	for (int i=0; i<n; ++i)
	{
		oss.str("");
		oss << str << i << ".ply";
		ifs.open(oss.str());
		do
		{
			std::getline(ifs,tmp);
		}while(tmp.find("element vertex") != 0);
		tmp.erase(tmp.begin(), tmp.begin()+15);
		std::istringstream(tmp)>>nb_vertex_tmp;
		nb_vertex_tmp/=(point_to_ignore+1);
		nb_vertex+=nb_vertex_tmp;
		nb_vertex_[i]=nb_vertex_tmp;
		ifs.close();
	}
	oss.str("");
	oss << str << "0.ply";
	ifs.open(oss.str());
	ofs.open(output);
	do
	{
		std::getline(ifs,tmp);
		if(*tmp.crbegin() == '\r')
			tmp.erase(tmp.end()-1);
			//tmp.erase(tmp.rbegin());
		if(tmp.find("element vertex") == 0)
			ofs << "element vertex " << nb_vertex << std::endl;
		else
			ofs << tmp << std::endl;
	}while(tmp!="end_header");
	for (int i=0; i<n; ++i)
	{
		oss.str("");
		oss << str << i << ".ply";
		std::cout << oss.str() << std::endl;
		ifs.open(oss.str());
		const int& end = nb_vertex_[i];
		for(int j=0; j<end;++j)
		{
			for(int k=0; k<=point_to_ignore;k++)
				std::getline(ifs,tmp);
			if(*tmp.crbegin() == '\r')
				tmp.erase(tmp.end()-1);
			ofs << tmp << std::endl;
		}
		ifs.close();
	}
	ofs.close();
}
void fusion_file_ply2(const char* file_scan_center, const char* output, unsigned int point_to_ignore)
{
	std::ifstream ifs;
	std::ofstream ofs;
	ifs.open(file_scan_center);
	int n;
	ifs>>n;
	ifs.close();
	std::string str = file_scan_center;
	str.erase(str.begin()+str.find("scan_centers.txt"),str.end());
	std::ostringstream oss;
	oss.str("");
	oss << str << "0.ply";
	ifs.open(oss.str());
	std::string tmp;
	std::vector<std::string> header;
	header.reserve(13);
	std::list<std::vector<std::string>> file(n);
	decltype(file)::iterator ptr_file = file.begin();
	//decltype(std::remove_reference<*ptr_file>::type)::iterator ptr_vertex = ptr_file->begin();
//	decltype(ptr_file->begin()) ptr_vertex = ptr_file->begin();
	std::size_t nb_vertex(0);
	std::size_t nb_vertex_tmp;
	do
	{
		std::getline(ifs,tmp);
		if(*tmp.crbegin() == '\r')
			tmp.erase(tmp.end()-1);
			//tmp.erase(tmp.rbegin());
		header.push_back(tmp);
	}while(tmp!="end_header");
	ifs.close();
	for (int i=0; i<n; ++i,++ptr_file)
	{
		oss.str("");
		oss << str << i << ".ply";
		std::cout << oss.str() << std::endl;
		ifs.open(oss.str());
		do
		{
			std::getline(ifs,tmp);
			if(*tmp.crbegin() == '\r')
				tmp.erase(tmp.end()-1);
			if(tmp.find("element vertex") == 0)
			{
				tmp.erase(tmp.begin(), tmp.begin()+15);
				std::istringstream(tmp)>>nb_vertex_tmp;
				nb_vertex_tmp/=(point_to_ignore+1);
				ptr_file->resize(nb_vertex_tmp);
				nb_vertex+=nb_vertex_tmp;
				std::cout << nb_vertex_tmp << std::endl;//**********
			std::cout << nb_vertex << std::endl;
			}
		}while(tmp!="end_header");
	//	int aaa=0;
		for(auto end = ptr_file->end(),ptr_vertex = ptr_file->begin();ptr_vertex!=end;++ptr_vertex)
		{
			for(int i=0; i<=point_to_ignore;i++)
				std::getline(ifs,tmp);
			if(*tmp.crbegin() == '\r')
				tmp.erase(tmp.end()-1);
			*ptr_vertex = tmp;
	//		++aaa;
		}
	//	std::cout << aaa << std::endl;
		ifs.close();
	}
	ofs.open(output);
	for(const std::string& a : header)
	{
		if(a.find("element vertex") == 0)
		{
			ofs << "element vertex " << nb_vertex << std::endl;
		}
		else
		{
			ofs << a << std::endl;
		}
	}
	for(const auto& a: file)
		for(const auto& b : a)
			ofs << b << std::endl;
	ofs.close();
}
//we can // the lecture of files
#endif

