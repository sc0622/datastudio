import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment
import qbs.TextFile
import 'tools/qbs/imports' as I

I.JSolution {
    projectName: 'datastudio'
    projectDisplayName: 'Data Studio'

    references: base.concat([
                                //'node/node.qbs'
                            ])
}
