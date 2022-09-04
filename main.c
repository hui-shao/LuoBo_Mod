#include "main.h"

boolean loopContinueFlag = TRUE;

HWND hwnd;
DWORD pid;
HANDLE hProcess;

struct
{
    DWORD moduleBase;
    DWORD module;
    DWORD coinNumBase;
    DWORD coinCheckBase;
    DWORD jumpCheckBase;
    DWORD plantSubBase;
    DWORD upgradeSubBase;
} Address;

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
        // Handle the CTRL-C signal.
        case CTRL_C_EVENT:
            pr_warn("Ctrl-C event\n\n");
            Beep(750, 300);
            loopContinueFlag = FALSE;
            return TRUE;

            // CTRL-CLOSE: confirm that the user wants to exit.
        case CTRL_CLOSE_EVENT:
            Beep(600, 200);
            pr_warn("Ctrl-Close event\n\n");
            sweep();
            _exit(0);

            // Pass other signals to the next handler.
        case CTRL_BREAK_EVENT:
            Beep(900, 200);
            pr_warn("Ctrl-Break event\n\n");
            sweep();
            _exit(0);

        case CTRL_LOGOFF_EVENT:
            Beep(1000, 200);
            pr_warn("Ctrl-Logoff event\n\n");
            sweep();
            _exit(0);

        case CTRL_SHUTDOWN_EVENT:
            Beep(750, 500);
            pr_warn("Ctrl-Shutdown event\n\n");
            sweep();
            _exit(0);

        default:
            return FALSE;
    }
}

void findWindow(void)
{
    hwnd = FindWindow(NULL, "保卫萝卜Beta");
    if (hwnd == NULL)
    {
        pr_err("无法获取窗口句柄，请检查进程是否存在!  Code: %lu\n", GetLastError());
        quit(1);
    }
}

void getPID(void)
{
    GetWindowThreadProcessId(hwnd, &pid); // 获取 pid
    if (pid == 0)
    {
        pr_err("无法获取PID!  Code: %lu\n", GetLastError());
        quit(1);
    }
    pr_info("PID: %lu\n", pid);
}

void openProcess(void)
{
    hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE,
                           pid);
    if (hProcess == NULL)
    {
        pr_err("无法获取句柄ID!  Code: %lu\n", GetLastError());
        quit(1);
    }
    pr_bug("hProcess ID: %lu\n", (unsigned long) hProcess);
}

void getAddress(void)
{
    // 获取每一个模块加载基址
    HMODULE hModule[128] = {0};
    DWORD dwRet = 0;
    DWORD totalModuleNum;     // 总模块个数
    int bRet = EnumProcessModulesEx(hProcess, (HMODULE *) (hModule), sizeof(hModule), &dwRet, LIST_MODULES_ALL);
    if (bRet == 0)
    {
        pr_err("EnumProcessModules Failed.  Code: %lu\n", GetLastError());
        quit(0);
    }
    totalModuleNum = dwRet / sizeof(HMODULE);
    pr_bug("模块总数: %lu\n", totalModuleNum)
    // 打印每一个模块加载基址
    char lpBaseName[100];
    for (int i = 0; i < totalModuleNum; i++)
    {
        GetModuleBaseName(hProcess, hModule[i], lpBaseName, sizeof(lpBaseName));
        pr_bug("%-3d %-30s基址: 0x%p\n", i, lpBaseName, hModule[i]);
    }

    Address.moduleBase = (DWORD) hModule[0];
    pr_info("BaseAddr: 0x%p\n", (void *) Address.moduleBase);

    ReadProcessMemory(hProcess, (LPCVOID) Address.moduleBase, &Address.module, 4, NULL);

    ReadProcessMemory(hProcess, (LPCVOID) (Address.moduleBase + 0x105E68), &Address.coinNumBase, 4, NULL);
    Address.coinNumBase += 0x74;
    pr_bug("金币基址: 0x%p\n", (void *) Address.coinNumBase);

    ReadProcessMemory(hProcess, (LPCVOID) (Address.moduleBase + 0x105E68), &Address.coinCheckBase, 4, NULL);
    Address.coinCheckBase += 0xEC;
    pr_bug("金币校验基址: 0x%p\n", (void *) Address.coinCheckBase);

    Address.jumpCheckBase = Address.moduleBase + 0x24580;
    pr_bug("跳转校验基址: 0x%p\n", (void *) Address.jumpCheckBase);

    Address.plantSubBase = Address.moduleBase + 0x21DC0;
    pr_bug("种植时减金币代码校验基址: 0x%p\n", (void *) Address.plantSubBase);

    Address.upgradeSubBase = Address.moduleBase + 0x24612;
    pr_bug("升级时减金币代码校验基址: 0x%p\n", (void *) Address.upgradeSubBase);
}

