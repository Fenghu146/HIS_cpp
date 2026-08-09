#include "log.h"
#include "../config/his_config.h"
#include <fstream>
#include <ctime>
#include <sstream>
using namespace std;

static string currentTimestamp() {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
    return string(buf);
}

void LogService::log(const string& role, const string& user_id, const string& action) {
    ofstream out("data/log.txt", ios::app);
    if (!out.is_open()) return;
    out << "[" << currentTimestamp() << "] "
        << "[" << role << "] "
        << "[" << user_id << "] "
        << action << "\n";
    out.close();
}

void LogService::logLogin(const string& role, const string& user_id, bool success) {
    log(role, user_id, success ? "登录成功" : "登录失败");
}

void LogService::logLogout(const string& role, const string& user_id) {
    log(role, user_id, "退出登录");
}
