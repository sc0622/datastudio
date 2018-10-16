#include "precomp.h"
#include "icdcore_global.h"
#include "3rdpart/jsoncpp/json/json.h"
#include "3rdpart/jsoncpp/json_tool.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <assert.h>

#ifdef _MSC_VER
#include <io.h>
#include <direct.h>
#include <stdint.h>
#include <objbase.h>
#elif defined(__unix__)
#include <unistd.h>
#include <sys/stat.h>
#elif defined(__APPLE__)
#include <unistd.h>
#include <sys/stat.h>
#else
#error(not supported os!)
#endif

#ifndef MAX_PATH_LEN
#define MAX_PATH_LEN 256
#endif

#ifdef _MSC_VER
#define ACCESS(fileName, accessMode) \
    _access(fileName, accessMode)
#define MKDIR(path) _mkdir(path)
#else
#define ACCESS(fileName, accessMode) \
    access(fileName, accessMode)
#define MKDIR(path) mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#endif

namespace Icd {

// functions

bool startsWith(const std::string &str, const std::string &header)
{
    return (str.compare(0, header.size(), header) == 0);
}

bool endsWith(const std::string &str, const std::string &tail)
{
    return (str.compare(str.size() - tail.size(), tail.size(), tail) == 0);
}

int atoi(const std::string &str)
{
    std::istringstream iss(str);
    if (startsWith(str, "0x")) {
        iss >> std::hex;
    }
    int value = 0;
    iss >> value;
    return value;
}

unsigned int atou(const std::string &str)
{
    std::istringstream iss(str);
    if (startsWith(str, "0x")) {
        iss >> std::hex;
    }
    unsigned int value = 0;
    iss >> value;
    return value;
}

icd_uint64 strtou64(const std::string &str, int radix)
{
#if defined(_MSC_VER)
    return _strtoui64(str.c_str(), nullptr, radix);
#else
    return strtoull(str.c_str(), nullptr, radix);
#endif
}

icd_uint64 atou64(const std::string &str)
{
    std::istringstream iss(str);
    if (startsWith(str, "0x")) {
        iss >> std::hex;
    }
    icd_uint64 value = 0;
    iss >> value;
    return value;
}

std::string itoa(int value, bool hex, int field)
{
    std::ostringstream oss;
    if (hex) {
        oss << "0x" << std::hex;
    }
    if (field > 0) {
        oss << std::setw(field) << std::setfill('0');
    }
    oss << value;
    return oss.str();
}

std::string utoa(unsigned int value, bool hex, int field)
{
    std::ostringstream oss;
    if (hex) {
        oss << "0x" << std::hex;
    }
    if (field > 0) {
        oss << std::setw(field) << std::setfill('0');
    }
    oss << value;
    return oss.str();
}

std::string u64toa(icd_uint64 value, bool hex, int field)
{
    std::ostringstream oss;
    if (hex) {
        oss << "0x" << std::hex;
    }
    if (field > 0) {
        oss << std::setw(field) << std::setfill('0');
    }
    oss << value;
    return oss.str();
}

double atod(const std::string &str)
{
    std::istringstream iss(str);
    if (startsWith(str, "0x")) {
        iss >> std::hex;
    }
    double value = 0;
    iss >> value;
    return value;
}

std::string dtoa(double value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

void tolower(std::string &str)
{
    std::transform(str.cbegin(), str.cend(), str.begin(), ::tolower);
}

void toupper(std::string &str)
{
    std::transform(str.cbegin(), str.cend(), str.begin(), ::toupper);
}

std::string tolowered(const std::string &str)
{
    std::string _str;
    _str.resize(str.size());
    std::transform(str.cbegin(), str.cend(), _str.begin(), ::tolower);
    return _str;
}

std::string touppered(const std::string &str)
{
    std::string _str;
    _str.resize(str.size());
    std::transform(str.cbegin(), str.cend(), _str.begin(), ::toupper);
    return _str;
}

std::string stringSection(const char *ch, char sep, int start, int end)
{
    return stringSection(std::string(ch), std::string(&sep, 1), start, end);
}

std::string stringSection(const std::string &str, char sep, int start, int end)
{
    return stringSection(str, std::string(&sep, 1), start, end);
}

std::string stringSection(const char *ch, const std::string &sep, int start, int end)
{
    return stringSection(std::string(ch), sep, start, end);
}

std::string stringSection(const std::string &str, const std::string &sep, int start, int end)
{
    if (str.empty()) {
        return std::string();
    }

    if (start == 0 && end == -1) {
        return str;
    }

    std::vector<std::string> sections;
    splitString(str, sep, sections, false);

    const int sectionsSize = int(sections.size());

    if (start < 0) {
        start += sectionsSize;
    }
    if (end < 0) {
        end += sectionsSize;
    }

    if (start >= sectionsSize || end < 0 || start > end) {
        return std::string();
    }

    std::string ret;
    for (int x = 0, i = 0; x <= end && i < sectionsSize; ++i) {
        const std::string &section = sections.at(std::vector<std::string>::size_type(i));
        const bool empty = section.empty();
        if (x >= start) {
            if (x > start && i > 0) {
                ret += sep;
            }
            ret += section;
        }
        if (!empty) {
            x++;
        }
    }

    return ret;
}

void splitString(const std::string &str, const std::string &delim,
                 std::vector<std::string> &ret, bool keepEmptyParts)
{
    ret.clear();
    std::size_t last = 0;
    std::size_t index = str.find_first_of(delim, last);
    std::string sTemp;
    while (index != std::string::npos) {
        sTemp = str.substr(last, index - last);
        if (!keepEmptyParts) {
            sTemp = trimString(sTemp);
        }
        ret.push_back(sTemp);
        last = index + 1;
        index = str.find_first_of(delim, last);
    }

    if (index - last > 0) {
        sTemp = str.substr(last, index - last);
        if (!keepEmptyParts) {
            sTemp = trimString(sTemp);
        }
        ret.push_back(sTemp);
    }
}

std::string trimString(const std::string &str)
{
    if (str.empty()) {
        return std::string();
    }

    std::string::size_type pos = str.find_first_not_of(' ');
    if (pos == std::string::npos) {
        return str;
    }

    std::string::size_type pos2 = str.find_last_not_of(' ');
    if (pos2 != std::string::npos) {
        return str.substr(pos, pos2 - pos + 1);
    }

    return str.substr(pos);
}

std::string &replaceString(std::string &str, const std::string &old_str,
                           const std::string &new_str)
{
    std::string::size_type pos = 0;
    const std::string::size_type old_length = old_str.size();
    const std::string::size_type new_length = new_str.size();
    while ((pos = str.find(old_str, pos)) != std::string::npos) {
        str.replace(pos, old_length, new_str);
        pos += new_length;
    }

    return str;
}

// add by iclosue
int createPath(const std::string &path)
{
    return Json::createPath(path);
}

std::string pathOfFile(const std::string &filePath)
{
    return Json::pathOfFile(filePath);
}

int asciiCountOfSize(int format, int size)
{
    switch (format) {
    case 2: return size * 8;
    case 8:
        switch (size) {
        case 1: return 3;
        case 2: return 6;
        case 4: return 11;
        case 8: return 22;
        default: break;
        }
        break;
    case 16: return size * 2;
    default:
        break;
    }

    return 0;
}

std::string createUuid()
{
    std::string ret;
    createUuid(ret);
    return ret;
}

void createUuid(std::string &uuid)
{
#ifdef _MSC_VER
    GUID guid;
    ::CoCreateGuid(&guid);

    char buf[40] = {0};         //   8  4   4   2   2   2   2   2   2   2   2
    sprintf_s(buf, sizeof(buf), "{%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X}",
        guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1],
        guid.Data4[2], guid.Data4[3],
        guid.Data4[4], guid.Data4[5],
        guid.Data4[6], guid.Data4[7]);
    uuid = std::string(buf);

#elif defined(__unix__)
#error "not implement"
#elif defined(__APPLE__)
#error "not implement"
#endif
}

bool initialize()
{
#ifdef _MSC_VER
    ::CoInitialize(nullptr);
#else
#endif
    return true;
}

void uninitialize()
{
#ifdef _MSC_VER
    ::CoUninitialize();
#else
#endif
}

}
