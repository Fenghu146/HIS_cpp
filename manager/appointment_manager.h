#pragma once

#include "data_manager.h"
#include "../model/entity.h"

class AppointmentManager : public DataManager<Appointment> {
public:
    static int next_id;

    void load() override;  // 重写标志
    void save() override;

    Appointment* findAppointment(const string& id);
    void listAppointment();
    void listByDoctor(const string& doctor_id);
    void listByPatient(const string& patient_id);
    void listByStatus(const string& status);
    bool updateStatus(const string& id, const string& new_status);
    Appointment* addAppointment(const string& patient_id, const string& doctor_id,
                                const string& dept_id, int fee);

    AppointmentManager() : DataManager(FILE_APPOINTMENT) {}

    // 生成挂号单 ID：前缀 + 自增序号，例 A1, A2, A3...
    string generateId() { return string(1, ID_APPOINTMENT) + to_string(next_id++); }
};
