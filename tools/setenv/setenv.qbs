import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment
import 'setenv.js' as Func

Project {

    Product {
        name: 'setenv-sync'
        type: base.concat([ 'header', 'library' ])
        Depends { name: 'cpp' }

        readonly property stringList depends: [
            'jchart', 'jencrypt', 'jutraledit', 'jwt', 'log4cpp', 'nodeeditor', 'qwt', 'tinyxml'
        ]

        // headers

        Group {
            id: header_3rdpart
            name: '3rdpart_header'
            prefix: Func.jframeDir(project) + '/include/3rdpart/'
            files: {
                var files = [];
                product.depends.forEach(function(item){
                    files.push(item + '/**/*.h');
                    files.push(item + '/**/*.hh');
                    files.push(item + '/**/*.hpp');
                });
                return files;
            }
            fileTags: [ name + '.in' ]
        }

        Rule {
            inputs: header_3rdpart.fileTags
            Artifact {
                filePath: {
                    var sourcePath = FileInfo.joinPaths(Func.jframeDir(project), 'include', '3rdpart');
                    return FileInfo.joinPaths(project.sourceDirectory, 'include', '3rdpart',
                                              FileInfo.relativePath(sourcePath, input.filePath));
                }
                fileTags: [ 'header' ]
            }
            prepare: {
                var cmd = new JavaScriptCommand();
                cmd.description = "coping " + input.fileName;
                //cmd.silent = true;
                cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
                return [cmd];
            }
        }

        // library

        Group {
            id: dynamic_3rdpart
            name: '3rdpart_dynamic'
            prefix: Func.jframeDir(project) + '/lib/3rdpart/'
            files: {
                var files = [];
                product.depends.forEach(function(item){
                    files.push(item + Func.dylibSuffix(product));
                });
                return files;
            }
            fileTags: [ name + '.in' ]
            qbs.install: true
            qbs.installPrefix: project.projectName
            qbs.installDir: 'bin'
        }

        Group {
            id: library_3rdpart
            name: '3rdpart_library'
            prefix: Func.jframeDir(project) + '/lib/3rdpart/'
            excludeFiles: [ Func.incDylibSuffixFuzzy(product) ]
            files: {
                var files = [];
                product.depends.forEach(function(item){
                    files.push(item + '*.lib');
                    files.push(item + '*.dll');
                });
                return files;
            }
            fileTags: [ name + '.in' ]
        }

        Rule {
            inputs: dynamic_3rdpart.fileTags.concat(library_3rdpart.fileTags)
            Artifact {
                filePath: {
                    var sourcePath = FileInfo.joinPaths(Func.jframeDir(project), 'lib', '3rdpart');
                    return FileInfo.joinPaths(project.sourceDirectory, 'lib', '3rdpart',
                                              FileInfo.relativePath(sourcePath, input.filePath));
                }
                fileTags: [ 'library' ]
            }
            prepare: {
                var cmd = new JavaScriptCommand();
                cmd.description = "coping " + input.fileName;
                //cmd.silent = true;
                cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
                return [cmd];
            }
        }
    }

    Product {
        name: 'setenv-install'

        Depends { name: 'cpp' }
        Depends { name: 'Qt.core' ; cpp.link: false }
        Depends { name: 'setenv-sync'; cpp.link: false }

        // 3rdpart - moxa

        Group {
            name: 'dynamic_3rdpart'
            prefix: FileInfo.joinPaths(project.sourceDirectory, 'lib', '3rdpart', 'moxa') + '/'
            files: [ 'pcomm.dll' ]
            qbs.install: true
            qbs.installPrefix: project.projectName
            qbs.installDir: 'bin'
        }

        // Qt - 3rdpart

        Group {
            name: 'dynamic_qt_3rdpart'
            prefix: Environment.getEnv('QTDIR') + '/bin/'
            files: {
                var files = [];
                [ 'lib*GL*' ].forEach(function(item){
                    files.push(item + project.variantSuffix + '.dll');
                });
                files.push('opengl*.dll');
                files.push('d3dcompiler*.dll');
                return files;
            }
            qbs.install: true
            qbs.installPrefix: project.projectName
            qbs.installDir: 'bin'
        }

        // Qt - base

        Group {
            name: 'dynamic_qt_base'
            prefix: Environment.getEnv('QTDIR') + '/bin/'
            files: {
                var files = [];
                [ 'Concurrent', 'Core', 'Gui', 'Network', 'PrintSupport', 'OpenGL',
                  'SerialPort', 'Sql', 'Svg', 'Widgets', 'Xml']
                .forEach(function(item){
                    files.push('Qt' + Qt.core.versionMajor + item + project.variantSuffix + '.dll');
                });
                return files;
            }
            qbs.install: true
            qbs.installPrefix: project.projectName
            qbs.installDir: 'bin'
        }

        // Qt - plugins

        Group {
            name: 'dynamic_qt_plugins'
            prefix: Environment.getEnv('QTDIR') + '/plugins/'
            files: {
                var files = [];
                [ 'bearer', 'iconengines', 'imageformats', 'platforms', 'translations', 'styles' ]
                .forEach(function(item){
                    files.push(item + '/*' + project.variantSuffix + '.dll');
                });
                return files;
            }
            qbs.install: true
            qbs.installPrefix: project.projectName
            qbs.installDir: 'bin'
            qbs.installSourceBase: prefix
        }

        // Qt - qml

        // Qt - others
    }
}
