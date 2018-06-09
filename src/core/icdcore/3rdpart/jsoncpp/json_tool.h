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
#include <assert.h>

#if defined(_MSC_VER)
#include <io.h>
#include <direct.h>
#include <stdint.h>
#elif defined(__unix__)
#include <unistd.h>
#include <sys/stat.h>
#elif defined(__APPLE__)
#include <unistd.h>
#include <stdint.h>
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

// add by iclosue

int JSON_API createPath(const std::string &path);
std::string JSON_API pathOfFile(const std::string &filePath);

bool JSON_API resolve(const char *filePath, Value &root);
bool JSON_API resolve(const std::string &filePath, Value &root);

Value JSON_API resolve(const char *filePath, const std::string &path);
Value JSON_API resolve(const std::string &filePath, const std::string &path);

bool JSON_API resolve(const char *filePath, const std::string &path, Value &value);
bool JSON_API resolve(const std::string &filePath, const std::string &path, Value &value);

Value JSON_API resolve(const Value &root, const std::string &path);

bool JSON_API parse(const std::string &content, Value &root);
Value JSON_API parse(const std::string &content, const std::string &path);
bool JSON_API parse(const std::string &content, const std::string &path, Value &value);

bool JSON_API make(const char *filePath, bool fast);
bool JSON_API make(const std::string &filePath, bool fast);

bool JSON_API make(const char *filePath, const Value &root,
                   bool create, bool fast = false);
bool JSON_API make(const std::string &filePath, const Value &root,
                   bool create, bool fast = false);

bool JSON_API make(const char *filePath, const std::string &path,
                   const Value &value, bool create, bool fast);
bool JSON_API make(const std::string &filePath, const std::string &path,
                   const Value &value, bool create, bool fast);

bool JSON_API makeArray(const char *filePath, const std::string &path,
                        const Value &value, bool unique, bool create, bool fast);
bool JSON_API makeArray(const std::string &filePath, const std::string &path,
                        const Value &value, bool unique, bool create, bool fast);

bool JSON_API replaceItem(const char *filePath, const std::string &path,
                          const Value &oldValue, const Value &newValue, bool create, bool fast);
bool JSON_API replaceItem(const std::string &filePath, const std::string &path,
                          const Value &oldValue, const Value &newValue, bool create, bool fast);

bool JSON_API removeItem(const char *filePath, const std::string &path,
                         const std::string &key, const Json::Value &value,
                         bool create, bool fast);
bool JSON_API removeItem(const std::string &filePath, const std::string &path,
                         const std::string &key, const Json::Value &value,
                         bool create, bool fast);

bool JSON_API make(Json::Value &root, const std::string &path, const Json::Value &value);

bool JSON_API merge(const char *filePath, const std::string &path,
                    const Json::Value &value, bool create, bool fast);
bool JSON_API merge(const std::string &filePath, const std::string &path,
                    const Json::Value &value, bool create, bool fast);

bool JSON_API merge(const Value &source, Value &target);

// class Serializable

class JSON_API Serializable
{
public:
    virtual ~Serializable() {}
    virtual inline std::string saveJson() const;
    virtual Value save() const = 0;
    virtual inline bool restore(const std::string &json);
    virtual bool restore(const Value &/*json*/, int /*deep*/ = -1) = 0;
    virtual void beginSerial() const {}
    virtual void endSerial() const {}
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
