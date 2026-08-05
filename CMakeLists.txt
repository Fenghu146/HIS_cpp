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
    data/patient_manager.h
    data/patient_manager.cpp
    data/doctor_manager.h
    data/doctor_manager.cpp
    data/dept_manager.h
    data/dept_manager.cpp
    data/drug_manager.h
    data/drug_manager.cpp
)
