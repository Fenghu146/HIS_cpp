#include "doctor_manager.h"

int DoctorManager::next_id = 1;

#include "../model/crud.h"
#include "../utils/input.h"
#include "../utils/validator.h"
#include "../utils/hash.h"

void DoctorManager::load() {
    ifstream in(filename);
    if (!in.is_open())
        return;

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
        // 兼容旧格式：读取剩余部分，判断是否包含 password_hash
        string remainder;
        getline(ss, remainder);
        if (!remainder.empty()) {
            // 新格式：有 password_hash
            d->password_hash = remainder;
        } else {
            // 旧格式：无 password_hash
            d->password_hash = "";
        }

        list.push_back(std::move(d));
    }
    in.close();

    for (auto& d : list) {
        if (d->id.length() > 1 && d->id[0] == ID_DOCTOR) {
            int num = stoi(d->id.substr(1));
            if (num >= next_id)
                next_id = num + 1;
        }
    }
}

void DoctorManager::save() {
    ofstream out(filename);
    if (!out.is_open())
        return;

    for (auto& d : list) {
        out << d->id << '|' << d->name << '|' << d->dept_name << '|' << d->specialty << '|'
            << d->account << '|' << (d->password_hash.empty() ? d->password : std::string()) << '|'
            << d->password_hash << '\n';
    }
    out.close();
}

void DoctorManager::registerDoctor(const std::function<bool(const std::string&)>& deptExists) {
    auto d = make_unique<Doctor>();

    cout << "请输入姓名：";
    inputLine(d->name);
    while (d->name.empty()) {
        cout << "[错误] 姓名不能为空，请重新输入：";
        inputLine(d->name);
    }
    cout << "请输入科室：";
    inputLine(d->dept_name);
    if (deptExists) {
        while (!deptExists(d->dept_name)) {
            cout << "[错误] 科室「" << d->dept_name << "」不存在，请先创建该科室或重新输入：";
            inputLine(d->dept_name);
        }
    }
    cout << "请输入擅长领域：";
    inputLine(d->specialty);
    cout << "请输入账号：";
    inputLine(d->account);
    while (d->account.empty() || findByAccount(d->account) != nullptr) {
        cout << (d->account.empty() ? "[错误] 账号不能为空，请重新输入："
                                    : "[错误] 账号已被注册，请重新输入：");
        inputLine(d->account);
    }
    cout << "请输入密码：";
    inputLine(d->password);
    while (d->password.empty()) {
        cout << "[错误] 密码不能为空，请重新输入：";
        inputLine(d->password);
    }
    d->password_hash = sha256(d->password);  // 统一 sha256 存储，明文不落盘
    d->password.clear();

    string newId = generateId();
    d->id = newId;

    list.push_back(std::move(d));
    save();

    cout << "注册成功！ID：" << newId << endl;
}

Doctor* DoctorManager::findDoctor(const string& id) {
    return findById(list, id);
}

Doctor* DoctorManager::findByAccount(const string& account) {
    for (auto& d : list) {
        if (d->account == account)
            return d.get();
    }
    return nullptr;
}

bool DoctorManager::deleteDoctor(const string& id) {
    bool ok = removeById(list, id);
    if (ok)
        save();
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

bool DoctorManager::validatePassword(const string& account, const string& password) {
    Doctor* d = nullptr;
    for (auto& doc : list) {
        if (doc->account == account) {
            d = doc.get();
            break;
        }
    }
    if (!d)
        return false;

    if (!d->password_hash.empty()) {
        return sha256(password) == d->password_hash;
    } else {
        if (d->password == password) {
            d->password_hash = sha256(password);
            save();
            return true;
        }
        return false;
    }
}

bool DoctorManager::changePassword(const string& id, const string& old_pwd, const string& new_pwd) {
    Doctor* d = nullptr;
    for (auto& doc : list) {
        if (doc->id == id) {
            d = doc.get();
            break;
        }
    }
    if (!d) {
        cout << "[错误] 未找到医生 " << id << endl;
        return false;
    }
    // 验证旧密码（支持哈希或明文）
    bool valid = !d->password_hash.empty() ? (sha256(old_pwd) == d->password_hash)
                                           : (d->password == old_pwd);
    if (!valid) {
        cout << "[错误] 原密码不正确！" << endl;
        return false;
    }
    d->password.clear();
    d->password_hash = sha256(new_pwd);
    save();
    cout << "密码修改成功！" << endl;
    return true;
}

int DoctorManager::countDoctorsInDept(const string& dept_name) const {
    int count = 0;
    for (auto& d : list) {
        if (d->dept_name == dept_name)
            count++;
    }
    return count;
}
