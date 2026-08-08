#include "prescription_manager.h"

int PrescriptionManager::next_id = 1;
int PrescriptionItemManager::next_id = 1;

#include "../model/crud.h"
#include "../utils/input.h"
#include "../config/his_config.h"

// ==================== PrescriptionItemManager ====================

void PrescriptionItemManager::load() {
    ifstream in(filename);
    if (!in.is_open()) return;

    string line;
    while (getline(in, line)) {
        stringstream ss(line);
        string field;

        auto item = make_unique<PrescriptionItem>();
        getline(ss, item->id, '|');
        getline(ss, item->prescription_id, '|');
        getline(ss, item->drug_id, '|');
        getline(ss, field, '|'); item->quantity = stoi(field);
        getline(ss, item->usage, '|');
        getline(ss, field, '|'); item->amount = stoi(field);

        list.push_back(std::move(item));
    }
    in.close();

    for (auto& item : list) {
        if (item->id.length() > 2 && item->id[0] == 'P' && item->id[1] == 'I') {
            int num = stoi(item->id.substr(2));
            if (num >= next_id) next_id = num + 1;
        }
    }
}

void PrescriptionItemManager::save() {
    ofstream out(filename);
    if (!out.is_open()) return;

    for (auto& item : list) {
        out << item->id << '|'
            << item->prescription_id << '|'
            << item->drug_id << '|'
            << item->quantity << '|'
            << item->usage << '|'
            << item->amount << '\n';
    }
    out.close();
}

PrescriptionItem* PrescriptionItemManager::addItem(const string& prescription_id,
    const string& drug_id, int quantity, const string& usage, int amount) {
    auto item = make_unique<PrescriptionItem>();
    item->id = generateId();
    item->prescription_id = prescription_id;
    item->drug_id = drug_id;
    item->quantity = quantity;
    item->usage = usage;
    item->amount = amount;
    PrescriptionItem* raw = item.get();
    list.push_back(std::move(item));
    save();
    return raw;
}

void PrescriptionItemManager::listByPrescription(const string& prescription_id) {
    bool found = false;
    for (auto& item : list) {
        if (item->prescription_id == prescription_id) {
            cout << *item << endl;
            found = true;
        }
    }
    if (!found) cout << "该处方暂无明细。\n";
}

void PrescriptionItemManager::listAll() {
    if (list.empty()) {
        cout << "暂无处方明细记录。\n";
        return;
    }
    for (auto& item : list) {
        cout << *item << endl;
    }
}

// ==================== PrescriptionManager ====================

void PrescriptionManager::load() {
    // 加载处方主表
    ifstream in(filename);
    if (!in.is_open()) return;

    string line;
    while (getline(in, line)) {
        stringstream ss(line);
        string field;

        auto p = make_unique<Prescription>();
        getline(ss, p->id, '|');
        getline(ss, p->record_id, '|');
        getline(ss, p->patient_id, '|');
        getline(ss, p->doctor_id, '|');
        getline(ss, field, '|'); p->total_amount = stoi(field);
        getline(ss, p->status, '|');
        getline(ss, p->create_time, '|');

        list.push_back(std::move(p));
    }
    in.close();

    for (auto& p : list) {
        if (p->id.length() > 2 && p->id[0] == 'R' && p->id[1] == 'X') {
            int num = stoi(p->id.substr(2));
            if (num >= next_id) next_id = num + 1;
        }
    }

    // 同时加载明细
    itemMgr.load();
}

void PrescriptionManager::save() {
    // 保存处方主表
    ofstream out(filename);
    if (!out.is_open()) return;

    for (auto& p : list) {
        out << p->id << '|'
            << p->record_id << '|'
            << p->patient_id << '|'
            << p->doctor_id << '|'
            << p->total_amount << '|'
            << p->status << '|'
            << p->create_time << '\n';
    }
    out.close();

    // 同时保存明细
    itemMgr.save();
}

Prescription* PrescriptionManager::findPrescription(const string& id) {
    return findById(list, id);
}

void PrescriptionManager::listPrescription() {
    if (list.empty()) {
        cout << "暂无处方记录。\n";
        return;
    }
    for (auto& p : list) {
        cout << *p << endl;
    }
}

void PrescriptionManager::listByPatient(const string& patient_id) {
    bool found = false;
    for (auto& p : list) {
        if (p->patient_id == patient_id) {
            cout << *p << endl;
            found = true;
        }
    }
    if (!found) cout << "该患者暂无处方记录。\n";
}

void PrescriptionManager::listByDoctor(const string& doctor_id) {
    bool found = false;
    for (auto& p : list) {
        if (p->doctor_id == doctor_id) {
            cout << *p << endl;
            found = true;
        }
    }
    if (!found) cout << "该医生暂无处方记录。\n";
}

void PrescriptionManager::listByStatus(const string& status) {
    bool found = false;
    for (auto& p : list) {
        if (p->status == status) {
            cout << *p << endl;
            found = true;
        }
    }
    if (!found) cout << "暂无状态为「" << status << "」的处方记录。\n";
}

bool PrescriptionManager::updateStatus(const string& id, const string& new_status) {
    Prescription* p = findPrescription(id);
    if (!p) {
        cout << "未找到处方\n";
        return false;
    }
    p->status = new_status;
    save();
    return true;
}

Prescription* PrescriptionManager::addPrescription(const string& record_id,
    const string& patient_id, const string& doctor_id, const vector<ItemInput>& items) {
    auto p = make_unique<Prescription>();
    p->id = generateId();
    p->record_id = record_id;
    p->patient_id = patient_id;
    p->doctor_id = doctor_id;
    p->status = PrescriptionStatus::UNPAID;
    p->create_time = "2026-08-06";

    // 计算总金额
    p->total_amount = 0;
    for (auto& item : items) {
        p->total_amount += item.amount;
    }

    Prescription* raw = p.get();
    list.push_back(std::move(p));
    save();

    // 添加明细
    for (auto& item : items) {
        itemMgr.addItem(raw->id, item.drug_id, item.quantity, item.usage, item.amount);
    }

    return raw;
}

void PrescriptionManager::displayPrescription(const string& prescription_id, DrugManager& drugMgr) {
    Prescription* p = findPrescription(prescription_id);
    if (!p) {
        cout << "未找到处方\n";
        return;
    }
    cout << *p << endl;
    cout << "  处方明细：\n";
    for (auto& item : itemMgr.list) {
        if (item->prescription_id == prescription_id) {
            Drug* d = drugMgr.findDrug(item->drug_id);
            string drugName = d ? d->general_name : item->drug_id;
            cout << "    药品：" << drugName
                 << " | 数量：" << item->quantity
                 << " | 用法：" << item->usage
                 << " | 金额：" << item->amount << "分" << endl;
        }
    }
}
