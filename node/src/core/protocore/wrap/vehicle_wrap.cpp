#include "precomp.h"
#include "vehicle_wrap.h"
#include "icdcore/icd_vehicle.h"
#include "object_wrap.h"

PROTOCORE_BEGIN

NODE_IMPL2(Vehicle)

VehicleWrap::VehicleWrap(Icd::Vehicle *data)
    : d_(data)
{

}

node::Object VehicleWrap::New(v8::Isolate* isolate, Icd::Vehicle *data)
{
    //isolate = v8::Isolate::GetCurrent();
    if (data) {
        v8::Local<v8::Context> context = isolate->GetCurrentContext();
        v8::Local<v8::Function> cons = v8::Local<v8::Function>::New(isolate, constructor);
        v8::Local<v8::Object> result = cons->NewInstance(context, 0, nullptr).ToLocalChecked();
        return result;
    } else {
        return node::Object();
    }
}

void VehicleWrap::New(const node::Args &args)
{
    if (args.IsConstructCall()) {
        if (args[0]->IsObject()) {
            VehicleWrap *object = Unwrap<VehicleWrap>(args[0]->ToObject());
            Icd::Vehicle *data = object->wrapped();
            if (data) {
                VehicleWrap *newObject = new VehicleWrap(data);
                newObject->Wrap(args.This());
                args.GetReturnValue().Set(args.This());
                return;
            }
        }
    } else {
        v8::Local<v8::Value> argv[1] = { args[0] };
        NODE_NEW_INST(argv);
    }
}

NODE_METHOD_MAP_BEGIN(VehicleWrap, Vehicle)
NODE_METHOD_INHERIT(protocore::ObjectWrap)
NODE_METHOD_MAP_END()

PROTOCORE_END
