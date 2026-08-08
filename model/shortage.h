#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
using namespace std;

// 缺药紧急度
enum class ShortageUrgency { NORMAL, URGENT };

// 缺药记录实体
class Shortage {
public:
    string id;                  // SH1, SH2, ...
    string drug_id;             // 缺药药品ID
    string drug_name;           // 药品名称（冗余，方便显示）
    int required_amount = 0;    // 需要数量
    int current_stock = 0;      // 当前库存
    string prescription_id;     // 关联处方ID（取药触发时有值，主动报告为空）
    string triggered_by;        // 触发来源：取药 / 主动报告
    ShortageUrgency urgency;    // 紧急度
    string status;              // 待处理 / 已补货 / 已处理
    string create_time;

    Shortage() = default;

    friend ostream& operator<<(ostream& os, const Shortage& s) {
        os << "ID:" << s.id
           << " | 药品:" << s.drug_name << "(" << s.drug_id << ")"
           << " | 需/存:" << s.required_amount << "/" << s.current_stock
           << " | 来源:" << s.triggered_by
           << " | 紧急度:" << (s.urgency == ShortageUrgency::URGENT ? "紧急" : "普通")
           << " | 状态:" << s.status
           << " | 时间:" << s.create_time;
        if (!s.prescription_id.empty()) os << " | 处方:" << s.prescription_id;
        return os;
    }
};

using ShortageList = vector<unique_ptr<Shortage>>;
