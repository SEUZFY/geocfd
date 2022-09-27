﻿/*
* process geometry for cfd simulation
* main.cpp
*/

#include <CGAL/Polyhedron_3.h>
#include "JsonWriter.hpp"

#define DATA_PATH "D:\\SP\\geoCFD\\data" // data path
#define _ENABLE_CONVEX_HULL_ // turn on/off convex hull method

int main(int argc, const char** argv)
{

	std::cout << "-- activated data folder: " << DATA_PATH << '\n';
	std::cout << "This is: " << argv[0] << '\n';

	//  std::cout<<"newly-added\n";
	//std::cout<<"data path is: "<<mypath<<'\n';

	//  char buffer[256];
	//  if (getcwd(buffer, sizeof(buffer)) != NULL) {
	//     printf("Current working directory : %s\n", buffer);
	//  } else {
	//     perror("getcwd() error");
	//     return 1;
	//  }

	//-- reading the (original)file with nlohmann json: https://github.com/nlohmann/json  
	std::string filename = "/3dbag_v210908_fd2cee53_5907.json";
	std::cout << "current reading file is: " << DATA_PATH + filename << '\n';
	std::ifstream input(DATA_PATH + filename);
	json j;
	input >> j;
	input.close();


	//read certain building
	JsonHandler jhandle1;
	std::string building1_id = "NL.IMBAG.Pand.0503100000019695-0";
	jhandle1.read_certain_building(j, building1_id);
	jhandle1.message();

	JsonHandler jhandle2;
	std::string building2_id = "NL.IMBAG.Pand.0503100000018413-0"; // adjacent to building1
	jhandle2.read_certain_building(j, building2_id);
	jhandle2.message();


	// build a vector to store the nef polyhedra(if built successfully)
	std::vector<Nef_polyhedron> Nefs;


	// build polyhedron test
	BuildPolyhedron::build_one_polyhedron(jhandle1, Nefs);
	BuildPolyhedron::build_one_polyhedron(jhandle2, Nefs);


	// prompt Nefs
	std::cout << "there are " << Nefs.size() << " Nef polyhedra now\n";


	// check if Nef is simple and convert it to polyhedron 3 for visualise
	// for(const auto& nef : Nefs)
	// {
	//    std::cout<<"is nef simple? "<<nef.is_simple()<<'\n';
	//    if(nef.is_simple())
	//    {
	//       Polyhedron p;
	//       nef.convert_to_Polyhedron(p);
	//       std::cout<<p;
	//    }
	// }


	// big Nef
	Nef_polyhedron big_nef;
	for (const auto& nef : Nefs)
		big_nef += nef;


	// check if big Nef is simple - simple: no internal rooms, not simple: multiple rooms?
	std::cout << "is bigNef simple? " << big_nef.is_simple() << '\n';

	// process the big nef to make it available for output
	std::vector<Shell_explorer> shell_explorers; // store the extracted geometries
	NefProcessing::extract_nef_geometries(big_nef, shell_explorers); // extract geometries of the bignef
	NefProcessing::process_shells_for_cityjson(shell_explorers); // process shells for writing to cityjson

	// prompt some info after cleaning operation
	std::cout << "info about shells after cleaning operations\n";
	std::cout << "shell explorers size: " << shell_explorers.size() << '\n';
	std::cout << "info for each shell\n";
	for (const auto& se : shell_explorers)
	{
		std::cout << "cleaned vertices size of this shell: " << se.cleaned_vertices.size() << '\n';
		std::cout << "cleaned faces size of this shell: " << se.cleaned_faces.size() << '\n';
		std::cout << '\n';
	}


	/* get the convex hull of the big_nef, use all cleaned vertices of all shells */
	// get cleaned vertices of shell_explorers[0] - the shell indicating the exterior of the big nef
	std::vector<Point_3>& convex_vertices = shell_explorers[0].cleaned_vertices;
	std::cout << "convex vertices size: " << convex_vertices.size() << '\n';

	// build convex hull of the big nef
	Polyhedron convex_polyhedron; // define polyhedron to hold convex hull
	Nef_polyhedron convex_big_nef;
	CGAL::convex_hull_3(convex_vertices.begin(), convex_vertices.end(), convex_polyhedron);
	std::cout << "is convex closed?" << convex_polyhedron.is_closed() << '\n';
	if (convex_polyhedron.is_closed()) {
		std::cout << "build convex hull for the big nef\n";
		Nef_polyhedron convex_nef(convex_polyhedron);
		convex_big_nef = convex_nef;
	}

	// process the convex big nef to make it available for output
	std::vector<Shell_explorer> convex_shell_explorers;
	NefProcessing::extract_nef_geometries(convex_big_nef, convex_shell_explorers);
	NefProcessing::process_shells_for_cityjson(convex_shell_explorers);

    // write file
	JsonWriter jwrite;
	std::string writeFilename = "\\bignefpolyhedron_convex.json";
	const Shell_explorer& shell = convex_shell_explorers[1]; // which shell is going to be written to the file
	jwrite.write_json_file(DATA_PATH + writeFilename, shell);

	return 0;
}