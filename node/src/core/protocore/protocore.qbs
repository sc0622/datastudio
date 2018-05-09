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
                                   tinyxml.prefix + '/..'
                                  ])
    cpp.defines: base.concat([ 'ICDCORE_JSON_LIB' ])

    Depends { name: 'Qt.core' }
    Depends { name: 'Qt.printsupport' }
    Depends {
        condition: qbs.targetOS.contains('windows')
        name: 'Qt.axcontainer'
    }

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
        name: 'core'
        prefix: product.sourceDir + '/icdcore/'
        files: [
            '**/*.h',
            '**/*.cpp',
        ]
    }

    Group {
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
}
