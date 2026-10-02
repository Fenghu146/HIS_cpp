#include "log.h"
#include "../config/his_config.h"
#include "../utils/datetime.h"
#include <fstream>
using namespace std;

void LogService::log(const string& role, const string& user_id, const string& action) {
    ofstream out(FILE_LOG, ios::app);
    if (!out.is_open())
        return;
    out << "[" << nowTimestamp() << "] "
        << "[" << role << "] "
        << "[" << user_id << "] " << action << "\n";
    out.close();
}

void LogService::logLogin(const string& role, const string& user_id, bool success) {
    log(role, user_id, success ? "登录成功" : "登录失败");
}

void LogService::logLogout(const string& role, const string& user_id) {
    log(role, user_id, "退出登录");
}
