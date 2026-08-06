#pragma once

#include "data_manager.h"
#include "drug_manager.h"

// 处方明细管理器（内部使用，不独立实例化）
class PrescriptionItemManager : public DataManager<PrescriptionItem> {
public:
    static int next_id;

    void load() override;
    void save() override;

    PrescriptionItem* addItem(const string& prescription_id, const string& drug_id,
                               int quantity, const string& usage, int amount);
    void listByPrescription(const string& prescription_id);
    void listAll();

    PrescriptionItemManager() : DataManager(FILE_PRESCRIPTION_ITEM){}

    string generateId() {
        return "PI" + to_string(next_id++);
    }
};

// 处方管理器
class PrescriptionManager : public DataManager<Prescription> {
public:
    static int next_id;
    PrescriptionItemManager itemMgr;  // 内部管理明细

    void load() override;
    void save() override;

    Prescription* findPrescription(const string& id);
    void listPrescription();
    void listByPatient(const string& patient_id);
    void listByDoctor(const string& doctor_id);
    void listByStatus(const string& status);
    bool updateStatus(const string& id, const string& new_status);

    // 创建处方（含明细）
    struct ItemInput {
        string drug_id;
        int quantity;
        string usage;
        int amount;
    };
    Prescription* addPrescription(const string& record_id, const string& patient_id,
                                   const string& doctor_id, const vector<ItemInput>& items);

    // 显示处方（含明细）
    void displayPrescription(const string& prescription_id, DrugManager& drugMgr);

    PrescriptionManager() : DataManager(FILE_PRESCRIPTION){}

    string generateId() {
        return "RX" + to_string(next_id++);
    }
};
