#include "drug_manager.h"

int DrugManager::next_id = 1;

#include "../model/crud.h"
#include "../utils/input.h"
#include "../utils/validator.h"
#include <algorithm>

// 辅助：去除字符串前后空格
static string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t");
    return s.substr(start, end - start + 1);
}

// 辅助：分割逗号分隔的ID
static vector<string> splitIds(const string& s) {
    vector<string> result;
    stringstream ss(s);
    string token;
    while (getline(ss, token, ',')) {
        token = trim(token);
        if (!token.empty()) result.push_back(token);
    }
    return result;
}

void DrugManager::load() {
    ifstream in(filename);
    if (!in.is_open()) return;

    string line;
    while (getline(in, line)) {
        stringstream ss(line);
        string field;

        auto d = make_unique<Drug>();
        getline(ss, d->id, '|');
        getline(ss, d->general_name, '|');
        getline(ss, d->trade_name, '|');
        getline(ss, d->alias, '|');
        getline(ss, field, '|'); d->price = stof(field);
        getline(ss, field, '|'); d->stock = stoi(field);
        getline(ss, field, '|'); d->warning_stock = stoi(field);
        getline(ss, d->dept_ids, '|');
        getline(ss, field, '|'); d->max_stock = stoi(field);

        list.push_back(std::move(d));
    }
    in.close();

    for (auto& d : list) {
        if (d->id.length() > 1 && d->id[0] == ID_DRUG) {
            int num = stoi(d->id.substr(1));
            if (num >= next_id) next_id = num + 1;
        }
    }
}

void DrugManager::save() {
    ofstream out(filename);
    if (!out.is_open()) return;

    for (auto& d : list) {
        out << d->id << '|'
            << d->general_name << '|'
            << d->trade_name << '|'
            << d->alias << '|'
            << d->price << '|'
            << d->stock << '|'
            << d->warning_stock << '|'
            << d->dept_ids << '|'
            << d->max_stock << '\n';
    }
    out.close();
}

void DrugManager::printDeptList(const DepartmentManager& deptMgr) {
    if (deptMgr.list.empty()) {
        cout << "  （暂无科室，将注册为通用药品）\n";
        return;
    }
    cout << "  可选科室列表：\n";
    int idx = 1;
    for (auto& d : deptMgr.list) {
        cout << "    " << idx++ << ". " << d->name << " (" << d->id << ")\n";
    }
}

string DrugManager::getDeptIdByIndex(const DepartmentManager& deptMgr, int index) {
    if (index < 1 || index > static_cast<int>(deptMgr.list.size())) return "";
    return deptMgr.list[index - 1]->id;
}

void DrugManager::displayDrug(const Drug& d, const DepartmentManager& deptMgr) {
    cout << "ID: " << d.id
         << " | 通用名: " << d.general_name
         << " | 商品名: " << (d.trade_name.empty() ? "-" : d.trade_name)
         << " | 别名: " << (d.alias.empty() ? "-" : d.alias)
         << " | 单价: " << d.price
         << " | 库存: " << d.stock
         << " | 阈值: " << d.warning_stock
         << " | 科室: ";
    if (d.dept_ids.empty()) {
        cout << "通用";
    } else {
        vector<string> ids = splitIds(d.dept_ids);
        for (size_t i = 0; i < ids.size(); i++) {
            if (i > 0) cout << ",";
            string name = deptMgr.getNameById(ids[i]);
            cout << (name.empty() ? ids[i] : name);
        }
    }
    cout << endl;
}

void DrugManager::registerDrug(DepartmentManager& deptMgr) {
    auto d = make_unique<Drug>();

    cout << "请输入通用名："; inputLine(d->general_name);
    cout << "请输入商品名（可空）："; inputLine(d->trade_name);
    cout << "请输入别名（可空）："; inputLine(d->alias);

    cout << "请输入单价（分）：";
    string priceStr; inputLine(priceStr);
    d->price = stof(priceStr) / 100.0f;

    cout << "请输入初始库存：";
    string stockStr; inputLine(stockStr);
    d->stock = stoi(stockStr);

    cout << "请输入最大库存（可空，用于预警计算）：";
    string maxStr; inputLine(maxStr);
    d->max_stock = maxStr.empty() ? 0 : stoi(maxStr);

    cout << "请输入预警阈值（0=自动计算）：";
    string warnStr; inputLine(warnStr);
    if (warnStr.empty() || warnStr == "0") {
        d->warning_stock = calcWarningStock(d->max_stock);
    } else {
        d->warning_stock = stoi(warnStr);
    }

    // 科室选择：打印编号列表，输入序号
    cout << "请选择适用科室（输入序号，逗号分隔，可空=通用药品）：\n";
    printDeptList(deptMgr);
    cout << "  请输入序号：";
    string idxInput; inputLine(idxInput);

    if (!idxInput.empty()) {
        vector<string> idxParts = splitIds(idxInput);
        vector<string> validIds;
        for (auto& part : idxParts) {
            int idx = stoi(part);
            string deptId = getDeptIdByIndex(deptMgr, idx);
            if (deptId.empty()) {
                cout << "[错误] 序号 " << idx << " 无效！\n";
                return;
            }
            validIds.push_back(deptId);
        }
        // 拼接ID
        string idsStr;
        for (size_t i = 0; i < validIds.size(); i++) {
            if (i > 0) idsStr += ",";
            idsStr += validIds[i];
        }
        d->dept_ids = idsStr;
    }

    d->id = generateId();
    string newId = d->id;

    list.push_back(std::move(d));
    save();

    cout << "注册成功！ID：" << newId << endl;
}

