#include "appointment_manager.h"

int AppointmentManager::next_id = 1;

#include "../model/crud.h"
#include "../utils/input.h"
#include "../config/his_config.h"

void AppointmentManager::load() {
    ifstream in(filename);
    if (!in.is_open()) return;

    string line;
    while (getline(in, line)) {
        stringstream ss(line);
        string field;

        auto a = make_unique<Appointment>();
        getline(ss, a->id, '|');
        getline(ss, a->patient_id, '|');
        getline(ss, a->doctor_id, '|');
        getline(ss, a->dept_id, '|');
        getline(ss, field, '|'); a->fee = stoi(field);
        getline(ss, a->status, '|');
        getline(ss, a->create_time, '|');

        list.push_back(std::move(a));
    }
    in.close();

    for (auto& a : list) {
        if (a->id.length() > 1 && a->id[0] == ID_APPOINTMENT) {
            int num = stoi(a->id.substr(1));
            if (num >= next_id) next_id = num + 1;
        }
    }
}

void AppointmentManager::save() {
    ofstream out(filename);
    if (!out.is_open()) return;

    for (auto& a : list) {
        out << a->id << '|'
            << a->patient_id << '|'
            << a->doctor_id << '|'
            << a->dept_id << '|'
            << a->fee << '|'
            << a->status << '|'
            << a->create_time << '\n';
    }
    out.close();
}

Appointment* AppointmentManager::findAppointment(const string& id) {
    return findById(list, id);
}

void AppointmentManager::listAppointment() {
    if (list.empty()) {
        cout << "暂无挂号记录。\n";
        return;
    }
    for (auto& a : list) {
        cout << *a << endl;
    }
}

void AppointmentManager::listByDoctor(const string& doctor_id) {
    bool found = false;
    for (auto& a : list) {
        if (a->doctor_id == doctor_id) {
            cout << *a << endl;
            found = true;
        }
    }
    if (!found) cout << "该医生暂无挂号记录。\n";
}

void AppointmentManager::listByPatient(const string& patient_id) {
    bool found = false;
    for (auto& a : list) {
        if (a->patient_id == patient_id) {
            cout << *a << endl;
            found = true;
        }
    }
    if (!found) cout << "该患者暂无挂号记录。\n";
}

void AppointmentManager::listByStatus(const string& status) {
    bool found = false;
    for (auto& a : list) {
        if (a->status == status) {
            cout << *a << endl;
            found = true;
        }
    }
    if (!found) cout << "暂无状态为「" << status << "」的挂号记录。\n";
}

bool AppointmentManager::updateStatus(const string& id, const string& new_status) {
    Appointment* a = findAppointment(id);
    if (!a) {
        cout << "未找到挂号单\n";
        return false;
    }
    a->status = new_status;
    save();
    return true;
}

Appointment* AppointmentManager::addAppointment(const string& patient_id,
    const string& doctor_id, const string& dept_id, int fee) {
    auto a = make_unique<Appointment>();
    a->id = generateId();
    a->patient_id = patient_id;
    a->doctor_id = doctor_id;
    a->dept_id = dept_id;
    a->fee = fee;
    a->status = AppointmentStatus::WAITING;
    a->create_time = "2026-08-06";  // 简化处理
    Appointment* raw = a.get();
    list.push_back(std::move(a));
    save();
    return raw;
}
