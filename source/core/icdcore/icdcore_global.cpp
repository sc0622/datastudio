#include "precomp.h"
#include "icdcore_global.h"
#include "json/reader.h"
#include <sstream>
#include <fstream>

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

// class JJson

bool JJson::parse(const std::string &filePath, Json::Value &rootJson, bool create)
{
    rootJson.clear();

    if (filePath.empty()) {
        qWarning() << QStringLiteral("FilePath is empty!");
        return false;
    }

    const QString _filePath = QString::fromStdString(filePath);
    if (create) {
        QFileInfo fileInfo(_filePath);
        if (!fileInfo.absoluteDir().exists()) {
            fileInfo.absoluteDir().mkpath(fileInfo.absolutePath());
        }
    }

    std::ios::openmode openMode = std::ios::in;
    if (create) {
        openMode |= std::ios::out;
    } else {
        if (!QFile::exists(_filePath)) {
            qWarning() << QStringLiteral("File \"%1\" is not exists!").arg(_filePath);
            return false;
        }
    }

    std::ifstream ifs;
    ifs.open(filePath, openMode);
    if (!ifs.is_open()) {
        qWarning() << QStringLiteral("File \"%1\" open failure!").arg(_filePath);
        return false;
    }
#if 0
    Json::Reader reader;
    if (!reader.parse(ifs, rootJson, false)) {
        qWarning() << QStringLiteral("Parse file \"%1\" error: %2!")
                      .arg(_filePath)
                      .arg(QString::fromStdString(reader.getFormatedErrorMessages()));
        return false;
    }
#else
    ifs >> rootJson;
#endif


    ifs.close();

    return true;
}

Json::Value JJson::value(const Json::Value &rootJson, const std::string &domain, bool create)
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

    foreach (auto &section, sections) {
        const std::string key = section;
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

Json::Value JJson::value(const std::string &filePath, const std::string &domain, bool create)
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

    std::string json;
    if (fast) {
        json = Json::FastWriter().write(rootJson);
    } else {
        json = Json::StyledWriter().write(rootJson);
    }

    std::ofstream ofs;
    ofs.open(filePath);
    if (!ofs.is_open()) {
        return false;
    }
    ofs << json;
    ofs.close();

    return true;
}

bool JJson::merge(const Json::Value &source, Json::Value &target)
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

bool JJson::setValue(Json::Value &parentJson, const std::string &domain, const Json::Value &value)
{
    if (domain.empty()) {
        return true;
    }

    const QString _domain = QString::fromStdString(domain);

    const std::string first = _domain.section('/', 0, 0, QString::SectionSkipEmpty)
            .trimmed().toStdString();
    if (first.empty()) {
        return false;
    }

    Json::Value object = parentJson[first];
    const std::string next = _domain.section('/', 1, -1, QString::SectionSkipEmpty)
            .trimmed().toStdString();
    if (next.empty()) {
        parentJson[first] = value;
        return true;
    }

    if (!setValue(object, next, value)) {
        return false;
    }

    parentJson[first] = object;

    return true;
}

}
