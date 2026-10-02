#!/usr/bin/env python3
"""把 docs/demo/full-flow.txt 渲染成终端风格的打字机演示 GIF。

用法：
    python3 docs/demo/gen_gif.py [输入.txt] [输出.gif]

依赖：Pillow（micromamba create -p <prefix> -c conda-forge pillow）
"""
import sys
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

# ---- 终端配色 ----
BG = (16, 20, 24)
FG = (212, 212, 212)
ACCENT = (78, 201, 176)      # 阶段横幅
GOOD = (106, 153, 85)        # 成功输出
BAD = (244, 71, 71)          # 失败/错误输出
DIM = (122, 130, 138)        # 补充信息

FONT_PATHS = [
    "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
    "/usr/share/fonts/dejavu/DejaVuSansMono.ttf",
]
FONT_SIZE = 14
LINE_H = 21
PAD = 24
WIDTH = 960
HEIGHT = 620
FPS = 14
LINES_PER_FRAME = 2
TAIL_HOLD = 26   # 末帧停留帧数（循环观感）


def load_font():
    for p in FONT_PATHS:
        if Path(p).exists():
            return ImageFont.truetype(p, FONT_SIZE)
    return ImageFont.load_default()


def line_color(line):
    if "━━━" in line:
        return ACCENT
    if "失败" in line or "错误" in line or "拒绝" in line:
        return BAD
    if "成功" in line or "✔" in line or "→" in line:
        return GOOD
    if line.startswith(" ") or line.startswith("  "):
        return DIM
    return FG


def main():
    src = Path(sys.argv[1]) if len(sys.argv) > 1 else Path(__file__).parent / "full-flow.txt"
    dst = Path(sys.argv[2]) if len(sys.argv) > 2 else Path(__file__).parent / "full-flow.gif"

    lines = src.read_text(encoding="utf-8").rstrip("\n").split("\n")
    font = load_font()
    max_visible = (HEIGHT - 2 * PAD) // LINE_H

    frames = []
    for k in range(0, len(lines) + 1, LINES_PER_FRAME):
        visible = lines[max(0, k - max_visible): k or 1]
        img = Image.new("RGB", (WIDTH, HEIGHT), BG)
        draw = ImageDraw.Draw(img)
        # 顶部提示条
        draw.rectangle([0, 0, WIDTH, 28], fill=(26, 32, 38))
        draw.text((PAD, 7), "HIS_cpp  ·  全流程演示", font=font, fill=ACCENT)
        y = PAD + 12
        for line in visible:
            draw.text((PAD, y), line[:118], font=font, fill=line_color(line))
            y += LINE_H
        # 光标
        if k < len(lines):
            draw.rectangle([PAD, y, PAD + 9, y + LINE_H - 5], fill=FG)
        frames.append(img)

    frames.extend([frames[-1]] * TAIL_HOLD)
    duration = int(1000 / FPS)
    frames[0].save(dst, save_all=True, append_images=frames[1:],
                   duration=duration, loop=0, optimize=True)
    print(f"{dst}：{len(frames)} 帧，{WIDTH}x{HEIGHT}，{dst.stat().st_size // 1024} KB")


if __name__ == "__main__":
    main()