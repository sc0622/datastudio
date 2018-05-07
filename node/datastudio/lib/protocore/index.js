'use strict';

function isDev() {
  // return process.mainModule.filename.indexOf('app.asar') === -1;
  return (process.env.ELECTRON_ENV === 'dev');
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
