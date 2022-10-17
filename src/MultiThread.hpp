#pragma once

#include <vector>
#include <functional> // for std::reference_wrapper<T>
#include <future> // for std::async
#include <mutex> // for std::mutex
#include <chrono> // for Timer
#include <thread> // for std::this_thread::sleep_for(seconds(5));

#include "Polyhedron.hpp"

using namespace std::chrono;

namespace Build_Nefs_Multi {

	std::vector<Nef_polyhedron*> m_nef_ptrs; // hold the nefs
	std::vector<std::future<void>> m_futures; // store the return value of std::async, necessary step to make async work
	std::mutex s_nefs_mutex; // for thread-safety


    Nef_polyhedron* get_one_nef(const std::string& building_id)
    {
        std::this_thread::sleep_for(seconds(5));

        // ... get nef, get minkowski-expanded nef
        Nef_polyhedron* nef_ptr = new(std::nothrow) Nef_polyhedron;
        if (nef_ptr == nullptr) {
            std::cerr << "pointer allocation not succeed, please check get_one_nef() function" << std::endl;
            return nullptr;
        }

        // operations ... get polyhedron, get nef, get minkowski nef

        return nef_ptr; // further optimization: using pointers
    }
    
    
    
    void get_nefs(std::string building_id) {

        Nef_polyhedron* nef_ptr = get_one_nef(building_id);
        if (nef_ptr == nullptr) {
            std::cerr << "pointer allocation not succeed, please check get_single_nef() function" << std::endl;
            return;
        }

        // using a local lock_guard to lock mtx guarantees unlocking on destruction / exception:
        std::lock_guard<std::mutex> lock(s_nefs_mutex); // lock the meshes to avoid conflict
        m_nef_ptrs.emplace_back(nef_ptr);

    }



    // in this function we call std::async() method
    void get_nefs_async(const std::vector<std::string>& building_ids/* read from a .txt file */) {
        
        std::cout << "size of building_ids: " << building_ids.size() << std::endl;

        /*
        * it is important to save the result of std::async()
        * to enable the async process
        */
        for (const auto& building_id : building_ids) {
            m_futures.emplace_back(std::async(std::launch::async, get_nefs, building_id));
            //get_nefs(building_id);
        }


        /*
        * if we wish to get the result value and keep processing
        * we need to use get() of every future object
        */
        for (auto& futureObject : m_futures) {
            futureObject.get();
        }
    }


    void clean() {
        for (unsigned int i = 0; i != m_nef_ptrs.size(); ++i) {
            delete m_nef_ptrs[i];
            m_nef_ptrs[i] = nullptr;
        }
    }


};