import qbs
import qbs.FileInfo

Product {

    Group {
        name: 'config'
        files: [ '**/*.*' ]
        excludeFiles: [ '**/*.pr[io]', '**/*.qbs' ]
        qbs.install: true
        qbs.installPrefix: project.installPrefix
        qbs.installDir: name
        qbs.installSourceBase: '.'
    }
}
