var proto = require('../datastudio/lib/protocore/protocore.node')

var object = new proto.Object();

object.objectType = 12;
object.id = 'helloworld';

console.log('objectType:', object.objectType);
console.log('id:', object.id);

var root = new proto.Root();
root.id = 'root';

console.log('root.id:', root.id);
console.log('root.objectType:', root.objectType);

for (var i in root.vehicles) {
    console.log(vehicles[i].id);
}

var vehicle = new proto.Vehicle();
vehicle.mark = "vehicle #1";
root.insertVehicle(2, vehicle);

for (var i in root.vehicles) {
    console.log(root.vehicles[i].mark);
}

console.log('query:', root.vehicleByMark("vehicle #1").mark);


