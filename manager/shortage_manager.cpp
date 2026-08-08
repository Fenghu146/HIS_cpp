#include "shortage_manager.h"
#include "../model/crud.h"

int ShortageManager::next_id = 1;

void ShortageManager::load() {
    ifstream in(filename);
    if (!in.is_open()) return;

    string line;
    while (getline(in, line)) {
        stringstream ss(line);
        string field;

        auto s = make_unique<Shortage>();
        getline(ss, s->id, '|');
        getline(ss, s->drug_id, '|');
        getline(ss, s->drug_name, '|');
        getline(ss, field, '|'); s->required_amount = stoi(field);
        getline(ss, field, '|'); s->current_stock = stoi(field);
        getline(ss, s->prescription_id, '|');
        getline(ss, s->triggered_by, '|');
        getline(ss, field, '|');
        s->urgency = (field == "紧急") ? ShortageUrgency::URGENT : ShortageUrgency::NORMAL;
        getline(ss, s->status, '|');
        getline(ss, s->create_time, '|');

        list.push_back(std::move(s));
    }
    in.close();

    for (auto& s : list) {
        if (s->id.length() > 1 && s->id[0] == ID_SHORTAGE) {
            int num = stoi(s->id.substr(1));
            if (num >= next_id) next_id = num + 1;
        }
    }
}

void ShortageManager::save() {
    ofstream out(filename);
    if (!out.is_open()) return;

    for (auto& s : list) {
        out << s->id << '|'
            << s->drug_id << '|'
            << s->drug_name << '|'
            << s->required_amount << '|'
            << s->current_stock << '|'
            << s->prescription_id << '|'
            << s->triggered_by << '|'
            << (s->urgency == ShortageUrgency::URGENT ? "紧急" : "普通") << '|'
            << s->status << '|'
            << s->create_time << '\n';
    }
    out.close();
}

Shortage* ShortageManager::addShortage(const string& drug_id, const string& drug_name,
                                       int required, int stock, const string& prescription_id,
                                       const string& triggered_by, ShortageUrgency urgency) {
    auto s = make_unique<Shortage>();
    s->id = generateId();
    s->drug_id = drug_id;
    s->drug_name = drug_name;
    s->required_amount = required;
    s->current_stock = stock;
    s->prescription_id = prescription_id;
    s->triggered_by = triggered_by;
    s->urgency = urgency;
    s->status = ShortageStatus::PENDING;
    s->create_time = "2026-08-08";

    Shortage* raw = s.get();
    list.push_back(std::move(s));
    save();
    return raw;
}

void ShortageManager::listAll() {
    if (list.empty()) {
        cout << "暂无缺药记录。\n";
        return;
    }
    for (auto& s : list) {
        cout << *s << endl;
    }
}

void ShortageManager::listByStatus(const string& status) {
    bool found = false;
    for (auto& s : list) {
        if (s->status == status) {
            cout << *s << endl;
            found = true;
        }
    }
    if (!found) cout << "暂无状态为「" << status << "」的缺药记录。\n";
}

void ShortageManager::listPending() {
    auto pending = getPending();
    if (pending.empty()) {
        cout << "暂无待处理缺药记录。\n";
        return;
    }
    // 按紧急度排序：紧急在前
    sort(pending.begin(), pending.end(), compareByUrgency);
    int idx = 1;
    for (auto* s : pending) {
        cout << "  " << idx++ << ". " << *s << endl;
    }
}

Shortage* ShortageManager::findShortage(const string& id) {
    return findById(list, id);
}

bool ShortageManager::markHandled(const string& id, const string& action) {
    Shortage* s = findShortage(id);
    if (!s) {
        cout << "未找到缺药记录 " << id << endl;
        return false;
    }
    s->status = action;  // 调用方传入 "已补货" 或 "已处理"
    save();
    return true;
}

vector<Shortage*> ShortageManager::getPending() {
    vector<Shortage*> result;
    for (auto& s : list) {
        if (s->status == ShortageStatus::PENDING) {
            result.push_back(s.get());
        }
    }
    return result;
}

bool ShortageManager::hasPendingForDrug(const string& drug_id) {
    for (auto& s : list) {
        if (s->drug_id == drug_id && s->status == ShortageStatus::PENDING) {
            return true;
        }
    }
    return false;
}

bool ShortageManager::compareByUrgency(const Shortage* a, const Shortage* b) {
    // 紧急排前面
    if (a->urgency != b->urgency) {
        return a->urgency == ShortageUrgency::URGENT;
    }
    // 同紧急度按时间（ID序号）
    return a->id < b->id;
}
