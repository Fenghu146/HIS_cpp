#include "auth.h"
#include "../utils/hash.h"
#include "../config/his_config.h"

// 患者登录：
//   - pin_hash 非空 → 比对哈希
//   - pin_hash 空   → 比对明文，成功后自动迁移为哈希
bool AuthService::loginPatient(PatientManager& mgr, const string& id, const string& pin) {
    Patient* p = mgr.findPatient(id);
    if (!p) return false;

    if (!p->pin_hash.empty()) {
        // 已迁移：直接比哈希
        return sha256(pin) == p->pin_hash;
    } else {
        // 未迁移：比明文，成功后自动迁移
        if (p->pin == pin) {
            p->pin_hash = sha256(pin);
            mgr.save();
            return true;
        }
        return false;
    }
}

// 医生登录：逻辑同患者
bool AuthService::loginDoctor(DoctorManager& mgr, const string& account, const string& password) {
    // 医生用 account（账号）登录，不是 ID
    Doctor* d = nullptr;
    for (auto& doc : mgr.list) {
        if (doc->account == account) {
            d = doc.get();
            break;
        }
    }
    if (!d) return false;

    if (!d->password_hash.empty()) {
        return sha256(password) == d->password_hash;
    } else {
        if (d->password == password) {
            d->password_hash = sha256(password);
            mgr.save();
            return true;
        }
        return false;
    }
}

// 管理员登录：比对 config 中的静态凭证
bool AuthService::loginAdmin(const string& username, const string& password) {
    return username == ADMIN_USERNAME && password == ADMIN_PASSWORD;
}
