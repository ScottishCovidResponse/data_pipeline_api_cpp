if(ENABLE_HDF5)

    add_executable(demo_hdf5_dataset
    demo_hdf5_dataset.cpp
    )
    target_link_libraries(
        demo_hdf5_dataset
        ${_hdf5_libs}
    )
    target_compile_options(demo_hdf5_dataset PRIVATE -Wall -Wextra -Wno-unused)
    set_target_properties(demo_hdf5_dataset PROPERTIES
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