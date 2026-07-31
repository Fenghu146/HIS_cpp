#pragma once

//1、长度单位定义
#define MAX_NAME_LEN = 50
#define MAX_SPECIALTY = 100
#define MAX_ID_LEN = 20
#define MAX_PWD_LEN = 20
#define MAX_ACCOUNT_LEN = 50

//2、ID生成前缀
#define ID_PATIENT = 'P'
#define ID_DOCTOR = 'D'
#define ID_DEPT = 'K'
#define ID_DRUG = 'M'

//3、数据文件路径定义
#define FILE_PATIENT        "data/patient.txt"      // 患者数据文件
#define FILE_DOCTOR         "data/doctor.txt"       // 医生数据文件
#define FILE_DEPT           "data/dept.txt"         // 科室数据文件
#define FILE_DRUG           "data/drug.txt"         // 药品数据文件

//4、业务默认值定义
#define REGISTRATION_FEE    1000    // 普通挂号费（分）
#define APPOINTMENT_FEE     2000    // 预约挂号费（分）
#define DEFAULT_INSURANCE   0.7f    // 默认医保报销比例 (70%)
#define DRUG_WARNING_RATIO  0.2f    // 库存预警系数 (低于阈值20%预警)
#define MAX_ID_RETRY        10      // ID 生成最大重试次数

//5、管理员配置
#define ADMIN_USERNAME      "admin"       // 管理员账号
#define ADMIN_PASSWORD      "123456"      // 管理员密码

//6、通用分隔符定义
#define FILE_SEP            "|"     // 文件字段分隔符
#define MENU_LINE_LEN       56      // 菜单分隔线长度

