#ifndef _CONIO_H_
#define _CONIO_H_

/*******************************************
 * File Name: conio.hpp
 * Function: 解决代码在 Linux 与 Windows 系统
 *           移植问题
 * Define: 1. Linux系统的 _getch() 无回显函数
 *         2. 双系统的清屏函数
 * ****************************************/

#include <iostream>

#ifdef _WIN32
#include <conio.h>
#elif __linux__

char _getch() {
    char c;
    system("stty -echo");
    system("stty -icanon");
    c = getchar();
    system("stty icanon");
    system("stty echo");
    return c;
}
#endif

void clear() {
    #ifdef _WIN32
    system("cls");
    #elif __linux__
    system("clear");
    #endif
}

#endif