import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment

CppDyLibrary {
    type: base.concat([ 'node.out' ])
    //readonly property path nodePath: FileInfo.fromNativeSeparators(Environment.getEnv('NODE_PATH'))
    readonly property path nodePath: FileInfo.joinPaths(project.sourceDirectory, 'node/lib/node/v8.11.1')
    property path destinationDir: destinationDirectory

    cpp.variantSuffix: ''
    cpp.includePaths: base.concat([ nodePath + '/include' ])
    cpp.libraryPaths: base.concat([ FileInfo.joinPaths(nodePath, 'lib', qbs.architecture) ])
    cpp.dynamicLibraries: base.concat([ FileInfo.joinPaths(nodePath, 'lib', qbs.architecture, 'node') ])
    cpp.defines: base.concat([ 'BUILDING_NODE_EXTENSION' ])
    cpp.cxxLanguageVersion: 'c++11'

    Rule {
        inputs: [ 'dynamiclibrary' ]
        Artifact {
            fileTags: [ 'node.out' ]
            filePath: FileInfo.joinPaths(product.destinationDir, input.completeBaseName + '.node')
        }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.description = 'generating ' + output.fileName;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [ cmd ];
        }
    }
}
