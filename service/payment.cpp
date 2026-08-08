#include "payment.h"
#include "../utils/input.h"
#include "../config/his_config.h"

ShortageUrgency PaymentService::calcUrgency(int required, int stock) {
    if (stock == 0) return ShortageUrgency::URGENT;
    // 缺少数量超过当前库存的一半 → 紧急
    if (required > stock && (required - stock) > stock / 2) return ShortageUrgency::URGENT;
    if (required > stock) return ShortageUrgency::NORMAL;
    return ShortageUrgency::NORMAL;
}

bool PaymentService::payPrescription(
    PatientManager& patientMgr,
    PrescriptionManager& prescriptionMgr,
    DrugManager& drugMgr,
    const string& patient_id) {

    Patient* p = patientMgr.findPatient(patient_id);
    if (!p) {
        cout << "[错误] 未找到患者 " << patient_id << endl;
        return false;
    }

    // 筛选未缴费处方
    vector<Prescription*> unpaid;
    for (auto& rx : prescriptionMgr.list) {
        if (rx->patient_id == patient_id && rx->status == PrescriptionStatus::UNPAID) {
            unpaid.push_back(rx.get());
        }
    }

    if (unpaid.empty()) {
        cout << "暂无未缴费处方。\n";
        return false;
    }

    cout << "\n=== 未缴费处方 ===\n";
    for (size_t i = 0; i < unpaid.size(); i++) {
        cout << "  " << (i + 1) << ". " << unpaid[i]->id
             << " | 总金额：" << unpaid[i]->total_amount << " 分（"
             << unpaid[i]->total_amount / 100.0 << " 元）"
             << " | " << unpaid[i]->create_time << endl;
    }
    cout << "  0. 返回\n";
    cout << "请选择要缴费的处方序号：";
    int idx = getValidChoice(0, static_cast<int>(unpaid.size()));
    if (idx == 0) return false;

    Prescription* rx = unpaid[idx - 1];

    // 显示处方详情
    cout << "\n--- 处方详情 ---\n";
    prescriptionMgr.displayPrescription(rx->id, drugMgr);
    cout << "\n需支付：" << rx->total_amount << " 分（" << rx->total_amount / 100.0 << " 元）\n";
    cout << "当前余额：" << p->balance << " 分\n";

    if (p->balance < rx->total_amount) {
        cout << "[错误] 余额不足！请先充值。\n";
        return false;
    }

    cout << "确认缴费？(y/n)：";
    if (!getConfirm()) {
        cout << "已取消缴费。\n";
        return false;
    }

    // 扣费 + 更新状态
    p->balance -= rx->total_amount;
    patientMgr.save();
    prescriptionMgr.updateStatus(rx->id, PrescriptionStatus::PAID);

    cout << "缴费成功！处方 " << rx->id << " 已缴费，当前余额：" << p->balance << " 分\n";
    return true;
}

bool PaymentService::dispensePrescription(
    PrescriptionManager& prescriptionMgr,
    DrugManager& drugMgr,
    ShortageManager& shortageMgr,
    const string& patient_id) {

    // 筛选已缴费（待取药）处方
    vector<Prescription*> paid;
    for (auto& rx : prescriptionMgr.list) {
        if (rx->patient_id == patient_id && rx->status == PrescriptionStatus::PAID) {
            paid.push_back(rx.get());
        }
    }

    if (paid.empty()) {
        cout << "暂无待取药（已缴费）处方。\n";
        return false;
    }

    cout << "\n=== 待取药处方 ===\n";
    for (size_t i = 0; i < paid.size(); i++) {
        cout << "  " << (i + 1) << ". " << paid[i]->id
             << " | 总金额：" << paid[i]->total_amount << " 分"
             << " | " << paid[i]->create_time << endl;
    }
    cout << "  0. 返回\n";
    cout << "请选择要取药的处方序号：";
    int idx = getValidChoice(0, static_cast<int>(paid.size()));
    if (idx == 0) return false;

    Prescription* rx = paid[idx - 1];

    // 收集该处方的明细
    vector<PrescriptionItem*> items;
    for (auto& item : prescriptionMgr.itemMgr.list) {
        if (item->prescription_id == rx->id) {
            items.push_back(item.get());
        }
    }

    if (items.empty()) {
        cout << "[错误] 处方 " << rx->id << " 无明细记录。\n";
        return false;
    }

    // Phase 1: 预检查所有明细库存（不修改任何数据）
    cout << "\n--- 检查库存 ---\n";
    bool allOk = true;
    for (auto* item : items) {
        Drug* d = drugMgr.findDrug(item->drug_id);
        if (!d) {
            cout << "[错误] 药品 " << item->drug_id << " 不存在\n";
            allOk = false;
            continue;
        }
        cout << "  " << d->general_name << "：需要 " << item->quantity
             << "，库存 " << d->stock;
        if (d->stock < item->quantity) {
            cout << "  ✗ 不足";
            allOk = false;
        } else {
            cout << "  ✓";
        }
        cout << endl;
    }

    if (!allOk) {
        cout << "\n[取药失败] 药品库存不足，无法取药。\n";

        // 登记缺药：对每一项不足的药品生成缺药记录
        cout << "已自动登记缺药信息，请联系医生处理（可换药或等待补货）。\n";
        for (auto* item : items) {
            Drug* d = drugMgr.findDrug(item->drug_id);
            if (!d) continue;
            if (d->stock < item->quantity) {
                ShortageUrgency urgency = calcUrgency(item->quantity, d->stock);
                shortageMgr.addShortage(
                    d->id, d->general_name,
                    item->quantity, d->stock,
                    rx->id, ShortageSource::DISPENSE,
                    urgency
                );
                cout << "  已登记缺药：" << d->general_name
                     << "（需 " << item->quantity << "，存 " << d->stock << "）"
                     << (urgency == ShortageUrgency::URGENT ? " [紧急]" : "") << endl;
            }
        }
        return false;
    }

    // Phase 2: 确认后执行出库
    cout << "\n确认取药？(y/n)：";
    if (!getConfirm()) {
        cout << "已取消取药。\n";
        return false;
    }

    for (auto* item : items) {
        drugMgr.stockOut(item->drug_id, item->quantity);
    }
    prescriptionMgr.updateStatus(rx->id, PrescriptionStatus::DISPENSED);

    cout << "取药成功！处方 " << rx->id << " 状态已更新为已取药。\n";
    return true;
}
