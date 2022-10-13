﻿/*
* process geometry for cfd simulation
* main.cpp
*/


#include <CGAL/Polyhedron_3.h>
#include "JsonWriter.hpp"

#include<chrono> //Timer
#include<future> //async



#define DATA_PATH "D:\\SP\\geoCFD\\data" // specify the data path
//#define _ENABLE_CONVEX_HULL_ // switch on/off convex hull method
//#define _ENABLE_MINKOWSKI_SUM_ // switch on/off minkowski sum method -> activated by default
//#define _ENABLE_MULTI_THREADING_ // switch on/off multi-threading -> activated by default

#define TAG CGAL::Parallel_if_available_tag



/* user defined parameters --------------------------------------------------------------------------------------------------*/

/* triangulation */
bool _ENABLE_TRIANGULATION_ = false; // true - activate the triangulation process, false otherwise

/* lod level */
const double lod = 2.2;

/* minkowski parameter */
const double minkowski_param = 0.1;

/* user defined parameters --------------------------------------------------------------------------------------------------*/



// Timer class -> used for tracking the run time
struct Timer //for counting the time
{
	std::chrono::time_point<std::chrono::steady_clock>start, end;
	std::chrono::duration<float>duration;

	Timer() //set default value
	{
		start = end = std::chrono::high_resolution_clock::now();
		duration = end - start;
	}

	~Timer() // get the end value and print the duration time
	{
		end = std::chrono::high_resolution_clock::now();
		duration = end - start;

		std::cout << "Time: " << duration.count() << "s\n";
	}
};



/* functions to perform multi-threading -------------------------------------------------------------------------------------*/

void build_nefs_subset_1(
	std::vector<JsonHandler>* jtr, 
	std::vector<Nef_polyhedron>* Nefs_1, 
	Nef_polyhedron* big_nef_1,
	double minkowski_param = 0.1)
{		
	/*
	* for lod 2.2 buildings, activating triangulation process is a must
	* for lod 1.2 and lod 1.3, users can select whether to activate the triangulation process
	*/
	bool triangulation_tag(false);
	if (std::abs(lod - 2.2) < epsilon) {
		triangulation_tag = true;
		std::cout << "current lod level: " << lod << ", the triangulation process is activated" << '\n';
	}
	else if (_ENABLE_TRIANGULATION_) {
		triangulation_tag = true;
		std::cout << "current lod level: " << lod << '\n';
		std::cout << "_ENABLE_TRIANGULATION_ is set to true, triangulation process is activated" << '\n';
	} 


	/*
	* build nef polyhedra
	*/
	for (auto const& jhandler : (*jtr)) {
		//std::cout << "proceed with thread 2" << '\n';
		BuildPolyhedron::build_nef_polyhedron(jhandler, *Nefs_1, triangulation_tag);
	} // build nefs for each jhandler


	/*
	* if defined _ENABLE_MINKOWSKI_SUM_, firstly applying minkowski sum on the nef and then performing union operation
	* if not, directly perform the union operation
	*/

#ifdef _ENABLE_MINKOWSKI_SUM_
	std::cout << "performing minkowski sum for nefs subset 1..." << '\n';
	for (auto& nef : *Nefs_1)
	{
		Nef_polyhedron merged_nef = NefProcessing::minkowski_sum(nef, minkowski_param); // cube size is 0.1 by default, can be altered
		(*big_nef_1) += merged_nef;
	}
	std::cout << "nefs subset 1 finished" << '\n';
#else
	for (auto& nef : *Nefs_1) {
		(*big_nef_1) += nef;
	}
#endif // _ENABLE_MINKOWSKI_SUM_

}


void build_nefs_subset_2(
	std::vector<JsonHandler>* jtr, 
	std::vector<Nef_polyhedron>* Nefs_2, 
	Nef_polyhedron* big_nef_2,
	double minkowski_param = 0.1)
{
	/*
	* for lod 2.2 buildings, activating triangulation process is a must
	* for lod 1.2 and lod 1.3, users can select whether to activate the triangulation process
	*/
	bool triangulation_tag(false);
	if (std::abs(lod - 2.2) < epsilon) {
		triangulation_tag = true;
		std::cout << "current lod level: " << lod << ", the triangulation process is activated" << '\n';
	}
	else if (_ENABLE_TRIANGULATION_) {
		triangulation_tag = true;
		std::cout << "current lod level: " << lod << '\n';
		std::cout << "_ENABLE_TRIANGULATION_ is set to true, triangulation process is activated" << '\n';
	}
	

	/*
	* build nef polyhedra
	*/
	for (auto const& jhandler : (*jtr)) {
		//std::cout << "proceed with thread 1" << '\n';
		BuildPolyhedron::build_nef_polyhedron(jhandler, *Nefs_2, triangulation_tag);
	} // build nefs for each jhandler


	/*
	* if defined _ENABLE_MINKOWSKI_SUM_, firstly applying minkowski sum on the nef and then performing union operation
	* if not, directly perform the union operation
	*/

#ifdef _ENABLE_MINKOWSKI_SUM_
	std::cout << "performing minkowski sum for nefs subset 2..." << '\n';
	for (auto& nef : *Nefs_2)
	{
		Nef_polyhedron merged_nef = NefProcessing::minkowski_sum(nef, minkowski_param); // cube size is 0.1 by default, can be altered
		(*big_nef_2) += merged_nef;
	}
	std::cout << "nefs subset 2 finished" << '\n';
#else
	for (auto& nef : *Nefs_2) {
		(*big_nef_2) += nef;
	}
#endif

}
/* functions to perform multi-threading -------------------------------------------------------------------------------------*/


