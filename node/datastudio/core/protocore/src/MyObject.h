#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <node/node.h>
#include <node/node_object_wrap.h>

namespace protocore {

class MyObject : public node::ObjectWrap
{
public:
    static void Init(v8::Local<v8::Object> exports);

private:
    explicit MyObject(double value = 0);
    ~MyObject();

    static void New(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void PlusOne(const v8::FunctionCallbackInfo<v8::Value> &args);
    static v8::Persistent<v8::Function> constructor;
    double value_;
};

}

#endif
