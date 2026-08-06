#pragma once

#include "../model/entity.h"
#include "../config/his_config.h"
#include <fstream>
#include <sstream>

template<typename T>
class DataManager {
public:
    string filename;
    vector<unique_ptr<T>> list;

    explicit DataManager(const string& fname) : filename(fname) {} //阻止编译器自动把 string 转成 DataManager 对象

    virtual void load() = 0;
    virtual void save() = 0;
    virtual ~DataManager() = default;
};


