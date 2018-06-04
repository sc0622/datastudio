import qbs
import qbs.File
import qbs.FileInfo

Product {
    type: [ 'setup.package' ]

    Depends { name: 'cpp' }
    Depends { name: 'Qt.core'; cpp.link: false }
    Depends { name: 'Qt.qml'; cpp.link: false }
    Depends { name: 'pack.data'; cpp.link: false }
    Depends { name: 'pack.meta'; cpp.link: false }

    readonly property path qtdir: Qt.core.binPath + '/..'
    readonly property path projectDir: FileInfo.joinPaths(project.projectInstallRoot, project.projectName)
    readonly property string domain: FileInfo.relativePath(project.packagesPath, sourceDirectory).replace(/\//g, '.')
    readonly property string installPrefix: FileInfo.joinPaths(project.setupDir, 'packages')
    readonly property string installDir: FileInfo.joinPaths(domain, 'data')
    readonly property path dataSourcePath: pack.data.sourcePath
    readonly property path dataTargetPath: pack.data.targetPath
    readonly property path metaSourcePath: pack.meta.sourcePath
    readonly property path metaTargetPath: pack.meta.targetPath
    readonly property string dataInstallPrefix: pack.data.installPrefix
    readonly property string metaInstallPrefix: pack.meta.installPrefix

    Group {
        fileTagsFilter: [ 'data.bin' ]
        qbs.install: true
        qbs.installPrefix: dataInstallPrefix
    }

    Group {
        fileTagsFilter: [ 'data.plugins' ]
        qbs.install: true
        qbs.installPrefix: dataInstallPrefix
        qbs.installDir: 'plugins'
        qbs.installSourceBase: Qt.core.pluginPath
    }

    Group {
        fileTagsFilter: [ 'data.qml' ]
        qbs.install: true
        qbs.installPrefix: dataInstallPrefix
        qbs.installDir: 'qml'
        qbs.installSourceBase: Qt.qml.qmlPath
    }
}
