#ifndef LUOBO_MOD_PRINT_H
#define LUOBO_MOD_PRINT_H

#include <stdio.h>
#include <windows.h>

DWORD enableColorful(void);

//* 配置 */
// #define PRINT_LEVEL LEVEL_DEBUG // 日志等级 此项目中该标志使用CMAKE定义
// #define PRINT_COLORFUL 1  // 是否启用彩色 此项目中该标志使用CMAKE定义

//* 日志等级定义 */
#define LEVEL_NANO 0
#define LEVEL_ERROR 1
#define LEVEL_WARN 2
#define LEVEL_INFO 3
#define LEVEL_ENTRY 4
#define LEVEL_DEBUG 5

//* 颜色定义 */
#define COLOR_GRAY "\033[37m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_DARKGRAY "\033[30m"
#define COLOR_BLACK "\033[30m"
#define COLOR_NOCOLOR "\033[0m"
#define COLOR_DEEPBLUE "\033[34m"
#define COLOR_RED "\033[31m"

//*   纯净模式   */
#define PR_CUST(level, ...)      \
    do                            \
    {                             \
        if (level <= PRINT_LEVEL) \
            printf(__VA_ARGS__);  \
    } while (0);

//*   分类定义   */
#if PRINT_COLORFUL
#define pr_nano(fmt, ...) PR_CUST(LEVEL_NANO, fmt, ##__VA_ARGS__)
#define pr_err(fmt, ...) PR_CUST(LEVEL_ERROR, COLOR_RED "ERROR:" fmt COLOR_NOCOLOR, ##__VA_ARGS__)
#define pr_warn(fmt, ...) PR_CUST(LEVEL_WARN, COLOR_YELLOW "WARN: " fmt COLOR_NOCOLOR, ##__VA_ARGS__)
#define pr_info(fmt, ...) PR_CUST(LEVEL_INFO, COLOR_GREEN "INFO: " fmt COLOR_NOCOLOR, ##__VA_ARGS__)
#define pr_bug(fmt, ...) PR_CUST(LEVEL_DEBUG, COLOR_DEEPBLUE "DEBUG: " fmt COLOR_NOCOLOR, ##__VA_ARGS__)
#define pr_entry(inout) PR_CUST(LEVEL_ENTRY, "%s() %s", __func__, #inout)
#else
#define pr_nano(fmt, ...) PR_CUST(LEVEL_NANO, fmt, ##__VA_ARGS__)
#define pr_err(fmt, ...) PR_CUST(LEVEL_ERROR, "ERROR:" fmt, ##__VA_ARGS__)
#define pr_warn(fmt, ...) PR_CUST(LEVEL_WARN, "WARN: " fmt, ##__VA_ARGS__)
#define pr_info(fmt, ...) PR_CUST(LEVEL_INFO, "INFO: " fmt, ##__VA_ARGS__)
#define pr_bug(fmt, ...) PR_CUST(LEVEL_DEBUG, "DEBUG: " fmt, ##__VA_ARGS__)
#define pr_entry(inout) PR_CUST(LEVEL_ENTRY, "%s() %s", __func__, #inout)
#endif // PR_COLORFUL

#endif // LUOBO_MOD_PRINT_H
