
var File = require('qbs.File');
var FileInfo = require('qbs.FileInfo');
var Environment = require('qbs.Environment');

function jframeDir(project) {
    return projectDir(project, 'jframework', 'JFRAME_DIR');
}

function datastudioDir(project) {
    return projectDir(project, 'datastudio', 'DATASTUDIO_DIR');
}

function projectDir(project, projectName, envName) {
    var path = Environment.getEnv(envName);
    if (!File.exists(qbsFilePath(path, projectName))) {
        path = FileInfo.joinPaths(project.sourceDirectory, '..', '3rdpart', projectName);
        if (!File.exists(qbsFilePath(path, projectName))) {
            path = FileInfo.joinPaths(project.sourceDirectory, '..', '..', projectName, 'project');
            if (!File.exists(qbsFilePath(path, projectName))) {
                path = FileInfo.joinPaths(project.sourceDirectory, '..', '..', '..', projectName, 'project');
                if (!File.exists(qbsFilePath(path, projectName))) {
                    path = FileInfo.joinPaths(project.sourceDirectory, '..', projectName);
                }
            }
        }
    }

    return FileInfo.cleanPath(path);
}

function qbsFilePath(path, projectName) {
    return path + '/' + projectName + '.qbs';
}

function dylibSuffix(product) {
    if (product.qbs.buildVariant === 'debug') {
        return 'd.dll';
    } else {
        return '.dll';
    }
}

function incDylibSuffixFuzzy(product) {
    if (product.qbs.buildVariant === 'debug') {
        return '*d.dll';
    } else {
        return '*[^d].dll';
    }
}

function excDylibSuffixFuzzy(product) {
    if (product.qbs.buildVariant === 'debug') {
        return '*[^d].dll';
    } else {
        return '*d.dll';
    }
}
