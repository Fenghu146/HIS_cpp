#pragma once

#include "../model/entity.h"
#include "../model/shortage.h"
#include "../manager/patient_manager.h"
#include "../manager/prescription_manager.h"
#include "../manager/drug_manager.h"
#include "../manager/shortage_manager.h"

// 患者缴费 + 取药服务
class PaymentService {
public:
    // 缴费：患者支付未缴费处方 → 扣余额 → 状态变"已缴费"
    static bool payPrescription(
        PatientManager& patientMgr,
        PrescriptionManager& prescriptionMgr,
        DrugManager& drugMgr,
        const string& patient_id
    );

    // 取药：已缴费处方 → 逐项出库 → 状态变"已取药"
    // 库存不足时登记缺药并返回 false
    static bool dispensePrescription(
        PrescriptionManager& prescriptionMgr,
        DrugManager& drugMgr,
        ShortageManager& shortageMgr,
        const string& patient_id
    );

private:
    // 判定缺药紧急度
    static ShortageUrgency calcUrgency(int required, int stock);
};
