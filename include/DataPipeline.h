#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <type_traits> // C++11

#include "ParameterTypes.h" // model input parameter types in toml
// type cast between C++ user types to toml can be done by code_generator.py

// "ResultTypes.h"    // model result output types in HDF5
#include "HDF5IO.h"
//#include "ResultTypes_h5types.h"  //  generated by code_generator.py

/// TODO: namespace naming needs reconsideration
namespace data
{
    class FileAPI
    {
    };

    /** 
     * NOTE: all methods must be in a class, NOT namespace to be a Header only lib
     * */
    class StandardAPI : public FileAPI
    {
    public:
        void write_distribution(const Distribution &)
        {
        }

        void write_estimation(const Estimation &)
        {
        }

        Distribution read_distribution(const std::string name)
        {
        }

        Estimation read_estimation(const std::string name)
        {
        }

        /// H5::Table or H5::DataSet
        static void write_table()
        {
        }

        /// == WriteVector ?
        static void write_array()
        {
        }
    };
} // namespace data