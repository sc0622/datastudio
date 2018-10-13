import qbs

CoreDyLibrary {

    Depends { name: 'Qt.concurrent' }
    Depends { name: 'Qt.widgets' }
    Depends { name: 'Qt.serialport' }
    Depends { name: 'icdcore' }
    Depends { name: 'icdcomm' }

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

    Properties {
        condition: qbs.targetOS.contains('windows')
        cpp.cxxStandardLibrary: 'c++98'
    }
}
