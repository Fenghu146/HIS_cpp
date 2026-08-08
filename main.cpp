#include <iostream>
#include "manager/patient_manager.h"
#include "manager/doctor_manager.h"
#include "manager/dept_manager.h"
#include "manager/drug_manager.h"
#include "manager/bed_manager.h"
#include "manager/appointment_manager.h"
#include "manager/medical_record_manager.h"
#include "manager/prescription_manager.h"
#include "service/registration.h"
#include "service/consultation.h"
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

void showBedMenu(BedManager& bedMgr, DepartmentManager& deptMgr) {
    while (true) {
        cout << "\n=== 床位管理 ===\n";
        cout << "1. 添加床位\n";
        cout << "2. 查找床位\n";
        cout << "3. 删除床位\n";
        cout << "4. 床位列表\n";
        cout << "5. 修改床位状态\n";
        cout << "0. 返回\n";

        int choice = getValidChoice(0, 5);
        switch (choice) {
            case 1: bedMgr.registerBed(deptMgr); break;
            case 2: {
                cout << "请输入床位ID：";
                string id; inputLine(id);
                Bed* b = bedMgr.findBed(id);
                if (b) cout << *b << endl;
                else cout << "未找到床位\n";
                break;
            }
            case 3: {
                cout << "请输入床位ID：";
                string id; inputLine(id);
                if (bedMgr.deleteBed(id)) cout << "删除成功\n";
                else cout << "删除失败\n";
                break;
            }
            case 4: bedMgr.listBed(); break;
            case 5: {
                cout << "请输入床位ID：";
                string id; inputLine(id);
                Bed* b = bedMgr.findBed(id);
                if (!b) { cout << "未找到床位\n"; break; }
                cout << "当前状态：" << b->status << endl;
                cout << "  1. 空闲\n  2. 占用\n  3. 清洁中\n";
                cout << "请选择新状态：";
                int s = getValidChoice(1, 3);
                string status;
                switch (s) {
                    case 1: status = "空闲"; break;
                    case 2: status = "占用"; break;
                    case 3: status = "清洁中"; break;
                }
                if (bedMgr.changeStatus(id, status)) cout << "状态已更新为：" << status << endl;
                break;
            }
            case 0: return;
        }
    }
}

void showRegistrationMenu(PatientManager& patientMgr, DoctorManager& doctorMgr,
                          DepartmentManager& deptMgr, AppointmentManager& appointmentMgr) {
    while (true) {
        cout << "\n=== 挂号 ===\n";
        cout << "1. 新建挂号\n";
        cout << "2. 查看挂号单\n";
        cout << "3. 挂号列表\n";
        cout << "4. 待诊列表\n";
        cout << "0. 返回\n";

        int choice = getValidChoice(0, 4);
        switch (choice) {
            case 1:
                RegistrationService::registerPatient(patientMgr, doctorMgr, deptMgr, appointmentMgr);
                break;
            case 2: {
                cout << "请输入挂号单ID：";
                string id; inputLine(id);
                Appointment* a = appointmentMgr.findAppointment(id);
                if (a) cout << *a << endl;
                else cout << "未找到挂号单\n";
                break;
            }
            case 3: appointmentMgr.listAppointment(); break;
            case 4: appointmentMgr.listByStatus(AppointmentStatus::WAITING); break;
            case 0: return;
        }
    }
}

void showDoctorMenu(AppointmentManager& appointmentMgr,
                    MedicalRecordManager& recordMgr,
                    PrescriptionManager& prescriptionMgr,
                    DrugManager& drugMgr,
                    DepartmentManager& deptMgr) {
    // 输入医生ID
    cout << "请输入医生ID：";
    string doctorId;
    inputLine(doctorId);

    while (true) {
        cout << "\n=== 医生菜单（" << doctorId << "）===\n";
        cout << "1. 待诊患者列表\n";
        cout << "2. 接诊\n";
        cout << "3. 我的病历记录\n";
        cout << "4. 我的处方记录\n";
        cout << "0. 返回\n";

        int choice = getValidChoice(0, 4);
        switch (choice) {
            case 1:
                ConsultationService::showWaitingList(appointmentMgr, doctorId);
                break;
            case 2:
                ConsultationService::consultPatient(appointmentMgr, recordMgr,
                    prescriptionMgr, drugMgr, deptMgr, doctorId);
                break;
            case 3:
                recordMgr.listByDoctor(doctorId);
                break;
            case 4:
                prescriptionMgr.listByDoctor(doctorId);
                break;
            case 0: return;
        }
    }
}

void showPatientMenu(PatientManager& patientMgr,
                     MedicalRecordManager& recordMgr,
                     PrescriptionManager& prescriptionMgr,
                     AppointmentManager& appointmentMgr) {
    // 输入患者ID
    cout << "请输入患者ID：";
    string patientId;
    inputLine(patientId);
    Patient* p = patientMgr.findPatient(patientId);
    if (!p) {
        cout << "[错误] 未找到患者 " << patientId << endl;
        return;
    }
    cout << "欢迎，" << p->name << "！余额：" << p->balance << " 分\n";

    while (true) {
        cout << "\n=== 患者菜单（" << p->name << "）===\n";
        cout << "1. 我的病历\n";
        cout << "2. 我的处方\n";
        cout << "3. 充值\n";
        cout << "0. 返回\n";

        int choice = getValidChoice(0, 3);
        switch (choice) {
            case 1:
                recordMgr.listByPatient(patientId);
                break;
            case 2:
                prescriptionMgr.listByPatient(patientId);
                break;
            case 3: {
                cout << "请输入充值金额（分）：";
                long long amt; cin >> amt; ClearInputBuffer();
                patientMgr.recharge(patientId, amt);
                break;
            }
            case 0: return;
        }
    }
}

int main() {
    PatientManager patientMgr;
    DoctorManager doctorMgr;
    DepartmentManager deptMgr;
    DrugManager drugMgr;
    BedManager bedMgr;
    AppointmentManager appointmentMgr;
    MedicalRecordManager recordMgr;
    PrescriptionManager prescriptionMgr;
    patientMgr.load();
    doctorMgr.load();
    deptMgr.load();
    drugMgr.load();
    bedMgr.load();
    appointmentMgr.load();
    recordMgr.load();
    prescriptionMgr.load();

    cout << "=== 医院信息管理系统 ===\n";
    while (true) {
        cout << "\n=== 主菜单 ===\n";
        cout << "1. 患者管理\n";
        cout << "2. 医生管理\n";
        cout << "3. 科室管理\n";
        cout << "4. 药品管理\n";
        cout << "5. 床位管理\n";
        cout << "6. 挂号\n";
        cout << "7. 医生入口\n";
        cout << "8. 患者入口\n";
        cout << "0. 退出\n";

        int choice = getValidChoice(0, 8);
        switch (choice) {
            case 1: showPatientMenu(patientMgr); break;
            case 2: showDoctorMenu(doctorMgr); break;
            case 3: showDeptMenu(deptMgr, doctorMgr, drugMgr); break;
            case 4: showDrugMenu(drugMgr, deptMgr); break;
            case 5: showBedMenu(bedMgr, deptMgr); break;
            case 6: showRegistrationMenu(patientMgr, doctorMgr, deptMgr, appointmentMgr); break;
            case 7: showDoctorMenu(appointmentMgr, recordMgr, prescriptionMgr, drugMgr, deptMgr); break;
            case 8: showPatientMenu(patientMgr, recordMgr, prescriptionMgr, appointmentMgr); break;
            case 0: return 0;
        }
    }
}
