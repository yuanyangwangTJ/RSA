/**************************************
 * Class Name: PRNG, PrimeGen
 * Function: 伪随机数与伪随机素数比特生成器
 * Introduction: PrimeGen 继承于 PRNG 类
 * ***********************************/

#include "Random.h"
#include "TDES.h"
#include <iostream>
#include <ctime>
#include <cstring>
#include <NTL/ZZ.h>
 
using namespace std;
using namespace NTL;

typedef unsigned long long size_ul;

/*---------------------------------
 * Class: PRNG
 *-------------------------------*/

// 构造函数，进行初始化操作
PRNG::PRNG(int n) {
    m = n;
    x = new bitset<64>[m];
    s = "";
}

// 析构函数，释放内存空间
PRNG::~PRNG() {
    delete [] x;
}

// 通过外界修改 m 的数值
void PRNG::SetM(int n) {
    m = n;
    delete [] x;
    x = new bitset<64>[m];
}

// 伪随机数比特生成函数
ZZ PRNG::GenerateRandom() {
    // 获取本地时间日期
    time_t now = time(0);
    bitset<64> Date(now);

    // 产生 64 比特随机种子 s
    size_ul ss = size_ul(random()) << 32 + random();
    bitset<64> s(ss);

    TDES td;
    td.plain = Date;
    bitset<64> Im = td.TDESEncrypt();
    for (int i = 0; i < m; i++) {
        td.plain = Im ^ s;
        x[i] = td.TDESEncrypt();
        td.plain = x[i] ^ Im;
        s = td.TDESEncrypt();
    }

    // 生成字符串类型保存
    bitsToString();
    ZZ res = bitsToNumber();
    return res;
}

// 将比特数转化为 ZZ 型数字
ZZ PRNG::bitsToNumber() {
    ZZ res(0);
    for (int i = 0; i < m; i++) {
        string str = x[i].to_string();
        for (int j = 0; j < 64; j++) {
            res = res * 2;
            if (str[j] == '1') {
                res = res + 1;
            }
        }
    }
    return res;
}

// 比特位转为字符串类型（01字符串）
void PRNG::bitsToString() {
    s = "";
    for (int i = 0; i < m; i++) {
        s += x[i].to_string();
    }
}

// 以十六进制打印比特数值
void PRNG::PrintInDER() {
    cout << "modulus:";
    string pairstr = "";
    for (size_t i = 0; i < s.length(); i += 4) {
        int index = 0;
        
        for (size_t j = i; j < i + 4; j++) {
            index = index << 1;
            if (s[j] == '1') {
                index += 1;
            }
        }
        pairstr += DERTable[index];
        if (pairstr.length() == 2) {
            cout << pairstr;
            if (i + 4 < s.length()) {
                cout << ":";
            }
            pairstr = "";
        }
        if (i % 120 == 0) {
            cout << endl << '\t';
        }
    }
    cout << endl;
}


/*---------------------------------
 * Class: PrimeGen
 *-------------------------------*/

PrimeGen::PrimeGen(int n) : PRNG(n) {
    cout << "Create a pseudorandom number generator.\n";
}

// 生成随机素数，使用 NTL 中库函数检验
// 素性检测算法为 Miller-Rabin 检测
ZZ PrimeGen::GeneratePrime() {
    ZZ res(0);
    // 此处进行 srandom，为产生 64 比特随机种子
    srandom((unsigned)time(NULL));
    do {
        res = GenerateRandom();
    } while(!ProbPrime(res, m * 32));

    return res;
}