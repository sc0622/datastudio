import '../3rdpart.qbs' as _3rdpart

_3rdpart {
    moduleName: 'nodeeditor'

    cpp.defines: [ 'NODE_EDITOR_SHARED' ]
    cpp.includePaths: [
        project.sourceDirectory + '/include/3rdpart',
        project.sourceDirectory + '/include/3rdpart/' + moduleName + '/include/nodes'
    ]
    cpp.libraryPaths: [ project.sourceDirectory + '/lib/3rdpart' ]
    cpp.dynamicLibraries: [ moduleName + (qbs.buildVariant == 'debug' ? 'd' : '') ]
}
