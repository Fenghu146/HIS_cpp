#include "doctor_manager.h"

int DoctorManager::next_id = 1;

#include "../model/crud.h"
#include "../utils/input.h"
#include "../utils/validator.h"

void DoctorManager::load() {
    ifstream in(filename);
    if (!in.is_open()) return;

    string line;
    while (getline(in, line)) {
        stringstream ss(line);
        string field;

        auto d = make_unique<Doctor>();
        getline(ss, d->id, '|');
        getline(ss, d->name, '|');
        getline(ss, d->dept_name, '|');
        getline(ss, d->specialty, '|');
        getline(ss, d->account, '|');
        getline(ss, d->password, '|');

        list.push_back(std::move(d));
    }
    in.close();

    for (auto& d : list) {
        if (d->id.length() > 1 && d->id[0] == ID_DOCTOR) {
            int num = stoi(d->id.substr(1));
            if (num >= next_id) next_id = num + 1;
        }
    }
}

void DoctorManager::save() {
    ofstream out(filename);
    if (!out.is_open()) return;

    for (auto& d : list) {
        out << d->id << '|'
            << d->name << '|'
            << d->dept_name << '|'
            << d->specialty << '|'
            << d->account << '|'
            << d->password << '\n';
    }
    out.close();
}

void DoctorManager::registerDoctor() {
    auto d = make_unique<Doctor>();

    cout << "请输入姓名："; inputLine(d->name); cerr << "[DBG] name=(" << d->name << ")" << endl;
    cout << "请输入科室："; inputLine(d->dept_name); cerr << "[DBG] dept_name=(" << d->dept_name << ")" << endl;
    cout << "请输入擅长领域："; inputLine(d->specialty); cerr << "[DBG] specialty=(" << d->specialty << ")" << endl;
    cout << "请输入账号："; inputLine(d->account); cerr << "[DBG] account=(" << d->account << ")" << endl;
    cout << "请输入密码："; inputLine(d->password); cerr << "[DBG] password=(" << d->password << ")" << endl;

    string newId = generateId();
    d->id = newId;

    list.push_back(std::move(d));
    save();

    cout << "注册成功！ID：" << newId << endl;
}

Doctor *DoctorManager::findDoctor(const string &id) {
    return findById(list, id);
}

bool DoctorManager::deleteDoctor(const string &id) {
    bool ok = removeById(list, id);
    if (ok) save();
    return ok;
}

void DoctorManager::listDoctor() {
    if (list.empty()) {
        cout << "暂无医生记录。\n";
        return;
    }
    for (auto& d : list) {
        cout << *d << endl;
    }
}

int DoctorManager::countDoctorsInDept(const string& dept_name) const {
    int count = 0;
    for (auto& d : list) {
        if (d->dept_name == dept_name) count++;
    }
    return count;
}
