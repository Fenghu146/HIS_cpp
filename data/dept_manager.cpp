#include "dept_manager.h"
#include "drug_manager.h"

int DepartmentManager::next_id = 1;

#include "../model/crud.h"
#include "../utils/input.h"

void DepartmentManager::load() {
    ifstream in(filename);
    if (!in.is_open()) return;

    string line;
    while (getline(in, line)) {
        stringstream ss(line);
        string field;

        auto d = make_unique<Department>();
        getline(ss, d->id, '|');
        getline(ss, d->name, '|');
        getline(ss, d->description, '|');
        getline(ss, d->director_name, '|');
        getline(ss, d->location, '|');

        list.push_back(std::move(d));
    }
    in.close();

    for (auto& d : list) {
        if (d->id.length() > 1 && d->id[0] == ID_DEPT) {
            int num = stoi(d->id.substr(1));
            if (num >= next_id) next_id = num + 1;
        }
    }
}

void DepartmentManager::save() {
    ofstream out(filename);
    if (!out.is_open()) return;

    for (auto& d : list) {
        out << d->id << '|'
            << d->name << '|'
            << d->description << '|'
            << d->director_name << '|'
            << d->location << '\n';
    }
    out.close();
}

void DepartmentManager::registerDepartment() {
    auto d = make_unique<Department>();

    cout << "请输入科室名称："; inputLine(d->name); cerr << "[DBG] name=(" << d->name << ")" << endl;
    cout << "请输入科室简介："; inputLine(d->description); cerr << "[DBG] description=(" << d->description << ")" << endl;
    cout << "请输入负责人姓名："; inputLine(d->director_name); cerr << "[DBG] director=(" << d->director_name << ")" << endl;
    cout << "请输入科室位置："; inputLine(d->location); cerr << "[DBG] location=(" << d->location << ")" << endl;

    d->id = generateId();
    string newId = d->id;

    list.push_back(std::move(d));
    save();

    cout << "注册成功！ID：" << newId << endl;
}

Department *DepartmentManager::findDepartment(const string &id) {
    return findById(list, id);
}

bool DepartmentManager::deleteDepartment(const string &id, DoctorManager &docMgr, DrugManager &drugMgr) {
    Department* d = findDepartment(id);
    if (!d) {
        cout << "未找到科室\n";
        return false;
    }
    if (docMgr.countDoctorsInDept(d->name) > 0) {
        cout << "[错误] 该科室下还有医生，无法删除！\n";
        return false;
    }
    if (hasDrugAssociation(id, drugMgr)) {
        cout << "[错误] 该科室下还有药品关联，无法删除！\n";
        return false;
    }
    bool ok = removeById(list, id);
    if (ok) save();
    return ok;
}

void DepartmentManager::listDepartment(DoctorManager &docMgr) {
    if (list.empty()) {
        cout << "暂无科室记录。\n";
        return;
    }
    for (auto& d : list) {
        d->doctor_count = docMgr.countDoctorsInDept(d->name);
        cout << *d << endl;
    }
}

bool DepartmentManager::exists(const string& id) const {
    return findById(list, id) != nullptr;
}

string DepartmentManager::getNameById(const string& id) const {
    const Department* d = findById(list, id);
    return d ? d->name : "";
}

bool DepartmentManager::hasDrugAssociation(const string& dept_id, DrugManager& drugMgr) {
    for (auto& drug : drugMgr.list) {
        if (drug->dept_ids.empty()) continue;
        stringstream ss(drug->dept_ids);
        string token;
        while (getline(ss, token, ',')) {
            if (token == dept_id) return true;
        }
    }
    return false;
}
