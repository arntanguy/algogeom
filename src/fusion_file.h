#ifndef FUSION_FILE_HEADER___
#define FUSION_FILE_HEADER___

#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<list>
#include<iostream>


//TODO the number of vertex is not update -> read how change a number in a file without copy all fie
void fusion_file_ply(const char* file_scan_center, const char* output, unsigned int point_to_ignore)
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
	ofs.open(output);
	int read_line;
	do
	{
		std::getline(ifs,tmp);
		if(*tmp.crbegin() == '\r')
			tmp.erase(tmp.end()-1);
			//tmp.erase(tmp.rbegin());
		ofs << tmp << std::endl;
	}while(tmp!="end_header");
	ifs.close();
	for (int i=0; i<n; ++i)
	{
		read_line = 0;
		oss.str("");
		oss << str << i << ".ply";
		std::cout << oss.str() << std::endl;
		ifs.open(oss.str());
		do
		{
			std::getline(ifs,tmp);
		if(*tmp.crbegin() == '\r')
			tmp.erase(tmp.end()-1);
		}while(tmp!="end_header");
		while(std::getline(ifs,tmp).good())
		{
			if(*tmp.crbegin() == '\r')
				tmp.erase(tmp.end()-1);
			if(point_to_ignore>=++read_line)
				continue;
		//	std::cout << read_line<<std::endl;
			read_line = 0;
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

