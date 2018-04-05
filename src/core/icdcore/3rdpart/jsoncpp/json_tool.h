// Copyright 2007-2010 Baptiste Lepilleur and The JsonCpp Authors
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef LIB_JSONCPP_JSON_TOOL_H_INCLUDED
#define LIB_JSONCPP_JSON_TOOL_H_INCLUDED


// Also support old flag NO_LOCALE_SUPPORT
#ifdef NO_LOCALE_SUPPORT
#define JSONCPP_NO_LOCALE_SUPPORT
#endif

#ifndef JSONCPP_NO_LOCALE_SUPPORT
#include <clocale>
#endif
#if !defined(JSON_IS_AMALGAMATION)
#include "json/value.h"
#include "json/reader.h"
#include "json/writer.h"
#endif // if !defined(JSON_IS_AMALGAMATION)

#include <sstream>
#include <fstream>
#include <iostream>
#include <io.h>
#include <assert.h>

#if defined(_MSC_VER)
#include <direct.h>
#include <stdint.h>
#elif defined(__unix__)
#include <unistd.h>
#include <sys/stat.h>
#else
#error(not supported os!)
#endif

/* This header provides common string manipulation support, such as UTF-8,
 * portable conversion from/to string...
 *
 * It is an internal header that must not be exposed.
 */

