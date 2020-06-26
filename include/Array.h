// copyright, Alex Meakins and Qingfeng Xia  @ UKAEA
// it is released under open source like BSD
// https://github.com/simple-access-layer/source
#pragma once

#include <cstdint>
#include <vector>
#include <string>

// todo:  element_type()
// this code has been widely unit tested, unit test code can be copied here soon
//        encoding decoding from python.array

/// type trait to get data type name (numpy.dtype) at compiling time
/// used in Array<T> and Atomic<T> instantation
template <typename DT>
static const char *to_dtype_name()
{
    /// TODO: using DT = std::remove_cv<DType>::type;
    /// decay() from pointer or reference to type

    /// NOTE: std::byte, std::complex, not necessary
    /// `if constexpr ()` or constexpr template is only for C++17
    if (std::is_same<DT, int64_t>::value)
        return "int64";
    else if (std::is_same<DT, int32_t>::value)
        return "int32";
    else if (std::is_same<DT, int16_t>::value)
        return "int16";
    else if (std::is_same<DT, int8_t>::value)
        return "int8";
    else if (std::is_same<DT, uint64_t>::value)
        return "uint64";
    else if (std::is_same<DT, uint32_t>::value)
        return "uint32";
    else if (std::is_same<DT, uint16_t>::value)
        return "uint16";
    else if (std::is_same<DT, uint8_t>::value)
        return "uint8";
    else if (std::is_same<DT, float>::value)
        return "float32";
    else if (std::is_same<DT, double>::value)
        return "float64";
    else if (std::is_same<DT, bool>::value)
        return "bool";
    else if (std::is_same<DT, std::string>::value)
        return "string";
    else
    {
        throw SALException("data type valid as Array element or Atomic value");
    }
}

/// a typedef to ease future refactoring on data structure
using ShapeType = std::vector<uint64_t>;

/// base class for all Array<T> as non-templated interface to array metadata
class IArray
{
public:
    IArray(ShapeType _shape)
    {

        this->m_dimension = _shape.size();
        this->m_shape = _shape;
        this->m_strides.resize(this->m_dimension);

        // calculate strides
        int16_t i = this->m_dimension - 1;
        this->m_strides[i] = 1;
        i--;
        while (i >= 0)
        {
            this->m_strides[i] = this->m_strides[i + 1] * this->m_shape[i + 1];
            i--;
        }
    }

    virtual ~IArray() // virtual destructor when virtual functions are present.
    {
    }
    /// those functions below could be made non-virtual for better performance
    inline ShapeType shape() const
    {
        return this->m_shape;
    };
    /// consider: plural name
    inline size_t dimension() const
    {
        return this->m_shape.size();
    };
    inline ShapeType strides() const
    {
        return this->m_strides;
    };

    //virtual AttributeType element_type() const = 0;
    virtual std::string element_type_name() const = 0;

    /// @{
    /** infra-structure for C-API */
    virtual uint64_t size() const = 0;

    virtual size_t byte_size() const = 0;

    /// read-only pointer to provide read view into the data buffer
    virtual const void *data_pointer() const = 0;

    /// modifiable raw pointer to data buffer, use it with care
    virtual void *data_pointer() = 0;

    virtual void *data_at(int i0, int64_t i1 = -1, int64_t i2 = -1, int64_t i3 = -1, int64_t i4 = -1,
                          int64_t i5 = -1, int64_t i6 = -1, int64_t i7 = -1, int64_t i8 = -1,
                          int64_t i9 = -1) = 0;
    /// @}
protected:
    uint8_t m_dimension; // CONSIDER: size_t otherwise lots of compiler warning
    ShapeType m_shape;
    ShapeType m_strides;
};

/*
         It is a multi-dimension array based on std::vector<T>
         No default constructor without parameter is allowed,
         so shape of the array, as std::vector<uint64_t>,  consistent with python numpy.array
         TODO: proxy pattern for m_data, so big data can not fit into memory can be supported.
         */
template <class T>
class Array : public IArray
{

protected:
    /// this non-public constructor should be called by other public constructors
    Array(ShapeType _shape, AttributeType _element_type, const std::string _element_type_name)
        : IArray(_shape), m_element_type(_element_type), m_element_type_name(_element_type_name)
    {
        // calculate array buffer length
        uint64_t element_size = 1;
        for (uint64_t d : this->m_shape)
            element_size *= d;

        this->data.resize(element_size);
    }

public:
    typedef T value_type;

    /*
            Array constructor.

            Initialises an array with the specified dimensions (shape). The
            array shape is a vector defining the length of each dimensions
            of the array. The number of elements in the shape vector
            defines the number of dimensions.

            This class is not intended to be used directly by the users, a
            set of typedefs are provided that define the supported SAL
            array types. For example:

                // create a 1D uint8 array with 1000 elements.
                UInt8Array a1({1000});

                // create a 2D int32 array with 50x20 elements.
                Int32Array a2({50, 20});

                // create a 3D float array with 512x512x3 elements.
                Float32Array a3({512, 512, 3});

            */
    Array(ShapeType _shape)
        : Array(_shape, Attribute::to_dtype<T>(), Attribute::to_dtype_name<T>())
    {
    }

    /// move constructor by take over (steal, move) the content of input vector
    Array(ShapeType _shape, std::vector<T> &&vec)
        : Array(_shape, Attribute::to_dtype<T>(), Attribute::to_dtype_name<T>())
    {
        this->data = vec;
    }

