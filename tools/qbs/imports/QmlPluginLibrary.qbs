import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment
import qbs.Process
import qbs.Utilities

PluginLibrary {

    property pathList dynamicLibraryPaths: [
        project.sourceDirectory + '/lib/3rdpart',
        project.sourceDirectory + '/lib/core'
    ]

    Depends { name: 'Qt.qml' }

    Group {
        name: 'qmlfiles'
        prefix: product.sourceDirectory + '/'
        files: [ 'qmldir' ]
        fileTags: [ 'qmldir.in' ]
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: product.installDir
    }

    type: base.concat([ 'qmldir.out', 'qmltype.out' ])
    cpp.defines: base.concat([ 'QML_IMPORTS_PATH="' + Qt.qml.qmlPath + '"'])

    Rule {
        inputs: [ 'qmldir.in' ]
        Artifact {
            filePath: FileInfo.joinPaths(product.destinationDirectory, input.fileName)
            fileTags: [ 'qmldir.out' ]
        }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.description = 'coping ' + input.fileName;
            cmd.sourceCode = function(){ File.copy(input.filePath, output.filePath); }
            return [cmd];
        }
    }

    Rule {
        multiplex: true
        inputs: [ 'dynamiclibrary', 'qmldir.in' ]
        Artifact { fileTags: [ 'qmltype.out' ] }
        prepare: {
            var envs = ['PATH='
                        + product.Qt.core.binPath
                        + product.qbs.pathListSeparator + product.Qt.core.libPath
                        + product.qbs.pathListSeparator + product.dynamicLibraryPaths.join(product.qbs.pathListSeparator)
                        + product.qbs.pathListSeparator + Environment.getEnv('PATH')];
            // generate
            var cmd = new Command(FileInfo.joinPaths(product.Qt.core.binPath, 'qmlplugindump'),
                                  ['-defaultplatform', '-nonrelocatable', product.uri, product.pluginVersion,
                                   product.destinationRoot, '-output', FileInfo.joinPaths(
                                       product.destinationDirectory, product.name + '.qmltypes')]);
            cmd.description = 'generating qmltypes...';
            cmd.workingDirectory = product.Qt.core.binPath;
            cmd.environment = envs;
            return [cmd];
        }
    }
}
