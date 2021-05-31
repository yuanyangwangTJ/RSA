/*****************************************
 * ClassName: PrimeGen
 * Function: 生成伪随机素数，此类继承与 PRNG 类
 * ***************************************/

#include "Engine.h"
#include <iostream>
#include <NTL/ZZ.h>

using namespace std;
using namespace NTL;

PrimeGen::PrimeGen(int n) : PRNG(n) {
    GeneratePrime(); 
}

// 生成随机素数，使用 NTL 中库函数检验
// 素性检测算法为 Miller-Rabin 检测
void PrimeGen::GeneratePrime() {
    // 此处进行 srand，为产生 64 比特随机种子
    srandom((unsigned)time(NULL));
    ZZ res(0);
    do {
        GenerateRandom();
        res = BitsToNumber();
    } while(!ProbPrime(res, m * 32));
    cout << res << endl;

}