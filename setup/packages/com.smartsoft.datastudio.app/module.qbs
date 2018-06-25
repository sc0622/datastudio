import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment
import qbs.TextFile

PackageApp {
    name: 'com.smartsoft.datastudio.app'
    type: base.concat([ 'data.tools.exe.out' ])

    // config
    Group {
        name: 'data-config'
        prefix: FileInfo.joinPaths(project.sourceDirectory, 'config') + '/'
        files: [ '**/*.*' ]
        excludeFiles: [ '**/*.pr[io]', '**/*.qbs', "**/~*" ]
        qbs.install: true
        qbs.installPrefix: dataInstallPrefix
        qbs.installDir: 'config'
        qbs.installSourceBase: prefix
    }

    // app
    Group {
        name: 'data-app'
        prefix: FileInfo.joinPaths(projectDir, 'bin') + '/'
        files: [ project.projectName + project.variantSuffix + '.exe' ]
        qbs.install: true
        qbs.installPrefix: dataInstallPrefix
        qbs.installDir: 'bin'
    }

    // tools
    Group {
        id: dataTools
        name: 'data-tools'
        prefix: project.sourceDirectory + '/tools/'
        files: [ 'WinSnap/*', 'DataAnalyse.exe.in' ]
        fileTags: [ 'data.tools.exe.in' ]
    }

    Rule {
        inputs: [ 'data.tools.exe.in' ]
        Artifact {
            fileTags: [ 'data.tools.exe.out' ]
            filePath: {
                var filePath = input.filePath;
                if (filePath.endsWith('.exe.in')) {
                    filePath = FileInfo.path(filePath) + '/' + input.completeBaseName;
                }
                var filePath = FileInfo.joinPaths(product.dataTargetPath,
                                                  FileInfo.relativePath(project.sourceDirectory, filePath));
                return filePath;
            }
        }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.description = 'coping file ' + output.filePath;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [ cmd ];
        }
    }
}
