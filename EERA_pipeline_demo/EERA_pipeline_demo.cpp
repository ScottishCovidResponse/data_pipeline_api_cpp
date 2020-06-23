//#include "DataPipeline.h"
#include "HDF5IO.h"

#include "EERA_types_toml.h"
#ifdef DATA_USE_HDF5_GENERATOR
#include "EERA_types_hdf5.h" // generated boiler-plate code
#else
#include "EERA_hw_h5types.h" // hand-written boiler-plate code
#endif
using namespace EERAModel;

#include <cassert> // will be disabled if NDEBUG macro is defined

// https://support.hdfgroup.org/HDF5/doc/cpplus_RM/compound_8cpp-example.html

#define H5FILE_NAME "DS_EERA.h5"
#define DS_NAME "DataStruct"

void test_toml()
{
#if DATA_USE_TOML

    toml::value tvalue{{"a", 1}, {"b", 2.3}, {"c", "message"}};
    // there must be a better way for cast
    //ext::foo ef = toml::from<ext::foo>::from_toml(tvalue);
    auto ef = toml::get<ext::foo>(tvalue);
    std::cout << "toml cast into C++ type: " << ef.c << std::endl;
    assert(ef.a == 1);

    // from C++ type to toml value is also possible
    toml::value t_value1(ef);
#endif
}

void test_EERA(std::shared_ptr<H5File> file)
{

    std::vector<InfectionState> values;
    InfectionState v1 = {1, 2, 3, 4}; // Aggregate initialization in C++17
    InfectionState v2 = {10, 20, 30, 40};
    values.push_back(v1); // Aggregate initialization inside push_back() needs C++17
    values.push_back(v2);

    data::IO::WriteVector<InfectionState>(values, file, DS_NAME, InfectionState_h5type);
    auto va = data::IO::ReadVector<InfectionState>(file, DS_NAME, InfectionState_h5type);
    assert(va.size() == 2);
}

int main()
{
    /// this section demo parsing model input parameter from toml files
    test_toml();

    /// this part demo model result writting into HDF5
    std::shared_ptr<H5File> file = std::make_shared<H5File>(H5FILE_NAME, H5F_ACC_TRUNC);
    init_h5types();

    test_EERA(file);
    file->close();
    std::cout << "demo completed successfully\n";
}