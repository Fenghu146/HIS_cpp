#pragma once

#include <string>
using namespace std;

bool isValidNumber(const string& str);
bool isValidPhone(const string& phone);
bool isValidIDCard(const string& id_card);
bool hasNoPipe(const string& str);

// ============================================================================
// 安全数值解析
// ----------------------------------------------------------------------------
// 直接对用户输入调用 stoi/stof 会在非法输入时抛出异常并导致程序崩溃。
// 以下函数解析失败时返回 false（同时不修改 out），调用方负责给出提示。
// 仅接受非负数字：整数不含正负号，浮点数额外允许一个小数点。
// ============================================================================
bool parseInt(const string& str, int& out);
bool parseLongLong(const string& str, long long& out);
bool parseFloat(const string& str, float& out);
