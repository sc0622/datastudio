#include "precomp.h"
#include "comresource.h"

//
J_EXTERN_C J_ATTR_EXPORT void *CreateComponent(void* attemper)
{
    return static_cast<IJComponent *>
            (new ComResource(reinterpret_cast<IJAttempter *>(attemper)));
}

ComResource::ComResource(IJAttempter *attemper)
    : d_attempter(attemper)
{

}

ComResource::~ComResource()
{
}

bool ComResource::loadInterface()
{
    bool result = true;

    //
    Q_INIT_RESOURCE(com_resource);

    return result;
}

bool ComResource::updateInterface()
{
    bool result = true;

    //

    return result;
}

void ComResource::releaseInterface()
{
    //
    Q_CLEANUP_RESOURCE(com_resource);
}

void* ComResource::queryInterface(const std::string &iid, unsigned int ver)
{
    Q_UNUSED(iid);
    Q_UNUSED(ver);

    return 0;
}

std::string ComResource::componentName() const
{
    return QStringLiteral("com_resource").toStdString();
}

std::string ComResource::componentDesc() const
{
    return QStringLiteral("全局资源组件").toStdString();
}
