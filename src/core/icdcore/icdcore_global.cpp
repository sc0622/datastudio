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

// class JJson

bool JJson::parse(const std::string &filePath, Json::Value &rootJson,
                  bool create)
{
    rootJson.clear();

    if (filePath.empty()) {
        std::cout << "filePath is empty!";
        return false;
    }

    const std::string path = pathOfFile(filePath);
    if (!path.empty() && create) {
        createPath(path);
    }

    std::ios::openmode openMode = std::ios::in;
    if (create) {
        openMode |= std::ios::out;
    }

    std::ifstream ifs(filePath, openMode);
    if (!ifs) {
        std::cout << "File \"" << filePath << "\" open failure!";
        return false;
    }

    try {
        ifs >> rootJson;
    } catch (Json::RuntimeError msg) {
        printf("%s\n", msg.what());
        return false;
    }

    ifs.close();

    return true;
}

Json::Value JJson::value(const Json::Value &rootJson, const std::string &domain,
                         bool create)
{
    if (rootJson.isNull()) {
        return Json::Value();
    }

    Json::Value json = rootJson;
    std::list<std::string> sections;
    Icd::splitString(domain, "/", sections);
    if (sections.empty()) {
        return Json::Value();
    }

    for (std::list<std::string>::const_iterator citer = sections.cbegin();
         citer != sections.cend(); ++citer) {
        const std::string key = *citer;
        if (key.empty()) {
            continue;
        }
        if (json.isMember(key)) {
            json = json[key];
        } else if (create) {
            json[key] = Json::Value();
            json = json[key];
        } else {
            json = Json::Value();
            break;
        }
    }

    return json;
}

Json::Value JJson::value(const std::string &filePath, const std::string &domain,
                         bool create)
{
    Json::Value rootJson;
    if (!parse(filePath, rootJson, create)) {
        return Json::Value();
    }

    return value(rootJson, domain, create);
}

bool JJson::setValue(const std::string &filePath, const std::string &domain,
                     const Json::Value &value, bool create, bool fast)
{
    Json::Value rootJson;
    if (domain.empty()) {
        rootJson = value;
    } else {
        if (!parse(filePath, rootJson, create)) {
            return false;
        }

        if (!setValue(rootJson, domain, value)) {
            return false;
        }
    }

    if (!save(filePath, rootJson, fast)) {
        return false;
    }

    return true;
}

bool JJson::merge(const std::string &filePath, const std::string &domain,
                  const Json::Value &value, bool create, bool fast)
{
    Json::Value rootJson;
    if (!parse(filePath, rootJson, create)) {
        return false;
    }

    if (!merge(rootJson, domain, value)) {
        return false;
    }

    if (!save(filePath, rootJson, fast)) {
        return false;
    }

    return true;
}

bool JJson::merge(const Json::Value &source, Json::Value &target)
{
    if (source.isNull()) {
        return true;
    }

    for (Json::Value::const_iterator citer = source.begin();
         citer != source.end(); ++citer) {
        const std::string sourceKey = citer.key().asString();
        const Json::Value sourceValue = *citer;
        if (!target.isMember(sourceKey)) {
            target[sourceKey] = sourceValue;    // replace
            continue;
        }
        Json::Value subTarget = target[sourceKey];
        if (subTarget.type() != sourceValue.type()) {
            target[sourceKey] = sourceValue;    // replace
            continue;
        }
        switch (sourceValue.type()) {
        case Json::arrayValue:
        {
            target[sourceKey] = sourceValue;    // replace
            break;
        }
        case Json::objectValue:
        {
            if (!merge(sourceValue, subTarget)) {
                return false;
            }
            target[sourceKey] = subTarget;      // replace
            break;
        }
        default:
            target[sourceKey] = sourceValue;    // replace
            break;
        }
    }

    return true;
}

bool JJson::save(const std::string &filePath, const Json::Value &json, bool fast)
{
    if (json.isNull()) {
        return false;
    }

    std::string rootJson;
    if (fast) {
        rootJson = Json::FastWriter().write(json);
    } else {
        rootJson = Json::StyledWriter().write(json);
    }

    std::ofstream ofs;
    ofs.open(filePath);
    if (!ofs.is_open()) {
        return false;
    }

    try {
        ofs << rootJson;
    } catch (Json::RuntimeError msg) {
        printf("%s\n", msg.what());
        return false;
    }

    ofs.close();

    return true;
}

bool JJson::setValue(Json::Value &parentJson, const std::string &domain,
                     const Json::Value &value)
{
    if (domain.empty()) {
        return false;
    }

    std::size_t index = domain.find_first_of('/');
    if (index == std::string::npos) {
        parentJson[domain] = value;
        return true;
    }

    const std::string first = trimString(domain.substr(0, index));
    if (first.empty()) {
        parentJson[domain] = value;
        return true;
    }

    const std::string next = trimString(domain.substr(index + 1));
    if (next.empty()) {
        parentJson[first] = value;
        return true;
    }

    Json::Value object = parentJson[first];

    if (!setValue(object, next, value)) {
        return false;
    }

    parentJson[first] = object;

    return true;
}

bool JJson::merge(Json::Value &parentJson, const std::string &domain,
                  const Json::Value &value)
{
    if (domain.empty()) {
        return merge(value, parentJson);
    }

    std::size_t index = domain. find_first_of('/');
    if (index == std::string::npos) {
        switch (value.type()) {
        case Json::objectValue:
            return merge(value, parentJson);
        default:
            parentJson[domain] = value;
            return true;
        }
    }

    const std::string first = domain.substr(0, index);
    if (first.empty()) {
        return merge(value, parentJson);
    }

    Json::Value target = parentJson[first];

    const std::string next = trimString(domain.substr(index + 1));
    if (next.empty()) {
        switch (value.type()) {
        case Json::arrayValue:
        {
            parentJson[first] = value;  // replace
            break;
        }
        case Json::objectValue:
        {
            if (target.type() != Json::objectValue) {
                parentJson[first] = value;  // replace
                break;
            }
            if (!merge(value, target)) {
                return false;
            }
            parentJson[first] = target;
            break;
        }
        default:
            parentJson[first] = value;  // replace
            break;
        }
        return true;
    }

    if (!target.isObject()) {
        target = Json::Value(Json::objectValue);
    }

    if (!merge(target, next, value)) {
        return false;
    }

    parentJson[first] = target;

    return true;
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

void splitString(const std::string &str, const std::string &delim,
                 std::list<std::string> &ret)
{
    ret.clear();
    std::size_t last = 0;
    std::size_t index = str.find_first_of(delim, last);
    while (index != std::string::npos) {
        ret.push_back(str.substr(last, index - last));
        last = index + 1;
        index = str.find_first_of(delim, last);
    }

    if (index - last > 0) {
        ret.push_back(str.substr(last, index - last));
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
