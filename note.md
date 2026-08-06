# HIS_cpp 开发计划

> 最后更新: 2026-08-05
> 数据持久化: 保持文本文件（data/*.txt），暂不迁移

---

## 一、项目现状

### 已完成模块
- **患者管理**: 注册/查找/删除/列表/充值
- **医生管理**: 注册/查找/删除/列表
- **科室管理**: 注册/查找/删除/列表（含级联检查：有医生/药品关联则禁止删除）
- **药品管理**: 注册/查找/删除/入库/出库/库存预警/修改信息

### 架构
```
config/his_config.h      常量配置（ID前缀、文件路径、业务默认值）
model/entity.h           实体类（Patient/Doctor/Department/Drug）
model/crud.h             CRUD 模板工具函数
data/data_manager.h      数据管理模板基类（load/save 接口）
data/patient_manager     患者数据管理
data/docto   医生数据管理
data/dept_manager        科室数据管理
data/drug_manager        药品数据管理
utils/input.h/cpp        输入处理工具
utils/validator.h/cpp    格式校验（手机号、身份证校验位）
main.cpp                 菜单入口 + 各模块菜单逻辑
```

### 待解决问题
- 残留调试输出 `cerr << "[DBG]"` 散布各 manager（patient 3处, doctor 5处, dept 4处）
- 无身份认证，所有操作匿名
- 无业务流程（挂号/病历/处方/结算）

---

## 二、开发路线图

### 阶段 1: 身份认证体系 ← 当前阶段
**目标**: 让系统安全可控，不同角色有不同操作权限

#### 已确定设计决策

| 决策项 | 选择 | 说明 |
|--------|------|------|
| 启动流程 | 先选角色再登录 | 启动→选角色(患者/医生/管理员)→登录验证→角色菜单 |
| 菜单结构 | 角色专属菜单 | 患者/医生/管理员各自看到不同菜单 |
| 会话管理 | 全局 Session 单例 | 一个全局 Session 对象保存当前登录身份 |

#### 角色专属菜单设计

**患者菜单：**
```
1. 挂号（选科室→选医生）
2. 我的病历
3. 我的处方
4. 充值
5. 修改密码
0. 退出登录
```

**医生菜单：**
```
1. 待诊患者列表
2. 接诊（写病历+开处方）
3. 修改密码
0. 退出登录
```

**管理员菜单：**（即当前主菜单，保持不变）
```
1. 患者管理
2. 医生管理
3. 科室管理
4. 药品管理
0. 退出
```

#### Session 单例设计草案
```cpp
// service/session.h
class Session {
    static Session* instance;
    string role;       // "patient" / "doctor" / "admin" / ""
    string user_id;
    string user_name;
public:
    static Session* getInstance();
    void login(const string& role, const string& id, const string& name);
    void logout();
    bool isLoggedIn() const;
    string getRole() const;
    string getUserId() const;
    string getUserName() const;
};
```

#### 实现步骤（Plan）

| # | 操作 | 文件 | 说明 |
|---|------|------|------|
| 1 | 新建 | `service/session.h` | Session 单例类 |
| 2 | 新建 | `utils/hash.h/cpp` | SHA256 哈希函数 |
| 3 | 新建 | `service/auth.h/cpp` | 登录验证：loginPatient/loginDoctor/loginAdmin |
| 4 | 新建 | `service/log.h/cpp` | 操作日志写入 `data/log.txt` |
| 5 | 重写 | `main.cpp` | 启动选角色→登录→角色菜单 |
| 6 | 修改 | `data/patient_manager.cpp` | 删 3 处 DBG，新增 validatePin() |
| 7 | 修改 | `data/doctor_manager.cpp` | 删 5 处 DBG，新增 validatePassword() |
| 8 | 修改 | `data/dept_manager.cpp` | 删 4 处 DBG |
| 9 | 修改 | `CMakeLists.txt` | 添加 auth.cpp、log.cpp、hash.cpp |
| 10 | 兼容 | auth 层 | 过渡期兼容明文/哈希，登录成功自动回写哈希 |

#### 新增文件清单
- `service/session.h` — 会话单例
- `service/auth.h` / `auth.cpp` — 认证服务
- `service/log.h` / `log.cpp` — 日志服务
- `utils/hash.h` / `hash.cpp` — SHA256 哈希

#### 密码迁移策略
- 现有数据中的 pin/password 是明文存储
- 认证时先比哈希，哈希不匹配则比明文（兼容期）
- 明文比对成功后，自动将哈希值写回文件
- 这样无需一次性迁移脚本，逐步完成

---

### 阶段 2: 核心业务闭环（HIS 灵魂）
**目标**: 形成完整就医流程

```
患者登录 → 挂号选科室/医生 → 候诊 → 医生接诊(写病历) → 开处方 → 结算取药
```

#### 任务清单

| # | 任务 | 说明 | 状态 |
|---|------|------|------|
| 2.1 | 挂号 | 选科室→选医生→生成挂号单，扣除挂号费 | ⬜ |
| 2.2 | 病历 | 医生查看当前挂号患者，记录主诉/诊断/医嘱 | ⬜ |
| 2.3 | 处方 | 医生开药→检查库存→生成处方明细 | ⬜ |
| 2.4 | 结算 | 汇总费用（挂号+药品），医保报销，余额扣款，库存扣减 | ⬜ |
| 2.5 | 挂号队列 | 医生查看自己当前待诊患者列表 | ⬜ |
| 2.6 | 就诊历史 | 患者查看自己的历史病历和处方 | ⬜ |

#### 新增实体与文件
- `model/appointment.h` — 挂号单（患者ID、医生ID、科室ID、挂号费、状态）
- `model/medical_record.h` — 病历（挂号ID、主诉、诊断、医嘱）
- `model/prescription.h` — 处方（挂号ID、总金额）+ 处方明细（药品ID、数量、用量）
- `data/appointment_manager.h/cpp`
- `data/medical_record_manager.h/cpp`
- `data/prescription_manager.h/cpp`
- `service/billing.h/cpp` — 结算服务（医保计算、余额扣款）
- `service/registration.h/cpp` — 挂号服务

#### 业务规则草案
- 挂号费: 1000分（config 中 REGISTRATION_FEE）
- 医保报销: 默认 70%（config 中 DEFAULT_INSURANCE）
- 结算流程: 处方总额 → 医保报销 → 余额支付 → 扣库存
- 挂号状态: 待诊/已接诊/已完成/已取消

---

### 阶段 3: 排班 + 管理员后台

#### 任务清单

| # | 任务 | 说明 | 状态 |
|---|------|------|------|
| 3.1 | 医生排班 | 设置出诊时间（星期+时段），挂号时只能选有排班的医生 | ⬜ |
| 3.2 | 管理员统计 | 日挂号量/收入/药品消耗 Top N | ⬜ |
| 3.3 | 数据导出 | 报表导出 CSV | ⬜ |
| 3.4 | 采购建议 | 库存低于阈值时生成采购建议单 | ⬜ |

#### 新增文件
- `model/schedule.h` — 排班（医生ID、星期、时段）
- `data/schedule_manager.h/cpp`
- `service/admin.h/cpp` — 管理员服务

---

### 阶段 4: 代码质量提升（持续进行）

| # | 任务 | 说明 | 状态 |
|---|------|------|------|
| 4.1 | 清除调试输出 | 删除所有 `cerr << "[DBG]"` 语句 | ✅ 阶段1顺带完成 |
| 4.2 | 统一异常处理 | 文件打开失败、数据格式错误的统一处理 | ⬜ |
| 4.3 | 输入校验增强 | 姓名不能含 `|`、空字符串检查等 | ⬜ |
| 4.4 | 单元测试 | validator、billing 等纯逻辑写测试 | ⬜ |
| 4.5 | 引入 service 层 | 业务逻辑从 main.cpp 分离到 service 层 | ⬜ |

---

## 三、执行顺序建议

```
阶段1（登录）1-2天 → 阶段2（核心闭环）5-7天 → 阶段3（排班+管理）2-3天 → 阶段4（持续）
```

---

## 四、设计原则

1. **新增不破坏已有**：已有的四个 CRUD 模块不重构，新功能在现有基础上叠加
2. **文本文件持久化**：新实体同样用 `|` 分隔文本文件存储
3. **先跑通再优化**：优先让流程端到端跑起来，再回头做异常处理和边界情况
4. **渐进式迁移**：密码哈希通过兼容逻辑逐步完成，无需一次性迁移
