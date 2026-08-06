#include "medical_record_manager.h"

int MedicalRecordManager::next_id = 1;

#include "../model/crud.h"
#include "../utils/input.h"
#include "../config/his_config.h"

void MedicalRecordManager::load() {
    ifstream in(filename);
    if (!in.is_open()) return;

    string line;
    while (getline(in, line)) {
        stringstream ss(line);
        string field;

        auto r = make_unique<MedicalRecord>();
        getline(ss, r->id, '|');
        getline(ss, r->appointment_id, '|');
        getline(ss, r->patient_id, '|');
        getline(ss, r->doctor_id, '|');
        getline(ss, r->complaint, '|');
        getline(ss, r->diagnosis, '|');
        getline(ss, r->orders, '|');
        getline(ss, r->create_time, '|');
        getline(ss, field, '|'); r->need_hospitalize = (field == "1");

        list.push_back(std::move(r));
    }
    in.close();

    for (auto& r : list) {
        if (r->id.length() > 2 && r->id[0] == 'M' && r->id[1] == 'R') {
            int num = stoi(r->id.substr(2));
            if (num >= next_id) next_id = num + 1;
        }
    }
}

void MedicalRecordManager::save() {
    ofstream out(filename);
    if (!out.is_open()) return;

    for (auto& r : list) {
        out << r->id << '|'
            << r->appointment_id << '|'
            << r->patient_id << '|'
            << r->doctor_id << '|'
            << r->complaint << '|'
            << r->diagnosis << '|'
            << r->orders << '|'
            << r->create_time << '|'
            << (r->need_hospitalize ? "1" : "0") << '\n';
    }
    out.close();
}

MedicalRecord* MedicalRecordManager::findRecord(const string& id) {
    return findById(list, id);
}

void MedicalRecordManager::listRecord() {
    if (list.empty()) {
        cout << "暂无病历记录。\n";
        return;
    }
    for (auto& r : list) {
        cout << *r << endl;
    }
}

void MedicalRecordManager::listByPatient(const string& patient_id) {
    bool found = false;
    for (auto& r : list) {
        if (r->patient_id == patient_id) {
            cout << *r << endl;
            found = true;
        }
    }
    if (!found) cout << "该患者暂无病历记录。\n";
}

void MedicalRecordManager::listByDoctor(const string& doctor_id) {
    bool found = false;
    for (auto& r : list) {
        if (r->doctor_id == doctor_id) {
            cout << *r << endl;
            found = true;
        }
    }
    if (!found) cout << "该医生暂无病历记录。\n";
}

void MedicalRecordManager::listByAppointment(const string& appointment_id) {
    bool found = false;
    for (auto& r : list) {
        if (r->appointment_id == appointment_id) {
            cout << *r << endl;
            found = true;
        }
    }
    if (!found) cout << "该挂号单暂无病历记录。\n";
}

MedicalRecord* MedicalRecordManager::addRecord(const string& appointment_id,
    const string& patient_id, const string& doctor_id, const string& complaint,
    const string& diagnosis, const string& orders, bool need_hospitalize) {
    auto r = make_unique<MedicalRecord>();
    r->id = generateId();
    r->appointment_id = appointment_id;
    r->patient_id = patient_id;
    r->doctor_id = doctor_id;
    r->complaint = complaint;
    r->diagnosis = diagnosis;
    r->orders = orders;
    r->create_time = "2026-08-06";
    r->need_hospitalize = need_hospitalize;
    MedicalRecord* raw = r.get();
    list.push_back(std::move(r));
    save();
    return raw;
}
