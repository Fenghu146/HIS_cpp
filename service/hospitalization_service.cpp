#include "hospitalization_service.h"
#include "../utils/input.h"
#include "../config/his_config.h"

bool HospitalizationService::admitPatient(PatientManager& patientMgr, BedManager& bedMgr,
                                          MedicalRecordManager& recordMgr, DoctorManager& doctorMgr,
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

    // 2. 通过病历中的医生 ID 找到医生，确定建议住院的科室名称
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

    // 说明：Doctor 仅保存科室名称（dept_name），而 Bed 使用科室 ID（dept_id），
    // 二者缺少直接映射；因此这里后退为「列出全部空闲床位」供选择。
    // 若需按科室精确筛选，应让病历/挂号单记录 dept_id 后再过滤。
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
        cout << "  " << (i + 1) << ". " << freeBeds[i]->bed_number << " (" << freeBeds[i]->id
             << ") 类型:" << freeBeds[i]->type << " 日费:" << freeBeds[i]->daily_price << "分/天\n";
    }
    cout << "  0. 取消\n";
    cout << "请选择床位序号：";
    int idx = getValidChoice(0, static_cast<int>(freeBeds.size()));
    if (idx == 0) {
        cout << "已取消住院。\n";
        return false;
    }

    Bed* bed = freeBeds[idx - 1];
    cout << "确认入住 " << bed->bed_number << "（" << bed->type << "，日费" << bed->daily_price
         << "分/天）？(y/n)：";
    if (!getConfirm()) {
        cout << "已取消住院。\n";
        return false;
    }

    // 扣首日费
    if (p->balance < bed->daily_price) {
        cout << "[错误] 余额不足！需要 " << bed->daily_price << " 分，当前 " << p->balance
             << " 分。\n";
        return false;
    }
    p->balance -= bed->daily_price;
    patientMgr.markDirty();
    patientMgr.save();
    bedMgr.occupyBed(bed->id, patient_id);
    cout << "住院办理成功！床位：" << bed->bed_number << "，扣除首日费：" << bed->daily_price
         << " 分\n";
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
