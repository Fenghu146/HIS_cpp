#pragma once

#include "../model/entity.h"
#include "../manager/appointment_manager.h"
#include "../manager/medical_record_manager.h"
#include "../manager/prescription_manager.h"
#include "../manager/drug_manager.h"
#include "../manager/patient_manager.h"
#include "../manager/dept_manager.h"

// 就诊服务：医生接诊、写病历、开处方
class ConsultationService {
public:
    // 查看某医生的待诊列表
    static void showWaitingList(AppointmentManager& appointmentMgr, const string& doctor_id);

    // 接诊（完整流程）
    static bool consultPatient(
        AppointmentManager& appointmentMgr,
        MedicalRecordManager& recordMgr,
        PrescriptionManager& prescriptionMgr,
        DrugManager& drugMgr,
        DepartmentManager& deptMgr,
        const string& doctor_id
    );

private:
    // 开处方子流程
    static bool prescribe(
        PrescriptionManager& prescriptionMgr,
        DrugManager& drugMgr,
        const string& record_id,
        const string& patient_id,
        const string& doctor_id
    );
};
