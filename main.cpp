#include <iostream>
#include "data/patient_manager.h"
#include "utils/input.h"
using namespace std;

void showPatientMenu(PatientManager& mgr) {
    while (true) {
        cout << "\n=== 患者管理 ===\n";
        cout << "1. 注册患者\n";
        cout << "2. 查找患者\n";
        cout << "3. 删除患者\n";
        cout << "4. 患者列表\n";
        cout << "5. 充值\n";
        cout << "0. 返回\n";

        int choice = getValidChoice(0, 5);
        switch (choice) {
            case 1: mgr.registerPatient(); break;
            case 2: {
                cout << "请输入患者ID:";
                string id; inputLine(id);
                Patient* p = mgr.findPatient(id);
                if (p) cout << *p << endl;
                else cout << "未找到患者\n";
                break;
            }
            case 3: {
                cout << "请输入患者ID:";
                string id; inputLine(id);
                if (mgr.deletePatient(id)) cout << "删除成功\n";
                else cout << "未找到患者\n";
                break;
            }
            case 4: mgr.listPatient(); break;
            case 5: {
                cout << "请输入患者ID:";
                string id; inputLine(id);
                cout << "请输入充值金额（分）:";
                long long amt; cin >> amt; ClearInputBuffer();
                mgr.recharge(id, amt);
                break;
            }
            case 0: return;
        }
    }
}

int main() {
    PatientManager mgr;
    mgr.load();

    cout << "=== 医院信息管理系统 ===\n";
    showPatientMenu(mgr);

    return 0;
}
