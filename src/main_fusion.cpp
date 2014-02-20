#include"fusion_file.h"


#include<chrono>
#include<iostream>

int main()
{
	/*
	std::chrono::time_point<std::chrono::system_clock> start, end;
	std::chrono::duration<double> elapsed_seconds;

	start = std::chrono::system_clock::now();
	fusion_file_ply("../data/laser/maison_Mougins/Mougins_scan_centers.txt","result1.ply",1*0);
	end = std::chrono::system_clock::now();
	elapsed_seconds = end - start;

	std::cout << "elapsed time: " << elapsed_seconds.count() << std::endl;

	start = std::chrono::system_clock::now();
	fusion_file_ply2("../data/laser/maison_Mougins/Mougins_scan_centers.txt","result2.ply",1*0);
	end = std::chrono::system_clock::now();

	elapsed_seconds = end - start;

	std::cout << "elapsed time: " << elapsed_seconds.count() << std::endl;

//*/
	fusion_file_ply("../data/laser/maison_Mougins/Mougins_scan_centers.txt","mougin.ply",10);


	return EXIT_SUCCESS;
}

