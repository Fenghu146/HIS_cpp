#include "his.h"

//查找
template<typename T>
T* findById(vector<unique_ptr<T>>& list,const string& id) {
    for (auto& item : list) {
        if (item->id == id) return item.get();
    }
    return nullptr;
}

//删除
template<typename T>
bool removeById(vector<unique_ptr<T>>& list,const string& id ) {
    for (auto it = list.begin();it != list.end();++it) {
        if ((*it)->id == id) {
            list.erase(it);
            return true;
        }
    }
    return false;
}

//遍历 对每个元素执行回调函数
//func: 函数指针，接受T*参数（对象裸指针），返回void
//调用方传入自定义操作，如打印、修改等
//例: forEach(patients, [](Patient* p){ cout << p->name; });
template<typename T>
void forEach(vector<unique_ptr<T>>& list,void (*func)(T*)) {
    for (auto& item : list) {
        func(item.get());  // 借出裸指针传给回调，不转让所有权
    }
}

//清空
template<typename T>
void clearList(vector<unique_ptr<T>>& list) {
    list.clear();
}

//添加 转移所有权进容器，返回裸指针供调用方使用
//参数按值传unique_ptr，调用方必须显式move，明确所有权转移
template<typename T>
T* addNode(vector<unique_ptr<T>>& list,unique_ptr<T> node) {
    T* raw = node.get();       // 先取裸指针（move后node变nullptr）
    list.push_back(move(node)); // 转移所有权进容器
    return raw;                 // 返回裸指针，调用方可立即操作
}

//模版实例化 模板定义在.cpp而非头文件，需显式实例化才能被其他编译单元链接
//格式: template 返回类型 函数名<具体类型>(参数类型);
//作用: 为每种类型生成具体代码，避免链接时"未定义引用"错误
//替代: 将模板定义移到his.h，则无需此行

//--- 查找 ---
template Patient* findById<Patient>(PatientList&, const std::string&);
template Doctor* findById<Doctor>(DoctorList&, const std::string&);
template Department* findById<Department>(DepartmentList&, const std::string&);
template Drug* findById<Drug>(DrugList&, const std::string&);

//--- 删除 ---
template bool removeById<Patient>(PatientList&, const std::string&);
template bool removeById<Doctor>(DoctorList&, const std::string&);
template bool removeById<Department>(DepartmentList&, const std::string&);
template bool removeById<Drug>(DrugList&, const std::string&);

//--- 遍历 ---
template void forEach<Patient>(PatientList&, void (*)(Patient*));
template void forEach<Doctor>(DoctorList&, void (*)(Doctor*));
template void forEach<Department>(DepartmentList&, void (*)(Department*));
template void forEach<Drug>(DrugList&, void (*)(Drug*));

//--- 清空 ---
template void clearList<Patient>(PatientList&);
template void clearList<Doctor>(DoctorList&);
template void clearList<Department>(DepartmentList&);
template void clearList<Drug>(DrugList&);

//--- 添加 ---
template Patient* addNode<Patient>(PatientList&, std::unique_ptr<Patient>);
template Doctor* addNode<Doctor>(DoctorList&, std::unique_ptr<Doctor>);
template Department* addNode<Department>(DepartmentList&, std::unique_ptr<Department>);
template Drug* addNode<Drug>(DrugList&, std::unique_ptr<Drug>);