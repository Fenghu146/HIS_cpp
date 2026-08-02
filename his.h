#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "his_config.h"
using namespace std;

//1、类的定义
class Patient {
public:
    string id;
    string name;
    int age;
    string gender;
    string phone;
    string id_card;
    long long balance = 0;
    string pin;
    float insurance_rate;

    Patient() = default;

    friend ostream& operator<<(ostream& os,const Patient& p) {
        os  << "ID:" << p.id
            << "| 姓名:" << p.name
            << "| 年龄:" << p.age
            << "| 性别:" << p.gender
            << "| 余额:" << p.balance
            << "| 医保比例:" <<(p.insurance_rate * 100) << "%";
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

    friend ostream& operator<<(ostream& os,const Doctor& d) {
        os  << "ID:" << d.id
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
    int doctor_count = 0;

    Department() = default;

    friend ostream& operator<<(ostream& os,const Department& d) {
        os  << "科室:" << d.name
            << "|ID:" << d.id
            << "|医生数:" << d.doctor_count;
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
    string dept_names;

    Drug() = default;

    friend ostream& operator<<(ostream& os,const Drug& d) {
        os << "ID: " << d.id
           << " | 通用名: " << d.general_name
           << " | 商品名: " << (d.trade_name.empty() ? "-" : d.trade_name)
           << " | 别名: " << (d.alias.empty() ? "-" : d.alias)
           << " | 单价: " << d.price
           << " | 库存: " << d.stock
           << " | 阈值: " << d.warning_stock
           << " | 科室: " << d.dept_names;
        return os;
    }
};

//2、通用链表定义
using PatientList = vector<unique_ptr<Patient>>;
using DoctorList = vector<unique_ptr<Doctor>>;
using DepartmentList = vector<unique_ptr<Department>>;
using DrugList = vector<unique_ptr<Drug>>;

//3、通用函数声明

//按ID查找节点
template<typename T>
T* findById(vector<unique_ptr<T>>& list,const string& id);

//按ID删除节点
template<typename T>
bool removeById(vector<unique_ptr<T>>& list,const string& id);

//清空
template<typename T>
void clearList(vector<unique_ptr<T>>& list);
