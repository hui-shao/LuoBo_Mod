#ifndef LUOBO_MOD_MAIN_H
#define LUOBO_MOD_MAIN_H

#include <windows.h>
#include <psapi.h>
#include "print.h"

#define VERSION "1.1"

void findWindow(void);

void getPID(void);

void openProcess(void);

void getAddress(void);

void modifyJumpCheck(void);

void modifyPlantSub(void);

void modifyUpgradeSub(void);

void modifyCoinNum(void);

void sweep(void);

void quit(int code);

#endif //LUOBO_MOD_MAIN_H
