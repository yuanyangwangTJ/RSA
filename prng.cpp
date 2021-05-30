/**************************************
 * Class Name: PRNG
 * Function: 伪随机数比特生成器
 * ***********************************/

#include "Engine.h"
#include "TDES.h"
#include <iostream>
#include <ctime>
#include <cstring>
#include <NTL/ZZ.h>
 
using namespace std;
using namespace NTL;

typedef unsigned long long size_ul;

// 构造函数，进行初始化操作
PRNG::PRNG(int n) {
    m = n;
    x = new bitset<64>[m];
    GenerateRandom();
    ZZ r = BitsToNumber();
    cout << r << endl;
}

// 通过外界修改 m 的数值
void PRNG::SetM(int n) {
    m = n;
    delete [] x;
    x = new bitset<64>[m];
}

// 伪随机数比特生成函数
void PRNG::GenerateRandom() {
    // 获取本地时间
    time_t now = time(0);
    tm *ltm = localtime(&now);
    size_ul d = size_ul(ltm->tm_hour) << 32 + size_t(ltm->tm_min) << 16 + ltm->tm_sec;
    bitset<64> Date(d);

    // 产生 64 比特随机种子
    srand((unsigned)time(NULL));
    size_ul ss = size_ul(random()) << 32 + random();
    
    bitset<64> tmp(ss);
    s = tmp;

    TDES td;
    td.plain = Date;
    bitset<64> Im = td.TDESEncrypt();
    for (int i = 0; i < m; i++) {
        td.plain = Im ^ s;
        x[i] = td.TDESEncrypt();
        td.plain = x[i] ^ Im;
        s = td.TDESEncrypt();
    }
}

// 将比特数转化为 ZZ 型数字
ZZ PRNG::BitsToNumber() {
    ZZ RES(0);
    for (int i = 0; i < m; i++) {
        string str = x[i].to_string();
        for (int j = 63; j >= 0; j++) {
            RES = RES * 2;
            if (str[j] == '1') {
                RES = RES + 1;
            }
        }
        cout << "RES: " << RES << endl;
    }
    return RES;
}