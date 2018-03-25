var qbs = require('qbs');

function dynamicLibName(module) {
    if (qbs.buildVaraint === 'debug') {
        return module + 'd';
    } else {
        return module;
    }
}
