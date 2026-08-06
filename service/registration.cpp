#include "registration.h"
#include "../utils/input.h"
#include "../config/his_config.h"

string RegistrationService::selectDoctor(DoctorManager& doctorMgr, const string& dept_name) {
    // 收集该科室的医生
    vector<Doctor*> deptDoctors;
    for (auto& d : doctorMgr.list) {
        if (d->dept_name == dept_name) {
            deptDoctors.push_back(d.get());
        }
    }

    if (deptDoctors.empty()) {
        cout << "该科室暂无医生！\n";
        return "";
    }

    cout << "可选医生：\n";
    for (size_t i = 0; i < deptDoctors.size(); i++) {
        cout << "  " << (i + 1) << ". " << deptDoctors[i]->name
             << " (" << deptDoctors[i]->id << ") "
             << "擅长：" << deptDoctors[i]->specialty << endl;
    }
    cout << "请选择医生序号：";
    int idx = getValidChoice(1, static_cast<int>(deptDoctors.size()));
    return deptDoctors[idx - 1]->id;
}

bool RegistrationService::registerPatient(
    PatientManager& patientMgr,
    DoctorManager& doctorMgr,
    DepartmentManager& deptMgr,
    AppointmentManager& appointmentMgr
) {
    // 1. 输入患者ID
    cout << "请输入患者ID：";
    string patientId;
    inputLine(patientId);
    Patient* p = patientMgr.findPatient(patientId);
    if (!p) {
        cout << "[错误] 未找到患者 " << patientId << endl;
        return false;
    }
    cout << "患者：" << p->name << "，当前余额：" << p->balance << " 分" << endl;

    // 2. 选择科室
    if (deptMgr.list.empty()) {
        cout << "[错误] 暂无科室！\n";
        return false;
    }
    cout << "可选科室：\n";
    int idx = 1;
    for (auto& d : deptMgr.list) {
        cout << "  " << idx++ << ". " << d->name << " (" << d->id << ")\n";
    }
    cout << "请选择科室序号：";
    int deptIdx = getValidChoice(1, static_cast<int>(deptMgr.list.size()));
    string deptId = deptMgr.list[deptIdx - 1]->id;
    string deptName = deptMgr.list[deptIdx - 1]->name;

    // 3. 选择医生
    string doctorId = selectDoctor(doctorMgr, deptName);
    if (doctorId.empty()) return false;
    Doctor* d = doctorMgr.findDoctor(doctorId);
    cout << "已选择医生：" << d->name << endl;

    // 4. 确认挂号费
    cout << "挂号费：" << REGISTRATION_FEE << " 分" << endl;
    if (p->balance < REGISTRATION_FEE) {
        cout << "[错误] 余额不足！当前余额 " << p->balance
             << " 分，需要 " << REGISTRATION_FEE << " 分。\n";
        cout << "请先充值后再挂号。\n";
        return false;
    }

    // 5. 确认
    cout << "确认挂号？(y/n)：";
    if (!getConfirm()) {
        cout << "已取消挂号。\n";
        return false;
    }

    // 6. 扣费
    p->balance -= REGISTRATION_FEE;
    patientMgr.save();

    // 7. 生成挂号单
    Appointment* apt = appointmentMgr.addAppointment(patientId, doctorId, deptId, REGISTRATION_FEE);

    cout << "挂号成功！\n";
    cout << "  挂号单号：" << apt->id << endl;
    cout << "  患者：" << p->name << endl;
    cout << "  医生：" << d->name << endl;
    cout << "  科室：" << deptName << endl;
    cout << "  扣除挂号费：" << REGISTRATION_FEE << " 分" << endl;
    cout << "  当前余额：" << p->balance << " 分" << endl;

    return true;
}
