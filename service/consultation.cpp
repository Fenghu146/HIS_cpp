#include "consultation.h"
#include "../utils/input.h"
#include "../utils/validator.h"
#include "../config/his_config.h"

void ConsultationService::showWaitingList(AppointmentManager& appointmentMgr, const string& doctor_id) {
    cout << "\n=== 待诊患者列表 ===\n";
    bool found = false;
    int idx = 1;
    for (auto& a : appointmentMgr.list) {
        if (a->doctor_id == doctor_id && a->status == AppointmentStatus::WAITING) {
            cout << "  " << idx++ << ". " << *a << endl;
            found = true;
        }
    }
    if (!found) {
        cout << "暂无待诊患者。\n";
    }
}

bool ConsultationService::prescribe(
    PrescriptionManager& prescriptionMgr,
    DrugManager& drugMgr,
    const string& record_id,
    const string& patient_id,
    const string& doctor_id) {

    vector<PrescriptionManager::ItemInput> items;

    while (true) {
        cout << "\n--- 开处方 ---\n";
        // 显示可选药品
        if (drugMgr.list.empty()) {
            cout << "暂无药品，无法开处方。\n";
            return false;
        }

        cout << "可选药品：\n";
        int idx = 1;
        for (auto& d : drugMgr.list) {
            cout << "  " << idx++ << ". " << d->general_name
                 << " (" << d->id << ") 库存:" << d->stock
                 << " 单价:" << d->price << "元" << endl;
        }
        cout << "  0. 完成开方\n";
        cout << "请选择药品序号：";
        int drugIdx = getValidChoice(0, static_cast<int>(drugMgr.list.size())); //static_cast<int> 用于将 size_t 转换为 int，避免编译器警告
        if (drugIdx == 0) break;

        Drug* drug = drugMgr.list[drugIdx - 1].get();
        if (drug->stock <= 0) {
            cout << "[错误] 该药品库存为0，请选择其他药品。\n";
            continue;
        }

        cout << "请输入数量：";
        string qtyStr; inputLine(qtyStr);
        int qty = stoi(qtyStr);
        if (qty <= 0) {
            cout << "[错误] 数量必须大于0\n";
            continue;
        }
        if (qty > drug->stock) {
            cout << "[错误] 库存不足！当前库存：" << drug->stock << endl;
            continue;
        }

        cout << "请输入用法用量：";
        string usage; inputLine(usage);

        // 计算金额（drug.price 是元，转为分）
        int amount = static_cast<int>(drug->price * 100) * qty;

        PrescriptionManager::ItemInput item;
        item.drug_id = drug->id;
        item.quantity = qty;
        item.usage = usage;
        item.amount = amount;
        items.push_back(item);

        cout << "已添加：" << drug->general_name << " ×" << qty << " = " << amount << "分\n";
        cout << "当前处方总金额：";
        int total = 0;
        for (auto& i : items) total += i.amount;
        cout << total << "分\n";
    }

    if (items.empty()) {
        cout << "未添加任何药品，取消开方。\n";
        return false;
    }

    // 创建处方
    Prescription* rx = prescriptionMgr.addPrescription(record_id, patient_id, doctor_id, items);
    cout << "处方已生成！\n";
    cout << "  处方号：" << rx->id << endl;
    cout << "  总金额：" << rx->total_amount << " 分（" << rx->total_amount / 100.0 << " 元）\n";
    cout << "  状态：未缴费\n";

    return true;
}

bool ConsultationService::consultPatient(
    AppointmentManager& appointmentMgr,
    MedicalRecordManager& recordMgr,
    PrescriptionManager& prescriptionMgr,
    DrugManager& drugMgr,
    DepartmentManager& deptMgr,
    const string& doctor_id) {

    // 1. 收集待诊患者
    vector<Appointment*> waiting;
    for (auto& a : appointmentMgr.list) {
        if (a->doctor_id == doctor_id && a->status == AppointmentStatus::WAITING) {
            waiting.push_back(a.get());
        }
    }

    if (waiting.empty()) {
        cout << "暂无待诊患者。\n";
        return false;
    }

    // 2. 选择患者
    cout << "\n=== 选择接诊患者 ===\n";
    for (size_t i = 0; i < waiting.size(); i++) {
        cout << "  " << (i + 1) << ". " << waiting[i]->patient_id
             << " (挂号单：" << waiting[i]->id << ")\n";
    }
    cout << "请选择患者序号：";
    int idx = getValidChoice(1, static_cast<int>(waiting.size()));
    Appointment* apt = waiting[idx - 1];

    cout << "\n接诊患者：" << apt->patient_id << "，挂号单：" << apt->id << endl;

    // 3. 写病历
    cout << "\n--- 书写病历 ---\n";
    string complaint, diagnosis, orders;

    do {
        cout << "请输入主诉：";
        inputLine(complaint);
        if (!hasNoPipe(complaint)) cout << "[错误] 不能包含 | 字符！\n";
    } while (!hasNoPipe(complaint)); //安全输入

    do {
        cout << "请输入诊断：";
        inputLine(diagnosis);
        if (!hasNoPipe(diagnosis)) cout << "[错误] 不能包含 | 字符！\n";
    } while (!hasNoPipe(diagnosis));

    do {
        cout << "请输入医嘱：";
        inputLine(orders);
        if (!hasNoPipe(orders)) cout << "[错误] 不能包含 | 字符！\n";
    } while (!hasNoPipe(orders));

    // 4. 是否建议住院
    cout << "是否建议住院？(y/n)：";
    bool needHosp = getConfirm();

    // 5. 创建病历
    MedicalRecord* record = recordMgr.addRecord(
        apt->id, apt->patient_id, doctor_id,
        complaint, diagnosis, orders, needHosp
    );
    cout << "病历已创建，ID：" << record->id << endl;

    // 6. 更新挂号单状态
    appointmentMgr.updateStatus(apt->id, AppointmentStatus::IN_CONSULT);

    // 7. 是否开处方
    cout << "是否开处方？(y/n)：";
    if (getConfirm()) {
        prescribe(prescriptionMgr, drugMgr, record->id, apt->patient_id, doctor_id);
    }

    // 8. 完成就诊
    appointmentMgr.updateStatus(apt->id, AppointmentStatus::COMPLETED);
    cout << "\n就诊完成！\n";

    if (needHosp) {
        cout << "[提示] 该患者建议住院，请办理住院手续。\n";
    }

    return true;
}
