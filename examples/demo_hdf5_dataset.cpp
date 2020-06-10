#include "../DataPipeline.h"

#include "EERA_h5types.h"
using namespace EERAModel;

#include <cassert> // will be disabled if NDEBUG macro is defined

// https://support.hdfgroup.org/HDF5/doc/cpplus_RM/compound_8cpp-example.html

#define H5FILE_NAME "DS_compound.h5"
#define DS_NAME "DataStruct"

int main()
{
    std::shared_ptr<H5File> file = std::make_shared<H5File>(H5FILE_NAME, H5F_ACC_TRUNC);

    init_h5_type();
    std::vector<InfectionState> values;
    values.push_back({1, 2, 3, 4});
    values.push_back({10, 20, 30, 40});

    //Group* g = new H5File(H5FILE_NAME, H5F_ACC_RDONLY);  // why not working?
    data::IO::WriteVector<InfectionState>(values, file, DS_NAME, ds_type);
    auto v = data::IO::ReadVector<InfectionState>(file, DS_NAME, ds_type);
    assert(v.size() == 2);

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

    file->close();
}