#include <NTL/ZZ.h>
#include <iostream>

using namespace std;
using namespace NTL;

int main()
{
    ZZ a(123123123);
    a = a << 13;
    cout << a ;
    return 0;
}