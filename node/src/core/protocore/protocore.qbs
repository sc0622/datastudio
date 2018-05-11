import qbs
import qbs.File
import qbs.FileInfo

ElectronLibrary {
    readonly property path sourceDir: FileInfo.joinPaths(project.sourceDirectory, 'src/core')

    destinationDir: {
        var path = FileInfo.joinPaths(project.sourceDirectory, '/node/datastudio/lib/protocore/');
        if (qbs.buildVariant == 'debug') {
            path += '/debug';
        }
        return path;
    }
    cpp.includePaths: base.concat(['.',
                                   sourceDir,
                                   sourceDir + '/..',
                                   sourceDir + '/icdcore/3rdpart/jsoncpp',
                                   tinyxml.prefix + '/..',
                                   comm.prefix + '/private/moxa'
                                  ])
    cpp.libraryPaths: base.concat([ comm.prefix + '/private/moxa' ])
    cpp.dynamicLibraries: base.concat(['pcomm' + (qbs.architecture == 'x86_64' ? '_x86_64' : '')
                                      ])
    cpp.defines: base.concat(['ICDCORE_JSON_LIB',
                              'SERIAL_USE_PCOMM'
                             ])

    Depends { name: 'Qt.core' }
    Depends { name: 'Qt.printsupport' }
    Depends {
        condition: qbs.targetOS.contains('windows')
        name: 'Qt.axcontainer'
    }
    Depends { name: 'Qt.network' }
    Depends { name: 'Qt.concurrent' }
    //Depends { name: 'Qt.widgets' }
    Depends { name: 'Qt.serialport' }

    Group {
        name: 'wrap'
        files: [ '**/*.h', '**/*.cpp' ]
    }

    Group {
        id: tinyxml
        name: 'tinyxml'
        prefix: project.sourceDirectory + '/src/3rdpart/tinyxml/'
        files: [ '*.h', '*.cpp' ]
    }

    Group {
        id: comm
        name: 'comm'
        prefix: product.sourceDir + '/icdcomm/'
        files: [
            '**/*.h',
            '**/*.cpp',
        ]
    }

    Group {
        id: core
        name: 'core'
        prefix: product.sourceDir + '/icdcore/'
        files: [
            '**/*.h',
            '**/*.cpp',
        ]
    }

    Group {
        id: parser
        name: 'parser'
        prefix: product.sourceDir + '/icdparser/'
        files: [
            '**/*.h',
            '**/*.cpp',
            'resource/*.qrc'
        ]
        excludeFiles: [
            '**/jexcel.*'
        ]
    }

    Group {
        id: worker
        name: 'worker'
        prefix: product.sourceDir + '/icdworker/'
        files: [
            '**/*.h',
            '**/*.cpp',
        ]
    }
}
