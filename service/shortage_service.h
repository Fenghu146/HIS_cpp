#pragma once

#include "../model/shortage.h"
#include "../manager/drug_manager.h"
#include "../manager/shortage_manager.h"

// 缺药登记服务：查看待办、主动报告、处理缺药
class ShortageService {
public:
    // 查看待办缺药清单（按紧急度排序）
    static void viewPendingShortages(ShortageManager& shortageMgr);

    // 主动报告库存不足（医生/任何人可用）
    static void reportShortage(
        DrugManager& drugMgr,
        ShortageManager& shortageMgr
    );

    // 处理缺药：管理员标记已补货（可选同时入库）
    static bool fulfillShortage(
        ShortageManager& shortageMgr,
        DrugManager& drugMgr
    );
};
