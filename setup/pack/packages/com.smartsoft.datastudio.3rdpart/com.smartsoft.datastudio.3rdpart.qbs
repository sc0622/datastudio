import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment

PackProduct {

    Depends { name: 'cpp' }
    Depends { name: 'Qt.core'; cpp.link: false }
    Depends { name: 'meta' }
    Depends { name: 'setenv-install'; cpp.link: false }

    // 3rdpart
    Group {
        name: 'data-3rdpart'
        prefix: FileInfo.joinPaths(projectDir, 'bin') + '/'
        //prefix: FileInfo.joinPaths(project.sourceDirectory, 'lib', '3rdpart') + '/'
        files: {
            var files = [];
            [ 'jchart', 'jencrypt', 'jutraledit', 'jwt', 'log4cpp',
              'nodeeditor', 'qwt', 'tinyxml' ].forEach(function(item){
                  files.push(item + project.variantSuffix + '.dll');
              });
            return files;
        }
        qbs.install: true
        qbs.installPrefix: installPrefix
        qbs.installDir: installDir
    }

    // 3rdpart - moxa
    Group {
        name: 'data-3rdpart-moxa'
        prefix: FileInfo.joinPaths(project.sourceDirectory, 'lib', '3rdpart', 'moxa') + '/'
        files: [ 'pcomm.dll' ]
        qbs.install: true
        qbs.installPrefix: installPrefix
        qbs.installDir: installDir
    }

    // Qt - 3rdpart
    Group {
        name: 'data-Qt-3rdpart'
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
        qbs.installPrefix: installPrefix
        qbs.installDir: installDir
    }
    // Qt - base

    Group {
        name: 'data-Qt-base'
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
        qbs.installPrefix: installPrefix
        qbs.installDir: installDir
    }

    // Qt - plugins

    Group {
        name: 'data-Qt-plugins'
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
        qbs.installPrefix: installPrefix
        qbs.installDir: installDir
        qbs.installSourceBase: prefix
    }

    // Qt - qml

    // Qt - others
}
