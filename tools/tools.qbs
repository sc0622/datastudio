import qbs

Product {

    files: [ '**/*.rc' ]

    Group {
        name: 'qbs'
        prefix: 'qbs/'
        files: [
            // imports
            'imports/**/*.qbs',
            // modules
            'modules/**/*.qbs'
        ]
    }

    Group {
        name: 'tools'
        prefix: './'
        files: [
            'WinSnap/*.*',
            'DataAnalyse.*'
        ]
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: 'tools'
        qbs.installSourceBase: prefix
    }
}
