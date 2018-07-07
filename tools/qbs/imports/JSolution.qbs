import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment
import qbs.TextFile

Project {
    id: root
    name: projectDisplayName
    minimumQbsVersion: '1.9.1'
    qbsSearchPaths: [ '../' ]

    property string projectName: 'app'
    property string projectDisplayName: 'App'
    property string version: probeVersion.version
    property bool buildQuote: false
    readonly property int versionMajor: parseInt(version.split('.')[0])
    readonly property int versionMinor: parseInt(version.split('.')[1])
    readonly property int versionPatch: parseInt(version.split('.')[2])
    readonly property string variantSuffix: qbs.buildVariant == 'debug' ? 'd' : ''
    readonly property path qtdir: FileInfo.fromWindowsSeparators(Environment.getEnv('QTDIR'))
    readonly property path installRoot: qbs.installRoot
    readonly property path workDirectory: FileInfo.joinPaths(installRoot, projectName);

    references: [
        sourceDirectory + '/setenv.qbs',
        sourceDirectory + '/config/config.qbs',
        sourceDirectory + '/src/src.qbs',
        sourceDirectory + '/setup/setup.qbs',
        sourceDirectory + '/tools/tools.qbs'
    ]

    Probe {
        id: probeVersion
        property string version: '0.0.0'
        readonly property path projectDirectory: sourceDirectory
        configure: {
            var filePath = FileInfo.joinPaths(projectDirectory, 'VERSION');
            var file = new TextFile(filePath, TextFile.ReadOnly);
            version = file.readAll().trim();
            file.close();
            found = true;
        }
    }

    Product {
        name: 'global'
        Group {
            name: 'files'
            prefix: project.sourceDirectory + '/'
            files: [
                'CHANGE',
                'LICENSE',
                'README.md',
                'VERSION'
            ]
            qbs.install: true
            qbs.installPrefix: project.projectName
        }
    }
}
