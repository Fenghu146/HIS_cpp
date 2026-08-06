#pragma once

#include "data_manager.h"
#include "doctor_manager.h"

class DrugManager;

class DepartmentManager : public DataManager<Department> {
public:
    static int next_id;

    void load() override;
    void save() override;

    void registerDepartment();
    Department* findDepartment(const string& id);
    bool deleteDepartment(const string& id, DoctorManager& docMgr, DrugManager& drugMgr);
    void listDepartment(DoctorManager& docMgr);
    bool exists(const string& id) const;
    string getNameById(const string& id) const;
    bool hasDrugAssociation(const string& dept_id, DrugManager& drugMgr);

    DepartmentManager() : DataManager(FILE_DEPT){}

    string generateId() {
        return string(1, ID_DEPT) + to_string(next_id++);
    }
};
