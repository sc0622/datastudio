
var File = require('qbs.File');
var FileInfo = require('qbs.FileInfo');
var Environment = require('qbs.Environment');
var Utilities = require('qbs.Utilities');

function isNewQbs(qbs) {
    return Utilities.versionCompare(qbs.version, "1.10.0") >= 0;
}

function hasDefinesBase(qbs) {
    return isNewQbs(qbs);
}

function jframeDir(project) {
    return projectDir(project, 'jframework', 'JFRAME_DIR');
}

function datastudioDir(project) {
    var path = projectDir(project, 'datastudio', 'DATASTUDIO_DIR');
    if (!FileInfo.cleanPath) {
        path = FileInfo.joinPaths(project.sourceDirectory,
                                  FileInfo.relativePath(project.sourceDirectory, path + '/'));
        if (path.endsWith('/.')) {
            path = path.substring(0, path.length - 2);
        }
    }
    return path;
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
                    if (!File.exists(qbsFilePath(path, projectName))) {
                        path = FileInfo.joinPaths(project.sourceDirectory);
                    }
                }
            }
        }
    }
    if (FileInfo.cleanPath) {
        return FileInfo.cleanPath(path);
    } else {
        return path;
    }
}

function qbsFilePath(path, projectName) {
    return path + '/' + projectName + '.qbs';
}

function ifwDir() {
    return FileInfo.fromWindowsSeparators(Environment.getEnv('QTIFW_DIR'))
}

function jframe3rdpartDir(project) {
    return FileInfo.joinPaths(jframeDir(project), 'src', '3rdpart');
}

function datastudioCoreDir(project) {
    return FileInfo.joinPaths(datastudioDir(project), 'src', 'core');
}

function jframe3rdpartVisible(project) {
    return (project.buildQuote && File.exists(jframe3rdpartDir(project) + '/3rdpart.qbs'));
}

function datastudioCoreVisible(project) {
    return (project.buildQuote && File.exists(datastudioCoreDir(project) + '/core.qbs'));
}

function jframe3rdpartPrefix(project) {
    return jframe3rdpartVisible(project) ? '' : '3rdpart';
}

function datastudioCorePrefix(project) {
    return datastudioCoreVisible(project) ? '' : 'core';
}

function dylibPrefix(qbs) {
    var targetOS = qbs.targetOS
    if (targetOS.contains('windows')) {
        return ''
    } else if (targetOS.contains('darwin')) {
        return 'lib'
    } else if (targetOS.contains('linux')) {
        return 'lib'
    } else {
        return ''
    }
}

function dylibExtension(qbs) {
    var targetOS = qbs.targetOS
    if (targetOS.contains('windows')) {
        return '.dll'
    } else if (targetOS.contains('darwin')) {
        return '.dylib'
    } else if (targetOS.contains('linux')) {
        return '.so'
    } else {
        return ''
    }
}

function dylibExtensionFuzzy(qbs) {
    var targetOS = qbs.targetOS
    if (targetOS.contains('windows')) {
        return '.dll'
    } else if (targetOS.contains('darwin')) {
        return '.*dylib'
    } else if (targetOS.contains('linux')) {
        return '.*so'
    } else {
        return ''
    }
}

function dylibSuffix(qbs) {
    var suffix = dylibExtensionFuzzy(qbs);
    if (qbs.buildVariant === 'debug') {
        suffix = 'd' + suffix;
    }
    return suffix;
}

function incDylibFuzzy(qbs, recursive, name) {
    var suffix = dylibExtensionFuzzy(qbs);
    if (qbs.buildVariant === 'debug') {
        suffix = '*d' + suffix;
    } else {
        suffix = '*[^d]' + suffix;
    }
    if (name) {
        suffix = name + suffix;
    }
    if (recursive) {
        return '**/' + suffix;
    } else {
        return suffix;
    }
}

function excDylibFuzzy(qbs, recursive, name) {
    var suffix = dylibExtensionFuzzy(qbs);
    if (qbs.buildVariant === 'debug') {
        suffix = '*[^d]' + suffix;
    } else {
        suffix = '*d' + suffix;
    }
    if (name) {
        suffix = name + suffix;
    }
    if (recursive) {
        return '**/' + suffix;
    } else {
        return suffix;
    }
}

function frameworkSuffix(qbs) {
    return '.framework';
}
