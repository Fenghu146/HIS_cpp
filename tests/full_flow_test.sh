#!/usr/bin/env bash
# ============================================================
# HIS_cpp 医院信息管理系统 — 全流程 + 边界回归测试
#
# 用法（Linux / macOS / Git Bash）：
#     bash tests/full_flow_test.sh
#
# 特点：
#   * 在 tests/.work/ 下使用独立 data 目录运行，不污染源码树
#   * 通过 stdin 驱动菜单完成「建科→建医生→建药品→建患者→充值→挂号
#     → 接诊写病历→开方→缴费→取药」的完整业务闭环
#   * 覆盖身份证校验位、账号唯一性、科室引用完整性、PIN 格式、
#     余额不足、库存不足等边界场景
#   * 验证重启后数据持久化一致性
#   * 每条断言打印 PASS/FAIL，最后给出汇总；任一失败退出码非 0
# ============================================================
set -u

# 字节级确定性：数据提取与字符串断言（grep -F）按字节工作，
# 不受 macOS/BSD 工具在 UTF-8 locale 下对非法字节序列的行为差异影响。
export LC_ALL=C

HERE="$(cd "$(dirname "$0")" && pwd)"
SRC="$HERE/.."
WORK="$HERE/.work"
DATA="$WORK/data"
OUT="$WORK/out"

PASS=0
FAIL=0
FAILED_LIST=""

