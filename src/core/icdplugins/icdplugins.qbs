import qbs
import qbs.File
import qbs.FileInfo
import qbs.Utilities

QmlPluginLibrary {

    condition: qbs.targetOS.contains('windows')     //TODO [fix other platform]
        && Utilities.versionCompare(Qt.core.version, '5.6.0') >= 0
    uri: 'Icd.Core'
    type: base.concat([ 'customfile.out' ])

    dynamicLibraryPaths: {
        var items = base
        if (qbs.targetOS.contains('windows')) {
            items = items.concat([project.sourceDirectory + '/lib/3rdpart/moxa'])
        }
        return items
    }
    Qt.core.resourceFileBaseName: name
    langPath: sourceDirectory + '/imports/lang'
    translationFileTags: base.concat([ 'qml', 'js' ])
    translations: [ 'zh_CN.ts' ]
    recursivePaths: [ sourceDirectory + '/imports' ]
    defaultTranslation: false //TODO

    Depends { name: "Qt.qminimal"; condition: Qt.core.staticBuild; }
    Depends { name: 'Qt.widgets' }
    Depends { name: 'Qt.quick' }
    Depends { name: 'Qt.quickcontrols2' }
    Depends { name: 'icdcore' }
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

    Group {
        condition: false
        name: 'Resources'
        files: [ 'resource/*.qrc' ]
    }

    Group {
        id: qml
        name: 'qml'
        prefix: 'imports/'
        files: [ '**/*.qml', '**/*.js', '**/*.png', '**/*.ts' ]
        //fileTags: [ 'qml', 'qml.in' ]
        fileTags: [ 'qt.core.resource_data' ]
        Qt.core.resourcePrefix: '/imports'
        Qt.core.resourceSourceBase: 'imports'
    }
/*
    Rule {
        inputs: qml.fileTags
        Artifact {
            fileTags: [ 'qml.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory, 'lib/core/plugins/meta/Icd/Core/imports',
                                         FileInfo.relativePath(product.sourceDirectory + '/imports',
                                                               input.filePath))
        }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.description = 'coping ' + input.fileName;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [ cmd ];
        }
    }
*/
    Group {
        id: customfile
        name: 'files'
        prefix: 'imports/'
        files: [ 'fonts/*.*' ]
        fileTags: [ 'customfile.in' ]
    }

    Rule {
        inputs: customfile.fileTags
        Artifact {
            fileTags: [ 'customfile.out' ]
            filePath: FileInfo.joinPaths(project.sourceDirectory, 'lib/core/plugins/meta/Icd/Core/imports',
                                         FileInfo.relativePath(product.sourceDirectory + '/imports',
                                                               input.filePath))
        }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.description = 'coping ' + input.fileName;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [ cmd ];
        }
    }
}
