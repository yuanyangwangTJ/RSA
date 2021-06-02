#include "RSA.h"
#include <NTL/ZZ.h>

using namespace NTL;
using namespace std;

int main()
{

    RSAUser Alice, Bob;
    Alice.GenerateKey();
    Alice.SendPublicKey(Bob);
    cout << Alice.pk.b << endl;
    cout << Bob.pk.b << endl;

    return 0;
}