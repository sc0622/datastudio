import qbs
import qbs.FileInfo

Module {
    id: root

    Depends { name: 'cpp' }

    Group {
        name: 'meta'
        files: [ FileInfo.joinPaths(product.sourceDirectory, 'meta', '**', '*') ]
        qbs.install: true
        qbs.installPrefix: FileInfo.joinPaths(project.setupDir, 'packages')
        qbs.installDir: FileInfo.joinPaths(!!product.domain ? product.domain : '', name)
        qbs.installSourceBase: name
    }
}
