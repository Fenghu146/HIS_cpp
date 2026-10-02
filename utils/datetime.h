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
    std::tm tmbuf{};
    // std::localtime 返回静态缓冲区，多线程/重入不安全且 MSVC 判定为
    // C4996 不安全函数；改用各自的可重入版本。
#if defined(_MSC_VER)
    if (localtime_s(&tmbuf, &now) != 0)
        return std::string();
#else
    if (!localtime_r(&now, &tmbuf))
        return std::string();
#endif

    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tmbuf);
    return std::string(buf);
}
