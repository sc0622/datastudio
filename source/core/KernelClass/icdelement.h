#ifndef ICDELEMENT_H
#define ICDELEMENT_H
#include "globalstruct.h"

#ifdef KERNELCLASS_LIB
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#   ifdef KERNELCLASS_BUILD
#       define KERNELCLASS_EXPORT __declspec(dllexport)
#   else
#       define KERNELCLASS_EXPORT __declspec(dllimport)
#       if defined(DEBUG) || defined(_DEBUG)
#           pragma comment(lib, "KernelClassd.lib")
#       else
#           pragma comment(lib, "KernelClass.lib")
#       endif
#   endif // !KERNELCLASS_BUILD
#endif // _MSC_VER || ...
#endif // KERNELCLASS_LIB

#ifndef KERNELCLASS_EXPORT
#define KERNELCLASS_EXPORT
#endif

#ifndef SMT_CONVERT
#define SMT_CONVERT(type, data) (std::dynamic_pointer_cast<type>(data))
#endif // !SMT_CONVERT

class ICDElementPrivate;
/**
 * @brief 公共基类
 */
class KERNELCLASS_EXPORT ICDElement
{
public:
    typedef std::shared_ptr<ICDElement> smtElement;
    typedef std::map<int, ICDElement::smtElement> elementMap;
    typedef std::vector<ICDElement::smtElement> elementVector;

public:
    /**
     * @brief 构造函数
     * @param [in] type : 数据类型
     */
    explicit ICDElement(GlobalDefine::NodeType type = GlobalDefine::ntUnknown);
    virtual ~ICDElement();

    // 获取数据类型
    GlobalDefine::NodeType objectType() const;

    // 设置/获取ID
    virtual void setID(std::string id);
    std::string id() const;

    // 设置/获取名称
    virtual void setName(const std::string& name);
    std::string name() const;

    // 设置/获取标识
    virtual void setSign(const std::string& sign);
    std::string sign() const;

    // 设置/获取描述
    virtual void setDescribe(const std::string& describe);
    std::string describe() const;
    /**
     * @brief 克隆（深拷贝）
     * @return 实例
     */
    ICDElement::smtElement clone() const;
    ICDElement(const ICDElement &rhs);
    ICDElement &operator = (const ICDElement &rhs);

private:
    ICDElementPrivate    *d;
};

#endif // ICDELEMENT_H
