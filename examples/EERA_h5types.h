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

/// for each data structure type, should be defined in a new header for EERA model
using namespace H5;

namespace EERAModel
{
    /// TODO: code here must be enclosed by a class, in order to work as head-only library
    CompType ds_type;

    /// boiler-plate code, to define a new user type
    void init_h5_type()
    {
        ds_type = CompType(sizeof(InfectionState));
        // use H5::PredType::NATIVE_FLOAT, not H5::H5T_NATIVE_FLOAT  otherwise runtime error
        ds_type.insertMember("detected", HOFFSET(InfectionState, detected), PredType::NATIVE_INT);
        ds_type.insertMember("hospitalised", HOFFSET(InfectionState, hospitalised), PredType::NATIVE_INT);
        ds_type.insertMember("deaths", HOFFSET(InfectionState, deaths), PredType::NATIVE_INT);
        ds_type.insertMember("hospital_deaths", HOFFSET(InfectionState, hospital_deaths), PredType::NATIVE_INT);
    }

} // namespace EERAModel