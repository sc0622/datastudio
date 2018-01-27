#ifndef COMRESOURCE_H
#define COMRESOURCE_H

#include "kernel/jframe_core.h"
#include "factory/jnotifier.h"

class ComResource
        : public IJComponent
{
public:
    explicit ComResource(IJAttempter* attemper);
    virtual ~ComResource();

    // IJUnknown interface
public:
    bool loadInterface();
    bool updateInterface();
    void releaseInterface();
    void *queryInterface(const std::string &iid, unsigned int ver);

    // IJComponent interface
public:
    std::string componentName() const;
    std::string componentDesc() const;

protected:

private:
    IJAttempter *d_attempter;
};

#endif // COMRESOURCE_H
