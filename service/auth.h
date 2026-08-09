#pragma once

#include "../model/entity.h"
#include "../manager/patient_manager.h"
#include "../manager/doctor_manager.h"

// 认证服务：患者/医生/管理员登录验证
class AuthService {
public:
    // 患者登录：验证 ID + pin，内部处理哈希迁移
    static bool loginPatient(PatientManager& mgr, const string& id, const string& pin);

    // 医生登录：验证 account + password，内部处理哈希迁移
    static bool loginDoctor(DoctorManager& mgr, const string& account, const string& password);

    // 管理员登录：验证 admin 用户名密码
    static bool loginAdmin(const string& username, const string& password);
};
