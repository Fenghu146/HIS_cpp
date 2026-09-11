#pragma once

#include "entity.h"

template<typename T>
T* findById(vector<unique_ptr<T>>& list, const string& id) {
    for (auto& item : list) {
        if (item->id == id) return item.get(); //返回指针，get() 方法返回 unique_ptr 管理的原始指针（T*），便于直接访问对象
    }
    return nullptr;
}

template<typename T>
const T* findById(const vector<unique_ptr<T>>& list, const string& id) {
    for (auto& item : list) {
        if (item->id == id) return item.get();
    }
    return nullptr;
}

template<typename T>
bool removeById(vector<unique_ptr<T>>& list, const string& id) {
    for (auto it = list.begin(); it != list.end(); ++it) { //遍历列表，it 是迭代器，指向 unique_ptr<T> 对象
        if ((*it)->id == id) {
            list.erase(it); //删除元素，unique_ptr 会自动释放内存
            return true;
        }
    }
    return false;
}


