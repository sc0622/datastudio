import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment
import 'setenv.js' as Func

Project {
    Product {
        name: 'sync'
        Depends { name: 'cpp' }

        readonly property stringList depends: [
            'jchart', 'jencrypt', 'jutraledit', 'jwt', 'log4cpp', 'nodeeditor', 'qwt', 'tinyxml'
        ]

        FileTagger {
            patterns: [ '*.h[h]*', '*.hpp' ]
            fileTags: [ 'header' ]
        }

        FileTagger {
            patterns: [ '*.dll', '*.lib' ]
            fileTags: [ 'library' ]
        }

        cpp.additionalProductTypes: [ 'header', 'library' ]

        // headers

        Group {
            id: header_3rdpart
            name: '3rdpart_header'
            prefix: Func.jframeDir(project) + '/'
            files: {
                var files = [];
                product.depends.forEach(function(item){
                    var path = FileInfo.joinPaths('include', '3rdpart', item);
                    files.push(path + '/**/*.h');
                    files.push(path + '/**/*.hh');
                    files.push(path + '/**/*.hpp');
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
            prefix: Func.jframeDir(project) + '/'
            files: {
                var files = [];
                product.depends.forEach(function(item){
                    var path = FileInfo.joinPaths('lib', '3rdpart', item);
                    files.push(path + '*.dll');
                });
                return files;
            }
            fileTags: [ name + '.in' ]
            qbs.install: true
            qbs.installPrefix: project.installPrefix
            qbs.installDir: 'bin'
        }

        Group {
            id: library_3rdpart
            name: '3rdpart_library'
            prefix: Func.jframeDir(project) + '/'
            files: {
                var files = [];
                product.depends.forEach(function(item){
                    var path = FileInfo.joinPaths('lib', '3rdpart', item);
                    files.push(path + '*.lib');
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
        name: 'install'
        Depends { name: 'cpp' }
        Depends { name: 'Qt.core' ; cpp.link: false }

        // 3rdpart - moxa

        Group {
            name: 'dynamic_3rdpart'
            prefix: FileInfo.joinPaths(project.sourceDirectory, 'lib', '3rdpart', 'moxa') + '/'
            files: [ 'pcomm.dll' ]
            qbs.install: true
            qbs.installPrefix: project.installPrefix
            qbs.installDir: 'bin'
        }

        // Qt - 3rdpart

        Group {
            name: 'dynamic_qt_3rdpart'
            prefix: Environment.getEnv('QTDIR') + '/bin/'
            files: {
                var files = [];
                [ 'lib*GL*', 'opengl*' ].forEach(function(item){
                    files.push(item + project.variantSuffix + '.dll');
                });
                files.push('d3dcompiler*.dll');
                return files;
            }
            qbs.install: true
            qbs.installPrefix: project.installPrefix
            qbs.installDir: 'bin'
        }

        // Qt - base

        Group {
            name: 'dynamic_qt_base'
            prefix: Environment.getEnv('QTDIR') + '/bin/'
            files: {
                var files = [];
                [ 'Concurrent', 'Core', 'Gui', 'Network', 'PrintSupport',
                  'SerialPort', 'Sql', 'Svg', 'Widgets', 'Xml']
                .forEach(function(item){
                    files.push('Qt' + Qt.core.versionMajor + item + project.variantSuffix + '.dll');
                });
                return files;
            }
            qbs.install: true
            qbs.installPrefix: project.installPrefix
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
            qbs.installPrefix: project.installPrefix
            qbs.installDir: 'bin'
            qbs.installSourceBase: prefix
        }

        // Qt - qml

        // Qt - others
    }
}
