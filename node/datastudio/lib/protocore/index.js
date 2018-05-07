'use strict';

function isDev() {
  // return process.mainModule.filename.indexOf('app.asar') === -1;
  return (process.env.NODE_ENV === 'development');
}

var protocore;

if (isDev()) {
  protocore = require('node-loader!./protocore.node'); 
} else {
  protocore = require('native-ext-loader!./protocore.node'); 
}

module.exports = {
  'protocore': protocore
}
