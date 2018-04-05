import qbs
import qbs.FileInfo
import '../../../tools/setenv/setenv.js' as Func

Module {
    id: root
    property string module
    property string prefix: Func.jframeDir(project) + '/'
    property string installSourceBase: ''
    property stringList files: []

    Depends { name: 'cpp' }

    Group {
        name: 'data' + (!!root.module ? '-' + root.module : '')
        prefix: root.prefix
        files: root.files
        qbs.install: true
        qbs.installPrefix: FileInfo.joinPaths(project.setupDir, 'packages')
        qbs.installDir: FileInfo.joinPaths(!!root.domain ? root.domain : '', 'data')
        qbs.installSourceBase: root.installSourceBase
    }
}
