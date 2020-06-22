#include "../DataPipeline.h"

#include "EERA_types_toml.h"
#ifdef DATA_USE_HDF5_GENERATOR
#include "EERA_types_hdf5.h" // generated boiler-plate code
#else
#include "EERA_hw_h5types.h" // hand-written boiler-plate code
#endif
using namespace EERAModel;

#include <cassert> // will be disabled if NDEBUG macro is defined

// https://support.hdfgroup.org/HDF5/doc/cpplus_RM/compound_8cpp-example.html

#define H5FILE_NAME "DS_compound.h5"
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

void test_h5(std::shared_ptr<H5File> file)
{
    std::vector<CDataStruct> values;
    CDataStruct v1 = {1, 2.3, {1.0, 2.0}};   // Aggregate initialization in C++17
    CDataStruct v2 = {10, 12.3, {3.0, 4.0}}; // Aggregate initialization in C++17
    values.push_back(v1);                    // Aggregate initialization inside push_back() needs C++17
    values.push_back(v2);

    std::vector<ComplexData> cvalues;
    // Aggregate initialization inside push_back() needs C++17
    ComplexData cd1(1.0, v1); // = {1.0, {1, 2, 3}, "complex", v1};
    ComplexData cd2(2.0, v2); //  = {2.0, {4, 5, 6}, "complex", v2};
    cvalues.push_back(cd1);
    cvalues.push_back(cd2);

#if 0 // there is bug , stack smashing detected ***: terminated
    data::IO::WriteVector<ComplexData>(cvalues, file, "complex_data", ComplexData_h5type);

    auto cv = data::IO::ReadVector<ComplexData>(file, "complex_data", ComplexData_h5type);
    for (const ComplexData &v : cv)
    {
        std::cout << v.ds.integer << std::endl;
    }
#endif

    std::vector<std::vector<int>> mat = {{1, 2, 3}, {4, 5, 6}};
    data::IO::WriteMatrix<int>(mat, file, "IntMatrix", PredType::NATIVE_INT);

    auto m = data::IO::ReadMatrix<int>(file, "IntMatrix", PredType::NATIVE_INT);
    assert(m.size() == 2);

#if DATA_USE_EIGEN

    Eigen::Matrix3d emat;
    emat << 1, 2, 3, 4, 5, 6, 7, 8, 9;

    /// directly use EigenHDF5::save,  it is working
    EigenHDF5::save(*file, "EigenMatrixDataSet", emat); // first param is a reference  type

    // still some compiling error, anyway, just use EigenHDF5::save
    //data::IO::WriteEigen(emat, *file, "EigenMatrix");
    //auto em = data::IO::ReadEigen<Eigen::Matrix3d>(*file, "EigenMatrix");
#endif
}

int main()
{
    /// this section demo parsing model input parameter from toml files
    test_toml();

    /// this part demo model result writting into HDF5
    std::shared_ptr<H5File> file = std::make_shared<H5File>(H5FILE_NAME, H5F_ACC_TRUNC);
    init_h5types();
    //test_vlen(file);  // error
    test_h5(file);
    test_EERA(file);
    file->close();
    std::cout << "demo completed successfully\n";
}