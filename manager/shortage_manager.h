#pragma once

#include "data_manager.h"
#include "../model/shortage.h"
#include "../config/his_config.h"

// 缺药记录管理器
class ShortageManager : public DataManager<Shortage> {
public:
    static int next_id;

    void load() override;
    void save() override;

    // 添加缺药记录
    Shortage* addShortage(const string& drug_id, const string& drug_name,
                          int required, int stock, const string& prescription_id,
                          const string& triggered_by, ShortageUrgency urgency);

    // 列出所有记录
    void listAll();

    // 按状态筛选列出
    void listByStatus(const string& status);

    // 列出待处理记录（按紧急度排序：紧急在前）
    void listPending();

    // 按ID查找
    Shortage* findShortage(const string& id);

    // 标记已处理
    bool markHandled(const string& id, const string& action);

    // 获取待处理记录列表
    vector<Shortage*> getPending();

    // 检查某药品是否已有待处理记录
    bool hasPendingForDrug(const string& drug_id);

    ShortageManager() : DataManager(FILE_SHORTAGE) {}

    string generateId() {
        return string(1, ID_SHORTAGE) + to_string(next_id++);
    }

private:
    // 按紧急度排序（紧急在前）
    static bool compareByUrgency(const Shortage* a, const Shortage* b);
};
