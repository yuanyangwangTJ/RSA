#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <bitset>
#include <NTL/ZZ.h>

using std::bitset;

const unsigned char DERTable[] = "0123456789ABBCDEEF";

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
    bitset<64> *x;
};

class PrimeGen: public PRNG
{
public:
    PrimeGen(int);
    void GeneratePrime();
private:
};

#endif