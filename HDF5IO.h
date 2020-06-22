#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <type_traits> // C++11
#include <functional>

#include <H5Cpp.h>
using namespace H5;

#if DATA_USE_EIGEN
// origin repo:  https://github.com/garrison/eigen3-hdf5  does not work with HDF5 1.10.1
// forked repo:  https://github.com/Gjacquenot/eigen3-hdf5 works with HDF5 1.10.1
#include "eigen3-hdf5.hpp"
//#include "eigen3-hdf5-sparse.hpp"
#endif

#if H5_VERSION_LE(1, 10, 0)
#define DATA_H5Location H5::CommonFG
#else
#define DATA_H5Location H5::Group
#endif

/// TODO: namespace naming needs reconsideration
namespace data
{
    /** 
     * NOTE: all methods must be in a class, NOT namespace to be a Header only lib, 
     * For Eigen3 library, there is third-party library `eigen3-hdf5` for serialization:
     * 
    */
    class IO
    {
    public:
        template <class T>
        using Serializer = std::function<bool(const T &, H5Object &)>;
        template <class T>
        using Deserializer = std::function<T(const H5Object &)>;

        // small helper
        void writeStringAttribute(H5Object *o, std::string key, std::string value)
        {
            StrType t_str = H5::StrType(H5::PredType::C_S1, value.size() + 1);
            Attribute at = o->createAttribute(key.c_str(), t_str, H5S_SCALAR);
            at.write(t_str, value.c_str());
        }

        /**
         * @brief raw buffer writing out, assuming little endian, alignment
         * 
         * @param vec data to write  of type `const std::vector<T>`
         * @param h5loc handle/pointer to H5File or H5::Group
         * @param attribute_name attribute name
         * @param dtype hdf5 data type definition (metadata)
         * 
         * @return true if successful 
         * 
         * */
        template <class T, typename = typename std::enable_if<std::is_trivially_copyable<T>::value>::type>
        static bool WriteAttribute(const T val, std::shared_ptr<DATA_H5Location> h5loc,
                                   std::string attribute_name, const DataType &dtype)
        {
            Attribute attrib(h5loc->createAttribute(attribute_name, dtype, H5S_SCALAR));

            const void *buf = std::addressof(val);
            attrib.write(dtype, buf); //  this write() applies to field/member DataType

            attrib.close();
            return true;
        }

        /**
         * @brief raw buffer writing out, assuming little endian, alignment
         * 
         * @param vec data to write  of type `const std::vector<T>`
         * @param h5loc handle/pointer to H5File or H5::Group
         * @param attribute_name attribute name
         * @param dtype hdf5 data type definition (metadata)
         * @param serializer std::function<bool(const T&, H5::Object&)>
         * 
         * @return true if successful 
         * 
         * */
        // ,typename = typename std::enable_if<!std::is_trivially_copyable<T>::value>::type
        template <class T>
        static bool WriteAttribute(const T &val, std::shared_ptr<DATA_H5Location> h5loc,
                                   std::string attribute_name, const DataType &dtype,
                                   Serializer<T> serializer)
        {
            hsize_t dims[1] = {1};
            DataSpace space(1, dims);
            Attribute attrib(h5loc->createAttribute(attribute_name, dtype, space));

            serializer(val, attrib);

            space.close();
            attrib.close();
            return true;
        }

        /**
         * @brief raw buffer writing out, assuming little endian, alignment
         * 
         * @param vec data to write  of type `const std::vector<T>`
         * @param h5loc handle/pointer to H5File
         * @param dataset_name dataset name
         * @param dtype hdf5 data type definition (metadata)
         * 
         * @return true if successful 
         * 
         * todo:  serializer
         *   is_contiguous  if T is numeric type, , typename = typename std::enable_if<std::is_trivially_copyable<T>::value>::type
         * 
         * */
        template <class T>
        static bool WriteVector(const std::vector<T> vec, std::shared_ptr<DATA_H5Location> h5loc,
                                std::string dataset_name, const DataType &dtype, Serializer<T> serializer = nullptr)
        {
            hsize_t dims[1] = {vec.size()};
            DataSpace space(1, dims);
            DataSet dataset(h5loc->createDataSet(dataset_name, dtype, space));

            // if buffer is contiguous, there is no need for a per element copy
            for (const auto &v : vec)
            {
                if (!serializer)
                {
                    const void *buf = std::addressof(v);
                    dataset.write(buf, dtype); // todo: will auto move pos cursor?
                }
                else
                {
                    // todo
                }
            }
            space.close();
            dataset.close();
            return true;
        }

        /**
         * @brief read dataset into a vector<DataStruct>
         * 
         * @param h5loc handle/pointer to H5File/Group
         * @param dtype hdf5 data type definition for the element
         * @param dataset_name dataset name
         * 
         * @return type `const std::vector<T>`
         * 
         * Template parameter `T` must be `std::is_trivially_copyable`
         * in order to use `memcpy()` to initialize class T
         * 
         * todo: , typename = typename std::enable_if<std::is_trivially_copyable<T>::value>::type
         *  then give the default val as nullptr
         * */
        template <class T>
        static const std::vector<T> ReadVector(std::shared_ptr<DATA_H5Location> h5loc, std::string dataset_name,
                                               const DataType &dtype, Deserializer<T> deserializer = nullptr)
        {
            DataSet dataset(h5loc->openDataSet(dataset_name));

            hsize_t dims[2];
            DataSpace space = dataset.getSpace();
            //assert(dataset.isSimple());                        // load all into memory from the whole file
            auto rank = space.getSimpleExtentDims(dims, NULL); // rank = 1
            const size_t length = dims[0];
            std::vector<T> vec;

            // if buffer is contiguous, all-in-one copy is possible
            // but it makes no diff per element or copy

            vec.reserve(length);
            for (size_t i = 0; i < length; i++)
            {
                T tmp;
                if (!deserializer)
                    dataset.read(&tmp, dtype); // memcpy()  todo: will auto move pos cursor?
                else
                {
                    //todo
                }
                vec.emplace_back(tmp);
            }
            space.close();
            dataset.close();
            return vec;
        }

