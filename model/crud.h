#pragma once

#include "entity.h"

template<typename T>
T* findById(vector<unique_ptr<T>>& list, const string& id) {
    for (auto& item : list) {
        if (item->id == id) return item.get();
    }
    return nullptr;
}

template<typename T>
bool removeById(vector<unique_ptr<T>>& list, const string& id) {
    for (auto it = list.begin(); it != list.end(); ++it) {
        if ((*it)->id == id) {
            list.erase(it);
            return true;
        }
    }
    return false;
}

template<typename T>
void forEach(vector<unique_ptr<T>>& list, void (*func)(T*)) {
    for (auto& item : list) {
        func(item.get());
    }
}

template<typename T>
void clearList(vector<unique_ptr<T>>& list) {
    list.clear();
}

template<typename T>
T* addNode(vector<unique_ptr<T>>& list, unique_ptr<T> node) {
    T* raw = node.get();
    list.push_back(std::move(node));
    return raw;
}
