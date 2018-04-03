import qbs

CoreDyLibrary {

    Depends { name: 'Qt.qml' }
    Depends { name: 'Qt.quick' }
    Depends { name: 'Qt.quickcontrols2' }
    Depends { name: 'Qt.serialport' }
    Depends { name: 'Qt.concurrent' }
    Depends { name: '3rdpart.jchart' }
    Depends { name: 'icdcomm' }
    Depends { name: 'icdcore' }
    Depends { name: 'icdparser' }
    Depends { name: 'icdworker' }

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

    cpp.dynamicLibraries: base.concat([ 'ws2_32', 'advapi32', 'user32', 'gdi32' ])
}
