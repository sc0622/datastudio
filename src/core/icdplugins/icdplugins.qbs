import qbs
import qbs.FileInfo

QmlPluginLibrary {
    uri: 'Icd.Core'
    dynamicLibraryPaths: base.concat([project.sourceDirectory + '/lib/3rdpart/moxa'])
    defaultInstall: false

    Depends { name: "Qt.qminimal"; condition: Qt.core.staticBuild; }
    Depends { name: 'Qt.widgets' }
    Depends { name: 'icdmeta' }

    Group {
        name: 'headers'
        files: [ '*.h' ]
        excludeFiles: [ 'private/*.h', '*_p.h' ]
    }

    Group {
        name: 'private'
        files: [ 'private/*.h', '*_p.h' ]
    }

    Group {
        name: 'sources'
        files: [ '*.cpp', 'private/*.cpp' ]
    }
}
