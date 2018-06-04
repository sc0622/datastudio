import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment

PackageThird {
    name: 'com.smartsoft.datastudio.3rdpart'

    module3rdpart: [
        'jchart', 'jencrypt', 'jutraledit', 'jwt', 'log4cpp',
        'qwt', 'tinyxml'
    ]

    // 3rdpart - moxa
    Group {
        name: 'data-3rdpart-moxa'
        prefix: FileInfo.joinPaths(project.sourceDirectory, 'lib', '3rdpart', 'moxa') + '/'
        files: [ 'pcomm.dll' ]
        qbs.install: true
        qbs.installPrefix: dataInstallPrefix
    }
}
