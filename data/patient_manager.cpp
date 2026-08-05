#include "patient_manager.h"

int PatientManager::next_id = 1;

#include "../model/crud.h"
#include "../utils/input.h"
#include "../utils/validator.h"

void PatientManager::load() {
    ifstream in(filename);
    if (!in.is_open()) return;

    string line;
    while (getline(in,line)) {
        stringstream ss(line);
        string field;

        auto p = make_unique<Patient>();
        getline(ss,p->id,'|');
        getline(ss,p->name,'|');
        getline(ss,field,'|'); p->age = stoi(field);
        getline(ss,p->gender,'|');
        getline(ss,p->phone,'|');
        getline(ss,p->id_card,'|');
        getline(ss,field,'|'); p->balance = stoll(field);
        getline(ss,p->pin,'|');
        getline(ss,field,'|'); p->insurance_rate = stof(field);

        list.push_back(std::move(p));
    }
    in.close();

    // 更新 next_id 为当前最大ID+1
    for (auto& p : list) {
        if (p->id.length() > 1 && p->id[0] == ID_PATIENT) {
            int num = stoi(p->id.substr(1));
            if (num >= next_id) next_id = num + 1;
        }
    }
}

void PatientManager::save() {
    ofstream out(filename);
    if (!out.is_open()) return;

    for (auto& p : list) {
        out << p->id << '|'
            << p->name << '|'
            << p->age << '|'
            << p->gender << '|'
            << p->phone << '|'
            << p->id_card << '|'
            << p->balance << '|'
            << p->pin << '|'
            << p->insurance_rate << '\n';
    }
    out.close();
}

void PatientManager::registerPatient() {
    auto p = make_unique<Patient>();

    cout << "请输入姓名："; inputLine(p->name); cerr<<"[DBG] name=("<<p->name<<")"<<endl;
    cout << "请输入年龄:"; p->age = getValidChoice(0, 150); cerr<<"[DBG] age="<<p->age<<endl;
    cout << "请输入性别:"; inputLine(p->gender); cerr<<"[DBG] gender=("<<p->gender<<")"<<endl;
    cout << "请输入手机号:"; inputLine(p->phone);
    if (!isValidPhone(p->phone)) { cout << "[错误] 手机号格式不正确！\n"; return; }

    cout << "请输入身份证:"; inputLine(p->id_card);
    if (!isValidIDCard(p->id_card)) { cout << "[错误] 身份证格式不正确！\n"; return; }
    cout << "请输入6位密码:"; inputLine(p->pin);

    string newId = generateId();
    p->id = newId;
    p->balance = 0;
    p->insurance_rate = DEFAULT_INSURANCE;

    list.push_back(std::move(p));
    save();

    cout << "注册成功！ID：" << newId <<endl;
}

Patient *PatientManager::findPatient(const string &id) {
    return findById(list,id);
}

bool PatientManager::deletePatient(const string &id) {
    bool ok = removeById(list, id);
    if (ok) save();
    return ok;
}

void PatientManager::listPatient() {
    if (list.empty()) {
        cout << "暂无患者记录。\n";
        return;
    }
    for (auto& p : list) {
        cout << *p << endl;
    }
}

bool PatientManager::recharge(const string &id, long long amount) {
    Patient* p = findPatient(id);
    if (!p) {
        cout << "[错误] 未找到患者" << id <<endl;
        return false;
    }
    if (amount <= 0) {
        cout << "[错误] 充值金额必须大于0" << endl;
        return false;
    }
    p->balance += amount;
    save();
    cout << "充值成功！当前余额：" << p->balance <<endl;
    return true;
}
