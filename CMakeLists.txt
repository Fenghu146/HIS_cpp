cmake_minimum_required(VERSION 3.16)

project(HIS_cpp LANGUAGES CXX)

# ---- 语言标准 ----
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 未显式指定构建类型时默认使用 Release
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ---- 源文件（头文件通过 #include 引入，无需在此列出）----
set(HIS_SOURCES
    main.cpp

    # 工具层
    utils/input.cpp
    utils/validator.cpp

    # 数据管理层（Manager）
    manager/patient_manager.cpp
    manager/doctor_manager.cpp
    manager/dept_manager.cpp
    manager/drug_manager.cpp
    manager/bed_manager.cpp
    manager/appointment_manager.cpp
    manager/medical_record_manager.cpp
    manager/prescription_manager.cpp
    manager/shortage_manager.cpp

    # 业务服务层（Service）
    service/registration.cpp
    service/consultation.cpp
    service/payment.cpp
    service/shortage_service.cpp
    service/hospitalization_service.cpp
    service/auth.cpp
    service/log.cpp
)

add_executable(HIS_cpp ${HIS_SOURCES})

# 允许以 #include "config/his_config.h" 这类项目根路径方式包含头文件
target_include_directories(HIS_cpp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# ---- 编译告警 ----
if(MSVC)
    target_compile_options(HIS_cpp PRIVATE /W4 /utf-8)
else()
    target_compile_options(HIS_cpp PRIVATE -Wall -Wextra)
endif()

# ---- 运行说明：程序使用相对路径读取 data/*.txt，需在项目根目录下运行可执行文件 ----
