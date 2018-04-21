var proto = require('../lib/debug/protocore')

var r = new proto.Root();

var vehicles = r.vehicles();

console.log(vehicles.length);
