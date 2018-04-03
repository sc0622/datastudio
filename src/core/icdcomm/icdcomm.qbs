import qbs

CoreDyLibrary {

    Depends { name: 'Qt.gui' }
    Depends { name: 'Qt.network' }

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
    cpp.libraryPaths: base.concat([ sourceDirectory + '/private/moxa' ])
    cpp.dynamicLibraries: base.concat([ 'pcomm' ])
    cpp.defines: base.concat([ 'SERIAL_USE_PCOMM' ])
}
