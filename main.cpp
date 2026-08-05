#include <iostream>
#include "data/patient_manager.h"
#include "data/doctor_manager.h"
#include "data/dept_manager.h"
#include "data/drug_manager.h"
#include "utils/input.h"
using namespace std;

void showPatientMenu(PatientManager& mgr) {
    while (true) {
        cout << "\n=== 患者管理 ===\n";
        cout << "1. 注册患者\n";
        cout << "2. 查找患者\n";
        cout << "3. 删除患者\n";
        cout << "4. 患者列表\n";
        cout << "5. 充值\n";
        cout << "0. 返回\n";

        int choice = getValidChoice(0, 5);
        switch (choice) {
            case 1: mgr.registerPatient(); break;
            case 2: {
                cout << "请输入患者ID:";
                string id; inputLine(id);
                Patient* p = mgr.findPatient(id);
                if (p) cout << *p << endl;
                else cout << "未找到患者\n";
                break;
            }
            case 3: {
                cout << "请输入患者ID:";
                string id; inputLine(id);
                if (mgr.deletePatient(id)) cout << "删除成功\n";
                else cout << "未找到患者\n";
                break;
            }
            case 4: mgr.listPatient(); break;
            case 5: {
                cout << "请输入患者ID:";
                string id; inputLine(id);
                cout << "请输入充值金额（分）:";
                long long amt; cin >> amt; ClearInputBuffer();
                mgr.recharge(id, amt);
                break;
            }
            case 0: return;
        }
    }
}

void showDoctorMenu(DoctorManager& mgr) {
    while (true) {
        cout << "\n=== 医生管理 ===\n";
        cout << "1. 注册医生\n";
        cout << "2. 查找医生\n";
        cout << "3. 删除医生\n";
        cout << "4. 医生列表\n";
        cout << "0. 返回\n";

        int choice = getValidChoice(0, 4);
        switch (choice) {
            case 1: mgr.registerDoctor(); break;
            case 2: {
                cout << "请输入医生ID:";
                string id; inputLine(id);
                Doctor* d = mgr.findDoctor(id);
                if (d) cout << *d << endl;
                else cout << "未找到医生\n";
                break;
            }
            case 3: {
                cout << "请输入医生ID:";
                string id; inputLine(id);
                if (mgr.deleteDoctor(id)) cout << "删除成功\n";
                else cout << "未找到医生\n";
                break;
            }
            case 4: mgr.listDoctor(); break;
            case 0: return;
        }
    }
}

void showDeptMenu(DepartmentManager& deptMgr, DoctorManager& docMgr, DrugManager& drugMgr) {
    while (true) {
        cout << "\n=== 科室管理 ===\n";
        cout << "1. 注册科室\n";
        cout << "2. 查找科室\n";
        cout << "3. 删除科室\n";
        cout << "4. 科室列表\n";
        cout << "0. 返回\n";

        int choice = getValidChoice(0, 4);
        switch (choice) {
            case 1: deptMgr.registerDepartment(); break;
            case 2: {
                cout << "请输入科室ID:";
                string id; inputLine(id);
                Department* d = deptMgr.findDepartment(id);
                if (d) cout << *d << endl;
                else cout << "未找到科室\n";
                break;
            }
            case 3: {
                cout << "请输入科室ID:";
                string id; inputLine(id);
                if (deptMgr.deleteDepartment(id, docMgr, drugMgr)) cout << "删除成功\n";
                else cout << "删除失败\n";
                break;
            }
            case 4: deptMgr.listDepartment(docMgr); break;
            case 0: return;
        }
    }
}

void showDrugMenu(DrugManager& drugMgr, DepartmentManager& deptMgr) {
    while (true) {
        cout << "\n=== 药品管理 ===\n";
        cout << "1. 注册药品\n";
        cout << "2. 查找药品\n";
        cout << "3. 删除药品\n";
        cout << "4. 药品列表\n";
        cout << "5. 入库\n";
        cout << "6. 出库\n";
        cout << "7. 库存预警\n";
        cout << "8. 修改药品信息\n";
        cout << "0. 返回\n";

        int choice = getValidChoice(0, 8);
        switch (choice) {
            case 1: drugMgr.registerDrug(deptMgr); break;
            case 2: {
                cout << "请输入药品ID:";
                string id; inputLine(id);
                Drug* d = drugMgr.findDrug(id);
                if (d) DrugManager::displayDrug(*d, deptMgr);
                else cout << "未找到药品\n";
                break;
            }
            case 3: {
                cout << "请输入药品ID:";
                string id; inputLine(id);
                if (drugMgr.deleteDrug(id)) cout << "删除成功\n";
                else cout << "删除失败\n";
                break;
            }
            case 4: drugMgr.listDrug(deptMgr); break;
            case 5: {
                cout << "请输入药品ID:";
                string id; inputLine(id);
                cout << "请输入入库数量：";
                string amt; inputLine(amt);
                drugMgr.stockIn(id, stoi(amt));
                break;
            }
            case 6: {
                cout << "请输入药品ID:";
                string id; inputLine(id);
                cout << "请输入出库数量：";
                string amt; inputLine(amt);
                drugMgr.stockOut(id, stoi(amt));
                break;
            }
            case 7: drugMgr.warningList(deptMgr); break;
            case 8: drugMgr.modifyDrug(deptMgr); break;
            case 0: return;
        }
    }
}

int main() {
    PatientManager patientMgr;
    DoctorManager doctorMgr;
    DepartmentManager deptMgr;
    DrugManager drugMgr;
    patientMgr.load();
    doctorMgr.load();
    deptMgr.load();
    drugMgr.load();

    cout << "=== 医院信息管理系统 ===\n";
    while (true) {
        cout << "\n=== 主菜单 ===\n";
        cout << "1. 患者管理\n";
        cout << "2. 医生管理\n";
        cout << "3. 科室管理\n";
        cout << "4. 药品管理\n";
        cout << "0. 退出\n";

        int choice = getValidChoice(0, 4);
        switch (choice) {
            case 1: showPatientMenu(patientMgr); break;
            case 2: showDoctorMenu(doctorMgr); break;
            case 3: showDeptMenu(deptMgr, doctorMgr, drugMgr); break;
            case 4: showDrugMenu(drugMgr, deptMgr); break;
            case 0: return 0;
        }
    }
}
