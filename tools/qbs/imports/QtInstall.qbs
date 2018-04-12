import qbs
import qbs.File
import qbs.FileInfo
import qbs.TextFile
import qbs.Environment
import '../../setenv/setenv.js' as Env

Product {
    name: 'QtInstall'
    type: [ 'qt.conf.out' ]

    property bool generateQtConf: true
    property stringList modules3rdpart: [
        'd3dcompiler*.dll',
        'opengl*.dll',
        'lib*GL*' + project.variantSuffix + '.dll'
    ]
    property stringList modulesBase: [
        'Concurrent', 'Core', 'Gui', 'Network', 'PrintSupport', 'OpenGL',
        'Svg', 'Widgets', 'Xml'
    ]
    property stringList modulesPlugins: [
        'bearer', 'iconengines', 'imageformats', 'platforms', 'translations', 'styles'
    ]
    property stringList modulesQml: []
    property stringList modulesResources: []
    property stringList modulesTranslations: []

    Depends { name: 'cpp' }
    Depends { name: 'Qt.core' ; cpp.link: false }
    Depends { name: 'Qt.qml' ; cpp.link: false }

    // Qt - 3rdpart

    Group {
        name: '3rdpart'
        prefix: Qt.core.binPath + '/'
        files: modules3rdpart
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: 'bin'
    }

    // Qt

    Group {
        name: 'general'
        prefix: Qt.core.binPath + '/'
        files: {
            var files = [];
            modulesBase.forEach(function(item){
                files.push('Qt' + Qt.core.versionMajor + item + Env.dylibSuffix(product));
            });
            return files;
        }
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: 'bin'
    }

    // Qt - plugins

    Group {
        name: 'plugins'
        prefix: Qt.core.pluginPath + '/'
        files: {
            var files = [];
            modulesPlugins.forEach(function(item){
                files.push(item + '/**/*.dll');
            });
            return files;
        }
        excludeFiles: [ Env.excDylibSuffixFuzzy(product) ]
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: 'bin/plugins'
        qbs.installSourceBase: prefix
    }

    // Qt - qml

    Group {
        name: 'qml'
        prefix: Qt.qml.qmlPath + '/'
        files: {
            var files = [];
            modulesQml.forEach(function(item){
                files.push(item + '/**/*');
            });
            return files;
        }
        excludeFiles: [
            Env.excDylibSuffixFuzzy(product),
            '**/*.qmlc', '**/*.jsc', '**/*.qmltypes'
        ]
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: 'bin/qml'
        qbs.installSourceBase: prefix
    }

    // Qt - resources

    Group {
        name: 'resources'
        prefix: Qt.core.binPath + '/../resources/'
        files: modulesResources
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: 'bin/resources'
        qbs.installSourceBase: prefix
    }

    // Qt - translations

    Group {
        name: 'translations'
        prefix: Qt.core.binPath + '/../translations/'
        files: modulesTranslations
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: 'bin/translations'
        qbs.installSourceBase: prefix
    }

    // Qt - qt.conf [generate]

    Rule {
        condition: generateQtConf
        multiplex: true
        Artifact {
            fileTags: [ 'qt.conf.out' ]
            filePath: FileInfo.joinPaths(project.workDirectory, 'bin', 'qt.conf')
        }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.description = 'generating ' + output.fileName + '...';
            cmd.highlight = 'codegen';
            cmd.sourceCode = function() {
                var file = new TextFile(output.filePath, TextFile.WriteOnly);
                file.write('[Paths]\n' +
                           'Prefix=.\n' +
                           'Binaries=.\n' +
                           'Libraries=.\n' +
                           'Plugins=plugins\n' +
                           'Imports=imports\n' +
                           'Qml2Imports=qml\n');
                file.close();
            }
            return [ cmd ];
        }
    }
}
