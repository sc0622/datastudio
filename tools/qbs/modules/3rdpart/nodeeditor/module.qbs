import '../3rdpart.qbs' as _3rdpart

_3rdpart {
    moduleName: 'nodeeditor'

    cpp.defines: [ 'NODE_EDITOR_SHARED' ]
    cpp.includePaths:  {
        var prefix = project.sourceDirectory + '/include/3rdpart/';
        var paths = [];
        paths.push(prefix);
        paths.push(prefix + moduleName + '/include');
        paths.push(prefix + moduleName + '/include/nodes');
        return paths;
    }
    cpp.libraryPaths: [ project.sourceDirectory + '/lib/3rdpart' ]
    cpp.dynamicLibraries: [ moduleName + (qbs.buildVariant == 'debug' ? 'd' : '') ]
}
