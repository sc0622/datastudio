var proto = require('../lib/debug/protocore')

//var object = new proto.Object();
/*
object.objectType = 12;
object.id = 'helloworld';

console.log('objectType:', object.objectType);
console.log('id:', object.id);

var root = new proto.Root();
root.id = 'root';

console.log('root.id:', root.id);
console.log('root.objectType:', root.objectType);

var vehicles = root.vehicles;

console.log('vehicles:', vehicles);

for (var i in vehicles) {
    console.log(vehicles[i].id);
}
*/

//console.log('object:', object);
//console.log('object.objectType:', object.objectType);
//object.id = 'hello';
//console.log('object.id:', object.id);

//

var root = new proto.Root();
console.log('root.mark:', root.mark);
var vehicles = root.vehicles;
console.log('----test method----start')
var vehicle0 = vehicles[0];
console.log(vehicle0.clearData);
//vehicle0.clearData();
console.log('----test method----end')

for (var i in vehicles) {
    console.log(vehicles[i].id);
}

//var object = new proto.Object();
//console.log(object.clearData);
//object.clearData();

var vehicle = new proto.Vehicle();
console.log(vehicle.clearData);
vehicle.clearData(null);
