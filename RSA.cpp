/************************************
 * ClassName: RSAUser
 * Function: 实现 RSA 加密使用者的类对象
 * *********************************/
#include "RSA.h"
#include "Random.h"
#include "AES.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstring>
#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <algorithm>

using namespace std;
using namespace NTL;

// _getch() 函数的系统移植问题解决
#ifdef _WIN32
#include <conio.h>
#elif __linux__
char _getch();
#endif

void printChoose() {
    cout << "Please choose the length of random prime: \n";
    cout << "1. 512  bits\n";
    cout << "2. 1024 bits\n";
}

RSAUser::RSAUser() {
    cout << "Create RSA user..." << endl;

}

void RSAUser::GenerateKey() {
    // 选择 RSA 素数的比特长度
    int m = 0;
    
    printChoose();
    do {
        char ch = _getch();
        switch(ch) {
            case '1': m = 8; break;
            case '2': m = 16; break;
            default: m = 0;
        }
    } while (m == 0);

    PrimeGen G(m);
    cout << "public key and private key are generating...\n";

    // 生成私钥的随机素数 p, q
    do {
        sk.p = G.GeneratePrime();
        sk.q = G.GeneratePrime();
    } while (sk.p == sk.q);

    // 生成公钥 n
    pk.n = sk.p * sk.q;
    ZZ Euler = (sk.p - 1) * (sk.q - 1);    // 欧拉函数数值

    do {
        pk.b = G.GenerateRandom() % Euler;
    } while(GCD(pk.b, Euler) != 1);

    // 使用 NTL 库中的求逆函数 InvModStatus()
    InvModStatus(sk.a, pk.b, Euler);
    cout << "All keys have been generated !\n";
    // 钥匙查看
    viewKey(m * 64);
}

// 发送 RSA 公钥
void RSAUser::SendPublicKey(RSAUser &B) {
    B.pk = this->pk;
}

// 创建临时密钥
void RSAUser::createTempKey() {
    PRNG G(2);
    k = G.GenerateRandom();
    cout << "Temp key has been created.\n";
    cout << "Do you want to view temp key, y/n ?\n";
    char ch = _getch();
    switch (ch) {
        case 'Y':
        case 'y':
            cout << "temp key\n";
            printKey(k, 128);
            break;
        default:
            break;
    }
}

// 加密信息，使用 CBC 模式
void RSAUser::EncryptMessage() {
    // 生成临时密钥
    createTempKey();
    // 利用公钥加密 k 得到 c1，此处使用 ZZ_p 类计算
    ZZ_p::init(pk.n);
    ZZ_p k_p = to_ZZ_p(k);
    M.c1 = rep(power(k_p, pk.b));

    // 将临时密钥 k 转化为 128 比特形式
    bitset<128> key(to_ulong(k));
    // 以此创建 AES 加密系统
    AES E(key);

    // 需要加密的文件路径输入
    string fileName, newfileName;
    cout << "Please input the file path: \n";
    cin >> fileName;
    _getch();   // 接收回车符

    // 密文存储为 源文件名 + ".cipher"
    newfileName = fileName + ".cipher";
    // 文件名作为信息传输
    M.fileName = fileName;

    // 打开文件
    ifstream fin(fileName, ios::binary);
    if (!fin) {
        cerr << "open file error.\n";
        exit(-1);
    }
    ofstream fout(newfileName, ios::binary);
    if (!fout) {
        cerr << "open file error.\n";
        exit(-1);
    }

    // 因为逐字符读取文件比较慢，使用缓存区的方式一次性读取 16*1024 字节
    bitset<128> buffer[512];
    // CBC 模式
    // former 记录密文 y(i-1) ，初始化为一个随机数
    PRNG G(2);
    M.IV = G.GenerateRandom();
    bitset<128> former(to_ulong(M.IV));
    // 将 IV 通过 RSA 加密发送
    ZZ_p IV_p = to_ZZ_p(M.IV);
    M.IV = rep(power(IV_p, pk.b));

    bitset<128> cipher;     // 密文

    // 开始读取需要加密的文件
    while (fin && !fin.eof()) {
        // 一次性读取最多 16*1024 字节的内容
        fin.read((char*)&buffer, 16*512);
        streamsize readNum = fin.gcount();
        streamsize i;
        for (i = 0; i < readNum / 16; i++) {
            cipher = E.AESEncrypt(former ^ buffer[i]);
            former = cipher;
            // 写入密文至文件中
            fout.write((char*)&cipher, 16);
        }
        streamsize end = readNum % 16;
        // 文件读取结束操作，使用 PKCS7Padding 处理末尾
        if (fin.eof()) {
            // 末尾填充的数字
            bitset<128> pkcs(16 - end);
            pkcs = pkcs << 120;
            for (streamsize j = 16; j > end; j--) {
                streamsize k = j - end - 1;
                buffer[i] = ((buffer[i] << k*8) ^ pkcs) >> k*8; 
            }
            cipher = E.AESEncrypt(former ^ buffer[i]);
            fout.write((char*)&cipher, 16);
        }
    }

    cout << "File encryption finished.\n";
    cout << "Encrypted file is located at " << newfileName << endl;
    // 关闭文件
    fin.close();
    fout.close();
}

