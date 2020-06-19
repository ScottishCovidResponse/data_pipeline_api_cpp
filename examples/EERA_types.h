#pragma once

#if DATA_USE_EERA
#include "ModelTypes.h"

namespace EERAModel
{
    /// example complex type data to write into HDF5,  not a realist data type
    struct ComplexData
    {
        double scalar;
        int int_array[3]; // H5::ArrayType, there is padding if alignment == 8
        //const char *cstr; // H5::StrType string_type(H5::PredType::C_S1, 1);
        char cstr[7]; // fixed length
        //std::string str;  // std::string is not trivial copyable
        InfectionState state;

        ComplexData() {}

        ComplexData(double _scalar, InfectionState _state)
            : scalar(_scalar), state(_state)
        {
        }
    };
} // namespace EERAModel
#else
// copy some basic types for testing
#include <vector>

namespace EERAModel
{
    struct InfectionState
    {
        int detected = 0;        /*!< Number of newly detected (due to testing or hospitalisation) cases at each time step. */
        int hospitalised = 0;    /*!< Number of newly detected cases due to their hospitalisation at each time step. */
        int deaths = 0;          /*!< Overall number of incident deaths due to covid at each time step.  */
        int hospital_deaths = 0; /*!< Number of incident deaths reported at hospital due to covid at each time step. */
    };

    /*
    /// this is the EERA inference model output type
    /// this type is trivial/standard_layout, needs `<class_name>serialize()`
    // 2D non-contiguous `std::vector<std::vector<int>>` needs to be flattened
    */
    struct particle
    {
        double nsse_cases;
        double nsse_deaths;
        std::vector<double> parameter_set;
        int iter;
        double weight;
        std::vector<int> simu_outs;
        std::vector<int> hospital_death_outs;
        std::vector<int> death_outs;
        std::vector<std::vector<int>> end_comps;
    };

} // namespace EERAModel

#endif