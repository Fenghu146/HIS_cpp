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
#include "service/payment.h"
#include "service/shortage_service.h"
#include "service/hospitalization_service.h"
#include "manager/shortage_manager.h"
#include "service/session.h"
#include "service/auth.h"
#include "service/log.h"
#include "utils/input.h"
#include "utils/validator.h"
#include <filesystem>
using namespace std;

// ==================== 前向声明 ====================

void showPatientMgmtMenu(PatientManager& mgr);
void showDoctorMgmtMenu(DoctorManager& mgr);
void showDeptMgmtMenu(DepartmentManager& deptMgr, DoctorManager& docMgr, DrugManager& drugMgr);
void showDrugMgmtMenu(DrugManager& drugMgr, DepartmentManager& deptMgr);
void showBedMgmtMenu(BedManager& bedMgr, DepartmentManager& deptMgr);
void showPharmacyMenu(DrugManager& drugMgr, DepartmentManager& deptMgr, ShortageManager& shortageMgr);
void showPatientMenu(PatientManager& patientMgr, MedicalRecordManager& recordMgr,
                     PrescriptionManager& prescriptionMgr, DrugManager& drugMgr,
                     ShortageManager& shortageMgr, AppointmentManager& appointmentMgr,
                     DoctorManager& doctorMgr, DepartmentManager& deptMgr,
                     BedManager& bedMgr);
void showDoctorMenu(AppointmentManager& appointmentMgr, MedicalRecordManager& recordMgr,
                     PrescriptionManager& prescriptionMgr, DrugManager& drugMgr,
                     DepartmentManager& deptMgr, ShortageManager& shortageMgr,
                     DoctorManager& doctorMgr, BedManager& bedMgr);
void patientLoginFlow(PatientManager& patientMgr, MedicalRecordManager& recordMgr,
                      PrescriptionManager& prescriptionMgr, DrugManager& drugMgr,
                      ShortageManager& shortageMgr, AppointmentManager& appointmentMgr,
                      DoctorManager& doctorMgr, DepartmentManager& deptMgr,
                      BedManager& bedMgr);
void doctorLoginFlow(DoctorManager& doctorMgr, AppointmentManager& appointmentMgr,
                     MedicalRecordManager& recordMgr, PrescriptionManager& prescriptionMgr,
                     DrugManager& drugMgr, DepartmentManager& deptMgr,
                     ShortageManager& shortageMgr);
void adminLoginFlow(PatientManager& patientMgr, DoctorManager& doctorMgr,
                    DepartmentManager& deptMgr, DrugManager& drugMgr,
                    BedManager& bedMgr, ShortageManager& shortageMgr);

// ==================== 管理员菜单（整合现有管理功能）====================

void showAdminMenu(PatientManager& patientMgr, DoctorManager& doctorMgr,
                   DepartmentManager& deptMgr, DrugManager& drugMgr,
                   BedManager& bedMgr, ShortageManager& shortageMgr) {
    while (true) {
        cout << "\n=== 管理员菜单 ===\n";
        cout << "1. 患者管理\n";
        cout << "2. 医生管理\n";
        cout << "3. 科室管理\n";
        cout << "4. 药品管理\n";
        cout << "5. 床位管理\n";
        cout << "6. 药房管理\n";
        cout << "7. 住院管理\n";
        cout << "0. 退出登录\n";

        int choice = getValidChoice(0, 7);
        switch (choice) {
            case 1: showPatientMgmtMenu(patientMgr); break;
            case 2: showDoctorMgmtMenu(doctorMgr); break;
            case 3: showDeptMgmtMenu(deptMgr, doctorMgr, drugMgr); break;
            case 4: showDrugMgmtMenu(drugMgr, deptMgr); break;
            case 5: showBedMgmtMenu(bedMgr, deptMgr); break;
            case 6: showPharmacyMenu(drugMgr, deptMgr, shortageMgr); break;
            case 7: HospitalizationService::listHospitalized(bedMgr); break;
            case 0:
                LogService::logLogout("admin", Session::getInstance()->getUserId());
                Session::getInstance()->logout();
                return;
        }
    }
}

// ==================== 患者管理子菜单 ====================

