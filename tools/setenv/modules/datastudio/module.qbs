import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment
import '../../setenv.js' as Func

Module {
    id: root
    Depends { name: 'cpp' }

    // 3rdpart - moxa

    Group {
        name: 'core_moxa'
        prefix: FileInfo.joinPaths(project.sourceDirectory, 'lib', '3rdpart', 'moxa') + '/'
        files: [ 'pcomm.dll' ]
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: 'bin'
    }
}
