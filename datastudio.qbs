import qbs

Project {

    readonly property string installPrefix: 'datastudio'
    readonly property string variantSuffix: qbs.buildVariant == 'debug' ? 'd' : ''

    qbs.enableDebugCode: true

    qbsSearchPaths: [
        'tools/qbs'
    ]

    references: [
        'config/config.qbs',
        'src/src.qbs',
        //'setup/setup.qbs',
        'tools/tools.qbs',
        'tools/setenv/setenv.qbs'
    ]

    Product {
        name: 'global'
        Group {
            name: 'config'
            files: [
                'CHANGE',
                'LICENSE',
                'README.md',
                'VERSION'
            ]
            qbs.install: true
            qbs.installPrefix: project.installPrefix
        }
    }
}
