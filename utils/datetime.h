#pragma once

#include <string>
#include <ctime>

// ============================================================================
// 时间工具
// ----------------------------------------------------------------------------
// 统一所有实体 create_time 字段的时间戳格式，避免在各处硬编码日期字符串。
// 格式：YYYY-MM-DD HH:MM:SS（19 字符，不含 '|'，可安全用于文本文件存储）
// ============================================================================

// 返回当前本地时间字符串，例如 "2026-09-11 14:30:05"
inline std::string nowTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* t = std::localtime(&now);
    if (!t) return std::string();

    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
    return std::string(buf);
}
