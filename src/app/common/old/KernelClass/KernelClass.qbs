import qbs

CoreDyLibrary {

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

    cpp.cxxStandardLibrary: 'c++98'
}
