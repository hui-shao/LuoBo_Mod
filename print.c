#include "print.h"
#include <windows.h>


/*!
 * 为传统控制台启用色彩支持
 * 本质上是利用了虚拟终端序列
 * 详见 https://docs.microsoft.com/zh-cn/windows/console/console-virtual-terminal-sequences
 */
DWORD enableColorful(void)
{
#if PRINT_COLORFUL
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return GetLastError();
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
        return GetLastError();
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
    {
        return GetLastError();
    }
#endif
    return 0;
}
