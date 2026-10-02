# 更新日志

本项目所有值得记录的变更都写在这里。
格式遵循 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.1.0/)，版本号遵循 [语义化版本](https://semver.org/lang/zh-CN/)。

> 定位说明：本仓库（HIS_cpp）是主力实现；`Fenghu146/HIS`（C 版）是课程设计存档版。

## [2.0.0] - 2026-10-02

生产级规范化基线。1.x 为课程迭代阶段（MVC 分层重构与批量 BUGFIX，未发布 tag），
自 2.0.0 起进入常态化工程维护。

### 新增

- 全流程回归测试套件：stdin 驱动真实菜单跑通「建科→建医生→建药→建患者→充值→挂号→接诊→开方→缴费→取药」闭环，
  含边界场景与重启后数据一致性验证（46 项断言，任一失败退出码非 0）
- GitHub Actions CI：Linux / macOS / Windows 三平台构建 + 零告警门（含 MSVC）+ 回归测试 +
  lint 门（shellcheck / cppcheck / clang-format 格式门，clang-format 钉版本 23.1.2）
- CTest 集成：`ctest` 直接运行回归套件，支持 `HIS_BIN` 注入产物路径
- 工程规范：`CHANGELOG.md`、`.editorconfig`、`.clang-format`；cppcheck 清零
  （成员零初始化、getter 按引用返回）；全量应用 clang-format 统一格式，
  格式化提交列入 `.git-blame-ignore-revs`

### 修复

- 医生账号无唯一性校验、科室无引用完整性校验（自由文本导致「科室 ID / 科室名」混淆后挂号找不到医生）
- 取药库存超卖：同一药品拆多条明细可绕过库存检查，改为按药品聚合需求量校验（附守护测试）
- 患者 PIN 无格式校验；接诊结束缺「无需住院」反馈
- 跨平台回归失败：测试脚本硬编码 `timeout` 命令与单配置产物路径，macOS/Windows 上二进制根本没跑起来
- MSVC C4996：`localtime` 静态缓冲（线程安全隐患）改为可重入的 `localtime_s` / `localtime_r`
- `hash.h` 定长缓冲越界隐患（消息长度 > 119 字节时写爆栈）

### 变更

- **凭据存储统一 SHA-256 摘要**（医生密码 / 患者 PIN / 管理员口令）：注册与改密即哈希、明文不再落盘；
  旧格式（明文）登录成功时自动迁移为摘要；管理员静态凭据改为摘要常量，源码中不再出现明文口令
- README 补齐十分钟上手、测试与回归、安全边界说明

## [1.x] - 2026-09

课程设计迭代阶段：MVC 分层架构重构、早期 BUGFIX 批量修复（未发布 tag）。

[2.0.0]: https://github.com/Fenghu146/HIS_cpp/releases/tag/v2.0.0
