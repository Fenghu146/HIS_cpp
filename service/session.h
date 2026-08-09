#pragma once

#include <string>
using namespace std;

// 全局会话单例：保存当前登录身份
class Session {
    static Session* instance;
    string role;       // "patient" / "doctor" / "admin" / ""
    string user_id;
    string user_name;

    Session() : role(""), user_id(""), user_name("") {}

public:
    static Session* getInstance();

    void login(const string& role, const string& id, const string& name);
    void logout();
    bool isLoggedIn() const;
    string getRole() const;
    string getUserId() const;
    string getUserName() const;

    // 禁止拷贝
    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;
};

// 内联实现，避免额外 .cpp 文件
inline Session* Session::instance = nullptr;

inline Session* Session::getInstance() {
    if (!instance) instance = new Session();
    return instance;
}

inline void Session::login(const string& r, const string& id, const string& name) {
    role = r;
    user_id = id;
    user_name = name;
}

inline void Session::logout() {
    role = "";
    user_id = "";
    user_name = "";
}

inline bool Session::isLoggedIn() const { return !role.empty(); }
inline string Session::getRole() const { return role; }
inline string Session::getUserId() const { return user_id; }
inline string Session::getUserName() const { return user_name; }