// when multi-threading is not enabled
void build_nefs(std::vector<JsonHandler>* jtr, std::vector<Nef_polyhedron>* Nefs, Nef_polyhedron* big_nef)
{
	/*
	* for lod 2.2 buildings, activating triangulation process is a must
	* for lod 1.2 and lod 1.3, users can select whether to activate the triangulation process
	*/
	bool triangulation_tag(false);
	if (std::abs(lod - 2.2) < epsilon) {
		triangulation_tag = true;
		std::cout << "current lod level: " << lod << ", the triangulation process is activated" << '\n';
	}
	else if (_ENABLE_TRIANGULATION_) {
		triangulation_tag = true;
		std::cout << "current lod level: " << lod << '\n';
		std::cout << "_ENABLE_TRIANGULATION_ is set to true, triangulation process is activated" << '\n';
	}


	/*
	* build nef polyhedra
	*/
	for (auto const& jhandler : (*jtr)) {
		BuildPolyhedron::build_nef_polyhedron(jhandler, *Nefs, triangulation_tag);
	} // build nefs for each jhandler


	/*
	* if defined _ENABLE_MINKOWSKI_SUM_, firstly applying minkowski sum on the nef and then performing union operation
	* if not, directly perform the union operation
	*/
	
#ifdef _ENABLE_MINKOWSKI_SUM_
	for (auto& nef : *Nefs)
	{
		Nef_polyhedron merged_nef = NefProcessing::minkowski_sum(nef, 0.1); // cube size is 1.0 by default, can be altered
		(*big_nef) += merged_nef;
	}
#else
	for (auto& nef : *Nefs) {
		(*big_nef) += nef;
	}
#endif
}



