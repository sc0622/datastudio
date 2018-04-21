#include "precomp.h"
#include "root_wrap.h"
#include "icdcore/icd_root.h"
#include "icdcore/icd_vehicle.h"
#include "object_wrap.h"
#include "vehicle_wrap.h"

PROTOCORE_BEGIN

NODE_IMPL2(Root)

RootWrap::RootWrap(Icd::Root *data)
    : d_(data)
{
    //test
    for (int i = 0; i < 3; ++i) {
        std::ostringstream oss;
        oss << "item " << i;
        Icd::VehiclePtr vehicle = Icd::VehiclePtr(new Icd::Vehicle(oss.str(), d_));
        d_->appendVehicle(vehicle);
    }
}

void RootWrap::AllVehicle(node::CArgsRef args)
{
    NODE_D(Root, args);
    const Icd::VehiclePtrArray &vehicles = d->allVehicle();
    const int vehicleCount = vehicles.size();
    node::Array array = v8::Array::New(isolate, vehicleCount);
    for (int i = 0; i < vehicleCount; ++i) {
        //VehicleWrap *newObject = new VehicleWrap(vehicles[i].get());
        //v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), newObject);
        array->Set(i, VehicleWrap::New(isolate, vehicles[i].get()));
    }
}

void RootWrap::New(const node::Args &args)
{
    if (args.IsConstructCall()) {
        if (args[0]->IsObject()) {
            RootWrap *object = Unwrap<RootWrap>(args[0]->ToObject());
            Icd::Root *data = object->wrapped();
            if (data) {
                RootWrap *newObject = new RootWrap(data);
                newObject->Wrap(args.This());
                args.GetReturnValue().Set(args.This());
                return;
            }
        } else {
            //TEST
            Icd::Root *data = new Icd::Root();
            RootWrap *newObject = new RootWrap(data);
            newObject->Wrap(args.This());
            args.GetReturnValue().Set(args.This());
        }
    } else {
        v8::Local<v8::Value> argv[1] = { args[0] };
        NODE_NEW_INST(argv);
    }
}

NODE_METHOD_MAP_BEGIN(RootWrap, Root)
NODE_METHOD_INHERIT(protocore::ObjectWrap)
NODE_METHOD_MAP(vehicles, AllVehicle)
NODE_METHOD_MAP_END()

PROTOCORE_END
