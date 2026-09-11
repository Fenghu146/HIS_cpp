<div align="center">

# 医院信息管理系统 (HIS_cpp)

**基于 C++20 的纯标准库、控制台版医院信息管理系统**

覆盖「挂号 → 就诊 → 处方 → 缴费 → 取药 → 住院」完整就医闭环，采用清晰的分层架构与对象化设计。

![C++](https://img.shields.io/badge/C%2B%2B-20-blue)
![CMake](https://img.shields.io/badge/CMake-3.16%2B-green)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey)
![Dependencies](https://img.shields.io/badge/Dependencies-None-brightgreen)
![License](https://img.shields.io/badge/License-See%20below-orange)

</div>

---

## 目录

- [项目简介](#项目简介)
- [主要功能](#主要功能)
- [技术架构](#技术架构)
- [目录结构](#目录结构)
- [环境要求](#环境要求)
- [快速开始](#快速开始)
- [使用说明](#使用说明)
- [数据持久化](#数据持久化)
- [配置说明](#配置说明)
- [业务规则](#业务规则)
- [安全性设计](#安全性设计)
- [代码质量与规范](#代码质量与规范)
- [已知限制与路线图](#已知限制与路线图)
- [贡献指南](#贡献指南)
- [许可证](#许可证)

---

## 项目简介

HIS_cpp（Hospital Information System）是一个使用 **C++20** 编写的医院信息管理系统，以命令行交互方式运行，**不依赖任何第三方库**（密码哈希为自实现的 SHA-256）。

项目的核心目标是在一个可运行、可扩展的代码库中完整实现医院的典型业务链路，并借此实践面向对象设计、分层架构、泛型编程与现代 C++ 资源管理（`std::unique_ptr` / RAII）等工程能力。

**设计取向：**

- **分层清晰**：配置层 / 模型层 / 数据访问层 / 业务服务层 / 交互层职责分明，业务逻辑不散落在入口函数中。
- **可扩展**：新增一个实体只需派生 `DataManager<T>` 并实现 `load()` / `save()`。
- **渐进式演进**：密码从明文平滑迁移至哈希存储，无需一次性数据迁移脚本。
- **零依赖**：仅使用 C++ 标准库，任何具备 C++20 支持的编译器均可构建。

---

## 主要功能

系统通过**角色选择 + 登录**进入，不同角色拥有独立菜单与权限边界。

### 角色与权限

| 角色 | 登录方式 | 主要能力 |
|------|----------|----------|
| 患者 | 患者 ID + 6 位密码 | 挂号、查看病历/处方、充值、缴费、取药、住院、修改密码 |
| 医生 | 账号 + 密码 | 查看待诊列表、接诊写病历、开处方、查看个人病历/处方记录、缺药登记 |
| 管理员 | 账号 + 密码 | 患者/医生/科室/药品/床位/药房/住院的全量管理 |

### 功能模块

**1. 身份认证与会话**

- 基于角色的登录鉴权，`Session` 单例保存当前登录身份
- 密码使用 SHA-256 哈希存储，支持明文 → 哈希的自动渐进迁移
- 登录 / 登出事件写入操作日志

**2. 基础档案管理（管理员）**

| 模块 | 功能 |
|------|------|
| 患者管理 | 注册（手机号 / 身份证校验）、查找、删除、列表、充值、修改密码 |
| 医生管理 | 注册、查找、删除、列表、修改密码 |
| 科室管理 | 注册、查找、删除、列表；删除前级联校验（存在医生或药品关联则拒绝） |
| 药品管理 | 注册、查找、删除、列表、入库、出库、库存预警、修改信息；支持多科室关联 |
| 床位管理 | 添加、查找、删除、列表、修改状态（空闲 / 占用 / 清洁中） |

**3. 核心业务闭环**

```
患者挂号 ──► 医生接诊 ──► 开处方 ──► 患者缴费 ──► 取药 ──► (必要时) 办理住院
 (扣挂号费)   (写病历)     (生成明细)   (扣余额)    (扣库存)      (扣首日日费)
```

- **挂号**：选择科室 → 选择该科室医生 → 校验余额 → 扣除挂号费 → 生成挂号单
- **就诊**：医生查看本人待诊队列 → 接诊 → 书写病历（主诉 / 诊断 / 医嘱 / 是否建议住院）→ 可选开处方
- **处方**：逐项选择药品、校验库存、计算金额、生成处方与明细
- **缴费**：选择未缴费处方 → 校验余额 → 扣款 → 状态置为「已缴费」
- **取药**：库存预检查 → 全部充足则出库并置「已取药」；否则**自动登记缺药**并中止
- **缺药管理**：待办清单（按紧急度排序）、主动上报、补货入库处理

**4. 住院管理**

- 依据病历中的「建议住院」标记与关联医生确定建议科室
- 展示可用床位 → 扣除首日日费 → 占用床位
- 管理员可查看当前住院患者列表

---

## 技术架构

### 分层结构

```
┌──────────────────────────────────────────────────────────────┐
│  交互层 (UI)          main.cpp                                │
│  角色路由 · 各级菜单 · 输入输出                                │
└───────────────────────────┬──────────────────────────────────┘
                            │ 调用
┌───────────────────────────▼──────────────────────────────────┐
│  业务服务层 (Service)   service/                              │
│  auth · session · log · registration · consultation           │
│  payment · shortage_service · hospitalization_service         │
└───────────────────────────┬──────────────────────────────────┘
                            │ 调用
┌───────────────────────────▼──────────────────────────────────┐
│  数据访问层 (Manager)   manager/                              │
│  DataManager<T> 抽象基类                                      │
│  patient · doctor · dept · drug · bed · appointment           │
│  medical_record · prescription(+item) · shortage              │
└───────────────────────────┬──────────────────────────────────┘
                            │ 使用
┌───────────────────────────▼──────────────────────────────────┐
│  模型层 (Model)         model/                                │
│  entity.h · shortage.h · crud.h（泛型 CRUD 模板）             │
├──────────────────────────────────────────────────────────────┤
│  工具层 (Utils)         utils/    input · validator · hash · datetime │
├──────────────────────────────────────────────────────────────┤
│  配置层 (Config)        config/his_config.h                    │
│  常量 · ID 前缀 · 文件路径 · 业务默认值 · 状态字面量            │
└──────────────────────────────────────────────────────────────┘
```

### 各层职责

| 层 | 目录 | 职责 |
|----|------|------|
| 配置层 | `config/` | 集中定义长度、ID 前缀、数据文件路径、业务默认值、状态常量，避免魔法值散落 |
| 模型层 | `model/` | 定义实体类与列表类型别名；`crud.h` 提供 `findById` / `removeById` 等泛型工具 |
| 数据层 | `manager/` | 每个实体一个 Manager，负责文本文件的载入 / 持久化与基础增删改查 |
| 服务层 | `service/` | 编排跨实体的业务流程、认证、日志与会话 |
| 工具层 | `utils/` | 输入容错、格式校验、安全数值解析、SHA-256、时间工具 |
| 交互层 | `main.cpp` | 角色选择、登录流程、各角色菜单与子菜单 |

### 关键设计

- **模板方法模式**：`DataManager<T>` 定义 `load()` / `save()` 纯虚接口与 `list` / `filename` / `dirty` 公共状态，子类仅实现数据编解码。
- **单例模式**：`Session` 全局保存当前登录身份（角色 / ID / 名称），禁止拷贝。
- **RAII 与智能指针**：所有实体以 `std::unique_ptr<T>` 存放于 `vector`，容器析构自动释放，无手动 `delete`。
- **渐进式密码迁移**：`AuthService` 优先比对哈希；若记录仅有明文则比对明文，成功后写回哈希并标记 `dirty`，由调用方持久化。

### 一次完整就诊的数据流

```
RegistrationService          AppointmentManager     PatientManager
   选科室/选医生 ─────────►  addAppointment()  ────►  扣挂号费
                                                     │
ConsultationService                                  ▼
   接诊 ─► MedicalRecordManager.addRecord()   ─► 病历落盘
        └► PrescriptionManager.addPrescription() + PrescriptionItemManager.addItem()
                                                     │
PaymentService                                       ▼
   缴费 ─► 扣余额 + 更新处方状态 → 已缴费
   取药 ─► 库存预检 → 出库 + 状态 → 已取药
            └─ 库存不足 → ShortageManager.addShortage()（缺药登记）
```

---

## 目录结构

```
HIS_cpp/
├── CMakeLists.txt              # 构建脚本（CMake ≥ 3.16）
├── main.cpp                    # 程序入口：角色路由 + 各角色菜单
├── README.md                   # 项目文档
├── .clang-format               # 统一代码格式配置
├── .editorconfig               # 编辑器统一约定
├── .gitignore                  # 忽略构建产物与运行时数据
├── note.md                     # 开发计划（阶段性路线图）
├── 项目笔记.md                 # 早期设计草稿
│
├── config/
│   └── his_config.h            # 全局常量：ID 前缀 / 文件路径 / 业务默认值 / 状态字面量
│
├── model/
│   ├── entity.h                # 实体：Patient / Doctor / Department / Drug / Bed
│   │                           #       Appointment / MedicalRecord / Prescription / PrescriptionItem
│   ├── shortage.h              # 实体：Shortage（缺药记录）+ 紧急度枚举
│   └── crud.h                  # 泛型工具：findById / removeById
│
├── manager/                    # 数据访问层
│   ├── data_manager.h          # DataManager<T> 抽象基类（load / save / dirty）
│   ├── patient_manager.{h,cpp}         # 患者
│   ├── doctor_manager.{h,cpp}          # 医生（含密码校验）
│   ├── dept_manager.{h,cpp}            # 科室（含级联删除保护）
│   ├── drug_manager.{h,cpp}            # 药品（库存 / 预警 / 多科室关联）
│   ├── bed_manager.{h,cpp}             # 床位（占用 / 释放 / 统计）
│   ├── appointment_manager.{h,cpp}     # 挂号单
│   ├── medical_record_manager.{h,cpp}  # 病历
│   ├── prescription_manager.{h,cpp}    # 处方 + 处方明细（含 O(1) 明细索引）
│   └── shortage_manager.{h,cpp}        # 缺药记录
│
├── service/                    # 业务服务层
│   ├── session.h               # 登录会话单例
│   ├── auth.{h,cpp}            # 认证：患者 / 医生 / 管理员登录
│   ├── log.{h,cpp}             # 操作日志（登录 / 登出）
│   ├── registration.{h,cpp}    # 挂号服务
│   ├── consultation.{h,cpp}    # 就诊服务（写病历 + 开处方）
│   ├── payment.{h,cpp}         # 缴费与取药服务
│   ├── shortage_service.{h,cpp}# 缺药登记与处理
│   └── hospitalization_service.{h,cpp} # 住院办理与住院列表
│
├── utils/
│   ├── input.{h,cpp}           # 行输入 / 确认 / 菜单选项读取
│   ├── validator.{h,cpp}       # 手机号、身份证校验位、安全数值解析
│   ├── hash.h                  # 自实现 SHA-256（仅头文件）
│   └── datetime.h              # 时间戳工具（统一 create_time 格式）
│
└── data/                       # 运行时自动创建，已被 .gitignore 忽略
    ├── patient.txt / doctor.txt / dept.txt / drug.txt / bed.txt
    ├── appointment.txt / record.txt / prescription.txt / prescription_item.txt
    ├── shortage.txt
    └── log.txt
```

---

## 环境要求

| 项目 | 要求 |
|------|------|
| 编译器 | 支持 **C++20**（GCC ≥ 10、Clang ≥ 12、MSVC ≥ 19.29） |
| 构建工具 | **CMake ≥ 3.16** |
| 第三方库 | 无 |
| 操作系统 | Windows / Linux / macOS |

> 开发与验证环境：GCC 15.2 (MinGW-w64) + CMake 3.28。

---

## 快速开始

### 1. 获取源码

```bash
git clone https://github.com/<your-account>/HIS_cpp.git
cd HIS_cpp
```

### 2. 构建

**Linux / macOS**

```bash
cmake -S . -B build
cmake --build build -j
```

**Windows（MinGW）**

```bash
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build -j
```

**Windows（MSVC）**

```bash
cmake -S . -B build
cmake --build build --config Release
```

### 3. 运行

> ⚠️ **请在项目根目录运行可执行文件**。程序按相对路径读写 `data/*.txt`，在其他目录运行会导致数据文件读写位置不符合预期。

```bash
# Linux / macOS
./build/HIS_cpp

# Windows (MinGW)
.\build\HIS_cpp.exe

# Windows (MSVC)
.\build\Release\HIS_cpp.exe
```

首次运行无需任何准备：程序会自动创建 `data/` 目录；数据文件不存在时按空数据启动。

### 4. 默认管理员账号

| 账号 | 密码 |
|------|------|
| `admin` | `123456` |

> 见 `config/his_config.h` 中的 `ADMIN_USERNAME` / `ADMIN_PASSWORD`，建议自行修改。

---

## 使用说明

### 启动流程

```
启动 → 请选择角色（1 患者 / 2 医生 / 3 管理员） → 输入凭证登录 → 进入角色菜单
```

### 角色菜单一览

**管理员菜单**

```
1. 患者管理      2. 医生管理      3. 科室管理      4. 药品管理
5. 床位管理      6. 药房管理      7. 住院管理      0. 退出登录
```

**患者菜单**

```
1. 挂号      2. 我的病历   3. 我的处方   4. 充值
5. 缴费      6. 取药       7. 修改密码   8. 办理住院     0. 退出登录
```

**医生菜单**

```
1. 待诊患者列表   2. 接诊        3. 我的病历记录   4. 我的处方记录
5. 缺药登记       6. 修改密码    0. 退出登录
```

### 典型体验路径

1. **以管理员登录**，依次创建：科室（如「内科」）→ 医生（绑定该科室）→ 药品（可指定适用科室、设置预警阈值）→ 床位（选择科室与类型）。
2. **以管理员注册患者**，或由患者自行注册后至管理员处**充值**。
3. **以患者登录**：挂号（选科室 → 选医生）→ 缴费 → 取药。
4. **以医生登录**：查看待诊列表 → 接诊并书写病历 → 开处方（若病历建议住院，患者可继续办理住院）。
5. **回到患者端**完成缴费与取药；若药品库存不足，系统会自动生成缺药记录。
6. **以管理员登录**，在「药房管理 → 缺药待办清单」中处理缺药并补货。

---

## 数据持久化

系统采用**纯文本文件**持久化，字段以 `|` 分隔、每行一条记录，便于人工查看与调试。

- 数据目录：`data/`（首次运行自动创建）
- 运行方式：启动时全量载入内存，变更后立即回写
- 内存管理：实体以 `std::unique_ptr` 持有于 `vector` 中

### 文件与字段格式

| 文件 | 字段顺序 |
|------|----------|
| `patient.txt` | `id \| name \| age \| gender \| phone \| id_card \| balance \| pin \| pin_hash \| insurance_rate` |
| `doctor.txt` | `id \| name \| dept_name \| specialty \| account \| password \| password_hash` |
| `dept.txt` | `id \| name \| description \| director_name \| location` |
| `drug.txt` | `id \| general_name \| trade_name \| alias \| price \| stock \| warning_stock \| dept_ids \| max_stock` |
| `bed.txt` | `id \| bed_number \| dept_id \| type \| daily_price \| status \| patient_id` |
| `appointment.txt` | `id \| patient_id \| doctor_id \| dept_id \| fee \| status \| create_time` |
| `record.txt` | `id \| appointment_id \| patient_id \| doctor_id \| complaint \| diagnosis \| orders \| create_time \| need_hospitalize` |
| `prescription.txt` | `id \| record_id \| patient_id \| doctor_id \| total_amount \| status \| create_time` |
| `prescription_item.txt` | `id \| prescription_id \| drug_id \| quantity \| usage \| amount` |
| `shortage.txt` | `id \| drug_id \| drug_name \| required_amount \| current_stock \| prescription_id \| triggered_by \| urgency \| status \| create_time` |
| `log.txt` | `[时间] [角色] [用户] 操作` |

### ID 规则

| 实体 | 前缀 | 示例 |
|------|------|------|
| 患者 | `P` | `P1`、`P2` |
| 医生 | `D` | `D1`、`D2` |
| 科室 | `K` | `K1`、`K2` |
| 药品 | `M` | `M1`、`M2` |
| 床位 | `B` | `B1`、`B2` |
| 挂号单 | `A` | `A1`、`A2` |
| 病历 | `MR` | `MR1`、`MR2` |
| 处方 | `RX` | `RX1`、`RX2` |
| 处方明细 | `PI` | `PI1`、`PI2` |
| 缺药记录 | `S` | `S1`、`S2` |

> 金额统一以 **分** 为单位存储为整数（药品 `price` 除外，其以 **元** 为单位）。ID 由各 Manager 在载入时扫描最大序号后自增分配。

---

## 配置说明

所有可调参数集中在 `config/his_config.h`，修改后重新编译即可生效。

### 业务默认值

| 常量 | 默认值 | 含义 |
|------|--------|------|
| `REGISTRATION_FEE` | `1000` | 挂号费（分），即 10 元 |
| `DEFAULT_INSURANCE` | `0.7f` | 默认医保比例（70%），随患者记录保存 |
| `DRUG_WARNING_RATIO` | `0.2f` | 自动预警阈值 = 最大库存 × 该比例 |
| `BED_FEE_NORMAL` | `5000` | 普通床位日费（分/天） |
| `BED_FEE_EMERGENCY` | `8000` | 急诊床位日费（分/天） |
| `BED_FEE_ICU` | `15000` | 重症床位日费（分/天） |

### 目录与文件路径

| 常量 | 值 |
|------|-----|
| `DATA_DIR` | `data` |
| `FILE_PATIENT` … `FILE_SHORTAGE` | `data/<实体>.txt` |
| `FILE_LOG` | `data/log.txt` |

### 管理员凭证

| 常量 | 默认值 |
|------|--------|
| `ADMIN_USERNAME` | `admin` |
| `ADMIN_PASSWORD` | `123456` |

### 状态字面量

状态字符串统一收敛到命名空间，避免魔法字符串与拼写错误：

- `AppointmentStatus`：`待诊` / `已接诊` / `已完成` / `已取消`
- `PrescriptionStatus`：`未缴费` / `已缴费` / `已取药`
- `BedStatus`：`空闲` / `占用` / `清洁中`
- `BedType`：`普通` / `急诊` / `重症`
- `ShortageStatus`：`待处理` / `已补货` / `已处理`
- `ShortageSource`：`取药` / `主动报告`

> 说明：`APPOINTMENT_FEE`、`MAX_ID_RETRY`、`MENU_LINE_LEN`、`MAX_*_LEN`、`FILE_SEP` 等常量为后续扩展预留，当前版本尚未参与逻辑。

---

## 业务规则

| 场景 | 规则 |
|------|------|
| 注册患者 | 手机号须为 11 位且以 `1` 开头；身份证须为 18 位并通过**校验位**验证 |
| 挂号 | 需先选择科室，且该科室存在医生；余额不足则拒绝挂号 |
| 删除科室 | 若科室下存在医生或关联药品，拒绝删除 |
| 删除药品 | 若药品仍有库存，拒绝删除 |
| 删除床位 | 仅当床位状态为「空闲」时允许删除 |
| 库存预警 | 当 `stock <= warning_stock` 时列入预警清单；阈值留空或填 `0` 时按 `最大库存 × 20%` 自动计算 |
| 缺药紧急度 | 库存 ≤ 0 视为**紧急**；缺口超过当前库存一半也视为**紧急**，否则为**普通** |
| 取药 | 先对全部明细做库存预检查，全部充足才出库；否则逐项登记缺药并中止，不产生部分出库 |
| 缴费 | 需余额不低于处方总额，扣款后处方状态置「已缴费」 |
| 住院 | 需存在「建议住院」的病历；扣除首日日费后占用床位，床位状态置「占用」 |
| 释放床位 | 置为「清洁中」并清空患者绑定 |

---

## 安全性设计

- **密码哈希**：患者 `pin` 与医生 `password` 使用自实现的 SHA-256（`utils/hash.h`）存储哈希值，明文字段在迁移后清空。
- **渐进式迁移**：兼容旧数据。登录时若记录仅含明文，则比对明文；成功后立即写入哈希并置 `dirty`，由调用方持久化，无需一次性迁移脚本。
- **输入约束**：病历主诉 / 诊断 / 医嘱禁止包含分隔符 `|`，避免破坏文本存储结构。
- **安全数值解析**：`utils/validator.h` 提供 `parseInt` / `parseLongLong` / `parseFloat`，非法输入返回 `false` 而非抛出异常，杜绝因误输入导致的崩溃。
- **操作审计**：登录、登出及关键动作记录到 `data/log.txt`，含真实时间戳。

> ⚠️ 本项目为教学 / 课程设计性质，管理员凭证为静态配置、数据文件未加密、无并发控制。**请勿直接用于真实医疗生产环境。**

---

## 代码质量与规范

### 统一风格

- [`.clang-format`](.clang-format)：4 空格缩进、K&R 大括号、列宽 100、指针引用靠左
- [`.editorconfig`](.editorconfig)：UTF-8、4 空格、去除行尾空白

```bash
# 一键格式化全部源码
clang-format -i $(git ls-files '*.cpp' '*.h')
```

### 编译告警

`CMakeLists.txt` 默认开启告警，建议保持零告警提交：

- MSVC：`/W4 /utf-8`
- GCC / Clang：`-Wall -Wextra`

### 工程实践

- **单一数据源**：ID 前缀、文件路径、状态字面量、时间格式统一由 `config` 与 `utils/datetime.h` 提供，避免多处硬编码。
- **消除重复逻辑**：缺药紧急度判定统一收敛到 `ShortageManager::calcUrgency`，被缴费与缺药服务复用。
- **安全解析**：所有用户数值输入经 `parseInt` / `parseLongLong` / `parseFloat` 校验，非法输入给出提示而非崩溃。
- **健壮启动**：`main()` 启动时自动创建数据目录，避免首次运行保存静默失败。
- **显式意图**：预留但未使用的参数以 `/*name*/` 形式标注，编译零告警同时保留接口稳定性。

---

## 已知限制与路线图

| 分类 | 说明 |
|------|------|
| 医保结算 | `insurance_rate` 字段已保存并展示，但缴费流程尚未应用报销计算 |
| 住院计费 | 目前仅扣除首日日费，未实现按天累计与出院结算 |
| 时间精度 | `create_time` 精确到秒，未记录时区；住院未记录入院 / 出院时间 |
| 数据存储 | 文本文件全量读写，无并发保护、无事务、无索引（仅处方明细有内存索引） |
| 并发 / 多用户 | 单进程单会话，无多用户并发能力 |
| 测试 | 尚无单元测试（`validator`、`billing` 等纯逻辑适合优先补充） |
| 头文件风格 | 部分头文件使用 `using namespace std;`，后续可改为完全限定名以提升封装性 |
| 待实现 | 医生排班、统计报表（日挂号量 / 收入 / Top 药品）、CSV 导出、采购建议单 |

阶段性开发计划详见 [`note.md`](note.md)。

---

## 贡献指南

欢迎提交 Issue 与 Pull Request。

### 提交前检查

1. **可编译**：确保 `cmake --build build` 成功且**无新增告警**。
2. **风格一致**：使用 `clang-format` 格式化改动文件。
3. **功能自测**：至少手动走通受影响的主流程（登录 → 对应菜单 → 关键操作）。
4. **数据兼容**：若修改了实体字段，请同步更新 `load()` / `save()` 并保证**向后兼容**旧数据文件（可参考密码字段的兼容读取写法）。

### 流程

```bash
# 1. Fork 本仓库并克隆
git clone https://github.com/<your-account>/HIS_cpp.git

# 2. 新建特性分支
git checkout -b feature/your-feature

# 3. 开发与格式化
clang-format -i <changed files>
cmake -S . -B build && cmake --build build

# 4. 提交（建议遵循 Conventional Commits）
git commit -m "feat(pharmacy): 支持药品批量入库"

# 5. 推送并创建 Pull Request
git push origin feature/your-feature
```

### 提交信息约定（建议）

| 前缀 | 用途 |
|------|------|
| `feat:` | 新增功能 |
| `fix:` | 缺陷修复 |
| `refactor:` | 重构（不改变外部行为） |
| `docs:` | 文档更新 |
| `style:` | 格式调整 |
| `test:` | 测试相关 |

### 代码约定

- 新增实体：在 `model/` 定义实体并派生 `DataManager<T>`，实现 `load()` / `save()`，在 `CMakeLists.txt` 登记源文件。
- 新增业务：优先放入 `service/`，避免在 `main.cpp` 堆积业务逻辑。
- 常量与状态字面量：统一放入 `config/his_config.h`，不要在代码中直接写裸字符串。
- 内存管理：统一使用 `std::unique_ptr` / `std::make_unique`，禁止裸 `new` / `delete`。

---

## 许可证

本项目当前未附带开源许可证文件。若计划公开协作或允许他人使用、修改与分发，建议补充一份许可证（如 [MIT](https://choosealicense.com/licenses/mit/) 或 [Apache-2.0](https://choosealicense.com/licenses/apache-2.0/)）以明确授权范围。

---

<div align="center">

**如果这个项目对你有帮助，欢迎 Star ⭐**

</div>
