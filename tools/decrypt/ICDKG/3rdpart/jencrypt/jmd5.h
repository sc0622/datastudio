// ********************************************
// name:
// function: the encryption  of software
// description:
// author: closure
// date£º2013-04-21 16:04
//
// how to use the module:
//   jencrypt::JMd5 md5;					// define MD5 object.
//   std::string input= "12345678";			// input string.
//   md5.update(input);						// update the string of input.
//   std::string output = md5.toString();	// get the string output by using md5.toString().
//
// ********************************************

#ifndef JMD5_H
#define JMD5_H

#include "jencrypt_global.h"
#include <fstream>

namespace jencrypt {

struct JMd5Data;

class JENCRYPT_EXPORT JMd5
{
public:
    explicit JMd5();
    explicit JMd5(const void* input, size_t length);
    explicit JMd5(const std::string& input);
    explicit JMd5(std::ifstream& in);
    ~JMd5();

    void update(const void* input, size_t length);
    void update(const std::string& input);
    void update(std::ifstream& input);
    const unsigned char *digest();
    std::string toString();
    void reset();

private:
    void update(const byte* input, size_t length);
    void final();
    void transform(const byte block[64]);
    void encode(const uint32* input, byte* output, size_t length);
    void decode(const byte* input, uint32* output, size_t length);
    std::string bytesToHexString(const byte* input, size_t length);

private:
    JMd5Data *data;
};

} // end of namespace jencrypt

#endif // JMD5_H