void showPatientMgmtMenu(PatientManager& mgr) {
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
                string amtStr; inputLine(amtStr);
                long long amt = 0;
                if (!parseLongLong(amtStr, amt)) {
                    cout << "[错误] 充值金额必须为非负整数！\n";
                    break;
                }
                mgr.recharge(id, amt);
                break;
            }
            case 0: return;
        }
    }
}

// ==================== 医生管理子菜单 ====================

void showDoctorMgmtMenu(DoctorManager& mgr) {
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

// ==================== 科室管理子菜单 ====================

void showDeptMgmtMenu(DepartmentManager& deptMgr, DoctorManager& docMgr, DrugManager& drugMgr) {
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

// ==================== 药品管理子菜单 ====================

void showDrugMgmtMenu(DrugManager& drugMgr, DepartmentManager& deptMgr) {
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
                int qty = 0;
                if (!parseInt(amt, qty)) { cout << "[错误] 数量必须为非负整数！\n"; break; }
                drugMgr.stockIn(id, qty);
                break;
            }
            case 6: {
                cout << "请输入药品ID:";
                string id; inputLine(id);
                cout << "请输入出库数量：";
                string amt; inputLine(amt);
                int qty = 0;
                if (!parseInt(amt, qty)) { cout << "[错误] 数量必须为非负整数！\n"; break; }
                drugMgr.stockOut(id, qty);
                break;
            }
            case 7: drugMgr.warningList(deptMgr); break;
            case 8: drugMgr.modifyDrug(deptMgr); break;
            case 0: return;
        }
    }
}

// ==================== 床位管理子菜单 ====================

