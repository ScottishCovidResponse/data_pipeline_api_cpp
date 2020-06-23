**generate H5Type for any C++ data class, clang's python binding.**

Copyright Qingfeng Xia @ UKAEA, 2020  
License:  same as RAMP

## Tutorial
Usage:  `h5type_generator.py input_header.h output_header_filename.h`

The generated header file `output_header_filename.h` declares a serial of `H5::CompType <class_name>_h5type` and `init_h5types();` in the original input header's same namespace. `init_h5types();` insert field type  def into those complex type declared.

### header, class and field requirement:
+ c-style struct/ C++ trivial-copyable data class  with public member will be saved
+ all classes in a single namespace
+ generate header-only, but it can be split into h and cpp in the future
field type supported
+ all builtin type like `int, double`
+ 1D fixed-size C-style array e.g. `int[3]`

=== yet completed or tested ===
+ Non-trivial C++ class with the help of the generated `serializer` function
+ C++ class with member of `std::vector, std::array`
+ string types:  C string `char*` and `std::string` hdf5 string type is special
+ For 2D matrix, using `Eigen::Matrix HDF5_Eigen`

## Future plan:
+ generate unit test 
+ serializer and deserializer functions
+ only generate h5 types for a selected
+ template class
+ filter class by hint in comment  `@to_hdf5`
+ QT types, which has property
+ for private fields: inject code into original class,  or derive class to expose 

https://github.com/cool-RR/PySnooper

## Installation

HDF5 C++ official API. 
The python module `clang` is clang C API's official python binding.

Install 
1. using pip `pip3 install clang`, you will also need to install `libclang-6`
To ensure the shared lib can be found, set the path by 
`Config.set_library_file("/usr/lib/llvm-6.0/lib/libclang.so.1")`

2. using linux software manager command line:  `apt install python3-clang`
