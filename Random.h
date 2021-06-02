#ifndef _RANDOM_H_
#define _RANDOM_H_

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
    NTL::ZZ GenerateRandom();
    void SetM(int);
    void PrintInDER();
    int m;

private:
    void bitsToString();
    NTL::ZZ bitsToNumber();
    bitset<64> *x;
    std::string s;
};

class PrimeGen: public PRNG
{
public:
    PrimeGen(int);
    NTL::ZZ GeneratePrime();
private:
};

#endif