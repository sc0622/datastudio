#include "precomp.h"
#include "icdcore_global.h"
#include "3rdpart/jsoncpp/json/json.h"
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

std::string Serializable::saveJson() const
{
    Json::Value value = save();
    return value.toStyledString();
}

bool Serializable::restore(const std::string &json)
{
    Json::Reader reader;
    Json::Value value;
    if (!reader.parse(json, value)) {
        return false;
    }
    return restore(value);
}

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

std::string itoa(int value, bool hex)
{
    std::ostringstream oss;
    if (hex) {
        oss << "0x" << std::hex;
    }
    oss << value;
    return oss.str();
}

std::string u64toa(icd_uint64 value, bool hex)
{
    std::ostringstream oss;
    if (hex) {
        oss << "0x" << std::hex;
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

std::string stringSection(const std::string &str, char sep, int start, int end)
{
    return stringSection(str, std::string(&sep, 1), start, end);
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

    const int sectionsSize = (int)sections.size();

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
    int first_i = start, last_i = end;
    for (int x = 0, i = 0; x <= end && i < sectionsSize; ++i) {
        const std::string &section = sections.at(i);
        const bool empty = section.empty();
        if (x >= start) {
            if(x == start) {
                first_i = i;
            }
            if(x == end) {
                last_i = i;
            }
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
    uint32_t pathLength = path.length();
    if (pathLength > MAX_PATH_LEN) {
        return -1;
    }

    char tPath[MAX_PATH_LEN] = {0};
    for (uint32_t i = 0;i < pathLength; ++i) {
        tPath[i] = path[i];
        if (tPath[i] == '\\' || tPath[i] == '/') {
            if (ACCESS(tPath, 0) != 0) {
                int ret = MKDIR(tPath);
                if (ret != 0) {
                    return ret;
                }
            }
        }
    }

    return 0;
}

std::string pathOfFile(const std::string &filePath)
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

}

namespace Json {

bool resolve(const std::string &filePath, Json::Value &root)
{
    if (filePath.empty()) {
        std::cout << "filePath is empty!";
        return false;
    }

    const std::string path = Icd::pathOfFile(filePath);
    if (!path.empty()) {
        //return false;
    }

    std::ifstream ifs(filePath);
    if (!ifs) {
        std::cout << "File \"" << filePath << "\" open failure!";
        return false;
    }

    try {
        root.clear();
        ifs >> root;
    } catch (Json::RuntimeError msg) {
        printf("%s\n", msg.what());
        return false;
    }

    ifs.close();

    return true;
}

Json::Value resolve(const std::string &filePath, const std::string &path)
{
    Json::Value root;
    if (!resolve(filePath, root)) {
        return Json::Value::nullSingleton();
    }

    try {
        return Json::Path(path).resolve(root);
    } catch (Json::RuntimeError msg) {
        printf("%s\n", msg.what());
        return Json::Value::null;
    }
}

Json::Value resolve(const Json::Value &root, const std::string &path)
{
    try {
        return Json::Path(path).resolve(root);
    } catch (Json::RuntimeError msg) {
        printf("%s\n", msg.what());
        return Json::Value::null;
    }
}

bool make(const std::string &filePath, const Json::Value &root, bool create, bool fast)
{
    if (filePath.empty()) {
        std::cout << "filePath is empty!";
        return false;
    }

    const std::string path = Icd::pathOfFile(filePath);
    if (!path.empty() && create) {
        Icd::createPath(path);
    }

    std::string contents;
    if (fast) {
        contents = Json::FastWriter().write(root);
    } else {
        contents = Json::StyledWriter().write(root);
    }

    std::ofstream ofs;
    ofs.open(filePath);
    if (!ofs.is_open()) {
        return false;
    }

    try {
        ofs << contents;
    } catch (Json::RuntimeError msg) {
        printf("%s\n", msg.what());
        return false;
    }

    ofs.close();

    return true;
}

bool make(const std::string &filePath, const std::string &path,
                 const Json::Value &value, bool create, bool fast)
{
    Json::Value root;
    if (!resolve(filePath, root) && !create) {
        return false;
    }

    try {
        Json::Path(path).make(root) = value;
    } catch (Json::RuntimeError msg) {
        printf("%s\n", msg.what());
        return false;
    }

    return make(filePath, root, create, fast);
}

bool merge(const Json::Value &source, Json::Value &target)
{
    if (source.isNull()) {
        return true;
    }

    switch (source.type()) {
    case Json::arrayValue:
    {
        for (Json::ValueConstIterator citer = source.begin();
             citer != source.end(); ++citer) {
            target.append(*citer);
        }
        break;
    }
    case Json::objectValue:
    {
        for (Json::ValueConstIterator citer = source.begin();
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

} // end of namespace Json
