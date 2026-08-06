#pragma once

#include "../model/entity.h"
#include "../manager/patient_manager.h"
#include "../manager/doctor_manager.h"
#include "../manager/dept_manager.h"
#include "../manager/appointment_manager.h"

// 挂号服务：选科室→选医生→扣费→生成挂号单
class RegistrationService {
public:
    static bool registerPatient(
        PatientManager& patientMgr,
        DoctorManager& doctorMgr,
        DepartmentManager& deptMgr,
        AppointmentManager& appointmentMgr
    );

    // 显示某科室下可挂号的医生列表，返回选择的医生ID
    static string selectDoctor(DoctorManager& doctorMgr, const string& dept_name);
};
