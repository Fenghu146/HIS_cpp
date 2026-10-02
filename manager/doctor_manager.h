#pragma once

#include <functional>
#include "data_manager.h"

class DoctorManager : public DataManager<Doctor> {
public:
    static int next_id;

    void load() override;
    void save() override;

    // deptExists：科室存在性校验回调（可选，未注入时跳过引用校验）
    void registerDoctor(const std::function<bool(const std::string&)>& deptExists = nullptr);
    Doctor* findDoctor(const string& id);
    Doctor* findByAccount(const string& account);
    bool deleteDoctor(const string& id);
    void listDoctor();
    int countDoctorsInDept(const string& dept_name) const;

    // 验证密码（内部处理哈希迁移）
    bool validatePassword(const string& account, const string& password);
    // 修改密码
    bool changePassword(const string& id, const string& old_pwd, const string& new_pwd);

    DoctorManager() : DataManager(FILE_DOCTOR) {}

    // 生成唯一ID: 前缀+自增序号，例 D1, D2, D3...
    string generateId() { return string(1, ID_DOCTOR) + to_string(next_id++); }
};
