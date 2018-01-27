#ifdef _MSC_VER
#include <Windows.h>
#include <string>
#elif defined(__unix__)
#include <sstream>
#endif

#include "core/jframe_facade.h"
#include <assert.h>

/**
 * @brief appDirPath : 获取 application 所在路径（不包含文件名称）
 * @return : application 所在路径（不包含文件名称）
 */
J_EXTERN_C J_ATTR_EXPORT const char* appDirPath()
{
    static std::string _path = "";
    if (_path.empty()) {
#ifdef __unix_
        std::stringstream ss;
        ss << "/proc/" << getpid() << "/exe";
#endif
        //
        char buffer[J_PATH_MAX + 2];
        unsigned int length = 0;
#ifdef _MSC_VER
        length = (unsigned int)GetModuleFileNameA(NULL, buffer, J_PATH_MAX + 1);
#elif defined(__unix__)
        length = (unsigned int)readlink(ss.str().c_str(), buffer, J_PATH_MAX);
#endif
        //
        if (length == 0) {
            // error
        } else if (length <= J_PATH_MAX) {
            buffer[J_PATH_MAX + 1] = '\0';
            _path = buffer;
        } else {
            // J_PATH_MAX sized buffer wasn't large enough to contain the full path, use heap
            char *bufferNew = 0;
            int i = 1;
            size_t size;
            do {
                ++i;
                size = J_PATH_MAX * i;
                bufferNew = reinterpret_cast<char *>(realloc(bufferNew, (size + 1) * sizeof(char)));
                if (bufferNew) {
#ifdef _MSC_VER
                    length = (unsigned int)GetModuleFileNameA(NULL, buffer, J_PATH_MAX + 1);
#elif defined(__unix__)
                    length = (unsigned int)readlink(ss.str().c_str(), buffer, J_PATH_MAX);
#endif
                }
            } while (bufferNew && length == size);

            if (bufferNew) {
                *(bufferNew + size) = '\0';
            }

            _path = bufferNew;
            free(bufferNew);
        }

        //
        if (!_path.empty()) {
            //
            int index = _path.find_last_of('/');
            if (index == -1) {
                index = _path.find_last_of('\\');
            }
            if (index != -1) {
                _path = _path.substr(0, index);
            }
            // replace '\\' with '/'
            std::string::iterator iter = _path.begin();
            for (; iter != _path.end(); ++iter) {
                if (*iter == '\\') {
                    *iter = '/';
                }
            }
        }
    }

    return _path.c_str();
}

/**
 * @brief libraryPrefix
 * @return
 */
const char* libraryPrefix()
{
    static std::string _v = "";
    if (_v.empty()) {
#ifdef _MSC_VER
        _v = "";
#elif defined(__unix__)
        _v = "lib";
#else
#pragma message("not supported!")
        _v = "";
#endif
    }

    return _v.c_str();
}

/**
 * @brief librarySuffix
 * @param debug
 * @return
 */
const char* librarySuffix(bool debug)
{
#ifdef _MSC_VER
    return debug ? "d.dll" : ".dll";
#elif defined(__unix__)
    (void)debug;
    return ".so";
#else
#pragma message("not supported!")
    (void)debug;
    return "";
#endif
}

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    // 加载库
    const std::string filePath = std::string(appDirPath()).append("/").append(libraryPrefix())
            .append("jframeworkdir").append(librarySuffix(false));

    // 获取 FrameFacadeInstace 导出接口
    typedef void* (J_ATTR_CDECL*FrameFacadeInstace)(int, char*);
    FrameFacadeInstace frameFacadeInstace = (FrameFacadeInstace)
            JLibrary::resolve(filePath, "FrameFacadeInstace");
    if (!frameFacadeInstace) {
        return -1;      // 获取导出接口失败
    }

    IJFrameFacade *frameFacade = dynamic_cast<IJFrameFacade *>
            ((IJUnknown *)(frameFacadeInstace(
                           #if defined(DEBUG) || defined(_DEBUG)
                               1
                           #else
                               0
                           #endif
                               , (char*)"editor"
                               )));
    if (!frameFacade) {
        return -1;      // 获取实例失败
    }

    // 加载框架
    if (!frameFacade->loadFrame(argc, argv, 0, "encrypt:enabled")) {
        frameFacade->exitFrame();
        return -1;      // 加载失败
    }

    // 显示框架主窗口
    frameFacade->showFrame(true, JFrame::WindowStateMaximized);

    //
    return frameFacade->runQApp(0);
}
