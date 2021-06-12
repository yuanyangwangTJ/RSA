#ifndef _RSA_H_
#define _RSA_H_

#include <NTL/ZZ.h>
#include <cstring>

using namespace NTL;

const unsigned char HexTable[] = "0123456789ABCDEF";

const std::string Base64Map =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";

struct PublicKey {
    ZZ n, b;
};

struct PrivateKey {
    ZZ p, q, a;
};

struct Message {
    ZZ c1;
    ZZ IV;
    std::string fileName;
};

class RSAUser {
public:
    RSAUser();
    void GenerateKey();
    void SendPublicKey(RSAUser&);
    void SendMessage(RSAUser&);
    void EncryptMessage();
    void DecryptMessage();

    PublicKey pk;
    Message M;

private:
    void createTempKey();
    void viewKey(const size_t);
    void printKey(ZZ, const size_t);
    void PrintInPEM(const std::string);
    void PrintInDER(const std::string);
    std::string ZZToBits(ZZ, const size_t);
    PrivateKey sk;
    ZZ k;       // 临时会话密钥

};


#endif