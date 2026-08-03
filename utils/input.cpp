#include "input.h"
#include "../config/his_config.h"

void ClearInputBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool inputLine(string& out) {
    if (!getline(cin, out)) return false;
    if (!out.empty() && out.back() == '\r') out.pop_back();
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
        if (!inputLine(input)) {
            cout << "[错误] 输入异常，请重新输入。\n";
            continue;
        }
        if (input.empty()) {
            cout << "[错误] 输入不能为空！\n";
            continue;
        }
        if (!isdigit(input[0])) {
            cout << "[错误] 只能输入数字！\n";
            continue;
        }
        int choice;
        try { choice = stoi(input); }
        catch (const out_of_range&) {
            cout << "[错误] 超出范围！\n";
            continue;
        }
        if (choice >= min && choice <= max) return choice;
        cout << "[错误] 超出范围！\n";
    }
}
