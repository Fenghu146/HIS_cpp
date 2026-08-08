#include "input.h"
#include "../config/his_config.h"

void ClearInputBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); //清空输入缓冲区，忽略所有字符直到换行符
}

bool inputLine(string& out) {
    if (!getline(cin, out)) return false;
    if (!out.empty() && out.back() == '\r') out.pop_back(); //去掉可能存在的回车符
    return true;
}

bool getConfirm() {
    string line;
    if (!inputLine(line)) return false;
    return line == "Y" || line == "y";
}

bool confirmAction(const string& msg) {
    cout << msg << "(y/n):";
    return getConfirm();
}

void waitForEnter() {
    cout << "\n按回车键继续...";
    ClearInputBuffer();
    cin.get();
}

int getValidChoice(int min, int max) {
    while (true) {
        cout << "请输入选择（" << min << "-" << max << "):";
        string input;
        // 先按整行读取，避免 cin >> 与 getline 混用造成的残留换行问题
        if (!inputLine(input)) {
            cout << "[错误] 输入异常，请重新输入。\n";
            continue;
        }
        // 不允许空输入
        if (input.empty()) {
            cout << "[错误] 输入不能为空！\n";
            continue;
        }
        // 第一位必须是数字，避免字母或符号进入 stoi
        if (!isdigit(input[0])) {
            cout << "[错误] 只能输入数字！\n";
            continue;
        }
        int choice;
        // 将字符串转成整数，若数值越界则捕获异常并重试
        try { choice = stoi(input); }
        catch (const out_of_range&) {
            cout << "[错误] 超出范围！\n";
            continue;
        }
        // 最后检查是否落在可接受的选项范围内
        if (choice >= min && choice <= max) return choice;
        cout << "[错误] 超出范围！\n";
    }
}
