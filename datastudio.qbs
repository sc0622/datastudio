import qbs
import qbs.File
import qbs.FileInfo
import qbs.TextFile

Project {

    minimumQbsVersion: '1.10.1'
    qbs.enableDebugCode: true
    qbsSearchPaths: [ 'tools/qbs' ]

    readonly property int versionMajor: 3
    readonly property int versionMinor: 0
    readonly property int versionPatch: 0
    readonly property string version: versionMajor + '.' + versionMinor + '.' + versionPatch
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
        type: base.concat([ 'global.out' ])
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
                cmd.description = 'updating \'VERSION\' file';
                cmd.sourceCode = function() {
                    var filePath = FileInfo.joinPaths(project.sourceDirectory, 'VERSION');
                    var file = new TextFile(filePath, TextFile.WriteOnly);
                    file.write(project.version);
                    file.close();
                }
                return [ cmd ];
            }
        }
    }
}
