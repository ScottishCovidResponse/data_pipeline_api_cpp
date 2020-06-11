#pragma once

#include <H5Cpp.h>

#if 0
#include "../ModelTypes.h"
#include "../Model.h"

#else
struct InfectionState
{
    int detected = 0;        /*!< Number of newly detected (due to testing or hospitalisation) cases at each time step. */
    int hospitalised = 0;    /*!< Number of newly detected cases due to their hospitalisation at each time step. */
    int deaths = 0;          /*!< Overall number of incident deaths due to covid at each time step.  */
    int hospital_deaths = 0; /*!< Number of incident deaths reported at hospital due to covid at each time step. */
};
#endif

struct ComplexData
{
    double scalar;
    int int_array[3]; // H5::ArrayType, there is padding if alignment == 8
    const char *cstr; // H5::StrType string_type(H5::PredType::C_S1, 1);
    //std::string str;  // std::string is not trivial copyable
    InfectionState state;
};

/// for each data structure type, should be defined in a new header for EERA model
using namespace H5;

namespace EERAModel
{
    /// TODO: code here must be enclosed by a class, in order to work as head-only library
    CompType ds_type;
    CompType cd_type;

    /// boiler-plate code, to define a new user type
    void init_h5_type()
    {
        ds_type = CompType(sizeof(InfectionState));
        // use H5::PredType::NATIVE_FLOAT, not H5::H5T_NATIVE_FLOAT  otherwise runtime error
        ds_type.insertMember("detected", HOFFSET(InfectionState, detected), PredType::NATIVE_INT);
        ds_type.insertMember("hospitalised", HOFFSET(InfectionState, hospitalised), PredType::NATIVE_INT);
        ds_type.insertMember("deaths", HOFFSET(InfectionState, deaths), PredType::NATIVE_INT);
        ds_type.insertMember("hospital_deaths", HOFFSET(InfectionState, hospital_deaths), PredType::NATIVE_INT);

        cd_type = CompType(sizeof(ComplexData));
        hid_t strtype = H5Tcopy(H5T_C_S1);
        H5Tset_size(strtype, H5T_VARIABLE);

        hsize_t adims[] = {3};
        auto array_type = ArrayType(PredType::NATIVE_INT, 1, adims);
        cd_type.insertMember("scalar", HOFFSET(ComplexData, scalar), PredType::NATIVE_DOUBLE);
        cd_type.insertMember("int_array", HOFFSET(ComplexData, int_array), array_type);
        cd_type.insertMember("cstr", HOFFSET(ComplexData, cstr), strtype);
        cd_type.insertMember("state", HOFFSET(ComplexData, state), ds_type);
    }

} // namespace EERAModel