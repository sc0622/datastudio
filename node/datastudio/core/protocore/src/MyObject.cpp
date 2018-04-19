#include "MyObject.h"
#include <iostream>

namespace protocore {

v8::Persistent<v8::Function> MyObject::constructor;

MyObject::MyObject(double value)
  : value_(value)
{
    std::cout << "helloworld";
}

MyObject::~MyObject()
{

}

void MyObject::Init(v8::Local<v8::Object> exports)
{
    v8::Isolate *isolate = exports->GetIsolate();

    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate, New);
    tpl->SetClassName(v8::String::NewFromUtf8(isolate, "MyObject"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    NODE_SET_PROTOTYPE_METHOD(tpl, "plusOne", PlusOne);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(v8::String::NewFromUtf8(isolate, "MyObject"), tpl->GetFunction());
}

void MyObject::New(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    v8::Isolate *isolate = args.GetIsolate();

    if (args.IsConstructCall()) {
        // Invoked as constructor: `new MyObject(...)`
        double value = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
        MyObject *obj = new MyObject(value);
        obj->Wrap(args.This());
        args.GetReturnValue().Set(args.This());
    } else {
        // Invoked as plain function `MyObject(...)`, trun into constructor call.
        const int argc = 1;
        v8::Local<v8::Value> argv[argc] = { args[0] };
        v8::Local<v8::Context> context = isolate->GetCurrentContext();
        v8::Local<v8::Function> cons = v8::Local<v8::Function>::New(isolate, constructor);
        v8::Local<v8::Object> result = cons->NewInstance(context, argc, argv).ToLocalChecked();
        args.GetReturnValue().Set(result);
    }
}

void MyObject::PlusOne(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    v8::Isolate *isolate = args.GetIsolate();

    MyObject *obj = ObjectWrap::Unwrap<MyObject>(args.Holder());
    obj->value_ += 1;

    args.GetReturnValue().Set(v8::Number::New(isolate, obj->value_));
}

}
