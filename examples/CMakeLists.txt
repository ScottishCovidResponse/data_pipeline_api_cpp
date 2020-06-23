if(ENABLE_HDF5)

    add_executable(CodeGen_demo
    CodeGen_demo.cpp
    )
    target_link_libraries(
        CodeGen_demo
        ${_hdf5_libs}
    )

    add_executable(EERA_pipeline_demo
    EERA_pipeline_demo.cpp
    )
    target_link_libraries(
        EERA_pipeline_demo
        ${_hdf5_libs}
    )
    # if this repo is sibling folder of EERA repo/root folder
    # if this repo is a submodule of EERA, then it should ignore this lines below
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../Covid19_EERAModel")
        add_definitions("-DDATA_USE_EERA")
        include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../Covid19_EERAModel/src/")
    endif()
    
    target_compile_options(EERA_pipeline_demo PRIVATE -Wall -Wextra -Wno-unused)
    # C++17 is needed to contructor types without constructor
    set_target_properties(EERA_pipeline_demo PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
    )
endif()


if(ENABLE_NETWORKING)
    if(Poco_FOUND)  # add executable
    add_executable(data_client_example  "data_client_example.cpp")
    target_link_libraries(data_client_example 
        PUBLIC ${Poco_LIBRARIES}
    )
    target_compile_options(data_client_example PRIVATE -Wall -Wextra -Wno-unused)

    else()
    message(FATAL_ERROR "Poco components is not found")
    endif()

endif()