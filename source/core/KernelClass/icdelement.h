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
 * @brief ��������
 */
class KERNELCLASS_EXPORT ICDElement
{
public:
    typedef std::shared_ptr<ICDElement> smtElement;
    typedef std::map<int, ICDElement::smtElement> elementMap;
    typedef std::vector<ICDElement::smtElement> elementVector;

public:
    /**
     * @brief ���캯��
     * @param [in] type : ��������
     */
    explicit ICDElement(GlobalDefine::NodeType type = GlobalDefine::ntUnknown);
    virtual ~ICDElement();

    // ��ȡ��������
    GlobalDefine::NodeType objectType() const;

    // ����/��ȡID
    virtual void setID(std::string id);
    std::string id() const;

    // ����/��ȡ����
    virtual void setName(const std::string& name);
    std::string name() const;

    // ����/��ȡ��ʶ
    virtual void setSign(const std::string& sign);
    std::string sign() const;

    // ����/��ȡ����
    virtual void setDescribe(const std::string& describe);
    std::string describe() const;
    /**
     * @brief ��¡�������
     * @return ʵ��
     */
    ICDElement::smtElement clone() const;
    ICDElement(const ICDElement &rhs);
    ICDElement &operator = (const ICDElement &rhs);

private:
    ICDElementPrivate    *d;
};

#endif // ICDELEMENT_H
