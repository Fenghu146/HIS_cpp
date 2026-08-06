#pragma once

#include "data_manager.h"

class MedicalRecordManager : public DataManager<MedicalRecord> {
public:
    static int next_id;

    void load() override;
    void save() override;

    MedicalRecord* findRecord(const string& id);
    void listRecord();
    void listByPatient(const string& patient_id);
    void listByDoctor(const string& doctor_id);
    void listByAppointment(const string& appointment_id);
    MedicalRecord* addRecord(const string& appointment_id, const string& patient_id,
                              const string& doctor_id, const string& complaint,
                              const string& diagnosis, const string& orders,
                              bool need_hospitalize);

    MedicalRecordManager() : DataManager(FILE_RECORD){}

    string generateId() {
        return "MR" + to_string(next_id++);
    }
};
