'use strict';

var protocore

if (process.env.DEBUG) {
    protocore  = require('node-loader!../../../lib/debug/protocore.node')
} else {
    protocore  = require('node-loader!../../../lib/release/protocore.node')
} 

module.exports = {
    'jcore': protocore
}