// 解密信息
void RSAUser::DecryptMessage() {
    // 利用密钥来解密 c1，得到临时密钥 k
    ZZ_p::init(pk.n);
    ZZ_p c1_p = to_ZZ_p(M.c1);
    k = rep(power(c1_p, sk.a));
    // 解密得到 IV
    ZZ_p IV_p = to_ZZ_p(M.IV);
    M.IV = rep(power(IV_p, sk.a));

    // 查看临时密钥
    cout << "Temp key has been decrypted.\n";
    cout << "Do you want to view temp key, y/n ?\n";
    char ch = _getch();
    switch (ch) {
        case 'Y':
        case 'y':
            cout << "temp key\n";
            printKey(k, 128);
            break;
        default:
            break;
    }

    // 使用密钥 k 解密
    bitset<128> key(to_ulong(k));
    // 创建 AES 加密解密系统
    AES D(key);

    // 密文文件
    string fileName = M.fileName + ".cipher";
    
    // 查找文件的名称中的 '\' 或者 '/' 最后出现位置
    int pos = M.fileName.length() - 1;
    for (; pos >= 0; pos--) {
        if (M.fileName[pos] == '/' || M.fileName[pos] == '\\')
            break;
    }
    // 解密文件位置
    string newfileName = M.fileName.insert(pos + 1, "new_");

    // 打开文件
    ifstream fin(fileName, ios::binary);
    if (!fin) {
        cerr << "open file error.\n";
        exit(-1);
    }
    ofstream fout(newfileName, ios::binary);
    if (!fout) {
        cerr << "open file error.\n";
        exit(-1);
    }

    // buffer 缓冲区读取
    bitset<128> buffer[512];
    // former 记录密文 C(i-1)，初始化为 M.IV
    bitset<128> former(to_ulong(M.IV));
    bitset<128> plain;      // 明文

    // 开始读取解密文件
    while (fin && !fin.eof()) {
        fin.read((char*)&buffer, 16*512);
        streamsize readNum = fin.gcount();

        bool flag = false;  // 判断是否结束的标志
        if (fin.eof() || fin.peek() == EOF) {
            flag = true;
        }
        streamsize i, bufferMax = readNum / 16;
        // 使用 end 记录需要解密的缓冲区最大编号
        if (flag) bufferMax--;

        for (i = 0; i < bufferMax; i++) {
            plain = D.AESDecrypt(buffer[i]) ^ former;
            former = buffer[i];
            // 写入解密文件
            fout.write((char*)&plain, 16);
        }
        // 处理 PKCS7Padding 结尾
        if (flag) {
            // end 表示额外填充的字节数
            // 最后一次解密
            plain = D.AESDecrypt(buffer[i]) ^ former;
            streamsize end = (plain >> 120).to_ulong();
            for (streamsize j = 16; j > end; j--) {
                // 最后的字节处理
                bitset<8> text((plain << (j-1)*8 >> 120).to_ulong());
                fout.write((char*)&text, 1);
            }
        }
    }

    cout << "File decryption finished.\n";
    cout << "Decrypted file is located at " << newfileName << endl;
    fin.close();
    fout.close();
}

// 发送信息
void RSAUser::SendMessage(RSAUser& A) {
    A.M = this->M;
}