void showBedMgmtMenu(BedManager& bedMgr, DepartmentManager& deptMgr) {
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

// ==================== 药房管理子菜单 ====================

void showPharmacyMenu(DrugManager& drugMgr, DepartmentManager& deptMgr, ShortageManager& shortageMgr) {
    while (true) {
        cout << "\n=== 药房管理 ===\n";
        cout << "1. 药品入库\n";
        cout << "2. 药品出库\n";
        cout << "3. 库存预警\n";
        cout << "4. 缺药待办清单\n";
        cout << "5. 处理缺药\n";
        cout << "0. 返回\n";

        int choice = getValidChoice(0, 5);
        switch (choice) {
            case 1: {
                cout << "请输入药品ID：";
                string id; inputLine(id);
                cout << "请输入入库数量：";
                string amt; inputLine(amt);
                int qty = 0;
                if (!parseInt(amt, qty)) { cout << "[错误] 数量必须为非负整数！\n"; break; }
                drugMgr.stockIn(id, qty);
                break;
            }
            case 2: {
                cout << "请输入药品ID：";
                string id; inputLine(id);
                cout << "请输入出库数量：";
                string amt; inputLine(amt);
                int qty = 0;
                if (!parseInt(amt, qty)) { cout << "[错误] 数量必须为非负整数！\n"; break; }
                drugMgr.stockOut(id, qty);
                break;
            }
            case 3: drugMgr.warningList(deptMgr); break;
            case 4: ShortageService::viewPendingShortages(shortageMgr); break;
            case 5: ShortageService::fulfillShortage(shortageMgr, drugMgr); break;
            case 0: return;
        }
    }
}

// ==================== 患者入口菜单 ====================

void showPatientMenu(PatientManager& patientMgr,
                     MedicalRecordManager& recordMgr,
                     PrescriptionManager& prescriptionMgr,
                     DrugManager& drugMgr,
                     ShortageManager& shortageMgr,
                     AppointmentManager& appointmentMgr,
                     DoctorManager& doctorMgr,
                     DepartmentManager& deptMgr,
                     BedManager& bedMgr) {
    string patientId = Session::getInstance()->getUserId();
    Patient* p = patientMgr.findPatient(patientId);
    if (!p) {
        cout << "[错误] 未找到患者 " << patientId << endl;
        return;
    }
    cout << "欢迎，" << p->name << "！余额：" << p->balance << " 分\n";

    while (true) {
        cout << "\n=== 患者菜单（" << p->name << "）===\n";
        cout << "1. 挂号\n";
        cout << "2. 我的病历\n";
        cout << "3. 我的处方\n";
        cout << "4. 充值\n";
        cout << "5. 缴费\n";
        cout << "6. 取药\n";
        cout << "7. 修改密码\n";
        cout << "8. 办理住院\n";
        cout << "0. 退出登录\n";

        int choice = getValidChoice(0, 8);
        switch (choice) {
            case 1:
                RegistrationService::registerPatient(patientMgr, doctorMgr, deptMgr, appointmentMgr);
                break;
            case 2:
                recordMgr.listByPatient(patientId);
                break;
            case 3:
                prescriptionMgr.listByPatient(patientId);
                break;
            case 4: {
                cout << "请输入充值金额（分）：";
                string amtStr; inputLine(amtStr);
                long long amt = 0;
                if (!parseLongLong(amtStr, amt)) {
                    cout << "[错误] 充值金额必须为非负整数！\n";
                    break;
                }
                patientMgr.recharge(patientId, amt);
                break;
            }
            case 5:
                PaymentService::payPrescription(patientMgr, prescriptionMgr, drugMgr, patientId);
                break;
            case 6:
                PaymentService::dispensePrescription(prescriptionMgr, drugMgr, shortageMgr, patientId);
                break;
            case 7: {
                cout << "请输入原密码：";
                string oldPin; inputLine(oldPin);
                cout << "请输入新密码：";
                string newPin; inputLine(newPin);
                patientMgr.changePin(patientId, oldPin, newPin);
                break;
            }
            case 8:
                HospitalizationService::admitPatient(patientMgr, bedMgr, recordMgr, doctorMgr, patientId);
                break;
            case 0:
                LogService::logLogout("patient", patientId);
                Session::getInstance()->logout();
                return;
        }
    }
}

// ==================== 医生入口菜单 ====================

void showDoctorMenu(AppointmentManager& appointmentMgr,
                    MedicalRecordManager& recordMgr,
                    PrescriptionManager& prescriptionMgr,
                    DrugManager& drugMgr,
                    DepartmentManager& deptMgr,
                    ShortageManager& shortageMgr,
                    DoctorManager& doctorMgr,
                    BedManager& /*bedMgr*/) {  // 预留：医生菜单暂不使用床位管理
    string doctorId = Session::getInstance()->getUserId();

    while (true) {
        cout << "\n=== 医生菜单（" << Session::getInstance()->getUserName() << "）===\n";
        cout << "1. 待诊患者列表\n";
        cout << "2. 接诊\n";
        cout << "3. 我的病历记录\n";
        cout << "4. 我的处方记录\n";
        cout << "5. 缺药登记\n";
        cout << "6. 修改密码\n";
        cout << "0. 退出登录\n";

        int choice = getValidChoice(0, 6);
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
            case 5: {
                cout << "\n--- 缺药登记 ---\n";
                cout << "1. 查看待办缺药清单\n";
                cout << "2. 主动报告库存不足\n";
                cout << "0. 返回\n";
                int sub = getValidChoice(0, 2);
                switch (sub) {
                    case 1: ShortageService::viewPendingShortages(shortageMgr); break;
                    case 2: ShortageService::reportShortage(drugMgr, shortageMgr); break;
                }
                break;
            }
            case 6: {
                cout << "请输入原密码：";
                string oldPwd; inputLine(oldPwd);
                cout << "请输入新密码：";
                string newPwd; inputLine(newPwd);
                doctorMgr.changePassword(doctorId, oldPwd, newPwd);
                break;
            }
            case 0:
                LogService::logLogout("doctor", doctorId);
                Session::getInstance()->logout();
                return;
        }
    }
}

// ==================== 登录流程 ====================

void patientLoginFlow(PatientManager& patientMgr, MedicalRecordManager& recordMgr,
                      PrescriptionManager& prescriptionMgr, DrugManager& drugMgr,
                      ShortageManager& shortageMgr, AppointmentManager& appointmentMgr,
                      DoctorManager& doctorMgr, DepartmentManager& deptMgr,
                      BedManager& bedMgr) {
    cout << "\n--- 患者登录 ---\n";
    cout << "患者ID：";
    string id; inputLine(id);
    cout << "密码：";
    string pin; inputLine(pin);

    if (AuthService::loginPatient(patientMgr, id, pin)) {
        Patient* p = patientMgr.findPatient(id);
        Session::getInstance()->login("patient", id, p ? p->name : "");
        LogService::logLogin("patient", id, true);
        // 若发生了明文→哈希迁移，持久化写入
        if (patientMgr.dirty) patientMgr.save();
        cout << "登录成功！\n";
        showPatientMenu(patientMgr, recordMgr, prescriptionMgr, drugMgr, shortageMgr,
                        appointmentMgr, doctorMgr, deptMgr, bedMgr);
    } else {
        LogService::logLogin("patient", id, false);
        cout << "登录失败！ID 或密码错误。\n";
    }
}

