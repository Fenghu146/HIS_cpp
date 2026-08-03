cmake_minimum_required(VERSION 4.3)
project(HIS_cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(HIS_cpp
    main.cpp
    config/his_config.h
    model/entity.h
    model/crud.h
    utils/input.h
    utils/input.cpp
    utils/validator.h
    utils/validator.cpp
    data/data_manager.h
)
