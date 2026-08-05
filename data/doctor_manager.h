#pragma once

#include "data_manager.h"

class DoctorManager : public DataManager<Doctor> {
public:
    static int next_id;

    void load() override;
    void save() override;

    void registerDoctor();
    Doctor* findDoctor(const string& id);
    bool deleteDoctor(const string& id);
    void listDoctor();
    int countDoctorsInDept(const string& dept_name) const;

    DoctorManager() : DataManager(FILE_DOCTOR){}

    // 生成唯一ID: 前缀+自增序号，例 D1, D2, D3...
    string generateId() {
        return string(1, ID_DOCTOR) + to_string(next_id++);
    }
};
