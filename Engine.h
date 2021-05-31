#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <bitset>
#include <cstring>
#include <NTL/ZZ.h>

using std::bitset;

const unsigned char DERTable[] = "0123456789ABCDEF";

class PRNG
{
public:
    PRNG(int);
    ~PRNG();
    void GenerateRandom();
    void SetM(int);
    NTL::ZZ BitsToNumber();
    void PrintInDER();
    int m;

private:
    void bitsToString();
    bitset<64> *x;
    std::string s;
};

class PrimeGen: public PRNG
{
public:
    PrimeGen(int);
    void GeneratePrime();
private:
};

#endif