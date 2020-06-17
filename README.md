# C++ data management library for Covid19 RAMP

## Introduction RAMP

### Data pipeline

Unified data management and workflow

Model cross-validation PPT:  UoGlasgow Team shared files

Zulip link:  https://zulip.scrc.uk/#narrow/stream/14-Data/topic/Questions.20about.20the.20new.20data.20system

@Richard Reeve: who is PM for data management
@Ian Hinder: who is lead RSE for the other C++ project, 
@Chris Hughes: who has been working out a lot of the requirement/design details

more doc in university glasgow Team "Files"

### API design

![data_pipeline_diagram](./doc/datapipeline_api.png)

https://github.com/ScottishCovidResponse/SCRC-internal/wiki/Model-data-pipeline-interaction



Python API
https://github.com/ScottishCovidResponse/data_pipeline_api/tree/master/data_pipeline_api

R data API
https://github.com/ScottishCovidResponse/SCRCdataAPI

Data registry: database schema
https://github.com/ScottishCovidResponse/data-registry

https://dbdiagram.io/d/5ebbb29439d18f5553ff207e

### Plan for C++ API

1. component selection, cmake setup, and test locally
> done 
2. develop pipeline API in a separate repo
  todo: 
  rename method names to write_*()
  Distribution class: just use `boost::random` 
  Parameter class
  unit test: 

3. integrate with EERA model,  CI install `libhdf5-dev`
  define h5 data types, see example EERA_h5types.h
  using macro to switch between current local csv files IO, and HDF5 IO

4. model cross-validation: 
  the key task is the data structure definition for parameter and result

5. how to pack/write post-processing result(currently in R by Thibaud) 
 Image can be pushed into HDF5, but log file may be treated as string
 Parameter are treated as HDF5::Table


## Third-party library selection
+ poco for networking: the only lib I have used, 
  other: restfulcpp, etc 
  header only: https://github.com/yhirose/cpp-httplib

+ json.hpp header only C++ json: extensible to decode/encode user types
+ HDF5: for result packing
+ toml11: header only lib: https://github.com/ToruNiina/toml11#converting-a-table
+ python style C++17 argparse: https://github.com/p-ranav/argparse

### HDF5 C++ libraries

1. Official C++ aPI
   	There are official C API and C++ wrapper `H5CPP`. 
   	**H5CPP is now MPI capable given c++17 capable MPI compiler**

	HDF5 v1.8 has significantly difference with HDF5 v1.10
   	https://support.hdfgroup.org/HDF5/doc/ADGuide/Changes.html

	For Eigen::Matrix support
	https://github.com/garrison/eigen3-hdf5  does not work with HDF5 1.10
	https://github.com/Gjacquenot/eigen3-hdf5 works with HDF5 1.10

2. HighFive
	https://github.com/BlueBrain/HighFive
	Header-only, with `T[][], boost::ublas, Eigen::Matrix, XTensor` support

### HDF5 file viewer on Ubuntu 18.04
Ubuntu 18.04 's **hdfview** from official repository has bug, it can not view the data.

`pip install h5pyViewer`  but it is python2 only, not updated since 2015.

`pip3 install vitables` also have some problem, pip3 can not install PyQt5,  even  after PyQt5 has been installed from system package manager,  pip3 seems does not find it,

so I download vitables.3.0.2 source code,  comment out 2 lines in `setup.py` and install it.


## Installation

This is C++ API is designed to be header-only, while dependency libraries need to be installed, see guide below. This API can be used  as a git submodule, or just copy this repo folder into specific model project, and then include this folder in the project root CMakeLists.txt.
Or git clone this repo as the sibling folder of model repo, e.g. `EERAModel`, it is just header include directionary to be sorted out in CMake

note: as git module is used, add `--recursive`when clone this repo, i.e. `git clone --recursive this_repo`. read more on using submodule: <https://www.vogella.com/tutorials/GitSubmodules/article.html>


### Code structure

Generic API
+ DataPipeline.h: the generic API for all C++ models
+ Restful.h: get json or file by URL

EERA model specific header and demo
 2 files

Some header only libraries are downloaded and copied into this repository for the moment.  `git submodule add -b master https://github.com/ToruNiina/toml11.git`
+ json.hpp   download as a copy
+ toml.hpp    git submodule 
+ eigen3-hdf5.hpp  download as a copy, with modification


### Install dependency on Ubuntu 18.04/20.04

`sudo apt install libssl-dev, libpoco-dev, libhdf5-dev`
optional: 
`sudo apt install libeigen3-dev`

