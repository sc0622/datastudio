import qbs

CoreDyLibrary {

    Depends { name: 'Qt.widgets' }
    Depends { name: '3rdpart.jchart' }
    Depends { name: '3rdpart.jutraledit' }
    Depends { name: 'icdcore' }
    Depends { name: 'icdparser' }
    Depends { name: 'icdworker' }
    Depends { name: 'icdwidget' }

    Group {
        name: 'Headers'
        files: [ '**/*.h' ]
        excludeFiles: [ '**/private/**/*.h', '**/*_p.h' ]
        fileTags: ['hpp.in' ]
    }

    Group {
        name: 'Headers_private'
        files: [ '**/private/**/*.h', '**/*_p.h' ]
    }

    Group {
        name: 'Sources'
        files: [ '**/*.cpp' ]
    }
}
