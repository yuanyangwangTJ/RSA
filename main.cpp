#include "RSA.h"
#include <NTL/ZZ.h>

using namespace NTL;
using namespace std;

int main()
{
    RSAUser Alice, Bob;
    Alice.GenerateKey();
    Alice.SendPublicKey(Bob);
    Bob.EncryptMessage();
    Bob.SendMessage(Alice);
    Alice.DecryptMessage();

    return 0;
}