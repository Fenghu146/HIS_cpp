#include "shortage_service.h"
#include "../utils/input.h"
#include "../config/his_config.h"

void ShortageService::viewPendingShortages(ShortageManager& shortageMgr) {
    cout << "\n=== 待办缺药清单 ===\n";
    cout << "（按紧急度排序：紧急优先）\n";
    shortageMgr.listPending();
}

void ShortageService::reportShortage(
    DrugManager& drugMgr,
    ShortageManager& shortageMgr) {

    if (drugMgr.list.empty()) {
        cout << "暂无药品记录。\n";
        return;
    }

    // 列出药品供选择
    cout << "\n=== 药品列表 ===\n";
    int idx = 1;
    for (auto& d : drugMgr.list) {
        cout << "  " << idx++ << ". " << d->general_name
             << " (" << d->id << ") 库存:" << d->stock << endl;
    }
    cout << "  0. 返回\n";
    cout << "请选择缺药药品序号：";
    int choice = getValidChoice(0, static_cast<int>(drugMgr.list.size()));
    if (choice == 0) return;

    Drug* d = drugMgr.list[choice - 1].get();

    // 检查是否已有待处理记录
    if (shortageMgr.hasPendingForDrug(d->id)) {
        cout << "[提示] 药品 " << d->general_name << " 已有待处理的缺药记录。\n";
        cout << "是否仍要追加报告？(y/n)：";
        if (!getConfirm()) return;
    }

    cout << "请输入需求数量：";
    string qtyStr; inputLine(qtyStr);
    int qty = stoi(qtyStr);
    if (qty <= 0) {
        cout << "[错误] 数量必须大于0\n";
        return;
    }

    // 判定紧急度：如果库存为0或需求远超库存 → 紧急
    ShortageUrgency urgency;
    if (d->stock == 0) {
        urgency = ShortageUrgency::URGENT;
    } else if (qty > d->stock && (qty - d->stock) > d->stock / 2) {
        urgency = ShortageUrgency::URGENT;
    } else {
        urgency = ShortageUrgency::NORMAL;
    }

    Shortage* s = shortageMgr.addShortage(
        d->id, d->general_name, qty, d->stock,
        "", ShortageSource::REPORT, urgency
    );

    cout << "已登记缺药报告：" << s->id
         << "（" << d->general_name << "，紧急度："
         << (urgency == ShortageUrgency::URGENT ? "紧急" : "普通") << "）\n";
}

bool ShortageService::fulfillShortage(
    ShortageManager& shortageMgr,
    DrugManager& drugMgr) {

    auto pending = shortageMgr.getPending();
    if (pending.empty()) {
        cout << "暂无待处理缺药记录。\n";
        return false;
    }

    // 按紧急度排序显示
    sort(pending.begin(), pending.end(),
         [](const Shortage* a, const Shortage* b) {
             if (a->urgency != b->urgency)
                 return a->urgency == ShortageUrgency::URGENT;
             return a->id < b->id;
         });

    cout << "\n=== 待处理缺药记录 ===\n";
    for (size_t i = 0; i < pending.size(); i++) {
        cout << "  " << (i + 1) << ". " << *pending[i] << endl;
    }
    cout << "  0. 返回\n";
    cout << "请选择要处理的缺药记录序号：";
    int idx = getValidChoice(0, static_cast<int>(pending.size()));
    if (idx == 0) return false;

    Shortage* s = pending[idx - 1];

    cout << "\n选中记录：" << s->drug_name << "（需 " << s->required_amount
         << "，当前库存 " << s->current_stock << "）\n";
    cout << "处理方式：\n";
    cout << "  1. 已补货（同时入库）\n";
    cout << "  2. 已处理（换药等其他方式）\n";
    cout << "  0. 取消\n";
    cout << "请选择：";
    int action = getValidChoice(0, 2);
    if (action == 0) return false;

    if (action == 1) {
        // 已补货，询问入库数量
        cout << "请输入补货数量（直接回车则按需求数量 " << s->required_amount << "）：";
        string qtyStr; inputLine(qtyStr);
        int addQty = qtyStr.empty() ? s->required_amount : stoi(qtyStr);
        if (addQty > 0) {
            drugMgr.stockIn(s->drug_id, addQty);
        }
        shortageMgr.markHandled(s->id, ShortageStatus::FULFILLED);
        cout << "已标记为「已补货」并更新库存。\n";
    } else {
        shortageMgr.markHandled(s->id, ShortageStatus::RESOLVED);
        cout << "已标记为「已处理」。\n";
    }

    return true;
}
