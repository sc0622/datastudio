// ********************************************
// name:
// function: the encryption  of software
// description:
// author: iclosure
// date：2013-04-21 16:04
//
// how to use the module:
//   jencrypt::JDes des;              // define JDes object.
//   clock_t start, end;
//
//   start = clock();
//   des.des_encrypt("1.txt", "key.txt", "2.txt");
//   end = clock();
//   printf("加密消耗%d毫秒\n", end - start);
//
//   system("pause");
//
//   start =clock();
//   des.des_decrypt("2.txt", "key.txt", "3.txt");
//   end = clock();
//   printf("解密消耗%d毫秒\n", end - start);
//
// ********************************************

// DES (Data Encrypt Standard, 数据库加密标准)

#ifndef JDES_H
#define JDES_H

#include "jencrypt_global.h"

namespace jencrypt {

typedef unsigned char	byte;
typedef unsigned int	uint32;

struct JDesData;

class JENCRYPT_EXPORT JDes
{
public:
    explicit JDes();
    ~JDes();

    int encrypt(const char* plain_file, const char* key_str, const char* cipher_file);
    int decrypt(const char* cipher_file, const char* key_str, const char* plain_file);

private:
	int byte_to_bit(const byte* src, byte *des, size_t n);
	int bit_to_byte(const byte* src, byte *des, size_t n);
	int transform(const byte* src, byte* des, const byte* table, size_t table_size);
	int pc1_transform(const byte src[64], byte des[56]);
	int pc2_transform(const byte src[56], byte des[48]);
	int ip_transform(byte data[64]);
	int ip_1_transform(byte data[64]);
	int e_transform(byte data[48]);
	int p_transform(byte data[32]);
	int s_box(const byte src[48], byte des[32]);
	int rol(byte data[56], int time);
	int _xor(byte* a, const byte* b, size_t size);
	int swap(byte left[32], byte right[32]);
	int make_sub_keys(byte key[64], byte sub_keys[16][48]);
	int encrypt_block(byte plain_block[8], byte sub_keys[16][48], byte cipher_file[8]);
	int decrypt_block(byte cipher_block[8], byte sub_keys[16][48], byte plain_block[8]);

private:
    JDesData *data;
};

} // end of namespace jencrypt

#endif // JDES_H