pass() { PASS=$((PASS + 1)); printf '  [PASS] %s\n' "$1"; }
fail() { FAIL=$((FAIL + 1)); FAILED_LIST="$FAILED_LIST
  - $1"; printf '  [FAIL] %s\n' "$1"; }

assert_has() {
    if grep -qF -- "$2" "$3"; then pass "$1"; else fail "$1  ← 输出中未找到「$2」"; fi
}
assert_not_has() {
    if grep -qF -- "$2" "$3"; then fail "$1  ← 输出中不应出现「$2」"; else pass "$1"; fi
}
assert_eq() {
    if [ "$2" = "$3" ]; then pass "$1"; else fail "$1  ← 期望 [$3] 实际 [$2]"; fi
}

# ---------- 构建 ----------
# 可执行文件定位：兼容单配置（Makefiles/Ninja，产物在 build/）与
# 多配置（MSVC，产物在 build/<Config>/HIS_cpp.exe）生成器。
find_bin() {
    local c
    # 优先尊重调用方注入的产物路径（CTest / CI 可用 HIS_BIN 指定）
    if [ -n "${HIS_BIN:-}" ] && [ -f "$HIS_BIN" ]; then
        printf '%s' "$HIS_BIN"
        return 0
    fi
    for c in \
        "$SRC/build/HIS_cpp" \
        "$SRC/build/HIS_cpp.exe" \
        "$SRC/build/Release/HIS_cpp" \
        "$SRC/build/Release/HIS_cpp.exe" \
        "$SRC/build/RelWithDebInfo/HIS_cpp.exe" \
        "$SRC/build/Debug/HIS_cpp.exe" \
        "$SRC/build/Debug/HIS_cpp"; do
        [ -f "$c" ] && { printf '%s' "$c"; return 0; }
    done
    return 1
}

echo "== 构建 HIS_cpp =="
if ! BIN=$(find_bin); then
    cmake -S "$SRC" -B "$SRC/build" -DCMAKE_BUILD_TYPE=Release >"$SRC/build-config.log" 2>&1 || {
        echo "CMake 配置失败，日志：$SRC/build-config.log"; exit 1
    }
    cmake --build "$SRC/build" --config Release -j2 >"$SRC/build.log" 2>&1 || {
        echo "构建失败，日志：$SRC/build.log"; exit 1
    }
    BIN=$(find_bin) || { echo "构建后仍未找到可执行文件"; exit 1; }
fi
pass "构建成功（$BIN）"

# ---------- 准备独立工作目录 ----------
rm -rf "$WORK"
mkdir -p "$WORK" "$OUT"

# 超时保护：功能性探测而非 command -v——Windows Git Bash 的 PATH 里
# timeout 解析到 System32\timeout.exe（拒绝 stdin 重定向，报错即退出），
# 只有能真正跑通 `timeout 1 true` 的实现才可用。
if timeout 1 true >/dev/null 2>&1; then TMO="timeout 60"; else TMO=""; fi

run_case() { # $1=用例名 $2=输入文件
    ( cd "$WORK" && rm -rf data && $TMO "$BIN" < "$2" > "$OUT/$1.out" 2>&1 )
    local rc=$?
    if [ "$rc" -ge 124 ]; then
        fail "$1 用例超时/被杀 (rc=$rc)"
    elif [ "$rc" -ne 0 ]; then
        fail "$1 用例非正常退出 (rc=$rc)"
    fi
}

# ============================================================
# 用例 1：全流程闭环（建科→医生→药品→患者→充值→挂号→接诊→开方→缴费→取药）
# ============================================================
echo
echo "== 阶段1：全流程闭环 =="
cat > "$WORK/case1.in" <<'EOF'
3
admin
123456
3
1
内科
常见内科疾病诊疗
王主任
门诊楼3层
0
2
1
张医生
内科
心血管疾病
dr001
pwd001
0
4
1
阿莫西林
阿莫西林胶囊
阿莫灵
1500
100
200
20
1
0
1
1
李患者
30
男
13800138000
110101199001011237
123456
0
0
1
P1
123456
4
100000
0
1
P1
123456
1
P1
1
1
y
0
2
dr001
pwd001
2
1
感冒发热
上呼吸道感染
休息多喝水
n
y
1
2
每日三次饭后服用
0
0
1
P1
123456
5
1
y
6
1
y
0
0
0
EOF
run_case case1 "$WORK/case1.in"
o="$OUT/case1.out"

assert_has "管理员登录成功" "登录成功" "$o"
assert_has "科室注册成功" "注册成功！ID：K1" "$o"
assert_has "医生注册成功" "注册成功！ID：D1" "$o"
assert_has "药品注册成功" "注册成功！ID：M1" "$o"
assert_has "患者注册成功" "注册成功！ID：P1" "$o"
assert_has "患者充值成功" "充值成功" "$o"
assert_has "挂号成功并扣挂号费" "挂号成功" "$o"
assert_has "挂号流水号生成" "挂号单号：A1" "$o"
assert_has "接诊患者与挂号单关联" "接诊患者：P1，挂号单：A1" "$o"
assert_has "病历创建成功" "病历已创建，ID：MR1" "$o"
assert_has "医生确认无需住院" "本次就诊无需住院" "$o"
assert_has "处方创建成功" "处方号：RX1" "$o"
assert_has "库存检查通过" "需要 2，库存 100" "$o"
assert_has "缴费成功" "缴费成功！处方 RX1 已缴费" "$o"
assert_has "药品出库成功" "出库成功！当前库存：98" "$o"
assert_has "取药成功" "取药成功！处方 RX1" "$o"
assert_not_has "全流程无错误提示" "[错误]" "$o"

# --- 凭据存储守护：注册即哈希，明文密码/PIN 不落盘 ---
assert_not_has "医生密码明文不落盘" "|pwd001|" "$DATA/doctor.txt"
assert_has "医生密码哈希已落盘" "72ba6aa729caf8f430e64db59fdcc506137be47ab0528288b956bca8f49f801b" "$DATA/doctor.txt"
assert_not_has "患者PIN明文不落盘" "|123456|" "$DATA/patient.txt"
assert_has "患者PIN哈希已落盘" "8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92" "$DATA/patient.txt"

# ============================================================
# 用例 2：边界 — 身份证校验位 / 账号唯一 / 科室引用 / PIN 格式
# ============================================================
echo
echo "== 阶段2：边界（身份证/账号唯一/科室引用/PIN） =="
cat > "$WORK/case2.in" <<'EOF'
3
admin
123456
3
1
内科
内科简介
王主任
门诊楼3层
0
2
1
张医生
内科
心血管疾病
dr001
pwd001
1
李医生
内科
呼吸疾病
dr001
dr002
pwd002
1
王医生
不存在科室
内科
呼吸疾病
dr003
pwd003
0
1
1
张患者
30
男
13800138000
110101199001011234
1
张患者
30
男
13800138000
110101199001011237
123
123456
0
0
0
EOF
run_case case2 "$WORK/case2.in"
o="$OUT/case2.out"

assert_has "身份证校验位错误被拒绝" "身份证格式不正确" "$o"
assert_has "医生账号重复被拒绝并提示重试" "账号已被注册" "$o"
assert_has "医生科室不存在被拒绝并提示重试" "不存在，请先创建该科室" "$o"
assert_has "PIN 格式错误被拒绝并提示重试" "6 位数字" "$o"
assert_has "重复账号重试后注册成功" "注册成功！ID：D2" "$o"
assert_has "无效科室重试后注册成功" "注册成功！ID：D3" "$o"
assert_has "合法身份证与 PIN 注册成功" "注册成功！ID：P1" "$o"

# ============================================================
# 用例 3：余额不足不能挂号
# ============================================================
echo
echo "== 阶段3：余额不足 =="
cat > "$WORK/case3.in" <<'EOF'
3
admin
123456
3
1
内科
内科简介
王主任
门诊楼3层
0
2
1
张医生
内科
心血管疾病
dr001
pwd001
0
1
1
张患者
30
男
13800138000
110101199001011237
123456
0
0
1
P1
123456
1
P1
1
1
y
0
0
0
EOF
run_case case3 "$WORK/case3.in"
o="$OUT/case3.out"

assert_has "余额不足被拒绝" "余额不足" "$o"
assert_not_has "余额不足时未生成挂号单" "挂号单号：" "$o"

# ============================================================
# 用例 4：库存不足取药被拦截
# ============================================================
echo
echo "== 阶段4：库存不足（同一药品多明细累积超卖守护） =="
cat > "$WORK/case4.in" <<'EOF'
3
admin
123456
3
1
内科
内科简介
王主任
门诊楼3层
0
2
1
张医生
内科
心血管疾病
dr001
pwd001
0
4
1
阿莫西林
阿莫西林胶囊
阿莫灵
1500
1
200
1
1
0
1
1
张患者
30
男
13800138000
110101199001011237
123456
0
0
1
P1
123456
4
100000
0
1
P1
123456
1
P1
1
1
y
0
2
dr001
pwd001
2
1
感冒发热
上呼吸道感染
休息多喝水
n
y
1
1
每日三次
1
1
每日两次
0
0
1
P1
123456
5
1
y
6
1
0
0
0
EOF
run_case case4 "$WORK/case4.in"
o="$OUT/case4.out"

assert_has "单明细开方成功（库存 1 开 1）" "处方号：RX1" "$o"
assert_has "库存不足在取药前被发现" "库存不足" "$o"
assert_not_has "库存不足未出库" "出库成功" "$o"
assert_has "缺药自动登记" "缺药" "$o"

# ============================================================
# 用例 5：重启后数据一致性
# ============================================================
echo
echo "== 阶段5：重启后数据一致性 =="
( cd "$WORK" && rm -rf data )
run_case case5 "$WORK/case1.in"

for f in patient.txt doctor.txt dept.txt drug.txt record.txt \
         prescription.txt prescription_item.txt appointment.txt; do
    if [ -s "$WORK/data/$f" ]; then pass "数据文件已生成：$f"; else fail "数据文件缺失或为空：$f"; fi
done
assert_has "病历数据落盘" "MR1" "$WORK/data/record.txt"
assert_has "处方数据落盘" "RX1" "$WORK/data/prescription.txt"
assert_has "挂号数据落盘" "A1" "$WORK/data/appointment.txt"

# ============================================================
# 汇总
# ============================================================
echo
echo "============================================================"
echo "测试汇总：PASS=$PASS  FAIL=$FAIL"
if [ "$FAIL" -gt 0 ]; then
    echo "失败项：$FAILED_LIST"
    echo "详细输出见: $OUT/"
    exit 1
fi
echo "全部通过 ✔"