### RHEL/Centos 7
Centos versions are older than Ubuntu, sometimes versions cause trouble.
poco-devel-1.6.1-3.el7.x86_64.rpm
hdf5-devel-1.8.12-11.el7.x86_64.rpm
`yum install epel-release -y && yum install poco-devel openssl-devel cmake3 hdf5-devel -y`

optional: `yum install install eigen3-devel`

This project's cmake can download and build latest HDF5 lib in local build folder. 

#### MacOS 
homebrew maybe is the way to go. The dependencies name and version can be checked online
https://formulae.brew.sh/formula/

If brew is not installed yet, run `/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"`

Install dependencies: `brew update && brew install eigen poco hdf5 openssl`

On macos-latest (), the default HDF5 version is 1.12. It is possible to install `hdf5@1.10`, but cmake failed to find it, version 1.12 is used.

###  build with cmake

Currently, it is not clear where this source folder should locate.
Only 2 demo cpp to test out platform cmake configuration. 


## Data pipeline flow 

To get started, I would expected all EERA model input parameter will come in toml file format (restful get / local hdf5), adapted for EERA model if needed.   

HDF5 write is for result, as HDF5 is good for large dataset in IO performance.


### model data flow
-> download json parameters/dataset (by restful GET API) from central store

-> parse parameters into model-specific config file (for the time being)/C++ data structure as model input (later) 
-> prepare dataset into model-specific input file (for the time being)/C++ data structure as model input (later)  

-> model-specific simulation to generate log and result files
-> model-specific post-processing into images, report files

-> packed all into one HDF5 file
-> upload to central store

It is assume model needs 2 kinds of inputs
+ small textual config file to tweak model behavior
+ large dataset files

Model author will focus on model simulation code and judge the quality of result

## EERA specific
### [EERAModel_Data.md](EERAModel_Data.md)

Parameters def from Peter

https://github.com/ScottishCovidResponse/temporary_data/tree/peter-t-fox/444-add-eera-params-for-staging/temporary/Covid19_EERAModel/fixed-parameters

### Thibaud is working on Irish model
more general model workflow, abstracted from all other models
It has been merged on date ???
Kristian is working on Data Abstraction Layer

To enable correlation validation by other models,  a proper Result data structure

## Input Parameter class design

`template<typename T> class Parameter` is not preferred, because  mixed type parameter dictionary, such as `std::map<std::string, Parameter>` is desired. Meanwhile, some C++ object can hold value of dynamic types with type info accessible, such as `Poco::Dynamic::Var` or `std::any`. For example, `nlohmann::json` or `toml::value` has  `type()` and type trait functions
```c++
is_primitive , is_structured , is_null , is_boolean , is_number , is_number_integer , is_number_unsigned , is_number_float , is_object , is_array , is_string , is_discarded , is_binary
```

Alternatively, there is no need to declare a new C++ type Parameter, just use json or toml::value as the Parameter object. to generate the model input ini or fill the ModelInputParameters object.

```c++

//using DT=nlohmann::json;
using DT=toml::value;
struct Parameter
{
	//  as dict key
	std::string name; // short name, potential can be used to gen code,
	std::string desc; // description/ doc string
	// if std::any is used, then data type enum/string is needed, 
	// or detect from json, while advanced data type std::vector<ET>
	DT value;  // T any type supported by json, can be std::vector<ET>
	std::string unit;  // empty if it is a non-unit 
};

// parameter table, group
class  ParameterTable;
// generate model input need Input Parameter data structures in ModelTypes.h
```

An adapter design pattern may transform server side paramter table into model-specific input configuration. 

EERA has fixed parameters and distribution parameters

### Data pipeline API
https://github.com/ScottishCovidResponse/SCRC-internal/wiki/Model-data-pipeline-API-specification

```py
[point-estimate]
value = 1.5
```

https://github.com/ScottishCovidResponse/temporary_data/blob/peter-t-fox/444-add-eera-params-for-staging/temporary/Covid19_EERAModel/fixed-parameters/T_inf/metadata/public/20200612.toml
```toml
[source]
version = "0.1"
description = "Mean asymptomatic period"
type = "parameter"
location = "data/1.toml"
max_warning = 3
messages = ["3: point estimate"]
hash = "xxx"
```

### my Questions
I am not sure about parameter unit for EERA model input parameters, where I can find those unit info?  

> Peter had defined model input parameter

Is json/toml an accepted file format?  store toml as a attribute of HDF5?
> 

Is EERA ini format compatible with toml file format?
> 

Can we use python get() and put() file instead of implementing in C++?

### Todo

1. RESTFUL put() 







