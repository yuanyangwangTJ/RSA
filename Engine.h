#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <bitset>
#include <NTL/ZZ.h>

using std::bitset;

class PRNG
{
public:
    PRNG(int);
    void GenerateRandom();
    void SetM(int);
    NTL::ZZ BitsToNumber();

private:
    bitset<64> s;       // 64 比特随机种子
    int m;
    bitset<64> *x;
};

#endif