namespace Json {
static char getDecimalPoint() {
#ifdef JSONCPP_NO_LOCALE_SUPPORT
    return '\0';
#else
    struct lconv* lc = localeconv();
    return lc ? *(lc->decimal_point) : '\0';
#endif
}

/// Converts a unicode code-point to UTF-8.
static inline JSONCPP_STRING codePointToUTF8(unsigned int cp) {
    JSONCPP_STRING result;

    // based on description from http://en.wikipedia.org/wiki/UTF-8

    if (cp <= 0x7f) {
        result.resize(1);
        result[0] = static_cast<char>(cp);
    } else if (cp <= 0x7FF) {
        result.resize(2);
        result[1] = static_cast<char>(0x80 | (0x3f & cp));
        result[0] = static_cast<char>(0xC0 | (0x1f & (cp >> 6)));
    } else if (cp <= 0xFFFF) {
        result.resize(3);
        result[2] = static_cast<char>(0x80 | (0x3f & cp));
        result[1] = static_cast<char>(0x80 | (0x3f & (cp >> 6)));
        result[0] = static_cast<char>(0xE0 | (0xf & (cp >> 12)));
    } else if (cp <= 0x10FFFF) {
        result.resize(4);
        result[3] = static_cast<char>(0x80 | (0x3f & cp));
        result[2] = static_cast<char>(0x80 | (0x3f & (cp >> 6)));
        result[1] = static_cast<char>(0x80 | (0x3f & (cp >> 12)));
        result[0] = static_cast<char>(0xF0 | (0x7 & (cp >> 18)));
    }

    return result;
}

enum {
    /// Constant that specify the size of the buffer that must be passed to
    /// uintToString.
    uintToStringBufferSize = 3 * sizeof(LargestUInt) + 1
};

// Defines a char buffer for use with uintToString().
typedef char UIntToStringBuffer[uintToStringBufferSize];

/** Converts an unsigned integer to string.
 * @param value Unsigned integer to convert to string
 * @param current Input/Output string buffer.
 *        Must have at least uintToStringBufferSize chars free.
 */
static inline void uintToString(LargestUInt value, char*& current) {
    *--current = 0;
    do {
        *--current = static_cast<char>(value % 10U + static_cast<unsigned>('0'));
        value /= 10;
    } while (value != 0);
}

/** Change ',' to '.' everywhere in buffer.
 *
 * We had a sophisticated way, but it did not work in WinCE.
 * @see https://github.com/open-source-parsers/jsoncpp/pull/9
 */
static inline void fixNumericLocale(char* begin, char* end) {
    while (begin < end) {
        if (*begin == ',') {
            *begin = '.';
        }
        ++begin;
    }
}

static inline void fixNumericLocaleInput(char* begin, char* end) {
    char decimalPoint = getDecimalPoint();
    if (decimalPoint != '\0' && decimalPoint != '.') {
        while (begin < end) {
            if (*begin == '.') {
                *begin = decimalPoint;
            }
            ++begin;
        }
    }
}

// custom

#ifndef JSON_MAX_PATH_LEN
#define JSON_MAX_PATH_LEN 256
#endif

#ifdef _MSC_VER
#define JSON_ACCESS(fileName, accessMode) \
    _access(fileName, accessMode)
#define JSON_MKDIR(path) _mkdir(path)
#else
#define JSON_ACCESS(fileName, accessMode) \
    access(fileName, accessMode)
#define JSON_MKDIR(path) mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#endif

// add by iclosue
static inline int createPath(const std::string &path)
{
    uint32_t pathLength = path.length();
    if (pathLength > JSON_MAX_PATH_LEN) {
        return -1;
    }

    char tPath[JSON_MAX_PATH_LEN] = {0};
    for (uint32_t i = 0;i < pathLength; ++i) {
        tPath[i] = path[i];
        if (tPath[i] == '\\' || tPath[i] == '/') {
            if (JSON_ACCESS(tPath, 0) != 0) {
                int ret = JSON_MKDIR(tPath);
                if (ret != 0) {
                    return ret;
                }
            }
        }
    }

    return 0;
}

static inline std::string pathOfFile(const std::string &filePath)
{
    std::string path = filePath;
    int index = path.find_last_of('/');
    if (index == -1) {
        index = path.find_last_of('\\');
    }

    if (index == -1) {
        path = std::string();
    } else {
        path = path.substr(0, index + 1);
    }

    return path;
}

static inline bool resolve(const std::string &filePath, Value &root)
{
    if (filePath.empty()) {
        printf("filePath is empty!");
        return false;
    }

    const std::string path = pathOfFile(filePath);
    if (!path.empty()) {
        //return false;
    }

    std::ifstream ifs(filePath);
    if (!ifs) {
        printf("File \"%s\" open failure!\n", filePath.c_str());
        return false;
    }

    try {
        root.clear();
        ifs >> root;
    } catch (RuntimeError msg) {
        printf("Parse json file \"%s\":\n%s\n", filePath.c_str(), msg.what());
        return false;
    }

    ifs.close();

    return true;
}

static inline Value resolve(const std::string &filePath, const std::string &path)
{
    Value root;
    if (!resolve(filePath, root)) {
        return Value::nullSingleton();
    }

    try {
        return Path(path).resolve(root);
    } catch (RuntimeError msg) {
        printf("Parse json file \"%s\":\n%s\n", filePath.c_str(), msg.what());
        return Value::null;
    }
}

static inline Value resolve(const Value &root, const std::string &path)
{
    try {
        return Path(path).resolve(root);
    } catch (RuntimeError msg) {
        printf("Parse json value with path \"%s\":\n%s\n", path.c_str(), msg.what());
        return Value::null;
    }
}

static inline bool parse(const std::string &document, Value &root)
{
    return Reader().parse(document, root);
}

static inline Value parse(const std::string &document, const std::string &path)
{
    Value root;
    if (!parse(document, root)) {
        return Value::nullSingleton();
    }

    try {
        return Path(path).resolve(root);
    } catch (RuntimeError msg) {
        printf("Parse json document with path \"%s\":\n%s\n", path.c_str(), msg.what());
        return Value::null;
    }
}

static inline bool make(const std::string &filePath, const Value &root,
                        bool create, bool fast = false)
{
    if (filePath.empty()) {
        printf("filePath is empty!");
        return false;
    }

    const std::string path = pathOfFile(filePath);
    if (!path.empty() && create) {
        createPath(path);
    }

    std::string contents;
    if (fast) {
        contents = FastWriter().write(root);
    } else {
        contents = StyledWriter().write(root);
    }

    std::ofstream ofs;
    ofs.open(filePath);
    if (!ofs.is_open()) {
        return false;
    }

    try {
        ofs << contents;
    } catch (RuntimeError msg) {
        printf("Save json file \"%s\":\n%s\n", filePath.c_str(), msg.what());
        return false;
    }

    ofs.close();

    return true;
}

static inline bool make(const std::string &filePath, const std::string &path,
                        const Value &value, bool create, bool fast = false)
{
    Value root;
    if (!resolve(filePath, root) && !create) {
        return false;
    }

    try {
        Path(path).make(root) = value;
    } catch (RuntimeError msg) {
        printf("Save json file \"%s\":\n%s\n", filePath.c_str(), msg.what());
        return false;
    }

    return make(filePath, root, create, fast);
}

static inline bool make(Json::Value &root, const std::string &path,
                        const Json::Value &value)
{
    try {
        Path(path).make(root) = value;
    } catch (RuntimeError msg) {
        printf("Save json value with path \"%s\":\n%s\n", path.c_str(), msg.what());
        printf("%s\n", msg.what());
        return false;
    }

    return true;
}

static inline bool merge(const Value &source, Value &target)
{
    if (source.isNull()) {
        return true;
    }

    switch (source.type()) {
    case arrayValue:
    {
        for (ValueConstIterator citer = source.begin();
             citer != source.end(); ++citer) {
            target.append(*citer);
        }
        break;
    }
    case objectValue:
    {
        for (ValueConstIterator citer = source.begin();
             citer != source.end(); ++citer) {
            target[citer.name()] = *citer;
        }
        break;
    }
    default:
        target = source;
        break;
    }

    return true;
}

// class Serializable

class JSON_API Serializable
{
public:
    virtual ~Serializable() {}
    virtual inline std::string saveJson() const;
    virtual Value save() const = 0;
    virtual inline bool restore(const std::string &json);
    virtual bool restore(const Value &/*json*/, int /*deep*/ = -1) = 0;
};

std::string Serializable::saveJson() const
{
    Value value = save();
    return value.toStyledString();
}

bool Serializable::restore(const std::string &json)
{
    Reader reader;
    Value value;
    if (!reader.parse(json, value)) {
        return false;
    }
    return restore(value);
}

} // namespace Json {

#endif // LIB_JSONCPP_JSON_TOOL_H_INCLUDED
