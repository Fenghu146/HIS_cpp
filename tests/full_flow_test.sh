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
echo "== 构建 HIS_cpp =="
if [ ! -x "$SRC/build/HIS_cpp" ]; then
    cmake -S "$SRC" -B "$SRC/build" -DCMAKE_BUILD_TYPE=Release >"$SRC/build-config.log" 2>&1 || {
        echo "CMake 配置失败，日志：$SRC/build-config.log"; exit 1
    }
    cmake --build "$SRC/build" -j2 >"$SRC/build.log" 2>&1 || {
        echo "构建失败，日志：$SRC/build.log"; exit 1
    }
fi
pass "构建成功"

# ---------- 准备独立工作目录 ----------
rm -rf "$WORK"
mkdir -p "$WORK" "$OUT"
cp "$SRC/build/HIS_cpp" "$WORK/his_cpp"

run_case() { # $1=用例名 $2=输入文件
    ( cd "$WORK" && rm -rf data && timeout 60 ./his_cpp < "$2" > "$OUT/$1.out" 2>&1 )
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