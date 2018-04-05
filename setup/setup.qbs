import qbs
import qbs.FileInfo

Project {

    readonly property string setupDir: 'setup'
    readonly property path completeSetupDir: FileInfo.joinPaths(qbs.installRoot, setupDir)

    qbsSearchPaths: [ '.' ]

    Product {
        name: 'modules'
        Depends { name: 'cpp' }
        files: [ 'modules/**/*.qbs' ]
    }

    references: [
        'clear/clear.qbs',
        'pack/pack.qbs'
    ]
}
