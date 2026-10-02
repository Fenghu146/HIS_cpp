#include "auth.h"
#include "../utils/hash.h"
#include "../config/his_config.h"

bool AuthService::loginPatient(PatientManager& mgr, const string& id, const string& pin) {
    Patient* p = mgr.findPatient(id);
    if (!p) return false;

    bool valid;
    if (!p->pin_hash.empty()) {
        valid = sha256(pin) == p->pin_hash;
    } else {
        valid = (p->pin == pin);
        if (valid) {
            p->pin_hash = sha256(pin);
            p->pin.clear();
            mgr.markDirty();
        }
    }
    return valid;
}

bool AuthService::loginDoctor(DoctorManager& mgr, const string& account, const string& password) {
    Doctor* d = nullptr;
    for (auto& doc : mgr.list) {
        if (doc->account == account) {
            d = doc.get();
            break;
        }
    }
    if (!d) return false;

    bool valid;
    if (!d->password_hash.empty()) {
        valid = sha256(password) == d->password_hash;
    } else {
        valid = (d->password == password);
        if (valid) {
            d->password_hash = sha256(password);
            d->password.clear();
            mgr.markDirty();
        }
    }
    return valid;
}

// 管理员登录：比对 config 中的静态凭证
bool AuthService::loginAdmin(const string& username, const string& password) {
    return username == ADMIN_USERNAME && sha256(password) == ADMIN_PASSWORD_HASH;
}
