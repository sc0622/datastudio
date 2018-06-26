#include "json_tool.h"
#if defined(QT_CORE_LIB) && defined(USE_QFILE)
#include <QFile>
#include <QString>
#endif

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

namespace Json {

int createPath(const std::string &path)
{
    std::string::size_type pathLength = path.length();
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

std::string pathOfFile(const std::string &filePath)
{
    std::string path = filePath;
    std::string::size_type index = path.find_last_of('/');
    if (index == std::string::npos) {
        index = path.find_last_of('\\');
    }

    if (index == std::string::npos) {
        path = std::string();
    } else {
        path = path.substr(0, index + 1);
    }

    return path;
}

bool resolve(const char *filePath, Value &root)
{
    return resolve(std::string(filePath), root);
}

bool resolve(const std::string &filePath, Value &root)
{
    if (filePath.empty()) {
        printf("filePath is empty!");
        return false;
    }

    const std::string path = pathOfFile(filePath);
    if (!path.empty()) {
        //return false;
    }
#if defined(QT_CORE_LIB) && defined(USE_QFILE)
    QFile file(QString::fromLocal8Bit(filePath.c_str()));
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    try {
        root.clear();
        Json::Reader reader;
        if (!reader.parse(file.readAll().constData(), root, true)) {
            return false;
        }
    } catch (RuntimeError msg) {
        printf("Parse json file \"%s\":\n%s\n", filePath.c_str(), msg.what());
        return false;
    }
#else
    std::ifstream ifs(filePath);
    if (!ifs) {
        printf("File \"%s\" open failure!\n", filePath.c_str());
        return false;
    }

    try {
        root.clear();
        Json::Reader reader;
        if (!reader.parse(ifs, root, true)) {
            return false;
        }
    } catch (RuntimeError msg) {
        printf("Parse json file \"%s\":\n%s\n", filePath.c_str(), msg.what());
        return false;
    }

    ifs.close();
#endif

    return true;
}

Value resolve(const char *filePath, const std::string &path)
{
    return resolve(std::string(filePath), path);
}

Value resolve(const std::string &filePath, const std::string &path)
{
    Value root;
    if (!resolve(filePath, path, root)) {
        return Value::nullSingleton();
    }

    return root;
}

bool resolve(const char *filePath, const std::string &path, Value &value)
{
    return resolve(std::string(filePath), path, value);
}

bool resolve(const std::string &filePath, const std::string &path, Value &value)
{
    Value root;
    if (!resolve(filePath, root)) {
        return false;
    }

    try {
        Value _value = Path(path).resolve(root);
        value = _value;
    } catch (RuntimeError msg) {
        printf("Parse json file \"%s\":\n%s\n", filePath.c_str(), msg.what());
        return false;
    }

    return true;
}

Value resolve(const Value &root, const std::string &path)
{
    try {
        return Path(path).resolve(root);
    } catch (RuntimeError msg) {
        printf("Parse json value with path \"%s\":\n%s\n", path.c_str(), msg.what());
        return Value::null;
    }
}

bool parse(const std::string &document, Value &root)
{
    return Reader().parse(document, root);
}

Value parse(const std::string &document, const std::string &path)
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

bool parse(const std::string &content, const std::string &path, Value &value)
{
    Value root;
    if (!parse(content, root)) {
        return false;
    }

    try {
        value = Path(path).resolve(root);
    } catch (RuntimeError msg) {
        printf("Parse json :\n%s\n", msg.what());
        return false;
    }

    return true;
}

bool make(const char *filePath, bool fast)
{
    return make(std::string(filePath), fast);
}

bool make(const std::string &filePath, bool fast)
{
    Value root(objectValue);
    return Json::make(filePath, root, true, fast);
}

bool make(const char *filePath, const Value &root, bool create, bool fast)
{
    return make(std::string(filePath), root, create, fast);
}

bool make(const std::string &filePath, const Value &root, bool create, bool fast)
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

bool make(const char *filePath, const std::string &path,
          const Value &value, bool create, bool fast)
{
    return make(std::string(filePath), path, value, create, fast);
}

bool make(const std::string &filePath, const std::string &path,
          const Value &value, bool create, bool fast)
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

bool makeArray(const char *filePath, const std::string &path,
               const Value &value, bool unique, bool create, bool fast)
{
    return makeArray(std::string(filePath), path, value, unique, create, fast);
}

bool makeArray(const std::string &filePath, const std::string &path,
               const Value &value, bool unique, bool create, bool fast)
{
    Value root;
    if (!resolve(filePath, root) && !create) {
        return false;
    }

    try {
        Value &_value = Path(path).make(root);
        if (unique) {
            if (_value.isArray()) {
                ArrayIndex i;
                for (i = 0; i < _value.size(); ++i) {
                    if (_value[i] == value) {
                        break;
                    }
                }
                if (i == _value.size()) {
                    _value.append(value);
                }
            } else {
                Json::Value v;
                v.append(value);
                _value = v;
            }
        } else {
            _value = value;
        }
    } catch (RuntimeError msg) {
        printf("Save json file \"%s\":\n%s\n", filePath.c_str(), msg.what());
        return false;
    }

    return make(filePath, root, create, fast);
}

bool replaceItem(const char *filePath, const std::string &path,
                 const Value &oldValue, const Value &newValue, bool create,
                 bool fast)
{
    return replaceItem(std::string(filePath), path, oldValue, newValue, create, fast);
}

bool replaceItem(const std::string &filePath, const std::string &path,
                 const Value &oldValue, const Value &newValue, bool create,
                 bool fast)
{
    Value root;
    if (!resolve(filePath, root) && !create) {
        return false;
    }

    try {
        Value &_value = Path(path).make(root);
        if (_value.isArray()) {
            for (ArrayIndex i = 0; i < _value.size(); ++i) {
                if (_value[i] == oldValue) {
                    if (newValue.isNull()) {
                        Value removed;
                        _value.removeIndex(i, &removed);
                    } else {
                        _value[i] = newValue;
                    }
                    break;
                }
            }
        } else if (_value.isObject() && newValue.isObject()) {
            Value::Members members = newValue.getMemberNames();
            if (members.size() != 1) {
                return false;
            }
            // oldValue - key; newValue - (key, value)
            for (Json::Value::iterator iter = _value.begin();
                 iter != _value.end(); ++iter) {
                const std::string key = oldValue.asString();
                if (iter.name() == key) {
                    _value.removeMember(key);
                    _value[members.at(0)] = newValue[members.at(0)];
                    break;
                }
            }
        } else if (!newValue.isNull()) {
            Value array;
            array.append(newValue);
            _value = array;
        }
    } catch (RuntimeError msg) {
        printf("Save json file \"%s\":\n%s\n", filePath.c_str(), msg.what());
        return false;
    }

    return make(filePath, root, create, fast);
}

bool removeItem(const char *filePath, const std::string &path,
                const std::string &key, const Json::Value &value,
                bool create, bool fast)
{
    return removeItem(std::string(filePath), path, key, value, create, fast);
}

bool removeItem(const std::string &filePath, const std::string &path,
                const std::string &key, const Json::Value &value,
                bool create, bool fast)
{
    Value root;
    if (!resolve(filePath, root) && !create) {
        return false;
    }

    try {
        Value &_value = Path(path).make(root);
        if (_value.isArray()) {
            for (ArrayIndex i = 0; i < _value.size(); ++i) {
                Value &item = _value[i];
                if (item.isMember(key) && item[key] == value) {
                    Value removed;
                    _value.removeIndex(i, &removed);
                    break;
                }
            }
        } else if (_value.isObject()) {
            for (Json::Value::iterator iter = _value.begin();
                 iter != _value.end(); ++iter) {
                if (iter.name() == key) {
                    if (!value.isNull() && *iter != value) {
                        continue;
                    }
                    _value.removeMember(key);
                    break;
                }
            }
        }
    } catch (RuntimeError msg) {
        printf("Save json file \"%s\":\n%s\n", filePath.c_str(), msg.what());
        return false;
    }

    return make(filePath, root, create, fast);
}

bool make(Json::Value &root, const std::string &path, const Json::Value &value)
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

bool merge(const char *filePath, const std::string &path,
           const Json::Value &value, bool create, bool fast)
{
    return merge(std::string(filePath), path, value, create, fast);
}

bool merge(const std::string &filePath, const std::string &path,
           const Json::Value &value, bool create, bool fast)
{
    Value root;
    if (!resolve(filePath, root) && !create) {
        return false;
    }

    try {
        Value &_value = Path(path).make(root);
        if (!_value.isObject()) {
            return false;
        }
        for (Json::ValueConstIterator citer = value.begin(); citer != value.end(); ++citer) {
            const std::string key = citer.key().asString();
            _value[key] = value[key];
        }
    } catch (RuntimeError msg) {
        printf("Save json file \"%s\":\n%s\n", filePath.c_str(), msg.what());
        return false;
    }

    return make(filePath, root, create, fast);
}

bool merge(const Value &source, Value &target)
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

}
