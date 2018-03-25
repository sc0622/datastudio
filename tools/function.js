var File = require("qbs.File");

function dynamicSuffix(qbs) {
    if (qbs.buildVariant === 'debug') {
        return 'd';
    } else {
        return '';
    }
}
