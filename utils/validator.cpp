#include "validator.h"
#include <algorithm>
#include <cctype>
#include <iostream>
using namespace std;

bool isValidNumber(const string& str) {
    if (str.empty()) return false;
    return all_of(str.begin(), str.end(), ::isdigit);
}

bool isValidPhone(const string& phone) {
    if (phone.size() != 11) return false;
    if (phone[0] != '1') return false;
    return all_of(phone.begin(), phone.end(), ::isdigit);
}

bool isValidIDCard(const string& id_card) {
    if (id_card.size() != 18) return false;

    for (size_t i = 0; i < 17; i++) {
        if (!isdigit(static_cast<unsigned char>(id_card[i]))) return false;
    }

    char last = id_card[17];
    if (!isdigit(static_cast<unsigned char>(last)) && last != 'X' && last != 'x') return false;

    static const int weights[17] = {7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2};
    static const char check_chars[] = "10X98765432";

    int sum = 0;
    for (size_t i = 0; i < 17; i++) {
        sum += (id_card[i] - '0') * weights[i];
    }

    char expected = check_chars[sum % 11];
    char actual = (last >= 'a' && last <= 'z') ? (last - 'a' + 'A') : last;

    if (actual != expected) {
        cout << "    [提示] 校验位应为 " << expected << "，实际输入为 " << actual << "\n";
    }
    return actual == expected;
}

bool hasNoPipe(const string& str) {
    return str.find('|') == string::npos;
}