// PEM 格式打印输出
void RSAUser::PrintInPEM(const string s) {
	string res = "";
	size_t len = s.length() / 8;	// 比特串转化为字节的长度
	size_t i;
	unsigned char triBytes[3];		// 存储三个字节

	for (i = 0; i+3 <= len; i += 3) {
		for (int j = 0; j < 3; j++) {
			bitset<8> tmp(s.substr(8*i + 8*j, 8));
			triBytes[j] = tmp.to_ulong();
		}
		res += Base64Map[triBytes[0] >> 2];
		res += Base64Map[((triBytes[0]<<4) & 0x30) | (triBytes[1] >> 4)];
		res += Base64Map[((triBytes[1]<<2) & 0x3c) | (triBytes[2] >> 6)];
		res += Base64Map[triBytes[2] & 0x3f];
	}

	if (i < len) {
		if (len - i == 1) {
			bitset<8> tmp(s.substr(8*i, 8));
			triBytes[0] = tmp.to_ulong();
			res += Base64Map[triBytes[0] >> 2];
			res += Base64Map[(triBytes[0]<<4) & 0x30];
			res += "==";
		} else {
			for (int j = 0; j < 2; j++) {
				bitset<8> tmp(s.substr(8*i + 8*j, 8));
				triBytes[j] = tmp.to_ulong();
			}
			res += Base64Map[triBytes[0] >> 2];
			res += Base64Map[((triBytes[0]<<4) & 0x30) | (triBytes[1] >> 4)];
			res += Base64Map[(triBytes[1]<<2) & 0x3c];
			res += "=";
		}
	}

	cout << res << endl;
}

// DER(十六进制)格式打印
void RSAUser::PrintInDER(const string s) {
    cout << "modulus:";
    string pairstr = "";
    for (size_t i = 0; i < s.length(); i += 4) {
        int index = 0;
        
        for (size_t j = i; j < i + 4; j++) {
            index = index << 1;
            if (s[j] == '1') {
                index += 1;
            }
        }
        pairstr += HexTable[index];
        if (pairstr.length() == 2) {
            cout << pairstr;
            if (i + 4 < s.length()) {
                cout << ":";
            }
            pairstr = "";
        }
        if (i % 120 == 0) {
            cout << endl << '\t';
        }
    }
    cout << endl;
}

// ZZ 类型转化为 Bits
string RSAUser::ZZToBits(ZZ num, const size_t n) {
	string s = "";
	ZZ last;

	while (num != 0) {
		last = num % 2;
		if (last == 1) {
			s += '1';
		} else {
			s += '0';
		}
		num /= 2;
	}
	for (size_t i = s.length(); i < n; i++) {
		s += '0';
	}
	reverse(s.begin(), s.end());
	return s;
}

// 选择输出格式打印
void printFormat() {
    cout << "Please choose the format to print key :\n";
    cout << "1. DER (hexadecimal)\n";
    cout << "2. PEM\n";
}

// 查看钥匙，n 为 p,q 的比特位数
void RSAUser::viewKey(const size_t n) {
    // 查看公钥
    cout << "Do you want to view public key, y/n ?\n";
    char ch = _getch();

    switch(ch) {
        case 'Y':
        case 'y': {
            cout << "Public key\n";
            cout << "Print n in public key :\n";
            printKey(pk.n, n*2);
            cout << "Print b in public key :\n";
            printKey(pk.b, n*2);
            break;
        }
        default: break;
    }
    // 查看私钥
    cout << "Do you want to view private key, y/n ?\n";
    ch = _getch();

    if (ch == 'y' || ch == 'Y') {
        cout << "Attention, please not reveal this information !!!\n";
        cout << "Read the warning, continue, y/n ?\n";
        ch = _getch();

        switch(ch) {
            case 'Y':
            case 'y': {
                cout << "Private key\n";
                cout << "Print p in private key :\n";
                printKey(sk.p, n);
                cout << "Print q in private key :\n";
                printKey(sk.q, n);
                cout << "Print a in private key :\n";
                printKey(sk.a, n*2);
                break;
            }
            default: break;
        }
    }

}

// 打印每一个钥匙
void RSAUser::printKey(ZZ num, const size_t n) {
    string s = ZZToBits(num, n);
    printFormat();
    bool flag = false;
    while (!flag) {
        char ch = _getch();
        switch(ch) {
            case '1': {
                PrintInDER(s);
                flag = true;
                break;
            }
            case '2': {
                PrintInPEM(s);
                flag = true;
                break;
            }
            default: break;
        }
    }
}