import qbs
import qbs.FileInfo

CoreDyLibrary {

    Depends { name: 'Qt.widgets' }
    Depends { name: 'Qt.concurrent' }
    Depends { name: 'Qt.xml' }
    Depends { name: '3rdpart.jwt' }
    Depends { name: 'icdcomm' }
    Depends { name: 'icdcore' }
    Depends { name: 'icdworker' }
    Depends { name: 'icdparser' }

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

    Group {
        name: 'Resources'
        files: [ 'resource/*.qrc' ]
    }

    cpp.cxxStandardLibrary: 'c++98'

}
