#pragma once

//////// this part will be replaced by generated code soon /////////////////////////
#include <H5Cpp.h>
/// for each data structure type, should be defined in a new header for EERA model
using namespace H5;

namespace EERAModel
{

    /// TODO: code here must be enclosed by a class, in order to work as head-only library
    CompType InfectionState_h5type;
    H5::CompType CDataStruct_h5type(sizeof(CDataStruct));
    CompType ComplexData_h5type(sizeof(ComplexData));
    CompType particle_type;

    /// boiler-plate code, to define a new user type
    void init_h5types()
    {
        InfectionState_h5type = CompType(sizeof(InfectionState));
        // use H5::PredType::NATIVE_FLOAT, not H5::H5T_NATIVE_FLOAT  otherwise runtime error
        InfectionState_h5type.insertMember("detected", HOFFSET(InfectionState, detected), PredType::NATIVE_INT);
        InfectionState_h5type.insertMember("hospitalised", HOFFSET(InfectionState, hospitalised), PredType::NATIVE_INT);
        InfectionState_h5type.insertMember("deaths", HOFFSET(InfectionState, deaths), PredType::NATIVE_INT);
        InfectionState_h5type.insertMember("hospital_deaths", HOFFSET(InfectionState, hospital_deaths), PredType::NATIVE_INT);

        hid_t strtype = H5Tcopy(H5T_C_S1);
        H5Tset_size(strtype, H5T_VARIABLE);

        CDataStruct_h5type.insertMember("integer",
                                        HOFFSET(CDataStruct, integer), PredType::NATIVE_INT);
        CDataStruct_h5type.insertMember("scalar",
                                        HOFFSET(CDataStruct, scalar), PredType::NATIVE_DOUBLE);
        hsize_t CDataStruct_scalar_array_dims[] = {3};
        auto CDataStruct_scalar_array_h5type = H5::ArrayType(PredType::NATIVE_FLOAT, 1, CDataStruct_scalar_array_dims);
        CDataStruct_h5type.insertMember("scalar_array",
                                        HOFFSET(CDataStruct, scalar_array), CDataStruct_scalar_array_h5type);
        // end of CompType member/field definition for CDataStruct

        ComplexData_h5type.insertMember("scalar", HOFFSET(ComplexData, scalar), PredType::NATIVE_DOUBLE);

        hsize_t adims[] = {2};
        auto array_type = ArrayType(PredType::NATIVE_INT, 1, adims);
        ComplexData_h5type.insertMember("int_array", HOFFSET(ComplexData, int_array), array_type);

        hsize_t ComplexData_char_array_dims[] = {8};
        auto ComplexData_char_array_h5type = H5::ArrayType(PredType::NATIVE_CHAR, 1, ComplexData_char_array_dims);
        ComplexData_h5type.insertMember("char_array",
                                        HOFFSET(ComplexData, char_array), ComplexData_char_array_h5type);

        ComplexData_h5type.insertMember("ds",
                                        HOFFSET(ComplexData, ds), CDataStruct_h5type);
    }

} // namespace EERAModel