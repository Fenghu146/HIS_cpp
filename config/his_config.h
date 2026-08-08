#pragma once

//1、长度单位定义
constexpr int MAX_NAME_LEN = 50; //constexpr 该表达式、变量或函数在满足条件下可以在编译期间求值，编译器会在编译期间计算其值，并将其替换为常量值，从而提高程序的性能和效率。
constexpr int MAX_SPECIALTY_LEN = 100;
constexpr int MAX_ID_LEN = 20;
constexpr int MAX_PWD_LEN = 20;
constexpr int MAX_ACCOUNT_LEN = 50;

//2、ID生成前缀
constexpr char ID_PATIENT = 'P';
constexpr char ID_DOCTOR = 'D';
constexpr char ID_DEPT = 'K';
constexpr char ID_DRUG = 'M';
constexpr char ID_BED = 'B';
constexpr char ID_APPOINTMENT = 'A';
constexpr char ID_RECORD = 'R';
constexpr char ID_PRESCRIPTION = 'X';
constexpr char ID_PRESCRIPTION_ITEM = 'I';

//3、数据文件路径定义
constexpr const char* FILE_PATIENT = "data/patient.txt";
constexpr const char* FILE_DOCTOR = "data/doctor.txt";
constexpr const char* FILE_DEPT = "data/dept.txt";
constexpr const char* FILE_DRUG = "data/drug.txt";
constexpr const char* FILE_BED = "data/bed.txt";
constexpr const char* FILE_APPOINTMENT = "data/appointment.txt";
constexpr const char* FILE_RECORD = "data/record.txt";
constexpr const char* FILE_PRESCRIPTION = "data/prescription.txt";
constexpr const char* FILE_PRESCRIPTION_ITEM = "data/prescription_item.txt";

//4、业务默认值定义
constexpr int REGISTRATION_FEE = 1000;
constexpr int APPOINTMENT_FEE = 2000;
constexpr float DEFAULT_INSURANCE = 0.7f;
constexpr float DRUG_WARNING_RATIO = 0.2f;
constexpr int MAX_ID_RETRY = 10;
constexpr int BED_FEE_NORMAL = 5000;     // 普通床日费（分/天）
constexpr int BED_FEE_EMERGENCY = 8000;  // 急诊床日费
constexpr int BED_FEE_ICU = 15000;       // 重症床日费

//5、管理员配置
constexpr const char* ADMIN_USERNAME = "admin";
constexpr const char* ADMIN_PASSWORD = "123456";

//6、通用分隔符定义
constexpr const char* FILE_SEP = "|"; //定义一个编译期常量分隔符字符串，用于统一表示文本数据文件的字段分隔符
constexpr int MENU_LINE_LEN = 56;

//7、状态常量定义 —— 统一管理状态字符串，避免魔法字符串散落，编译期检查拼写
namespace AppointmentStatus {
    constexpr const char* WAITING    = "待诊";
    constexpr const char* IN_CONSULT = "已接诊";
    constexpr const char* COMPLETED  = "已完成";
    constexpr const char* CANCELLED  = "已取消";
}

namespace PrescriptionStatus {
    constexpr const char* UNPAID    = "未缴费";
    constexpr const char* PAID      = "已缴费";
    constexpr const char* DISPENSED = "已取药";
}

namespace BedStatus {
    constexpr const char* FREE     = "空闲";
    constexpr const char* OCCUPIED = "占用";
    constexpr const char* CLEANING = "清洁中";
}

namespace BedType {
    constexpr const char* NORMAL    = "普通";
    constexpr const char* EMERGENCY = "急诊";
    constexpr const char* ICU       = "重症";
}
