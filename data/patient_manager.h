#pragma once

#include "data_manager.h"

class PatientManager : public DataManager<Patient> {
public:
    static int next_id;

    void load() override;
    void save() override;

    void registerPatient();
    Patient* findPatient(const string& id);
    bool deletePatient(const string& id);
    void listPatient();
    bool recharge(const string& id,long long amount);

    PatientManager() : DataManager(FILE_PATIENT){}

    // 生成唯一ID: 前缀+自增序号，例 P1, P2, P3...
    string generateId() {
        return string(1,ID_PATIENT) + to_string(next_id++);
    }
};