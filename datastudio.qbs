import qbs
import qbs.File
import qbs.FileInfo
import qbs.TextFile

Project {
    minimumQbsVersion: '1.10.1'
    qbs.enableDebugCode: true
    qbsSearchPaths: [ 'tools/qbs' ]

    property string version: probeVersion.version
    readonly property int versionMajor: parseInt(version.split('.')[0])
    readonly property int versionMinor: parseInt(version.split('.')[1])
    readonly property int versionPatch: parseInt(version.split('.')[2])
    readonly property string projectName: 'datastudio'
    readonly property string variantSuffix: qbs.buildVariant == 'debug' ? 'd' : ''

    references: [
        'config/config.qbs',
        'src/src.qbs',
        'setup/setup.qbs',
        'tools/tools.qbs',
        'tools/setenv/setenv.qbs'
    ]

    Product {
        name: 'global'
        type: [ 'global.out' ]
        Depends { name: 'cpp'; cpp.link: false }

        Group {
            name: 'config'
            files: [
                'CHANGE',
                'LICENSE',
                'README.md',
                'VERSION'
            ]
            fileTags: [ 'global.in' ]
            qbs.install: true
            qbs.installPrefix: project.projectName
        }

        Rule {
            inputs: [ 'global.in' ]
            multiplex: true
            Artifact { fileTags: [ 'global.out' ] }
            prepare: {
                var cmd = new JavaScriptCommand;
                cmd.description = 'reading \'VERSION\' file';
                cmd.sourceCode = function() {
                    var filePath = FileInfo.joinPaths(project.sourceDirectory, 'VERSION');
                    var file = new TextFile(filePath, TextFile.ReadOnly);
                    project.version = file.readAll().trim();
                    file.close();
                }
                return [ cmd ];
            }
        }
    }

    Probe {
        id: probeVersion
        property string version: '0.0.0'
        configure: {
            var filePath = FileInfo.joinPaths(sourceDirectory, 'VERSION');
            var file = new TextFile(filePath, TextFile.ReadOnly);
            version = file.readAll().trim();
            file.close();
            found = true;
        }
    }
}