        /**
         * @brief write 2D array into a H5::DataSet
         *  template parameter T  can be scalar or any trivially_copyable user type
         * 
         * @param mat data to write  of type `const std::vector<std::vector<T>>`
         * @param h5loc handle/pointer to H5File
         * @param dataset_name dataset name
         * @param dtype hdf5 data type definition for the element, CompType or PredType::NATIVE_*
         * 
         * @return true if successful 
         * 
         * */
        template <class T, typename = typename std::enable_if<std::is_trivially_copyable<T>::value>::type>
        static bool WriteMatrix(const std::vector<std::vector<T>> mat, std::shared_ptr<DATA_H5Location> h5loc,
                                const std::string dataset_name, const DataType &dtype)
        {
            const int RANK = 2;
            hsize_t colsize = mat[0].size();
            hsize_t offset[2] = {0, 0}; // starting point row, col index
            hsize_t count[2] = {1, 1};  // block count
            hsize_t stride[2] = {1, 1}; // block stride
            hsize_t block[2] = {1, colsize};
            hsize_t dims[RANK] = {mat.size(), colsize};
            DataSpace space(RANK, dims);
            DataSet dataset(h5loc->createDataSet(dataset_name, dtype, space));
            DataSpace memspace(RANK, block, NULL); // sub dataspace for each row

            // subset selection: hyperslab selections and element selections, to avoid creating T[][]
            // https://support.hdfgroup.org/ftp/HDF5/current/src/unpacked/c++/examples/h5tutr_subset.cpp
            for (const auto &v : mat)
            {
                space.selectHyperslab(H5S_SELECT_SET, count, offset, stride, block);
                const void *buf = v.data();
                dataset.write(buf, dtype, memspace, space);
                offset[0] = offset[0] + 1;
            }
            memspace.close();
            space.close();
            dataset.close();
            return true;
        }

        /**
         * @brief read 2D array from a H5::DataSet
         *  template parameter T  can be scalar or any trivially_copyable user type
         * 
         * @param h5loc handle/pointer to H5File
         * @param dataset_name dataset name
         * @param dtype hdf5 data type definition for the element, CompType or PredType::NATIVE_*
         * 
         * @return matrix of the type `std::vector<std::vector<T>>`
         * 
         * */
        template <class T, typename = typename std::enable_if<std::is_trivially_copyable<T>::value>::type>
        static std::vector<std::vector<T>> ReadMatrix(std::shared_ptr<DATA_H5Location> h5loc,
                                                      std::string dataset_name, const DataType &dtype)
        {
            std::vector<std::vector<T>> mat;
            const int RANK = 2;
            DataSet dataset(h5loc->openDataSet(dataset_name));

            hsize_t dims[RANK];
            DataSpace space = dataset.getSpace();
            //assert(dataset.isSimple());                        // load all into memory from the whole file
            auto rank = space.getSimpleExtentDims(dims, NULL); // rank = 1
            const hsize_t rowsize = dims[0];
            const hsize_t colsize = dims[1];

            hsize_t offset[RANK] = {0, 0}; // starting point row, col index
            hsize_t count[RANK] = {1, 1};  // block count
            hsize_t stride[RANK] = {1, 1}; // block stride
            hsize_t block[RANK] = {1, colsize};

            DataSpace memspace(RANK, block, NULL); // sub dataspace for each row

            // subset selection: hyperslab selections and element selections, to avoid creating T[][]
            // https://support.hdfgroup.org/ftp/HDF5/current/src/unpacked/c++/examples/h5tutr_subset.cpp
            for (hsize_t i = 0; i < rowsize; i++)
            {
                space.selectHyperslab(H5S_SELECT_SET, count, offset, stride, block);
                std::vector<T> vec(colsize);
                void *buf = vec.data();
                dataset.read(buf, dtype, memspace, space);
                mat.emplace_back(vec);
                offset[0] = offset[0] + 1;
            }
            memspace.close();
            space.close();
            dataset.close();
            return mat;
        }

#if DATA_USE_EIGEN
        /**
         *  write any Eigen dense Vector and Matrix types
         * */
        template <typename Derived>
        void WriteEigen(const Eigen::EigenBase<Derived> &mat, std::shared_ptr<DATA_H5Location> h5loc, const std::string dataset_name)
        {
            EigenHDF5::save(*h5loc, dataset_name, mat);
        }

        /**
         *  read any Eigen dense Vector and Matrix types
         * */
        template <typename Derived>
        Eigen::DenseBase<Derived> ReadEigen(std::shared_ptr<DATA_H5Location> h5loc, const std::string dataset_name)
        {
            Eigen::DenseBase<Derived> mat;
            EigenHDF5::load(*h5loc, dataset_name, mat);
            return mat;
        }
#endif
    };
} // namespace data