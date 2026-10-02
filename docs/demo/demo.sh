#!/usr/bin/env bash
# ============================================================
#  HIS_cpp 一键全流程演示
#
#  真实二进制 + 回归测试同源输入，按阶段标注输出：
#  建科 → 医生 → 药品 → 患者 → 充值 → 挂号 → 接诊 → 开方 → 缴费 → 取药
#
#  用法：bash docs/demo/demo.sh
#        HIS_BIN=/path/to/HIS_cpp bash docs/demo/demo.sh   # 指定产物
# ============================================================
set -u

SRC=$(cd "$(dirname "$0")/../.." && pwd)
DEMO_DIR=$(cd "$(dirname "$0")" && pwd)

# ---- 定位可执行文件（与测试脚本同一约定）----
find_bin() {
    if [ -n "${HIS_BIN:-}" ] && [ -f "$HIS_BIN" ]; then printf '%s' "$HIS_BIN"; return 0; fi
    local c
    for c in \
        "$SRC/build/HIS_cpp" \
        "$SRC/build/HIS_cpp.exe" \
        "$SRC/build/Release/HIS_cpp" \
        "$SRC/build/Release/HIS_cpp.exe" \
        "$SRC/build/Debug/HIS_cpp" \
        "$SRC/build/Debug/HIS_cpp.exe"; do
        [ -f "$c" ] && { printf '%s' "$c"; return 0; }
    done
    return 1
}

BIN=$(find_bin) || {
    echo "未找到 HIS_cpp 产物。先构建：cmake -S . -B build && cmake --build build"
    exit 1
}

# ---- 演示输入：直接复用回归测试的全流程输入，保证与 CI 跑的是同一条链路 ----
INPUT=$(awk '/^cat > .*case1\.in.*<<.EOF.$/{f=1;next} /^EOF$/{f=0} f' "$SRC/tests/full_flow_test.sh")
[ -n "$INPUT" ] || { echo "无法从 tests/full_flow_test.sh 提取演示输入"; exit 1; }

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

echo "HIS_cpp 全流程演示"
echo "产物：$BIN    数据目录：$WORK/data（临时，跑完自动清理）"
echo

# ---- 真实跑一遍全流程 ----
( cd "$WORK" && printf '%s\n' "$INPUT" | "$BIN" > raw.out 2>&1 )

# ---- 按阶段锚点插入横幅（锚点取自回归测试的真实输出断言）+ 落盘核验 ----
{
awk '
/登录成功/                  && !b1++ { print "━━━ ① 管理员登录 ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" }
/注册成功！ID：K1/          && !b2++ { print "━━━ ② 创建科室（内科） ━━━━━━━━━━━━━━━━━━━━━━━" }
/注册成功！ID：D1/          && !b3++ { print "━━━ ③ 注册医生（张医生 / dr001） ━━━━━━━━━━━━━━" }
/注册成功！ID：M1/          && !b4++ { print "━━━ ④ 录入药品（阿莫西林 ×100） ━━━━━━━━━━━━━━━" }
/注册成功！ID：P1/          && !b5++ { print "━━━ ⑤ 注册患者（李患者 / P1） ━━━━━━━━━━━━━━━━━" }
/充值成功/                  && !b6++ { print "━━━ ⑥ 患者充值 ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" }
/挂号成功/                  && !b7++ { print "━━━ ⑦ 患者挂号（扣挂号费、生成流水） ━━━━━━━━━━━━" }
/接诊患者：P1/              && !b8++ { print "━━━ ⑧ 医生接诊 · 病历 ━━━━━━━━━━━━━━━━━━━━━━━━" }
/处方号：RX1/               && !b9++ { print "━━━ ⑨ 医生开方（含库存检查） ━━━━━━━━━━━━━━━━━━━" }
/缴费成功/                  && !b10++ { print "━━━ ⑩ 患者缴费 ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" }
/出库成功/                  && !b11++ { print "━━━ ⑪ 药房取药 · 出库核销 ━━━━━━━━━━━━━━━━━━━━━" }
{ print }
' "$WORK/raw.out"
echo
echo "━━━ ⑫ 落盘核验（持久化数据） ━━━━━━━━━━━━━━━━━━━"
( cd "$WORK" && wc -l data/*.txt 2>/dev/null | sed 's/^/  /' )
} | tee "$DEMO_DIR/full-flow.txt"

echo
echo "完整输出已存：docs/demo/full-flow.txt"
echo "生成演示 GIF：python3 docs/demo/gen_gif.py"