void modifyJumpCheck(void)
{
    static const BYTE originalCode[] = {0x74, 0x70};  // je
    static const BYTE targetCode[] = {0xEB, 0x70};  // jmp
    BYTE tempBuf[2] = {0};
    ReadProcessMemory(hProcess, (LPCVOID) Address.jumpCheckBase, tempBuf, sizeof(tempBuf), NULL);
    pr_bug("Previous Code - JumpCheck: %x %x\n", tempBuf[0], tempBuf[1]);
    if (tempBuf[0] == originalCode[0] && tempBuf[1] == originalCode[1])
    {
        pr_info("CoinNumCheck patch point found. Trying to patch.\n");
        WriteProcessMemory(hProcess, (LPVOID) Address.jumpCheckBase, targetCode, sizeof(targetCode), NULL);
    } else if (tempBuf[0] == targetCode[0] && tempBuf[1] == targetCode[1])
    {
        pr_info("CoinNumCheck has already been patched.\n");
    } else
    {
        pr_warn("Unknown CoinNumCheck patch state.\n");
    }
}

void modifyPlantSub(void)
{
    static const BYTE originalCode[] = {0x29, 0x5F, 0x74};  // sub
    static const BYTE targetCode[] = {0x01, 0x5F, 0x74};  // add
    BYTE tempBuf[3] = {0};
    ReadProcessMemory(hProcess, (LPCVOID) Address.plantSubBase, tempBuf, sizeof(tempBuf), NULL);
    pr_bug("Previous Code - PlantSub: %x %x\n", tempBuf[0], tempBuf[1]);
    if (tempBuf[0] == originalCode[0] && tempBuf[1] == originalCode[1])
    {
        pr_info("PlantSub patch point found. Trying to patch.\n");
        WriteProcessMemory(hProcess, (LPVOID) Address.plantSubBase, targetCode, sizeof(targetCode), NULL);
    } else if (tempBuf[0] == targetCode[0] && tempBuf[1] == targetCode[1])
    {
        pr_info("PlantSub has already been patched.\n");
    } else
    {
        pr_warn("Unknown PlantSub patch state.\n");
    }
}

void modifyUpgradeSub(void)
{
    static const BYTE originalCode[] = {0x01, 0x46, 0x74};  // add
    static const BYTE targetCode[] = {0x29, 0x46, 0x74};  // sub
    BYTE tempBuf[3] = {0};
    ReadProcessMemory(hProcess, (LPCVOID) Address.upgradeSubBase, tempBuf, sizeof(tempBuf), NULL);
    pr_bug("Previous Code - UpgradeSub: %x %x\n", tempBuf[0], tempBuf[1]);
    if (tempBuf[0] == originalCode[0] && tempBuf[1] == originalCode[1])
    {
        pr_info("UpgradeSub patch point found. Trying to patch.\n");
        WriteProcessMemory(hProcess, (LPVOID) Address.upgradeSubBase, targetCode, sizeof(targetCode), NULL);
    } else if (tempBuf[0] == targetCode[0] && tempBuf[1] == targetCode[1])
    {
        pr_info("UpgradeSub has already been patched.\n");
    } else
    {
        pr_warn("Unknown UpgradeSub patch state.\n");
    }
}

void modifyCoinNum(void)
{
    DWORD coinNum_previous = 0;
    ReadProcessMemory(hProcess, (LPCVOID) Address.coinNumBase, &coinNum_previous, 4, NULL);
    pr_info("Previous Coin Num: %lu\n", coinNum_previous);
    DWORD coinNum_target = 6666;
    if (coinNum_previous < coinNum_target - 100)
    {
        WriteProcessMemory(hProcess, (LPVOID) Address.coinNumBase, &coinNum_target, 4, NULL);
        pr_info("Set Coin Num to: %lu\n", coinNum_target);
    }
}


void sweep(void)
{
    if (hProcess != NULL)
    {
        CloseHandle(hProcess);
        hProcess = NULL;
    }
}

void quit(int code)
{
    sweep();
    pr_nano("按任意键退出程序...\n");
    system("pause>nul");
    exit(code);
}

int main()
{
    system("title 保卫萝卜内存修改器 v" VERSION "  By Hui-Shao");
    SetConsoleCtrlHandler(CtrlHandler, TRUE);
    enableColorful();
    findWindow();
    getPID();
    openProcess();
    getAddress();
    modifyJumpCheck();
    modifyPlantSub();
    modifyUpgradeSub();
    while (loopContinueFlag)
    {
        modifyCoinNum();
        Sleep(5 * 1000);
    }
    quit(0);
}
