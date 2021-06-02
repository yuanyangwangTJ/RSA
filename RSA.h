#ifndef _RSA_H_
#define _RSA_H_

#include <NTL/ZZ.h>

using namespace NTL;

struct PublicKey {
    ZZ n, b;
};

struct PrivateKey {
    ZZ p, q, a;
};

class RSAUser {
public:
    RSAUser();
    void GenerateKey();
    void SendPublicKey(RSAUser&);
    void SendMessege(RSAUser);

    PublicKey pk;


private:
    PrivateKey sk;

};



#endif