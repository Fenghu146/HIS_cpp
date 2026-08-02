#include "his_tool.h"
#include <fstream>
#include <sstream>


//清除输入缓冲区
void ClearInputBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(),'\n');
}

//字符串输入
bool inputLine(string& out) {
    if (!getline(cin,out)) {
        return false;
    }
    if (!out.empty() && out.back() == '\r') {
        out.pop_back();
    }
    return true;
}

//确认输入
bool getConfirm() {
    string line;
    if (!inputLine(line)) return false;
    return line == "Y" || line == "y";
}

//带提示的确认输入
bool confirmAction(const string& msg) {
    cout<<msg<<"(y/n):";
    return getConfirm();
}

//等待回车
void waitForEnter() {
    cout<<"\n按回车键继续...";
    ClearInputBuffer();
    cin.get();
}

//选择输入校验
int getValidChoice(int min,int max) {
    while (true) {
        cout<<"请输入选择（"<<min<<"-"<<max<<"):";
        string input;
        if (!inputLine(input)) {
            cout<<"[错误] 输入异常，请重新输入。\n";
            continue;
        }

        if (input.empty()) {
            cout<<"[错误] 输入不能为空！\n";
            continue;
        }

        if (!isValidNumber(input)) {
            cout<<"[错误] 只能输入数字！\n";
            continue;
        }

        int choice = stoi(input); //字符串转整数
        if (choice >=min && choice <=max) {
            return choice;
        }
        cout<<"[错误] 超出范围！\n";
    }
}

//校验数字
bool isValidNumber(const string& str) {
    if (str.empty()) return false;
    return all_of(str.begin(),str.end(),::isdigit);
}


//校验手机号
bool isValidPhone(const string& phone) {
    if (phone.size() != 11) return false;
    if (phone[0] !='1') return false;
    return all_of(phone.begin(),phone.end(),::isdigit);
}

//校验身份证
bool isValidIDCard(const string& id_card) {
    if (id_card.size() !=18) return false;

    for (size_t i = 0;i < 17;i++) {
        // static_cast<unsigned char>: char可能为有符号，高位字符值变负数，isdigit(负数)是未定义行为
        if (!isdigit(static_cast<unsigned char>(id_card[i])))
        return false;
    }

    char last = id_card[17];
    if (!isdigit(static_cast<unsigned char>(last)) && last != 'X' && last != 'x') {
        return false;
    }

    // GB 11643-1999 加权校验
    static const int weights[17] = {7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2};
    static const char check_chars[] = "10X98765432";

    int sum = 0;
    for (size_t i = 0; i < 17; i++) {
        sum += (id_card[i] - '0') * weights[i];
    }

    char expected = check_chars[sum % 11];
    char actual = (last >= 'a' && last <= 'z') ? (last - 'a' + 'A') : last;

    if (actual != expected) {
        std::cout << "    [提示] 校验位应为 " << expected << "，实际输入为 " << actual << "\n";
    }
    return actual == expected;
}

//
bool hasNoPipe(const string& str) {
    return str.find('|') == string::npos;
}