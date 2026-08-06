#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
using namespace std;

class Patient {
public:
    string id;
    string name;
    int age = 0;
    string gender;
    string phone;
    string id_card;
    long long balance = 0;
    string pin;
    float insurance_rate = 0.0f;

    Patient() = default;

    friend ostream& operator<<(ostream& os, const Patient& p) {
        os << "ID:" << p.id
           << "| 姓名:" << p.name
           << "| 年龄:" << p.age
           << "| 性别:" << p.gender
           << "| 余额:" << p.balance
           << "| 医保比例:" << (p.insurance_rate * 100) << "%";
        return os;
    }
};

class Doctor {
public:
    string id;
    string name;
    string dept_name;
    string specialty;
    string account;
    string password;

    Doctor() = default;

    friend ostream& operator<<(ostream& os, const Doctor& d) {
        os << "ID:" << d.id
           << "|姓名:" << d.name
           << "|账号:" << d.account
           << "|科室:" << d.dept_name
           << "|擅长领域:" << d.specialty;
        return os;
    }
};

class Department {
public:
    string id;
    string name;
    string description;
    string director_name;
    int doctor_count = 0;
    string location;

    Department() = default;

    friend ostream& operator<<(ostream& os, const Department& d) {
        os << "ID:" << d.id
           << "|科室:" << d.name
           << "|负责人:" << d.director_name
           << "|位置:" << d.location
           << "|医生数:" << d.doctor_count
           << "|简介:" << d.description;
        return os;
    }
};

class Drug {
public:
    string id;
    string general_name;
    string trade_name;
    string alias;
    float price = 0.0f;
    int stock = 0;
    int warning_stock = 0;
    string dept_ids;        // 科室ID，逗号分隔；空串表示通用药品
    int max_stock = 0;      // 最大库存，用于预留自动计算预警阈值接口

    Drug() = default; //默认构造函数

    friend ostream& operator<<(ostream& os, const Drug& d) {
        os << "ID: " << d.id
           << " | 通用名: " << d.general_name
           << " | 商品名: " << (d.trade_name.empty() ? "-" : d.trade_name)
           << " | 别名: " << (d.alias.empty() ? "-" : d.alias)
           << " | 单价: " << d.price
           << " | 库存: " << d.stock
           << " | 阈值: " << d.warning_stock
           << " | 科室ID: " << (d.dept_ids.empty() ? "通用" : d.dept_ids);
        return os;
    }

    // 显示用：把 Drug 的科室 ID 转成名称后输出，便于可读性
    friend ostream& displayWithDeptNames(ostream& os, const Drug& d,
        const string& (*idToName)(const string&)) { //函数指针参数，用于把科室ID转成名称
        os << "ID: " << d.id
           << " | 通用名: " << d.general_name
           << " | 商品名: " << (d.trade_name.empty() ? "-" : d.trade_name)
           << " | 别名: " << (d.alias.empty() ? "-" : d.alias)
           << " | 单价: " << d.price
           << " | 库存: " << d.stock
           << " | 阈值: " << d.warning_stock;
        os << " | 科室: ";
        if (d.dept_ids.empty()) {
            os << "通用"; // 空串表示通用药品
        } else {
            vector<string> ids;
            stringstream ss(d.dept_ids);
            string token;
            while (getline(ss, token, ',')) {
                string name = idToName(token); // 通过外部函数把 ID 转成名称
                ids.push_back(name.empty() ? token : name); // 若无法转换则显示原 ID
            }
            for (size_t i = 0; i < ids.size(); i++) {
                if (i > 0) os << ","; // 多个科室用逗号分隔
                os << ids[i];
            }
        }
        return os;
    }
};

// 床位
class Bed {
public:
    string id;              // B1, B2, ...
    string bed_number;      // 床号，如 "A-101"
    string dept_id;         // 所属科室ID
    string type;            // 普通/急诊/重症
    int daily_price = 0;    // 每日费用（分）
    string status;          // 空闲/占用/清洁中
    string patient_id;      // 当前占用患者ID（空=空闲）

