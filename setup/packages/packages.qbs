import qbs
import qbs.FileInfo
import qbs.Environment

Project {

    readonly property path packagesPath: path

    references: [
        'com/smartsoft/datastudio/3rdpart/module.qbs',
        'com/smartsoft/datastudio/core/module.qbs',
        'com/smartsoft/datastudio/app/module.qbs'
    ]
}
