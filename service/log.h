#pragma once

#include <string>
using namespace std;

// 日志服务：记录登录/登出/关键操作到 data/log.txt
class LogService {
public:
    // 通用日志记录
    static void log(const string& role, const string& user_id, const string& action);

    // 登录事件
    static void logLogin(const string& role, const string& user_id, bool success);

    // 登出事件
    static void logLogout(const string& role, const string& user_id);
};
