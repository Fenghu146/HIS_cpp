#pragma once

#include "data_manager.h"
#include "dept_manager.h"
#include "../model/entity.h"

class BedManager : public DataManager<Bed> {
public:
    static int next_id;

    void load() override;
    void save() override;

    void registerBed(DepartmentManager& deptMgr);
    Bed* findBed(const string& id);
    Bed* findBedByNumber(const string& bed_number);
    bool deleteBed(const string& id);
    void listBed();
    void listBedByDept(const string& dept_id);
    bool changeStatus(const string& id, const string& new_status);
    bool occupyBed(const string& id, const string& patient_id);
    bool releaseBed(const string& id);
    int countBedsByDept(const string& dept_id) const;
    int countAvailableBedsByDept(const string& dept_id) const;

    BedManager() : DataManager(FILE_BED){}

    string generateId() {
        return string(1, ID_BED) + to_string(next_id++);
    }
};
