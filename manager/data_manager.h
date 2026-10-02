#pragma once

#include "../model/entity.h"
#include "../config/his_config.h"
#include <fstream>
#include <sstream>

template <typename T>
class DataManager {
public:
    string filename;
    vector<unique_ptr<T>> list;
    bool dirty = false;

    explicit DataManager(const string& fname) : filename(fname) {}

    void markDirty() { dirty = true; }

    virtual void load() = 0;
    virtual void save() = 0;
    virtual ~DataManager() = default;
};