void doctorLoginFlow(DoctorManager& doctorMgr, AppointmentManager& appointmentMgr,
                     MedicalRecordManager& recordMgr, PrescriptionManager& prescriptionMgr,
                     DrugManager& drugMgr, DepartmentManager& deptMgr,
                     ShortageManager& shortageMgr, BedManager& bedMgr) {
    cout << "\n--- 医生登录 ---\n";
    cout << "账号：";
    string account; inputLine(account);
    cout << "密码：";
    string password; inputLine(password);

    if (AuthService::loginDoctor(doctorMgr, account, password)) {
        // 找到医生ID
        string doctorId;
        string doctorName;
        for (auto& d : doctorMgr.list) {
            if (d->account == account) {
                doctorId = d->id;
                doctorName = d->name;
                break;
            }
        }
        Session::getInstance()->login("doctor", doctorId, doctorName);
        LogService::logLogin("doctor", account, true);
        // 若发生了明文→哈希迁移，持久化写入
        if (doctorMgr.dirty) doctorMgr.save();
        cout << "登录成功！\n";
        showDoctorMenu(appointmentMgr, recordMgr, prescriptionMgr, drugMgr, deptMgr, shortageMgr, doctorMgr, bedMgr);
    } else {
        LogService::logLogin("doctor", account, false);
        cout << "登录失败！账号或密码错误。\n";
    }
}

void adminLoginFlow(PatientManager& patientMgr, DoctorManager& doctorMgr,
                    DepartmentManager& deptMgr, DrugManager& drugMgr,
                    BedManager& bedMgr, ShortageManager& shortageMgr) {
    cout << "\n--- 管理员登录 ---\n";
    cout << "账号：";
    string user; inputLine(user);
    cout << "密码：";
    string pwd; inputLine(pwd);

    if (AuthService::loginAdmin(user, pwd)) {
        Session::getInstance()->login("admin", user, "管理员");
        LogService::logLogin("admin", user, true);
        cout << "登录成功！\n";
        showAdminMenu(patientMgr, doctorMgr, deptMgr, drugMgr, bedMgr, shortageMgr);
    } else {
        LogService::logLogin("admin", user, false);
        cout << "登录失败！账号或密码错误。\n";
    }
}

// ==================== 主函数 ====================

int main() {
    // 确保数据目录存在：否则首次运行时各 Manager::save() 会因文件打开失败而静默丢弃数据
    {
        error_code ec;
        filesystem::create_directories(DATA_DIR, ec);
    }

    PatientManager patientMgr;
    DoctorManager doctorMgr;
    DepartmentManager deptMgr;
    DrugManager drugMgr;
    BedManager bedMgr;
    AppointmentManager appointmentMgr;
    MedicalRecordManager recordMgr;
    PrescriptionManager prescriptionMgr;
    ShortageManager shortageMgr;
    patientMgr.load();
    doctorMgr.load();
    deptMgr.load();
    drugMgr.load();
    bedMgr.load();
    appointmentMgr.load();
    recordMgr.load();
    prescriptionMgr.load();
    shortageMgr.load();

    cout << "=== 医院信息管理系统 ===\n";
    while (true) {
        cout << "\n请选择角色：\n";
        cout << "1. 患者\n";
        cout << "2. 医生\n";
        cout << "3. 管理员\n";
        cout << "0. 退出\n";

        int choice = getValidChoice(0, 3);
        switch (choice) {
            case 1:
                patientLoginFlow(patientMgr, recordMgr, prescriptionMgr, drugMgr,
                                   shortageMgr, appointmentMgr, doctorMgr, deptMgr, bedMgr);
                break;
            case 2:
                doctorLoginFlow(doctorMgr, appointmentMgr, recordMgr, prescriptionMgr,
                                 drugMgr, deptMgr, shortageMgr, bedMgr);
                break;
            case 3:
                adminLoginFlow(patientMgr, doctorMgr, deptMgr, drugMgr, bedMgr, shortageMgr);
                break;
            case 0: return 0;
        }
    }
}
