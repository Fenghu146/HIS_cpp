#pragma once

//1、长度单位定义
constexpr int MAX_NAME_LEN = 50;
constexpr int MAX_SPECIALTY_LEN = 100;
constexpr int MAX_ID_LEN = 20;
constexpr int MAX_PWD_LEN = 20;
constexpr int MAX_ACCOUNT_LEN = 50;

//2、ID生成前缀
constexpr char ID_PATIENT = 'P';
constexpr char ID_DOCTOR = 'D';
constexpr char ID_DEPT = 'K';
constexpr char ID_DRUG = 'M';

//3、数据文件路径定义
constexpr const char* FILE_PATIENT = "data/patient.txt";
constexpr const char* FILE_DOCTOR = "data/doctor.txt";
constexpr const char* FILE_DEPT = "data/dept.txt";
constexpr const char* FILE_DRUG = "data/drug.txt";

//4、业务默认值定义
constexpr int REGISTRATION_FEE = 1000;
constexpr int APPOINTMENT_FEE = 2000;
constexpr float DEFAULT_INSURANCE = 0.7f;
constexpr float DRUG_WARNING_RATIO = 0.2f;
constexpr int MAX_ID_RETRY = 10;

//5、管理员配置
constexpr const char* ADMIN_USERNAME = "admin";
constexpr const char* ADMIN_PASSWORD = "123456";

//6、通用分隔符定义
constexpr const char* FILE_SEP = "|";
constexpr int MENU_LINE_LEN = 56;
