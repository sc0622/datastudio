import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment
import '../../setenv.js' as Func

Module {
    Depends { name: 'cpp' }
    Depends { name: 'Qt.core' ; cpp.link: false }

    // Qt - 3rdpart

    Group {
        name: 'Qt_3rdpart_dynamic'
        prefix: Qt.core.binPath + '/'
        files: [ 'd3dcompiler*.dll', 'opengl*.dll', 'lib*GL*' + project.variantSuffix + '.dll' ]
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: 'bin'
    }

    // Qt

    Group {
        name: 'Qt_base_dynamic'
        prefix: Qt.core.binPath + '/'
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
        name: 'Qt_plugins'
        prefix: Qt.core.binPath + '/plugins/'
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
}
