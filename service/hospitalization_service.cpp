#include "hospitalization_service.h"
#include "../utils/input.h"
#include "../config/his_config.h"
#include <algorithm>

bool HospitalizationService::admitPatient(
    PatientManager& patientMgr,
    BedManager& bedMgr,
    MedicalRecordManager& recordMgr,
    DoctorManager& doctorMgr,
    const string& patient_id) {

    Patient* p = patientMgr.findPatient(patient_id);
    if (!p) {
        cout << "[错误] 未找到患者 " << patient_id << endl;
        return false;
    }

    // 1. 查找该患者最近一条建议住院的病历
    MedicalRecord* hospRecord = nullptr;
    for (auto& r : recordMgr.list) {
        if (r->patient_id == patient_id && r->need_hospitalize) {
            hospRecord = r.get();
        }
    }
    if (!hospRecord) {
        cout << "[错误] 该患者无建议住院的病历记录，无法办理住院。\n";
        return false;
    }

    // 2. 通过病历中的医生 ID 找到医生，再找科室 ID
    Doctor* doctor = nullptr;
    for (auto& d : doctorMgr.list) {
        if (d->id == hospRecord->doctor_id) {
            doctor = d.get();
            break;
        }
    }
    if (!doctor) {
        cout << "[错误] 找不到病历对应医生，无法确定住院科室。\n";
        return false;
    }

    string deptId = doctor->dept_name; // 用科室名称匹配床位列表中的 dept_id（不精确，改为找科室ID）
    // 实际通过科室管理器找科室ID（这里简化：床位按 dept_id 存储，而医生有 dept_name）
    // 从床位管理中找属于该医生科室名称的空闲床（需要知道科室名到ID的映射）
    // 由于 Doctor 只有 dept_name，我们遍历所有床位找名字匹配的科室
    // 更好的做法：让 Bed 用 dept_id，这里通过科室名称反查
    string targetDeptId;
    // 暂时无法从 dept_name 直接查 dept_id，改用所有空闲床位中 dept_id 与医生 dept_name 关联
    // 实际数据中 dept_id 与 dept_name 不一致，这里改为：列出所有空闲床位让患者选择
    // （更合理的方案是病历中记录 dept_id，但现有结构只用 doctor_id）

    // 由于数据结构限制，我们展示所有空闲床位供选择
    vector<Bed*> freeBeds;
    for (auto& b : bedMgr.list) {
        if (b->status == BedStatus::FREE) {
            freeBeds.push_back(b.get());
        }
    }

    if (freeBeds.empty()) {
        cout << "[错误] 暂无空闲床位，请稍后再试。\n";
        return false;
    }

    cout << "\n=== 办理住院 ===\n";
    cout << "患者：" << p->name << "（" << patient_id << "）\n";
    cout << "建议住院科室：" << doctor->dept_name << "\n";
    cout << "\n可用床位：\n";
    for (size_t i = 0; i < freeBeds.size(); i++) {
        cout << "  " << (i + 1) << ". " << freeBeds[i]->bed_number
             << " (" << freeBeds[i]->id << ") 类型:" << freeBeds[i]->type
             << " 日费:" << freeBeds[i]->daily_price << "分/天\n";
    }
    cout << "  0. 取消\n";
    cout << "请选择床位序号：";
    int idx = getValidChoice(0, static_cast<int>(freeBeds.size()));
    if (idx == 0) {
        cout << "已取消住院。\n";
        return false;
    }

    Bed* bed = freeBeds[idx - 1];
    cout << "确认入住 " << bed->bed_number << "（" << bed->type << "，日费" << bed->daily_price << "分/天）？(y/n)：";
    if (!getConfirm()) {
        cout << "已取消住院。\n";
        return false;
    }

    // 扣首日费
    if (p->balance < bed->daily_price) {
        cout << "[错误] 余额不足！需要 " << bed->daily_price << " 分，当前 " << p->balance << " 分。\n";
        return false;
    }
    p->balance -= bed->daily_price;
    patientMgr.markDirty();
    patientMgr.save();
    bedMgr.occupyBed(bed->id, patient_id);
    cout << "住院办理成功！床位：" << bed->bed_number
         << "，扣除首日费：" << bed->daily_price << " 分\n";
    cout << "当前余额：" << p->balance << " 分\n";
    return true;
}

void HospitalizationService::listHospitalized(BedManager& bedMgr) {
    cout << "\n=== 住院患者列表 ===\n";
    bool found = false;
    for (auto& b : bedMgr.list) {
        if (b->status == BedStatus::OCCUPIED && !b->patient_id.empty()) {
            cout << *b << endl;
            found = true;
        }
    }
    if (!found) {
        cout << "暂无住院患者。\n";
    }
}
