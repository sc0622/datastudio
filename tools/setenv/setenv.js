
var File = require('qbs.File');
var FileInfo = require('qbs.FileInfo');
var Environment = require('qbs.Environment');

function jframeDir(project) {
    var path = Environment.getEnv('JFRAME_DIR');
    if (!File.exists(path)) {
        path = FileInfo.joinPaths(project.sourceDirectory, '..', 'jframework');
        if (!File.exists(path)) {
            path = FileInfo.joinPaths(project.sourceDirectory, '..', '..', 'jframework');
        }
    }

    return path;
}
