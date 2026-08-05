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

    Drug() = default;

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

    // 显示用：ID → 名称（需要外部传入转换函数）
    friend ostream& displayWithDeptNames(ostream& os, const Drug& d,
        const string& (*idToName)(const string&)) {
        os << "ID: " << d.id
           << " | 通用名: " << d.general_name
           << " | 商品名: " << (d.trade_name.empty() ? "-" : d.trade_name)
           << " | 别名: " << (d.alias.empty() ? "-" : d.alias)
           << " | 单价: " << d.price
           << " | 库存: " << d.stock
           << " | 阈值: " << d.warning_stock;
        os << " | 科室: ";
        if (d.dept_ids.empty()) {
            os << "通用";
        } else {
            vector<string> ids;
            stringstream ss(d.dept_ids);
            string token;
            while (getline(ss, token, ',')) {
                string name = idToName(token);
                ids.push_back(name.empty() ? token : name);
            }
            for (size_t i = 0; i < ids.size(); i++) {
                if (i > 0) os << ",";
                os << ids[i];
            }
        }
        return os;
    }
};

using PatientList = vector<unique_ptr<Patient>>;
using DoctorList = vector<unique_ptr<Doctor>>;
using DepartmentList = vector<unique_ptr<Department>>;
using DrugList = vector<unique_ptr<Drug>>;
