#pragma once

#include "data_manager.h"
#include "dept_manager.h"
#include "../config/his_config.h"

class DrugManager : public DataManager<Drug> {
public:
    static int next_id;

    void load() override;
    void save() override;

    void registerDrug(DepartmentManager& deptMgr);
    Drug* findDrug(const string& id);
    bool deleteDrug(const string& id);
    void listDrug(DepartmentManager& deptMgr);
    bool stockIn(const string& id, int amount);
    bool stockOut(const string& id, int amount);
    void warningList(DepartmentManager& deptMgr);
    void modifyDrug(DepartmentManager& deptMgr);

    DrugManager() : DataManager(FILE_DRUG){}

    string generateId() {
        return string(1, ID_DRUG) + to_string(next_id++);
    }

    static int calcWarningStock(int max_stock) {
        if (max_stock <= 0) return 20;
        return static_cast<int>(max_stock * DRUG_WARNING_RATIO);
    }

private:
    // 打印编号科室列表供选择
    static void printDeptList(const DepartmentManager& deptMgr);
    // 根据序号获取科室ID
    static string getDeptIdByIndex(const DepartmentManager& deptMgr, int index);
    // 修改科室关联子接口
    static void modifyDeptIds(Drug& d, DepartmentManager& deptMgr);

public:
    // 显示单个药品（带科室名）
    static void displayDrug(const Drug& d, const DepartmentManager& deptMgr);
};
