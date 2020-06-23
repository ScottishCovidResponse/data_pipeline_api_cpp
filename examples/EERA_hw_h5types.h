#pragma once

//////// this part will be replaced by generated code soon /////////////////////////
#include <H5Cpp.h>
/// for each data structure type, should be defined in a new header for EERA model
using namespace H5;

namespace EERAModel
{

    /// TODO: code here must be enclosed by a class, in order to work as head-only library
    CompType InfectionState_h5type;
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
    }

} // namespace EERAModel