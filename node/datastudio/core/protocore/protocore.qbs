import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment

DynamicLibrary {

    type: base.concat([ 'node.out' ])
    targetName: 'protocore'
    readonly property path nodePath: FileInfo.fromNativeSeparators(Environment.getEnv('NODE_PATH'))

    Depends { name: 'cpp' }

    Group {
        name: 'Headers'
        files: [ '**/*.h' ]
        excludeFiles: [ '**/node_modules/**/*' ]
    }

    Group {
        name: 'Sources'
        files: [ '**/*.cpp' ]
        excludeFiles: [ '**/node_modules/**/*' ]
    }

    cpp.includePaths: [ nodePath + '/include' ]
    cpp.libraryPaths: [ nodePath + '/lib' ]
    cpp.dynamicLibraries: [ nodePath + '/lib/node' ]
    cpp.defines: [ 'BUILDING_NODE_EXTENSION' ]

    Rule {
        inputs: [ 'dynamiclibrary' ]
        Artifact {
            fileTags: [ 'node.out' ]
            filePath: FileInfo.joinPaths(product.qbs.installRoot,
                                         input.completeBaseName + '.node')
        }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.description = 'coping ' + input.fileName + '...';
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [ cmd ];
        }
    }
}