    Bed() = default;

    friend ostream& operator<<(ostream& os, const Bed& b) {
        os << "ID:" << b.id
           << "| 床号:" << b.bed_number
           << "| 科室ID:" << b.dept_id
           << "| 类型:" << b.type
           << "| 日费:" << b.daily_price
           << "| 状态:" << b.status
           << "| 患者:" << (b.patient_id.empty() ? "-" : b.patient_id);
        return os;
    }
};

// 挂号单
class Appointment {
public:
    string id;              // AP1, AP2, ...
    string patient_id;
    string doctor_id;
    string dept_id;
    int fee = 0;            // 挂号费（分）
    string status;          // 待诊/已接诊/已完成/已取消
    string create_time;

    Appointment() = default;

    friend ostream& operator<<(ostream& os, const Appointment& a) {
        os << "ID:" << a.id
           << "| 患者:" << a.patient_id
           << "| 医生:" << a.doctor_id
           << "| 科室:" << a.dept_id
           << "| 费用:" << a.fee
           << "| 状态:" << a.status
           << "| 时间:" << a.create_time;
        return os;
    }
};

// 病历
class MedicalRecord {
public:
    string id;              // MR1, MR2, ...
    string appointment_id;  // 关联挂号单
    string patient_id;
    string doctor_id;
    string complaint;       // 主诉
    string diagnosis;       // 诊断
    string orders;          // 医嘱
    string create_time;
    bool need_hospitalize = false;  // 是否建议住院

    MedicalRecord() = default;

    friend ostream& operator<<(ostream& os, const MedicalRecord& r) {
        os << "ID:" << r.id
           << "| 挂号:" << r.appointment_id
           << "| 患者:" << r.patient_id
           << "| 医生:" << r.doctor_id
           << "| 主诉:" << r.complaint
           << "| 诊断:" << r.diagnosis
           << "| 医嘱:" << r.orders
           << "| 时间:" << r.create_time
           << "| 建议住院:" << (r.need_hospitalize ? "是" : "否");
        return os;
    }
};

// 处方
class Prescription {
public:
    string id;              // RX1, RX2, ...
    string record_id;       // 关联病历
    string patient_id;
    string doctor_id;
    int total_amount = 0;   // 总金额（分）
    string status;          // 未缴费/已缴费/已取药
    string create_time;

    Prescription() = default;

    friend ostream& operator<<(ostream& os, const Prescription& p) {
        os << "ID:" << p.id
           << "| 病历:" << p.record_id
           << "| 患者:" << p.patient_id
           << "| 医生:" << p.doctor_id
           << "| 总金额:" << p.total_amount
           << "| 状态:" << p.status
           << "| 时间:" << p.create_time;
        return os;
    }
};

// 处方明细
class PrescriptionItem {
public:
    string id;              // PI1, PI2, ...
    string prescription_id;
    string drug_id;
    int quantity = 0;
    string usage;           // 用法用量
    int amount = 0;         // 该项金额（分）

    PrescriptionItem() = default;

    friend ostream& operator<<(ostream& os, const PrescriptionItem& item) {
        os << "  明细ID:" << item.id
           << "| 处方:" << item.prescription_id
           << "| 药品:" << item.drug_id
           << "| 数量:" << item.quantity
           << "| 用法:" << item.usage
           << "| 金额:" << item.amount << "分";
        return os;
    }
};

using PatientList = vector<unique_ptr<Patient>>; //类型别名，表示患者列表，使用 unique_ptr 管理内存
using DoctorList = vector<unique_ptr<Doctor>>;
using DepartmentList = vector<unique_ptr<Department>>;
using DrugList = vector<unique_ptr<Drug>>;
using BedList = vector<unique_ptr<Bed>>;
using AppointmentList = vector<unique_ptr<Appointment>>;
using MedicalRecordList = vector<unique_ptr<MedicalRecord>>;
using PrescriptionList = vector<unique_ptr<Prescription>>;
using PrescriptionItemList = vector<unique_ptr<PrescriptionItem>>;