Drug *DrugManager::findDrug(const string &id) {
    return findById(list, id);
}

bool DrugManager::deleteDrug(const string &id) {
    Drug* d = findDrug(id);
    if (!d) {
        cout << "未找到药品\n";
        return false;
    }
    if (d->stock > 0) {
        cout << "[错误] 该药品还有库存（" << d->stock << "），无法删除！\n";
        return false;
    }
    bool ok = removeById(list, id);
    if (ok) save();
    return ok;
}

void DrugManager::listDrug(DepartmentManager& deptMgr) {
    if (list.empty()) {
        cout << "暂无药品记录。\n";
        return;
    }
    for (auto& d : list) {
        displayDrug(*d, deptMgr);
    }
}

bool DrugManager::stockIn(const string &id, int amount) {
    Drug* d = findDrug(id);
    if (!d) {
        cout << "未找到药品\n";
        return false;
    }
    if (amount <= 0) {
        cout << "[错误] 入库数量必须大于0\n";
        return false;
    }
    d->stock += amount;
    save();
    cout << "入库成功！当前库存：" << d->stock << endl;
    return true;
}

bool DrugManager::stockOut(const string &id, int amount) {
    Drug* d = findDrug(id);
    if (!d) {
        cout << "未找到药品\n";
        return false;
    }
    if (amount <= 0) {
        cout << "[错误] 出库数量必须大于0\n";
        return false;
    }
    if (amount > d->stock) {
        cout << "[错误] 库存不足！当前库存：" << d->stock << endl;
        return false;
    }
    d->stock -= amount;
    save();
    cout << "出库成功！当前库存：" << d->stock << endl;
    return true;
}

void DrugManager::warningList(DepartmentManager& deptMgr) {
    bool hasWarning = false;
    for (auto& d : list) {
        if (d->stock <= d->warning_stock) {
            displayDrug(*d, deptMgr);
            hasWarning = true;
        }
    }
    if (!hasWarning) {
        cout << "暂无库存预警药品。\n";
    }
}

void DrugManager::modifyDrug(DepartmentManager& deptMgr) {
    cout << "请输入要修改的药品ID：";
    string id; inputLine(id);
    Drug* d = findDrug(id);
    if (!d) {
        cout << "未找到药品\n";
        return;
    }

    cout << "当前信息：\n  ";
    displayDrug(*d, deptMgr);

    cout << "\n=== 修改菜单 ===\n";
    cout << "1. 修改通用名\n";
    cout << "2. 修改商品名\n";
    cout << "3. 修改别名\n";
    cout << "4. 修改单价\n";
    cout << "5. 修改预警阈值\n";
    cout << "6. 修改科室关联\n";
    cout << "0. 取消\n";

    int choice = getValidChoice(0, 6);
    switch (choice) {
        case 1: {
            cout << "请输入新通用名："; inputLine(d->general_name); break;
        }
        case 2: {
            cout << "请输入新商品名："; inputLine(d->trade_name); break;
        }
        case 3: {
            cout << "请输入新别名："; inputLine(d->alias); break;
        }
        case 4: {
            cout << "请输入新单价（分）：";
            string priceStr; inputLine(priceStr);
            d->price = stof(priceStr) / 100.0f;
            break;
        }
        case 5: {
            cout << "请输入新预警阈值（0=自动计算）：";
            string warnStr; inputLine(warnStr);
            if (warnStr.empty() || warnStr == "0") {
                d->warning_stock = calcWarningStock(d->max_stock);
            } else {
                d->warning_stock = stoi(warnStr);
            }
            break;
        }
        case 6: {
            modifyDeptIds(*d, deptMgr);
            break;
        }
        case 0: return;
    }
    save();
    cout << "修改成功！\n";
}

void DrugManager::modifyDeptIds(Drug& d, DepartmentManager& deptMgr) {
    cout << "当前科室关联：";
    if (d.dept_ids.empty()) {
        cout << "通用\n";
    } else {
        vector<string> ids = splitIds(d.dept_ids);
        for (size_t i = 0; i < ids.size(); i++) {
            if (i > 0) cout << ",";
            cout << deptMgr.getNameById(ids[i]);
        }
        cout << "\n";
    }

    cout << "请重新选择科室（输入序号，逗号分隔，可空=通用药品）：\n";
    printDeptList(deptMgr);
    cout << "  请输入序号：";
    string idxInput; inputLine(idxInput);

    if (idxInput.empty()) {
        d.dept_ids = "";
        return;
    }

    vector<string> idxParts = splitIds(idxInput);
    vector<string> validIds;
    for (auto& part : idxParts) {
        int idx = stoi(part);
        string deptId = getDeptIdByIndex(deptMgr, idx);
        if (deptId.empty()) {
            cout << "[错误] 序号 " << idx << " 无效，已跳过\n";
            continue;
        }
        validIds.push_back(deptId);
    }

    string idsStr;
    for (size_t i = 0; i < validIds.size(); i++) {
        if (i > 0) idsStr += ",";
        idsStr += validIds[i];
    }
    d.dept_ids = idsStr;
}