    Array(ShapeType _shape, const std::vector<T> &vec)
        : Array(_shape, Attribute::to_dtype<T>(), Attribute::to_dtype_name<T>())
    {
        this->data = vec;
    }

    // CONSIDER: disable those constructors, force shared_ptr<>
    //            Array(const Array&);
    //            Array& operator= (const Array&);
    //            Array(Array&&);
    //            Array& operator= (Array&&);

    /*
            virtual destructor
            */
    virtual ~Array(){};

    inline virtual AttributeType element_type() const override
    {
        return m_element_type;
    }

    inline virtual std::string element_type_name() const
    {
        return m_element_type_name;
    }

    /// @{ STL container API
    /*
            Returns the length of the array buffer, element_size, not byte size
            flattened 1D array from all dimensions
            */
    inline virtual uint64_t size() const
    {
        return this->data.size();
    };

    /// todo: STL iterators
    /// @}

    /// @{ Infrastructure for C-API
    inline virtual size_t byte_size() const
    {
        /// NOTE: std::vector<bool> stores bit instead of byte for each element
        return this->data.size() * sizeof(T);
    };

    /// read-only pointer to provide read view into the data buffer
    inline virtual const void *data_pointer() const
    {
        // std::enable<> does not work for virtual function, so must check at runtime
        if (element_type_name() != "string")
            return this->data.data();
        else // Array<String> buffer addess does not contains contents but addr to content
        {
            throw SALException("Should not use Array<String>::data_pointer()");
        }
    }

    /// modifiable raw pointer to data buffer, use it with care
    inline virtual void *data_pointer()
    {
        if (element_type_name() != "string") // std::enable<> does not work for virtual function
            return this->data.data();
        else
        {
            throw SALException("Should not use Array<String>::data_pointer()");
        }
    }

    /// todo: more than 5 dim is kind of nonsense,
    /// using array as index can be more decent
    inline virtual void *data_at(int i0, int64_t i1 = -1, int64_t i2 = -1, int64_t i3 = -1, int64_t i4 = -1,
                                 int64_t i5 = -1, int64_t i6 = -1, int64_t i7 = -1, int64_t i8 = -1,
                                 int64_t i9 = -1) override
    {
        return std::addressof(this->at(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9));
    }
    /// @}

    /*
            Fast element access via direct indexing of the array buffer (flattened ID array).

            The Array holds the data in a 1D strided array. Indexing into
            multidimensional arrays therefore requires the user to
            appropriately stride across the data. See the stride attribute.

            No bounds checking is performed.
            */
    inline T &operator[](const uint64_t index)
    {
        return this->data[index];
    };

    inline const T &operator[](const uint64_t index) const
    {
        return this->data[index];
    };

    /// expose reference to the underneath data container (std::vector)
    inline std::vector<T> &values()
    {
        return this->data;
    };
    /// expose the const reference to the underneath data container (std::vector)
    inline const std::vector<T> &values() const
    {
        return this->data;
    };

    // C++14 provide <T indices ...>

    /// quickly access an element for 2D matrix row and col,  without bound check
    /// `array(row_index, col_index)`  all zero for the first element
    inline T &operator()(const uint64_t row, const uint64_t column)
    {
        // assert(m_dimension == 2);
        uint64_t index = row * this->m_strides[0] + column;
        return this->data[index];
    };

    inline const T &operator()(const uint64_t row, const uint64_t column) const
    {
        uint64_t index = row * this->m_strides[0] + column;
        return this->data[index];
    };

    /*
            Access an element of the array.

            std::vector<T> has two versions
            reference at (size_type n);
            const_reference at (size_type n) const;

            This method performs bounds checking and accepts a variable
            number of array indices corresponding to the dimensionality of
            the array.

            Data access is slower than direct buffer indexing, however it
            handles striding for the user.

            Due to the method of implementing this functionality in C++, it
            only supports arrays with a maximum of 10 dimensions.
            */
    virtual T &at(int i0, int64_t i1 = -1, int64_t i2 = -1, int64_t i3 = -1, int64_t i4 = -1, int64_t i5 = -1,
                  int64_t i6 = -1, int64_t i7 = -1, int64_t i8 = -1, int64_t i9 = -1) throw()
    {
        // NOTE: index are signed integer, assigning -1 means max unsigned interger
        if (this->m_dimension > 10)
        {
            throw std::out_of_range("The at() method can only be used with arrays of 10 dimensions of less.");
        }

        // convert the list or arguments to an array for convenience
        std::array<int64_t, 10> dim_index = {i0, i1, i2, i3, i4, i5, i6, i7, i8, i9};

        uint64_t element_index = 0;
        for (uint8_t i = 0; i < this->m_dimension; i++)
        {
            // check the indices are inside the array bounds
            if ((dim_index[i] < 0) || (static_cast<uint64_t>(dim_index[i]) > this->m_shape[i] - 1UL))
            {
                throw std::out_of_range("An array index is missing or is out of bounds.");
            }

            element_index += dim_index[i] * this->m_strides[i];
        }

        return this->data[element_index];
    }

protected:
    // change element type is not possible without re-create the array object
    const AttributeType m_element_type;
    const std::string m_element_type_name;
    std::vector<T> data;
};