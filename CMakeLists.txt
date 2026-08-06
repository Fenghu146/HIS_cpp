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
    manager/data_manager.h
    manager/patient_manager.h
    manager/patient_manager.cpp
    manager/doctor_manager.h
    manager/doctor_manager.cpp
    manager/dept_manager.h
    manager/dept_manager.cpp
    manager/drug_manager.h
    manager/drug_manager.cpp
    manager/bed_manager.h
    manager/bed_manager.cpp
    manager/appointment_manager.h
    manager/appointment_manager.cpp
    manager/medical_record_manager.h
    manager/medical_record_manager.cpp
    manager/prescription_manager.h
    manager/prescription_manager.cpp
    service/registration.h
    service/registration.cpp
    service/consultation.h
    service/consultation.cpp
)
