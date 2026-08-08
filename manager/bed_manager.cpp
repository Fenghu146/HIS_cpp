#include "bed_manager.h"

int BedManager::next_id = 1;

#include "../model/crud.h"
#include "../utils/input.h"
#include "../config/his_config.h"

void BedManager::load() {
    // 打开文件读取流，filename 来自基类 DataManager
    ifstream in(filename);
    if (!in.is_open()) return; // 文件不存在或无法打开时直接返回，不报错

    string line;
    // 按行读取文件，每一行对应一个 Bed 记录
    while (getline(in, line)) {
        // 先把整行放到 stringstream，再按分隔符解析字段
        stringstream ss(line);
        string field;

        // 创建一个 Bed 对象，并用 unique_ptr 管理它的内存
        // make_unique 会返回一个 std::unique_ptr<Bed>，避免你手动写 new/delete
        auto b = make_unique<Bed>();
        // 按照约定的字段顺序读取数据：id | bed_number | dept_id | type | daily_price | status | patient_id
        getline(ss, b->id, '|');
        getline(ss, b->bed_number, '|');
        getline(ss, b->dept_id, '|');
        getline(ss, b->type, '|');
        getline(ss, field, '|'); b->daily_price = stoi(field); // 字符串转整数
        getline(ss, b->status, '|');
        getline(ss, b->patient_id, '|');

        // 把解析好的 Bed 对象加入内存列表
        // 使用 std::move 将 unique_ptr 的所有权移动到 list 中，
        // 因为 unique_ptr 不能复制，只能移动
        list.push_back(std::move(b));
    }
    in.close(); // 读取完毕后关闭文件

    // 读取完成后，更新 next_id，保证生成的新床位 ID 不会与已有 ID 冲突
    for (auto& b : list) {
        // 只处理床位 ID，格式应为 B+数字
        if (b->id.length() > 1 && b->id[0] == ID_BED) {
            // 取出数字部分，例如 B12 -> 12
            int num = stoi(b->id.substr(1));
            // 如果已有编号比 next_id 大，就把 next_id 设为它的下一个值
            if (num >= next_id) next_id = num + 1;
        }
    }
}

void BedManager::save() {
    // 打开文件写入流，注意这里会覆盖原文件内容
    ofstream out(filename);
    if (!out.is_open()) return; // 如果打开失败则不继续写入

    // 遍历内存中的床位列表，把每个床位写成一行文本
    for (auto& b : list) {
        out << b->id << '|'              // 床位 ID
            << b->bed_number << '|'      // 床号
            << b->dept_id << '|'         // 科室 ID
            << b->type << '|'            // 床位类型
            << b->daily_price << '|'     // 每日价格
            << b->status << '|'          // 当前状态
            << b->patient_id << '\n';   // 患者 ID，最后以换行结束
    }
    out.close(); // 写入完毕后关闭文件
}

void BedManager::registerBed(DepartmentManager& deptMgr) {
    auto b = make_unique<Bed>();

    cout << "请输入床号（如 A-101）：";
    inputLine(b->bed_number);

    // 检查床号是否重复
    if (findBedByNumber(b->bed_number)) {
        cout << "[错误] 床号 " << b->bed_number << " 已存在！\n";
        return;
    }

    // 选择科室
    if (deptMgr.list.empty()) {
        cout << "[错误] 暂无科室，请先创建科室！\n";
        return;
    }
    cout << "可选科室：\n";
    int idx = 1;
    for (auto& d : deptMgr.list) {
        cout << "  " << idx++ << ". " << d->name << " (" << d->id << ")\n";
    }
    cout << "请选择科室序号：";
    int deptIdx = getValidChoice(1, static_cast<int>(deptMgr.list.size()));
    b->dept_id = deptMgr.list[deptIdx - 1]->id;

    // 选择类型
    cout << "床位类型：\n";
    cout << "  1. 普通（日费 " << BED_FEE_NORMAL << " 分）\n";
    cout << "  2. 急诊（日费 " << BED_FEE_EMERGENCY << " 分）\n";
    cout << "  3. 重症（日费 " << BED_FEE_ICU << " 分）\n";
    cout << "请选择类型：";
    int typeChoice = getValidChoice(1, 3);
    switch (typeChoice) {
        case 1: b->type = "普通"; b->daily_price = BED_FEE_NORMAL; break;
        case 2: b->type = "急诊"; b->daily_price = BED_FEE_EMERGENCY; break;
        case 3: b->type = "重症"; b->daily_price = BED_FEE_ICU; break;
    }

    b->id = generateId();
    b->status = BedStatus::FREE;
    b->patient_id = "";

    list.push_back(std::move(b));
    save();

    cout << "床位添加成功！ID：" << list.back()->id << " 床号：" << list.back()->bed_number << endl;
}

Bed* BedManager::findBed(const string& id) {
    return findById(list, id);
}

Bed* BedManager::findBedByNumber(const string& bed_number) {
    for (auto& b : list) {
        if (b->bed_number == bed_number) return b.get();
    }
    return nullptr;
}

bool BedManager::deleteBed(const string& id) {
    Bed* b = findBed(id);
    if (!b) {
        cout << "未找到床位\n";
        return false;
    }
    if (b->status != BedStatus::FREE) {
        cout << "[错误] 床位当前状态为 " << b->status << "，无法删除！\n";
        return false;
    }
    bool ok = removeById(list, id);
    if (ok) save();
    return ok;
}

void BedManager::listBed() {
    if (list.empty()) {
        cout << "暂无床位记录。\n";
        return;
    }
    for (auto& b : list) {
        cout << *b << endl;
    }
}

void BedManager::listBedByDept(const string& dept_id) {
    bool found = false;
    for (auto& b : list) {
        if (b->dept_id == dept_id) {
            cout << *b << endl;
            found = true;
        }
    }
    if (!found) cout << "该科室暂无床位。\n";
}

bool BedManager::changeStatus(const string& id, const string& new_status) {
    Bed* b = findBed(id);
    if (!b) {
        cout << "未找到床位\n";
        return false;
    }
    b->status = new_status;
    save();
    return true;
}

bool BedManager::occupyBed(const string& id, const string& patient_id) {
    Bed* b = findBed(id);
    if (!b) {
        cout << "未找到床位\n";
        return false;
    }
    if (b->status != BedStatus::FREE) {
        cout << "[错误] 床位 " << b->bed_number << " 当前" << b->status << "，无法占用！\n";
        return false;
    }
    b->status = BedStatus::OCCUPIED;
    b->patient_id = patient_id;
    save();
    return true;
}

bool BedManager::releaseBed(const string& id) {
    Bed* b = findBed(id);
    if (!b) {
        cout << "未找到床位\n";
        return false;
    }
    b->status = BedStatus::CLEANING;
    b->patient_id = "";
    save();
    return true;
}

int BedManager::countBedsByDept(const string& dept_id) const {
    int count = 0;
    for (auto& b : list) {
        if (b->dept_id == dept_id) count++;
    }
    return count;
}

int BedManager::countAvailableBedsByDept(const string& dept_id) const {
    int count = 0;
    for (auto& b : list) {
        if (b->dept_id == dept_id && b->status == BedStatus::FREE) count++;
    }
    return count;
}
