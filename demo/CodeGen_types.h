#pragma once

#include <vector>
#include <string>
#include <array>

namespace CodeGen
{
    struct CDataStruct
    {
        int integer;
        double scalar;
        float scalar_array[3];
    };

    /// example complex type data to write into HDF5,  not a realist data type
    struct ComplexData
    {
        double scalar;
        int int_array[2];            // H5::ArrayType, there is padding if alignment == 8
        unsigned char byte_array[8]; // fixed length byte buffer
        CDataStruct ds;

#define DATA_USE_COMPLEX_FIELDS 1
#if DATA_USE_COMPLEX_FIELDS
        // char* is working, but can not write vector<ComplexData>.data()
        const char *c_str; // H5::StrType string_type(H5::PredType::C_S1, H5T_VARIABLE);
        // needs extra hvl_t for std::string
        //std::string std_str; // std::string is not trivial copyable

        std::array<double, 3> std_array; // it is fixed-size, trivially-copyable
        std::vector<int> vlen_vector;    // variable length array/vector

        //int &int_reference;  // must init in ctor
        //double *scalar_pointer;

        ComplexData(double _scalar, CDataStruct _ds, std::string _str, std::array<double, 3> _array, std::vector<int> _vector)
        {
            scalar = _scalar;
            ds = _ds;
            c_str = _str.c_str();
            //std_str = _str;
            std_array = _array;
            //vlen_vector = _vector;
        }
#endif

        ComplexData()
        {
        }

        ComplexData(double _scalar, CDataStruct _ds)
            : scalar(_scalar), ds(_ds)
        {
        }
    };
} // namespace CodeGen