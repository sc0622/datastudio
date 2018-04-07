import qbs
import qbs.File
import qbs.FileInfo
import qbs.TextFile

CppDyLibrary {
    type: base.concat([ 'pluginSpec', 'qt_plugin_metadata' ])
    module: 'core/plugins'

    Depends { name: 'Qt.core' }
    //Depends { name: 'setenv-sync'; required: false; cpp.link: false } //

    property string uri: ''
    property string uriDir: uri.replace('.', '/')
    property string pluginVersion: version.split('.', 2).join('.')
    property path destinationRoot: FileInfo.joinPaths(project.sourceDirectory,
                                                      'lib', module, 'meta')
    property stringList dependencies: []

    destinationDirectory: FileInfo.joinPaths(destinationRoot, uriDir)
    installDir: FileInfo.joinPaths(installPrefix, 'meta', uriDir)

    //desc.condition: false
    defaultCopyHeader: false
    defaultCopyDynamic: false
    defaultInstall: false
    defaultExport: false
    //cpp.useCxxPrecompiledHeader: false

    cpp.defines: {
        var defines = base;
        if (uri.length > 0) {
            defines.push('PLUGIN_URI=\"' + uri + '\"');
        }
        return defines;
    }

    Group {
        fileTagsFilter: installFileTags
        qbs.install: true
        qbs.installPrefix: project.projectName
        qbs.installDir: installDir
    }
}