// entry point
int main(int argc, const char** argv)
{
	Timer timer; // count the run time
	
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
	std::string filename = "\\3dbag_v210908_fd2cee53_5907.json";
	std::cout << "current reading file is: " << DATA_PATH + filename << '\n';
	std::ifstream input(DATA_PATH + filename);
	json j;
	input >> j;
	input.close();

	// get ids of adjacent buildings
	const char* adjacency[] = { "NL.IMBAG.Pand.0503100000018412-0" , "NL.IMBAG.Pand.0503100000018408-0" };


	//read certain building, stores in jhandlers vector
	std::vector<JsonHandler> jhandlers;
	//jhandlers.reserve(size); -> if we know the length of adjacency lsit, we can use reserve()
	std::cout << "------------------------ building(part) info ------------------------\n";
	
	for (auto const& building_name : adjacency) // get each building
	{
		JsonHandler jhandler;
		jhandler.read_certain_building(j, building_name, lod); // read in the building
		jhandler.message();
		jhandlers.emplace_back(jhandler); // add to the jhandlers vector
	}

	std::cout << "---------------------------------------------------------------------\n";


#ifdef _ENABLE_MULTI_THREADING_
	std::cout << "enable multi threading" << '\n';
	unsigned int size = (unsigned int)jhandlers.size(); // i.e. size = 23

	unsigned int size_1 = size / 2; // i.e. size_1 = 11
	unsigned int size_2 = size - size_1; // i.e. size_2 = 12

	// now split the jhandlers into two subsets
	std::vector<JsonHandler> jhandlers_subset_1;
	jhandlers_subset_1.reserve(size_1);
	std::vector<JsonHandler> jhandlers_subset_2;
	jhandlers_subset_2.reserve(size_2);

	// get sub_set 1
	for (unsigned int i = 0; i != size_1; ++i) {
		jhandlers_subset_1.emplace_back(jhandlers[i]);
	}

	// get sub_set 2
	for (unsigned int i = size_1; i != size; ++i) {
		jhandlers_subset_2.emplace_back(jhandlers[i]);
	}

	std::cout << "total size: " << jhandlers.size() << '\n';
	std::cout << "sub_set 1 size: " << jhandlers_subset_1.size() << '\n';
	std::cout << "sub_set 2 size: " << jhandlers_subset_2.size() << '\n';

	//two Nefs subsets, correspond to two jhandlers subsets
	std::vector<Nef_polyhedron> Nefs_1;
	Nefs_1.reserve(size_1);
	std::vector<Nef_polyhedron> Nefs_2;
	Nefs_2.reserve(size_2);

	//two big nefs
	Nef_polyhedron big_nef_1;
	Nef_polyhedron big_nef_2;


	// get nef, and apply minkowski sum, get big nef -----------------------------------------


	// std::async for build_nefs_subset_1() function - in a new thread
	std::future<void> resultFromSet1 = std::async(
										std::launch::async, 
										build_nefs_subset_1, 
										&jhandlers_subset_1, 
										&Nefs_1, 
										&big_nef_1,
										minkowski_param);

	// build_nefs_subset_2() function, in the original thread
	build_nefs_subset_2(&jhandlers_subset_2, &Nefs_2, &big_nef_2, minkowski_param);

	resultFromSet1.get(); // this is important, we need to wait until func1 finishes

	std::cout << "Nefs subset 1 size: " << Nefs_1.size() << '\n'; // i.e. 11
	std::cout << "Nefs subset 2 size: " << Nefs_2.size() << '\n'; // i.e. 12

	std::cout << "building big nef ..." << '\n';
	Nef_polyhedron big_nef;
	big_nef = big_nef_1 + big_nef_2;
	std::cout << "done" << '\n';
#else
	std::cout << "not enable multi-threading" << '\n';
	std::cout << "building big nef ..." << '\n';
	std::vector<Nef_polyhedron> Nefs;
	Nef_polyhedron big_nef;
	build_nefs(&jhandlers, &Nefs, &big_nef);
	std::cout << "done" << '\n';
#endif

	// check if big Nef is simple - simple: no internal rooms, not simple: multiple rooms?
	std::cout << "is bigNef simple? " << big_nef.is_simple() << '\n';

	// process the big nef to make it available for output
	std::vector<Shell_explorer> shell_explorers; // store the extracted geometries
	NefProcessing::extract_nef_geometries(big_nef, shell_explorers); // extract geometries of the bignef
	NefProcessing::process_shells_for_cityjson(shell_explorers); // process shells for writing to cityjson



	/* ------------------------------------------------------------- */

	// build surface mesh
	std::cout << "building mesh ...\n";
	Mesh mesh;
	BuildPolyhedron::build_surface_mesh(jhandlers[0], mesh);
	std::cout << "done\n";
	//std::cout << mesh << std::endl;


	/* ------------------------------------------------------------- */


	/* ------------------------------------------------------------- */
	std::cout << "calculating Hausdorff distance ...\n";
	Polyhedron poly1;
	Nefs[0].convert_to_polyhedron(poly1);
	Polyhedron poly2;
	Nef_polyhedron N0_merged = NefProcessing::minkowski_sum(Nefs[0], 0.01);
	N0_merged.convert_to_polyhedron(poly2);

	//Mesh mesh_1;
	//Mesh mesh_2;

	//CGAL::copy_face_graph(poly1, mesh_1);
	//CGAL::copy_face_graph(poly2, mesh_2);

	CGAL::Polygon_mesh_processing::triangulate_faces(poly1);
	CGAL::Polygon_mesh_processing::triangulate_faces(poly2);
	double hausdorff_distance = CGAL::Polygon_mesh_processing::approximate_Hausdorff_distance<TAG>(poly1, poly2);

	std::cout << "hausdorff distance is: " << hausdorff_distance << '\n';

	/* ------------------------------------------------------------- */

	
#ifdef _ENABLE_CONVEX_HULL_
	/* get the convex hull of the big_nef, use all cleaned vertices of all shells */
	// get cleaned vertices of shell_explorers[0] - the shell indicating the exterior of the big nef
	std::vector<Point_3>& convex_vertices = shell_explorers[0].cleaned_vertices;

	// build convex hull of the big nef
	Polyhedron convex_polyhedron; // define polyhedron to hold convex hull
	Nef_polyhedron convex_big_nef;
	CGAL::convex_hull_3(convex_vertices.begin(), convex_vertices.end(), convex_polyhedron);
	std::cout << "is convex closed? " << convex_polyhedron.is_closed() << '\n';
	if (convex_polyhedron.is_closed()) {
		std::cout << "build convex hull for the big nef...\n";
		Nef_polyhedron convex_nef(convex_polyhedron);
		convex_big_nef = convex_nef;
		std::cout << "build convex hull for the big nef done\n";
	}

	// process the convex big nef to make it available for output
	std::vector<Shell_explorer> convex_shell_explorers;
	NefProcessing::extract_nef_geometries(convex_big_nef, convex_shell_explorers);
	NefProcessing::process_shells_for_cityjson(convex_shell_explorers);
#endif


    // write file
	JsonWriter jwrite;
	std::string writeFilename = "\\building_18412_0.json";
	const Shell_explorer& shell = shell_explorers[0]; // which shell is going to be written to the file, 0 - exterior, 1 - interior
	std::cout << "writing the result to cityjson file...\n";
	jwrite.write_json_file(DATA_PATH + writeFilename, shell, lod);

	return 0;
}