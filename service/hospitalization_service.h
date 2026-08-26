#pragma once

#include "../manager/bed_manager.h"
#include "../manager/medical_record_manager.h"
#include "../manager/patient_manager.h"
#include "../manager/doctor_manager.h"

class HospitalizationService {
public:
    // 为患者办理住院：
    // 1. 查找该患者最近一条 need_hospitalize=true 的病历
    // 2. 通过病历找到医生所在科室，获取科室ID
    // 3. 显示该科室的空闲床位列表
    // 4. 患者选择床位，扣取首日日费，占床
    static bool admitPatient(
        PatientManager& patientMgr,
        BedManager& bedMgr,
        MedicalRecordManager& recordMgr,
        DoctorManager& doctorMgr,
        const string& patient_id);

    // 查看当前住院患者列表（管理员用）
    static void listHospitalized(BedManager& bedMgr);
};
