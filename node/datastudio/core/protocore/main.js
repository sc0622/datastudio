// main.js
//const protocore = require('./build/Debug/protocore')
const protocore = require('../../../lib/debug/protocore')

const obj = new protocore.Object('helloworld', 2)

obj.setDomain("what");

console.log(obj.domain());

